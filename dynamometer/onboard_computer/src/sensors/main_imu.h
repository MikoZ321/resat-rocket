// Integrates the LSM9DSOX
#ifndef MAIN_IMU_H
#define MAIN_IMU_H

namespace main_imu {
    bool begin(); // Initializes the main IMU, returns true if successful
    bool readSensorData(); // Polls the main IMU for new data (m/s^2, rad/s), returns true if successful
    void fill(float acceleration[3], float angular_velocity[3]); // Fills the provided arrays with the latest main IMU data (m/s^2, rad/s)
}

#endif
