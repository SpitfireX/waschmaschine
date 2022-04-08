#include "dots.h"

LEDDots::LEDDots(CRGB color, u8 spacing) {
    this->color = color;
    this->spacing = spacing;
    this->last = millis();
}

void LEDDots::show(CRGB leds[], size_t lednum) {
    static u8 counter;

    if (millis() >= this->last + 100) {
        this->last = millis();

        u8 offset = counter % this->spacing;

        for (size_t i = 0; i < lednum; i++) {
            if ((i + offset) % this->spacing == 0) {
                leds[i] = color;
            } else {
                leds[i] = CRGB::Black;
            }
        }

        counter++;
    }
}
