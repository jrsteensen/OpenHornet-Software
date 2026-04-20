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
 * @file      2A2A1A8_STANDBY_INSTRUMENT.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting and indicators for the Standby Instruments panel.
 *            It consists of two parts:
 *            - An array with the LEDs and their roles (LED_INSTR_BL, LED_STBY_ATT, LED_STBY_HDG)
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef STANDBY_INSTRUMENT_PANEL_H
#define STANDBY_INSTRUMENT_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"
#include "../helpers/Colors.h"

// Total number of LEDs in the panel
const int STANDBY_INSTRUMENT_LED_COUNT = 6;

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const Led standbyInstrumentLedTable[STANDBY_INSTRUMENT_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL_CGRB}, {1, LED_INSTR_BL_CGRB}, {2, LED_INSTR_BL_CGRB},
    {3, LED_INSTR_BL_CGRB}, {4, LED_INSTR_BL_CGRB}, {5, LED_INSTR_BL_CGRB}
};

/********************************************************************************************************************
 * @brief   Standby Instrument Panel class
 * @details Backlighting controller for the Standby Instrument panel.
 *          Total LEDs: 6
 *          Backlight LEDs: 6 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class StandbyInstrumentPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the StandbyInstrumentPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static StandbyInstrumentPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new StandbyInstrumentPanel(startIndex, ledStrip);
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
    StandbyInstrumentPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = STANDBY_INSTRUMENT_LED_COUNT;
        ledTable = standbyInstrumentLedTable;
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
    static StandbyInstrumentPanel* instance;
};

// Initialize static instance pointer
StandbyInstrumentPanel* StandbyInstrumentPanel::instance = nullptr;

#endif