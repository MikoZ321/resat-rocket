#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"

void setup() {
  Serial.begin(115200);

  ticker::begin();
  state::begin();
  scheduler::begin();
}

void loop() {
  if (!ticker::consume()) return;

  // Delay to prevent error in tick count
  delay(1);
  scheduler::runTick(ticker::getTickCount());
}
