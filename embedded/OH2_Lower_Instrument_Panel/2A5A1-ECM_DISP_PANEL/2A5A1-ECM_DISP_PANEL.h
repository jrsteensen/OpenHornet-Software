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
 *   Copyright 2016-2026 OpenHornet
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
 * @file 2A5A1-ECM_DISP_PANEL.h
 * @author Lahiru (debounce class adapted from Arribe's 5A5A1-ECS_PANEL.h)
 * @date 10.14.2026
 *
 * @brief Header file for the ECM DISP panel's ECM mode multiposition knob.
 * Defines a debounced multiposition switch so the knob does not report its unwired OFF position
 * while it is rotating between detents.
 */

#ifndef OH_2A5A1_ECM_DISP_PANEL_H
#define OH_2A5A1_ECM_DISP_PANEL_H

#include "Arduino.h"
#include "DcsBios.h"

/**
 * @class SwitchMultiPosDebounce
 * @brief Provides debounce logic for multiposition switches used in DCS to prevent erroneous readings during switch transitions.
 *
 * This class mirrors the DCS-BIOS library's SwitchMultiPos and adds a settling time. A position is only sent
 * to DCS once it has been stable for the debounce delay, so a knob with an unwired (PIN_NC) position does not
 * bounce through that position between detents.
 *
 * @todo Remove this class if the debounce feature is integrated into the DCS-BIOS Arduino library.
 */
class SwitchMultiPosDebounce {
private:
    const char* msg_;              ///< The DCS-BIOS message associated with the switch.
    const byte* pins_;             ///< Array of pin numbers connected to each position of the switch.
    char numberOfPins_;            ///< Total number of pins (positions) of the switch.
    char lastState_;               ///< Last position sent to DCS.
    bool reverse_;                 ///< Flag to reverse the reading logic (HIGH/LOW).
    char debounceSteadyState_;     ///< Position currently being timed for stability.
    unsigned long debounceDelay_;  ///< Milliseconds a position must be stable before it is sent.
    unsigned long lastDebounceTime_ = 0;  ///< Timestamp of the last position change.

    /**
     * Reads the current position of the switch by checking each pin.
     * @return The index of the first pin that is active, or the PIN_NC index when none is.
     */
    char readState() {
        unsigned char ncPinIdx = lastState_;
        for (unsigned char i = 0; i < numberOfPins_; i++) {
            if (pins_[i] == DcsBios::PIN_NC)
                ncPinIdx = i;
            else {
                if (digitalRead(pins_[i]) == LOW && reverse_ == false) return i;
                else if (digitalRead(pins_[i]) == HIGH && reverse_ == true) return i;
            }
        }
        return ncPinIdx;
    }

    /**
     * Forces the next poll to resend the position, used after a DCS-BIOS resync.
     */
    void resetState() {
        lastState_ = (lastState_ == 0) ? -1 : 0;
    }

    /**
     * Polls the pins, applies the settling time and sends the position to DCS once it is stable and changed.
     */
    void pollInput() {
        char state = readState();

        unsigned long now = millis();
        if (state != debounceSteadyState_) {
            lastDebounceTime_ = now;
            debounceSteadyState_ = state;
        }
        if ((now - lastDebounceTime_) >= debounceDelay_) {
            if (state != lastState_) {
                char buf[7];
                utoa(state, buf, 10);
                if (DcsBios::tryToSendDcsBiosMessage(msg_, buf))
                    lastState_ = state;
            }
        }
    }

public:
    /**
     * Constructor for a debounced multiposition switch.
     * @param msg The DCS-BIOS message identifier associated with this switch.
     * @param pins Array of pin numbers connected to the switch positions, DcsBios::PIN_NC for an unwired position.
     * @param numberOfPins Number of positions (pins) on the switch.
     * @param reverse Set to true to reverse the reading logic (for normally high switches).
     * @param debounceDelay Settling time in milliseconds.
     */
    SwitchMultiPosDebounce(const char* msg, const byte* pins, char numberOfPins, bool reverse = false, unsigned long debounceDelay = 50) {
        msg_ = msg;
        pins_ = pins;
        reverse_ = reverse;
        numberOfPins_ = numberOfPins;
        for (unsigned char i = 0; i < numberOfPins; i++) {
            if (pins[i] != DcsBios::PIN_NC)
                pinMode(pins[i], INPUT_PULLUP);
        }
        lastState_ = readState();
        debounceSteadyState_ = lastState_;
        debounceDelay_ = debounceDelay;
    }

    /**
     * Sets or changes the DCS-BIOS message for the switch control.
     * @param msg New DCS-BIOS message identifier for the switch.
     */
    void SetControl(const char* msg) {
        msg_ = msg;
    }

    /**
     * Public interface to reset the state of the switch.
     */
    void resetThisState() {
        this->resetState();
    }

    /**
     * Public interface to poll the switch. Call it once per loop() pass.
     */
    void pollThisInput() {
        this->pollInput();
    }
};

#endif  // OH_2A5A1_ECM_DISP_PANEL_H
