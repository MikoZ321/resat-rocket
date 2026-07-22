// The global state machine handler
#ifndef STATE_H
#define STATE_H

#include <cstdint>

#include "shared/types.h"

namespace state {
    void begin(); // Initializes the state machine handler
    ArmState getArmState(); // Returns the current arm state
    CommandResult getCommandResult(); // Returns the most recent command result
    FlightPhase getFlightPhase(); // Returns the current flight phase
    void persistFlightPhase(); // Persists the current flight phase to RTC memory, so it can be recovered after deep sleep
    void setCommandResult(CommandResult new_command_result);
    void setFlightPhase(FlightPhase new_flight_phase); // Switches the current flight phase to the new argument, which must be within the FlightPhase enum range
    void setValidMaskBit(int bit_position); // Sets the specified bit as valid in the mask
    void clearValidMaskBit(int bit_position); // Clears the specified bit
    std::uint16_t getValidMask(); // Returns the valid sensor reading mask
}

#endif
