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
 * @file 2A2A1A8_STANDBY_INSTRUMENT.h
 * @author Ulukaii
 * @date 05.05.2025
 * @version u 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Implements backlighting and indicators for the Standby Instrument panel.
 *********************************************************************************************************************/


#ifndef STANDBY_INSTRUMENT_PANEL_H
#define STANDBY_INSTRUMENT_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

// Total number of LEDs in the panel
const int STANDBY_INSTRUMENT_LED_COUNT = 6;

// LED table definition
const Led standbyInstrumentLedTable[STANDBY_INSTRUMENT_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT, ""}, {1, LED_BACKLIGHT, ""}, {2, LED_BACKLIGHT, ""},
    {3, LED_BACKLIGHT, ""}, {4, LED_BACKLIGHT, ""}, {5, LED_BACKLIGHT, ""}
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
    static StandbyInstrumentPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new StandbyInstrumentPanel(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    StandbyInstrumentPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = STANDBY_INSTRUMENT_LED_COUNT;
        ledTable = standbyInstrumentLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static StandbyInstrumentPanel* instance;
};

// Initialize static instance pointer
StandbyInstrumentPanel* StandbyInstrumentPanel::instance = nullptr;

#endif