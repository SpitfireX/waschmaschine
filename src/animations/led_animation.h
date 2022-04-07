#pragma once

#include <Arduino.h>
#include "FastLED.h"

class LEDAnimation {
    public:
        virtual void updateLEDs(CRGB leds[], size_t lednum) = 0;
        virtual void activate();
};