#include "temp.h"

LEDTemp::LEDTemp(u8 temperature) {
    this->changed = true;
    this->settings[0] = SettingValue {"Temperatur", DataType::U8, 1, temperature};
}

void LEDTemp::activate() {
    this->changed = true;
}

void LEDTemp::updateLEDs(CRGB leds[], size_t lednum) {
    if (this->changed) {
        unsigned char temp = this->settings[this->temp].value;

        float g = 147+(temp*0.424F);
        float b = 41+(temp*0.840F);
        CRGB color = CRGB(255, g, b);

        for (size_t i = 0; i < lednum; i++) {
            leds[i] = color;
        }
        this->changed = false;
    }
}
