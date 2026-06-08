// Integrates all of the sensors running through the main ADS1115 ADC, including the fuel and oxidizer pressure transducers and the pyro and main battery voltage levels
#ifndef ANALOG_SENSORS_H
#define ANALOG_SENSORS_H

namespace analog_sensors {
    bool begin(); // Initializes the ADC, returns true if successful
    bool readSensorData(); // Reads data from the ADC, returns true if successful
    void fill(float& fuel_pressure, float& oxidizer_pressure, float& pyro_battery_voltage, float& main_battery_voltage); // Fills the provided variables with the latest sensor readings, in engineering units (Pa for pressures, V for voltages)
}

#endif