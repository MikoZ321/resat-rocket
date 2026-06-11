#include "oxidizer_loadcell.h"

#include <Adafruit_NAU7802.h>

#include "config.h"
#include "core/state.h"

Adafruit_NAU7802 nau;

static float s_oxidizer_weight;

namespace oxidizer_loadcell {
    bool begin() {
        if (!nau.begin()) {
            return false;
        }
        nau.setGain(NAU7802_GAIN_128);
        nau.setRate(NAU7802_RATE_80SPS);

        return true;
    }

    bool readSensorData() {
        if (!nau.available()) {
            state::clearValidMaskBit(OXIDIZER_LOADCELL_VALID_MASK_BIT);
            return false;
        }

        int32_t reading = nau.read();
        s_oxidizer_weight = reading * OXIDIZER_LOADCELL_SCALE + OXIDIZER_LOADCELL_OFFSET;

        state::setValidMaskBit(OXIDIZER_LOADCELL_VALID_MASK_BIT);
        return true; 
    }

    void fill(float& oxidizer_weight) {
        oxidizer_weight = s_oxidizer_weight;
    }
}