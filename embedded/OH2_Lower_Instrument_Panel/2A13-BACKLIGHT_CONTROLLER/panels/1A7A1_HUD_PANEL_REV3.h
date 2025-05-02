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
 * @file 1A7A1_HUD_PANEL_REV3.h
 * @author Ulukaii, Arribe, Higgins
 * @date 04.30.2025
 * @version t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Implements backlighting for the HUD panel (Revision 3).
 *********************************************************************************************************************/

#ifndef __HUD_PANEL_REV3_H
#define __HUD_PANEL_REV3_H

#include "DcsBios.h"
#include "Panel.h"
#include "FastLED.h"
#include "Colors.h"
#include <avr/pgmspace.h>


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Type" in this context refers to the LED type enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int HUD_PANEL_REV3_LED_COUNT = 56;  // Total number of LEDs in the panel (Revision 3)
const LedInfo hudRev3LedIndicesTable[HUD_PANEL_REV3_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT}, 
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT}, 
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}, {23, LED_BACKLIGHT}, {24, LED_BACKLIGHT}, 
    {25, LED_BACKLIGHT}, {26, LED_BACKLIGHT}, {27, LED_BACKLIGHT}, {28, LED_BACKLIGHT}, {29, LED_BACKLIGHT},
    {30, LED_BACKLIGHT}, {31, LED_BACKLIGHT}, {32, LED_BACKLIGHT}, {33, LED_BACKLIGHT}, {34, LED_BACKLIGHT}, 
    {35, LED_BACKLIGHT}, {36, LED_BACKLIGHT}, {37, LED_BACKLIGHT}, {38, LED_BACKLIGHT}, {39, LED_BACKLIGHT},
    {40, LED_BACKLIGHT}, {41, LED_BACKLIGHT}, {42, LED_BACKLIGHT}, {43, LED_BACKLIGHT}, {44, LED_BACKLIGHT}, 
    {45, LED_BACKLIGHT}, {46, LED_BACKLIGHT}, {47, LED_BACKLIGHT}, {48, LED_BACKLIGHT}, {49, LED_BACKLIGHT},
    {50, LED_BACKLIGHT}, {51, LED_BACKLIGHT}, {52, LED_BACKLIGHT}, {53, LED_BACKLIGHT}, {54, LED_BACKLIGHT}, 
    {55, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   HUD Panel class (Revision 3)
 * @details Backlighting controller for the HUD panel (Revision 3).
 *          Total LEDs: 56
 *          All LEDs are backlight LEDs
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class HudPanelRev3 : public Panel {
public:
    static HudPanelRev3* getInstance(int startIndex = 0, CRGB* ledArray = nullptr) {
        if (!instance) {
            instance = new HudPanelRev3(startIndex, ledArray);
        }
        return instance;
    }

    // Implementation of pure virtual methods
    virtual int getStartIndex() const override { return panelStartIndex; }
    virtual int getLedCount() const override { return ledCount; }
    virtual const LedInfo* getLedIndicesTable() const override { return hudRev3LedIndicesTable; }
    virtual CRGB* getLedArray() const override { return leds; }

private:
    // Private constructor
    HudPanelRev3(int startIndex, CRGB* ledArray) {
        panelStartIndex = startIndex;
        leds = ledArray;
        ledCount = HUD_PANEL_REV3_LED_COUNT;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static HudPanelRev3* instance;
    int panelStartIndex;
    int ledCount;
    CRGB* leds;
};

// Initialize static instance pointer
HudPanelRev3* HudPanelRev3::instance = nullptr;

#endif 