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
 * @file      2A2A1A4_Jett_Placard_Panel.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Jett Placard panel.
 *            It consists of two parts:
 *            - An array with the LEDs and their roles (LED_INSTR_BL)
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef __JETT_PLACARD_PANEL_H
#define __JETT_PLACARD_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const int JETT_PLACARD_LED_COUNT = 8;  // Total number of LEDs in the panel
const Led jettPlacardLedTable[JETT_PLACARD_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL},
    {4, LED_INSTR_BL}, {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, {7, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   Jett Placard Panel class
 * @details Backlighting controller for the Jett Placard panel.
 *          Total LEDs: 8
 *          Backlight LEDs: 8 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 * @see     Panel.h for the base class implementation
 ********************************************************************************************************************/
class JettPlacardPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the JettPlacardPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static JettPlacardPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new JettPlacardPanel(startIndex, ledStrip);
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
    JettPlacardPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = JETT_PLACARD_LED_COUNT;
        ledTable = jettPlacardLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static JettPlacardPanel* instance;
};

// Initialize static instance pointer
JettPlacardPanel* JettPlacardPanel::instance = nullptr;

#endif 