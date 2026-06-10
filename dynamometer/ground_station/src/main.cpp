#include <Arduino.h>

#include "config.h"
#include "radio.h"

void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD_RATE);
  radio::begin();
}

void loop() {

}