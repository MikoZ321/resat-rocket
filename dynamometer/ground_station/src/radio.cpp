#include "radio.h"

#include <Arduino.h>

#include "config.h"
#include "shared/communication_protocol.h"

namespace radio {
    bool begin() {
       Serial1.begin(RADIO_BAUD_RATE, SERIAL_8N1, RADIO_RX_PIN, RADIO_TX_PIN); 
       
        // TODO: remaining config
        return true;
    }

    void process() {
        // TODO: process packets 
    }
}