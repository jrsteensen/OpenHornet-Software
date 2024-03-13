/**
 * @file 5A7A1-SNSR_PANEL.h
 * @author Arribe
 * @date 03.13.2024
 *
 * @brief Header file for the SNSR panel's INS, and Radar rotary / multiposition switches.
*/





#include <math.h>
#include "Arduino.h"
#include "DcsBios.h"

/**
* @brief Adds  debounce logic to the SwitchMultiPos method for DCS to ensure that while the knob is rotating its output doesn't bounce to the unconnected / default position in the middle of a step.
*
* @todo If/When https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/pull/56 is accepted by DCS Skunkworks remove the SwitchMultiPosDebounce class.
*/

class SwitchMultiPosDebounce {
private:
  const char* msg_;
  const byte* pins_;
  char numberOfPins_;
  char lastState_;
  bool reverse_;
  char debounceSteadyState_;
  unsigned long debounceDelay_;
  unsigned long lastDebounceTime = 0;

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
  void resetState() {
    lastState_ = (lastState_ == 0) ? -1 : 0;
  }
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

  void SetControl(const char* msg) {
    msg_ = msg;
  }

  void resetThisState() {
    this->resetState();
  }

  void pollThisInput() {
    this->pollInput();
  }
};

/**
* @brief Adds  debounce logic to the SwitchMultiPos method for DCS to ensure that while the knob is rotating its output doesn't bounce to the unconnected / default position in the middle of a step.
* In addition the class implements the "pull" for the radar knob so that it may turn into and out of the EMERG position.
*
*/

class SwitchRadar {
private:
  const unsigned int pullDelayMs = 200;
  const char* msg_;
  const char* pullMsg_;
  char pullPositionNumber_;
  const byte* pins_;
  char numberOfPins_;
  char lastState_;
  bool reverse_;
  char debounceSteadyState_;
  unsigned long debounceDelay_;
  unsigned long lastDebounceTime = 0;

  enum switchPullStateEnum {
    stOFF_PUSHED = 0,
    stOFF_PULLED = 1,
    stON_PULLED = 2
  };

  switchPullStateEnum switchPullState_;
  unsigned long lastSwitchStateTime;

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
  void resetState() {
    lastState_ = (lastState_ == 0) ? -1 : 0;
  }
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

  void SetControl(const char* msg, const char* pullMsg) {
    msg_ = msg;
    pullMsg_ = pullMsg;
  }

  void resetThisState() {
    this->resetState();
  }

  void pollThisInput() {
    this->pollInput();
  }
};