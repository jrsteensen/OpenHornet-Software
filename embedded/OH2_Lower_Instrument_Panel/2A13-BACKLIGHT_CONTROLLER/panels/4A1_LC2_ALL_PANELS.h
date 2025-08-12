/**********************************************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *  
 * @file      4A1_LC2_ALL_PANELS.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for Left Console 2 panels.
 * @warning   This is a shorthanded version; a future implementation foresees individual files for each panel on the LC.
 *********************************************************************************************************************/


#ifndef __LC2_ALL_PANELS_H
#define __LC2_ALL_PANELS_H

#include "DcsBios.h"
#include "../helpers/Panel.h"

/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int LC2_ALL_PANELS_LED_COUNT = 215;  // Total number of LEDs in the panel
const Led lc2AllPanelsLedTable[LC2_ALL_PANELS_LED_COUNT] PROGMEM = {
    {0, LED_CONSOLE_BL}, {1, LED_CONSOLE_BL}, {2, LED_CONSOLE_BL}, {3, LED_CONSOLE_BL}, {4, LED_CONSOLE_BL}, 
    {5, LED_CONSOLE_BL}, {6, LED_CONSOLE_BL}, {7, LED_CONSOLE_BL}, {8, LED_CONSOLE_BL}, {9, LED_CONSOLE_BL},
    {10, LED_CONSOLE_BL}, {11, LED_CONSOLE_BL}, {12, LED_CONSOLE_BL}, {13, LED_CONSOLE_BL}, {14, LED_CONSOLE_BL}, 
    {15, LED_CONSOLE_BL}, {16, LED_CONSOLE_BL}, {17, LED_CONSOLE_BL}, {18, LED_CONSOLE_BL}, {19, LED_CONSOLE_BL},
    {20, LED_CONSOLE_BL}, {21, LED_CONSOLE_BL}, {22, LED_CONSOLE_BL}, {23, LED_CONSOLE_BL}, {24, LED_CONSOLE_BL}, 
    {25, LED_CONSOLE_BL}, {26, LED_CONSOLE_BL}, {27, LED_CONSOLE_BL}, {28, LED_CONSOLE_BL}, {29, LED_CONSOLE_BL},
    {30, LED_CONSOLE_BL}, {31, LED_CONSOLE_BL}, {32, LED_CONSOLE_BL}, {33, LED_CONSOLE_BL}, {34, LED_CONSOLE_BL}, 
    {35, LED_CONSOLE_BL}, {36, LED_CONSOLE_BL}, {37, LED_CONSOLE_BL}, {38, LED_CONSOLE_BL}, {39, LED_CONSOLE_BL},
    {40, LED_CONSOLE_BL}, {41, LED_CONSOLE_BL}, {42, LED_CONSOLE_BL}, {43, LED_CONSOLE_BL}, {44, LED_CONSOLE_BL}, 
    {45, LED_CONSOLE_BL}, {46, LED_CONSOLE_BL}, {47, LED_CONSOLE_BL}, {48, LED_CONSOLE_BL}, {49, LED_CONSOLE_BL},
    {50, LED_CONSOLE_BL}, {51, LED_CONSOLE_BL}, {52, LED_CONSOLE_BL}, {53, LED_CONSOLE_BL}, {54, LED_CONSOLE_BL}, 
    {55, LED_CONSOLE_BL}, {56, LED_CONSOLE_BL}, {57, LED_CONSOLE_BL}, {58, LED_CONSOLE_BL}, {59, LED_CONSOLE_BL},
    {60, LED_CONSOLE_BL}, {61, LED_CONSOLE_BL}, {62, LED_CONSOLE_BL}, {63, LED_CONSOLE_BL}, {64, LED_CONSOLE_BL}, 
    {65, LED_CONSOLE_BL}, {66, LED_CONSOLE_BL}, {67, LED_CONSOLE_BL}, {68, LED_CONSOLE_BL}, {69, LED_CONSOLE_BL},
    {70, LED_CONSOLE_BL}, {71, LED_CONSOLE_BL}, {72, LED_CONSOLE_BL}, {73, LED_CONSOLE_BL}, {74, LED_CONSOLE_BL}, 
    {75, LED_CONSOLE_BL}, {76, LED_CONSOLE_BL}, {77, LED_CONSOLE_BL}, {78, LED_CONSOLE_BL}, {79, LED_CONSOLE_BL},
    {80, LED_CONSOLE_BL}, {81, LED_CONSOLE_BL}, {82, LED_CONSOLE_BL}, {83, LED_CONSOLE_BL}, {84, LED_CONSOLE_BL}, 
    {85, LED_CONSOLE_BL}, {86, LED_CONSOLE_BL}, {87, LED_CONSOLE_BL}, {88, LED_CONSOLE_BL}, {89, LED_CONSOLE_BL},
    {90, LED_CONSOLE_BL}, {91, LED_CONSOLE_BL}, {92, LED_CONSOLE_BL}, {93, LED_CONSOLE_BL}, {94, LED_CONSOLE_BL}, 
    {95, LED_CONSOLE_BL}, {96, LED_CONSOLE_BL}, {97, LED_CONSOLE_BL}, {98, LED_CONSOLE_BL}, {99, LED_CONSOLE_BL},
    {100, LED_CONSOLE_BL}, {101, LED_CONSOLE_BL}, {102, LED_CONSOLE_BL}, {103, LED_CONSOLE_BL}, {104, LED_CONSOLE_BL}, 
    {105, LED_CONSOLE_BL}, {106, LED_CONSOLE_BL}, {107, LED_CONSOLE_BL}, {108, LED_CONSOLE_BL}, {109, LED_CONSOLE_BL},
    {110, LED_CONSOLE_BL}, {111, LED_CONSOLE_BL}, {112, LED_CONSOLE_BL}, {113, LED_CONSOLE_BL}, {114, LED_CONSOLE_BL}, 
    {115, LED_CONSOLE_BL}, {116, LED_CONSOLE_BL}, {117, LED_CONSOLE_BL}, {118, LED_CONSOLE_BL}, {119, LED_CONSOLE_BL},
    {120, LED_CONSOLE_BL}, {121, LED_CONSOLE_BL}, {122, LED_CONSOLE_BL}, {123, LED_CONSOLE_BL}, {124, LED_CONSOLE_BL}, 
    {125, LED_CONSOLE_BL}, {126, LED_CONSOLE_BL}, {127, LED_CONSOLE_BL}, {128, LED_CONSOLE_BL}, {129, LED_CONSOLE_BL},
    {130, LED_CONSOLE_BL}, {131, LED_CONSOLE_BL}, {132, LED_CONSOLE_BL}, {133, LED_CONSOLE_BL}, {134, LED_CONSOLE_BL}, 
    {135, LED_CONSOLE_BL}, {136, LED_CONSOLE_BL}, {137, LED_CONSOLE_BL}, {138, LED_CONSOLE_BL}, {139, LED_CONSOLE_BL},
    {140, LED_CONSOLE_BL}, {141, LED_CONSOLE_BL}, {142, LED_CONSOLE_BL}, {143, LED_CONSOLE_BL}, {144, LED_CONSOLE_BL}, 
    {145, LED_CONSOLE_BL}, {146, LED_CONSOLE_BL}, {147, LED_CONSOLE_BL}, {148, LED_CONSOLE_BL}, {149, LED_CONSOLE_BL},
    {150, LED_CONSOLE_BL}, {151, LED_CONSOLE_BL}, {152, LED_CONSOLE_BL}, {153, LED_CONSOLE_BL}, {154, LED_CONSOLE_BL}, 
    {155, LED_CONSOLE_BL}, {156, LED_CONSOLE_BL}, {157, LED_CONSOLE_BL}, {158, LED_CONSOLE_BL}, {159, LED_CONSOLE_BL},
    {160, LED_CONSOLE_BL}, {161, LED_CONSOLE_BL}, {162, LED_CONSOLE_BL}, {163, LED_CONSOLE_BL}, {164, LED_CONSOLE_BL}, 
    {165, LED_CONSOLE_BL}, {166, LED_CONSOLE_BL}, {167, LED_CONSOLE_BL}, {168, LED_CONSOLE_BL}, {169, LED_CONSOLE_BL},
    {170, LED_CONSOLE_BL}, {171, LED_CONSOLE_BL}, {172, LED_CONSOLE_BL}, {173, LED_CONSOLE_BL}, {174, LED_CONSOLE_BL}, 
    {175, LED_CONSOLE_BL}, {176, LED_CONSOLE_BL}, {177, LED_CONSOLE_BL}, {178, LED_CONSOLE_BL}, {179, LED_CONSOLE_BL},
    {180, LED_CONSOLE_BL}, {181, LED_CONSOLE_BL}, {182, LED_CONSOLE_BL}, {183, LED_CONSOLE_BL}, {184, LED_CONSOLE_BL}, 
    {185, LED_CONSOLE_BL}, {186, LED_CONSOLE_BL}, {187, LED_CONSOLE_BL}, {188, LED_CONSOLE_BL}, {189, LED_CONSOLE_BL},
    {190, LED_CONSOLE_BL}, {191, LED_CONSOLE_BL}, {192, LED_CONSOLE_BL}, {193, LED_CONSOLE_BL}, {194, LED_CONSOLE_BL}, 
    {195, LED_CONSOLE_BL}, {196, LED_CONSOLE_BL}, {197, LED_CONSOLE_BL}, {198, LED_CONSOLE_BL}, {199, LED_CONSOLE_BL},
    {200, LED_CONSOLE_BL}, {201, LED_CONSOLE_BL}, {202, LED_CONSOLE_BL}, {203, LED_CONSOLE_BL}, {204, LED_CONSOLE_BL}, 
    {205, LED_CONSOLE_BL}, {206, LED_CONSOLE_BL}, {207, LED_CONSOLE_BL}, {208, LED_CONSOLE_BL}, {209, LED_CONSOLE_BL},
    {210, LED_CONSOLE_BL}, {211, LED_CONSOLE_BL}, {212, LED_CONSOLE_BL}, {213, LED_CONSOLE_BL}, {214, LED_CONSOLE_BL}
};

/********************************************************************************************************************
 * @brief   Left Console 2 All Panels class
 * @details Backlighting controller for Left Console 2 panels.
 *          Total LEDs: 215
 *          Backlight LEDs: 215 (all LEDs are backlights)
 *          Indicator LEDs: 0 (no indicators in this panel)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class Lc2AllPanels : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the Lc2AllPanels class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static Lc2AllPanels* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new Lc2AllPanels(startIndex, ledStrip);
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
    Lc2AllPanels(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = LC2_ALL_PANELS_LED_COUNT;
        ledTable = lc2AllPanelsLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static Lc2AllPanels* instance;
};

// Initialize static instance pointer
Lc2AllPanels* Lc2AllPanels::instance = nullptr;

#endif 