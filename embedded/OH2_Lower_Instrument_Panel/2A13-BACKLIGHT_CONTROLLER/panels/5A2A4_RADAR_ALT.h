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
#include "../helpers/Colors.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RADAR_ALT_LED_COUNT = 2;  // Total number of LEDs in the panel
const Led radarAltLedTable[RADAR_ALT_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL_CGRB}, {1, LED_INSTR_BL_CGRB}
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
    /**
     * @brief Gets the singleton instance of the RadarAltPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static RadarAltPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new RadarAltPanel(startIndex, ledStrip);
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
    RadarAltPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RADAR_ALT_LED_COUNT;
        ledTable = radarAltLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        int scale = map(newValue, 0, 65535, 0, 255);                  // Map the brightness scale factor to a range of 0-255
        CRGB target = NVIS_CGRB_GREEN_A;                              // Use NVIS_CGRB_GREEN_A for GRB LEDs
        target.nscale8_video(scale);                                  // Use FastLED's nscale8_video to apply the scale factor
        if (instance) instance->setIndicatorColor(LED_INSTR_BL_CGRB, target);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};



    // Instance data
    static RadarAltPanel* instance;
};

// Initialize static instance pointer
RadarAltPanel* RadarAltPanel::instance = nullptr;

#endif 