#include "oxidizer_loadcell.h"

#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

#include "config.h"
#include "core/state.h"

NAU7802 nau;

static float s_oxidizer_weight;
static float s_offset {OXIDIZER_LOADCELL_OFFSET};
static float s_scale {OXIDIZER_LOADCELL_SCALE};

static constexpr float FREEFALL_ACCELERATION {9.81f};

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
        s_oxidizer_weight = (reading - s_offset) * s_scale;

        state::setValidMaskBit(OXIDIZER_LOADCELL_VALID_MASK_BIT);
        return true; 
    }

    void fill(float& oxidizer_weight) {
        oxidizer_weight = s_oxidizer_weight;
    }

    void setScale(float scale) {
        s_scale = scale;
    }

    void setOffset(float offset) {
        s_offset = offset;
    }

    void tare(int sample_count) {
        float sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            sum += nau.getReading();
            delay(5);
        }
        float avg = sum / sample_count;
        setOffset(avg);
    }


    void calibrate(float known_mass, int sample_count) {
        float sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            sum += nau.getReading();
            delay(5);
        }
        float avg = sum / sample_count;
        float delta = avg - s_offset;
        if (delta == 0.0f) {
            // Avoid divide-by-zero; caller should have tared first with real weight applied
            return;
        }
        float scale = (known_mass * FREEFALL_ACCELERATION) / delta;
        setScale(scale);
    }
}