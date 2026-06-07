// Integrates the oxidizer MH-series loadcell via the NAU7802 ADC
#ifndef OXIDIZER_LOADCELL_H
#define OXIDIZER_LOADCELL_H

namespace oxidizer_loadcell {
    bool begin(); // Initializes the oxidizer load cell, returns true if successful
    bool readSensorData(); // Reads data from the oxidizer load cell in Newtons, returns true if successful
    void fill(float& oxidizer_weight); // Fills the provided reference with the latest oxidizer load cell data in Newtons
}

#endif
