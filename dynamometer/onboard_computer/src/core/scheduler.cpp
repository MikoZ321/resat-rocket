#include "scheduler.h"

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "core/state.h"
#include "core/telemetry.h"
#include "sensors/atmospheric_sensor.h"
#include "sensors/high_g_accelerometer.h"
#include "sensors/main_imu.h"
#include "sensors/oxidizer_loadcell.h"
#include "sensors/thrust_loadcell.h"

namespace scheduler {
    void begin() {
        // Initialize I2C
        Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY_HZ);
        Wire.setTimeOut(I2C_TIMEOUT_MS);
        Serial.println("[INIT] OK: I2C");

        // TODO: handle init failures
        // Initialize Tier A sensors
        main_imu::begin();
        high_g_accelerometer::begin();
        atmospheric_sensor::begin();
        thrust_loadcell::begin();
        // Initialize Tier B sensors
        oxidizer_loadcell::begin();
    }

    void runTick(std::uint32_t tick_number) {
        // Poll Tier A sensors
        main_imu::readSensorData();
        high_g_accelerometer::readSensorData();
        atmospheric_sensor::readSensorData();
        thrust_loadcell::readSensorData();

        bool is_slow_tick = !(tick_number % TICK_SLOW_DIVISOR);
        if (is_slow_tick) {
            // Stand-in for actual tier B sensor polling
            Serial.print(", B");
            oxidizer_loadcell::readSensorData();
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
