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
 * @file     DCS_State_Checker.h
 * @author   Ulukaii
 * @date     18.01.2026
 * @version  0.8.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief    DCS State Monitor for OpenHornet
 * @details  Monitors DCS-BIOS communication and aircraft telemetry to determine
 *           the current simulation state.
 *
 *           States:
 *           - EXITED:      DCS not running, in menu, or crashed (no heartbeat for 30min)
 *           - PAUSED:      Mission loaded but paused (no heartbeat for 10s)
 *           - GROUND_COLD: On ground, at least one engine below 50% RPM
 *           - GROUND_HOT:  On ground, both engines above 50% RPM
 *           - AIRBORNE:    In flight (weight off wheels)
 *
 *           Usage:
 *           (1) Include this header in your .ino file
 *           (2) Call getDcsState() in your loop to get the current state
 */

#ifndef DCS_STATE_CHECKER_H
#define DCS_STATE_CHECKER_H

#include <DcsBios.h>

// ─────────────────────────────────────────────────────────────────────────────
// Note: This is a lightweight variant of DCS_State_Checker for use in the
// 2A13-BACKLIGHT_CONTROLLER. It uses heartbeat-only detection to avoid the
// StringBuffer and extra IntegerBuffer overhead of the full version, which
// would exceed the Arduino Mega 2560's SRAM budget for this sketch.
// RPM/WoW/aircraft-name states are not needed here; Board.h only requires
// the EXITED / PAUSED / active distinction.
// ─────────────────────────────────────────────────────────────────────────────

enum class DcsState {
  EXITED,       // DCS not running, in menu, or crashed (no heartbeat for 30 min)
  PAUSED,       // Mission loaded but paused (no heartbeat for 10 s)
  GROUND_COLD,  // Heartbeat active - DCS is running (used as generic "active" state)
  GROUND_HOT,   // (not used in this variant)
  AIRBORNE      // (not used in this variant)
};

// ─────────────────────────────────────────────────────────────────────────────
// Heartbeat tracking
// ─────────────────────────────────────────────────────────────────────────────
static unsigned int  currDcsHeartbeat = 0;
static unsigned int  prevDcsHeartbeat = 0;
static unsigned long dcsLastUpdateTime = 0;

static const unsigned long PAUSED_TIMEOUT_MS = 10000;    // 10 seconds  -> PAUSED
static const unsigned long EXITED_TIMEOUT_MS = 1800000;  // 30 minutes  -> EXITED

// ─────────────────────────────────────────────────────────────────────────────
// State determination function
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Determine the current DCS connection state (heartbeat-only variant)
 * @details Returns EXITED after 30 min without heartbeat, PAUSED after 10 s,
 *          or GROUND_COLD when heartbeat is active (DCS running).
 * @return  DcsState enum indicating current state
 * @note    Must be called regularly (every loop cycle) for accurate timing
 */
DcsState getDcsState() {
  unsigned long currentTime = millis();
  unsigned long timeSinceUpdate = currentTime - dcsLastUpdateTime;

  if (currDcsHeartbeat != prevDcsHeartbeat) {
    dcsLastUpdateTime = currentTime;
    prevDcsHeartbeat = currDcsHeartbeat;
    timeSinceUpdate = 0;
  }

  if (timeSinceUpdate >= EXITED_TIMEOUT_MS) return DcsState::EXITED;
  if (timeSinceUpdate >= PAUSED_TIMEOUT_MS) return DcsState::PAUSED;
  return DcsState::AIRBORNE;
}

/**
 * @brief   Legacy compatibility wrapper
 * @return  true if DCS is running (any state except EXITED)
 */
bool checkDcsRunning() {
  return getDcsState() != DcsState::EXITED;
}

// ─────────────────────────────────────────────────────────────────────────────
// DCS-BIOS Callback — heartbeat counter only
// ─────────────────────────────────────────────────────────────────────────────
void onDcsUpdateCounterChange(unsigned int newValue) {
  currDcsHeartbeat = newValue;
}
DcsBios::IntegerBuffer dcsUpdateCounterBuffer(0xfffe, 0x00ff, 0, onDcsUpdateCounterChange);

#endif // DCS_STATE_CHECKER_H