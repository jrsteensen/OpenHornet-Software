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
 * @file      2A2A1A7_RWR_CONTROL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting and indicators for the RWR Control panel.
 *********************************************************************************************************************/


#ifndef __RWR_CONTROL_H
#define __RWR_CONTROL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RWR_CONTROL_LED_COUNT = 32;  // Total number of LEDs in the panel
const Led rwrControlLedTable[RWR_CONTROL_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT},
    {4, LED_BACKLIGHT}, {5, LED_BACKLIGHT}, {6, LED_RWR_BIT}, {7, LED_RWR_BIT_FAIL},
    {8, LED_RWR_BIT_FAIL}, {9, LED_RWR_BIT}, {10, LED_RWR_OFFSET}, {11, LED_RWR_OFFSET_EN},
    {12, LED_RWR_OFFSET_EN}, {13, LED_RWR_OFFSET}, {14, LED_RWR_SPECIAL}, {15, LED_RWR_SPECIAL_EN},
    {16, LED_RWR_SPECIAL_EN}, {17, LED_RWR_SPECIAL}, {18, LED_RWR_DISPLAY}, {19, LED_RWR_LIMIT},
    {20, LED_RWR_LIMIT}, {21, LED_RWR_DISPLAY}, {22, LED_RWR_POWER}, {23, LED_RWR_POWER},
    {24, LED_RWR_NONE}, {25, LED_RWR_NONE}, {26, LED_BACKLIGHT}, {27, LED_BACKLIGHT},
    {28, LED_BACKLIGHT}, {29, LED_BACKLIGHT}, {30, LED_BACKLIGHT}, {31, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   RWR Control Panel class
 * @details Backlighting controller for the RWR Control panel.
 *          Total LEDs: 32
 *          Backlight LEDs: 13 
 *          Indicator LEDs: 19 (indices 7-25)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class RwrControlPanel : public Panel {
public:
    static RwrControlPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new RwrControlPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    RwrControlPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RWR_CONTROL_LED_COUNT;
        ledTable = rwrControlLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    // RWR indicator callbacks
    static void onRwrBitLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_BIT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrBitLtBuffer{FA_18C_hornet_RWR_BIT_LT, onRwrBitLtChange};

    static void onRwrFailLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_BIT_FAIL, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrFailLtBuffer{FA_18C_hornet_RWR_FAIL_LT, onRwrFailLtChange};

    static void onRwrOffsetLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_OFFSET, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrOffsetLtBuffer{FA_18C_hornet_RWR_OFFSET_LT, onRwrOffsetLtChange};

    static void onRwrEnableLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_OFFSET_EN, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrEnableLtBuffer{FA_18C_hornet_RWR_ENABLE_LT, onRwrEnableLtChange};

    static void onRwrSpecialLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_SPECIAL, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrSpecialLtBuffer{FA_18C_hornet_RWR_SPECIAL_LT, onRwrSpecialLtChange};

    static void onRwrSpecialEnLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_SPECIAL_EN, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrSpecialEnLtBuffer{FA_18C_hornet_RWR_SPECIAL_EN_LT, onRwrSpecialEnLtChange};

    static void onRwrDisplayLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_DISPLAY, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrDisplayLtBuffer{FA_18C_hornet_RWR_DISPLAY_LT, onRwrDisplayLtChange};

    static void onRwrLimitLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_LIMIT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrLimitLtBuffer{FA_18C_hornet_RWR_LIMIT_LT, onRwrLimitLtChange};

    static void onRwrLowerLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RWR_POWER, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rwrLowerLtBuffer{FA_18C_hornet_RWR_LOWER_LT, onRwrLowerLtChange};

    // Instance data
    static RwrControlPanel* instance;
};

// Initialize static instance pointer
RwrControlPanel* RwrControlPanel::instance = nullptr;

#endif 