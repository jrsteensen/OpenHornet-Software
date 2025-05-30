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
 * @file      1A6A1_SPN_RCVY_BL.h
 * @author    Ulukaii, Arribe, Higgins
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Spin Recovery panel.
 *********************************************************************************************************************/


#ifndef __SPN_RCVY_BL_H
#define __SPN_RCVY_BL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int SPN_RCVY_LED_COUNT = 63;  // Total number of LEDs in the panel
const Led spnRcvyLedTable[SPN_RCVY_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL}, {4, LED_INSTR_BL}, 
    {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, {7, LED_INSTR_BL}, {8, LED_INSTR_BL}, {9, LED_INSTR_BL},
    {10, LED_INSTR_BL}, {11, LED_INSTR_BL}, {12, LED_INSTR_BL}, {13, LED_INSTR_BL}, {14, LED_INSTR_BL}, 
    {15, LED_INSTR_BL}, {16, LED_INSTR_BL}, {17, LED_INSTR_BL}, {18, LED_INSTR_BL}, {19, LED_INSTR_BL},
    {20, LED_INSTR_BL}, {21, LED_INSTR_BL}, {22, LED_INSTR_BL}, {23, LED_INSTR_BL}, {24, LED_INSTR_BL}, 
    {25, LED_INSTR_BL}, {26, LED_INSTR_BL}, {27, LED_INSTR_BL}, {28, LED_INSTR_BL}, {29, LED_SPIN},
    {30, LED_INSTR_BL}, {31, LED_INSTR_BL}, {32, LED_INSTR_BL}, {33, LED_INSTR_BL}, {34, LED_INSTR_BL}, 
    {35, LED_INSTR_BL}, {36, LED_SPIN}, {37, LED_INSTR_BL}, {38, LED_INSTR_BL}, {39, LED_INSTR_BL},
    {40, LED_INSTR_BL}, {41, LED_INSTR_BL}, {42, LED_INSTR_BL}, {43, LED_INSTR_BL}, {44, LED_INSTR_BL}, 
    {45, LED_INSTR_BL}, {46, LED_INSTR_BL}, {47, LED_INSTR_BL}, {48, LED_INSTR_BL}, {49, LED_INSTR_BL},
    {50, LED_INSTR_BL}, {51, LED_INSTR_BL}, {52, LED_INSTR_BL}, {53, LED_INSTR_BL}, {54, LED_INSTR_BL}, 
    {55, LED_INSTR_BL}, {56, LED_INSTR_BL}, {57, LED_INSTR_BL}, {58, LED_INSTR_BL}, {59, LED_INSTR_BL},
    {60, LED_INSTR_BL}, {61, LED_INSTR_BL}, {62, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   Spin Recovery Panel class
 * @details Backlighting and indicator controller for the Spin Recovery panel.
 *          Total LEDs: 63
 *          Backlight LEDs: 61 (all except positions 29 and 36)
 *          Indicator LEDs: 2 (SPIN: 29, SPIN2: 36)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class SpnRcvyPanel : public Panel {
public:
    static SpnRcvyPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new SpnRcvyPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    SpnRcvyPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = SPN_RCVY_LED_COUNT;
        ledTable = spnRcvyLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onSpinLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPIN, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer spinLtBuffer{FA_18C_hornet_SPIN_LT, onSpinLtChange};

    // Instance data
    static SpnRcvyPanel* instance;
};

// Initialize static instance pointer
SpnRcvyPanel* SpnRcvyPanel::instance = nullptr;

#endif