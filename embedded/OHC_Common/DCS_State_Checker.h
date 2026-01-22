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
#include <string.h>
#include <stdlib.h>

enum class DcsState {
  EXITED,       // DCS not running, in menu, or crashed
  PAUSED,       // Mission loaded but paused (no heartbeat for 10s)
  GROUND_COLD,  // On ground, at least one engine below 50% RPM
  GROUND_HOT,   // On ground, both engines above 50% RPM
  AIRBORNE      // In flight (weight off wheels)
};

// ─────────────────────────────────────────────────────────────────────────────
// State tracking variables
// ─────────────────────────────────────────────────────────────────────────────
static char         acftName[25] = "";    // Aircraft name (24 chars + null terminator)
static int          rpmLeft = 0;          // Left engine RPM percentage
static int          rpmRight = 0;         // Right engine RPM percentage
static unsigned int wowLeft = 1;          // Weight on wheels left  (1=ground, 0=air)
static unsigned int wowRight = 1;         // Weight on wheels right (1=ground, 0=air)

// Heartbeat tracking
static unsigned int  currDcsHeartbeat = 0;
static unsigned int  prevDcsHeartbeat = 0;
static unsigned long dcsLastUpdateTime = 0;

// Timing constants
static const unsigned long PAUSED_TIMEOUT_MS = 10000;    // 10 seconds  -> PAUSED
static const unsigned long EXITED_TIMEOUT_MS = 1800000;  // 30 minutes  -> EXITED (crash detection)
static const int           RPM_THRESHOLD = 50;           // RPM threshold for hot/cold

// ─────────────────────────────────────────────────────────────────────────────
// State determination function
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief   Determine the current DCS simulation state
 * @details Evaluates aircraft name, heartbeat, RPM, and weight-on-wheels data
 *          to determine the current state of the simulation.
 * @return  DcsState enum indicating current state
 * @note    Must be called regularly (every loop cycle) for accurate timing
 */
DcsState getDcsState() {
  unsigned long currentTime = millis();
  unsigned long timeSinceUpdate = currentTime - dcsLastUpdateTime;
  
  // Update heartbeat tracking if changed
  if (currDcsHeartbeat != prevDcsHeartbeat) {
    dcsLastUpdateTime = currentTime;
    prevDcsHeartbeat = currDcsHeartbeat;
    timeSinceUpdate = 0;
  }
  
  // Check if aircraft name is empty (DCS in menu or not running)
  bool hasAircraft = (acftName[0] != '\0');
  
  // ── EXITED: No aircraft OR timeout exceeded (crash detection) ──
  if (!hasAircraft || timeSinceUpdate >= EXITED_TIMEOUT_MS) {
    return DcsState::EXITED;
  }
  
  // ── PAUSED: Aircraft loaded but heartbeat stalled ──
  if (timeSinceUpdate >= PAUSED_TIMEOUT_MS) {
    return DcsState::PAUSED;
  }
  
  // ── AIRBORNE: Both wheels off ground (engine state irrelevant) ──
  if (wowLeft == 0 && wowRight == 0) {
    return DcsState::AIRBORNE;
  }
  
  // ── On ground: Determine hot/cold by engine RPM ──
  // GROUND_HOT: Both engines >= 50% RPM
  // GROUND_COLD: At least one engine < 50% RPM
  if (rpmLeft >= RPM_THRESHOLD && rpmRight >= RPM_THRESHOLD) {
    return DcsState::GROUND_HOT;
  } else {
    return DcsState::GROUND_COLD;
  }
}

/**
 * @brief   Legacy compatibility wrapper
 * @return  true if DCS is running (any state except EXITED)
 */
bool checkDcsRunning() {
  return getDcsState() != DcsState::EXITED;
}



// ─────────────────────────────────────────────────────────────────────────────
// DCS-BIOS Callbacks
// ─────────────────────────────────────────────────────────────────────────────

// Heartbeat counter - updates every DCS-BIOS cycle
void onDcsUpdateCounterChange(unsigned int newValue) {
  currDcsHeartbeat = newValue;
}
DcsBios::IntegerBuffer dcsUpdateCounterBuffer(0xfffe, 0x00ff, 0, onDcsUpdateCounterChange);

// Aircraft name - empty when no mission or DCS in menu
void onAcftNameChange(char* newValue) {
  if (newValue != nullptr && newValue[0] != '\0') {
    strncpy(acftName, newValue, sizeof(acftName) - 1);
    acftName[sizeof(acftName) - 1] = '\0';
  } else {
    acftName[0] = '\0';
  }
}
DcsBios::StringBuffer<24> AcftNameBuffer(MetadataStart_ACFT_NAME_A, onAcftNameChange);

// Left engine RPM (string "0"-"100")
void onIfeiRpmLChange(char* newValue) {
  rpmLeft = (newValue != nullptr && newValue[0] != '\0') ? atoi(newValue) : 0;
}
DcsBios::StringBuffer<3> ifeiRpmLBuffer(FA_18C_hornet_IFEI_RPM_L_A, onIfeiRpmLChange);

// Right engine RPM (string "0"-"100")
void onIfeiRpmRChange(char* newValue) {
  rpmRight = (newValue != nullptr && newValue[0] != '\0') ? atoi(newValue) : 0;
}
DcsBios::StringBuffer<3> ifeiRpmRBuffer(FA_18C_hornet_IFEI_RPM_R_A, onIfeiRpmRChange);

// Weight on wheels - left gear (1=on ground, 0=airborne)
void onExtWowLeftChange(unsigned int newValue) {
  wowLeft = newValue;
}
DcsBios::IntegerBuffer extWowLeftBuffer(FA_18C_hornet_EXT_WOW_LEFT, onExtWowLeftChange);

// Weight on wheels - right gear (1=on ground, 0=airborne)
void onExtWowRightChange(unsigned int newValue) {
  wowRight = newValue;
}
DcsBios::IntegerBuffer extWowRightBuffer(FA_18C_hornet_EXT_WOW_RIGHT, onExtWowRightChange);

#endif // DCS_STATE_CHECKER_H