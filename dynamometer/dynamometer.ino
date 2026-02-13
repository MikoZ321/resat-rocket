// dummy code to test integration with dashboard on only one variable

void setup() {
  // must correspond to bandwith of serial connection in groundStation.py
  // arbitrarily set at highest possible, will be decreased after more calculations of data transfer needs
  Serial.begin(115200);
}

void loop() {
  // transmitting two alternating readings, delays correspond to a test sensor frequency of 10 Hz
  Serial.println("10");
  delay(100);
  Serial.println("87");
  delay(100);
}
