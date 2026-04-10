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
 * @file      2A2A1A7_RWR_CONTROL.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Implements backlighting and indicators for the RWR Control panel.
 *            It consists of two parts:
 *            - An array with the LEDs and their roles (LED_INSTR_BL, LED_RWR_BIT, LED_RWR_FAIL)
 *            - A class that implements the panel's functionality
 *********************************************************************************************************************/


#ifndef __RWR_CONTROL_H
#define __RWR_CONTROL_H

#include "DcsBios.h"
#include "../helpers/Panel.h"


/********************************************************************************************************************
 * @brief   This table defines the panel's LEDs.
 * @details "Role" in this context refers to the LED role enum in the LedRole.h file (enum used for memory efficiency).
 * @remark  This table is stored in PROGMEM for memory efficiency.
 * @see     LedRole.h for the list of LED roles and LedStruct.h for the Led structure.
 ********************************************************************************************************************/
const int RWR_CONTROL_LED_COUNT = 32;  // Total number of LEDs in the panel
const Led rwrControlLedTable[RWR_CONTROL_LED_COUNT] PROGMEM = {
    {0, LED_INSTR_BL}, {1, LED_INSTR_BL}, {2, LED_INSTR_BL}, {3, LED_INSTR_BL},
    {4, LED_INSTR_BL}, {5, LED_INSTR_BL}, {6, LED_RWR_BIT}, {7, LED_RWR_BIT_FAIL},
    {8, LED_RWR_BIT_FAIL}, {9, LED_RWR_BIT}, {10, LED_RWR_OFFSET}, {11, LED_RWR_OFFSET_EN},
    {12, LED_RWR_OFFSET_EN}, {13, LED_RWR_OFFSET}, {14, LED_RWR_SPECIAL}, {15, LED_RWR_SPECIAL_EN},
    {16, LED_RWR_SPECIAL_EN}, {17, LED_RWR_SPECIAL}, {18, LED_RWR_DISPLAY}, {19, LED_RWR_LIMIT},
    {20, LED_RWR_LIMIT}, {21, LED_RWR_DISPLAY}, {22, LED_RWR_POWER}, {23, LED_RWR_POWER},
    {24, LED_RWR_NONE}, {25, LED_RWR_NONE}, {26, LED_INSTR_BL}, {27, LED_INSTR_BL},
    {28, LED_INSTR_BL}, {29, LED_INSTR_BL}, {30, LED_INSTR_BL}, {31, LED_INSTR_BL}
};

/********************************************************************************************************************
 * @brief   RWR Control Panel class
 * @details Backlighting controller for the RWR Control panel.
 *          Total LEDs: 32
 *          Backlight LEDs: 13 
 *          Indicator LEDs: 19 (indices 7-25)
 * @remark  This class inherits from the "basic" Panel class in panels/Panel.h
 *          It also enforces a singleton pattern; this is required to use DCS-BIOS callbacks in class methods.
 * @see     Panel.h for the base class implementation
 ********************************************************************************************************************/
class RwrControlPanel : public Panel {
public:
    /**
     * @brief Gets the singleton instance of the RwrControlPanel class
     * @param startIndex The starting index for this panel's LEDs on the strip
     * @param ledStrip Pointer to the LED strip array
     * @return Pointer to the singleton instance
     * @see This method is called by the main .ino file's addPanel() method to create the panel instance
     */
    static RwrControlPanel* getInstance(int startIndex = 0, CRGB* ledStrip = nullptr) {
        if (!instance) {
            instance = new RwrControlPanel(startIndex, ledStrip);
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
    RwrControlPanel(int startIndex, CRGB* ledStrip) {
        panelStartIndex = startIndex;
        this->ledStrip = ledStrip;
        ledCount = RWR_CONTROL_LED_COUNT;
        ledTable = rwrControlLedTable;
        
        // Initialize state variables
        rwrPanelOn = false;
        rwrFailActive = false;
        rwrOffsetEnableActive = false;
        rwrSpecialEnableActive = false;
        rwrLimitDisplayActive = false;
    }

    // State variables
    bool rwrPanelOn;           // RWR panel power state
    bool rwrFailActive;        // RWR fail indicator state
    bool rwrOffsetEnableActive; // RWR offset enable state
    bool rwrSpecialEnableActive; // RWR special enable state
    bool rwrLimitDisplayActive; // RWR limit display state
    /**
     * @brief Updates all LED colors based on current state
     * @details This method implements the state machine logic for LED colors
     */
    void updateLedColors() {
        if (!rwrPanelOn) {
            // When RWR panel is OFF, all indicator LEDs are black
            setIndicatorColor(LED_RWR_POWER, NVIS_BLACK);
            setIndicatorColor(LED_RWR_BIT, NVIS_BLACK);
            setIndicatorColor(LED_RWR_BIT_FAIL, NVIS_BLACK);
            setIndicatorColor(LED_RWR_OFFSET, NVIS_BLACK);
            setIndicatorColor(LED_RWR_OFFSET_EN, NVIS_BLACK);
            setIndicatorColor(LED_RWR_DISPLAY, NVIS_BLACK);
            setIndicatorColor(LED_RWR_LIMIT, NVIS_BLACK);
            setIndicatorColor(LED_RWR_SPECIAL, NVIS_BLACK);
            setIndicatorColor(LED_RWR_SPECIAL_EN, NVIS_BLACK);
        } else {
            // When RWR panel is ON, set colors based on state
            // RWR POWER
            setIndicatorColor(LED_RWR_POWER, NVIS_GREEN_A);
            // RWR BIT and RWR BIT FAIL
            if (rwrFailActive) {
                setIndicatorColor(LED_RWR_BIT, NVIS_RED);
                setIndicatorColor(LED_RWR_BIT_FAIL, NVIS_RED);
            } else {
                setIndicatorColor(LED_RWR_BIT, NVIS_WHITE);
                setIndicatorColor(LED_RWR_BIT_FAIL, NVIS_WHITE);
            }

            // RWR OFFSET and RWR OFFSET EN
            if (rwrOffsetEnableActive) {
                setIndicatorColor(LED_RWR_OFFSET, NVIS_GREEN_A);
                setIndicatorColor(LED_RWR_OFFSET_EN, NVIS_GREEN_A);
            } else {
                setIndicatorColor(LED_RWR_OFFSET, NVIS_WHITE);
                setIndicatorColor(LED_RWR_OFFSET_EN, NVIS_WHITE);
            }

            // RWR SPECIAL and RWR SPECIAL EN
            if (rwrSpecialEnableActive) {
                setIndicatorColor(LED_RWR_SPECIAL, NVIS_GREEN_A);
                setIndicatorColor(LED_RWR_SPECIAL_EN, NVIS_GREEN_A);
            } else {
                setIndicatorColor(LED_RWR_SPECIAL, NVIS_WHITE);
                setIndicatorColor(LED_RWR_SPECIAL_EN, NVIS_WHITE);
            }

            // RWR LIMIT DISPLAY 
            if (rwrLimitDisplayActive) {
                setIndicatorColor(LED_RWR_DISPLAY, NVIS_GREEN_A);
                setIndicatorColor(LED_RWR_LIMIT, NVIS_GREEN_A);
            } else {
                setIndicatorColor(LED_RWR_DISPLAY, NVIS_WHITE);
                setIndicatorColor(LED_RWR_LIMIT, NVIS_WHITE);
            }
        }
    }

    // Static callback functions for DCS-BIOS

    // RWR power state callback - controls the main panel state
    static void onRwrLowerLtChange(unsigned int newValue) {
        if (instance) {
            instance->rwrPanelOn = (newValue != 0);
            instance->updateLedColors();
        }
    }
    DcsBios::IntegerBuffer rwrLowerLtBuffer{FA_18C_hornet_RWR_LOWER_LT, onRwrLowerLtChange};

    // RWR fail indicator callback
    static void onRwrFailLtChange(unsigned int newValue) {
        if (instance) {
            instance->rwrFailActive = (newValue != 0);
            instance->updateLedColors();
        }
    }
    DcsBios::IntegerBuffer rwrFailLtBuffer{FA_18C_hornet_RWR_FAIL_LT, onRwrFailLtChange};

    // RWR offset enable callback
    static void onRwrOffsetEnableLtChange(unsigned int newValue) {
        if (instance) {
            instance->rwrOffsetEnableActive = (newValue != 0);
            instance->updateLedColors();
        }
    }
    DcsBios::IntegerBuffer rwrEnableLtBuffer{FA_18C_hornet_RWR_ENABLE_LT, onRwrOffsetEnableLtChange};

    // RWR special enable callback
    static void onRwrSpecialEnLtChange(unsigned int newValue) {
        if (instance) {
            instance->rwrSpecialEnableActive = (newValue != 0);
            instance->updateLedColors();
        }
    }
    DcsBios::IntegerBuffer rwrSpecialEnLtBuffer{FA_18C_hornet_RWR_SPECIAL_EN_LT, onRwrSpecialEnLtChange};


    // RWR LIMIT DISPLAY callback
    static void onRwrLimitDisplayLtChange(unsigned int newValue) {
        if (instance) {
            instance->rwrLimitDisplayActive = (newValue != 0);
            instance->updateLedColors();
        }
    }
    DcsBios::IntegerBuffer rwrLimitDisplayLtBuffer{FA_18C_hornet_RWR_LIMIT_LT, onRwrLimitDisplayLtChange};


    // Instance data
    static RwrControlPanel* instance;
};

// Initialize static instance pointer
RwrControlPanel* RwrControlPanel::instance = nullptr;

#endif 