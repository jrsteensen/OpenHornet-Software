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
 * @file      4A1_RC_Flood.h
 * @author    Ulukaii
 * @date      12.10.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements flood lighting for the Right Console. Assumes all lights are connected to the same WS2812 strip.
 *********************************************************************************************************************/


#ifndef __RC_FLOOD_H
#define __RC_FLOOD_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RCF_LED_COUNT = 100;  // Total number of LEDs in the panel
const Led rcFloodLedTable[RCF_LED_COUNT] PROGMEM = {
    {0, LED_FLOOD}, {1, LED_FLOOD}, {2, LED_FLOOD}, {3, LED_FLOOD}, {4, LED_FLOOD}, 
    {5, LED_FLOOD}, {6, LED_FLOOD}, {7, LED_FLOOD}, {8, LED_FLOOD}, {9, LED_FLOOD},
    {10, LED_FLOOD}, {11, LED_FLOOD}, {12, LED_FLOOD}, {13, LED_FLOOD}, {14, LED_FLOOD}, 
    {15, LED_FLOOD}, {16, LED_FLOOD}, {17, LED_FLOOD}, {18, LED_FLOOD}, {19, LED_FLOOD},
    {20, LED_FLOOD}, {21, LED_FLOOD}, {22, LED_FLOOD}, {23, LED_FLOOD}, {24, LED_FLOOD}, 
    {25, LED_FLOOD}, {26, LED_FLOOD}, {27, LED_FLOOD}, {28, LED_FLOOD}, {29, LED_FLOOD},
    {30, LED_FLOOD}, {31, LED_FLOOD}, {32, LED_FLOOD}, {33, LED_FLOOD}, {34, LED_FLOOD}, 
    {35, LED_FLOOD}, {36, LED_FLOOD}, {37, LED_FLOOD}, {38, LED_FLOOD}, {39, LED_FLOOD},
    {40, LED_FLOOD}, {41, LED_FLOOD}, {42, LED_FLOOD}, {43, LED_FLOOD}, {44, LED_FLOOD}, 
    {45, LED_FLOOD}, {46, LED_FLOOD}, {47, LED_FLOOD}, {48, LED_FLOOD}, {49, LED_FLOOD},
    {50, LED_FLOOD}, {51, LED_FLOOD}, {52, LED_FLOOD}, {53, LED_FLOOD}, {54, LED_FLOOD}, 
    {55, LED_FLOOD}, {56, LED_FLOOD}, {57, LED_FLOOD}, {58, LED_FLOOD}, {59, LED_FLOOD},
    {60, LED_FLOOD}, {61, LED_FLOOD}, {62, LED_FLOOD}, {63, LED_FLOOD}, {64, LED_FLOOD}, 
    {65, LED_FLOOD}, {66, LED_FLOOD}, {67, LED_FLOOD}, {68, LED_FLOOD}, {69, LED_FLOOD},
    {70, LED_FLOOD}, {71, LED_FLOOD}, {72, LED_FLOOD}, {73, LED_FLOOD}, {74, LED_FLOOD}, 
    {75, LED_FLOOD}, {76, LED_FLOOD}, {77, LED_FLOOD}, {78, LED_FLOOD}, {79, LED_FLOOD},
    {80, LED_FLOOD}, {81, LED_FLOOD}, {82, LED_FLOOD}, {83, LED_FLOOD}, {84, LED_FLOOD}, 
    {85, LED_FLOOD}, {86, LED_FLOOD}, {87, LED_FLOOD}, {88, LED_FLOOD}, {89, LED_FLOOD},
    {90, LED_FLOOD}, {91, LED_FLOOD}, {92, LED_FLOOD}, {93, LED_FLOOD}, {94, LED_FLOOD}, 
    {95, LED_FLOOD}, {96, LED_FLOOD}, {97, LED_FLOOD}, {98, LED_FLOOD}, {99, LED_FLOOD}
};

/********************************************************************************************************************
 * @brief   Right Console Flood Lighting class
 * @details Backlighting controller for the Left Console flood lighting.
 *          Total LEDs: 100
 *          Backlight LEDs: 100 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class RcFloodLights : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the RcFloodLights class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static RcFloodLights* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new RcFloodLights(startIndex, ledStrip);
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
    RcFloodLights(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RCF_LED_COUNT;
        ledTable = rcFloodLedTable;
    }

    // Static callback functions for DCS-BIOS
    /**
     * @brief Callback for FLOOD DIMMER
     * @param newValue The new state of the indicator (0-100)
     * @see This method is called by DCS-BIOS when the FLOOD DIMMER state changes
     */
    static void onFloodDimmerChange(unsigned int newValue) {
        if (instance) instance->setFloodlights(newValue);
    }
    DcsBios::IntegerBuffer floodDimmerBuffer{FA_18C_hornet_FLOOD_DIMMER, onFloodDimmerChange};

    // Instance data
    static RcFloodLights* instance;
};

// Initialize static instance pointer
RcFloodLights* RcFloodLights::instance = nullptr;

#endif 