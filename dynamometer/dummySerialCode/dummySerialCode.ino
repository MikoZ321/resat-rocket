// dummy code to test integration with dashboard on only one variable
const int TIME_BETWEEN_PACKETS = 100; // in miliseconds

void setup() {
  // must correspond to bandwith of serial connection in computerDashboard.py
  // arbitrarily set at highest possible, will be decreased after more calculations of data transfer needs
  Serial.begin(115200);
}

void loop() {
  // transmitting four alternating readings, delays correspond to a test sensor frequency of 10 Hz
  // values transmitted according to order described in README.md, hall effect sensors not implemented
  Serial.print(millis());
  Serial.println(";10;11;12;13;14;15;16;-17");
  delay(TIME_BETWEEN_PACKETS);
  Serial.print(millis());
  Serial.println(";80;81;82;83;84;85;86;-87");
  delay(TIME_BETWEEN_PACKETS);
  Serial.print(millis());
  Serial.println(";50;51;52;53;54;55;56;-57");
  delay(TIME_BETWEEN_PACKETS);
  Serial.print(millis());
  Serial.println(";100;101;102;103;104;105;106;-107");
  delay(TIME_BETWEEN_PACKETS);
}
