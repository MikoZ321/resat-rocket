// Integrates the external Hall effect sensors used to gauge the piston position
#ifndef PISTON_POSITION_SENSOR_H
#define PISTON_POSITION_SENSOR_H

#include <cstdint>

namespace piston_position_sensor {
    bool begin(); // Initializes the external module, returns true if successful
    void dumpHallData(); // Dumps the raw Hall effect readings
    bool readSensorData(); // Estimates the piston position, returns true if successful
    void fill(std::uint8_t& piston_position); // Fills the provided address with the most recent sensor reading
}

#endif