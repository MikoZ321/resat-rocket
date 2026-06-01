#include "main_imu.h"

#include <Adafruit_LSM6DSOX.h>

#include "config.h"

Adafruit_LSM6DSOX lsm6dsox;

static float s_acceleration[3] = {};
static float s_angular_velocity[3] = {};

namespace main_imu {
    bool begin() {
        if (!lsm6dsox.begin_I2C(LSM6DSOX_I2C_ADDRESS, &Wire, 0)) {
            Serial.println("[ERROR] Failed to initialize main IMU");
            return false;
        }

        lsm6dsox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
        lsm6dsox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
        lsm6dsox.setAccelDataRate(LSM6DS_RATE_104_HZ);
        lsm6dsox.setGyroDataRate(LSM6DS_RATE_104_HZ);

        return true; 
    }

    bool readSensorData() {
        sensors_event_t accel;
        sensors_event_t gyro;
        sensors_event_t temp; // Value initalized and disregarded to prevent potential crashes from using nullptr
        if (!lsm6dsox.getEvent(&accel, &gyro, &temp)) return false; 

        s_acceleration[0] = accel.acceleration.x;
        s_acceleration[1] = accel.acceleration.y;
        s_acceleration[2] = accel.acceleration.z;

        s_angular_velocity[0] = gyro.gyro.x;
        s_angular_velocity[1] = gyro.gyro.y;
        s_angular_velocity[2] = gyro.gyro.z;
        return true;
    }

    void fill(float acceleration[3], float angular_velocity[3]) {
        acceleration[0] = s_acceleration[0];
        acceleration[1] = s_acceleration[1];
        acceleration[2] = s_acceleration[2];

        angular_velocity[0] = s_angular_velocity[0];
        angular_velocity[1] = s_angular_velocity[1];
        angular_velocity[2] = s_angular_velocity[2];
    }
}
