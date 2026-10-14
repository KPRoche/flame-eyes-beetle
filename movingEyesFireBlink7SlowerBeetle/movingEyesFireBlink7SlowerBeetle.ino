#include <Flash.h>
// this version has the eyes 4 pixels apart, and uses a paleviolet dot at the center of the pupil


#include "WS2812_Definitions.h"
// #include <LowPower.h>
#include <EEPROM.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 9

// eye table rewritten to fill from bottom up; this makes it easier to "open" the eye after a blink
const uint32_t eye[18][4] = {
{7, 7, RED, 128},
{8, 7, RED, 128},
{6, 6, RED, 128},
{7, 6, RED, 128},
{8, 6, RED, 128},
{9, 6, RED, 128},
{10, 6, RED, 128},
{5, 5, RED, 128},
{6, 5, RED, 128},
{7, 5, CRIMSON, 255},
{8, 5, CRIMSON, 255},
{9, 5, RED, 128},
{10, 5, RED, 128},
{8, 4, PALEVIOLETRED, 255},
{9, 4, CRIMSON, 255},
{10, 4, RED, 128},
{11, 4, RED, 128},
{11, 4, RED, 128}
};

const unsigned int flicker2[22][2] = {
{14, 1}
, {12, 2}
, {13, 2}
, {12, 3}
, {13, 3}
, {14, 3}
, {12, 4}
, {13, 4}
, {11, 5}
, {5, 4}
, {6, 4}
, {7, 4}
, {5, 3}
, {6, 3}
, {8, 3}
, {9, 3}
, {10, 3}
, {6, 2}
, {9, 2}
, {10, 2}
, {6, 1}
, {10, 1}
};



// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wiRED,128} for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wiRED,128} for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)
};
// declare globals

int pupil = 0;
int w    = matrix.width();
unsigned int blink_ms;
unsigned int elapsed_ms;
unsigned int blink_start;
unsigned int tempo = 50;
int dir = true;

void setMatrixPixel( uint16_t pix_x,
                     uint16_t pix_y,
                     uint32_t color ,
                     uint8_t brightness) {
  uint8_t
  r = (uint8_t)(color >> 16),
  g = (uint8_t)(color >>  8),
  b = (uint8_t)color;

  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;

  matrix.drawPixel(pix_x, pix_y, matrix.Color(r, g, b));
}






//int pass = 0;

void blink_eyes(int startcol = 0, int pace = 10, int closed_times=3) {
  Serial.println(F("blinking"));
  for (int i = 0; i < 9; i++) {
    matrix.fillRoundRect(0, 0, w / 2 - startcol, i, i / 2, 0);
    matrix.fillRoundRect(w / 2 + startcol, 0, w / 2 - startcol, i, i / 2, 0);
    matrix.show();
    delay(pace);
  }
  delay(pace * closed_times);

}

void drawEye(int start_col, bool right = true) {
  int num_rows = 18;
  uint32_t fcolor = 0;
  int fcindex = 0;
  Serial.print(F("number of rows: "));
  Serial.println(num_rows);
  uint16_t pix_x;
  uint16_t pix_y;
  uint16_t color;
  if (right) {
    Serial.println(F("drawing right eye"));
  }
  else {
    Serial.println(F("drawing left eye"));
  }
  for (int r = 0; r < num_rows; r++) {
    if (right) {
      pix_x = start_col + eye[r][0];
    }
    else {
      pix_x = start_col - eye[r][0];
    }
    pix_y = eye[r][1];
    setMatrixPixel(pix_x, pix_y, eye[r][2], eye[r][3]);

  }
  //Now add the flickers
  for (int f = 0; f < 22; f++) {
    fcindex = random(4);
    switch (fcindex) {
      case 0:
        fcolor = BLACK;
        break;
      case 1:
        fcolor = 0xFFCC00;
        break;
      case 2:
        fcolor = ORANGERED;
        break;
      case 3:
        fcolor = WHITE;
        break;
      default:
        fcolor = BLACK;
    }
    if (right) {
      pix_x = start_col +  flicker2[f][0];
    }
    else {
      pix_x = start_col - (flicker2[f][0]);
    }
    pix_y = flicker2[f][1];
    setMatrixPixel(pix_x, pix_y, fcolor, 64);
  }

  matrix.show();
}




void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Reaper Eyes v.Blink6"));
  matrix.begin();
  matrix.setTextWrap(false);
  //matrix.setBrightness(255);
  matrix.setTextColor(colors[0]);
  matrix.fillScreen(0);
  randomSeed(analogRead(0));
  blink_ms = random(3000, 5000);
  blink_start = millis();
  }

void loop() {
  drawEye(w / 2 - 3, true);
  drawEye((w / 2) + 3 - 1, false);
  digitalWrite(13, HIGH);
  //  pupil++;
  //pupil = pupil % 4;
  delay(tempo);
  digitalWrite(13, LOW);
  elapsed_ms = millis() - blink_start;
  if (elapsed_ms >= blink_ms) {
    blink_eyes(1, 12,2);
    blink_ms = random(3000, 8000);
    blink_start = millis();
  }

}
