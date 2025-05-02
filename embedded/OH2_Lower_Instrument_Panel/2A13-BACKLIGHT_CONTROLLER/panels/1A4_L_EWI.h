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
 * @file 1A4_L_EWI_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware.
 * @brief Implements indicators for the Left EWI panel.
*/

#ifndef __L_EWI_BL_H
#define __L_EWI_BL_H

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
const int L_EWI_LED_COUNT = 30;  // Total number of LEDs in the panel
const LedInfo lEwiLedIndicesTable[L_EWI_LED_COUNT] PROGMEM = {
    {0, LED_FIRE}, {1, LED_FIRE}, {2, LED_FIRE}, {3, LED_FIRE}, {4, LED_CAUTION}, 
    {5, LED_CAUTION}, {6, LED_CAUTION}, {7, LED_CAUTION}, {8, LED_GO}, {9, LED_GO},
    {10, LED_NO_GO}, {11, LED_NO_GO}, {12, LED_R_BLEED}, {13, LED_R_BLEED}, {14, LED_L_BLEED}, 
    {15, LED_L_BLEED}, {16, LED_SPD_BRK}, {17, LED_SPD_BRK}, {18, LED_STBY}, {19, LED_STBY},
    {20, LED_REC}, {21, LED_REC}, {22, LED_L_BAR1}, {23, LED_L_BAR1}, {24, LED_L_BAR2}, 
    {25, LED_L_BAR2}, {26, LED_XMIT}, {27, LED_XMIT}, {28, LED_ASPJ_ON}, {29, LED_ASPJ_ON}
};

/********************************************************************************************************************
 * @brief   Left EWI Panel class
 * @details Indicator controller for the Left EWI panel.
 *          Total LEDs: 30
 *          Indicator LEDs: 30 (FIRE: 0-3, CAUTION: 4-7, GO: 8-9, NO_GO: 10-11, R_BLEED: 12-13,
 *                             L_BLEED: 14-15, SPD_BRK: 16-17, STBY: 18-19, REC: 20-21,
 *                             L_BAR1: 22-23, L_BAR2: 24-25, XMIT: 26-27, ASPJ_ON: 28-29)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class EwiPanel : public Panel<EwiPanel> {
public:
    // No need for instance declaration or getInstance() - they're now in the base class

private:
    // Constructor now just needs to set up the panel-specific data
    EwiPanel(int startIndex, CRGB* ledArray) : Panel(startIndex, ledArray) {
        ledIndicesTable = lEwiLedIndicesTable;
        ledCount = L_EWI_LED_COUNT;
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{0x7560, 0xffff, 0, onInstrIntLtChange};

    static void onFireLeftLtChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_FIRE, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer fireLeftLtBuffer{0x7408, 0x0040, 6, onFireLeftLtChange};

    static void onMasterCautionLtChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_CAUTION, newValue ? COLOR_YELLOW : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer masterCautionLtBuffer{0x7408, 0x0200, 9, onMasterCautionLtChange};

    static void onLhAdvAspjOhChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_ASPJ_ON, newValue ? COLOR_YELLOW : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvAspjOhBuffer{0x740a, 0x0008, 3, onLhAdvAspjOhChange};

    static void onLhAdvGoChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_GO, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvGoBuffer{0x740a, 0x0010, 4, onLhAdvGoChange};

    static void onLhAdvLBarGreenChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_L_BAR2, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBarGreenBuffer{0x740a, 0x0002, 1, onLhAdvLBarGreenChange};

    static void onLhAdvLBarRedChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_L_BAR1, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBarRedBuffer{0x7408, 0x8000, 15, onLhAdvLBarRedChange};

    static void onLhAdvLBleedChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_L_BLEED, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvLBleedBuffer{0x7408, 0x0800, 11, onLhAdvLBleedChange};

    static void onLhAdvNoGoChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_NO_GO, newValue ? COLOR_YELLOW : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvNoGoBuffer{0x740a, 0x0020, 5, onLhAdvNoGoChange};

    static void onLhAdvRBleedChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_R_BLEED, newValue ? COLOR_RED : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvRBleedBuffer{0x7408, 0x1000, 12, onLhAdvRBleedChange};

    static void onLhAdvRecChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_REC, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvRecBuffer{0x740a, 0x0001, 0, onLhAdvRecChange};

    static void onLhAdvSpdBrkChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_SPD_BRK, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvSpdBrkBuffer{0x7408, 0x2000, 13, onLhAdvSpdBrkChange};

    static void onLhAdvStbyChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_STBY, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvStbyBuffer{0x7408, 0x4000, 14, onLhAdvStbyChange};

    static void onLhAdvXmitChange(unsigned int newValue) {
        instance->setIndicatorColor(LED_XMIT, newValue ? COLOR_GREEN : COLOR_BLACK);
    }
    DcsBios::IntegerBuffer lhAdvXmitBuffer{0x740a, 0x0004, 2, onLhAdvXmitChange};

    // Configuration data
    int panelStartIndex;                            // The starting index of the panel's LEDs
    int ledCount;                                   // Number of LEDs in the panel
};

#endif