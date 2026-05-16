#include <Arduino.h>

#include "scheduler/scheduler.h"
#include "ticker/ticker.h"

void setup() {
  Serial.begin(115200);
  scheduler::begin();
  ticker::begin();
}

void loop() {
  if (!ticker::consume()) return;

  scheduler::runTick(ticker::getTickCount());
}
