#include "temp.h"

LEDTemp::LEDTemp(u8 temperature) {
    this->temp = temperature;
    this->changed = true;
}

void LEDTemp::activate() {
    this->changed = true;
}

void LEDTemp::updateLEDs(CRGB leds[], size_t lednum) {
    if (this->changed) {
        float g = 147+(this->temp*0.424F);
        float b = 41+(this->temp*0.840F);
        CRGB color = CRGB(255, g, b);

        for (size_t i = 0; i < lednum; i++) {
            leds[i] = color;
        }
        this->changed = false;
    }
}
