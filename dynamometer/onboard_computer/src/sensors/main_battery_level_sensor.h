#ifndef MAIN_BATTERY_LEVEL_SENSOR_H
#define MAIN_BATTERY_LEVEL_SENSOR_H

namespace main_battery_level_sensor {
    bool begin(); // Initializes the battery level sensor, returns true if successful
    bool readSensorData(); // Reads the most recent main battery level in percentage, returns true if successful
    void fill(float& main_battery_level); // Fills the provided reference with the most recent battery level as a percentage
}

#endif
