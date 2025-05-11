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
 * @file 4A1_LC_ALL_PANELS.h
 * @author Ulukaii
 * @date 05.05.2025
 * @version t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Implements backlighting for all Left Console panels.
 * @warning This is a stub; a future implementation foresees individual files for each panel on the Left Console.
 *********************************************************************************************************************/


#ifndef __LC_ALL_PANELS_H
#define __LC_ALL_PANELS_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LC_ALL_PANELS_LED_COUNT = 200;  // Total number of LEDs in the panel
const Led lcAllPanelsLedTable[LC_ALL_PANELS_LED_COUNT] PROGMEM = {
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
    {80, LED_BACKLIGHT}, {81, LED_BACKLIGHT}, {82, LED_BACKLIGHT}, {83, LED_BACKLIGHT}, {84, LED_BACKLIGHT}, 
    {85, LED_BACKLIGHT}, {86, LED_BACKLIGHT}, {87, LED_BACKLIGHT}, {88, LED_BACKLIGHT}, {89, LED_BACKLIGHT},
    {90, LED_BACKLIGHT}, {91, LED_BACKLIGHT}, {92, LED_BACKLIGHT}, {93, LED_BACKLIGHT}, {94, LED_BACKLIGHT}, 
    {95, LED_BACKLIGHT}, {96, LED_BACKLIGHT}, {97, LED_BACKLIGHT}, {98, LED_BACKLIGHT}, {99, LED_BACKLIGHT},
    {100, LED_BACKLIGHT}, {101, LED_BACKLIGHT}, {102, LED_BACKLIGHT}, {103, LED_BACKLIGHT}, {104, LED_BACKLIGHT}, 
    {105, LED_BACKLIGHT}, {106, LED_BACKLIGHT}, {107, LED_BACKLIGHT}, {108, LED_BACKLIGHT}, {109, LED_BACKLIGHT},
    {110, LED_BACKLIGHT}, {111, LED_BACKLIGHT}, {112, LED_BACKLIGHT}, {113, LED_BACKLIGHT}, {114, LED_BACKLIGHT}, 
    {115, LED_BACKLIGHT}, {116, LED_BACKLIGHT}, {117, LED_BACKLIGHT}, {118, LED_BACKLIGHT}, {119, LED_BACKLIGHT},
    {120, LED_BACKLIGHT}, {121, LED_BACKLIGHT}, {122, LED_BACKLIGHT}, {123, LED_BACKLIGHT}, {124, LED_BACKLIGHT}, 
    {125, LED_BACKLIGHT}, {126, LED_BACKLIGHT}, {127, LED_BACKLIGHT}, {128, LED_BACKLIGHT}, {129, LED_BACKLIGHT},
    {130, LED_BACKLIGHT}, {131, LED_BACKLIGHT}, {132, LED_BACKLIGHT}, {133, LED_BACKLIGHT}, {134, LED_BACKLIGHT}, 
    {135, LED_BACKLIGHT}, {136, LED_BACKLIGHT}, {137, LED_BACKLIGHT}, {138, LED_BACKLIGHT}, {139, LED_BACKLIGHT},
    {140, LED_BACKLIGHT}, {141, LED_BACKLIGHT}, {142, LED_BACKLIGHT}, {143, LED_BACKLIGHT}, {144, LED_BACKLIGHT}, 
    {145, LED_BACKLIGHT}, {146, LED_BACKLIGHT}, {147, LED_BACKLIGHT}, {148, LED_BACKLIGHT}, {149, LED_BACKLIGHT},
    {150, LED_BACKLIGHT}, {151, LED_BACKLIGHT}, {152, LED_BACKLIGHT}, {153, LED_BACKLIGHT}, {154, LED_BACKLIGHT}, 
    {155, LED_BACKLIGHT}, {156, LED_BACKLIGHT}, {157, LED_BACKLIGHT}, {158, LED_BACKLIGHT}, {159, LED_BACKLIGHT},
    {160, LED_BACKLIGHT}, {161, LED_BACKLIGHT}, {162, LED_BACKLIGHT}, {163, LED_BACKLIGHT}, {164, LED_BACKLIGHT}, 
    {165, LED_BACKLIGHT}, {166, LED_BACKLIGHT}, {167, LED_BACKLIGHT}, {168, LED_BACKLIGHT}, {169, LED_BACKLIGHT},
    {170, LED_BACKLIGHT}, {171, LED_BACKLIGHT}, {172, LED_BACKLIGHT}, {173, LED_BACKLIGHT}, {174, LED_BACKLIGHT}, 
    {175, LED_BACKLIGHT}, {176, LED_BACKLIGHT}, {177, LED_BACKLIGHT}, {178, LED_BACKLIGHT}, {179, LED_BACKLIGHT},
    {180, LED_BACKLIGHT}, {181, LED_BACKLIGHT}, {182, LED_BACKLIGHT}, {183, LED_BACKLIGHT}, {184, LED_BACKLIGHT}, 
    {185, LED_BACKLIGHT}, {186, LED_BACKLIGHT}, {187, LED_BACKLIGHT}, {188, LED_BACKLIGHT}, {189, LED_BACKLIGHT},
    {190, LED_BACKLIGHT}, {191, LED_BACKLIGHT}, {192, LED_BACKLIGHT}, {193, LED_BACKLIGHT}, {194, LED_BACKLIGHT}, 
    {195, LED_BACKLIGHT}, {196, LED_BACKLIGHT}, {197, LED_BACKLIGHT}, {198, LED_BACKLIGHT}, {199, LED_BACKLIGHT}
};

/********************************************************************************************************************
 * @brief   Left Console All Panels class
 * @details Backlighting controller for all Left Console panels.
 *          Total LEDs: 200
 *          Backlight LEDs: 200 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class LcAllPanels : public Panel {
public:
    static LcAllPanels* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new LcAllPanels(startIndex, ledStrip);
        }
        return instance;
    }

private:
    // Private constructor
    LcAllPanels(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LC_ALL_PANELS_LED_COUNT;
        ledTable = lcAllPanelsLedTable;
    }

    // Static callback functions for DCS-BIOS
    static void onConsolesDimmerChange(unsigned int newValue) {
        if (instance) instance->setBacklights(newValue);
    }
    DcsBios::IntegerBuffer consolesDimmerBuffer{0x7544, 0xffff, 0, onConsolesDimmerChange};

    // Instance data
    static LcAllPanels* instance;
};

// Initialize static instance pointer
LcAllPanels* LcAllPanels::instance = nullptr;

#endif 