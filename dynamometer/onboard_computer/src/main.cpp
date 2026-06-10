#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"
#include "outputs/leds.h"


void setup() {
  Serial.begin(115200);

  ticker::begin();
  scheduler::begin();
  state::begin();
}

void loop() {
  if (!ticker::consume()) return;

  leds::lightShow();
}
