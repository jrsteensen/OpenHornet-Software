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
 * @file      LedUpdateState.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     This class serves just one purpose: track whether the LEDs need to be updated.
 *            The flag is read by board.h, and if TRUE, used to trigger the update of the LEDs.
 * @details   Technical implementation: singleton state machine and using interrupt pausing to ensure atomicity
 *            when writing the flag.
 *********************************************************************************************************************/

#ifndef __LED_UPDATE_STATE_H
#define __LED_UPDATE_STATE_H

#include <Arduino.h>
#include <avr/interrupt.h>                                            // For interrupt control functions

class LedUpdateState {
private:
    static LedUpdateState* instance;
    volatile bool ledsNeedUpdate;                                     // volatile to prevent compiler optimization
    
    LedUpdateState() {                                                // Private constructor to enforce singleton pattern
        ledsNeedUpdate = false;
    }

public:
    static LedUpdateState* getInstance() {
        if (!instance) {
            instance = new LedUpdateState();
        }
        return instance;
    }
    
    void setUpdateFlag(bool requireUpdate) {
        cli();                                                        // Disable interrupts (same as noInterrupts())
        ledsNeedUpdate = requireUpdate;
        sei();                                                        // Re-enable interrupts (same as interrupts())
    }

    bool getUpdateFlag() const {
        return ledsNeedUpdate;
    }
};

// Initialize static instance pointer
LedUpdateState* LedUpdateState::instance = nullptr;

#endif 