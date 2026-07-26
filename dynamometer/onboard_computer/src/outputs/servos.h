#ifndef SERVOS_H
#define SERVOS_H

namespace servos {
    bool begin(); // Intializes the servos, returns true if successful
    bool rotateFuel(int degrees); // Rotates the fuel tank servo, returns true if successful
    bool rotateOxidizer(int degrees); // Rotates the oxidizer tank servo, returns true if successful
}

#endif
