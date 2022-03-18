#include "big_digit.h"
#include <LiquidCrystal.h>

extern LiquidCrystal lcd;

const uint8_t zero[] =  { TL, HH, HH, TR,
                          VV, SS, SS, VV,
                          BL, HH, HH, BR };
const uint8_t one[] =   { SS, HH, TR, SS,
                          SS, SS, VV, SS,
                          SS, HH, HH, HH };
const uint8_t two[] =   { HH, HH, HH, TR,
                          TL, HH, HH, BR,
                          HH, HH, HH, HH };
const uint8_t three[] = { HH, HH, HH, TR,
                          SS, HH, HH, VV,
                          HH, HH, HH, BR };
const uint8_t four[] =  { VV, SS, SS, VV,
                          BL, HH, HH, VV,
                          SS, SS, SS, HH };
const uint8_t five[] =  { VV, HH, HH, HH,
                          BL, HH, HH, TR,
                          HH, HH, HH, BR };
const uint8_t six[] =   { TL, HH, HH, HH,
                          VV, HH, HH, TR,
                          BL, HH, HH, BR };
const uint8_t seven[] = { HH, HH, HH, TR,
                          SS, SS, SS, VV,
                          SS, SS, SS, HH };
const uint8_t eight[] = { TL, HH, HH, TR,
                          VV, HH, HH, VV,
                          BL, HH, HH, BR };
const uint8_t nine[] =  { TL, HH, HH, TR,
                          BL, HH, HH, VV,
                          HH, HH, HH, BR };

const uint8_t *digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};

uint8_t VV_data[8] = {
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

uint8_t HH_data[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

uint8_t TL_data[8] = {
  0b00011,
  0b01111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

uint8_t TR_data[8] = {
  0b11000,
  0b11110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

uint8_t BL_data[8] = {
  0b11111,
  0b01111,
  0b00011,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

uint8_t BR_data[8] = {
  0b11111,
  0b11110,
  0b11000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

uint8_t oil_data[8] = {
  0b00000,
  0b00100,
  0b01110,
  0b10111,
  0b11111,
  0b11111,
  0b01110,
  0b00000
};



void big_digit_init(void) {
  lcd.createChar(HH, HH_data);
  lcd.createChar(TL, TL_data);
  lcd.createChar(TR, TR_data);
  lcd.createChar(BL, BL_data);
  lcd.createChar(BR, BR_data);

/*  lcd.createChar(A, a_data);*/
  lcd.createChar(OIL, oil_data);
}

void big_digit(uint8_t sx, uint8_t sy, uint8_t pos, uint8_t val) {
  uint8_t x = 0, y = 0;
  sx = sx + pos * (DIGIT_WIDTH);

  while (y < 3) {
    x = 0;
    while (x < DIGIT_WIDTH) {
      lcd.setCursor(sx + x, sy + y);
      lcd.write((uint8_t) digits[val][y * DIGIT_WIDTH + x]);
      ++x;
    }
    ++y;

  }
}

void big_space(uint8_t sx, uint8_t sy, uint8_t pos) {
  uint8_t x = 0, y = 0;
  sx = sx + pos * DIGIT_WIDTH;

  while (y < 3) {
    x = 0;
    lcd.setCursor(sx + x, sy + y);
    lcd.print(DIGIT_BLANK_LINE);
    ++y;
  }
}
