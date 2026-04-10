/**********************************************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *  
 * @file      10A1A1_SIM_PWR_PANEL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Sim Pwr Panel.
 *********************************************************************************************************************/


#ifndef __SIM_PWR_PANEL_H
#define __SIM_PWR_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int SIM_PWR_PANEL_LED_COUNT = 35;  // Total number of LEDs in the panel
const Led simPwrPanelLedTable[SIM_PWR_PANEL_LED_COUNT] PROGMEM = {
    {0, LED_CONSOLE_BL}, {1, LED_CONSOLE_BL}, {2, LED_CONSOLE_BL}, {3, LED_CONSOLE_BL}, {4, LED_CONSOLE_BL}, 
    {5, LED_CONSOLE_BL}, {6, LED_CONSOLE_BL}, {7, LED_CONSOLE_BL}, {8, LED_CONSOLE_BL}, {9, LED_CONSOLE_BL},
    {10, LED_CONSOLE_BL}, {11, LED_CONSOLE_BL}, {12, LED_CONSOLE_BL}, {13, LED_CONSOLE_BL}, {14, LED_CONSOLE_BL}, 
    {15, LED_CONSOLE_BL}, {16, LED_CONSOLE_BL}, {17, LED_CONSOLE_BL}, {18, LED_CONSOLE_BL}, {19, LED_CONSOLE_BL},
    {20, LED_CONSOLE_BL}, {21, LED_CONSOLE_BL}, {22, LED_CONSOLE_BL}, {23, LED_CONSOLE_BL}, {24, LED_CONSOLE_BL}, 
    {25, LED_CONSOLE_BL}, {26, LED_CONSOLE_BL}, {27, LED_CONSOLE_BL}, {28, LED_CONSOLE_BL}, {29, LED_CONSOLE_BL},
    {30, LED_CONSOLE_BL}, {31, LED_CONSOLE_BL}, {32, LED_CONSOLE_BL}, {33, LED_CONSOLE_BL}, {34, LED_CONSOLE_BL}
};

/********************************************************************************************************************
 * @brief   Sim Pwr Panel class
 * @details Backlighting controller for the Sim Pwr Panel.
 *          Total LEDs: 35
 *          Backlight LEDs: 35 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class SimPwrPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the SimPwrPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static SimPwrPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new SimPwrPanel(startIndex, ledStrip);
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
    SimPwrPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = SIM_PWR_PANEL_LED_COUNT;
        ledTable = simPwrPanelLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static SimPwrPanel* instance;
};

// Initialize static instance pointer
SimPwrPanel* SimPwrPanel::instance = nullptr;

#endif

