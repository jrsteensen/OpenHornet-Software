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
    //Virtual methods. Specific panels must implement them INDIVIDUALLY.
    virtual int getStartIndex() const { return panelStartIndex; }
    virtual int getLedCount() const { return ledCount; }
    virtual const Led* getLedTable() const { return ledTable; }
    virtual CRGB* getLedStrip() const { return ledStrip; }
    
    //Static variables and methods. SHARED across all panels.
    static CRGB targetColor;                                          //Save the current backlight color
    static bool ledsNeedUpdate;                                       //Track if any LEDs need updating
    static int updCountdown;                                         //Countdown before triggiering FastLED.show()

    static void updateLeds() {
        if (ledsNeedUpdate) {
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    //If countdown is 0, this is the first cycle, so set to 8
            updCountdown--;
            if (updCountdown == 0) {                                  //Trigger FastLED.show() at end of countdown
                FastLED.show();                                       //This countdown logic allows to capture multiple
                ledsNeedUpdate = false;                               //DCS-BIOS loop cycles into one FastLED.show() call
            }
        }
    }

protected:
    // Protected constructor to prevent direct instantiation
    Panel() {
        last_brightness = 64;
        last_flood_brightness = 64;
    }

    // Protected variables. Specific panels must set them INDIVIDUALLY.
    int panelStartIndex;                                              // Start index of the panel on the LED strip
    int ledCount;                                                     // Number of LEDs in the panel
    const Led* ledTable;                                              // Pointer to the LED table
    CRGB* ledStrip;                                                   // Pointer to the LED strip
    uint8_t last_brightness;                                          // Last brightness value
    uint8_t last_flood_brightness;                                    // Last brightness value for floodlights


    //The following methods are used to set the color of the LEDs
    //They are SHARED across all panels.

    // Calculate the green color for given brightness and optional offset
    CRGB getBrightness(int brightness, int offset = 0) {
        int adjustedBrightness = brightness + offset;
        if (adjustedBrightness < 0) adjustedBrightness = 0;
        if (adjustedBrightness > 255) adjustedBrightness = 255;
        return CRGB(0, adjustedBrightness, 0);
    }

    // Set the color of LEDs with role LED_BACKLIGHT
    void setBacklights(int newValue) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        int brightness = map(newValue, 0, 65535, 0, 255);             // Determine the brightness value
        if (brightness == last_brightness) return;                    // Add value-based debouncing
        last_brightness = brightness;
        if (!ledsNeedUpdate) targetColor = getBrightness(brightness); // Only calculate the color if we are in the first method call of this update cycle (ledsNeedUpdate is still false)
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read infor from PROGMEM; if LED is BACKLIGHT, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_BACKLIGHT) {
                getLedStrip()[ledIndex] = targetColor;
            }
        }
        ledsNeedUpdate = true;                                        // Set "LED update required" flag
    }

    // Set the color of LEDs with a specific role (parameter "role"))
    void setIndicatorColor(LedRole role, const CRGB& color) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        int n = getLedCount();                                        
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED matches role, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == role) {
                getLedStrip()[ledIndex] = color;
            }
        }
        ledsNeedUpdate = true;                                           // Set "LED update required" flag
    }

    // Set the color of LEDs with role LED_FLOOD
    void setFloodlights(int newValue) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);         // Map DCS-BIOS value to brightness
        if (brightness == last_flood_brightness) return;              // Add value-based debouncing
        last_flood_brightness = brightness;                           // Store new brightness
        CRGB color = CRGB(brightness, brightness, brightness);        // Create white color with the given brightness
        int n = getLedCount();                                        // Get LED count once for clarity
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is FLOOD, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            if (led.role == LED_FLOOD) {
                getLedStrip()[led.index + getStartIndex()] = color;
            }
        }
        ledsNeedUpdate = true;                                           // Set "LED update required" flag
    }
};

// Define the static member variables
bool Panel::ledsNeedUpdate;
CRGB Panel::targetColor = CRGB(0, 0, 0);
int Panel::updCountdown = 0;

#endif 