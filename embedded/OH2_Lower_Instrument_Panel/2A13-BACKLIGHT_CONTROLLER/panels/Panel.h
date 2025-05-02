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
 * @author Ulukaii, Arribe, Higgins
 * @date 04.30.2025
 * @version t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 *********************************************************************************************************************/


#ifndef __PANEL_H
#define __PANEL_H

#include <Arduino.h>
#include <string.h>
#include "FastLED.h"
#include <avr/pgmspace.h> 

/**********************************************************************************************************************
 * @brief LED type enumeration: lists all different LED types; LED info structure: bundles the index and type of an LED
 * @details Defines the different types of LEDs in a panel.
 *          This enum is used for memory efficiency (Enum pointers only need 1 byte)
 *********************************************************************************************************************/
enum LedType {
    LED_BACKLIGHT,
    LED_READY,
    LED_DISCH,
    LED_AG,
    LED_AA,
    // EWI Panel specific types
    LED_FIRE,
    LED_CAUTION,
    LED_GO,
    LED_NO_GO,
    LED_R_BLEED,
    LED_L_BLEED,
    LED_SPD_BRK,
    LED_STBY,
    LED_REC,
    LED_L_BAR1,
    LED_L_BAR2,
    LED_XMIT,
    LED_ASPJ_ON,
    // Right EWI Panel specific types
    LED_R_FIRE,
    LED_APU_FIRE,
    LED_DISP,
    LED_RCDRON,
    LED_BLANK1,  
    LED_BLANK2,  
    LED_BLANK3,  
    LED_BLANK4,  
    LED_BLANK5,  
    LED_SAM,
    LED_AAA,
    LED_AI,
    LED_CW,
    // Spin Recovery Panel specific types
    LED_SPIN
};

struct LedInfo {
    uint16_t index;     // Local index (will be added to panel's start index)
    LedType type;       // Type of LED as enum
};

/**********************************************************************************************************************
 * @brief Abstract panel class.
 * @details This class provides the interface for all panel backlight and indicator lighting implementations.
 * @remark The specific panel classes need to inherit from this base class and implement:
 *         - Panel-specific DCS-BIOS callback methods
 *         The specific panel classes then may use the static methods herein to set the backlight and indicator colors.
 *********************************************************************************************************************/
class Panel {
public:
    // Pure virtual methods that derived classes must implement
    virtual int getStartIndex() const { return panelStartIndex; }
    virtual int getLedCount() const { return ledCount; }
    virtual const LedInfo* getLedIndicesTable() const { return ledIndicesTable; }
    virtual CRGB* getLedArray() const { return leds; }

protected:
    // Protected constructor to prevent direct instantiation
    Panel() {
        last_brightness = 255;
    }

    // Protected member variables that derived classes must set
    int panelStartIndex;
    int ledCount;
    const LedInfo* ledIndicesTable;
    CRGB* leds;

    // Instance variable for tracking brightness
    uint8_t last_brightness;

    // Protected methods that derived classes can use
    void setBacklights(int newValue) {
        // Safety checks
        if (!getLedArray() || !getLedIndicesTable()) return;

        // Determine the brightness value
        uint8_t brightness = map(newValue, 0, 65535, 0, 255);
        
        // Add value-based debouncing
        if (brightness == last_brightness) return;
        last_brightness = brightness;

        // Set the LED colors
        CRGB color = CRGB(0, 100 * brightness / 255, 0); 
        // Read LED info from PROGMEM for each LED, check LED type is BACKLIGHT and set color
        for (int i = 0; i < getLedCount(); i++) {
            LedInfo info;
            memcpy_P(&info, &getLedIndicesTable()[i], sizeof(LedInfo));
            uint16_t ledIndex = info.index + getStartIndex();
            if (info.type == LED_BACKLIGHT) {
                getLedArray()[ledIndex] = color;
            }
        }
        FastLED.show();
    }

    void setIndicatorColor(LedType type, const CRGB& color) {
        // Safety checks
        if (!getLedArray() || !getLedIndicesTable()) return;

        // Read LED info from PROGMEM for each LED, check LED type is specified type and set color
        for (int i = 0; i < getLedCount(); i++) {
            LedInfo info;
            memcpy_P(&info, &getLedIndicesTable()[i], sizeof(LedInfo));
            uint16_t ledIndex = info.index + getStartIndex();
            if (info.type == type) {
                getLedArray()[ledIndex] = color;
            }
        }
        FastLED.show();
    }
};

#endif 