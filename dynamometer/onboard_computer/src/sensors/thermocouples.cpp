#include "thermocouples.h"

#include <Adafruit_MAX31856.h>

#include "config.h"

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
        if (thermocouple_top.readFault()) return false;
        s_top_temperature_c = thermocouple_top.readThermocoupleTemperature();

        if (thermocouple_bottom.readFault()) return false;
        s_bottom_temperature_c = thermocouple_bottom.readThermocoupleTemperature();

        return true;
    }

    void fill(float& top_temperature_c, float& bottom_temperature_c) {
        top_temperature_c = s_top_temperature_c;
        bottom_temperature_c = s_bottom_temperature_c;
    }
}
