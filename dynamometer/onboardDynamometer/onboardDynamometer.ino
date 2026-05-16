#include "ticker.h"

void setup() {
    Serial.begin(115200);
    ticker::begin();
}

void loop() {
    if (!ticker::consume()) return;
    Serial.print("Current tick: ");
    Serial.print(ticker::getTickCount());
    Serial.print(", Number of missed ticks: ");
    Serial.println(ticker::getMissedTickCount());
}