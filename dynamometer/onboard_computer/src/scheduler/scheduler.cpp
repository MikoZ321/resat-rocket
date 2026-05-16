#include "scheduler.h"

#include <Arduino.h>

#include "config.h"

namespace scheduler {
    void begin() {
        return;
    }

    void runTick(std::uint32_t tickNumber) {
        Serial.print("Current tick: ");
        Serial.print(tickNumber);
        // Stand-in for actual tier A sensor polling
        Serial.print(" Tier A");

        bool is_slow_tick = !(tickNumber % TICK_SLOW_DIVISOR);
        if (is_slow_tick) {
            // Stand-in for actual tier B sensor polling
            Serial.print(", B");
        }
        
        bool is_house_tick = !(tickNumber % TICK_HOUSE_DIVISOR);
        if (is_house_tick) {
            // Stand-in for actual tier C sensor polling
            Serial.print(", C");
        }
        Serial.println("");
    }
}
