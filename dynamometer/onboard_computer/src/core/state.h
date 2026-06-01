// The global state machine handler
#ifndef STATE_H
#define STATE_H

#include <cstdint>

#include "shared/types.h"

namespace state {
    void begin(); // Initializes the state machine handler
    FlightPhase getFlightPhase(); // Returns the current flight phase
    void persistFlightPhase(); // Persists the current flight phase to RTC memory, so it can be recovered after deep sleep
    void setFlightPhase(FlightPhase new_flight_phase); // Switches the current flight phase to the new argument, which must be within the FlightPhase enum range
}

#endif
