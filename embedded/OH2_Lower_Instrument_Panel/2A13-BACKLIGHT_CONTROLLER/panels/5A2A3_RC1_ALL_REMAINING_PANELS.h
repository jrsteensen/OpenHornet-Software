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
 * @file      5A2A3_RC1_ALL_REMAINING_PANELS.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for all remaining panels in RC1.
 *********************************************************************************************************************/


#ifndef __RC1_ALL_REMAINING_PANELS_H
#define __RC1_ALL_REMAINING_PANELS_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RC1_ALL_REMAINING_LED_COUNT = 78;  // Total number of LEDs in the panel
const Led rc1AllRemainingLedTable[RC1_ALL_REMAINING_LED_COUNT] PROGMEM = {
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
    {55, LED_BACKLIGHT}, {56, LED_BACKLIGHT}, {57, LED_BACKLIGHT}, {58, LED_BACKLIGHT}, {59, LED_BACKLIGHT},
    {60, LED_BACKLIGHT}, {61, LED_BACKLIGHT}, {62, LED_BACKLIGHT}, {63, LED_BACKLIGHT}, {64, LED_BACKLIGHT}, 
    {65, LED_BACKLIGHT}, {66, LED_BACKLIGHT}, {67, LED_BACKLIGHT}, {68, LED_BACKLIGHT}, {69, LED_BACKLIGHT},
    {70, LED_BACKLIGHT}, {71, LED_BACKLIGHT}, {72, LED_BACKLIGHT}, {73, LED_BACKLIGHT}, {74, LED_BACKLIGHT}, 
    {75, LED_BACKLIGHT}, {76, LED_BACKLIGHT}, {77, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RC1_ALL_REMAINING_TEXT_COUNT = 0;  // Number of LEDs that need text
const LedText rc1AllRemainingTextTable[RC1_ALL_REMAINING_TEXT_COUNT] PROGMEM = {
    // No text needed for this panel
};

/********************************************************************************************************************
 * @brief   RC1 All Remaining Panels class
 * @details Backlighting controller for all remaining panels in RC1.
 *          Total LEDs: 78
 *          Backlight LEDs: 78 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class Rc1AllRemainingPanels : public Panel {
public:
    static Rc1AllRemainingPanels* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new Rc1AllRemainingPanels(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    Rc1AllRemainingPanels(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RC1_ALL_REMAINING_LED_COUNT;
        ledTable = rc1AllRemainingLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static Rc1AllRemainingPanels* instance;
};

// Initialize static instance pointer
Rc1AllRemainingPanels* Rc1AllRemainingPanels::instance = nullptr;

#endif 