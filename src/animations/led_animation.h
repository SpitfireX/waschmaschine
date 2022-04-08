#pragma once

#include <Arduino.h>
#include "FastLED.h"

#include "data_structures.h"

class LEDAnimation {
    public:
        virtual void show(CRGB leds[], size_t lednum) = 0;
        virtual void update() = 0;
        virtual size_t getNumSettings() { return 0; }
        virtual SettingValue* getSettings() { return NULL; }
};