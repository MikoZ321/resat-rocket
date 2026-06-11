#include "radio.h"

#include <Arduino.h>
#include <cstddef>
#include <cstdint>

#include "config.h"
#include "shared/communication_protocol.h"
#include "shared/crc.h"
#include "shared/types.h"

static const std::uint32_t AT_GUARD_MS = 1100;  // 1 s + margin
static const std::uint32_t AT_RESPONSE_MS = 500;
static const std::uint32_t AT_DRAIN_MS = 50;

// Enter AT command mode. Returns true if "OK" received.
static bool enterATMode() {
    // Flush any pending TX data first
    Serial1.flush();
    delay(AT_GUARD_MS);
    Serial1.print("+++");
    delay(AT_GUARD_MS);

    // Read response — expect "OK"
    std::uint32_t start = millis();
    String response = "";
    while (millis() - start < AT_RESPONSE_MS) {
        while (Serial1.available())
            response += (char)Serial1.read();
        if (response.indexOf("OK") >= 0) return true;
    }
    return false;
}

// Exit AT command mode.
static void exitATMode() {
    Serial1.println("ATO");
    delay(AT_DRAIN_MS);
    // Drain any response bytes
    while (Serial1.available()) Serial1.read();
}

// Send one AT command and return the response string.
static String sendAT(const char* cmd) {
    // Drain input before sending
    while (Serial1.available()) Serial1.read();
    Serial1.println(cmd);
    delay(AT_RESPONSE_MS);
    String resp = "";
    while (Serial1.available())
        resp += (char)Serial1.read();
    return resp;
}

// Raw ring buffer — ISR or polling fills head, scanner advances tail
static std::uint8_t  s_rx_buffer[RX_BUFFER_SIZE];
static std::uint16_t s_rx_head_pointer = 0;
static std::uint16_t s_rx_tail_pointer = 0;

// Returns the number of available rx buffer bytes
static std::uint16_t getRxBufferAvailableByteCount() {
    return (s_rx_head_pointer - s_rx_tail_pointer + RX_BUFFER_SIZE) % RX_BUFFER_SIZE;
}

// Reads one byte from the rx buffer without copying it
static std::uint8_t rxBufferPeekByte(std::uint16_t offset) {
    return s_rx_buffer[(s_rx_tail_pointer + offset) % RX_BUFFER_SIZE];
}

// Move the tail pointer n bytes forward
static void rxConsume(std::uint16_t n) {
    s_rx_tail_pointer = (s_rx_tail_pointer + n) % RX_BUFFER_SIZE;
}

static std::uint32_t s_frames_received_count = 0;
static std::uint32_t s_crc_fail_count = 0;
static std::uint32_t s_frame_index_gap_count = 0;
static std::uint16_t s_last_full_frame_index = 0xFFFF;

// Try to parse and validate a full_telemetry_frame_t starting at current tail.
// Returns true and advances tail if valid. Returns false and advances 1 byte if not.
static bool tryFullFrame() {
    if (getRxBufferAvailableByteCount() < sizeof(full_telemetry_frame_t)) return false;

    // Verify sync bytes without copying
    if (rxBufferPeekByte(0) != FULL_FRAME_SYNC_BYTE_0 || rxBufferPeekByte(1) != FULL_FRAME_SYNC_BYTE_1) return false;

    // Copy candidate frame
    full_telemetry_frame_t full_frame;
    for (std::size_t i = 0; i < sizeof(full_telemetry_frame_t); i++)
        ((std::uint8_t*)&full_frame)[i] = s_rx_buffer[(s_rx_tail_pointer + i) % RX_BUFFER_SIZE];

    bool is_correct_crc = crc::verify((const std::uint8_t*) &full_frame,
                                      sizeof(full_telemetry_frame_t) - sizeof(std::uint16_t), full_frame.crc);
    if (!is_correct_crc) {
        s_crc_fail_count++;
        Serial.println("[GS] Full frame CRC fail.");
        return false;  // caller advances by 1 byte
    }

    // Sequence gap detection
    if (s_last_full_frame_index != 0xFFFF) {
        // Mini frame gets s_last_full_frame_index + 1
        std::uint16_t expected_telemetry_frame_index = s_last_full_frame_index + 2;
        if (full_frame.telemetry_frame_index != expected_telemetry_frame_index) {
            std::uint16_t gap = (std::uint16_t)(full_frame.telemetry_frame_index - expected_telemetry_frame_index) / 2;
            s_frame_index_gap_count++;
            Serial.print("[GS] Seq gap, ");
            Serial.print(gap);
            Serial.println(" frames lost)");
        }
    }
    s_last_full_frame_index = full_frame.telemetry_frame_index;
    s_frames_received_count++;

    // TODO: forward to usb
    // TODO: save to sd

    rxConsume(sizeof(full_telemetry_frame_t));
    return true;
}

namespace radio {
    bool begin() {
        Serial1.begin(RADIO_BAUD_RATE, SERIAL_8N1, RADIO_RX_PIN, RADIO_TX_PIN); 
       
        if (!enterATMode()) {
            Serial.println("[Radio] RFD868 did not respond to +++ — modem may be in data mode already");
            // Not necessarily a failure — modem may already be configured
            // and in data mode from a previous session
            // Return true and continue — we will detect real failures by missing ACKs
            return true;
        }

        Serial.println("[Radio] RFD868 in AT mode.");
        // Read firmware version for logging
        String version = sendAT("ATI1");
        Serial.print("[Radio] Firmware: ");
        Serial.print(version);
 
        sendAT("ATS2=200"); // Set air data rate to 200 kbps (S2=200)
        sendAT("ATS6=0"); // Disable Mavlink framing
        sendAT("AT&W"); // Save settings to EEPROM
        exitATMode();

        Serial.println("[Radio] RFD868 configured and in data mode.");
        return true;
    }

    void process() {
        // Drain Serial1 into ring buffer
        while (Serial1.available()) {
            std::uint16_t next = (s_rx_head_pointer + 1) % RX_BUFFER_SIZE;
            if (next == s_rx_tail_pointer) break;  // buffer full — drop byte
            s_rx_buffer[s_rx_head_pointer] = (std::uint8_t)Serial1.read();
            s_rx_head_pointer = next;
        }

        // Scan for frames — keep scanning until no progress
        while (getRxBufferAvailableByteCount() > 0) {
            std::uint8_t first_byte = rxBufferPeekByte(0);

            if (first_byte == FULL_FRAME_SYNC_BYTE_0 && getRxBufferAvailableByteCount() > 1 && rxBufferPeekByte(1) == FULL_FRAME_SYNC_BYTE_1) {
                if (getRxBufferAvailableByteCount() < sizeof(full_telemetry_frame_t)) break;  // wait for more data
                if (tryFullFrame()) continue;
                rxConsume(1);  // CRC failed — advance past bad sync
                continue;
            }

            // Not a known sync byte — discard
            rxConsume(1);
        }
    }
}