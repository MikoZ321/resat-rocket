#include "thrust_loadcell.h"

#include <Arduino.h>

#include "config.h"
#include "core/state.h"

static float s_engine_thrust;
static float s_scale {THRUST_LOADCELL_SCALE};
static float s_offset {THRUST_LOADCELL_OFFSET};

static constexpr float FREEFALL_ACCELERATION = 9.81f;

static std::int32_t getRaw() {
    std::uint32_t value = 0;

    noInterrupts();

    for (int i = 0; i < 24; i++)
    {
        digitalWrite(ADS1232_SCLK_PIN, HIGH);

        value <<= 1;

        if (digitalRead(ADS1232_DOUT_PIN))
            value |= 1;

        digitalWrite(ADS1232_SCLK_PIN, LOW);
    }

    // Gain = 128, Channel A
    digitalWrite(ADS1232_SCLK_PIN, HIGH);
    digitalWrite(ADS1232_SCLK_PIN, LOW);

    interrupts();
    // Sign extend
    std::int32_t raw;
    if (value & 0x800000) raw = (std::int32_t)(value | 0xFF000000);
    else raw = (std::int32_t)value;

    return raw;
}

namespace thrust_loadcell {
    // TODO: check setup and return value
    bool begin() {
        pinMode(ADS1232_SCLK_PIN, OUTPUT);
        digitalWrite(ADS1232_SCLK_PIN, LOW);

        pinMode(ADS1232_DOUT_PIN, INPUT);

        return true;
    }
    
    bool readSensorData() {
        // Data not ready
        if (digitalRead(ADS1232_DOUT_PIN)) {
            state::clearValidMaskBit(THRUST_LOADCELL_VALID_MASK_BIT);
            return false;
        }

        std::int32_t raw = getRaw();
        s_engine_thrust = (raw - s_offset) * s_scale;

        state::setValidMaskBit(THRUST_LOADCELL_VALID_MASK_BIT);
        return true;
    }

    void fill(float& engine_thrust) {
        engine_thrust = s_engine_thrust;
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
            sum += getRaw();
            delay(5);
        }
        float avg = sum / sample_count;
        setOffset(avg);
    }

    void calibrate(float known_mass, int sample_count) {
        float sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            sum += getRaw();
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