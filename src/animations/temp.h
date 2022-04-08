#pragma once

#include "led_animation.h"

class LEDTemp: public LEDAnimation {
    public:
        static const size_t num_settings = 1;
        SettingValue settings[num_settings];

        LEDTemp(u8 temperature);
        void update();
        void show(CRGB leds[], size_t lednum);
        size_t getNumSettings() { return this->num_settings; }
        SettingValue* getSettings() { return this->settings; }

    private:
        const static size_t temp = 0;
        bool changed;
};