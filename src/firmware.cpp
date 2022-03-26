#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "FastLED.h"

#include <EncoderButton.h>

#define LED_DATA 12
#define NUM_LEDS 79

#define NUMMIRROR 29 // spiegel
#define NUMINNER 26 // innen
#define NUMRIM 24 // bullauge

CRGB leds[NUM_LEDS];

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

EncoderButton eb1(5, 4, 3);

int delayval = 50;
int encoder_pos = 0;
int encoder_delta = 0;
bool power = false;
bool redraw = true;
bool clicked = false;
bool interaction = false;

class LEDAnimation {
    virtual void updateLEDs() = 0;
};

struct LEDPreset {
    const char* title;
    LEDAnimation* animation;
};

LEDPreset presets[] = {
    LEDPreset { "Buntwäsche", nullptr },
    LEDPreset { "Einfarbig", nullptr },
    LEDPreset { "Schnellprogramm", nullptr },
    LEDPreset { "Auswaschen", nullptr },
    LEDPreset { "Schleudergang", nullptr },
    LEDPreset { "Nachtlicht", nullptr },
    LEDPreset { "Farbverlauf", nullptr },
};

size_t selected_preset;
const size_t num_presets = sizeof presets / sizeof presets[0];

void on_encoder(EncoderButton& eb) {
    encoder_pos = eb.position();
    encoder_delta = eb.increment();
    redraw = true;
    interaction = true;
}

void on_click(EncoderButton& eb) {
    if (!power) {
        power = true;
    } else {
        clicked = true;
    }
    redraw = true;
    interaction = true;
}

void on_long_press(EncoderButton& eb) {
    if (power)
        power = false;
    redraw = true;
    interaction = true;
}

void draw() {
    auto p = presets[selected_preset];

    u8g2.setFont(u8g2_font_logisoso16_tf);    // set the target font
    u8g2.setFontMode(0);    // enable transparent mode, which is faster
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);

    u8g2.drawFrame(0, 0, 128, 48);

    u8g2.drawUTF8(2, 2, p.title);

    auto const ypos = 53;
    auto const xstart = 1;

    for (size_t i = 0; i < num_presets; i++) {
        
        if (i == selected_preset) {
            u8g2.drawBox(xstart + (i*7), ypos, 7, 7);
        } else {
            u8g2.drawBox(xstart + (i*7) + 2, ypos+2, 3, 3);
        }
    }

    // u8g2.setCursor(0, 20);
    // u8g2.print(encoder_pos);
}

void setup() {
    selected_preset = 0;

    // Setup Serial Monitor
    Serial.begin(9600);
    Serial.println("Hello World!");

    u8g2.begin();

    FastLED.addLeds<NEOPIXEL, LED_DATA>(leds, NUM_LEDS);
    leds[78] = CRGB::Green;
    FastLED.show();
    
    eb1.setEncoderHandler(on_encoder);
    eb1.setClickHandler(on_click);
    eb1.setLongPressHandler(on_long_press);

//  for (int i = NUMMIRROR-1; i < NUMMIRROR+NUMINNER; i++) {
//    int color[3];
//    hueToRGB((i-NUMMIRROR)*9, color);
//    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2])); // Moderately bright green color.
//    pixels.show(); // This sends the updated pixel color to the hardware.
//    delay(delayval); // Delay for a period of time (in milliseconds).
//  }
}

void set_leds() {
    memset(leds, 0, sizeof(leds));
    if (power) {
        leds[encoder_pos % 29] = CRGB::White;
    }
    FastLED.show();
}

void loop() {
    if (redraw) {
        auto new_preset = ((int) selected_preset + encoder_delta) % (int) num_presets;
        selected_preset = (new_preset >= 0) ? new_preset : num_presets-1; 
        Serial.println(new_preset);

        u8g2.firstPage();
        do {
            eb1.update();
            draw();
        } while ( u8g2.nextPage() );

        set_leds();

        redraw = false;
    } else {
        eb1.update();
    }
}
