#include "usb_forward.h"

#include <Arduino.h>
#include <cstdint>

#include "config.h"

namespace usb_forward {
    bool begin() {
        // TODO: add verification from pc that connection is good
        Serial.begin(SERIAL_MONITOR_BAUD_RATE);

        return true;
    }

    void sendFullFrame(const full_telemetry_frame_t& full_frame, std::uint32_t frames_received_count, std::uint32_t crc_fail_count, std::uint32_t frame_index_gap_count) {
        // Header
        Serial.print(full_frame.timestamp_ms); Serial.print(";");
        Serial.print(full_frame.telemetry_frame_index); Serial.print(";");

        // Main imu
        Serial.print(full_frame.acceleration_main[0], 4); Serial.print(";");
        Serial.print(full_frame.acceleration_main[1], 4); Serial.print(";");
        Serial.print(full_frame.acceleration_main[2], 4); Serial.print(";");
        Serial.print(full_frame.angular_velocity[0], 4); Serial.print(";");
        Serial.print(full_frame.angular_velocity[1], 4); Serial.print(";");
        Serial.print(full_frame.angular_velocity[2], 4); Serial.print(";");

        // High-g accelerometer
        Serial.print(full_frame.acceleration_high_g[0], 4); Serial.print(";");
        Serial.print(full_frame.acceleration_high_g[1], 4); Serial.print(";");
        Serial.print(full_frame.acceleration_high_g[2], 4); Serial.print(";");

        // Atmospheric sensor
        Serial.print(full_frame.air_pressure, 4); Serial.print(";"); 
        Serial.print(full_frame.air_temperature, 4); Serial.print(";"); 

        Serial.print(full_frame.altitude, 4); Serial.print(";");
        Serial.print(full_frame.engine_thrust, 4); Serial.print(";"); 
        Serial.print(full_frame.oxidizer_weight, 4); Serial.print(";");

        // Pressure transducers
        Serial.print(full_frame.fuel_pressure, 4); Serial.print(";");
        Serial.print(full_frame.oxidizer_pressure, 4); Serial.print(";");

        // Analog sensors
        Serial.print(full_frame.pyro_battery_voltage, 4); Serial.print(";");
        Serial.print(full_frame.main_battery_voltage, 4); Serial.print(";");
        
        // Thermocouples
        Serial.print(full_frame.engine_temperature_top, 4); Serial.print(";");
        Serial.print(full_frame.engine_temperature_bottom, 4); Serial.print(";");

        Serial.print(full_frame.piston_position); Serial.print(";");

        // GPS data
        Serial.print(full_frame.gps_latitude); Serial.print(";");
        Serial.print(full_frame.gps_longitude); Serial.print(";");
        Serial.print(full_frame.gps_altitude, 4); Serial.print(";");
        Serial.print(full_frame.gps_sattelite_count); Serial.print(";");

        Serial.print(full_frame.main_battery_level, 4); Serial.print(";");

        // Tail
        Serial.print(full_frame.is_valid_reading_mask); Serial.print(";");
        Serial.print((std::uint8_t)full_frame.flight_phase); Serial.print(";");
        
        // Radio metadata
        Serial.print(frames_received_count); Serial.print(";");
        Serial.print(crc_fail_count); Serial.print(";");
        Serial.print(frame_index_gap_count); Serial.print(";");
    }
}