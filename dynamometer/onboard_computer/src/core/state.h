// The global state machine handler
#ifndef STATE_H
#define STATE_H

#include <cstdint>

namespace state {
    void begin(); // Initializes the state machine handler
    std::uint8_t getFlightPhase(); // Returns the current flight phase
    void setFlightPhase(std::uint8_t new_flight_phase); // Switches the current flight phase to the new argument, which must be within the FlightPhase enum range
}

#endif
