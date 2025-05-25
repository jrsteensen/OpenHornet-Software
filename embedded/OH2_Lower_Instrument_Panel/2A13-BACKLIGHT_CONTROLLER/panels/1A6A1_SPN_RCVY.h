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
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT}, 
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT}, 
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}, {23, LED_BACKLIGHT}, {24, LED_BACKLIGHT}, 
    {25, LED_BACKLIGHT}, {26, LED_BACKLIGHT}, {27, LED_BACKLIGHT}, {28, LED_BACKLIGHT}, {29, LED_SPIN},
    {30, LED_BACKLIGHT}, {31, LED_BACKLIGHT}, {32, LED_BACKLIGHT}, {33, LED_BACKLIGHT}, {34, LED_BACKLIGHT}, 
    {35, LED_BACKLIGHT}, {36, LED_SPIN}, {37, LED_BACKLIGHT}, {38, LED_BACKLIGHT}, {39, LED_BACKLIGHT},
    {40, LED_BACKLIGHT}, {41, LED_BACKLIGHT}, {42, LED_BACKLIGHT}, {43, LED_BACKLIGHT}, {44, LED_BACKLIGHT}, 
    {45, LED_BACKLIGHT}, {46, LED_BACKLIGHT}, {47, LED_BACKLIGHT}, {48, LED_BACKLIGHT}, {49, LED_BACKLIGHT},
    {50, LED_BACKLIGHT}, {51, LED_BACKLIGHT}, {52, LED_BACKLIGHT}, {53, LED_BACKLIGHT}, {54, LED_BACKLIGHT}, 
    {55, LED_BACKLIGHT}, {56, LED_BACKLIGHT}, {57, LED_BACKLIGHT}, {58, LED_BACKLIGHT}, {59, LED_BACKLIGHT},
    {60, LED_BACKLIGHT}, {61, LED_BACKLIGHT}, {62, LED_BACKLIGHT}
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
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

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