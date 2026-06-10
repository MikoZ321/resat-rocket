#include "main_gps.h"

#include <Arduino.h>
#include <TinyGPSPlus.h>

#include "config.h"

TinyGPSPlus tiny_gps;

static float s_latitude, s_longitude, s_altitude;
static std::uint8_t s_sattelite_count;

namespace main_gps {
    bool begin() {
        // TODO: add error checking
        Serial2.begin(MAIN_GPS_BAUD_RATE, SERIAL_8N1, MAIN_GPS_RX_PIN, MAIN_GPS_TX_PIN);

        return true;
    }

    bool readSensorData() {
        while (Serial2.available()) tiny_gps.encode(Serial2.read());

        bool is_complete_read = true;

        if (tiny_gps.location.isValid()) {
            s_latitude = tiny_gps.location.lat();
            s_longitude = tiny_gps.location.lng();
        }
        else is_complete_read = false;

        if (tiny_gps.altitude.isValid()) s_altitude = tiny_gps.altitude.meters();
        else is_complete_read = false;
        
        if (tiny_gps.satellites.isValid()) s_sattelite_count = (std::uint8_t) tiny_gps.satellites.value();
        else is_complete_read = false;

        return is_complete_read;
    }

    void fill(float& latitude, float& longitude, float& altitude, std::uint8_t& sattelite_count) {
        latitude = s_latitude;
        longitude = s_longitude;
        altitude = s_altitude;
        sattelite_count = s_sattelite_count;
    }
}
