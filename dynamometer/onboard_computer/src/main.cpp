#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"

#include "sensors/thrust_loadcell.h"

void setup() {
  Serial.begin(115200);

  ticker::begin();
  state::begin();
  scheduler::begin();
}

void loop() {
  if (!ticker::consume()) return;

  // scheduler::runTick(ticker::getTickCount());
  thrust_loadcell::readSensorData();
  float temp {};
  thrust_loadcell::fill(temp);
  Serial.println(temp);
}
