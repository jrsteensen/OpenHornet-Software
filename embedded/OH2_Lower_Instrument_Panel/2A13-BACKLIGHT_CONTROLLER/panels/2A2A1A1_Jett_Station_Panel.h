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
 * @file      2A2A1A1_Jett_Station_Panel.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements indicators for the Jett Station panel.
 *            It consists of two parts:
 *            - An array with the LEDs and their roles (LED_JETT_RO_1, LED_JETT_RI_1, LED_JETT_CTR_1, ...)
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef __JETT_STATION_PANEL_H
#define __JETT_STATION_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const int JETT_STATION_LED_COUNT = 32;  // Total number of LEDs in the panel
const Led jettStationLedTable[JETT_STATION_LED_COUNT] PROGMEM = {
    // RO (Right Outer) Station LEDs
    {0, LED_JETT_RO_1}, {1, LED_JETT_RO_1}, {2, LED_JETT_RO_2}, {3, LED_JETT_RO_2},
    // RI (Right Inner) Station LEDs
    {4, LED_JETT_RI_1}, {5, LED_JETT_RI_1}, {6, LED_JETT_RI_2}, {7, LED_JETT_RI_2},
    // CTR (Center) Station LEDs
    {8, LED_JETT_CTR_1}, {9, LED_JETT_CTR_1}, {10, LED_JETT_CTR_2}, {11, LED_JETT_CTR_2},
    // LI (Left Inner) Station LEDs
    {12, LED_JETT_LI_1}, {13, LED_JETT_LI_1}, {14, LED_JETT_LI_2}, {15, LED_JETT_LI_2},
    // LO (Left Outer) Station LEDs
    {16, LED_JETT_LO_1}, {17, LED_JETT_LO_1}, {18, LED_JETT_LO_2}, {19, LED_JETT_LO_2},
    // Nose Station LED
    {20, LED_JETT_NOSE}, {31, LED_JETT_NOSE},
    // Left/Right Station LEDs
    {23, LED_JETT_LEFT}, {24, LED_JETT_LEFT}, {21, LED_JETT_RIGHT}, {22, LED_JETT_RIGHT},
    // Half/Full Station LEDs
    {25, LED_JETT_HALF}, {26, LED_JETT_HALF}, {27, LED_JETT_FULL}, {28, LED_JETT_FULL},
    // Flaps Station LEDs
    {29, LED_JETT_FLAPS}, {30, LED_JETT_FLAPS}
};

/********************************************************************************************************************
 * @brief   Jett Station Panel class
 * @details Indicator controller for the Jett Station panel.
 *          Total LEDs: 32
 *          Indicator LEDs: 32 (various jettison station indicators)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 * @see     Panel.h for the base class implementation
 ********************************************************************************************************************/
class JettStationPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the JettStationPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static JettStationPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new JettStationPanel(startIndex, ledStrip);
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
    JettStationPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = JETT_STATION_LED_COUNT;
        ledTable = jettStationLedTable;
    }

    // Static callback functions for DCS-BIOS

    // STATION JETTISON SELECT panel indicators
    static void onSjRoLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_RO_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjRoLtBuffer{FA_18C_hornet_SJ_RO_LT, onSjRoLtChange};

    static void onSjRiLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_RI_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjRiLtBuffer{FA_18C_hornet_SJ_RI_LT, onSjRiLtChange};

    static void onSjCtrLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_CTR_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjCtrLtBuffer{FA_18C_hornet_SJ_CTR_LT, onSjCtrLtChange};

    static void onSjLiLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LI_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjLiLtBuffer{FA_18C_hornet_SJ_LI_LT, onSjLiLtChange};

    static void onSjLoLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LO_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjLoLtBuffer{FA_18C_hornet_SJ_LO_LT, onSjLoLtChange};

    // STORES INDICATOR panel
    static void onFlpLgNoseGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_NOSE, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgNoseGearLtBuffer{FA_18C_hornet_FLP_LG_NOSE_GEAR_LT, onFlpLgNoseGearLtChange};

    static void onFlpLgRightGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_RIGHT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgRightGearLtBuffer{FA_18C_hornet_FLP_LG_RIGHT_GEAR_LT, onFlpLgRightGearLtChange};

    static void onFlpLgLeftGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LEFT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgLeftGearLtBuffer{FA_18C_hornet_FLP_LG_LEFT_GEAR_LT, onFlpLgLeftGearLtChange};

    static void onFlpLgHalfFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_HALF, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgHalfFlapsLtBuffer{FA_18C_hornet_FLP_LG_HALF_FLAPS_LT, onFlpLgHalfFlapsLtChange};

    static void onFlpLgFullFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_FULL, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgFullFlapsLtBuffer{FA_18C_hornet_FLP_LG_FULL_FLAPS_LT, onFlpLgFullFlapsLtChange};

    static void onFlpLgFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_FLAPS, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgFlapsLtBuffer{FA_18C_hornet_FLP_LG_FLAPS_LT, onFlpLgFlapsLtChange};

    // Instance data
    static JettStationPanel* instance;
};

// Initialize static instance pointer
JettStationPanel* JettStationPanel::instance = nullptr;

#endif 