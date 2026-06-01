#include "scheduler.h"

#include <Arduino.h>

#include "config.h"
#include "core/state.h"
#include "core/telemetry.h"

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

        telemetry::assembleMiniFrame();
        // TODO: write mini frame to flash

        if (is_slow_tick) {
            telemetry::assembleFullFrame();
            // TODO: write full frame to flash
            // TODO: transmit full frame to ground station
            state::persistFlightPhase();
        }
    }
}
