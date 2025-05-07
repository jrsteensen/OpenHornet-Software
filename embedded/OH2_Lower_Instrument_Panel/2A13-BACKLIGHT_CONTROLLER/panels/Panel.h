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
 *********************************************************************************************************************/


#ifndef __PANEL_H
#define __PANEL_H

#include "DcsBios.h"
#include "FastLED.h"
#include "Colors.h"
#include <Arduino.h>
#include <string.h>
#include <avr/pgmspace.h> 

/**********************************************************************************************************************
 * @brief   LED role enumeration: lists all different LED roles; LED info structure: bundles properties of one LED
 * @details Defines the different roles of LEDs in a panel.
 *          This enum is used for memory efficiency (Enum pointers only need 1 byte)
 *********************************************************************************************************************/
enum LedRole {
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

// Maximum length for legend text (including null terminator)
#define MAX_LEN 16

struct Led {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    LedRole  role;                // Role of LED as the enum defined above
    char     text[MAX_LEN];       // Free text to associate LED with legend text on the panel
};

/**********************************************************************************************************************
 * @brief   Abstract panel class.
 * @details This class provides the interface for all panel backlight and indicator lighting implementations.
 * @remark  The specific panel classes inherit basic functionality from this class and extend it by implementing their 
 *          own DCS-BIOS callback methods.
 *********************************************************************************************************************/
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
    }

    // Protected member variables that inheriting classes must set
    int panelStartIndex;             // Start index of the panel on the LED strip
    int ledCount;                    // Number of LEDs in the panel
    const Led* ledTable;             // Pointer to the LED table
    CRGB* ledStrip;                  // Pointer to the LED strip
    uint8_t last_brightness;         // Last brightness value

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
};

// Define the static member variable
bool Panel::ledsNeedUpdate;

#endif 