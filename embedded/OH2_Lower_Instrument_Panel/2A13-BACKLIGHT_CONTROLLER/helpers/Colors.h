#ifndef __COLORS_H
#define __COLORS_H

#include "FastLED.h"
#include <avr/pgmspace.h>

// Color definitions for indicators and backlights
#define COLOR_YELLOW  CRGB(172, 144, 0)      // Yellow Indicators
#define COLOR_RED     CRGB(158, 4, 4)        // Red Indicators
#define COLOR_GREEN_A CRGB(51, 102, 0)       // Green Backlighting
#define COLOR_GREEN_B CRGB(85, 138, 0)       // Green indicators
#define COLOR_WHITE   CRGB(40, 40, 30)       // Dimmed white, e.g. for Jett Station Select toggle light
#define COLOR_BLACK   CRGB(0, 0, 0)          // No colour

#endif