#include "radio.h"

#include <Arduino.h>
#include <cstdint>

#include "config.h"
#include "shared/communication_protocol.h"

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

        // Set air data rate to 200 kbps (S2=200)
        // At 25 Hz with 126-byte frames: 25 * 126 * 8 = ~25 kbps — well within limit
        sendAT("ATS2=200");

        // Disable MAVLink framing — we use our own binary protocol
        sendAT("ATS6=0");

        // Save settings to EEPROM
        sendAT("AT&W");

        // Return to data mode
        exitATMode();

        Serial.println("[Radio] RFD868 configured and in data mode.");
        return true;
    }

    void process() {
        // TODO: process packets 
    }
}