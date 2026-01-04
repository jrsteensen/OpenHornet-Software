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
 * @file      5A5_RC2_ALL_PANELS.h
 * @author    Ulukaii
 * @date      20.05.2025
 * @version   u 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting for all remaining panels in RC2.
 * @details   This includes:
 *            - ECS Panel (63 LEDs)
 *            - DEFOG Panel (23 LEDs)
 *            - INTR LT Panel (65 LEDs)
 *            - SNSR Panel (58 LEDs)
 *            - SIM CNTL Panel (61 LEDs)
 *            - KY58 Panel (79 LEDs)
 *            Total: 349 LEDs
 *********************************************************************************************************************/


#ifndef __RC2_ALL_PANELS_H
#define __RC2_ALL_PANELS_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the Panel.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RC2_ALL_PANELS_LED_COUNT = 349;  // Total number of LEDs in all panels
const Led rc2AllPanelsLedTable[RC2_ALL_PANELS_LED_COUNT] PROGMEM = {
    // ECS Panel (63 LEDs)
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
    {60, LED_CONSOLE_BL}, {61, LED_CONSOLE_BL}, {62, LED_CONSOLE_BL},
    
    // DEFOG Panel (23 LEDs)
    {63, LED_CONSOLE_BL}, {64, LED_CONSOLE_BL}, {65, LED_CONSOLE_BL}, {66, LED_CONSOLE_BL}, {67, LED_CONSOLE_BL},
    {68, LED_CONSOLE_BL}, {69, LED_CONSOLE_BL}, {70, LED_CONSOLE_BL}, {71, LED_CONSOLE_BL}, {72, LED_CONSOLE_BL},
    {73, LED_CONSOLE_BL}, {74, LED_CONSOLE_BL}, {75, LED_CONSOLE_BL}, {76, LED_CONSOLE_BL}, {77, LED_CONSOLE_BL},
    {78, LED_CONSOLE_BL}, {79, LED_CONSOLE_BL}, {80, LED_CONSOLE_BL}, {81, LED_CONSOLE_BL}, {82, LED_CONSOLE_BL},
    {83, LED_CONSOLE_BL}, {84, LED_CONSOLE_BL}, {85, LED_CONSOLE_BL},
    
    // INTR LT Panel (65 LEDs)
    {86, LED_CONSOLE_BL}, {87, LED_CONSOLE_BL}, {88, LED_CONSOLE_BL}, {89, LED_CONSOLE_BL}, {90, LED_CONSOLE_BL},
    {91, LED_CONSOLE_BL}, {92, LED_CONSOLE_BL}, {93, LED_CONSOLE_BL}, {94, LED_CONSOLE_BL}, {95, LED_CONSOLE_BL},
    {96, LED_CONSOLE_BL}, {97, LED_CONSOLE_BL}, {98, LED_CONSOLE_BL}, {99, LED_CONSOLE_BL}, {100, LED_CONSOLE_BL},
    {101, LED_CONSOLE_BL}, {102, LED_CONSOLE_BL}, {103, LED_CONSOLE_BL}, {104, LED_CONSOLE_BL}, {105, LED_CONSOLE_BL},
    {106, LED_CONSOLE_BL}, {107, LED_CONSOLE_BL}, {108, LED_CONSOLE_BL}, {109, LED_CONSOLE_BL}, {110, LED_CONSOLE_BL},
    {111, LED_CONSOLE_BL}, {112, LED_CONSOLE_BL}, {113, LED_CONSOLE_BL}, {114, LED_CONSOLE_BL}, {115, LED_CONSOLE_BL},
    {116, LED_CONSOLE_BL}, {117, LED_CONSOLE_BL}, {118, LED_CONSOLE_BL}, {119, LED_CONSOLE_BL}, {120, LED_CONSOLE_BL},
    {121, LED_CONSOLE_BL}, {122, LED_CONSOLE_BL}, {123, LED_CONSOLE_BL}, {124, LED_CONSOLE_BL}, {125, LED_CONSOLE_BL},
    {126, LED_CONSOLE_BL}, {127, LED_CONSOLE_BL}, {128, LED_CONSOLE_BL}, {129, LED_CONSOLE_BL}, {130, LED_CONSOLE_BL},
    {131, LED_CONSOLE_BL}, {132, LED_CONSOLE_BL}, {133, LED_CONSOLE_BL}, {134, LED_CONSOLE_BL}, {135, LED_CONSOLE_BL},
    {136, LED_CONSOLE_BL}, {137, LED_CONSOLE_BL}, {138, LED_CONSOLE_BL}, {139, LED_CONSOLE_BL}, {140, LED_CONSOLE_BL},
    {141, LED_CONSOLE_BL}, {142, LED_CONSOLE_BL}, {143, LED_CONSOLE_BL}, {144, LED_CONSOLE_BL}, {145, LED_CONSOLE_BL},
    {146, LED_CONSOLE_BL}, {147, LED_CONSOLE_BL}, {148, LED_CONSOLE_BL}, {149, LED_CONSOLE_BL}, {150, LED_CONSOLE_BL},
    
    // SNSR Panel (58 LEDs)
    {151, LED_CONSOLE_BL}, {152, LED_CONSOLE_BL}, {153, LED_CONSOLE_BL}, {154, LED_CONSOLE_BL}, {155, LED_CONSOLE_BL},
    {156, LED_CONSOLE_BL}, {157, LED_CONSOLE_BL}, {158, LED_CONSOLE_BL}, {159, LED_CONSOLE_BL}, {160, LED_CONSOLE_BL},
    {161, LED_CONSOLE_BL}, {162, LED_CONSOLE_BL}, {163, LED_CONSOLE_BL}, {164, LED_CONSOLE_BL}, {165, LED_CONSOLE_BL},
    {166, LED_CONSOLE_BL}, {167, LED_CONSOLE_BL}, {168, LED_CONSOLE_BL}, {169, LED_CONSOLE_BL}, {170, LED_CONSOLE_BL},
    {171, LED_CONSOLE_BL}, {172, LED_CONSOLE_BL}, {173, LED_CONSOLE_BL}, {174, LED_CONSOLE_BL}, {175, LED_CONSOLE_BL},
    {176, LED_CONSOLE_BL}, {177, LED_CONSOLE_BL}, {178, LED_CONSOLE_BL}, {179, LED_CONSOLE_BL}, {180, LED_CONSOLE_BL},
    {181, LED_CONSOLE_BL}, {182, LED_CONSOLE_BL}, {183, LED_CONSOLE_BL}, {184, LED_CONSOLE_BL}, {185, LED_CONSOLE_BL},
    {186, LED_CONSOLE_BL}, {187, LED_CONSOLE_BL}, {188, LED_CONSOLE_BL}, {189, LED_CONSOLE_BL}, {190, LED_CONSOLE_BL},
    {191, LED_CONSOLE_BL}, {192, LED_CONSOLE_BL}, {193, LED_CONSOLE_BL}, {194, LED_CONSOLE_BL}, {195, LED_CONSOLE_BL},
    {196, LED_CONSOLE_BL}, {197, LED_CONSOLE_BL}, {198, LED_CONSOLE_BL}, {199, LED_CONSOLE_BL}, {200, LED_CONSOLE_BL},
    {201, LED_CONSOLE_BL}, {202, LED_CONSOLE_BL}, {203, LED_CONSOLE_BL}, {204, LED_CONSOLE_BL}, {205, LED_CONSOLE_BL},
    {206, LED_CONSOLE_BL}, {207, LED_CONSOLE_BL}, {208, LED_CONSOLE_BL},
    
    // SIM CNTL Panel (61 LEDs)
    {209, LED_CONSOLE_BL}, {210, LED_CONSOLE_BL}, {211, LED_CONSOLE_BL}, {212, LED_CONSOLE_BL}, {213, LED_CONSOLE_BL},
    {214, LED_CONSOLE_BL}, {215, LED_CONSOLE_BL}, {216, LED_CONSOLE_BL}, {217, LED_CONSOLE_BL}, {218, LED_CONSOLE_BL},
    {219, LED_CONSOLE_BL}, {220, LED_CONSOLE_BL}, {221, LED_CONSOLE_BL}, {222, LED_CONSOLE_BL}, {223, LED_CONSOLE_BL},
    {224, LED_CONSOLE_BL}, {225, LED_CONSOLE_BL}, {226, LED_CONSOLE_BL}, {227, LED_CONSOLE_BL}, {228, LED_CONSOLE_BL},
    {229, LED_CONSOLE_BL}, {230, LED_CONSOLE_BL}, {231, LED_CONSOLE_BL}, {232, LED_CONSOLE_BL}, {233, LED_CONSOLE_BL},
    {234, LED_CONSOLE_BL}, {235, LED_CONSOLE_BL}, {236, LED_CONSOLE_BL}, {237, LED_CONSOLE_BL}, {238, LED_CONSOLE_BL},
    {239, LED_CONSOLE_BL}, {240, LED_CONSOLE_BL}, {241, LED_CONSOLE_BL}, {242, LED_CONSOLE_BL}, {243, LED_CONSOLE_BL},
    {244, LED_CONSOLE_BL}, {245, LED_CONSOLE_BL}, {246, LED_CONSOLE_BL}, {247, LED_CONSOLE_BL}, {248, LED_CONSOLE_BL},
    {249, LED_CONSOLE_BL}, {250, LED_CONSOLE_BL}, {251, LED_CONSOLE_BL}, {252, LED_CONSOLE_BL}, {253, LED_CONSOLE_BL},
    {254, LED_CONSOLE_BL}, {255, LED_CONSOLE_BL}, {256, LED_CONSOLE_BL}, {257, LED_CONSOLE_BL}, {258, LED_CONSOLE_BL},
    {259, LED_CONSOLE_BL}, {260, LED_CONSOLE_BL}, {261, LED_CONSOLE_BL}, {262, LED_CONSOLE_BL}, {263, LED_CONSOLE_BL},
    {264, LED_CONSOLE_BL}, {265, LED_CONSOLE_BL}, {266, LED_CONSOLE_BL}, {267, LED_CONSOLE_BL}, {268, LED_CONSOLE_BL},
    {269, LED_CONSOLE_BL},
    
    // KY58 Panel (79 LEDs)
    {270, LED_CONSOLE_BL}, {271, LED_CONSOLE_BL}, {272, LED_CONSOLE_BL}, {273, LED_CONSOLE_BL}, {274, LED_CONSOLE_BL},
    {275, LED_CONSOLE_BL}, {276, LED_CONSOLE_BL}, {277, LED_CONSOLE_BL}, {278, LED_CONSOLE_BL}, {279, LED_CONSOLE_BL},
    {280, LED_CONSOLE_BL}, {281, LED_CONSOLE_BL}, {282, LED_CONSOLE_BL}, {283, LED_CONSOLE_BL}, {284, LED_CONSOLE_BL},
    {285, LED_CONSOLE_BL}, {286, LED_CONSOLE_BL}, {287, LED_CONSOLE_BL}, {288, LED_CONSOLE_BL}, {289, LED_CONSOLE_BL},
    {290, LED_CONSOLE_BL}, {291, LED_CONSOLE_BL}, {292, LED_CONSOLE_BL}, {293, LED_CONSOLE_BL}, {294, LED_CONSOLE_BL},
    {295, LED_CONSOLE_BL}, {296, LED_CONSOLE_BL}, {297, LED_CONSOLE_BL}, {298, LED_CONSOLE_BL}, {299, LED_CONSOLE_BL},
    {300, LED_CONSOLE_BL}, {301, LED_CONSOLE_BL}, {302, LED_CONSOLE_BL}, {303, LED_CONSOLE_BL}, {304, LED_CONSOLE_BL},
    {305, LED_CONSOLE_BL}, {306, LED_CONSOLE_BL}, {307, LED_CONSOLE_BL}, {308, LED_CONSOLE_BL}, {309, LED_CONSOLE_BL},
    {310, LED_CONSOLE_BL}, {311, LED_CONSOLE_BL}, {312, LED_CONSOLE_BL}, {313, LED_CONSOLE_BL}, {314, LED_CONSOLE_BL},
    {315, LED_CONSOLE_BL}, {316, LED_CONSOLE_BL}, {317, LED_CONSOLE_BL}, {318, LED_CONSOLE_BL}, {319, LED_CONSOLE_BL},
    {320, LED_CONSOLE_BL}, {321, LED_CONSOLE_BL}, {322, LED_CONSOLE_BL}, {323, LED_CONSOLE_BL}, {324, LED_CONSOLE_BL},
    {325, LED_CONSOLE_BL}, {326, LED_CONSOLE_BL}, {327, LED_CONSOLE_BL}, {328, LED_CONSOLE_BL}, {329, LED_CONSOLE_BL},
    {330, LED_CONSOLE_BL}, {331, LED_CONSOLE_BL}, {332, LED_CONSOLE_BL}, {333, LED_CONSOLE_BL}, {334, LED_CONSOLE_BL},
    {335, LED_CONSOLE_BL}, {336, LED_CONSOLE_BL}, {337, LED_CONSOLE_BL}, {338, LED_CONSOLE_BL}, {339, LED_CONSOLE_BL},
    {340, LED_CONSOLE_BL}, {341, LED_CONSOLE_BL}, {342, LED_CONSOLE_BL}, {343, LED_CONSOLE_BL}, {344, LED_CONSOLE_BL},
    {345, LED_CONSOLE_BL}, {346, LED_CONSOLE_BL}, {347, LED_CONSOLE_BL}, {348, LED_CONSOLE_BL}
};

/********************************************************************************************************************
 * @brief   This table defines the optional legend text for specific backlight LEDs.
 * @details Only LEDs that need text are included in this table.
 * @remark  This table is stored in PROGMEM for memory efficiency.
 ********************************************************************************************************************/
const int RC2_ALL_PANELS_TEXT_COUNT = 0;  // Number of LEDs that need text
const LedText rc2AllPanelsTextTable[RC2_ALL_PANELS_TEXT_COUNT] PROGMEM = {
    // No text needed for this panel
};

/********************************************************************************************************************
 * @brief   RC2 All Panels class
 * @details Backlighting controller for all remaining panels in RC2.
 *          Total LEDs: 349
 *          Backlight LEDs: 349 (all LEDs are backlights)
 *          Panel breakdown:
 *          - ECS Panel: 63 LEDs
 *          - DEFOG Panel: 23 LEDs
 *          - INTR LT Panel: 65 LEDs
 *          - SNSR Panel: 58 LEDs
 *          - SIM CNTL Panel: 61 LEDs
 *          - KY58 Panel: 79 LEDs
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 ********************************************************************************************************************/
class Rc2AllPanels : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the Rc2AllPanels class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static Rc2AllPanels* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new Rc2AllPanels(startIndex, ledStrip);
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
    Rc2AllPanels(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RC2_ALL_PANELS_LED_COUNT;
        ledTable = rc2AllPanelsLedTable;
    }

    // Static callback functions for DCS-BIOS
    // NIL

    // Instance data
    static Rc2AllPanels* instance;
};

// Initialize static instance pointer
Rc2AllPanels* Rc2AllPanels::instance = nullptr;

#endif 