#include <Arduino.h>

#include "command_relay.h"
#include "config.h"
#include "radio.h"
#include "usb_forward.h"

void setup() {
  usb_forward::begin();
  radio::begin();
  command_relay::begin();
}

void loop() {
  radio::process();
  command_relay::process();
  delay(1);
}