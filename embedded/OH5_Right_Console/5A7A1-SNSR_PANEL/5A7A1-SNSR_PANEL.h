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
 * @file 5A7A1-SNSR_PANEL.h
 * @author Arribe
 * @date 03.13.2024
 *
 * @brief Header file for the SNSR (Sensor) panel's INS (Inertial Navigation System), and Radar rotary/multiposition switches.
 * This header defines classes to manage debounce logic for multiposition switches interfacing with the DCS (Digital Combat Simulator) Bios, enhancing the reliability of switch position readings.
 */


#include <math.h>
#include "Arduino.h"
#include "DcsBios.h"

/**
 * @class SwitchMultiPosDebounce
 * @brief Provides debounce logic for multiposition switches used in DCS to prevent erroneous readings during switch transitions.
 *
 * This class extends the functionality of the DCS BIOS library's SwitchMultiPos method by adding debounce logic.
 * This ensures that while a knob is rotating, its output doesn't mistakenly bounce to an unconnected or default position between detents.
 *
 * @todo Remove the SwitchMultiPosDebounce class if the debounce feature is integrated into the DCS-BIOS Arduino library as per the pull request https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/pull/56.
 */
class SwitchMultiPosDebounce {
private:
    const char* msg_; ///< The DCS BIOS message associated with the switch.
    const byte* pins_; ///< Array of pin numbers connected to each position of the switch.
    char numberOfPins_; ///< Total number of pins (positions) of the switch.
    char lastState_; ///< Last stable state of the switch.
    bool reverse_; ///< Flag to reverse the reading logic (HIGH/LOW).
    char debounceSteadyState_; ///< The current steady state used for debounce comparison.
    unsigned long debounceDelay_; ///< Delay in milliseconds to consider the switch stable.
    unsigned long lastDebounceTime = 0; ///< Timestamp of the last debounce check.

    /**
     * Reads the current state of the switch by checking each pin's voltage level.
     * @return The current state (position) of the switch.
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
     * Resets the last known state of the switch to its default. Useful in initialization or error recovery.
     */
    void resetState() {
        lastState_ = (lastState_ == 0) ? -1 : 0;
    }

    /**
     * Checks the switch's pins at regular intervals to apply debounce logic and update the switch state in DCS if a change is detected.
     */
    void pollInput() {
        char state = readState();

        unsigned long now = millis();
        if (state != debounceSteadyState_) {
            lastDebounceTime = now;
            debounceSteadyState_ = state;
        }
        if ((now - lastDebounceTime) >= debounceDelay_) {
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
     * Constructor for initializing a debounced multiposition switch.
     * @param msg The DCS BIOS message identifier associated with this switch.
     * @param pins Array of physical pin numbers connected to the switch positions.
     * @param numberOfPins Number of positions (pins) on the switch.
     * @param reverse Set to true to reverse the reading logic (for normally high switches).
     * @param debounceDelay Debounce time in milliseconds.
     */
    SwitchMultiPosDebounce(const char* msg, const byte* pins, char numberOfPins, bool reverse = false, unsigned long debounceDelay = 50) {
        msg_ = msg;
        pins_ = pins;
        reverse_ = reverse;
        numberOfPins_ = numberOfPins;
        unsigned char i;
        for (i = 0; i < numberOfPins; i++) {
            if (pins[i] != DcsBios::PIN_NC)
                pinMode(pins[i], INPUT_PULLUP);
        }
        lastState_ = readState();
        debounceSteadyState_ = lastState_;
        debounceDelay_ = debounceDelay;
    }

    /**
     * Sets or changes the DCS BIOS message for the switch control.
     * @param msg New DCS BIOS message identifier for the switch.
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
     * Public interface to check the input state of the switch and apply debounce logic.
     */
    void pollThisInput() {
        this->pollInput();
    }
};

/**
 * @class SwitchRadar
 * @brief Specialized class for radar switches with pull-to-unlock functionality.
 *
 * Extends the debounce logic by implementing additional "pull" logic for radar switches that have a lock or emergency position that requires a pull action before rotating.
 */
class SwitchRadar {
private:
    const unsigned int pullDelayMs = 200; ///< Delay in milliseconds to simulate the pull action.
    const char* msg_; ///< DCS BIOS message for the switch state.
    const char* pullMsg_; ///< DCS BIOS message for the pull action.
    char pullPositionNumber_; ///< Switch position number that requires a pull action.
    const byte* pins_; ///< Array of pins for the switch positions.
    char numberOfPins_; ///< Total number of pins (positions).
    char lastState_; ///< Last known stable state.
    bool reverse_; ///< Flag to reverse the reading logic (HIGH/LOW).
    char debounceSteadyState_; ///< The current steady state for debounce logic.
    unsigned long debounceDelay_; ///< Debounce delay in milliseconds.
    unsigned long lastDebounceTime = 0; ///< Timestamp of the last debounce check.

    /**
     * Enum to track the pull state of the switch for switches requiring a pull before rotation.
     */
    enum switchPullStateEnum {
        stOFF_PUSHED = 0, ///< State when the switch is pushed in and off.
        stOFF_PULLED = 1, ///< State when the switch is pulled out and off.
        stON_PULLED = 2 ///< State when the switch is pulled out and on.
    };

    switchPullStateEnum switchPullState_; ///< Current pull state of the switch.
    unsigned long lastSwitchStateTime; ///< Timestamp of the last switch state change.

    /**
     * Reads the current state of the switch by checking each pin's voltage level.
     * @return The current state (position) of the switch.
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
     * Resets the last known state of the switch to its default. Useful in initialization or error recovery.
     */
    void resetState() {
        lastState_ = (lastState_ == 0) ? -1 : 0;
    }

    /**
     * Checks the switch's pins at regular intervals to apply debounce and pull logic, updating the switch state in DCS if a change is detected.
     */
    void pollInput() {
        char state = readState();

        unsigned long now = millis();
        if (state != debounceSteadyState_) {
            lastDebounceTime = now;
            debounceSteadyState_ = state;
        }
        if ((now - lastDebounceTime) >= debounceDelay_) {
            if (now - lastSwitchStateTime > pullDelayMs) {
                if (state != lastState_) {
                    char buf[7];
                    utoa(state, buf, 10);
                    if (state != pullPositionNumber_) {
                        switch (switchPullState_) {
                            case stON_PULLED:                                         ///< to get out of radar knob's locked position, need to pull.
                                if (DcsBios::tryToSendDcsBiosMessage(pullMsg_, "1")) {  //
                                    switchPullState_ = stOFF_PULLED;
                                }
                                break;

                            case stOFF_PULLED:  ///< ready to rotate into position.
                                if (DcsBios::tryToSendDcsBiosMessage(msg_, buf)) {
                                    switchPullState_ = stOFF_PUSHED;
                                    lastSwitchStateTime = millis();  ///< switch animation delay needed.
                                    lastState_ = state;
                                }
                                break;

                            case stOFF_PUSHED:  ///< switch is down just rotate.
                                if (DcsBios::tryToSendDcsBiosMessage(msg_, buf)) {
                                    lastState_ = state;
                                }
                                break;
                        }
                    } else {  // state is in the locked out position, need to handle pull and rotate.
                        switch (switchPullState_) {
                            case stOFF_PUSHED:  ///< need to pull and then later rotate.
                                if (DcsBios::tryToSendDcsBiosMessage(pullMsg_, "1")) {
                                    switchPullState_ = stOFF_PULLED;
                                    lastSwitchStateTime = millis();  // switch animation delay needed.
                                }
                                break;

                            case stOFF_PULLED:  ///< switch is pulled need to rotate.
                                if (DcsBios::tryToSendDcsBiosMessage(msg_, buf)) {
                                    switchPullState_ = stON_PULLED;
                                    lastState_ = state;
                                }
                            case stON_PULLED:
                                // no changes needed.
                                break;
                        }
                    }
                }
            }
        }
    }
public:
    /**
     * Constructor for initializing a radar switch with debounce and pull-to-unlock logic.
     * @param msg DCS BIOS message for the switch state.
     * @param pullMsg DCS BIOS message for the pull action.
     * @param pullPositionNumber Switch position number requiring a pull action.
     * @param pins Array of physical pin numbers for switch positions.
     * @param numberOfPins Number of positions (pins) on the switch.
     * @param reverse Set to true to reverse the reading logic (for normally high switches).
     * @param debounceDelay Debounce time in milliseconds.
     */
    SwitchRadar(const char* msg, const char* pullMsg, char pullPositionNumber, const byte* pins, char numberOfPins, bool reverse = false, unsigned long debounceDelay = 50) {
        msg_ = msg;
        pullMsg_ = pullMsg;
        pins_ = pins;
        pullPositionNumber_ = pullPositionNumber;
        reverse_ = reverse;
        numberOfPins_ = numberOfPins;
        unsigned char i;
        for (i = 0; i < numberOfPins; i++) {
            if (pins[i] != DcsBios::PIN_NC)
                pinMode(pins[i], INPUT_PULLUP);
        }
        lastState_ = readState();
        debounceSteadyState_ = lastState_;
        debounceDelay_ = debounceDelay;
    }

    /**
     * Sets or changes the DCS BIOS messages for the switch and pull actions.
     * @param msg New DCS BIOS message for the switch state.
     * @param pullMsg New DCS BIOS message for the pull action.
     */
    void SetControl(const char* msg, const char* pullMsg) {
        msg_ = msg;
        pullMsg_ = pullMsg;
    }

    /**
     * Public interface to reset the state of the switch.
     */
    void resetThisState() {
        this->resetState();
    }

    /**
     * Public interface to check the input state of the switch and apply debounce and pull logic.
     */
    void pollThisInput() {
        this->pollInput();
    }
};
