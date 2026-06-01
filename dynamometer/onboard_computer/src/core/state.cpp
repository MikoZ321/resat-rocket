#include "state.h"

#include "esp_attr.h"

RTC_DATA_ATTR static FlightPhase s_persisted_phase = FlightPhase::NOT_SET;

FlightPhase s_current_flight_phase;

namespace state {
    // TODO: improve persistence
    void begin() {
        if (s_persisted_phase != FlightPhase::NOT_SET) {
            s_current_flight_phase = s_persisted_phase;
            return;
        }
        s_current_flight_phase = FlightPhase::PRELAUNCH;
    }

    FlightPhase getFlightPhase() {
        return s_current_flight_phase;
    }
    
    void persistFlightPhase() {
        s_persisted_phase = s_current_flight_phase;
    }

    void setFlightPhase(FlightPhase new_flight_phase) {
        s_current_flight_phase = new_flight_phase;
    }
}
