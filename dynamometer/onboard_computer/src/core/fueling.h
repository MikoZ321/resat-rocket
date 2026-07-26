#ifndef FUELING_H
#define FUELING_H

namespace fueling {
    bool begin(float target_mass); // Begin fueling
    void update(); // Async fueling execution
    void abort(); // Finishes the fueling process instantly
}

#endif
