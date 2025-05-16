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
 * @file      Board.h
 * @author    Ulukaii
 * @date      16.05.2025
 * @version   t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Defines the Board class for managing LED strips.
 * @details   During setup, a single board object is created. It manages the update of the LEDs centrally.
 *            It is the only place from which the expensive FastLED.show() function is called.
 *********************************************************************************************************************/



#ifndef __BOARD_H
#define __BOARD_H

#include "FastLED.h"


class Board {
public:
    static Board* getInstance() {
        if (!instance) {
            instance = new Board();
        }
        return instance;
    }

    // Method to check if LEDs need updating
    bool isLedsNeedUpdate() const {
        return ledsNeedUpdate;
    }

    // Method to indicate that LEDs need updating
    void setLedsNeedUpdate() {
        ledsNeedUpdate = true;
    }

    // Method to update LEDs if needed
    void updateLeds() {
        if (ledsNeedUpdate) {
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    // If countdown is 0, this is the first cycle, so set to 8
            updCountdown--;
            if (updCountdown == 0) {                                  // Trigger FastLED.show() at end of countdown
                FastLED.show();                                       // This countdown logic allows to capture multiple
                ledsNeedUpdate = false;                               // DCS-BIOS loop cycles into one FastLED.show() call
            }
        }
    }

private:
    // Private constructor to enforce singleton pattern
    Board() {
        ledsNeedUpdate = false;
        updCountdown = 0;
    }

    // Static instance pointer
    static Board* instance;

    // LED update state
    bool ledsNeedUpdate;
    int updCountdown;
};

// Initialize static instance pointer
Board* Board::instance = nullptr;

#endif 