// Integrates all of the sensors running through the main ADS1115 ADC, including the fuel and oxidizer pressure transducers and the pyro and main battery voltage levels
#ifndef ANALOG_SENSORS_H
#define ANALOG_SENSORS_H

namespace analog_sensors {
    bool begin(); // Initializes the ADC, returns true if successful
    bool readSensorData(); // Reads data from the ADC, returns true if successful
    void fill(float& fuel_pressure, float& oxidizer_pressure, float& pyro_battery_voltage, float& main_battery_voltage); // Fills the provided variables with the latest sensor readings, in engineering units (Pa for pressures, V for voltages)
    void setFuelScale(float scale); // Sets the scale factor of the fuel pressure transducer
    void setFuelOffset(float offset); // Sets the offset of the fuel pressure transducer
    void setOxidizerScale(float scale); // Sets the scale factor of the oxidizer pressure transducer
    void setOxidizerOffset(float offset); // Sets the offset of the oxidizer pressure transducer
    void tarePressure(float atmospheric_pressure, int sample_count = 32); // Measures the averaged raw readings of the pressure transducers at atmospheric pressure
    void calibrateFuel(float pressure, int sample_count = 32); // Calibrates the fuel transducer given a known pressure reading, assumes tarePressure has been run
    void calibrateOxidizer(float pressure, int sample_count = 32); // Calibrates the oxidizer transducer given a known pressure reading, assumes tarePressure has been run
}

#endif