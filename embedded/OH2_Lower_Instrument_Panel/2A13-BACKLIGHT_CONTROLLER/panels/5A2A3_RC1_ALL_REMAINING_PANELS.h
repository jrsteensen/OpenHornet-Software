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
const int RC1_ALL_REMAINING_LED_COUNT = 66;  // Total number of LEDs in the panel
const Led rc1AllRemainingLedTable[RC1_ALL_REMAINING_LED_COUNT] PROGMEM = {
    {0, LED_CONSOLE_BL}, {1, LED_CONSOLE_BL}, {2, LED_CONSOLE_BL}, {3, LED_CONSOLE_BL}, {4, LED_CONSOLE_BL}, 
    {5, LED_CONSOLE_BL}, {6, LED_CONSOLE_BL}, {7, LED_CONSOLE_BL}, {8, LED_CONSOLE_BL}, {9, LED_CONSOLE_BL},
    {10, LED_CONSOLE_BL}, {11, LED_CONSOLE_BL}, {12, LED_CONSOLE_BL}, {13, LED_CONSOLE_BL}, {14, LED_CONSOLE_BL}, 
    {15, LED_CONSOLE_BL}, {16, LED_CONSOLE_BL}, {17, LED_CONSOLE_BL}, {18, LED_CONSOLE_BL}, {19, LED_CONSOLE_BL},
    {20, LED_CONSOLE_BL}, {21, LED_CONSOLE_BL}, {22, LED_CONSOLE_BL}, {23, LED_CONSOLE_BL}, {24, LED_CONSOLE_BL}, 
    {25, LED_CONSOLE_BL}, {26, LED_CONSOLE_BL}, {27, LED_CONSOLE_BL}, {28, LED_CONSOLE_BL}, {29, LED_CONSOLE_BL},
    {30, LED_CONSOLE_BL}, {31, LED_CONSOLE_BL}, {32, LED_CONSOLE_BL}, {33, LED_CONSOLE_BL}, {34, LED_CONSOLE_BL}, 
    {35, LED_CONSOLE_BL}, {36, LED_CONSOLE_BL}, {37, LED_CONSOLE_BL}, {38, LED_CONSOLE_BL}, {39, LED_CONSOLE_BL},
    {40, LED_CONSOLE_BL}, {41, LED_CONSOLE_BL}, {42, LED_CONSOLE_BL}, {43, LED_CONSOLE_BL}, {44, LED_CONSOLE_BL}, 
    {45, LED_CONSOLE_BL}, {46, LED_CONSOLE_BL}, {47, LED_CONSOLE_BL}, {48, LED_CONSOLE_BL}, {49, LED_CONSOLE_BL},
    {50, LED_CONSOLE_BL}, {51, LED_CONSOLE_BL}, {52, LED_CONSOLE_BL}, {53, LED_CONSOLE_BL}, {54, LED_CONSOLE_BL}, 
    {55, LED_CONSOLE_BL}, {56, LED_CONSOLE_BL}, {57, LED_CONSOLE_BL}, {58, LED_CONSOLE_BL}, {59, LED_CONSOLE_BL},
    {60, LED_CONSOLE_BL}, {61, LED_CONSOLE_BL}, {62, LED_CONSOLE_BL}, {63, LED_CONSOLE_BL}, {64, LED_CONSOLE_BL}, 
    {65, LED_CONSOLE_BL}
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
 *          Total LEDs: 66
 *          Backlight LEDs: 66 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class Rc1AllRemainingPanels : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the Rc1AllRemainingPanels class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static Rc1AllRemainingPanels* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new Rc1AllRemainingPanels(startIndex, ledStrip);
        }
        return instance;
    }

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @see This method is called by the public getInstance() if and only if no instance exists yet
     */
    Rc1AllRemainingPanels(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RC1_ALL_REMAINING_LED_COUNT;
        ledTable = rc1AllRemainingLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static Rc1AllRemainingPanels* instance;
};

// Initialize static instance pointer
Rc1AllRemainingPanels* Rc1AllRemainingPanels::instance = nullptr;

#endif 