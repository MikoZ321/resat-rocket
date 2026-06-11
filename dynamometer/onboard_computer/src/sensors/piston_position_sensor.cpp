#include "piston_position_sensor.h"

#include <Arduino.h>
#include <ADS1X15.h>

#include "config.h"

ADS1115 adc_0(EXTERNAL_ADS1115_0_I2C_ADDRESS);
ADS1115 adc_1(EXTERNAL_ADS1115_1_I2C_ADDRESS);
ADS1115 adc_2(EXTERNAL_ADS1115_2_I2C_ADDRESS);

static float s_hall_data[12] = {};

namespace piston_position_sensor {
    bool begin() {
        if (!adc_0.begin()) return false;
        if (!adc_1.begin()) return false;
        if (!adc_2.begin()) return false;

        return true;
    }

    void dumpHallData() {
        for (int i = 0; i < 12; i++) {
            Serial.print(s_hall_data[i]);
            Serial.print(", ");
        }

        Serial.println("");
    }

    bool readSensorData() {
        for (int i = 0; i < 4; i++) {
            s_hall_data[i] = adc_0.readADC(i);
        }

        for (int i = 0; i < 4; i++) {
            s_hall_data[4 + i] = adc_1.readADC(i);
        }

        for (int i = 0; i < 4; i++) {
            s_hall_data[8 + i] = adc_2.readADC(i);
        }

        return true;
    }

    void fill(std::uint8_t& piston_position) {
        // TODO: implement
        piston_position = 0;
        return;
    }
}