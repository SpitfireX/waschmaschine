#pragma once

#include "led_animation.h"

class LEDPride: public LEDAnimation {
    public:
        LEDPride() {}
        void update() {}
        void show(CRGB leds[], size_t lednum);
};
