// Integrates the WS2812 LEDs
#ifndef LEDS_H
#define LEDS_H

namespace leds {
    bool begin(); // Initializes the leds, returns true if successful
    void lightShow(); // Conducts a small light show
}

#endif