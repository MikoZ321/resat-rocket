#include "high_g_accelerometer.h"

#include <Adafruit_H3LIS331.h>

#include "config.h"
#include "core/state.h"

Adafruit_H3LIS331 h3lis331 = Adafruit_H3LIS331();

static float s_acceleration_high_g[3] = {};

namespace high_g_accelerometer {
    bool begin() {
        if (!h3lis331.begin_I2C(H3LIS331_I2C_ADDRESS, &Wire, 0)) return false;

        h3lis331.setRange(H3LIS331_RANGE_100_G);
        h3lis331.setDataRate(LIS331_DATARATE_50_HZ);

        return true;
    }

    bool readSensorData() {
        sensors_event_t acceleration_high_g;
        if (!h3lis331.getEvent(&acceleration_high_g)) {
            state::clearValidMaskBit(H3LIS331_VALID_MASK_BIT);
            return false;
        }

        s_acceleration_high_g[0] = acceleration_high_g.acceleration.x;
        s_acceleration_high_g[1] = acceleration_high_g.acceleration.y;
        s_acceleration_high_g[2] = acceleration_high_g.acceleration.z;

        state::setValidMaskBit(H3LIS331_VALID_MASK_BIT);
        return true;
    }

    void fill(float acceleration_high_g[3]) {
        acceleration_high_g[0] = s_acceleration_high_g[0];
        acceleration_high_g[1] = s_acceleration_high_g[1];
        acceleration_high_g[2] = s_acceleration_high_g[2];
    }
}