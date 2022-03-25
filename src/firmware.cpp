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

void onEncoder(EncoderButton& eb) {
    encoder_pos = (encoder_pos + eb.increment()) % NUMMIRROR;
    if (encoder_pos < 0)
        encoder_pos = NUMMIRROR-1;
    encoder_delta += eb.increment();
    redraw = true;
}

void onClick(EncoderButton& eb) {
    if (!power) {
        power = true;
    } else {
        clicked = true;
    }
    redraw = true;
}

void onLongPress(EncoderButton& eb) {
    if (power)
        power = false;
    redraw = true;
}

void draw() {
    u8g2.setFont(u8g2_font_inb16_mr);    // set the target font
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);

    if (power) {
        u8g2.drawStr( 0, 0, "Power ON");
    } else {
        u8g2.drawStr( 0, 0, "Power OFF");
    }

    u8g2.drawFrame(64, 0, 127, 64);
    
    u8g2.setCursor(0, 20);
    u8g2.print(encoder_delta);
}

void setup() {
    // Setup Serial Monitor
    Serial.begin(9600);
    Serial.println("Hello World!");

    u8g2.begin();  
    u8g2.setFont(u8g2_font_inb30_mr);  // set the target font to calculate the pixel width
    u8g2.setFontMode(0);    // enable transparent mode, which is faster
    
    FastLED.addLeds<NEOPIXEL, LED_DATA>(leds, NUM_LEDS);
    leds[78] = CRGB::Green;
    FastLED.show();
    
    eb1.setEncoderHandler(onEncoder);
    eb1.setClickHandler(onClick);
    eb1.setLongPressHandler(onLongPress);
    eb1.setDebounceInterval(50);

//  for (int i = NUMMIRROR-1; i < NUMMIRROR+NUMINNER; i++) {
//    int color[3];
//    hueToRGB((i-NUMMIRROR)*9, color);
//    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2])); // Moderately bright green color.
//    pixels.show(); // This sends the updated pixel color to the hardware.
//    delay(delayval); // Delay for a period of time (in milliseconds).
//  }
}

void critical_gui() {
    eb1.update();
    memset(leds, 0, sizeof(leds));
    if (power) {
        leds[encoder_pos] = CRGB::White;
    }
    FastLED.show();
}

void loop() {
    // if (redraw) {
        Serial.println("redraw");

        u8g2.firstPage();
        do {
            critical_gui();
            draw();
        } while ( u8g2.nextPage() );

        redraw = false;
    // } else {
    //     eb1.update();
    // }
}
