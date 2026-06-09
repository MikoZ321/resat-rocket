// Integrates the M10Q-5883 GPS over UART
#ifndef MAIN_GPS_H
#define MAIN_GPS_H

#include <cstdint>

namespace main_gps {
    bool begin(); // Initializes the gps, returns true if successful
    bool readSensorData(); // Reads the latitude, longitude, altitude, and sattelite count
    void fill(float& latitude, float& longitude, float& altitude, std::uint8_t& sattelite_count); // Fills the provided addresses with the most recent data
}

#endif
