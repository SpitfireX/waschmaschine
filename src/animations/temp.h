#pragma once

#include "led_animation.h"

class LEDTemp: public LEDAnimation {
    public:
        static const size_t num_settings = 1;
        SettingValue settings[num_settings];

        LEDTemp(u8 temperature);
        void activate();
        void updateLEDs(CRGB leds[], size_t lednum);
        size_t getNumSettings() { return num_settings; }
        SettingValue* getSettings() { return settings; }

    private:
        u8 temp;
        bool changed;
};