#include "command.h"

#include <Arduino.h>
#include <HardwareSerial.h>

#include "core/state.h"
#include "memory/spi_flash.h"
#include "sensors/thrust_loadcell.h"
#include "shared/communication_protocol.h"
#include "shared/crc.h"
#include "shared/types.h"

static std::uint16_t s_last_command_index {0xFFFF};
extern HardwareSerial radio_serial;

std::uint16_t command::getCommandFrameIndex() {
    return s_last_command_index;
}

// ── Phase allowed table ────────────────────────────────────────────────────
// Bitmask of FlightPhase values in which each command type is permitted.
// Bit N set = allowed in phase N.
static constexpr std::uint8_t PHASE_ALLOWED[] = {
    // indexed by CommandType byte - sparse, only defined types matter
    0b00000100, // ENTER_CONFIG - PRELAUNCH only
    0b00000001, // EXIT_CONFIG - CONFIG only
    0b00000001, // SET_THRUST_SCALE - CONFIG only
    0b00000001, // SET_THRUST_OFFSET - CONFIG only
    0b00000001, // DUMP_FLASH - CONFIG only
};

// ── Ring buffer for raw received bytes ────────────────────────────────────
static constexpr int RX_BUF_SIZE = 256;
static volatile std::uint8_t s_rx_buffer[RX_BUF_SIZE];
static volatile std::uint16_t s_rx_head = 0;
static std::uint16_t s_rx_tail = 0;

// ── Parsed command queue ───────────────────────────────────────────────────
static constexpr int COMMAND_QUEUE_SIZE = 8;
static command_frame_t s_command_queue[COMMAND_QUEUE_SIZE];
static std::uint8_t s_command_queue_head = 0, s_command_queue_tail = 0;

void command::onByteReceived(std::uint8_t byte) {
    std::uint16_t next = (s_rx_head + 1) % RX_BUF_SIZE;
    if (next != s_rx_tail) { // Drop if full
        s_rx_buffer[s_rx_head] = byte;
        s_rx_head = next;
    }
}

// Scan ring buffer for a complete valid command_frame_t
static bool tryParseFrame(command_frame_t& out) {
    std::uint16_t avail = (s_rx_head - s_rx_tail + RX_BUF_SIZE) % RX_BUF_SIZE;
    if (avail < sizeof(command_frame_t)) return false;

    while ((s_rx_head - s_rx_tail + RX_BUF_SIZE) % RX_BUF_SIZE >= sizeof(command_frame_t)) {
        if (s_rx_buffer[s_rx_tail] == COMMAND_FRAME_SYNC_BYTE_0 &&
            s_rx_buffer[(s_rx_tail+1) % RX_BUF_SIZE] == COMMAND_FRAME_SYNC_BYTE_1) {
            // Copy candidate frame
            for (std::size_t i = 0; i < sizeof(command_frame_t); i++)
                ((std::uint8_t*)&out)[i] = s_rx_buffer[(s_rx_tail + i) % RX_BUF_SIZE];
            // Validate CRC over all bytes except last 2
            if (crc::verify((std::uint8_t*)&out,
                            sizeof(command_frame_t) - sizeof(std::uint16_t),
                            out.crc)) {
                s_rx_tail = (s_rx_tail + sizeof(command_frame_t)) % RX_BUF_SIZE;
                return true;
            }
        }
        s_rx_tail = (s_rx_tail + 1) % RX_BUF_SIZE; // Advance past bad byte
    }
    return false;
}

void command::begin() {
    // Attach UART RX interrupt
    radio_serial.onReceive([]() {
       while (radio_serial.available()) onByteReceived(radio_serial.read());
    });
}

void command::executeOne() {
    // First try to parse any waiting bytes into the queue
    command_frame_t potential_command_frame;
    while (((s_command_queue_head + 1) % COMMAND_QUEUE_SIZE) != s_command_queue_tail) {
        if (!tryParseFrame(potential_command_frame)) break;
        // Replay guard
        if (potential_command_frame.command_frame_index <= s_last_command_index && s_last_command_index != 0xFFFF) {
            state::setCommandResult(CommandResult::REPLAY_REJECT);
            continue;
        }
        s_command_queue[s_command_queue_head] = potential_command_frame;
        s_command_queue_head = (s_command_queue_head + 1) % COMMAND_QUEUE_SIZE;
    }

   if (s_command_queue_head == s_command_queue_tail) return; // Queue empty

    command_frame_t& cmd = s_command_queue[s_command_queue_tail];
    s_command_queue_tail = (s_command_queue_tail + 1) % COMMAND_QUEUE_SIZE;
    s_last_command_index = cmd.command_frame_index;

    // Phase gate
    std::uint8_t phaseBit = 1 << (std::uint8_t)state::getFlightPhase();
    if (!(PHASE_ALLOWED[static_cast<std::uint8_t>(cmd.type)] & phaseBit)) {
        state::setCommandResult(CommandResult::WRONG_PHASE);
        return;
    } 

    // Dispatch
    switch (float converted_payload; (CommandType)cmd.type) {
        case CommandType::ENTER_CONFIG:
            Serial.println("[OC] Received ENTER_CONFIG command.");
            state::setFlightPhase(FlightPhase::CONFIG);
            break;
        case CommandType::EXIT_CONFIG:
            Serial.println("[OC] Received EXIT_CONFIG command.");
            state::setFlightPhase(FlightPhase::PRELAUNCH);
            break;
        case CommandType::SET_THRUST_SCALE:
            Serial.println("[OC] Received SET_THRUST_SCALE command.");
            memcpy(&converted_payload, cmd.payload, 4);
            Serial.println(converted_payload);
            thrust_loadcell::setScale(converted_payload);
            break;
        case CommandType::SET_THRUST_OFFSET:
            Serial.println("[OC] Received SET_THRUST_OFFSET command.");
            memcpy(&converted_payload, cmd.payload, 4);
            Serial.println(converted_payload);
            thrust_loadcell::setOffset(converted_payload);
            break;
        case CommandType::DUMP_FLASH:
            Serial.println("[OC] Received DUMP_FLASH command.");
            spi_flash::dumpToSerial();
            break;
        default:
            break;
    }
    state::setCommandResult(CommandResult::OK);
}
