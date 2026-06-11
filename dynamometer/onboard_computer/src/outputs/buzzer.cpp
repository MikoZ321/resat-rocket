#include "buzzer.h"

#include <Arduino.h>

#include "config.h"

namespace buzzer {
    bool begin() {
        pinMode(BUZZER_PIN, OUTPUT);
        digitalWrite(BUZZER_PIN, LOW);
        return true;
    }

    void sendPulse() {
        // TODO: make non-blocking version
        digitalWrite(BUZZER_PIN, HIGH);
        delay(TICK_FAST_INTERVAL_MS);
        digitalWrite(BUZZER_PIN, LOW);
    }
}