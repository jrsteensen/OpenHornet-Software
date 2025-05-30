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
 * @file      1A7A1_HUD_PANEL_REV4.h
 * @author    Ulukaii, Arribe, Higgins
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the HUD panel (Revision 4).
 *********************************************************************************************************************/


#ifndef __HUD_PANEL_REV4_H
#define __HUD_PANEL_REV4_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int HUD_LED_COUNT = 50;  // Total number of LEDs in the panel
const Led hudLedTable[HUD_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL}, {4, LED_INSTR_BL}, 
    {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, {7, LED_INSTR_BL}, {8, LED_INSTR_BL}, {9, LED_INSTR_BL},
    {10, LED_INSTR_BL}, {11, LED_INSTR_BL}, {12, LED_INSTR_BL}, {13, LED_INSTR_BL}, {14, LED_INSTR_BL}, 
    {15, LED_INSTR_BL}, {16, LED_INSTR_BL}, {17, LED_INSTR_BL}, {18, LED_INSTR_BL}, {19, LED_INSTR_BL},
    {20, LED_INSTR_BL}, {21, LED_INSTR_BL}, {22, LED_INSTR_BL}, {23, LED_INSTR_BL}, {24, LED_INSTR_BL}, 
    {25, LED_INSTR_BL}, {26, LED_INSTR_BL}, {27, LED_INSTR_BL}, {28, LED_INSTR_BL}, {29, LED_INSTR_BL},
    {30, LED_INSTR_BL}, {31, LED_INSTR_BL}, {32, LED_INSTR_BL}, {33, LED_INSTR_BL}, {34, LED_INSTR_BL}, 
    {35, LED_INSTR_BL}, {36, LED_INSTR_BL}, {37, LED_INSTR_BL}, {38, LED_INSTR_BL}, {39, LED_INSTR_BL},
    {40, LED_INSTR_BL}, {41, LED_INSTR_BL}, {42, LED_INSTR_BL}, {43, LED_INSTR_BL}, {44, LED_INSTR_BL}, 
    {45, LED_INSTR_BL}, {46, LED_INSTR_BL}, {47, LED_INSTR_BL}, {48, LED_INSTR_BL}, {49, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   HUD Panel class
 * @details Backlighting controller for the HUD panel.
 *          Total LEDs: 50
 *          Backlight LEDs: 50 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class HudPanel : public Panel {
public:
    static HudPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new HudPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    HudPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = HUD_LED_COUNT;
        ledTable = hudLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static HudPanel* instance;
};

// Initialize static instance pointer
HudPanel* HudPanel::instance = nullptr;

#endif 