#include "telemetry.h"

#include <Arduino.h>

#include "sensors/analog_sensors.h"
#include "sensors/atmospheric_sensor.h"
#include "sensors/high_g_accelerometer.h"
#include "sensors/main_gps.h"
#include "sensors/main_imu.h"
#include "sensors/oxidizer_loadcell.h"
#include "sensors/thermocouples.h"
#include "sensors/thrust_loadcell.h"
#include "shared/crc.h"
#include "state.h"

static full_telemetry_frame_t s_full_frame;
static mini_telemetry_frame_t s_mini_frame;

static std::uint16_t s_current_frame_index = 0;

namespace telemetry {
    void assembleFullFrame() {
        s_full_frame.sync_bytes[0] = 0xAA;
        s_full_frame.sync_bytes[1] = 0x55;

        s_full_frame.telemetry_frame_index = s_current_frame_index;
        s_current_frame_index++;

        s_full_frame.timestamp_ms = millis();
        
        // Fill Tier A sensor readings
        main_imu::fill(s_full_frame.acceleration_main, s_full_frame.angular_velocity);
        high_g_accelerometer::fill(s_full_frame.acceleration_high_g);
        // Temp values used to prevent float misalignament 
        float temp_pressure_pa, temp_temperature_c;
        atmospheric_sensor::fill(temp_pressure_pa, temp_temperature_c);
        s_full_frame.air_pressure = temp_pressure_pa;
        s_full_frame.air_temperature = temp_temperature_c;
        // TODO: altitude calculation
        float temp_engine_thrust;
        thrust_loadcell::fill(temp_engine_thrust);
        s_full_frame.engine_thrust = temp_engine_thrust;

        // Fill Tier B sensor readings
        float temp_oxidizer_weight;
        oxidizer_loadcell::fill(temp_oxidizer_weight);
        s_full_frame.oxidizer_weight = temp_oxidizer_weight;
        float temp_fuel_pressure, temp_oxidizer_pressure, temp_pyro_battery_voltage, temp_main_battery_voltage;
        analog_sensors::fill(temp_fuel_pressure, temp_oxidizer_pressure, temp_pyro_battery_voltage, temp_main_battery_voltage);
        s_full_frame.fuel_pressure = temp_fuel_pressure;
        s_full_frame.oxidizer_pressure = temp_oxidizer_pressure;
        s_full_frame.pyro_battery_voltage = temp_pyro_battery_voltage;
        s_full_frame.main_battery_voltage = temp_main_battery_voltage;
        float temp_engine_temperature_top, temp_engine_temperature_bottom;
        thermocouples::fill(temp_engine_temperature_top, temp_engine_temperature_bottom);
        s_full_frame.engine_temperature_top = temp_engine_temperature_top;
        s_full_frame.engine_temperature_bottom = temp_engine_temperature_bottom;
        // TODO: fill piston position

        // TODO: Fill Tier C sensor readings
        float temp_gps_latitude, temp_gps_longitude, temp_gps_altitude;
        std::uint8_t temp_gps_sattelite_count;
        main_gps::fill(temp_gps_latitude, temp_gps_longitude, temp_gps_altitude, temp_gps_sattelite_count);
        s_full_frame.gps_latitude = temp_gps_latitude;
        s_full_frame.gps_longitude = temp_gps_longitude;
        s_full_frame.gps_altitude = temp_gps_altitude;
        s_full_frame.gps_sattelite_count = temp_gps_sattelite_count;

        // TODO: Fill is_valid_reading_mask
        s_full_frame.flight_phase = state::getFlightPhase();
        s_full_frame.crc = crc::compute(reinterpret_cast<const std::uint8_t*>(&s_full_frame), sizeof(s_full_frame) - sizeof(s_full_frame.crc));
    }

    void assembleMiniFrame() {
        s_mini_frame.sync_bytes[0] = 0xBB;
        s_mini_frame.sync_bytes[1] = 0x66;

        s_mini_frame.telemetry_frame_index = s_current_frame_index;
        s_current_frame_index++;

        s_mini_frame.timestamp_ms = millis();

        main_imu::fill(s_mini_frame.acceleration_main, s_mini_frame.angular_velocity);
        high_g_accelerometer::fill(s_mini_frame.acceleration_high_g);
        // Temp values used to prevent float misalignament 
        float temp_pressure_pa, temp_temperature_c;
        atmospheric_sensor::fill(temp_pressure_pa, temp_temperature_c);
        s_mini_frame.air_pressure = temp_pressure_pa;
        s_mini_frame.air_temperature = temp_temperature_c;
        // TODO: altitude calculation
        float temp_engine_thrust;
        thrust_loadcell::fill(temp_engine_thrust);
        s_mini_frame.engine_thrust = temp_engine_thrust;

        // TODO: Fill is_valid_reading_mask
        s_mini_frame.crc = crc::compute(reinterpret_cast<const std::uint8_t*>(&s_mini_frame), sizeof(s_mini_frame) - sizeof(s_mini_frame.crc));
    }

    const full_telemetry_frame_t& getFullFrame() {
        return s_full_frame;
    }

    const mini_telemetry_frame_t& getMiniFrame() {
        return s_mini_frame;
    }
}
