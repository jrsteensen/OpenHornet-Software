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
 * @file OpenHornetDcsMission.h
 * @author lahirunirmalx
 * @date 10.14.2026
 * @version 0.2.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Tells whether a DCS mission is running, from the DCS-BIOS aircraft name and heartbeat.
 *
 * @details
 * Used by the DDI (1A3, 1A9) and AMPCD (2A3A1) sketches to keep the backlight diagnostic
 * mode of OpenHornetDiagnostics.h out of a running mission. Include it once in the sketch,
 * after DcsBios.h, and call ohDcsMissionActive() every loop().
 *
 * How it works:
 *  * DCS-BIOS exports the aircraft name (_ACFT_NAME, address 0x0000) in every frame:
 *    "FA-18C_hornet" in the cockpit, "NONE" while no aircraft is selected, and an empty
 *    name in the single frame it sends when the mission ends.
 *  * DCS-BIOS also exports an update counter (_UPDATE_COUNTER, address 0xFFFE) that
 *    changes every frame. It is used as a heartbeat.
 *  * A mission counts as active while the last aircraft name was a real aircraft and a
 *    heartbeat arrived within OH_DCS_MISSION_TIMEOUT_MS. The timeout covers DCS crashing
 *    or being killed without the end-of-mission frame. A mission paused for longer than
 *    the timeout also counts as inactive until it is resumed.
 *
 * This is a lighter cousin of embedded/OHC_Common/DCS_State_Checker.h: it only needs the
 * name and heartbeat buffers (no RPM or weight-on-wheels data), and it lives under
 * libraries/ so the Arduino IDE can find it.
 *
 * **Usage:**
 * @code
 * #include "DcsBios.h"
 * #include "OpenHornetDcsMission.h"
 *
 * void loop() {
 *   DcsBios::loop();
 *   bool inMission = ohDcsMissionActive();
 * }
 * @endcode
 */

#ifndef OPENHORNET_DCS_MISSION_H
#define OPENHORNET_DCS_MISSION_H

#include "Arduino.h"
#include "DcsBios.h"
#include <string.h>

#ifndef OH_DCS_MISSION_TIMEOUT_MS
#define OH_DCS_MISSION_TIMEOUT_MS 30000UL  ///< A mission counts as over after this long without a DCS-BIOS heartbeat, in ms.
#endif

static bool ohDcsMissionHasAircraft_ = false;     ///< The last aircraft name named a real aircraft.
static unsigned long ohDcsMissionLastFrame_ = 0;  ///< millis() of the last DCS-BIOS frame seen.

/**
 * @brief DCS-BIOS callback for the aircraft name.
 * @param name Up to 24 characters: the aircraft, "NONE" without an aircraft, empty at mission end.
 */
static void ohDcsMissionOnAcftName(char* name) {
  ohDcsMissionHasAircraft_ = (name != NULL) && (name[0] != '\0') && (name[0] != ' ')
                             && (strncmp(name, "NONE", 4) != 0);
  if (ohDcsMissionHasAircraft_) {
    ohDcsMissionLastFrame_ = millis();
  }
}
// DCS-BIOS listener for the 24 character aircraft name at address 0x0000.
/// @cond DCSBIOS_LISTENERS
DcsBios::StringBuffer<24> ohDcsMissionAcftNameBuffer(MetadataStart_ACFT_NAME_A, ohDcsMissionOnAcftName);
/// @endcond

/**
 * @brief DCS-BIOS callback for the update counter. Records the heartbeat time.
 * @param newValue Counter value, unused; it changes every frame.
 */
static void ohDcsMissionOnUpdateCounter(unsigned int newValue) {
  (void)newValue;
  ohDcsMissionLastFrame_ = millis();
}
// DCS-BIOS listener for the per-frame update counter at address 0xFFFE (heartbeat).
/// @cond DCSBIOS_LISTENERS
DcsBios::IntegerBuffer ohDcsMissionUpdateCounterBuffer(MetadataEnd_UPDATE_COUNTER, ohDcsMissionOnUpdateCounter);
/// @endcond

/**
 * @brief Whether a DCS mission is running right now.
 * @return true while DCS-BIOS last reported a real aircraft and a heartbeat arrived
 *         within OH_DCS_MISSION_TIMEOUT_MS.
 */
static inline bool ohDcsMissionActive() {
  return ohDcsMissionHasAircraft_ && (millis() - ohDcsMissionLastFrame_) < OH_DCS_MISSION_TIMEOUT_MS;
}

#endif  // OPENHORNET_DCS_MISSION_H
