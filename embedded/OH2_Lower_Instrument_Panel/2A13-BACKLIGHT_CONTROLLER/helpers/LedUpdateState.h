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
#include <FastLED.h>

class LedUpdateState {
private:
    static LedUpdateState* instance;
    volatile bool ledsNeedUpdate;                                     // volatile to prevent compiler optimization
    static const uint8_t MAX_ARRAYS = 10;                             // Max number of LED arrays (= channels) to update  
    CRGB* arrays[MAX_ARRAYS];                                         // Array of pointers to LED arrays
    uint8_t arrayCount;                                               // No of LED arrays registered so far
    volatile uint16_t dirtyMask;                                      // Bitmask with meaining: bit i is set --> Channel i needs upd 

    /**
     * @brief Private constructor to enforce singleton pattern
     * @see This method is called by getInstance() when creating the singleton instance
     */
    LedUpdateState() {                                                // Private constructor to enforce singleton pattern
        ledsNeedUpdate = false;
        dirtyMask = 0;
        arrayCount = 0;
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

    void registerArray(CRGB* arr) {
        if (arrayCount < MAX_ARRAYS) {
            arrays[arrayCount++] = arr;
        }
    }
    
    /**
     * @brief Sets the LED update flag in an atomic operation
     * @param requireUpdate The new state of the update flag
     * @see This method is called by Board methods that modify LED states
     */
    void setUpdateFlag(CRGB* arr) {
        for (uint8_t i = 0; i < arrayCount; i++) {
            if (arrays[i] == arr) {
                cli();
                dirtyMask = dirtyMask | (1 << i);                     // Bitwise OR to set the bit for the channel
                sei();
                return;
            }
        setUpdateFlag(true);                                          // If the array is not found, set the update flag for all arrays
        }
    }

    void setUpdateFlag(bool requireUpdate) {
        cli();
        dirtyMask = requireUpdate ? (uint16_t)((1UL << arrayCount) - 1) : 0;
        sei();
    }

    bool getUpdateFlag() const {
        return dirtyMask != 0;
    }

    bool needsUpdate() const {
        return dirtyMask != 0;
    }

    uint16_t getDirtyMask() const {
        return dirtyMask;
    }
    void clearBit(uint8_t bit) {
        cli();
        dirtyMask &= ~(1 << bit);
        sei();
    }
};

// Initialize static instance pointer
LedUpdateState* LedUpdateState::instance = nullptr;

#endif 