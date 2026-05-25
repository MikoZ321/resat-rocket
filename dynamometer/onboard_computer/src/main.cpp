#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"

void setup() {
  Serial.begin(115200);
  scheduler::begin();
  state::begin();
  ticker::begin();
}

void loop() {
  if (!ticker::consume()) return;

  scheduler::runTick(ticker::getTickCount());
}
