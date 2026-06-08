// Integrates two thermocouples positioned at the top and bottom of the engine via MAX31855
#ifndef THERMOCOUPLES_H
#define THERMOCOUPLES_H

namespace thermocouples {
    bool begin(); // Initializes both thermocouples, returns true if successful
    bool readSensorData(); // Reads data from both thermocouples (degrees Celsius), returns true if successful
    void fill(float& top_temperature_c, float& bottom_temperature_c); // Fills the provided references with the latest temperature readings from the top and bottom thermocouples, respectively
}

#endif
