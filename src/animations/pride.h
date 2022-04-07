#pragma once

#include "led_animation.h"

class LEDPride: public LEDAnimation {
    public:
        LEDPride() {}
        void activate() {}
        void updateLEDs(CRGB leds[], size_t lednum);
};
