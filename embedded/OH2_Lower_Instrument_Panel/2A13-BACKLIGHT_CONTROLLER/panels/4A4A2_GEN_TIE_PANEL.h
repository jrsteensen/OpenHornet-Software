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
 * @file      4A4A2_GEN_TIE_PANEL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Gen Tie Panel.
 *********************************************************************************************************************/


#ifndef __GEN_TIE_PANEL_H
#define __GEN_TIE_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int GEN_TIE_PANEL_LED_COUNT = 9;  // Total number of LEDs in the panel
const Led genTiePanelLedTable[GEN_TIE_PANEL_LED_COUNT] PROGMEM = {
    {0, LED_CONSOLE_BL}, {1, LED_CONSOLE_BL}, {2, LED_CONSOLE_BL}, {3, LED_CONSOLE_BL}, {4, LED_CONSOLE_BL}, 
    {5, LED_CONSOLE_BL}, {6, LED_CONSOLE_BL}, {7, LED_CONSOLE_BL}, {8, LED_CONSOLE_BL}
};

/********************************************************************************************************************
 * @brief   Gen Tie Panel class
 * @details Backlighting controller for the Gen Tie Panel.
 *          Total LEDs: 9
 *          Backlight LEDs: 9 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class GenTiePanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the GenTiePanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static GenTiePanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new GenTiePanel(startIndex, ledStrip);
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
    GenTiePanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = GEN_TIE_PANEL_LED_COUNT;
        ledTable = genTiePanelLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static GenTiePanel* instance;
};

// Initialize static instance pointer
GenTiePanel* GenTiePanel::instance = nullptr;

#endif

