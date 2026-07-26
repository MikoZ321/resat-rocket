#include "analog_sensors.h"

#include <ADS1X15.h>

#include "config.h"
#include "core/state.h"

ADS1115 adc(MAIN_ADS1115_I2C_ADDRESS);

static float s_fuel_pressure;
static float s_oxidizer_pressure;
static float s_pyro_battery_voltage;
static float s_main_battery_voltage;

static float s_fuel_offset {FUEL_PRESSURE_OFFSET};
static float s_fuel_scale {FUEL_PRESSURE_SCALE};
static float s_oxidizer_scale {OXIDIZER_PRESSURE_SCALE};
static float s_oxidizer_offset {OXIDIZER_PRESSURE_OFFSET};

static float s_atmospheric_pressure;
static float s_fuel_atmospheric_reading;
static float s_oxidizer_atmospheric_reading;

namespace analog_sensors {
    bool begin() {
        if (!adc.begin()) {
            return false;
        }

        adc.setGain(ADS1X15_GAIN_6144MV);

        return true;
    }

    bool readSensorData() {
        // TODO: potentially check adc.getError() returns if not too time consuming
        if (!adc.isReady()) {
            state::clearValidMaskBit(FUEL_PRESSURE_VALID_MASK_BIT);
            state::clearValidMaskBit(OXIDIZER_PRESSURE_VALID_MASK_BIT);
            state::clearValidMaskBit(MAIN_BATTERY_VOLTAGE_VALID_MASK_BIT);
            state::clearValidMaskBit(PYRO_BATTERY_VOLTAGE_VALID_MASK_BIT);
            return false;
        }

        s_fuel_pressure = (adc.toVoltage(adc.readADC(FUEL_PRESSURE_ADC_CHANNEL)) - s_fuel_offset) * s_fuel_scale;
        s_oxidizer_pressure = (adc.toVoltage(adc.readADC(OXIDIZER_PRESSURE_ADC_CHANNEL)) - s_oxidizer_offset) * s_oxidizer_scale;
        s_pyro_battery_voltage = adc.toVoltage(adc.readADC(PYRO_BATTERY_VOLTAGE_ADC_CHANNEL)) * PYRO_BATTERY_VOLTAGE_SCALE + PYRO_BATTERY_VOLTAGE_OFFSET;
        s_main_battery_voltage = adc.toVoltage(adc.readADC(MAIN_BATTERY_VOLTAGE_ADC_CHANNEL)) * MAIN_BATTERY_VOLTAGE_SCALE + MAIN_BATTERY_VOLTAGE_OFFSET;

        state::setValidMaskBit(FUEL_PRESSURE_VALID_MASK_BIT);
        state::setValidMaskBit(OXIDIZER_PRESSURE_VALID_MASK_BIT);
        state::setValidMaskBit(MAIN_BATTERY_VOLTAGE_VALID_MASK_BIT);
        state::setValidMaskBit(PYRO_BATTERY_VOLTAGE_VALID_MASK_BIT);

        return true;
    }

    void fill(float& fuel_pressure, float& oxidizer_pressure, float& pyro_battery_voltage, float& main_battery_voltage) {
        fuel_pressure = s_fuel_pressure;
        oxidizer_pressure = s_oxidizer_pressure;
        pyro_battery_voltage = s_pyro_battery_voltage;
        main_battery_voltage = s_main_battery_voltage;
    }

    void setFuelScale(float scale) {
        s_fuel_scale = scale;
    }

    void setFuelOffset(float offset) {
        s_fuel_offset = offset;
    }

    void setOxidizerScale(float scale) {
        s_oxidizer_scale = scale;
    }

    void setOxidizerOffset(float offset) {
        s_oxidizer_offset = offset;
    }

    void tarePressure(float atmospheric_pressure, int sample_count) {
        s_atmospheric_pressure = atmospheric_pressure;
        float fuel_sum = 0.0f;
        float oxidizer_sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            fuel_sum += adc.toVoltage(adc.readADC(FUEL_PRESSURE_ADC_CHANNEL));
            oxidizer_sum += adc.toVoltage(adc.readADC(OXIDIZER_PRESSURE_ADC_CHANNEL));
            delay(5);
        }
        s_fuel_atmospheric_reading = fuel_sum / sample_count;
        s_oxidizer_atmospheric_reading = oxidizer_sum / sample_count;
    } 

    void calibrateFuel(float pressure, int sample_count) {
        float sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            sum += adc.toVoltage(adc.readADC(FUEL_PRESSURE_ADC_CHANNEL));
            delay(5);
        }
        float raw_p2 = sum / sample_count;

        float delta_raw = raw_p2 - s_fuel_atmospheric_reading;
        float scale  = (pressure - s_atmospheric_pressure) / delta_raw;
        float offset = s_fuel_atmospheric_reading - (s_atmospheric_pressure / scale);

        if (scale <= 0.0f) {
            return;   // non-physical result, reject
        }

        setFuelScale(scale);
        setFuelOffset(offset);
    }

    void calibrateOxidizer(float pressure, int sample_count) {
        float sum = 0.0f;
        for (int i = 0; i < sample_count; i++) {
            sum += adc.toVoltage(adc.readADC(OXIDIZER_PRESSURE_ADC_CHANNEL));
            delay(5);
        }
        float raw_p2 = sum / sample_count;

        float delta_raw = raw_p2 - s_oxidizer_atmospheric_reading;
        float scale  = (pressure - s_atmospheric_pressure) / delta_raw;
        float offset = s_oxidizer_atmospheric_reading - (s_atmospheric_pressure / scale);

        if (scale <= 0.0f) {
            return;   // non-physical result, reject
        }

        setOxidizerScale(scale);
        setOxidizerOffset(offset);
    }
}
