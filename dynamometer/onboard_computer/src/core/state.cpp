#include "state.h"

std::uint8_t s_current_flight_phase;

namespace state {
    void begin() {
        s_current_flight_phase = 0;
    }

    std::uint8_t getFlightPhase() {
        return s_current_flight_phase;
    }

    void setFlightPhase(std::uint8_t new_flight_phase) {
        s_current_flight_phase = new_flight_phase;
    }
}
