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

 * @file Panel.h
 * @author Ulukaii
 * @date 05.05.2025
 * @version t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Abstract base class for all panels.
 * @details Each panel is a derived class from this base class.
 *********************************************************************************************************************/


#ifndef __PANEL_H
#define __PANEL_H

#include <Arduino.h>
#include <avr/pgmspace.h> 
#include "DcsBios.h"
#include "FastLED.h"
#include "Colors.h"
#include "LedRole.h"
#include "LedStruct.h"

class Panel {
public:
    // Pure virtual methods that inheriting classes must implement
    virtual int getStartIndex() const { return panelStartIndex; }
    virtual int getLedCount() const { return ledCount; }
    virtual const Led* getLedTable() const { return ledTable; }
    virtual CRGB* getLedStrip() const { return ledStrip; }

    // Static flag to track if any LEDs need updating across all panels. Shared across all panel instances
    static bool ledsNeedUpdate;

    // Static method to update LEDs if any changes are pending
    static void updateLeds() {
        if (ledsNeedUpdate) {
            FastLED.show();
            ledsNeedUpdate = false;
        }
    }

protected:
    // Protected constructor to prevent direct instantiation
    Panel() {
        last_brightness = 128;
        last_flood_brightness = 128;
    }

    // Protected member variables that inheriting classes must set
    int panelStartIndex;             // Start index of the panel on the LED strip
    int ledCount;                    // Number of LEDs in the panel
    const Led* ledTable;             // Pointer to the LED table
    CRGB* ledStrip;                  // Pointer to the LED strip
    uint8_t last_brightness;         // Last brightness value
    uint8_t last_flood_brightness;   // Last brightness value for floodlights

    // Get color for brightness level using lookup table
    static CRGB getColorForBrightness(uint8_t brightness) {
        CRGB color;
        memcpy_P(&color, &BRIGHTNESS_TABLE[brightness], sizeof(CRGB));
        return color;
    }

    // Protected methods that derived classes can use
    void setBacklights(int newValue) {
        // Safety checks
        if (!getLedStrip() || !getLedTable()) return;

        // Determine the brightness value
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);

        // Add value-based debouncing
        if (brightness == last_brightness) return;
        last_brightness = brightness;

        // Get the color for this brightness level
        CRGB color = getColorForBrightness(brightness);

        // Read LED info from PROGMEM for each LED, check LED role is BACKLIGHT and set color
        for (int i = 0; i < getLedCount(); i++) {
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_BACKLIGHT) {
                getLedStrip()[ledIndex] = color;
            }
        }
        // Mark that LEDs need updating. Do not call FastLED.show() directly just yet.
        ledsNeedUpdate = true;
    }

    void setIndicatorColor(LedRole role, const CRGB& color) {
        // Safety checks
        if (!getLedStrip() || !getLedTable()) return;

        // Read LED info from PROGMEM for each LED, check LED role is specified role and set color
        for (int i = 0; i < getLedCount(); i++) {
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == role) {
                getLedStrip()[ledIndex] = color;
            }
        }
        // Mark that LEDs need updating. Do not call FastLED.show() directly just yet.
        ledsNeedUpdate = true;
    }

    void setFloodlights(int newValue) {
        // Map the DCS-BIOS value (0-65535) to brightness (0-255)
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);
        
        // Only update if brightness has changed
        if (brightness != last_flood_brightness) {
            last_flood_brightness = brightness;
            
            // Create white color with the given brightness
            CRGB color = CRGB(brightness, brightness, brightness);
            
            // Update all floodlight LEDs
            for (int i = 0; i < ledCount; i++) {
                Led led;
                memcpy_P(&led, &ledTable[i], sizeof(Led));
                if (led.role == LED_FLOOD) {
                    ledStrip[panelStartIndex + led.index] = color;
                }
            }
            
            ledsNeedUpdate = true;
        }
    }
};

// Define the static member variable
bool Panel::ledsNeedUpdate;

#endif 