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
#include "LedRole.h"
#include "LedStruct.h"
#include "Board.h"
#include "Colors.h"

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
    }

    // Protected variables. Specific panels must set them INDIVIDUALLY.
    int panelStartIndex;                                              // Start index of the panel on the LED strip
    int ledCount;                                                     // Number of LEDs in the panel
    const Led* ledTable;                                              // Pointer to the LED table
    CRGB* ledStrip;                                                   // Pointer to the LED strip
    uint16_t current_backl_brightness;                                // Current br. value for backlights (0-65535)
    uint16_t current_flood_brightness;                                // Current br. value for floodlights (0-65535)


    //The following methods are used to set LEDs
    //They are SHARED across all panels.

    void setBacklights(uint16_t newValue) {                           // Set the color of all LEDs with role LED_BACKLIGHT
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        if (newValue == current_backl_brightness) return;             // Exit if no brightness change
        int scale = map(newValue, 0, 65535, 0, 255);                  // Map the brightness scale factor to a range of 0-255
        CRGB target = NVIS_GREEN_A;
        target.nscale8_video(scale);                                  // Use FastLED's nscale8_video to apply the scale factor
        current_backl_brightness = newValue;                          // Update and save the current brightness value
                 
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is BACKLIGHT, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           // getLedTable() accesses the panel's LED table
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_BACKLIGHT) {
                getLedStrip()[ledIndex] = target;
            }
        }
        Board::getInstance()->setLedsNeedUpdate();                    // Inform the board that the LEDs need to be updated
    }


    void setIndicatorColor(LedRole role, const CRGB& color) {         // Set color of specific LEDs ("role" parameter)
        if (!getLedStrip() || !getLedTable()) return;                 
        int n = getLedCount();                                        
        for (int i = 0; i < n; i++) {                                 
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == role) {
                getLedStrip()[ledIndex] = color;
            }
        }
        Board::getInstance()->setLedsNeedUpdate();                    
    }


    void setFloodlights(uint16_t newValue) {                          // Set the brightness of LEDs with role LED_FLOOD
        if (!getLedStrip() || !getLedTable()) return;                 // Same structure as setBacklights()
        if (newValue == current_flood_brightness) return;             
        current_flood_brightness = newValue;
        
        uint8_t scale = map(newValue, 0, 65535, 0, 255);
        
        CRGB target = NVIS_WHITE;
        target.nscale8_video(scale);
        
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_FLOOD) {
                getLedStrip()[ledIndex] = target;
            }
        }
        Board::getInstance()->setLedsNeedUpdate();                    
    }
};

#endif 