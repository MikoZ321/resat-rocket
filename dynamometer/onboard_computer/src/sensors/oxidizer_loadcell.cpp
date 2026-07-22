#include "oxidizer_loadcell.h"

#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

#include "config.h"
#include "core/state.h"

NAU7802 nau;

static float s_oxidizer_weight;

namespace oxidizer_loadcell {
    bool begin() {
        if (!nau.begin()) {
            return false;
        }
        nau.setGain(NAU7802_GAIN_128);
        nau.setSampleRate(NAU7802_SPS_80);

        return true;
    }

    bool readSensorData() {
        if (!nau.available()) {
            state::clearValidMaskBit(OXIDIZER_LOADCELL_VALID_MASK_BIT);
            return false;
        }

        int32_t reading = nau.getReading();
        s_oxidizer_weight = reading * OXIDIZER_LOADCELL_SCALE + OXIDIZER_LOADCELL_OFFSET;

        state::setValidMaskBit(OXIDIZER_LOADCELL_VALID_MASK_BIT);
        return true; 
    }

    void fill(float& oxidizer_weight) {
        oxidizer_weight = s_oxidizer_weight;
    }
}