#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"
#include "sensors/piston_position_sensor.h"

void setup() {
  Serial.begin(115200);

  ticker::begin();
  scheduler::begin();
  state::begin();
}

void loop() {
  if (!ticker::consume()) return;

  piston_position_sensor::readSensorData();
  piston_position_sensor::dumpHallData();
}
