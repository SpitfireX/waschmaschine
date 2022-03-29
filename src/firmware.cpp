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

int encoder_pos = 0;
int encoder_delta = 0;
bool power = false;
bool power_changed = true;
bool redraw = true;
bool clicked = false;
bool interaction = false;

class LEDAnimation {
    public:
        virtual void updateLEDs(CRGB leds[], size_t lednum) = 0;
        virtual void activate();
};

class LEDSolid: public LEDAnimation {
    public:
        LEDSolid(CRGB color) {
            this->color = color;
            this->changed = true;
        }

        void activate() {
            this->changed = true;
        }

        void updateLEDs(CRGB leds[], size_t lednum) {
            if (this->changed) {
                for (size_t i = 0; i < lednum; i++) {
                    leds[i] = this->color;
                }
                this->changed = false;
            }
        }
    private:
        CRGB color;
        bool changed;
};

class LEDTemp: public LEDAnimation {
    public:
        LEDTemp(u8 temperature) {
            this->temp = temperature;
            this->changed = true;
        }

        void activate() {
            this->changed = true;
        }

        void updateLEDs(CRGB leds[], size_t lednum) {
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

    private:
        u8 temp;
        bool changed;
};

class LEDDots: public LEDAnimation {
    public:
        LEDDots(CRGB color, u8 spacing) {
            this->color = color;
            this->spacing = spacing;
            this->last = millis();
        }

        void activate() {}

        void updateLEDs(CRGB leds[], size_t lednum) {
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
    
    private:
        CRGB color;
        u8 spacing;
        u32 last;
} ;

class LEDPride: public LEDAnimation {
    public:
        LEDPride() {

        }

        void activate() {

        }

        void updateLEDs(CRGB leds[], size_t lednum) {
            static uint16_t sPseudotime = 0;
            static uint16_t sLastMillis = 0;
            static uint16_t sHue16 = 0;
            
            uint8_t sat8 = beatsin88( 87, 220, 250);
            uint8_t brightdepth = beatsin88( 341, 96, 224);
            uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
            uint8_t msmultiplier = beatsin88(147, 23, 60);

            uint16_t hue16 = sHue16;//gHue * 256;
            uint16_t hueinc16 = beatsin88(113, 1, 3000);
            
            uint16_t ms = millis();
            uint16_t deltams = ms - sLastMillis ;
            sLastMillis  = ms;
            sPseudotime += deltams * msmultiplier;
            sHue16 += deltams * beatsin88( 400, 5,9);
            uint16_t brightnesstheta16 = sPseudotime;
            
            for( uint16_t i = 0 ; i < lednum; i++) {
                hue16 += hueinc16;
                uint8_t hue8 = hue16 / 256;

                brightnesstheta16  += brightnessthetainc16;
                uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

                uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
                uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
                bri8 += (255 - brightdepth);
                
                CRGB newcolor = CHSV( hue8, sat8, bri8);
                
                uint16_t pixelnumber = i;
                pixelnumber = (NUM_LEDS-1) - pixelnumber;
                
                nblend( leds[pixelnumber], newcolor, 64);
            }
        }
};

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

struct MenuEntry {
    const char* title;
    LEDAnimation* animation;
};

MenuEntry main_menu[] = {
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
};

size_t selected_preset;
const size_t num_presets = sizeof main_menu / sizeof main_menu[0];

void on_encoder(EncoderButton& eb) {
    encoder_pos = eb.position();
    encoder_delta = eb.increment();
    redraw = true;
    interaction = true;
    analogWrite(9, 127);
}

void on_click(EncoderButton& eb) {
    if (!power) {
        power = true;
        power_changed = true;
    } else {
        clicked = true;
    }
    redraw = true;
    interaction = true;
    analogWrite(9, 127);
}

void on_long_press(EncoderButton& eb) {
    if (power) {
        power = false;
        power_changed = true;
    }
    redraw = true;
    interaction = true;
    analogWrite(9, 127);
}

void draw() {
    auto p = main_menu[selected_preset];

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

void clear_leds() {
    memset(leds, 0, sizeof(leds));
}

void clear_screen() {
    u8g2.clearDisplay();
}

void setup() {
    selected_preset = 0;

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
    eb1.setLongPressHandler(on_long_press);
}

void loop() {
    eb1.update();

    auto new_preset = ((int) selected_preset + encoder_delta) % (int) num_presets;
    new_preset = (new_preset >= 0) ? new_preset : num_presets-1;
    if (new_preset != selected_preset || power_changed)
        main_menu[selected_preset].animation->activate();
    selected_preset = new_preset;
    encoder_delta = 0;

    if (power) {
        main_menu[selected_preset].animation->updateLEDs(leds, NUM_LEDS);
        FastLED.show();

        if (interaction) {
            analogWrite(9, 0);
        }

        if (redraw) {
            u8g2.firstPage();
            do {
                // eb1.update();
                draw();
            } while ( u8g2.nextPage() );

            redraw = false;
        }
    } else {
        clear_leds();
        FastLED.show();

        if (interaction) {
            analogWrite(9, 0);
        }

        if (redraw) {
            u8g2.firstPage();
            do {
                // eb1.update();
            } while ( u8g2.nextPage() );

            redraw = false;
        }
    }

    // reset all the flags
    power_changed = false;
    redraw = false;
    clicked = false;
    interaction = false;
}
