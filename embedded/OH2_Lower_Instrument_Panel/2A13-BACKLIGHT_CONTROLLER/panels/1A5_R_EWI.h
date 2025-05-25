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
 * @file      1A5_R_EWI_BL.h
 * @author    Ulukaii, Arribe, Higgins
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Implements backlighting for the Right EWI panel.
 *********************************************************************************************************************/


#ifndef __R_EWI_BL_H
#define __R_EWI_BL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int R_EWI_LED_COUNT = 30;  // Total number of LEDs in the panel
const Led rEwiLedTable[R_EWI_LED_COUNT] PROGMEM = {
    {0, LED_R_FIRE}, {1, LED_R_FIRE}, {2, LED_R_FIRE}, {3, LED_R_FIRE}, {4, LED_APU_FIRE},
    {5, LED_APU_FIRE}, {6, LED_APU_FIRE}, {7, LED_APU_FIRE}, {8, LED_DISP}, {9, LED_DISP},
    {10, LED_RCDRON}, {11, LED_RCDRON}, {12, LED_SPARE1}, {13, LED_SPARE1}, {14, LED_SPARE2},
    {15, LED_SPARE2}, {16, LED_SPARE3}, {17, LED_SPARE3}, {18, LED_SPARE4}, {19, LED_SPARE4},
    {20, LED_SPARE5}, {21, LED_SPARE5}, {22, LED_SAM}, {23, LED_SAM}, {24, LED_AAA},
    {25, LED_AAA}, {26, LED_AI}, {27, LED_AI}, {28, LED_CW}, {29, LED_CW}
};

/********************************************************************************************************************
 * @brief   Right EWI Panel class
 * @details Backlighting and indicator controller for the Right EWI panel.
 *          Total LEDs: 30
 *          Indicator LEDs: 30 (FIRE: 0-3, APU FIRE: 4-7, DISP: 8-9, RCDRON ON: 10-11, L BLANK: 12-13,
 *                             R BLANK: 14-15, R BLANK2: 16-17, L BLANK2: 18-19, L BLANK3: 20-21,
 *                             SAM: 22-23, AAA: 24-25, AI: 26-27, CW: 28-29)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class REwiPanel : public Panel {
public:
    static REwiPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new REwiPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    REwiPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = R_EWI_LED_COUNT;
        ledTable = rEwiLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    static void onFireRightLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_R_FIRE, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer fireRightLtBuffer{FA_18C_hornet_FIRE_RIGHT_LT, onFireRightLtChange};

    static void onFireApuLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_APU_FIRE, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer fireApuLtBuffer{FA_18C_hornet_FIRE_APU_LT, onFireApuLtChange};

    static void onRhAdvAaaChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_AAA, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvAaaBuffer{FA_18C_hornet_RH_ADV_AAA, onRhAdvAaaChange};

    static void onRhAdvAiChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_AI, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvAiBuffer{FA_18C_hornet_RH_ADV_AI, onRhAdvAiChange};

    static void onRhAdvCwChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_CW, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvCwBuffer{FA_18C_hornet_RH_ADV_CW, onRhAdvCwChange};

    static void onRhAdvDispChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_DISP, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvDispBuffer{FA_18C_hornet_RH_ADV_DISP, onRhAdvDispChange};

    static void onRhAdvRcdrOnChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_RCDRON, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvRcdrOnBuffer{FA_18C_hornet_RH_ADV_RCDR_ON, onRhAdvRcdrOnChange};

    static void onRhAdvSamChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SAM, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSamBuffer{FA_18C_hornet_RH_ADV_SAM, onRhAdvSamChange};

    static void onRhAdvSpareRh1Change(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE1, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh1Buffer{FA_18C_hornet_RH_ADV_SPARE_RH1, onRhAdvSpareRh1Change};

    static void onRhAdvSpareRh2Change(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE2, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh2Buffer{FA_18C_hornet_RH_ADV_SPARE_RH2, onRhAdvSpareRh2Change};

    static void onRhAdvSpareRh3Change(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE3, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh3Buffer{FA_18C_hornet_RH_ADV_SPARE_RH3, onRhAdvSpareRh3Change};

    static void onRhAdvSpareRh4Change(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE4, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh4Buffer{FA_18C_hornet_RH_ADV_SPARE_RH4, onRhAdvSpareRh4Change};

    static void onRhAdvSpareRh5Change(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE5, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh5Buffer{FA_18C_hornet_RH_ADV_SPARE_RH5, onRhAdvSpareRh5Change};

    // Instance data
    static REwiPanel* instance;
};

// Initialize static instance pointer
REwiPanel* REwiPanel::instance = nullptr;

#endif