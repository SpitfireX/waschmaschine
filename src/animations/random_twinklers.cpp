#include "random_twinklers.h"

LEDRandomTwinklers::LEDRandomTwinklers() {
    for (size_t i = 0; i < NUMTWINKLERS; i++) {
        this->twinklers[i] = {0, 0, -1};
    }
    this->last = millis();
}

void LEDRandomTwinklers::show(CRGB leds[], size_t lednum) {
    if (millis() < this->last + 17) {
        return;
    } else {
        this->last = millis();
    }

    // new twinkler
    if (random8() < 40) {
        size_t i = 0;
        bool add = false;
        for (; i < NUMTWINKLERS; i++) {
            if (this->twinklers[i].age < 0) {
                add = true;
                break;
            }
        }

        if (add) {
            this->twinklers[i] = {
                random8(0, lednum),
                random8(),
                0
            };
        }
    }

    // clear leds
    for (size_t i = 0; i < lednum; i++) {
        leds[i] = CRGB::Black;
    }

    // write out twinklers
    for (size_t i = 0; i < NUMTWINKLERS; i++) {
        auto twinkler = &this->twinklers[i];
        if (twinkler->age >= 0 && static_cast<CRGB>(leds[twinkler->index]) == static_cast<CRGB>(CRGB::Black)) {
            leds[twinkler->index] = CRGB().setHSV(twinkler->hue, 255, quadwave8(twinkler->age));
            if (twinkler->age < 255) {
                twinkler->age++;
            } else {
                twinkler->age = -1;
            }
        }
    }
}
