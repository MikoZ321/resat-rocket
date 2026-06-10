// Integrates radio communication through the RFD868
#ifndef RADIO_H
#define RADIO_H

namespace radio {
    bool begin(); // Initializes the radio, returns true if successful
    void process(); // Drains UART buffer, validates frame, forwards to USB and saves
}

#endif
