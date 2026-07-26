#include "fueling.h"

#include <Arduino.h>

#include "core/state.h"
#include "sensors/oxidizer_loadcell.h"
#include "shared/types.h"
#include "outputs/servos.h"

constexpr float FREEFALL_ACCELERATION = 9.81f;
constexpr int FUELING_TIMEOUT = 30000;
constexpr int ACCEPTABLE_MASS_DELTA = 0.1;

static int begin_fueling_timestamp;
static float target_oxidizer_mass;

bool fueling::begin(float target_mass) {
    state::setFlightPhase(FlightPhase::FUELING);
    target_oxidizer_mass = target_mass;
    begin_fueling_timestamp = millis();
    servos::rotateOxidizer(90);

    return true;
}

void fueling::update() {
    if (millis() - begin_fueling_timestamp > FUELING_TIMEOUT) {
        fueling::abort();
        return;
    }

    float current_oxidizer_mass;
    oxidizer_loadcell::fill(current_oxidizer_mass);
    current_oxidizer_mass /= FREEFALL_ACCELERATION;
    if (current_oxidizer_mass > target_oxidizer_mass - ACCEPTABLE_MASS_DELTA || current_oxidizer_mass < target_oxidizer_mass + ACCEPTABLE_MASS_DELTA) {
        fueling::abort();
        return;
    }
}

void fueling::abort() {
    servos::rotateOxidizer(0);
    servos::rotateQuickDisconnect(90);
    state::setFlightPhase(FlightPhase::PRELAUNCH);
    return;
}
