#include <Arduino.h>

#include "config.h"
#include "radio.h"
#include "usb_forward.h"

void setup() {
  usb_forward::begin();
  radio::begin();
}

void loop() {
  radio::process();
  delay(1);
}