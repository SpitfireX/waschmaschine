#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#include "FastLED.h"

#include <EncoderButton.h>

#include "animations.h"

#define LED_DATA 12
#define NUM_LEDS 79

#define NUM_MIRROR 29 // spiegel
#define NUM_INNER 26 // innen
#define NUM_RIM 24 // bullauge

CRGB leds[NUM_LEDS];

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

EncoderButton eb1(2, 3, 4);

int encoder_pos = 0;
int encoder_delta = 0;
bool event = false;

LEDSolid solid_white = LEDSolid(CRGB::White);
LEDSolid solid_orange = LEDSolid(CRGB::Orange);
LEDSolid solid_aqua = LEDSolid(CRGB::Aquamarine);
LEDSolid solid_chartreuse = LEDSolid(CRGB::Chartreuse);

LEDPride pride = LEDPride();

LEDTemp templow = LEDTemp(0);
LEDTemp temphigh = LEDTemp(255);

LEDDots dots2 = LEDDots(CRGB::Fuchsia, 2);
LEDDots dots3 = LEDDots(CRGB::DodgerBlue, 3);
LEDDots dots4 = LEDDots(CRGB::Crimson, 4);

LEDRandomTwinklers twinklers = LEDRandomTwinklers();

struct MenuEntry {
    const char* title;
    LEDAnimation* animation;
};

MenuEntry presets[] = {
    MenuEntry { "Buntwäsche", &pride },
    MenuEntry { "Einfarbig", &solid_white },
    MenuEntry { "Schnellprogramm", &solid_orange },
    MenuEntry { "Auswaschen", &solid_aqua },
    MenuEntry { "Schleudergang", &solid_chartreuse },
    MenuEntry { "Nachtlicht", &templow },
    MenuEntry { "Farbverlauf", &temphigh },
    MenuEntry { "Dots 2", &dots2 },
    MenuEntry { "Dots 3", &dots3 },
    MenuEntry { "Dots 4", &dots4 },
    MenuEntry { "Twinklers", &twinklers },
};

enum MenuState {
    OFF,
    MAIN_MENU,
    ANIM_CONFIG,
    VALUE_INPUT,
};

MenuState menu_state = MenuState::OFF;

enum InputEvent {
    NONE,
    INCREMENT,
    CLICK,
    DOUBLE_CLICK,
    LONG_PRESS,
};

InputEvent last_input_event = InputEvent::NONE;

size_t selected_preset;
const size_t num_presets = sizeof presets / sizeof presets[0];

void on_encoder(EncoderButton& eb) {
    encoder_pos = eb.position();
    encoder_delta = eb.increment();
    last_input_event = InputEvent::INCREMENT;
    event = true;
}

void on_click(EncoderButton& eb) {
    last_input_event = InputEvent::CLICK;
    event = true;
}

void on_double_click(EncoderButton& eb) {
    last_input_event = InputEvent::DOUBLE_CLICK;
    event = true;
}

void on_long_press(EncoderButton& eb) {
    last_input_event = InputEvent::LONG_PRESS;
    event = true;
}

void setup() {
    selected_preset = 0;

    randomSeed(analogRead(0));

    // Setup Serial Monitor
    Serial.begin(9600);
    Serial.println("Hello World!");

    u8g2.begin();

    FastLED.addLeds<NEOPIXEL, LED_DATA>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.setCorrection(CRGB(255, 200, 130));
    leds[78] = CRGB::Green;
    FastLED.show();
    
    eb1.setEncoderHandler(on_encoder);
    eb1.setClickHandler(on_click);
    eb1.setDoubleClickHandler(on_double_click);
    eb1.setLongPressHandler(on_long_press);
}

void draw_main_menu() {
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
    auto const xstart = (128-(7*num_presets))/2;

    for (size_t i = 0; i < num_presets; i++) {
        
        if (i == selected_preset) {
            u8g2.drawBox(xstart + (i*7), ypos, 7, 7);
        } else {
            u8g2.drawBox(xstart + (i*7) + 2, ypos+2, 3, 3);
        }
    }
}

inline void update_screen(void (*draw_fun)()) {
    u8g2.firstPage();
    do {
        draw_fun();
    } while ( u8g2.nextPage() );
}

inline void clear_screen() {}

void clear_leds() {
    memset(leds, 0, sizeof(leds));
}

void loop() {
    // this polls the encoder/button and generates InputEvents
    eb1.update();
    if (event) {
        analogWrite(9, 127);
    }

    // animate or turn off LEDs
    if (menu_state == MenuState::OFF) {
        clear_leds();
    } else {
        presets[selected_preset].animation->updateLEDs(leds, NUM_LEDS);
    }
    FastLED.show();

    if (event) {
        analogWrite(9, 0); // clear buzzer that clicks for every event

        // handle inputs and redraws
        switch (menu_state) {
            case MenuState::OFF:
                if (last_input_event == InputEvent::CLICK) {
                    menu_state = MenuState::MAIN_MENU;
                    presets[selected_preset].animation->activate(); // re-activate animation
                    update_screen(draw_main_menu);
                }
                break;
            
            case MenuState::MAIN_MENU:
                if (last_input_event == InputEvent::INCREMENT) {
                    auto new_preset = ((int) selected_preset + encoder_delta) % (int) num_presets;
                    new_preset = (new_preset >= 0) ? new_preset : num_presets-1;
                    if (new_preset != selected_preset)
                        presets[selected_preset].animation->activate();
                    selected_preset = new_preset;
                    encoder_delta = 0;
                    update_screen(draw_main_menu);
                } else if (last_input_event == InputEvent::LONG_PRESS) {
                    menu_state = MenuState::OFF;
                    update_screen(clear_screen);
                }
                break;

            case MenuState::ANIM_CONFIG:
                break;
            case MenuState::VALUE_INPUT:
                break;
        }
    }

    // reset all the flags
    event = false;
    last_input_event = InputEvent::NONE;
}
