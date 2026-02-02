// dummy code to test integration with dashboard on only one variable

void setup() {
  // arbitrarily set at highest possible, will be decreased after more calculations of data transfer needs
  Serial.begin(115200);
}

void loop() {
  // transmitting two alternating readings, delays correspond to expected sensor collection frequency of 100 Hz
  Serial.println("10");
  delay(10);
  Serial.println("87");
  delay(10);
}
