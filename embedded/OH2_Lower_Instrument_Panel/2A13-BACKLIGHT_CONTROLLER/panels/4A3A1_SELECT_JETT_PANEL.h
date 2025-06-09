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
 * @file      4A3A1_SELECT_JETT_PANEL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting and indicators for the Select Jettison panel.
 *********************************************************************************************************************/

#ifndef __SELECT_JETT_PANEL_H
#define __SELECT_JETT_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int SELECT_JETT_LED_COUNT = 81;  // Total number of LEDs in the panel
const Led selectJettLedTable[SELECT_JETT_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL}, {4, LED_INSTR_BL},
    {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, {7, LED_INSTR_BL}, {8, LED_INSTR_BL}, {9, LED_INSTR_BL},
    {10, LED_INSTR_BL}, {11, LED_INSTR_BL}, {12, LED_INSTR_BL}, {13, LED_INSTR_BL}, {14, LED_INSTR_BL},
    {15, LED_INSTR_BL}, {16, LED_INSTR_BL}, {17, LED_INSTR_BL}, {18, LED_INSTR_BL}, {19, LED_INSTR_BL},
    {20, LED_INSTR_BL}, {21, LED_INSTR_BL}, {22, LED_INSTR_BL}, {23, LED_INSTR_BL}, {24, LED_INSTR_BL},
    {25, LED_INSTR_BL}, {26, LED_INSTR_BL}, {27, LED_INSTR_BL}, {28, LED_INSTR_BL}, {29, LED_INSTR_BL},
    {30, LED_INSTR_BL}, {31, LED_INSTR_BL}, {32, LED_INSTR_BL}, {33, LED_INSTR_BL}, {34, LED_INSTR_BL},
    {35, LED_INSTR_BL}, {36, LED_INSTR_BL}, {37, LED_INSTR_BL}, {38, LED_INSTR_BL}, {39, LED_INSTR_BL},
    {40, LED_INSTR_BL}, {41, LED_INSTR_BL}, {42, LED_INSTR_BL}, {43, LED_INSTR_BL}, {44, LED_INSTR_BL},
    {45, LED_INSTR_BL}, {46, LED_INSTR_BL}, {47, LED_INSTR_BL}, {48, LED_INSTR_BL}, {49, LED_INSTR_BL},
    {50, LED_INSTR_BL}, {51, LED_INSTR_BL}, {52, LED_INSTR_BL}, {53, LED_INSTR_BL}, {54, LED_INSTR_BL},
    {55, LED_INSTR_BL}, {56, LED_INSTR_BL}, {57, LED_INSTR_BL}, {58, LED_INSTR_BL}, {59, LED_INSTR_BL},
    {60, LED_INSTR_BL}, {61, LED_INSTR_BL}, {62, LED_INSTR_BL}, {63, LED_INSTR_BL}, {64, LED_INSTR_BL},
    {65, LED_INSTR_BL}, {66, LED_INSTR_BL}, {67, LED_INSTR_BL}, {68, LED_INSTR_BL}, {69, LED_INSTR_BL},
    {70, LED_INSTR_BL}, {71, LED_INSTR_BL}, {72, LED_INSTR_BL}, {73, LED_INSTR_BL}, {74, LED_INSTR_BL},
    {75, LED_INSTR_BL}, {76, LED_INSTR_BL}, {77, LED_INSTR_BL}, {78, LED_INSTR_BL}, {79, LED_INSTR_BL},
    {80, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   Select Jettison Panel class
 * @details Backlighting controller for the Select Jettison panel.
 *          Total LEDs: 81
 *          Backlight LEDs: 81 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class SelectJettPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the SelectJettPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static SelectJettPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new SelectJettPanel(startIndex, ledStrip);
        }
        return instance;
    }

    int getLedCount() const override {
        return SELECT_JETT_LED_COUNT;
    }

    const Led* getLedTable() const override {
        return selectJettLedTable;
    }

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @see This method is called by the public getInstance() if and only if no instance exists yet
     */
    SelectJettPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = SELECT_JETT_LED_COUNT;
        ledTable = selectJettLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static SelectJettPanel* instance;
};

// Initialize static instance pointer
SelectJettPanel* SelectJettPanel::instance = nullptr;

#endif 