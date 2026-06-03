#include "atmospheric_sensor.h"

#include <SparkFun_BMP581_Arduino_Library.h>

#include "config.h"

BMP581 bmp581 = BMP581();

static float s_pressure_pa = 0.0f;
static float s_temperature_c = 0.0f;

namespace atmospheric_sensor {
    bool begin() {
        if (bmp581.beginI2C(BMP581_I2C_ADDRESS, Wire) != BMP5_OK) return false;

        // TODO: check if not redundant with what is below
        bmp581.setODRFrequency(BMP5_ODR_50_HZ);

        bmp5_osr_odr_press_config osr_config = {};
        osr_config.osr_p = BMP5_OVERSAMPLING_4X;
        osr_config.odr = BMP5_ODR_50_HZ;
        bmp581.setOSRMultipliers(&osr_config);

        return true;
    }

    bool readSensorData() {
        bmp5_sensor_data data;
        if (bmp581.getSensorData(&data) != BMP5_OK) return false;

        s_pressure_pa = data.pressure;
        s_temperature_c = data.temperature;

        return true;
    }

    void fill(float& pressure_pa, float& temperature_c) {
        pressure_pa = s_pressure_pa;
        temperature_c = s_temperature_c;
    }
}