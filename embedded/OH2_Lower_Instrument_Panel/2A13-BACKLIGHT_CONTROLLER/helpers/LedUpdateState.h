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
 *            The flag can be set by any panel as it processes DCS-BIOS updates.
 *            Then, the flag is read by board.h in each loop and if TRUE, used to trigger the update of the LEDs.
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
    
    /**
     * @brief Private constructor to enforce singleton pattern
     * @see This method is called by getInstance() when creating the singleton instance
     */
    LedUpdateState() {                                                // Private constructor to enforce singleton pattern
        ledsNeedUpdate = false;
    }

public:
    /**
     * @brief Gets the singleton instance of the LedUpdateState class
     * @return Pointer to the singleton instance
     * @see This method is called by Board::updateLeds() and other methods that need to check or set the update flag
     */
    static LedUpdateState* getInstance() {
        if (!instance) {
            instance = new LedUpdateState();
        }
        return instance;
    }
    
    /**
     * @brief Sets the LED update flag in an atomic operation
     * @param requireUpdate The new state of the update flag
     * @see This method is called by Board methods that modify LED states
     */
    void setUpdateFlag(bool requireUpdate) {
        cli();                                                        // Disable interrupts (same as noInterrupts())
        ledsNeedUpdate = requireUpdate;
        sei();                                                        // Re-enable interrupts (same as interrupts())
    }

    /**
     * @brief Gets the current state of the LED update flag
     * @return True if LEDs need to be updated, false otherwise
     * @see This method is called by Board::updateLeds() to check if a physical update is needed
     */
    bool getUpdateFlag() const {
        return ledsNeedUpdate;
    }
};

// Initialize static instance pointer
LedUpdateState* LedUpdateState::instance = nullptr;

#endif 