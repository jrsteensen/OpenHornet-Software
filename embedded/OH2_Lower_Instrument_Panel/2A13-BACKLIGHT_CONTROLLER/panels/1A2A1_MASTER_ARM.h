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
 * @file      1A2A1_MASTER_ARM_BL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Master Arm panel.
 *********************************************************************************************************************/


#ifndef __MASTER_ARM_BL_H
#define __MASTER_ARM_BL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"



/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int MASTER_ARM_LED_COUNT = 29;  // Total number of LEDs in the panel
const Led masterArmLedTable[MASTER_ARM_LED_COUNT] PROGMEM = {
    {0, LED_READY}, {1, LED_READY}, {2, LED_DISCH}, {3, LED_DISCH}, 
    {4, LED_BACKLIGHT}, {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, 
    {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, 
    {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT}, {15, LED_BACKLIGHT}, 
    {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, 
    {19, LED_BACKLIGHT}, {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, 
    {22, LED_BACKLIGHT}, {23, LED_BACKLIGHT}, {24, LED_BACKLIGHT}, 
    {25, LED_AG}, {26, LED_AG}, {27, LED_AA}, {28, LED_AA}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int MASTER_ARM_TEXT_COUNT = 3;  // Number of LEDs that need text
const LedText masterArmTextTable[MASTER_ARM_TEXT_COUNT] PROGMEM = {
    {18, "MASTER"},
    {20, "MASTER"},
    {21, "MASTER"}
};

/********************************************************************************************************************
 * @brief   Master Arm Panel class
 * @details Backlighting and indicator controller for the Master Arm panel.
 *          Total LEDs: 29
 *          Backlight LEDs: 21 (indices 4-24)
 *          Indicator LEDs: 8 (READY: 0-1, DISCH: 2-3, A/G: 25-26, A/A: 27-28)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class MasterArmPanel : public Panel {
public:
    static MasterArmPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new MasterArmPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    MasterArmPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = MASTER_ARM_LED_COUNT;
        ledTable = masterArmLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};
    
    static void onMcReadyChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_READY, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer mcReadyBuffer{FA_18C_hornet_MC_READY, onMcReadyChange};

    static void onMcDischChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_DISCH, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer mcDischBuffer{FA_18C_hornet_MC_DISCH, onMcDischChange};

    static void onMasterModeAgLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_AG, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer masterModeAgLtBuffer{FA_18C_hornet_MASTER_MODE_AG_LT, onMasterModeAgLtChange};

    static void onMasterModeAaLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_AA, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer masterModeAaLtBuffer{FA_18C_hornet_MASTER_MODE_AA_LT, onMasterModeAaLtChange};

    // Instance data
    static MasterArmPanel* instance;
};

// Initialize static instance pointer
MasterArmPanel* MasterArmPanel::instance = nullptr;

#endif