#include "thrust_loadcell.h"

#include <ADS123X.h>

#include "config.h"

ADS123X ads;

static float s_engine_thrust;

namespace thrust_loadcell {
    // TODO: check setup and return value
    bool begin() {
        ads.begin(ADS1232_DOUT_PIN, ADS1232_SCLK_PIN, ADS1232_PWDN_PIN, -1, -1, -1, -1, -1);
        // Without remaining pin config cannot set gain or speed
        ads.setGain(GAIN128);
        ads.setSpeed(FAST);
        ads.set_scale(AIN1, THRUST_LOADCELL_SCALE);
        ads.set_offset(AIN1, THRUST_LOADCELL_OFFSET);
        return ads.is_ready();
    }
    // TODO: Check if blocking is not a problem
    bool readSensorData() {
        if (ads.get_units(AIN1, s_engine_thrust) != NoERROR) {
            return false;
        }

        return true; 
    }

    void fill(float& engine_thrust) {
        engine_thrust = s_engine_thrust;
    }
}