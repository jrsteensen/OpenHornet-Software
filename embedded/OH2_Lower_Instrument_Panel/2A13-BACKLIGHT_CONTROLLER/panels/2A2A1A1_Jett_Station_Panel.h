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
 *********************************************************************************************************************/


#ifndef __JETT_STATION_PANEL_H
#define __JETT_STATION_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
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
    {20, LED_JETT_NOSE},
    // Left/Right Station LEDs
    {21, LED_JETT_LEFT}, {22, LED_JETT_LEFT}, {23, LED_JETT_RIGHT}, {24, LED_JETT_RIGHT},
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
 ********************************************************************************************************************/
class JettStationPanel : public Panel {
public:
    static JettStationPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new JettStationPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
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
    DcsBios::IntegerBuffer sjRoLtBuffer{0x7430, 0x0400, 10, onSjRoLtChange};

    static void onSjRiLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_RI_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjRiLtBuffer{0x7430, 0x0200, 9, onSjRiLtChange};

    static void onSjCtrLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_CTR_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjCtrLtBuffer{0x742e, 0x4000, 14, onSjCtrLtChange};

    static void onSjLiLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LI_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjLiLtBuffer{0x742e, 0x8000, 15, onSjLiLtChange};

    static void onSjLoLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LO_1, newValue ? NVIS_WHITE : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer sjLoLtBuffer{0x7430, 0x0100, 8, onSjLoLtChange};

    // STORES INDICATOR panel
    static void onFlpLgNoseGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_NOSE, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgNoseGearLtBuffer{0x7430, 0x0800, 11, onFlpLgNoseGearLtChange};

    static void onFlpLgRightGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_RIGHT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgRightGearLtBuffer{0x7430, 0x2000, 13, onFlpLgRightGearLtChange};

    static void onFlpLgLeftGearLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_LEFT, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgLeftGearLtBuffer{0x7430, 0x1000, 12, onFlpLgLeftGearLtChange};

    static void onFlpLgHalfFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_HALF, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgHalfFlapsLtBuffer{0x7432, 0x4000, 14, onFlpLgHalfFlapsLtChange};

    static void onFlpLgFullFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_FULL, newValue ? NVIS_GREEN_A : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgFullFlapsLtBuffer{0x7432, 0x8000, 15, onFlpLgFullFlapsLtChange};

    static void onFlpLgFlapsLtChange(unsigned int newValue) {
        if (instance) instance->setIndicatorColor(LED_JETT_FLAPS, newValue ? NVIS_YELLOW : NVIS_BLACK);
    }
    DcsBios::IntegerBuffer flpLgFlapsLtBuffer{0x7466, 0x0001, 0, onFlpLgFlapsLtChange};

    // Instance data
    static JettStationPanel* instance;
};

// Initialize static instance pointer
JettStationPanel* JettStationPanel::instance = nullptr;

#endif 