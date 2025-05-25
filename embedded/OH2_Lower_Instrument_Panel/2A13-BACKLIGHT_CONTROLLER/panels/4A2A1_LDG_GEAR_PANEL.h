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
 * @file      4A2A1_LDG_GEAR_PANEL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting and indicators for the Landing Gear panel.
 *********************************************************************************************************************/
#ifndef __LDG_GEAR_PANEL_H
#define __LDG_GEAR_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LDG_GEAR_LED_COUNT = 23;  // Total number of LEDs in the panel
const Led ldgGearLedTable[LDG_GEAR_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT},
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT},
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   Landing Gear Panel class
 * @details Backlighting controller for the Landing Gear panel.
 *          Total LEDs: 23
 *          Backlight LEDs: 23 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class LdgGearPanel : public Panel {
public:
    static LdgGearPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new LdgGearPanel(startIndex, ledStrip);
        }
        return instance;
    }

    int getLedCount() const override {
        return LDG_GEAR_LED_COUNT;
    }

    const Led* getLedTable() const override {
        return ldgGearLedTable;
    }

private:
    // Private constructor
    LdgGearPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LDG_GEAR_LED_COUNT;
        ledTable = ldgGearLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    // Instance data
    static LdgGearPanel* instance;
};

// Initialize static instance pointer
LdgGearPanel* LdgGearPanel::instance = nullptr;

#endif 