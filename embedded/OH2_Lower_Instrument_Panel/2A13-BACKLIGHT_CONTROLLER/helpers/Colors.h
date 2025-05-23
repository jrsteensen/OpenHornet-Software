#ifndef COLORS_H
#define COLORS_H

#include "FastLED.h"

// Color definitions according MIL-STD-3099. Adapt to your visual preferences.
#define NVIS_YELLOW  CRGB(172, 144, 0)                                // Yellow Indicators
#define NVIS_RED     CRGB(158, 4, 4)                                  // Red Indicators
#define NVIS_GREEN_A CRGB(51, 102, 0)                                 // Green Backlighting
#define NVIS_GREEN_B CRGB(85, 138, 0)                                 // Green indicators
#define NVIS_WHITE   CRGB(40, 40, 30)                                 // Dimmed white, e.g. for Jett Station Select toggle light
#define NVIS_BLACK   CRGB(0, 0, 0)                                    // No colour / OFF

#endif // COLORS_H 