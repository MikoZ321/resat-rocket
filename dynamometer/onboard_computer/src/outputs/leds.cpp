#include "leds.h"

#include <Arduino.h>
#include <FastLED.h>

#include "config.h"

CRGB s_leds[LED_COUNT];

namespace leds {
    // TODO: fix
    bool begin() {
        FastLED.addLeds<WS2812, LED_PIN, RGB>(s_leds, LED_COUNT);
        FastLED.setBrightness(50);
    }

    void lightShow() {
        s_leds[0] = CRGB::Red;
        s_leds[1] = CRGB::Blue;
        FastLED.show();
        delay(100);

        s_leds[0] = CRGB::Blue;
        s_leds[1] = CRGB::Green;
        FastLED.show();
        delay(100);
        
        s_leds[0] = CRGB::Green;
        s_leds[1] = CRGB::Red;
        FastLED.show();
        delay(100);
    }
}