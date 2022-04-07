#pragma once

#include "led_animation.h"

class LEDTemp: public LEDAnimation {
    public:
        LEDTemp(u8 temperature);
        void activate();
        void updateLEDs(CRGB leds[], size_t lednum);

    private:
        u8 temp;
        bool changed;
};