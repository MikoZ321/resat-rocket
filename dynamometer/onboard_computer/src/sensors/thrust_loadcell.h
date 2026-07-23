// Integrates the thrust load cell via the ADS1232 ADC
#ifndef THRUST_LOADCELL_H
#define THRUST_LOADCELL_H

namespace thrust_loadcell {
    bool begin(); // Initializes the thrust load cell, returns true if successful
    bool readSensorData(); // Reads data from the thrust load cell in Newtons, returns true if successful
    void fill(float& engine_thrust); // Fills the provided reference with the latest thrust load cell data in Newtons
    void setScale(float scale); // Sets the scale factor for the thrust load cell
    void setOffset(float offset); // Sets the offset for the thrust load cell
}

#endif
