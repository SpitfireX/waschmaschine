#pragma once

#include "led_animation.h"

class LEDSolid: public LEDAnimation {
    public:
        LEDSolid(CRGB color);
        void show(CRGB leds[], size_t lednum);
        void update();

    private:
        CRGB color;
        bool changed;
};
