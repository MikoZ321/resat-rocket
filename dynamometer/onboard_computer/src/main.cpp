#include <Arduino.h>

#include "core/scheduler.h"
#include "core/state.h"
#include "core/ticker.h"
#include "sensors/analog_sensors.h"

float oxidizer_pressure, fuel_pressure, pyro_battery_voltage, main_battery_voltage;

void setup() {
  Serial.begin(115200);

  ticker::begin();
  scheduler::begin();
  state::begin();
}

void loop() {
  if (!ticker::consume()) return;

  analog_sensors::readSensorData();
  
  analog_sensors::fill(fuel_pressure, oxidizer_pressure, pyro_battery_voltage, main_battery_voltage);

  Serial.print(ticker::getTickCount());
  Serial.print(": ");
  Serial.print(fuel_pressure);
  Serial.print(" ");
  Serial.print(oxidizer_pressure);
  Serial.print(" ");
  Serial.print(pyro_battery_voltage);
  Serial.print(" ");
  Serial.println(main_battery_voltage);
}
