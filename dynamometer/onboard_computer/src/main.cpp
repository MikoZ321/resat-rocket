#include <Arduino.h>

#include "core/scheduler.h"
#include "core/ticker.h"

void setup() {
  Serial.begin(115200);
  scheduler::begin();
  ticker::begin();
}

void loop() {
  if (!ticker::consume()) return;

  scheduler::runTick(ticker::getTickCount());
}
