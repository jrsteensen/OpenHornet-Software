#ifndef __COLORS_H
#define __COLORS_H

#include "FastLED.h"
#include <avr/pgmspace.h>

// Color definitions for indicators and backlights
#define COLOR_YELLOW  CRGB(128, 128, 0)      // Yellow Indicators
#define COLOR_RED     CRGB(128, 0, 0)        // Red Indicators
#define COLOR_GREEN   CRGB(0, 128, 0)        // Green Indicators
#define COLOR_WHITE   CRGB(40, 40, 30)       // White for Jett Station Select toggle light
#define COLOR_BLACK   CRGB(0, 0, 0)          // No colour
#define COLOR_YELLOW2 CRGB(32, 32, 0)        // Yellow Indicators, lower brightness
#define COLOR_RED2    CRGB(32, 0, 0)         // Red Indicators, lower brightness
#define COLOR_GREEN2  CRGB(0, 32, 0)         // Green Indicators, lower brightness

// Pre-calculated brightness table for green color (0-255) - pre-calculation improves performance on long strips
const CRGB BRIGHTNESS_TABLE[256] PROGMEM = {
    CRGB(0, 0, 0), CRGB(0, 1, 0), CRGB(0, 2, 0), CRGB(0, 3, 0), CRGB(0, 4, 0), CRGB(0, 5, 0), CRGB(0, 6, 0), CRGB(0, 7, 0),
    CRGB(0, 8, 0), CRGB(0, 9, 0), CRGB(0, 10, 0), CRGB(0, 11, 0), CRGB(0, 12, 0), CRGB(0, 13, 0), CRGB(0, 14, 0), CRGB(0, 15, 0),
    CRGB(0, 16, 0), CRGB(0, 17, 0), CRGB(0, 18, 0), CRGB(0, 19, 0), CRGB(0, 20, 0), CRGB(0, 21, 0), CRGB(0, 22, 0), CRGB(0, 23, 0),
    CRGB(0, 24, 0), CRGB(0, 25, 0), CRGB(0, 26, 0), CRGB(0, 27, 0), CRGB(0, 28, 0), CRGB(0, 29, 0), CRGB(0, 30, 0), CRGB(0, 31, 0),
    CRGB(0, 32, 0), CRGB(0, 33, 0), CRGB(0, 34, 0), CRGB(0, 35, 0), CRGB(0, 36, 0), CRGB(0, 37, 0), CRGB(0, 38, 0), CRGB(0, 39, 0),
    CRGB(0, 40, 0), CRGB(0, 41, 0), CRGB(0, 42, 0), CRGB(0, 43, 0), CRGB(0, 44, 0), CRGB(0, 45, 0), CRGB(0, 46, 0), CRGB(0, 47, 0),
    CRGB(0, 48, 0), CRGB(0, 49, 0), CRGB(0, 50, 0), CRGB(0, 51, 0), CRGB(0, 52, 0), CRGB(0, 53, 0), CRGB(0, 54, 0), CRGB(0, 55, 0),
    CRGB(0, 56, 0), CRGB(0, 57, 0), CRGB(0, 58, 0), CRGB(0, 59, 0), CRGB(0, 60, 0), CRGB(0, 61, 0), CRGB(0, 62, 0), CRGB(0, 63, 0),
    CRGB(0, 64, 0), CRGB(0, 65, 0), CRGB(0, 66, 0), CRGB(0, 67, 0), CRGB(0, 68, 0), CRGB(0, 69, 0), CRGB(0, 70, 0), CRGB(0, 71, 0),
    CRGB(0, 72, 0), CRGB(0, 73, 0), CRGB(0, 74, 0), CRGB(0, 75, 0), CRGB(0, 76, 0), CRGB(0, 77, 0), CRGB(0, 78, 0), CRGB(0, 79, 0),
    CRGB(0, 80, 0), CRGB(0, 81, 0), CRGB(0, 82, 0), CRGB(0, 83, 0), CRGB(0, 84, 0), CRGB(0, 85, 0), CRGB(0, 86, 0), CRGB(0, 87, 0),
    CRGB(0, 88, 0), CRGB(0, 89, 0), CRGB(0, 90, 0), CRGB(0, 91, 0), CRGB(0, 92, 0), CRGB(0, 93, 0), CRGB(0, 94, 0), CRGB(0, 95, 0),
    CRGB(0, 96, 0), CRGB(0, 97, 0), CRGB(0, 98, 0), CRGB(0, 99, 0), CRGB(0, 100, 0), CRGB(0, 101, 0), CRGB(0, 102, 0), CRGB(0, 103, 0),
    CRGB(0, 104, 0), CRGB(0, 105, 0), CRGB(0, 106, 0), CRGB(0, 107, 0), CRGB(0, 108, 0), CRGB(0, 109, 0), CRGB(0, 110, 0), CRGB(0, 111, 0),
    CRGB(0, 112, 0), CRGB(0, 113, 0), CRGB(0, 114, 0), CRGB(0, 115, 0), CRGB(0, 116, 0), CRGB(0, 117, 0), CRGB(0, 118, 0), CRGB(0, 119, 0),
    CRGB(0, 120, 0), CRGB(0, 121, 0), CRGB(0, 122, 0), CRGB(0, 123, 0), CRGB(0, 124, 0), CRGB(0, 125, 0), CRGB(0, 126, 0), CRGB(0, 127, 0),
    CRGB(0, 128, 0), CRGB(0, 129, 0), CRGB(0, 130, 0), CRGB(0, 131, 0), CRGB(0, 132, 0), CRGB(0, 133, 0), CRGB(0, 134, 0), CRGB(0, 135, 0),
    CRGB(0, 136, 0), CRGB(0, 137, 0), CRGB(0, 138, 0), CRGB(0, 139, 0), CRGB(0, 140, 0), CRGB(0, 141, 0), CRGB(0, 142, 0), CRGB(0, 143, 0),
    CRGB(0, 144, 0), CRGB(0, 145, 0), CRGB(0, 146, 0), CRGB(0, 147, 0), CRGB(0, 148, 0), CRGB(0, 149, 0), CRGB(0, 150, 0), CRGB(0, 151, 0),
    CRGB(0, 152, 0), CRGB(0, 153, 0), CRGB(0, 154, 0), CRGB(0, 155, 0), CRGB(0, 156, 0), CRGB(0, 157, 0), CRGB(0, 158, 0), CRGB(0, 159, 0),
    CRGB(0, 160, 0), CRGB(0, 161, 0), CRGB(0, 162, 0), CRGB(0, 163, 0), CRGB(0, 164, 0), CRGB(0, 165, 0), CRGB(0, 166, 0), CRGB(0, 167, 0),
    CRGB(0, 168, 0), CRGB(0, 169, 0), CRGB(0, 170, 0), CRGB(0, 171, 0), CRGB(0, 172, 0), CRGB(0, 173, 0), CRGB(0, 174, 0), CRGB(0, 175, 0),
    CRGB(0, 176, 0), CRGB(0, 177, 0), CRGB(0, 178, 0), CRGB(0, 179, 0), CRGB(0, 180, 0), CRGB(0, 181, 0), CRGB(0, 182, 0), CRGB(0, 183, 0),
    CRGB(0, 184, 0), CRGB(0, 185, 0), CRGB(0, 186, 0), CRGB(0, 187, 0), CRGB(0, 188, 0), CRGB(0, 189, 0), CRGB(0, 190, 0), CRGB(0, 191, 0),
    CRGB(0, 192, 0), CRGB(0, 193, 0), CRGB(0, 194, 0), CRGB(0, 195, 0), CRGB(0, 196, 0), CRGB(0, 197, 0), CRGB(0, 198, 0), CRGB(0, 199, 0),
    CRGB(0, 200, 0), CRGB(0, 201, 0), CRGB(0, 202, 0), CRGB(0, 203, 0), CRGB(0, 204, 0), CRGB(0, 205, 0), CRGB(0, 206, 0), CRGB(0, 207, 0),
    CRGB(0, 208, 0), CRGB(0, 209, 0), CRGB(0, 210, 0), CRGB(0, 211, 0), CRGB(0, 212, 0), CRGB(0, 213, 0), CRGB(0, 214, 0), CRGB(0, 215, 0),
    CRGB(0, 216, 0), CRGB(0, 217, 0), CRGB(0, 218, 0), CRGB(0, 219, 0), CRGB(0, 220, 0), CRGB(0, 221, 0), CRGB(0, 222, 0), CRGB(0, 223, 0),
    CRGB(0, 224, 0), CRGB(0, 225, 0), CRGB(0, 226, 0), CRGB(0, 227, 0), CRGB(0, 228, 0), CRGB(0, 229, 0), CRGB(0, 230, 0), CRGB(0, 231, 0),
    CRGB(0, 232, 0), CRGB(0, 233, 0), CRGB(0, 234, 0), CRGB(0, 235, 0), CRGB(0, 236, 0), CRGB(0, 237, 0), CRGB(0, 238, 0), CRGB(0, 239, 0),
    CRGB(0, 240, 0), CRGB(0, 241, 0), CRGB(0, 242, 0), CRGB(0, 243, 0), CRGB(0, 244, 0), CRGB(0, 245, 0), CRGB(0, 246, 0), CRGB(0, 247, 0),
    CRGB(0, 248, 0), CRGB(0, 249, 0), CRGB(0, 250, 0), CRGB(0, 251, 0), CRGB(0, 252, 0), CRGB(0, 253, 0), CRGB(0, 254, 0), CRGB(0, 255, 0)
};

#endif 