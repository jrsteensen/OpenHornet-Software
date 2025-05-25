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
 * @file      5A2A4_RADAR_ALT.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   u 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Radar Altimeter panel.
 *********************************************************************************************************************/


#ifndef __RADAR_ALT_H
#define __RADAR_ALT_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RADAR_ALT_LED_COUNT = 2;  // Total number of LEDs in the panel
const Led radarAltLedTable[RADAR_ALT_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RADAR_ALT_TEXT_COUNT = 0;  // Number of LEDs that need text
const LedText radarAltTextTable[RADAR_ALT_TEXT_COUNT] PROGMEM = {
    // No text needed for this panel
};

/********************************************************************************************************************
 * @brief   Radar Altimeter Panel class
 * @details Backlighting controller for the Radar Altimeter panel.
 *          Total LEDs: 2
 *          Backlight LEDs: 2 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class RadarAltPanel : public Panel {
public:
    static RadarAltPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new RadarAltPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    RadarAltPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RADAR_ALT_LED_COUNT;
        ledTable = radarAltLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    // Instance data
    static RadarAltPanel* instance;
};

// Initialize static instance pointer
RadarAltPanel* RadarAltPanel::instance = nullptr;

#endif 