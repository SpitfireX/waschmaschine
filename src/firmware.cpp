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

EncoderButton eb1(3, 2, 4);

int encoder_pos = 0;
int encoder_delta = 0;
bool event = false;

LEDSolid solid = LEDSolid(69, 255);

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
    MenuEntry { "Einfarbig", &solid },
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
    PRESET_CONFIG,
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
size_t list_cursor = 0;
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

    for (size_t i = 0; i < num_presets; i++) {
        if (presets[i].animation->getNumSettings() > 0) {
            Serial.print("Preset ");
            Serial.print(i);
            Serial.print(" has ");
            Serial.print(presets[i].animation->getNumSettings());
            Serial.println(" settings:");

            for (size_t j = 0; j < presets[i].animation->getNumSettings(); j++) {
                auto setting = presets[i].animation->getSettings()[j];
                Serial.print("\t");
                Serial.print(setting.name);
                Serial.print(", ");
                Serial.print(setting.data_type);
                Serial.print(", ");
                Serial.print(setting.stepsize);
                Serial.print(", ");
                Serial.println(setting.value);
            }
        } else {
            Serial.print("Preset ");
            Serial.print(i);
            Serial.println(" has no settings.");
        }
    }
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

void draw_preset_config() {
    auto n = presets[selected_preset].animation->getNumSettings()+1;
    auto pages = (n % 3 == 0) ? n/3 : (n/3)+1;

    u8g2.setFont(u8g2_font_logisoso16_tf);    // set the target font
    u8g2.setFontMode(0);    // enable transparent mode, which is faster
    u8g2.setFontRefHeightExtendedText();
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);

    size_t frame_w;
    auto current_page = list_cursor/3;

    if (pages > 1) {
        frame_w = 122;
    } else {
        frame_w = 128;
    }

    Serial.println(list_cursor);
    Serial.println(pages);
    Serial.println(current_page);

    for (size_t i = current_page*3; i < (current_page*3)+3; i++) {
        auto lli = i%3; // local on screen list index, i.e. 0-2
        
        if (i == list_cursor) {
            u8g2.setDrawColor(1);
        } else {
            u8g2.setDrawColor(0);
        }

        u8g2.drawBox(2, 2+(lli*20), frame_w-4, 20);

        if (i == list_cursor) {
            u8g2.setDrawColor(0);
        } else {
            u8g2.setDrawColor(1);
        }

        if (i == 0) {
            u8g2.drawUTF8(2, 3, "Zurück");
        } else {
            if (i < n) {
                auto v = presets[selected_preset].animation->getSettings()[i-1];
                u8g2.drawUTF8(2, 3 + (lli*20), v.name);
            } else {
                break;
            }
        }
    }

    u8g2.setDrawColor(1);
    u8g2.drawFrame(0, 0, frame_w, 64);
    if (pages > 1) {
        auto barheight = 64/pages;

        u8g2.drawVLine(126, 0, 64);
        u8g2.drawBox(125, current_page*barheight, 3, barheight);
    }
}

inline void draw_bar(size_t bar_width) {
    // bar
    u8g2.drawBox(0, 29, bar_width, 7);

    // decorative frames
    u8g2.setDrawColor(0);
    u8g2.drawFrame(1, 28, 125, 9);
    u8g2.setDrawColor(1);
    u8g2.drawFrame(0, 27, 127, 11);

    // position for decorative cursor, cannot < 0 or > 127
    // cursor is 8x17 px
    int cpos = ((int) bar_width) - 4;
    size_t ncpos;
    if (cpos < 0) {
        ncpos = 0;
    } else if (cpos > (127 - 8)) {
        ncpos = (127 - 8);
    } else {
        ncpos = cpos;
    }

    u8g2.setDrawColor(0);
    u8g2.drawRBox(ncpos, 24, 8, 17, 2);
    u8g2.setDrawColor(1);
    u8g2.drawRFrame(ncpos, 24, 8, 17, 2);
}

void draw_value_input() {
    u8g2.setFont(u8g2_font_logisoso16_tf);    // set the target font
    u8g2.setFontMode(0);    // enable transparent mode, which is faster
    u8g2.setFontRefHeightExtendedText();
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.setDrawColor(1);

    auto v = presets[selected_preset].animation->getSettings()[list_cursor-1]; // list_cursor = 0 is virtual "back" entry
    const char* vstr; 

    auto tpos = (128 - u8g2.getStrWidth(v.name))/2;
    u8g2.drawUTF8(tpos, 0, v.name);

    switch (v.data_type) {
        case DataType::U8: {
            vstr = u8x8_utoa(v.value);

            float frac = ((unsigned char) v.value)/255.0F;
            size_t bar_width = 127 * frac;

            draw_bar(bar_width);

            break;
        }
        
        case DataType::U16: {
            vstr = u8x8_utoa(v.value);

            float frac = v.value/65535.0F;
            size_t bar_width = 127 * frac;

            draw_bar(bar_width);

            break;
        }
        
        case DataType::BOOL:
            if (v.value) {
                vstr = "An";

                u8g2.setDrawColor(1);
                u8g2.drawRBox(46, 23, 40, 19, 9);
                u8g2.setDrawColor(0);
                u8g2.drawDisc(68+8, 24+8, 7);
            } else {
                vstr = "Aus";
                
                u8g2.setDrawColor(1);
                u8g2.drawRFrame(46, 23, 40, 19, 9);
                u8g2.drawDisc(48+8, 24+8, 7);
            }
            break;
    }

    u8g2.setDrawColor(1);
    auto vpos = (128 - u8g2.getStrWidth(vstr))/2;
    u8g2.drawStr(vpos, 47, vstr);
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
        presets[selected_preset].animation->show(leds, NUM_LEDS);
    }
    FastLED.show();

    if (event) {
        analogWrite(9, 0); // clear buzzer that clicks for every event

        // handle inputs and redraws
        switch (menu_state) {
            case MenuState::OFF:
                if (last_input_event == InputEvent::CLICK) {
                    menu_state = MenuState::MAIN_MENU;
                    presets[selected_preset].animation->update(); // re-activate animation
                    update_screen(draw_main_menu);
                }
                break;
            
            case MenuState::MAIN_MENU:
                if (last_input_event == InputEvent::INCREMENT) {
                    auto new_preset = ((int) selected_preset + encoder_delta) % (int) num_presets;
                    new_preset = (new_preset >= 0) ? new_preset : num_presets-1;
                    if (new_preset != selected_preset)
                        presets[selected_preset].animation->update();
                    selected_preset = new_preset;
                    encoder_delta = 0;
                    update_screen(draw_main_menu);
                } else if (last_input_event == InputEvent::LONG_PRESS) {
                    menu_state = MenuState::OFF;
                    update_screen(clear_screen);
                } else if (last_input_event == InputEvent::CLICK) {
                    if (presets[selected_preset].animation->getNumSettings() == 1) {
                        menu_state = MenuState::VALUE_INPUT;
                        list_cursor = 1;
                        update_screen(draw_value_input);
                    } else if (presets[selected_preset].animation->getNumSettings() > 1) {
                        menu_state = MenuState::PRESET_CONFIG;
                        list_cursor = 0;
                        update_screen(draw_preset_config);
                    }
                }
                break;

            case MenuState::PRESET_CONFIG:
                if (last_input_event == InputEvent::CLICK) {
                    if (list_cursor == 0) {
                        menu_state = MenuState::MAIN_MENU;
                        update_screen(draw_main_menu);
                    } else {
                        menu_state = MenuState::VALUE_INPUT;
                        update_screen(draw_value_input);
                    }
                } else if (last_input_event == InputEvent::INCREMENT) {
                    int new_cursor = list_cursor + encoder_delta;
                    if (new_cursor < 0) {
                        list_cursor = 0;
                    } else if (new_cursor > presets[selected_preset].animation->getNumSettings()) {
                        list_cursor = presets[selected_preset].animation->getNumSettings();
                    } else {
                        list_cursor = (size_t) new_cursor;
                    }
                    update_screen(draw_preset_config);
                }
                break;
            
            case MenuState::VALUE_INPUT:
                if (last_input_event == InputEvent::CLICK) {
                    if (presets[selected_preset].animation->getNumSettings() == 1) {
                        menu_state = MenuState::MAIN_MENU;
                        update_screen(draw_main_menu);
                    } else {
                        menu_state = MenuState::PRESET_CONFIG;
                        update_screen(draw_preset_config);
                    }
                } else if (last_input_event == InputEvent::INCREMENT) {
                    SettingValue* v = &presets[selected_preset].animation->getSettings()[list_cursor-1]; // list_cursor = 0 is virtual "back" entry

                    switch (v->data_type) {
                        case DataType::U8: {
                            int rdelta = encoder_delta*v->stepsize;
                            int newval = ((unsigned char) v->value) + rdelta;
                            if (newval < 0) {
                                v->value = 0;
                            } else if (newval > 255) {
                                v->value = 255;
                            } else {
                                v->value = (unsigned char) newval;
                            }
                            break;
                        }

                        case DataType::U16: {
                            int rdelta = encoder_delta*v->stepsize;
                            long newval = ((unsigned long) v->value) + rdelta;
                            if (newval < 0) {
                                v->value = 0;
                            } else if (newval > 65535) {
                                v->value = 65535;
                            } else {
                                v->value = (unsigned short) newval;
                            }
                            break;
                        }

                        case DataType::BOOL:
                            if (encoder_delta > 0) {
                                v->value = true;
                            } else {
                                v->value = false;
                            }
                            break;
                    }

                    presets[selected_preset].animation->update();
                    update_screen(draw_value_input);
                }
                break;
        }
    }

    // reset all the flags
    event = false;
    last_input_event = InputEvent::NONE;
}
