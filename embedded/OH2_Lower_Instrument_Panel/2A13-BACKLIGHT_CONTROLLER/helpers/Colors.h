/**********************************************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *  
 * @file      Colors.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Defines the color constants for the LED strips.
 * @details   The color constants are used to set the color of the LED strips.
 *            The color constants are defined according to MIL-STD-3099.
 *            Adapt to your visual preferences.
 *            Note that methods that are dimming these LEDs are using FastLED's nscale8_video() function.
 *            This function provides a more color-preserving dimming effect than pure RGB value recalculation.
 *********************************************************************************************************************/

#ifndef COLORS_H
#define COLORS_H

#include "FastLED.h"

// Color definitions according MIL-STD-3099. Adapt to your visual preferences.
#define NVIS_YELLOW  CRGB(172, 144, 0)                                // Yellow Indicators
#define NVIS_RED     CRGB(158, 4, 4)                                  // Red Indicators
#define NVIS_GREEN_A CRGB(25, 155, 0)                                 // Green Backlighting (Mil-Spec: 51, 102, 0)
#define NVIS_GREEN_B CRGB(25, 155, 0)                                 // Green indicators (Mil-Spec: 85, 138, 0)
#define NVIS_WHITE   CRGB(40, 40, 30)                                 // Dimmed white, e.g. for Jett Station Select toggle light
#define NVIS_BLACK   CRGB(0, 0, 0)                                    // No colour / OFF
#define NVIS_CGRB_GREEN_A CRGB(155, 25, 0)                           // For GRB LEDs (e.g. Radar Altimeter and Standby Instruments)

#endif // COLORS_H 