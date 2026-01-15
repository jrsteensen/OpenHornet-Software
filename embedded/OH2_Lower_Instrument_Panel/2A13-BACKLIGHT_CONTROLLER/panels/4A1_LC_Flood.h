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
 * @file      4A1_LC_Flood.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements flood lighting for the Left Console. Assumes all lights are connected to the same WS2812 strip.
 *********************************************************************************************************************/


#ifndef __LC_FLOOD_H
#define __LC_FLOOD_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LCF_LED_COUNT = 40;  // Total number of LEDs in the panel
const Led lcFloodLedTable[LCF_LED_COUNT] PROGMEM = {
    {0, LED_FLOOD}, {1, LED_FLOOD}, {2, LED_FLOOD}, {3, LED_FLOOD}, {4, LED_FLOOD}, 
    {5, LED_FLOOD}, {6, LED_FLOOD}, {7, LED_FLOOD}, {8, LED_FLOOD}, {9, LED_FLOOD},
    {10, LED_FLOOD}, {11, LED_FLOOD}, {12, LED_FLOOD}, {13, LED_FLOOD}, {14, LED_FLOOD}, 
    {15, LED_FLOOD}, {16, LED_FLOOD}, {17, LED_FLOOD}, {18, LED_FLOOD}, {19, LED_FLOOD},
    {20, LED_FLOOD}, {21, LED_FLOOD}, {22, LED_FLOOD}, {23, LED_FLOOD}, {24, LED_FLOOD}, 
    {25, LED_FLOOD}, {26, LED_FLOOD}, {27, LED_FLOOD}, {28, LED_FLOOD}, {29, LED_FLOOD},
    {30, LED_FLOOD}, {31, LED_FLOOD}, {32, LED_FLOOD}, {33, LED_FLOOD}, {34, LED_FLOOD}, 
    {35, LED_FLOOD}, {36, LED_FLOOD}, {37, LED_FLOOD}, {38, LED_FLOOD}, {39, LED_FLOOD}
};

/********************************************************************************************************************
 * @brief   Left Console Flood Lighting class
 * @details Backlighting controller for the Left Console flood lighting.
 *          Total LEDs: 55
 *          Backlight LEDs: 55 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class LcFloodLights : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the LcFloodLights class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static LcFloodLights* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new LcFloodLights(startIndex, ledStrip);
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
    LcFloodLights(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LCF_LED_COUNT;
        ledTable = lcFloodLedTable;
    }

    // Instance data
    // Note: Flood dimmer callback is handled centrally in Board.h
    static LcFloodLights* instance;
};

// Initialize static instance pointer
LcFloodLights* LcFloodLights::instance = nullptr;

#endif 