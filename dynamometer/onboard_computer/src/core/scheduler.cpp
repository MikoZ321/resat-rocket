#include "scheduler.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "communication/radio.h"
#include "config.h"
#include "core/state.h"
#include "core/telemetry.h"
#include "memory/sd_card.h"
#include "memory/spi_flash.h"
#include "outputs/camera.h"
#include "outputs/leds.h"
#include "sensors/analog_sensors.h"
#include "sensors/atmospheric_sensor.h"
#include "sensors/high_g_accelerometer.h"
#include "sensors/main_gps.h"
#include "sensors/main_imu.h"
#include "sensors/oxidizer_loadcell.h"
#include "sensors/thermocouples.h"
#include "sensors/thrust_loadcell.h"

namespace scheduler {
    void begin() {
        // Initialize radio
        radio::begin();

        // Initialize I2C
        Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY_HZ);
        Wire.setTimeOut(I2C_TIMEOUT_MS);
        Serial.println("[INIT] OK: I2C");

        // Initialize SPI
        SPI.begin(SPI_SCLK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
        Serial.println("[INIT] OK: SPI");

        // TODO: handle init failures
        // Initialize Tier A sensors
        main_imu::begin();
        high_g_accelerometer::begin();
        atmospheric_sensor::begin();
        thrust_loadcell::begin();

        // Initialize Tier B sensors
        oxidizer_loadcell::begin();
        analog_sensors::begin();
        thermocouples::begin();
        // TODO: init piston position sensor

        // Initialize Tier C sensors
        main_gps::begin();
        // TODO: init battery level sensor

        // Initialize memory
        spi_flash::begin();
        sd_card::begin();

        // Initialize outputs
        leds::begin();
        camera::begin();
    }

    void runTick(std::uint32_t tick_number) {
        bool is_slow_tick = !(tick_number % TICK_SLOW_DIVISOR);
        bool is_house_tick = !(tick_number % TICK_HOUSE_DIVISOR);

        // Poll Tier A sensors
        main_imu::readSensorData();
        high_g_accelerometer::readSensorData();
        atmospheric_sensor::readSensorData();
        thrust_loadcell::readSensorData();

        // Poll Tier B sensors
        if (is_slow_tick) {
            oxidizer_loadcell::readSensorData();
            analog_sensors::readSensorData();
            thermocouples::readSensorData();
            // TODO: gauge piston position
        }
        
        // Poll Tier C sensors
        if (is_house_tick) {
            main_gps::readSensorData();
            // TODO: read battery level
        }

        // TODO: calculate fusion altitude

        telemetry::assembleMiniFrame();
        spi_flash::writeMiniTelemetryFrame(telemetry::getMiniFrame());

        if (is_slow_tick) {
            telemetry::assembleFullFrame();
            full_telemetry_frame_t full_frame = telemetry::getFullFrame();

            spi_flash::writeFullTelemetryFrame(full_frame);
            sd_card::writeFullTelemetryFrame(full_frame);
            
            radio::enqueueFullTelemetryFrame(full_frame);

            spi_flash::periodicMaintenance(tick_number / TICK_SLOW_DIVISOR);
            state::persistFlightPhase();
        }
    }
}
