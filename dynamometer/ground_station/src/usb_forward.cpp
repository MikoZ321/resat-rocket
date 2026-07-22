#include "usb_forward.h"

#include <Arduino.h>
#include <cstdint>

#include "config.h"

template <typename T>
inline static void printValue(const T& value) {
    Serial.print(value);
    Serial.print(";");
}

inline static void printValue(const float& value, int precision) {
    Serial.print(value, precision);
    Serial.print(";");
}

inline static void printVector(const float* vector, std::size_t size, int precision) {
    for (std::size_t i{}; i < size; ++i) {
        Serial.print(vector[i], precision);
        Serial.print(";");
    }
}

namespace usb_forward {
    bool begin() {
        // TODO: add verification from pc that connection is good
        Serial.begin(SERIAL_MONITOR_BAUD_RATE);

        return true;
    }

    void sendFullFrame(const full_telemetry_frame_t& full_frame, std::uint32_t frames_received_count, std::uint32_t crc_fail_count, std::uint32_t frame_index_gap_count) {
        // Header
        printValue(full_frame.timestamp_ms);
        printValue(full_frame.telemetry_frame_index);

        // Main imu
        printVector(full_frame.acceleration_main, 3, FLOAT_PRECISION);
        printVector(full_frame.angular_velocity, 3, FLOAT_PRECISION);

        // High-g accelerometer
        printVector(full_frame.acceleration_high_g, 3, FLOAT_PRECISION);

        // Atmospheric sensor
        printValue(full_frame.air_pressure, FLOAT_PRECISION);
        printValue(full_frame.air_temperature, FLOAT_PRECISION);

        // Fusion value
        printValue(full_frame.altitude, FLOAT_PRECISION);
        
        // Loadcells
        printValue(full_frame.engine_thrust, FLOAT_PRECISION);
        printValue(full_frame.oxidizer_weight, FLOAT_PRECISION);

        // Pressure transducers        
        printValue(full_frame.fuel_pressure, FLOAT_PRECISION);
        printValue(full_frame.oxidizer_pressure, FLOAT_PRECISION);

        // Analog sensors
        printValue(full_frame.pyro_battery_voltage, FLOAT_PRECISION);
        printValue(full_frame.main_battery_voltage, FLOAT_PRECISION);
        
        // Thermocouples
        printValue(full_frame.engine_temperature_top, FLOAT_PRECISION);
        printValue(full_frame.engine_temperature_bottom, FLOAT_PRECISION);

        // Piston position
        printValue(full_frame.piston_position);

        // GPS data
        printValue(full_frame.gps_latitude, GPS_PRECISION);
        printValue(full_frame.gps_longitude, GPS_PRECISION);
        printValue(full_frame.gps_altitude, FLOAT_PRECISION);
        printValue(full_frame.gps_sattelite_count);

        printValue(full_frame.main_battery_level, FLOAT_PRECISION);

        // Tail
        printValue(full_frame.is_valid_reading_mask);
        printValue(static_cast<std::uint8_t>(full_frame.flight_phase));
        printValue(static_cast<std::uint8_t>(full_frame.arm_state));
        printValue(full_frame.last_command_frame_index);
        printValue(static_cast<std::uint8_t>(full_frame.last_command_result));
        
        // Radio metadata
        printValue(frames_received_count);
        printValue(crc_fail_count);
        printValue(frame_index_gap_count);
        Serial.println();
    }
}