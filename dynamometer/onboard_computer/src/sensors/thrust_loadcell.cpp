#include "thrust_loadcell.h"

#include <Arduino.h>

#include "config.h"
#include "core/state.h"

static float s_engine_thrust;
static float s_thrust_scale;
static float s_thrust_offset;

namespace thrust_loadcell {
    // TODO: check setup and return value
    bool begin() {
        pinMode(ADS1232_SCLK_PIN, OUTPUT);
        digitalWrite(ADS1232_SCLK_PIN, LOW);

        pinMode(ADS1232_DOUT_PIN, INPUT);

        s_thrust_scale = THRUST_LOADCELL_SCALE;
        s_thrust_offset = THRUST_LOADCELL_OFFSET;

        return true;
    }
    // TODO: Check if blocking is not a problem
    bool readSensorData() {
        // Data not ready
        if (digitalRead(ADS1232_DOUT_PIN))
            return false;

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
        if (value & 0x800000)
            raw = (std::int32_t)(value | 0xFF000000);
        else
            raw = (std::int32_t)value;

        s_engine_thrust = raw * s_thrust_scale + s_thrust_offset;

        return true;
    }

    void fill(float& engine_thrust) {
        engine_thrust = s_engine_thrust;
    }

    void setScale(float scale) {
        s_thrust_scale = scale;
    }

    void setOffset(float offset) {
        s_thrust_offset = offset;
    }
}