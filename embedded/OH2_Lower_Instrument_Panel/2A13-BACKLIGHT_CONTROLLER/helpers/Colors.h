#ifndef __COLORS_H
#define __COLORS_H

#include "FastLED.h"
#include <avr/pgmspace.h>

// Color definitions for indicators and backlights
#define COLOR_YELLOW  CRGB(128, 128, 0)      // Yellow Indicators
#define COLOR_RED     CRGB(128, 0, 0)        // Red Indicators
#define COLOR_GREEN   CRGB(0, 128, 0)        // Green Indicators
#define COLOR_WHITE   CRGB(40, 40, 30)       // Dimmed white, e.g. for Jett Station Select toggle light
#define COLOR_BLACK   CRGB(0, 0, 0)          // No colour
#define COLOR_YELLOW2 CRGB(32, 32, 0)        // Yellow Indicators, lower brightness
#define COLOR_RED2    CRGB(32, 0, 0)         // Red Indicators, lower brightness
#define COLOR_GREEN2  CRGB(0, 32, 0)         // Green Indicators, lower brightness

#endif