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
 * @file      Panel.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Abstract base class for all panels. Each panel must be a derived class from this base class.
 * @details   It provides functions that are repeatedly required across all panels: 
 *            setInstrLights(), setIndicatorColor(), setFloodlights().
 *            Panels are added to Channels. For memory efficiency, panels are organized in a linked list within each 
 *            channel. This conserves stack memory.
 *            This approach avoids allocating fixed-size arrays for panel pointers in each channel, 
 *            which would exhaust the limited stack space on the Arduino Mega 2560 (I tested it).
 *            Instead, this class provides a pointer to the next panel in its channel.
 *            Thus,the parent channels still can iterate through all of their panels.
 *********************************************************************************************************************/


#ifndef __PANEL_H
#define __PANEL_H

#include <Arduino.h>
#include <avr/pgmspace.h> 
#include "DcsBios.h"
#include "FastLED.h"
#include "LedRole.h"
#include "LedStruct.h"
#include "LedUpdateState.h"
#include "Colors.h"

class Panel {
public:
    /**
     * @brief Gets the start index of this panel on the LED strip
     * @return The start index
     */
    virtual int getStartIndex() const { return panelStartIndex; }

    /**
     * @brief Gets the number of LEDs in this panel
     * @return The LED count
     */
    virtual int getLedCount() const { return ledCount; }

    /**
     * @brief Gets the LED table for this panel
     * @return Pointer to the LED table
     */
    virtual const Led* getLedTable() const { return ledTable; }

    /**
     * @brief Gets the LED strip for this panel
     * @return Pointer to the LED strip
     */
    virtual CRGB* getLedStrip() const { return ledStrip; }
    
    // Add friend declaration to allow Channel to access nextPanel and its protected methods
    friend class Channel;
    
protected:
    /**
     * @brief Protected constructor to prevent direct instantiation
     * @see This method is called by derived panel classes
     */
    Panel() {
        current_backl_brightness = 0;
        current_console_brightness = 0;
        current_flood_brightness = 0;
        nextPanel = nullptr;  // Initialize next panel pointer
    }


    int panelStartIndex;                                              // Start index of the panel on the LED strip
    int ledCount;                                                     // Number of LEDs in the panel
    const Led* ledTable;                                              // Pointer to the LED table
    CRGB* ledStrip;                                                   // Pointer to the LED strip
    uint16_t current_backl_brightness;                                // Current br. value for backlights (0-65535)
    uint16_t current_console_brightness;                              // Current br. value for console lights (0-65535)
    uint16_t current_flood_brightness;                                // Current br. value for floodlights (0-65535)
    Panel* nextPanel;                                                 // Pointer to next panel in the channel


    /**
     * @brief Set the color of all instrument backlight LEDs
     * @param newValue The new brightness value (0-65535)
     * @param color The color to set (defaults to NVIS_GREEN_A)
     * @see This method is called by Channel::updateInstrLights()
     */
    void setInstrLights(uint16_t newValue, const CRGB& color = NVIS_GREEN_A) {                           
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        if (newValue == current_backl_brightness) return;             // Exit if no brightness change
        int scale = map(newValue, 0, 65535, 0, 255);                  // Map the brightness scale factor to a range of 0-255
        CRGB target = color;
        target.nscale8_video(scale);                                  // Use FastLED's nscale8_video to apply the scale factor
        CRGB target2 = NVIS_CGRB_GREEN_A;                             // For GRB LEDs (e.g. Radar Altimeter and Standby Instruments)
        target2.nscale8_video(scale);
        current_backl_brightness = newValue;                          // Update and save the current brightness value
                 
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is BACKLIGHT, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           // getLedTable() accesses the panel's LED table
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_INSTR_BL) {
                getLedStrip()[ledIndex] = target;
            }
            if (led.role == LED_INSTR_BL_CGRB) {
                getLedStrip()[ledIndex] = target2;
            }
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Sets the color of all console backlight LEDs
     * @param newValue The new brightness value (0-65535)
     * @param color The color to set (defaults to NVIS_GREEN_A)
     * @see This method is called by Channel::updateConsoleLights()
     */
    void setConsoleLights(uint16_t newValue, const CRGB& color = NVIS_GREEN_A) {                        // Set the color of all LEDs with role LED_CONSOLE_BL
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        if (newValue == current_console_brightness) return;           // Exit if no brightness change
        int scale = map(newValue, 0, 65535, 0, 255);                  // Map the brightness scale factor to a range of 0-255
        CRGB target = color;
        target.nscale8_video(scale);                                  // Use FastLED's nscale8_video to apply the scale factor
        current_console_brightness = newValue;                        // Update and save the current brightness value
                 
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if LED is BACKLIGHT, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           // getLedTable() accesses the panel's LED table
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_CONSOLE_BL) {
                getLedStrip()[ledIndex] = target;
            }
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Sets the color of LEDs with a specific role
     * @param role The role of LEDs to update
     * @param color The color to set
     * @see This method is called by derived panel classes to update indicator lights
     */
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
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Sets the brightness of floodlight LEDs
     * @param newValue The new brightness value (0-65535)
     * @see This method is called by Channel::updateFloodLights()
     */
    void setFloodlights(uint16_t newValue) {                          // Set the brightness of LEDs with role LED_FLOOD
        if (!getLedStrip() || !getLedTable()) return;                 // Same structure as setInstrLights()
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
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Turns off all lights in this panel, irrespective of their role, and resets brightness state
     * @see This method is called by Channel::setAllLightsOff()
     */
    void setAllLightsOff() {                                          // Turn off all lights and reset brightness state
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        
        CRGB* ledArray = getLedStrip();
        int startIndex = getStartIndex();
        int panelLedCount = getLedCount();
        
        for (int i = 0; i < panelLedCount; i++) {
            ledArray[startIndex + i] = NVIS_BLACK;
        }
        
        // Reset all brightness variables to 0
        current_backl_brightness = 0;
        current_console_brightness = 0;
        current_flood_brightness = 0;
        
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }
};

#endif 