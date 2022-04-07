#pragma once

#include "led_animation.h"

class LEDDots: public LEDAnimation {
    public:
        LEDDots(CRGB color, u8 spacing);
        void activate() {}
        void updateLEDs(CRGB leds[], size_t lednum);
    
    private:
        CRGB color;
        u8 spacing;
        u32 last;
} ;
