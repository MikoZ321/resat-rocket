// Integrates the oxidizer MH-series loadcell via the NAU7802 ADC
#ifndef OXIDIZER_LOADCELL_H
#define OXIDIZER_LOADCELL_H

namespace oxidizer_loadcell {
    bool begin(); // Initializes the oxidizer load cell, returns true if successful
    bool readSensorData(); // Reads data from the oxidizer load cell in Newtons, returns true if successful
    void fill(float& oxidizer_weight); // Fills the provided reference with the latest oxidizer load cell data in Newtons
    void setScale(float scale); // Sets the scale for the oxidizer loadcell
    void setOffset(float offset); // Sets the offset for the oxidizer loadcell
    void tare(int sample_count = 32); // Tares the loadcell averaging the provided number of samples to set the offset
    void calibrate(float known_mass, int sample_count = 32); // Calibrates the loadcell averaging the provided number of samples to set the correct scale based on the provided mass in kg
}

#endif
