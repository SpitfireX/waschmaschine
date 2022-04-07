#pragma once

#include "led_animation.h"

#define NUMTWINKLERS 20

struct Twinkler {
    u8 index;
    u8 hue;
    int age;
};


class LEDRandomTwinklers: public LEDAnimation {
    public:
        LEDRandomTwinklers();
        void activate() {}
        void updateLEDs(CRGB leds[], size_t lednum);

    private:
        Twinkler twinklers[NUMTWINKLERS];
        u32 last;
};
