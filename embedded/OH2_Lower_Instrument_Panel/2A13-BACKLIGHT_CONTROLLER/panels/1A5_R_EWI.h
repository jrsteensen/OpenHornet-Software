/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *   Copyright 2016-2024 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *   ----------------------------------------------------------------------------------
 *   Note: All other portions of OpenHornet not within the 'OpenHornet-Software' 
 *   GitHub repository is released under the Creative Commons Attribution -
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/
/**
 * @file 1A5_R_EWI_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware.
 * @brief Implements backlighting and indicators for the Right EWI panel.
*/

#ifndef __R_EWI_BL_H
#define __R_EWI_BL_H

#include "DcsBios.h"
#include "Panel.h"
#include "FastLED.h"
#include "Colors.h"
#include <avr/pgmspace.h>


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Type" in this context refers to the LED type enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int R_EWI_LED_COUNT = 30;  // Total number of LEDs in the panel
const LedInfo rEwiLedIndicesTable[R_EWI_LED_COUNT] PROGMEM = {
    {0, LED_R_FIRE}, {1, LED_R_FIRE}, {2, LED_R_FIRE}, {3, LED_R_FIRE}, {4, LED_APU_FIRE}, 
    {5, LED_APU_FIRE}, {6, LED_APU_FIRE}, {7, LED_APU_FIRE}, {8, LED_DISP}, {9, LED_DISP},
    {10, LED_RCDRON}, {11, LED_RCDRON}, {12, LED_BLANK1}, {13, LED_BLANK1}, {14, LED_BLANK2}, 
    {15, LED_BLANK2}, {16, LED_BLANK3}, {17, LED_BLANK3}, {18, LED_BLANK4}, {19, LED_BLANK4},
    {20, LED_BLANK5}, {21, LED_BLANK5}, {22, LED_SAM}, {23, LED_SAM}, {24, LED_AAA}, 
    {25, LED_AAA}, {26, LED_AI}, {27, LED_AI}, {28, LED_CW}, {29, LED_CW}
};

/********************************************************************************************************************
 * @brief   Right EWI Panel class
 * @details Backlighting and indicator controller for the Right EWI panel.
 *          Total LEDs: 30
 *          Indicator LEDs: 30 (FIRE: 0-3, APU FIRE: 4-7, DISP: 8-9, RCDRON ON: 10-11, L BLANK: 12-13,
 *                             R BLANK: 14-15, R BLANK2: 16-17, L BLANK2: 18-19, L BLANK3: 20-21,
 *                             SAM: 22-23, AAA: 24-25, AI: 26-27, CW: 28-29)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class REwiPanel : public Panel<REwiPanel> {
public:
    // No need for instance declaration or getInstance() - they're now in the base class

private:
    // Constructor now just needs to set up the panel-specific data
    REwiPanel(int startIndex, CRGB* ledArray) : Panel(startIndex, ledArray) {
        ledIndicesTable = rEwiLedIndicesTable;
        ledCount = R_EWI_LED_COUNT;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    static void onFireRightLtChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_R_FIRE, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer fireRightLtBuffer{0x740c, 0x0010, 4, onFireRightLtChange};

    static void onFireApuLtChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_APU_FIRE, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer fireApuLtBuffer{0x740c, 0x0004, 2, onFireApuLtChange};

    static void onRhAdvAaaChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_AAA, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvAaaBuffer{0x740a, 0x0800, 11, onRhAdvAaaChange};

    static void onRhAdvAiChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_AI, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvAiBuffer{0x740a, 0x0400, 10, onRhAdvAiChange};

    static void onRhAdvCwChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_CW, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvCwBuffer{0x740a, 0x1000, 12, onRhAdvCwChange};

    static void onRhAdvDispChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_DISP, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvDispBuffer{0x740a, 0x0100, 8, onRhAdvDispChange};

    static void onRhAdvRcdrOnChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_RCDRON, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvRcdrOnBuffer{0x740a, 0x0080, 7, onRhAdvRcdrOnChange};

    static void onRhAdvSamChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_SAM, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSamBuffer{0x740a, 0x0200, 9, onRhAdvSamChange};

    static void onRhAdvSpareRh1Change(unsigned int newValue) {
        instance->setIndicatorColor(LED_BLANK1, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh1Buffer{0x740a, 0x2000, 13, onRhAdvSpareRh1Change};

    static void onRhAdvSpareRh2Change(unsigned int newValue) {
        instance->setIndicatorColor(LED_BLANK2, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh2Buffer{0x740a, 0x4000, 14, onRhAdvSpareRh2Change};

    static void onRhAdvSpareRh3Change(unsigned int newValue) {
        instance->setIndicatorColor(LED_BLANK3, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh3Buffer{0x740a, 0x8000, 15, onRhAdvSpareRh3Change};

    static void onRhAdvSpareRh4Change(unsigned int newValue) {
        instance->setIndicatorColor(LED_BLANK4, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh4Buffer{0x740c, 0x0001, 0, onRhAdvSpareRh4Change};

    static void onRhAdvSpareRh5Change(unsigned int newValue) {
        instance->setIndicatorColor(LED_BLANK5, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer rhAdvSpareRh5Buffer{0x740c, 0x0002, 1, onRhAdvSpareRh5Change};

    // Configuration data
    int panelStartIndex;                            // The starting index of the panel's LEDs
    int ledCount;                                   // Number of LEDs in the panel
};

#endif