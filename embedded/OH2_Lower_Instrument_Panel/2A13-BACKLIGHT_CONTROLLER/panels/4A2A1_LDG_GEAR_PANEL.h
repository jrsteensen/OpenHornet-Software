#ifndef __LDG_GEAR_PANEL_H
#define __LDG_GEAR_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LDG_GEAR_LED_COUNT = 23;  // Total number of LEDs in the panel
const Led ldgGearLedTable[LDG_GEAR_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT},
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT},
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   Landing Gear Panel class
 * @details Backlighting controller for the Landing Gear panel.
 *          Total LEDs: 23
 *          Backlight LEDs: 23 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class LdgGearPanel : public Panel {
public:
    static LdgGearPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new LdgGearPanel(startIndex, ledStrip);
        }
        return instance;
    }

    int getLedCount() const override {
        return LDG_GEAR_LED_COUNT;
    }

    const Led* getLedTable() const override {
        return ldgGearLedTable;
    }

private:
    // Private constructor
    LdgGearPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LDG_GEAR_LED_COUNT;
        ledTable = ldgGearLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static LdgGearPanel* instance;
};

// Initialize static instance pointer
LdgGearPanel* LdgGearPanel::instance = nullptr;

#endif 