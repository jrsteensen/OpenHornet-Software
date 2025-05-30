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
 * @file      2A2A1A3_Video_Record_Panel.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for the Video Record panel.
 *********************************************************************************************************************/


#ifndef __VIDEO_RECORD_PANEL_H
#define __VIDEO_RECORD_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int VIDEO_RECORD_LED_COUNT = 16;  // Total number of LEDs in the panel
const Led videoRecordLedTable[VIDEO_RECORD_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL}, {4, LED_INSTR_BL}, 
    {5, LED_INSTR_BL}, {6, LED_INSTR_BL}, {7, LED_INSTR_BL}, {8, LED_INSTR_BL}, {9, LED_INSTR_BL},
    {10, LED_INSTR_BL}, {11, LED_INSTR_BL}, {12, LED_INSTR_BL}, {13, LED_INSTR_BL}, {14, LED_INSTR_BL}, 
    {15, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   Video Record Panel class
 * @details Backlighting controller for the Video Record panel.
 *          Total LEDs: 16
 *          Backlight LEDs: 16 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class VideoRecordPanel : public Panel {
public:
    static VideoRecordPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new VideoRecordPanel(startIndex, ledStrip);
        }
        return instance;
    }

    // Explicitly override virtual methods from base class
    //int getStartIndex() const override { return panelStartIndex; }
    //int getLedCount() const override { return ledCount; }
    //const Led* getLedTable() const override { return ledTable; }
    //CRGB* getLedStrip() const override { return ledStrip; }

private:
    // Private constructor
    VideoRecordPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = VIDEO_RECORD_LED_COUNT;
        ledTable = videoRecordLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static VideoRecordPanel* instance;
};

// Initialize static instance pointer
VideoRecordPanel* VideoRecordPanel::instance = nullptr;

#endif 