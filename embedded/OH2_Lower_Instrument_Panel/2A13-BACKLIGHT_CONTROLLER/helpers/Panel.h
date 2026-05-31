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
 *            applyInstrLights(), applyConsoleLights(), applyFloodlights(), setIndicatorColor().
 *            The brightness math (16-bit DCS value -> 8-bit scale -> CRGB target) is centralized
 *            in Board; this class only receives the pre-scaled CRGB targets and writes them to the
 *            LEDs whose role matches.
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
        nextPanel = nullptr;  // Initialize next panel pointer
    }


    int panelStartIndex;                                              // Start index of the panel on the LED strip
    int ledCount;                                                     // Number of LEDs in the panel
    const Led* ledTable;                                              // Pointer to the LED table
    CRGB* ledStrip;                                                   // Pointer to the LED strip
    Panel* nextPanel;                                                 // Pointer to next panel in the channel


    /**
     * @brief Writes the pre-scaled instrument-backlight targets to all matching LEDs
     * @param instrTarget Pre-scaled CRGB for LEDs with role LED_INSTR_BL
     * @param cgrbTarget  Pre-scaled CRGB for LEDs with role LED_INSTR_BL_CGRB
     *                    (used by panels such as Radar Altimeter and Standby Instruments)
     * @see This method is called by Channel::applyInstrLights()
     */
    void applyInstrLights(const CRGB& instrTarget, const CRGB& cgrbTarget) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if it is a backlight, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           // getLedTable() accesses the panel's LED table
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_INSTR_BL) {
                getLedStrip()[ledIndex] = instrTarget;
            } else if (led.role == LED_INSTR_BL_CGRB) {
                getLedStrip()[ledIndex] = cgrbTarget;
            }
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Writes the pre-scaled console-backlight target to all matching LEDs
     * @param consoleTarget Pre-scaled CRGB for LEDs with role LED_CONSOLE_BL
     * @see This method is called by Channel::applyConsoleLights()
     */
    void applyConsoleLights(const CRGB& consoleTarget) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        int n = getLedCount();
        for (int i = 0; i < n; i++) {                                 // For each LED, read info from PROGMEM; if it is a console backlight, set color
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));           // getLedTable() accesses the panel's LED table
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_CONSOLE_BL) {
                getLedStrip()[ledIndex] = consoleTarget;
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
     * @brief Writes the pre-scaled floodlight target to all matching LEDs
     * @param floodTarget Pre-scaled CRGB for LEDs with role LED_FLOOD
     * @see This method is called by Channel::applyFloodlights()
     */
    void applyFloodlights(const CRGB& floodTarget) {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        int n = getLedCount();
        for (int i = 0; i < n; i++) {
            Led led;
            memcpy_P(&led, &getLedTable()[i], sizeof(Led));
            uint16_t ledIndex = led.index + getStartIndex();
            if (led.role == LED_FLOOD) {
                getLedStrip()[ledIndex] = floodTarget;
            }
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }

    /**
     * @brief Turns off all lights in this panel, irrespective of their role
     * @see This method is called by Channel::setAllLightsOff()
     */
    void setAllLightsOff() {
        if (!getLedStrip() || !getLedTable()) return;                 // Safety checks
        
        CRGB* ledArray = getLedStrip();
        int startIndex = getStartIndex();
        int panelLedCount = getLedCount();
        
        for (int i = 0; i < panelLedCount; i++) {
            ledArray[startIndex + i] = NVIS_BLACK;
        }
        
        LedUpdateState::getInstance()->setUpdateFlag(true);           // Inform that LEDs need to be updated
    }
};

#endif 