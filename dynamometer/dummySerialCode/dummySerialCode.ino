// dummy code to test integration with dashboard on only one variable

void setup() {
  // must correspond to bandwith of serial connection in computerDashboard.py
  // arbitrarily set at highest possible, will be decreased after more calculations of data transfer needs
  Serial.begin(115200);
}

void loop() {
  // transmitting four alternating readings, delays correspond to a test sensor frequency of 1 Hz
  Serial.println("10");
  delay(1000);
  Serial.println("87");
  delay(1000);
  Serial.println("50");
  delay(1000);
  Serial.println("100");
  delay(1000);
}
