#include "scheduler.h"

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "core/state.h"
#include "core/telemetry.h"
#include "sensors/main_imu.h"

namespace scheduler {
    void begin() {
        // Initialize I2C
        Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY_HZ);
        Wire.setTimeOut(I2C_TIMEOUT_MS);
        Serial.println("[INIT] OK: I2C");

        // TODO: handle init failures
        // Initialize Tier A sensors
        main_imu::begin();
    }

    void runTick(std::uint32_t tick_number) {
        Serial.print("Current tick: ");
        Serial.print(tick_number);
        // Stand-in for actual tier A sensor polling
        Serial.print(" Tier A");
        // TODO: implement bit mask
        // Poll Tier A sensors
        main_imu::readSensorData();

        bool is_slow_tick = !(tick_number % TICK_SLOW_DIVISOR);
        if (is_slow_tick) {
            // Stand-in for actual tier B sensor polling
            Serial.print(", B");
        }
        
        bool is_house_tick = !(tick_number % TICK_HOUSE_DIVISOR);
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
