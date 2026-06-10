#include "camera.h"

#include <Arduino.h>

#include "config.h"

namespace camera {
    bool begin() {
        pinMode(CAMERA_TRIGGER_PIN, OUTPUT);
        digitalWrite(CAMERA_TRIGGER_PIN, HIGH);

        return true;
    }

    void trigger() {
        digitalWrite(CAMERA_TRIGGER_PIN, LOW);
        delay(200);
        digitalWrite(CAMERA_TRIGGER_PIN, HIGH);
    }
}