// Integrates a camera trigger
#ifndef CAMERA_H
#define CAMERA_H

namespace camera {
    bool begin(); // Initializes the camera trigger, returns true if successful
    void trigger(); // Triggers the camera
}

#endif