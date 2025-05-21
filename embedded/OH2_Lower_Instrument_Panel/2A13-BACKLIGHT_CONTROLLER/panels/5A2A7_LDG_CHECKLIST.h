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
 * @file      5A2A7_LDG_CHECKLIST.h
 * @author    Ulukaii
 * @date      20.05.2025
 * @version   t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Landing Checklist panel.
 *********************************************************************************************************************/


#ifndef __LDG_CHECKLIST_H
#define __LDG_CHECKLIST_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LDG_CHECKLIST_LED_COUNT = 24;  // Total number of LEDs in the panel
const Led ldgChecklistLedTable[LDG_CHECKLIST_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT}, 
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT}, 
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}, {23, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LDG_CHECKLIST_TEXT_COUNT = 0;  // Number of LEDs that need text
const LedText ldgChecklistTextTable[LDG_CHECKLIST_TEXT_COUNT] PROGMEM = {
    // No text needed for this panel
};

/********************************************************************************************************************
 * @brief   Landing Checklist Panel class
 * @details Backlighting controller for the Landing Checklist panel.
 *          Total LEDs: 24
 *          Backlight LEDs: 24 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class LdgChecklistPanel : public Panel {
public:
    static LdgChecklistPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new LdgChecklistPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    LdgChecklistPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LDG_CHECKLIST_LED_COUNT;
        ledTable = ldgChecklistLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static LdgChecklistPanel* instance;
};

// Initialize static instance pointer
LdgChecklistPanel* LdgChecklistPanel::instance = nullptr;

#endif 