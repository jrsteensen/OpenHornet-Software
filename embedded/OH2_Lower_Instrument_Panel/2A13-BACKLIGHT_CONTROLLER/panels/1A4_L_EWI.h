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
 * @file      1A4_L_EWI_BL.h
 * @author    Ulukaii, Arribe, Higgins
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Left EWI panel.
 *            It consists of two parts:
 *            - An array with the LEDs and their roles (LED_FIRE, LED_CAUTION, LED_GO, LED_NO_GO, ...)
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef __L_EWI_BL_H
#define __L_EWI_BL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const int L_EWI_LED_COUNT = 30;  // Total number of LEDs in the panel
const Led lEwiLedTable[L_EWI_LED_COUNT] PROGMEM = {
    {0, LED_FIRE}, {1, LED_FIRE}, {2, LED_FIRE}, {3, LED_FIRE}, {4, LED_CAUTION}, 
    {5, LED_CAUTION}, {6, LED_CAUTION}, {7, LED_CAUTION}, {8, LED_GO}, {9, LED_GO},
    {10, LED_NO_GO}, {11, LED_NO_GO}, {12, LED_R_BLEED}, {13, LED_R_BLEED}, {14, LED_L_BLEED}, 
    {15, LED_L_BLEED}, {16, LED_SPD_BRK}, {17, LED_SPD_BRK}, {18, LED_STBY}, {19, LED_STBY},
    {20, LED_REC}, {21, LED_REC}, {22, LED_L_BAR1}, {23, LED_L_BAR1}, {24, LED_L_BAR2}, 
    {25, LED_L_BAR2}, {26, LED_XMIT}, {27, LED_XMIT}, {28, LED_ASPJ_ON}, {29, LED_ASPJ_ON}
};

/********************************************************************************************************************
 * @brief   Left EWI Panel class
 * @details Indicator controller for the Left EWI panel.
 *          Total LEDs: 30
 *          Indicator LEDs: 30 (FIRE: 0-3, CAUTION: 4-7, GO: 8-9, NO_GO: 10-11, R_BLEED: 12-13,
 *                             L_BLEED: 14-15, SPD_BRK: 16-17, STBY: 18-19, REC: 20-21,
 *                             L_BAR1: 22-23, L_BAR2: 24-25, XMIT: 26-27, ASPJ_ON: 28-29)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 * @see     Panel.h for the base class implementation
 ********************************************************************************************************************/
class EwiPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the EwiPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static EwiPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new EwiPanel(startIndex, ledStrip);
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
    EwiPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = L_EWI_LED_COUNT;
        ledTable = lEwiLedTable;
    }

    /**
     * @brief Callback for FIRE LEFT indicator
     * @param newValue The new state of the indicator (0 or 1)
     * @see This method is called by DCS-BIOS when the FIRE LEFT indicator state changes
     */
    static void onFireLeftLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_FIRE, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer fireLeftLtBuffer{FA_18C_hornet_FIRE_LEFT_LT, onFireLeftLtChange};

    static void onMasterCautionLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_CAUTION, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer masterCautionLtBuffer{FA_18C_hornet_MASTER_CAUTION_LT, onMasterCautionLtChange};

    static void onLhAdvAspjOhChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_ASPJ_ON, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvAspjOhBuffer{FA_18C_hornet_LH_ADV_ASPJ_OH, onLhAdvAspjOhChange};

    static void onLhAdvGoChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_GO, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvGoBuffer{FA_18C_hornet_LH_ADV_GO, onLhAdvGoChange};

    static void onLhAdvLBarGreenChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_L_BAR2, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBarGreenBuffer{FA_18C_hornet_LH_ADV_L_BAR_GREEN, onLhAdvLBarGreenChange};

    static void onLhAdvLBarRedChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_L_BAR1, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBarRedBuffer{FA_18C_hornet_LH_ADV_L_BAR_RED, onLhAdvLBarRedChange};

    static void onLhAdvLBleedChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_L_BLEED, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBleedBuffer{FA_18C_hornet_LH_ADV_L_BLEED, onLhAdvLBleedChange};

    static void onLhAdvNoGoChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_NO_GO, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvNoGoBuffer{FA_18C_hornet_LH_ADV_NO_GO, onLhAdvNoGoChange};

    static void onLhAdvRBleedChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_R_BLEED, newValue ? NVIS_RED : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvRBleedBuffer{FA_18C_hornet_LH_ADV_R_BLEED, onLhAdvRBleedChange};

    static void onLhAdvRecChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_REC, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvRecBuffer{FA_18C_hornet_LH_ADV_REC, onLhAdvRecChange};

    static void onLhAdvSpdBrkChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_SPD_BRK, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvSpdBrkBuffer{FA_18C_hornet_LH_ADV_SPD_BRK, onLhAdvSpdBrkChange};

    static void onLhAdvStbyChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_STBY, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvStbyBuffer{FA_18C_hornet_LH_ADV_STBY, onLhAdvStbyChange};

    static void onLhAdvXmitChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_XMIT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvXmitBuffer{FA_18C_hornet_LH_ADV_XMIT, onLhAdvXmitChange};

    // Instance data
    static EwiPanel* instance;
};

// Initialize static instance pointer
EwiPanel* EwiPanel::instance = nullptr;

#endif