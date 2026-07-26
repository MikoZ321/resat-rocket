// Integrates the thrust load cell via the ADS1232 ADC
#ifndef THRUST_LOADCELL_H
#define THRUST_LOADCELL_H

#include <cstdint>

namespace thrust_loadcell {
    bool begin(); // Initializes the thrust load cell, returns true if successful
    bool readSensorData(); // Reads data from the thrust load cell in Newtons, returns true if successful
    void fill(float& engine_thrust); // Fills the provided reference with the latest thrust load cell data in Newtons
    void setScale(float scale); // Sets the scale factor for the thrust load cell
    void setOffset(float offset); // Sets the offset for the thrust load cell
    void tare(int sample_count = 32); // Tares the loadcell averaging the provided number of samples to set the offset
    void calibrate(float known_mass, int sample_count = 32); // Calibrates the loadcell averaging the provided number of samples to set the correct scale based on the provided mass in kg
}

#endif
