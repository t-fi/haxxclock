#pragma once

#include <vector>
#include <array>
#include "HD108_driver.h"

static const std::array<uint8_t, 7> segment_order = {0, 1, 2, 3, 4, 5, 6};

// Stolen coz we are ninjas
// https://github.com/dmadison/LED-SEGMENT-ASCII
const uint8_t seven_segment_ascii[96] = {
        0x00, /* (space) */
        0x86, /* ! */
        0x22, /* " */
        0x7E, /* # */
        0x6D, /* $ */
        0xD2, /* % */
        0x46, /* & */
        0x20, /* ' */
        0x29, /* ( */
        0x0B, /* ) */
        0x21, /* * */
        0x70, /* + */
        0x10, /* , */
        0x40, /* - */
        0x80, /* . */
        0x52, /* / */
        0xEE, /* 0 */
        0x22, /* 1 */
        0x7C, /* 2 */
        0x76, /* 3 */
        0xB2, /* 4 */
        0xD6, /* 5 */
        0xDE, /* 6 */
        0x62, /* 7 */
        0xFE, /* 8 */
        0xF2, /* 9 */
        0x09, /* : */
        0x0D, /* ; */
        0x61, /* < */
        0x48, /* = */
        0x43, /* > */
        0xD3, /* ? */
        0x5F, /* @ */
        0xFA, /* A */
        0x9E, /* B */
        0x1C, /* C */
        0x3E, /* D */
        0xDC, /* E */
        0xD8, /* F */
        0x3D, /* G */
        0x76, /* H */
        0x30, /* I */
        0x1E, /* J */
        0x75, /* K */
        0x38, /* L */
        0x15, /* M */
        0x37, /* N */
        0x3F, /* O */
        0x73, /* P */
        0x6B, /* Q */
        0x33, /* R */
        0x6D, /* S */
        0x78, /* T */
        0x3E, /* U */
        0x3E, /* V */
        0x2A, /* W */
        0x76, /* X */
        0x6E, /* Y */
        0x5B, /* Z */
        0x39, /* [ */
        0x64, /* \ */
        0x0F, /* ] */
        0x23, /* ^ */
        0x08, /* _ */
        0x02, /* ` */
        0xFA, /* a */
        0x9E, /* b */
        0x1C, /* c */
        0x3E, /* d */
        0xDC, /* e */
        0xD8, /* f */
        0x6F, /* g */
        0x74, /* h */
        0x10, /* i */
        0x0C, /* j */
        0x75, /* k */
        0x30, /* l */
        0x14, /* m */
        0x54, /* n */
        0x5C, /* o */
        0x73, /* p */
        0x67, /* q */
        0x50, /* r */
        0x6D, /* s */
        0x78, /* t */
        0x1C, /* u */
        0x1C, /* v */
        0x14, /* w */
        0x76, /* x */
        0x6E, /* y */
        0x5B, /* z */
        0x46, /* { */
        0x30, /* | */
        0x70, /* } */
        0x01, /* ~ */
        0x00, /* (del) */
};

void set_color(char* color, double brightness);
std::vector<led_value> get_digit_frame(char digit);
std::vector<led_value> get_clock_frame(struct tm* time, suseconds_t microseconds);
