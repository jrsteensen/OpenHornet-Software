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
#include "Board.h"

class Panel {
public:
    //Virtual methods. Specific panels must implement them INDIVIDUALLY.
    virtual int getStartIndex() const { return panelStartIndex; }
    virtual int getLedCount() const { return ledCount; }
    virtual const Led* getLedTable() const { return ledTable; }
    virtual CRGB* getLedStrip() const { return ledStrip; }
    
protected:
    // Protected constructor to prevent direct instantiation
    Panel() {
        current_backl_brightness = 64;
        current_flood_brightness = 64;
        current_color = CRGB(0, 0, 0);  // Initialize target color to black
    }

    // Protected variables. Specific panels must set them INDIVIDUALLY.
    int panelStartIndex;                                              // Start index of the panel on the LED strip
    int ledCount;                                                     // Number of LEDs in the panel
    const Led* ledTable;                                              // Pointer to the LED table
    CRGB* ledStrip;                                                   // Pointer to the LED strip
    uint8_t current_backl_brightness;                                 // Current brightness value for backlights
    uint8_t current_flood_brightness;                                 // Current brightness value for floodlights
    CRGB current_color;                                               // Current target color for backlights


    //The following methods are used to set the color of the LEDs
    //They are SHARED across all panels.

    // Set the color of LEDs with role LED_BACKLIGHT
    void setBacklights(uint16_t newValue) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        if (newValue == current_backl_brightness) return;             // Value-based debouncing: exit if no brightness change
        current_backl_brightness = newValue;
        
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);         // Map DCS-BIOS value (0-65535) to FastLED brightness (0-255)
        current_color = CRGB(0, brightness, 0);                       // Set the color to green with the calculated brightness

        
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is BACKLIGHT, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_BACKLIGHT) {
                getLedStrip()[ledIndex] = current_color;
            }
        }
        Board::getInstance()->setLedsNeedUpdate();                    // Inform the board that the LEDs need to be updated
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
        Board::getInstance()->setLedsNeedUpdate();                    // Inform the board that the LEDs need to be updated
    }

    // Set the brightness of LEDs with role LED_FLOOD
    void setFloodlights(uint16_t newValue) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        if (newValue == current_flood_brightness) return;             // Value-based debouncing: exit if no brightness change
        current_flood_brightness = newValue;
        
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);     // Map DCS-BIOS value (0-65535) to FastLED brightness (0-255)
        current_color = CRGB(brightness, brightness, brightness); // Set the color to white with the calculated brightness
        
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is FLOOD, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_FLOOD) {
                getLedStrip()[ledIndex] = current_color;
            }
        }
        Board::getInstance()->setLedsNeedUpdate();                    // Inform the board that the LEDs need to be updated
    }
};

#endif 