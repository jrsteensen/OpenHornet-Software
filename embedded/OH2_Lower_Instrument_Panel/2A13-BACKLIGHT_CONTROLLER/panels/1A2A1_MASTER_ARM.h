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
 *            It consists of three parts:
 *            - An array with the LEDs and their roles (LED_READY, LED_DISCH, LED_AG, LED_AA, ...)
 *            - An array with optional legend text for specific LEDs
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef __MASTER_ARM_BL_H
#define __MASTER_ARM_BL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"



/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const int MASTER_ARM_LED_COUNT = 29;  // Total number of LEDs in the panel
const Led masterArmLedTable[MASTER_ARM_LED_COUNT] PROGMEM = {
    {0, LED_READY}, {1, LED_READY}, {2, LED_DISCH}, {3, LED_DISCH}, 
    {4, LED_INSTR_BL}, {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, 
    {7, LED_INSTR_BL}, {8, LED_INSTR_BL}, {9, LED_INSTR_BL},
    {10, LED_INSTR_BL}, {11, LED_INSTR_BL}, {12, LED_INSTR_BL}, 
    {13, LED_INSTR_BL}, {14, LED_INSTR_BL}, {15, LED_INSTR_BL}, 
    {16, LED_INSTR_BL}, {17, LED_INSTR_BL}, {18, LED_INSTR_BL}, 
    {19, LED_INSTR_BL}, {20, LED_INSTR_BL}, {21, LED_INSTR_BL}, 
    {22, LED_INSTR_BL}, {23, LED_INSTR_BL}, {24, LED_INSTR_BL}, 
    {25, LED_AG}, {26, LED_AG}, {27, LED_AA}, {28, LED_AA}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedStruct.h for the Led structure.
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
    /**
     * @brief Gets the singleton instance of the MasterArmPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by Channel::addPanel() when adding the Master Arm panel to a channel
     */
    static MasterArmPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new MasterArmPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @see This method is called by the public getInstance() if and only if not instance exists yet.
     */
    MasterArmPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = MASTER_ARM_LED_COUNT;
        ledTable = masterArmLedTable;
    }

    /**
     * @brief Callback for Master Caution Ready light changes from DCS-BIOS
     * @param newValue The new state of the Ready light (0 or 1)
     * @see This method is called by DCS-BIOS when the Ready light state changes
     */
    static void onMcReadyChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_READY, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer mcReadyBuffer{FA_18C_hornet_MC_READY, onMcReadyChange};

    /**
     * @brief Callback for Master Caution Discharge light changes from DCS-BIOS
     * @param newValue The new state of the Discharge light (0 or 1)
     * @see This method is called by DCS-BIOS when the Discharge light state changes
     */
    static void onMcDischChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_DISCH, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer mcDischBuffer{FA_18C_hornet_MC_DISCH, onMcDischChange};

    /**
     * @brief Callback for Master Mode A/G light changes from DCS-BIOS
     * @param newValue The new state of the A/G light (0 or 1)
     * @see This method is called by DCS-BIOS when the A/G light state changes
     */
    static void onMasterModeAgLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_AG, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer masterModeAgLtBuffer{FA_18C_hornet_MASTER_MODE_AG_LT, onMasterModeAgLtChange};

    /**
     * @brief Callback for Master Mode A/A light changes from DCS-BIOS
     * @param newValue The new state of the A/A light (0 or 1)
     * @see This method is called by DCS-BIOS when the A/A light state changes
     */
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