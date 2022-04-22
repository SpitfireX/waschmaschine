#include "solid.h"

LEDSolid::LEDSolid() {
    this->changed = true;

    this->settings[0] = SettingValue {"Farbton", DataType::U8, 8, 0};
    this->settings[1] = SettingValue {"Sättigung", DataType::U8, 1, 255};
    this->color = CHSV(0, 255, 255);
}

LEDSolid::LEDSolid(u8 hue, u8 sat) {
    this->changed = true;

    this->settings[0] = SettingValue {"Farbton", DataType::U8, 8, hue};
    this->settings[1] = SettingValue {"Sättigung", DataType::U8, 1, sat};
    this->color = CHSV(hue, sat, 255);
}

void LEDSolid::update() {
    this->color = CHSV(this->settings[0].value, this->settings[1].value, 255);
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
