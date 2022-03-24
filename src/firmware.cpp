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

u8g2_uint_t offset;      // current offset for the scrolling text
u8g2_uint_t width;      // pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined
const char *text = "riesenpenis ";  // scroll this text from right to left

EncoderButton eb1(5, 4, 3);

int delayval = 50;
int encoder_pos = 0;
bool power = false;
bool event = true;

void onEb1Encoder(EncoderButton& eb) {
  encoder_pos = (encoder_pos + eb.increment()) % NUMMIRROR;
  if (encoder_pos < 0)
    encoder_pos = NUMMIRROR-1;
  event = true;
}

void onEb1Click(EncoderButton& eb) {
  power = !power;
  event = true;
}

void draw() {
//  u8g2.setFont(u8g2_font_6x10_tf);
//  u8g2.setFontRefHeightExtendedText();
//  u8g2.setDrawColor(1);
//  u8g2.setFontPosTop();
//  u8g2.setFontDirection(0);
//
//  if (power) {
//    u8g2.drawStr( 0, 0, "Power ON");
//  } else {
//    u8g2.drawStr( 0, 0, "Power OFF");
//  }

  u8g2.drawFrame(64, 0, 127, 64);
  
//  u8g2.setCursor(20, 20);
//  u8g2.print(encoder_pos);
}

void setup() {
  // Setup Serial Monitor
  Serial.begin(9600);
  Serial.println("Hello World!");

  u8g2.begin();  
  u8g2.setFont(u8g2_font_inb30_mr);  // set the target font to calculate the pixel width
  width = u8g2.getUTF8Width(text);    // calculate the pixel width of the text
  u8g2.setFontMode(0);    // enable transparent mode, which is faster
  
  FastLED.addLeds<NEOPIXEL, LED_DATA>(leds, NUM_LEDS);
  leds[78] = CRGB::Green;
  FastLED.show();
  
  eb1.setEncoderHandler(onEb1Encoder);
  eb1.setClickHandler(onEb1Click);

  for (int i = 0; i < NUMMIRROR; i++) {
    
  }

//  for (int i = NUMMIRROR-1; i < NUMMIRROR+NUMINNER; i++) {
//    int color[3];
//    hueToRGB((i-NUMMIRROR)*9, color);
//    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2])); // Moderately bright green color.
//    pixels.show(); // This sends the updated pixel color to the hardware.
//    delay(delayval); // Delay for a period of time (in milliseconds).
//  }
}

void loop() {
  eb1.update();

  if (event) {
    Serial.println("redraw");
    
//    memset(leds, 0, sizeof(leds));
//    if (power) {
//      leds[encoder_pos] = CRGB::White;
//    }
//    FastLED.show();

    event = false;
  }

    u8g2_uint_t x;
  
  u8g2.firstPage();
  do {

    // draw the scrolling text at current offset
    x = offset;
    u8g2.setFont(u8g2_font_inb30_mr);    // set the target font
    do {                // repeated drawing of the scrolling text...
      u8g2.drawUTF8(x, 30, text);     // draw the scolling text
      x += width;           // add the pixel width of the scrolling text
    } while( x < u8g2.getDisplayWidth() );    // draw again until the complete display is filled
    
    u8g2.setFont(u8g2_font_inb16_mr);   // draw the current pixel width
    u8g2.setCursor(0, 58);
    u8g2.print(width);          // this value must be lesser than 128 unless U8G2_16BIT is set
    
  } while ( u8g2.nextPage() );
  
  offset-=1;              // scroll by one pixel
  if ( (u8g2_uint_t)offset < (u8g2_uint_t)-width )  
    offset = 0;             // start over again
}

void hueToRGB(int hue, int colors[3]) {
  // hue: 0-359, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;
  const int val = 255;
  const int sat = 255;

  base = ((255 - sat) * val) >> 8;
  switch(hue / 60) 
    {
    case 0:
      r = val;
      g = (((val - base) * hue) / 60) + base;
      b = base;
      break;
    case 1:
      r = (((val - base) * (60 - (hue % 60))) / 60) + base;
      g = val;
      b = base;
      break;
    case 2:
      r = base;
      g = val;
      b = (((val - base) * (hue % 60)) / 60) + base;
      break;
    case 3:
      r = base;
      g = (((val - base) * (60 - (hue % 60))) / 60) + base;
      b = val;
      break;
    case 4:
      r = (((val - base) * (hue % 60)) / 60) + base;
      g = base;
      b = val;
      break;
    case 5:
      r = val;
      g = base;
      b = (((val - base) * (60 - (hue % 60))) / 60) + base;
      break;
    }
    
    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
 }
