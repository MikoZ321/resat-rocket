// Integrates the H3LIS331DL high-g accelerometer
#ifndef HIGH_G_ACCELEROMETER_H
#define HIGH_G_ACCELEROMETER_H

namespace high_g_accelerometer {
    bool begin(); // Initializes the high-g accelerometer, returns true if successful
    bool readSensorData(); // Reads data from the high-g accelerometer in m/s^2, returns true if successful
    void fill(float acceleration_high_g[3]); // Fills the provided array with the latest high-g accelerometer data in m/s^2
}

#endif