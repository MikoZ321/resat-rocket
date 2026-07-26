#include "state.h"

#include "esp_attr.h"

RTC_DATA_ATTR static FlightPhase s_persisted_phase = FlightPhase::NOT_SET;

static ArmState s_current_arm_state;
static CommandResult s_last_command_result;
static FlightPhase s_current_flight_phase;
static std::uint16_t s_is_valid_reading_mask;

namespace state {
    // TODO: improve persistence
    void begin() {
        if (s_persisted_phase != FlightPhase::NOT_SET) {
            s_current_flight_phase = s_persisted_phase;
            return;
        }
        s_current_flight_phase = FlightPhase::PRELAUNCH;

        s_is_valid_reading_mask = 0;
    }

    ArmState getArmState() {
        return s_current_arm_state;
    }

    CommandResult getCommandResult() {
        return s_last_command_result;
    }

    FlightPhase getFlightPhase() {
        return s_current_flight_phase;
    }
    
    void persistFlightPhase() {
        s_persisted_phase = s_current_flight_phase;
    }

    void setArmState(ArmState new_arm_state) {
        s_current_arm_state = new_arm_state;
    }

    void setCommandResult(CommandResult new_command_result) {
        s_last_command_result = new_command_result;
    }

    void setFlightPhase(FlightPhase new_flight_phase) {
        s_current_flight_phase = new_flight_phase;
    }

    void setValidMaskBit(int bit_position) {
        if (bit_position > 15 || bit_position < 0) return;

        std::uint16_t mask = 1 << bit_position;
        s_is_valid_reading_mask |= mask;
    }

    void clearValidMaskBit(int bit_position) {
        if (bit_position > 15 || bit_position < 0) return;

        std::uint16_t mask = 1 << bit_position;
        s_is_valid_reading_mask &= ~mask;
    }

    std::uint16_t getValidMask() {
        return s_is_valid_reading_mask;
    }
}
