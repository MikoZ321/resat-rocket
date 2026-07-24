// Contains all of the global type and enum definitions
#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

enum class FlightPhase : std::uint8_t {
   CONFIG,
   PRELAUNCH,
   ARMED,
   LAUNCHED,
   DESCENT,
   LANDED,
   NOT_SET = 0xFF
};

enum class ArmState : std::uint8_t {
   IDLE,
   ARMED,
};

enum class CommandResult : std::uint8_t {
   OK,
   NOT_ARMED,
   REPLAY_REJECT,
   WRONG_PHASE,
};

enum class CommandType : std::uint8_t {
   ARM,
   DISARM,
   SET_THRUST_SCALE,
   SET_THRUST_OFFSET,
   DUMP_FLASH,
};

typedef struct __attribute__((packed)) {
   // Header
   std::uint8_t sync_bytes[2];
   std::uint16_t telemetry_frame_index;
   std::uint32_t timestamp_ms; 
   // Tier A sensors
   float acceleration_main[3];
   float angular_velocity[3];
   float acceleration_high_g[3];
   float air_pressure;
   float air_temperature;
   float altitude;
   float engine_thrust;
   // Tier B sensors
   float oxidizer_weight;
   float fuel_pressure;
   float oxidizer_pressure;
   float pyro_battery_voltage;
   float main_battery_voltage;
   float engine_temperature_top;
   float engine_temperature_bottom;
   std::uint8_t piston_position;
   // Tier C sensors
   float gps_latitude;
   float gps_longitude;
   float gps_altitude;
   std::uint8_t gps_sattelite_count;
   float main_battery_level;
   // Tail metadata
   std::uint16_t is_valid_reading_mask;
   FlightPhase flight_phase;
   ArmState arm_state;
   std::uint16_t last_command_frame_index;
   CommandResult last_command_result;
   std::uint16_t crc;
} full_telemetry_frame_t;

typedef struct __attribute__((packed)) {
   // Header
   std::uint8_t sync_bytes[2]; 
   std::uint16_t telemetry_frame_index;
   std::uint32_t timestamp_ms; 
   // Tier A sensors
   float acceleration_main[3];
   float angular_velocity[3];
   float acceleration_high_g[3];
   float air_pressure;
   float air_temperature;
   float altitude;
   float engine_thrust;
   // Truncated tail metadata
   std::uint16_t is_valid_reading_mask;
   std::uint16_t crc;
} mini_telemetry_frame_t;

typedef struct __attribute__((packed)) {
   std::uint8_t sync[2];
   std::uint16_t command_frame_index;
   CommandType type;
   std::uint8_t payload[4];
   std::uint16_t crc;
} command_frame_t;

#endif
