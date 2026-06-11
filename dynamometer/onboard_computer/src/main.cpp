#include <Arduino.h>
#include <Wire.h>

#include "config.h"
//#include "core/scheduler.h"
//#include "core/state.h"
#include "core/ticker.h"
#include "sensors/analog_sensors.h"
#include "outputs/multiplexer.h"

void setup() {
  Serial.begin(115200);

  ticker::begin();
  //scheduler::begin();
  //state::begin();
  // Initialize I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQUENCY_HZ);
  Wire.setTimeOut(I2C_TIMEOUT_MS);
  Serial.println("[INIT] OK: I2C");

  multiplexer::begin();
  analog_sensors::begin();
}

static float main_battery_voltage, pyro_battery_voltage, fuel_pressure, oxidizer_pressure;

void loop() {
  if (!ticker::consume()) return;

  analog_sensors::readSensorData();
  analog_sensors::fill(fuel_pressure, oxidizer_pressure, pyro_battery_voltage, main_battery_voltage);
  Serial.print(main_battery_voltage);
  Serial.print(", ");
  Serial.println(pyro_battery_voltage);

  /*if (ticker::getTickCount() % 40 == 0) {
    Serial.println("Igniting");
    multiplexer::igniteEngine();
  }*/
}
