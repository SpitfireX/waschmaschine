#include "solid.h"

LEDSolid::LEDSolid(CRGB color) {
    this->color = color;
    this->changed = true;
}

void LEDSolid::update() {
    this->changed = true;
}

void LEDSolid::show(CRGB leds[], size_t lednum) {
    if (this->changed) {
        for (size_t i = 0; i < lednum; i++) {
            leds[i] = this->color;
        }
        this->changed = false;
    }
}
