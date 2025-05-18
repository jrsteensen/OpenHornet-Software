#ifndef __SELECT_JETT_PANEL_H
#define __SELECT_JETT_PANEL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int SELECT_JETT_LED_COUNT = 81;  // Total number of LEDs in the panel
const Led selectJettLedTable[SELECT_JETT_LED_COUNT] PROGMEM = {
    {0, LED_BACKLIGHT}, {1, LED_BACKLIGHT}, {2, LED_BACKLIGHT}, {3, LED_BACKLIGHT}, {4, LED_BACKLIGHT},
    {5, LED_BACKLIGHT}, {6, LED_BACKLIGHT}, {7, LED_BACKLIGHT}, {8, LED_BACKLIGHT}, {9, LED_BACKLIGHT},
    {10, LED_BACKLIGHT}, {11, LED_BACKLIGHT}, {12, LED_BACKLIGHT}, {13, LED_BACKLIGHT}, {14, LED_BACKLIGHT},
    {15, LED_BACKLIGHT}, {16, LED_BACKLIGHT}, {17, LED_BACKLIGHT}, {18, LED_BACKLIGHT}, {19, LED_BACKLIGHT},
    {20, LED_BACKLIGHT}, {21, LED_BACKLIGHT}, {22, LED_BACKLIGHT}, {23, LED_BACKLIGHT}, {24, LED_BACKLIGHT},
    {25, LED_BACKLIGHT}, {26, LED_BACKLIGHT}, {27, LED_BACKLIGHT}, {28, LED_BACKLIGHT}, {29, LED_BACKLIGHT},
    {30, LED_BACKLIGHT}, {31, LED_BACKLIGHT}, {32, LED_BACKLIGHT}, {33, LED_BACKLIGHT}, {34, LED_BACKLIGHT},
    {35, LED_BACKLIGHT}, {36, LED_BACKLIGHT}, {37, LED_BACKLIGHT}, {38, LED_BACKLIGHT}, {39, LED_BACKLIGHT},
    {40, LED_BACKLIGHT}, {41, LED_BACKLIGHT}, {42, LED_BACKLIGHT}, {43, LED_BACKLIGHT}, {44, LED_BACKLIGHT},
    {45, LED_BACKLIGHT}, {46, LED_BACKLIGHT}, {47, LED_BACKLIGHT}, {48, LED_BACKLIGHT}, {49, LED_BACKLIGHT},
    {50, LED_BACKLIGHT}, {51, LED_BACKLIGHT}, {52, LED_BACKLIGHT}, {53, LED_BACKLIGHT}, {54, LED_BACKLIGHT},
    {55, LED_BACKLIGHT}, {56, LED_BACKLIGHT}, {57, LED_BACKLIGHT}, {58, LED_BACKLIGHT}, {59, LED_BACKLIGHT},
    {60, LED_BACKLIGHT}, {61, LED_BACKLIGHT}, {62, LED_BACKLIGHT}, {63, LED_BACKLIGHT}, {64, LED_BACKLIGHT},
    {65, LED_BACKLIGHT}, {66, LED_BACKLIGHT}, {67, LED_BACKLIGHT}, {68, LED_BACKLIGHT}, {69, LED_BACKLIGHT},
    {70, LED_BACKLIGHT}, {71, LED_BACKLIGHT}, {72, LED_BACKLIGHT}, {73, LED_BACKLIGHT}, {74, LED_BACKLIGHT},
    {75, LED_BACKLIGHT}, {76, LED_BACKLIGHT}, {77, LED_BACKLIGHT}, {78, LED_BACKLIGHT}, {79, LED_BACKLIGHT},
    {80, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   Select Jettison Panel class
 * @details Backlighting controller for the Select Jettison panel.
 *          Total LEDs: 81
 *          Backlight LEDs: 81 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class SelectJettPanel : public Panel {
public:
    static SelectJettPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new SelectJettPanel(startIndex, ledStrip);
        }
        return instance;
    }

    int getLedCount() const override {
        return SELECT_JETT_LED_COUNT;
    }

    const Led* getLedTable() const override {
        return selectJettLedTable;
    }

private:
    // Private constructor
    SelectJettPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = SELECT_JETT_LED_COUNT;
        ledTable = selectJettLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    // Instance data
    static SelectJettPanel* instance;
};

// Initialize static instance pointer
SelectJettPanel* SelectJettPanel::instance = nullptr;

#endif 