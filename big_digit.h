#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DIGIT_WIDTH         4
#define DIGIT_BLANK_LINE    "    "

// Character cell codes.
#define HH      0
#define TR      1
#define TL      2
#define BR      3
#define BL      4
#define OIL     5
#define VV      0xff
#define SS      0x20


void big_digit_init(void);
void big_digit(uint8_t sx, uint8_t sy, uint8_t pos, uint8_t val);
void big_space(uint8_t sx, uint8_t sy, uint8_t pos);
