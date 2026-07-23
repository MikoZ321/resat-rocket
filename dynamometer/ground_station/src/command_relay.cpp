#include "command_relay.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#include "config.h"
#include "radio.h"
#include "shared/communication_protocol.h"
#include "shared/crc.h"
#include "shared/types.h"

extern HardwareSerial radio_serial;
// ── State ─────────────────────────────────────────────────────────────────────
static std::uint16_t s_current_command_index = 0;    // monotonically increasing uplink counter
static std::uint32_t s_sent_command_count = 0;
static std::uint32_t s_rejected_command_count = 0;

// USB receive buffer — looking for USB_CMD_LEN byte wrappers
static std::uint8_t  s_usb_buffer[USB_COMMAND_WRAPPER_SIZE];
static std::uint8_t  s_usb_buffer_pointer_position = 0;

// ── Build and send a full command_frame_t from a validated USB wrapper ───────────────
static void forwardCommand(CommandType type, const std::uint8_t payload[4]) {
    command_frame_t command_frame;

    // Sync bytes
    command_frame.sync[0] = COMMAND_FRAME_SYNC_BYTE_0;
    command_frame.sync[1] = COMMAND_FRAME_SYNC_BYTE_1;

    // Sequence number — monotonically increasing
    command_frame.command_frame_index = s_current_command_index++;

    // Command fields
    command_frame.type = type;
    memcpy(command_frame.payload, payload, 4);

    // CRC over all bytes except the final 2 (the CRC itself)
    command_frame.crc = crc::compute((const std::uint8_t*)&command_frame,
                          sizeof(command_frame_t) - sizeof(std::uint16_t));

    radio_serial.write((const std::uint8_t*)&command_frame, sizeof(command_frame_t));
    radio_serial.flush();

    s_sent_command_count++;
}

// ── USB wrapper validation ────────────────────────────────────────────────────
// Returns true if the 12-byte buffer contains a valid USB command wrapper.
static bool validateWrapper(const std::uint8_t* buf) {
    // Sync bytes
    if (buf[0] != COMMAND_FRAME_SYNC_BYTE_0 || buf[1] != COMMAND_FRAME_SYNC_BYTE_1) return false;

    // Terminator
    if (buf[USB_COMMAND_WRAPPER_SIZE - 1] != USB_COMMAND_WRAPPER_TERMINATOR) return false;

    return true;
}

bool command_relay::begin() {
    s_current_command_index = 0;
    s_sent_command_count = 0;
    s_rejected_command_count = 0;
    s_usb_buffer_pointer_position = 0;
    return true;
}

void command_relay::process() {
    // Drain USB serial looking for command wrappers
    while (Serial.available()) { 
        std::uint8_t b = (std::uint8_t)Serial.read();

        if (s_usb_buffer_pointer_position == 0 && b != COMMAND_FRAME_SYNC_BYTE_0) {
            continue;   // not the start of a command — discard
        }
        if (s_usb_buffer_pointer_position == 1 && b != COMMAND_FRAME_SYNC_BYTE_1) {
            s_usb_buffer_pointer_position = 0;   // false start — reset
            continue;
        }

        s_usb_buffer[s_usb_buffer_pointer_position++] = b;

        if (s_usb_buffer_pointer_position == USB_COMMAND_WRAPPER_SIZE) {
            // Full wrapper received
            s_usb_buffer_pointer_position = 0;

            if (!validateWrapper(s_usb_buffer)) {
                s_rejected_command_count++;
                continue;
            }

            // Extract fields from wrapper
            std::uint8_t type = s_usb_buffer[2];
            std::uint8_t payload[4];
            memcpy(payload, s_usb_buffer + 3, 4);

            forwardCommand(static_cast<CommandType>(type), payload);
        }
    }
}
