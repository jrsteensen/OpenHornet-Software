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
 * @file OpenHornetDiagnostics.h
 * @author lahirunirmalx
 * @date 10.14.2026
 * @version 0.1.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Backlight power-on test, DCS sync indication and diagnostic mode for
 *        panels with a PWM backlight and pushbuttons (DDIs, AMPCD).
 *
 * @details
 * Implements the behaviour requested in OpenHornet-Software issue #117:
 *
 *  * **Power on:** the backlight fades from off to full and back to off.
 *  * **DCS sync:** the first time a DCS-BIOS value arrives the backlight flashes
 *    three times, then follows the brightness DCS sends.
 *  * **Diagnostic mode:** hold any three pushbuttons for three seconds to enter.
 *    The backlight goes to full. Every pushbutton, rotary switch, encoder or
 *    rocker movement blinks the backlight off and on. Hold any three pushbuttons
 *    for three seconds again to exit. Diagnostic mode works with or without DCS
 *    running, and DCS-BIOS messages keep flowing while it is active.
 *
 * All timing is non-blocking and driven by millis(), so DcsBios::loop() keeps
 * running. The helper never touches I2C; the sketch keeps reading its own
 * buttons and passes in the number currently pressed.
 *
 * **Usage:**
 * @code
 * OhBacklightDiagnostics backlight(BACKLIGHT_PIN);
 *
 * void onInstrIntLtChange(unsigned int newValue) {
 *   backlight.onDcsBrightness(map(newValue, 0, 65535, 0, 255));
 * }
 *
 * void setup() {
 *   backlight.watchDigitalPin(ROTARY_PIN);   // optional, repeat per input pin
 *   backlight.watchAnalogPin(POT_PIN);       // optional, one analog input
 *   backlight.begin();
 * }
 *
 * void loop() {
 *   // ... read buttons, count how many are pressed, note if any input changed
 *   backlight.update(pressedCount, inputChanged);
 * }
 * @endcode
 *
 * The timing constants below can be overridden by defining the OH_DIAG_* macro
 * before the include.
 */

#ifndef OPENHORNET_DIAGNOSTICS_H
#define OPENHORNET_DIAGNOSTICS_H

#include "Arduino.h"

#ifndef OH_DIAG_FADE_MS
#define OH_DIAG_FADE_MS 1000  ///< Duration of each half of the power-on fade (up, then down) in ms.
#endif
#ifndef OH_DIAG_SYNC_FLASHES
#define OH_DIAG_SYNC_FLASHES 3  ///< Number of backlight flashes when DCS first syncs.
#endif
#ifndef OH_DIAG_SYNC_FLASH_MS
#define OH_DIAG_SYNC_FLASH_MS 200  ///< On time and off time of each sync flash in ms.
#endif
#ifndef OH_DIAG_HOLD_BUTTONS
#define OH_DIAG_HOLD_BUTTONS 3  ///< Number of pushbuttons that must be held to toggle diagnostic mode.
#endif
#ifndef OH_DIAG_HOLD_MS
#define OH_DIAG_HOLD_MS 3000  ///< How long the pushbuttons must be held to toggle diagnostic mode, in ms.
#endif
#ifndef OH_DIAG_ACTIVITY_FLASH_MS
#define OH_DIAG_ACTIVITY_FLASH_MS 150  ///< Backlight off time per input event in diagnostic mode, in ms.
#endif
#ifndef OH_DIAG_MAX_WATCHED_PINS
#define OH_DIAG_MAX_WATCHED_PINS 12  ///< Maximum number of digital pins watched for activity in diagnostic mode.
#endif
#ifndef OH_DIAG_ANALOG_THRESHOLD
#define OH_DIAG_ANALOG_THRESHOLD 8  ///< Minimum analogRead() change (0-1023) counted as activity.
#endif

/**
 * @brief Non-blocking backlight state machine: power-on fade, DCS sync flash and diagnostic mode.
 */
class OhBacklightDiagnostics {
public:
  /// Backlight controller states.
  enum State : uint8_t {
    FADE_UP,     ///< Power-on: fading from off to full.
    FADE_DOWN,   ///< Power-on: fading from full back to off.
    WAIT_SYNC,   ///< Dark, waiting for the first DCS-BIOS value.
    SYNC_FLASH,  ///< Flashing to announce DCS sync.
    NORMAL,      ///< Backlight follows the brightness sent by DCS.
    DIAGNOSTIC   ///< Diagnostic mode: full bright, blinks on every input.
  };

  /**
   * @param backlightPin PWM capable pin driving the backlight.
   */
  explicit OhBacklightDiagnostics(uint8_t backlightPin)
    : pin_(backlightPin), state_(WAIT_SYNC), dcsLevel_(0), synced_(false), syncAnnounced_(false),
      holdArmed_(true), holding_(false), holdStart_(0), phaseStart_(0), flashUntil_(0),
      watchedCount_(0), watchedMask_(0), analogPin_(0xFF), lastAnalog_(0) {}

  /**
   * @brief Configure the backlight pin and start the power-on fade. Call from setup().
   */
  void begin() {
    pinMode(pin_, OUTPUT);
    write(0);
    phaseStart_ = millis();
    state_ = FADE_UP;
  }

  /**
   * @brief Add a digital input pin to watch for activity while in diagnostic mode.
   * @param pin Pin number. The sketch (or DCS-BIOS control) is responsible for its pinMode.
   * @note Up to OH_DIAG_MAX_WATCHED_PINS pins; extra pins are ignored.
   */
  void watchDigitalPin(uint8_t pin) {
    if (watchedCount_ < OH_DIAG_MAX_WATCHED_PINS) {
      watchedPins_[watchedCount_++] = pin;
    }
  }

  /**
   * @brief Set one analog input (potentiometer) to watch for activity while in diagnostic mode.
   * @param pin Analog pin number.
   */
  void watchAnalogPin(uint8_t pin) {
    analogPin_ = pin;
  }

  /**
   * @brief Feed the brightness received from DCS-BIOS. The first call counts as "synced with DCS".
   * @param level 0-255 backlight level.
   */
  void onDcsBrightness(uint8_t level) {
    dcsLevel_ = level;
    synced_ = true;
    if (state_ == NORMAL) {
      write(level);
    }
  }

  /**
   * @brief Run the state machine. Call once per loop().
   * @param pressedButtonCount Number of pushbuttons currently pressed (raw, undebounced is fine).
   * @param inputChanged True if the sketch saw any input change this loop (buttons, rockers).
   *        Watched pins are polled internally and do not need to be reported here.
   */
  void update(uint8_t pressedButtonCount, bool inputChanged = false) {
    unsigned long now = millis();
    updateHold(pressedButtonCount, now);

    switch (state_) {
      case FADE_UP: {
        unsigned long elapsed = now - phaseStart_;
        if (elapsed >= OH_DIAG_FADE_MS) {
          write(255);
          phaseStart_ = now;
          state_ = FADE_DOWN;
        } else {
          write((uint8_t)((elapsed * 255UL) / OH_DIAG_FADE_MS));
        }
        break;
      }
      case FADE_DOWN: {
        unsigned long elapsed = now - phaseStart_;
        if (elapsed >= OH_DIAG_FADE_MS) {
          write(0);
          enterIdle(now);
        } else {
          write((uint8_t)(255 - (elapsed * 255UL) / OH_DIAG_FADE_MS));
        }
        break;
      }
      case WAIT_SYNC:
        if (synced_) {
          enterIdle(now);
        }
        break;
      case SYNC_FLASH: {
        unsigned long slot = (now - phaseStart_) / OH_DIAG_SYNC_FLASH_MS;
        if (slot >= (unsigned long)(2 * OH_DIAG_SYNC_FLASHES)) {
          state_ = NORMAL;
          write(dcsLevel_);
        } else {
          write((slot & 1) ? 0 : 255);
        }
        break;
      }
      case NORMAL:
        break;  // onDcsBrightness() drives the pin.
      case DIAGNOSTIC: {
        bool flashing = (long)(flashUntil_ - now) > 0;
        if (pollWatchedInputs() || inputChanged) {
          if (!flashing) {
            flashUntil_ = now + OH_DIAG_ACTIVITY_FLASH_MS;
            flashing = true;
          }
        }
        write(flashing ? 0 : 255);
        break;
      }
    }
  }

  /// @return true while diagnostic mode is active.
  bool isDiagnosticActive() const { return state_ == DIAGNOSTIC; }

  /// @return current controller state.
  State state() const { return state_; }

private:
  void write(uint8_t level) { analogWrite(pin_, level); }

  /// Decide what to do once the power-on fade or diagnostic mode ends.
  void enterIdle(unsigned long now) {
    if (synced_ && !syncAnnounced_) {
      syncAnnounced_ = true;
      phaseStart_ = now;
      state_ = SYNC_FLASH;
      write(255);
    } else if (synced_) {
      state_ = NORMAL;
      write(dcsLevel_);
    } else {
      state_ = WAIT_SYNC;
      write(0);
    }
  }

  /// Hold-to-toggle detection with re-arm: buttons must be released between toggles.
  void updateHold(uint8_t pressed, unsigned long now) {
    if (pressed >= OH_DIAG_HOLD_BUTTONS) {
      if (!holdArmed_) {
        return;  // still held from the previous toggle
      }
      if (!holding_) {
        holding_ = true;
        holdStart_ = now;
      } else if (now - holdStart_ >= OH_DIAG_HOLD_MS) {
        holdArmed_ = false;
        holding_ = false;
        toggleDiagnostic(now);
      }
    } else {
      holding_ = false;
      holdArmed_ = true;
    }
  }

  void toggleDiagnostic(unsigned long now) {
    if (state_ == DIAGNOSTIC) {
      enterIdle(now);
    } else {
      state_ = DIAGNOSTIC;
      flashUntil_ = now;
      snapshotWatchedInputs();
      write(255);
    }
  }

  uint16_t readWatchedMask() const {
    uint16_t mask = 0;
    for (uint8_t i = 0; i < watchedCount_; i++) {
      if (digitalRead(watchedPins_[i])) {
        mask |= (uint16_t)1 << i;
      }
    }
    return mask;
  }

  void snapshotWatchedInputs() {
    watchedMask_ = readWatchedMask();
    if (analogPin_ != 0xFF) {
      lastAnalog_ = analogRead(analogPin_);
    }
  }

  /// @return true if any watched digital pin changed or the analog input moved past the threshold.
  bool pollWatchedInputs() {
    bool changed = false;
    uint16_t mask = readWatchedMask();
    if (mask != watchedMask_) {
      watchedMask_ = mask;
      changed = true;
    }
    if (analogPin_ != 0xFF) {
      int value = analogRead(analogPin_);
      if (abs(value - lastAnalog_) >= OH_DIAG_ANALOG_THRESHOLD) {
        lastAnalog_ = value;
        changed = true;
      }
    }
    return changed;
  }

  uint8_t pin_;
  State state_;
  uint8_t dcsLevel_;
  bool synced_;
  bool syncAnnounced_;
  bool holdArmed_;
  bool holding_;
  unsigned long holdStart_;
  unsigned long phaseStart_;
  unsigned long flashUntil_;
  uint8_t watchedPins_[OH_DIAG_MAX_WATCHED_PINS];
  uint8_t watchedCount_;
  uint16_t watchedMask_;
  uint8_t analogPin_;
  int lastAnalog_;
};

#endif  // OPENHORNET_DIAGNOSTICS_H
