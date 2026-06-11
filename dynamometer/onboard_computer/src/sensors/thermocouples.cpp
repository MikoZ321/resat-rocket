#include "thermocouples.h"

#include <Adafruit_MAX31856.h>

#include "config.h"
#include "core/state.h"

Adafruit_MAX31856 thermocouple_top(THERMOCOUPLE_TOP_CS_PIN);
Adafruit_MAX31856 thermocouple_bottom(THERMOCOUPLE_BOTTOM_CS_PIN);

static float s_top_temperature_c;
static float s_bottom_temperature_c;

namespace thermocouples {
    bool begin() {
        if (!thermocouple_top.begin()) return false;
        if (!thermocouple_bottom.begin()) return false;

        // TODO: check actual thermocouple types 
        thermocouple_top.setThermocoupleType(MAX31856_TCTYPE_K);
        thermocouple_bottom.setThermocoupleType(MAX31856_TCTYPE_K);

        return true;
    }

    bool readSensorData() {
        bool is_valid_read = true;

        if (thermocouple_top.readFault()) {
            state::clearValidMaskBit(THERMOCOUPLE_TOP_VALID_MASK_BIT);
            is_valid_read = false;
        }
        else {
            s_top_temperature_c = thermocouple_top.readThermocoupleTemperature();
            state::setValidMaskBit(THERMOCOUPLE_TOP_VALID_MASK_BIT);
        }

        if (thermocouple_bottom.readFault()) {
            state::clearValidMaskBit(THERMOCOUPLE_BOTTOM_VALID_MASK_BIT);
            is_valid_read = false;
        }
        else {
            s_bottom_temperature_c = thermocouple_bottom.readThermocoupleTemperature();
            state::setValidMaskBit(THERMOCOUPLE_BOTTOM_VALID_MASK_BIT);
        }

        return is_valid_read;
    }

    void fill(float& top_temperature_c, float& bottom_temperature_c) {
        top_temperature_c = s_top_temperature_c;
        bottom_temperature_c = s_bottom_temperature_c;
    }
}
