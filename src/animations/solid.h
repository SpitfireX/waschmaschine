#pragma once

#include "led_animation.h"

class LEDSolid: public LEDAnimation {
    public:
        LEDSolid(CRGB color);
        void updateLEDs(CRGB leds[], size_t lednum);
        void activate();

    private:
        CRGB color;
        bool changed;
};
