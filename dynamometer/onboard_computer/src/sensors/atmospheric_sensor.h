// Integrates the BMP581 barometric pressure and temperature sensor
#ifndef ATMOSPHERIC_SENSOR_H
#define ATMOSPHERIC_SENSOR_H

namespace atmospheric_sensor {
    bool begin(); // Initializes the atmospheric sensor, returns true if successful
    bool readSensorData(); // Reads data from the atmospheric sensor (Pa, *C), returns true if successful
    void fill(float& pressure_pa, float& temperature_c); // Fills the provided variables with the latest pressure in Pa and temperature in degrees Celsius
}

#endif