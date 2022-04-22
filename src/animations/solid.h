#pragma once

#include "led_animation.h"

class LEDSolid: public LEDAnimation {
    public:
        static const size_t num_settings = 2;
        SettingValue settings[num_settings];

        LEDSolid();
        LEDSolid(u8 hue, u8 sat);
        void show(CRGB leds[], size_t lednum);
        void update();
        size_t getNumSettings() { return this->num_settings; }
        SettingValue* getSettings() { return this->settings; }

    private:
        CRGB color;
        bool changed;
};
