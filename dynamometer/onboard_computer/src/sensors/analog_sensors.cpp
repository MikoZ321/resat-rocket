#include "analog_sensors.h"

#include <ADS1X15.h>

#include "config.h"

ADS1115 adc(MAIN_ADS1115_I2C_ADDRESS);

static float s_fuel_pressure;
static float s_oxidizer_pressure;
static float s_pyro_battery_voltage;
static float s_main_battery_voltage;

namespace analog_sensors {
    bool begin() {
        if (!adc.begin()) {
            return false;
        }

        return true;
    }

    bool readSensorData() {
        // TODO: potentially check adc.getError() returns if not too time consuming
        if (!adc.isReady()) {
            return false;
        }

        s_fuel_pressure = adc.toVoltage(adc.readADC(FUEL_PRESSURE_ADC_CHANNEL)) * FUEL_PRESSURE_SCALE + FUEL_PRESSURE_OFFSET;
        s_oxidizer_pressure = adc.toVoltage(adc.readADC(OXIDIZER_PRESSURE_ADC_CHANNEL)) * OXIDIZER_PRESSURE_SCALE + OXIDIZER_PRESSURE_OFFSET;
        s_pyro_battery_voltage = adc.toVoltage(adc.readADC(PYRO_BATTERY_VOLTAGE_ADC_CHANNEL)) * PYRO_BATTERY_VOLTAGE_SCALE + PYRO_BATTERY_VOLTAGE_OFFSET;
        s_main_battery_voltage = adc.toVoltage(adc.readADC(MAIN_BATTERY_VOLTAGE_ADC_CHANNEL)) * MAIN_BATTERY_VOLTAGE_SCALE + MAIN_BATTERY_VOLTAGE_OFFSET;

        return true;
    }

    void fill(float& fuel_pressure, float& oxidizer_pressure, float& pyro_battery_voltage, float& main_battery_voltage) {
        fuel_pressure = s_fuel_pressure;
        oxidizer_pressure = s_oxidizer_pressure;
        pyro_battery_voltage = s_pyro_battery_voltage;
        main_battery_voltage = s_main_battery_voltage;
    }
}
