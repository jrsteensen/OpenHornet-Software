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
 * @file      4A4A1_FIRE_TEST_PANEL.h
 * @author    Ulukaii
 * @date      04.11.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Fire Test Panel.
 *********************************************************************************************************************/


#ifndef __FIRE_TEST_PANEL_H
#define __FIRE_TEST_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int FIRE_TEST_PANEL_LED_COUNT = 10;  // Total number of LEDs in the panel
const Led fireTestPanelLedTable[FIRE_TEST_PANEL_LED_COUNT] PROGMEM = {
    {0, LED_CONSOLE_BL}, {1, LED_CONSOLE_BL}, {2, LED_CONSOLE_BL}, {3, LED_CONSOLE_BL}, {4, LED_CONSOLE_BL}, 
    {5, LED_CONSOLE_BL}, {6, LED_CONSOLE_BL}, {7, LED_CONSOLE_BL}, {8, LED_CONSOLE_BL}, {9, LED_CONSOLE_BL}
};

/********************************************************************************************************************
 * @brief   Fire Test Panel class
 * @details Backlighting controller for the Fire Test Panel.
 *          Total LEDs: 10
 *          Backlight LEDs: 10 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class FireTestPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the FireTestPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static FireTestPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new FireTestPanel(startIndex, ledStrip);
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
    FireTestPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = FIRE_TEST_PANEL_LED_COUNT;
        ledTable = fireTestPanelLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static FireTestPanel* instance;
};

// Initialize static instance pointer
FireTestPanel* FireTestPanel::instance = nullptr;

#endif

