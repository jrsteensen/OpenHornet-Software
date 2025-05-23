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
 * @file      5A3A1_CAUTION.h
 * @author    Ulukaii
 * @date      20.05.2025
 * @version   u 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Caution panel.
 *********************************************************************************************************************/


#ifndef __CAUTION_H
#define __CAUTION_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int CAUTION_LED_COUNT = 24;  // Total number of LEDs in the panel
const Led cautionLedTable[CAUTION_LED_COUNT] PROGMEM = {
    {0, LED_CK_SEAT}, {1, LED_CK_SEAT},           // CK SEAT
    {2, LED_APU_ACC}, {3, LED_APU_ACC},           // APU ACC
    {4, LED_BATT_SW}, {5, LED_BATT_SW},           // BATT SW
    {6, LED_FCS_HOT}, {7, LED_FCS_HOT},           // FCS HOT
    {8, LED_GEN_TIE}, {9, LED_GEN_TIE},           // GEN TIE
    {10, LED_SPARE1}, {11, LED_SPARE1},           // Spare1
    {12, LED_FUEL_LO}, {13, LED_FUEL_LO},         // FUEL LO
    {14, LED_FCES}, {15, LED_FCES},               // FCES
    {16, LED_SPARE2}, {17, LED_SPARE2},           // Spare2
    {18, LED_L_GEN}, {19, LED_L_GEN},             // L GEN
    {20, LED_R_GEN}, {21, LED_R_GEN},             // R GEN
    {22, LED_SPARE3}, {23, LED_SPARE3}            // Spare3
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int CAUTION_TEXT_COUNT = 0;  // Number of LEDs that need text
const LedText cautionTextTable[CAUTION_TEXT_COUNT] PROGMEM = {
    // No text needed for this panel
};

/********************************************************************************************************************
 * @brief   Caution Panel class
 * @details Backlighting controller for the Caution panel.
 *          Total LEDs: 24
 *          Backlight LEDs: 24 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class CautionPanel : public Panel {
public:
    static CautionPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new CautionPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    CautionPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = CAUTION_LED_COUNT;
        ledTable = cautionLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onClipApuAccLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_APU_ACC, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipApuAccLtBuffer{0x74a4, 0x0100, 8, onClipApuAccLtChange};

    static void onClipBattSwLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_BATT_SW, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipBattSwLtBuffer{0x74a4, 0x0200, 9, onClipBattSwLtChange};

    static void onClipCkSeatLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_CK_SEAT, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipCkSeatLtBuffer{0x74a0, 0x8000, 15, onClipCkSeatLtChange};

    static void onClipFcesLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_FCES, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipFcesLtBuffer{0x74a4, 0x4000, 14, onClipFcesLtChange};

    static void onClipFcsHotLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_FCS_HOT, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipFcsHotLtBuffer{0x74a4, 0x0400, 10, onClipFcsHotLtChange};

    static void onClipFuelLoLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_FUEL_LO, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipFuelLoLtBuffer{0x74a4, 0x2000, 13, onClipFuelLoLtChange};

    static void onClipGenTieLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_GEN_TIE, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipGenTieLtBuffer{0x74a4, 0x0800, 11, onClipGenTieLtChange};

    static void onClipLGenLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_L_GEN, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipLGenLtBuffer{0x74a8, 0x0100, 8, onClipLGenLtChange};

    static void onClipRGenLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_R_GEN, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipRGenLtBuffer{0x74a8, 0x0200, 9, onClipRGenLtChange};

    static void onClipSpareCtn1LtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE1, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipSpareCtn1LtBuffer{0x74a4, 0x1000, 12, onClipSpareCtn1LtChange};

    static void onClipSpareCtn2LtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE2, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipSpareCtn2LtBuffer{0x74a4, 0x8000, 15, onClipSpareCtn2LtChange};

    static void onClipSpareCtn3LtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPARE3, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer clipSpareCtn3LtBuffer{0x74a8, 0x0400, 10, onClipSpareCtn3LtChange};

    // Instance data
    static CautionPanel* instance;
};

// Initialize static instance pointer
CautionPanel* CautionPanel::instance = nullptr;

#endif 