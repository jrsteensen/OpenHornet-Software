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
 * @file 2A4A1-RWR_CONTROL_PANEL.ino
 * @author Arribe
 * @date 03.21.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the RWR CONTROL panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 2A4A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 3
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | RWR Audio Volume
 * 2   | POWER
 * A2  | RWR Dimmer
 * 3   | RWR LIMIT Switch
 * A1  | SPECIAL
 * 4   | RWR Offset
 * A0  | BIT Switch
 * 15  | RWR Dis I
 * 6   | RWR Dis A
 * 14  | RWR Dis U
 * 7   | RWR Dis F
 * 
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 3 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL  ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL  ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/


#include "DcsBios.h"
#include "5A7A1-SNSR_PANEL.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define RWR_A_AUDIO A3  ///< RWR Audio Volume
#define POWER_SW 2      ///< POWER
#define RWR_A_DMR A2    ///< RWR Dimmer
#define RWR_LIMIT_SW 3  ///< RWR LIMIT Switch
#define SPECIAL A1      ///< SPECIAL
#define RWR_OFFSET 4    ///< RWR Offset
#define BIT_SW A0       ///< BIT Switch
#define RWR_DIS_I 15    ///< RWR Dis I
#define RWR_DIS_A 6     ///< RWR Dis A
#define RWR_DIS_U 14    ///< RWR Dis U
#define RWR_DIS_F 7     ///< RWR Dis F

// Connect switches to DCS-BIOS
DcsBios::Potentiometer rwrAudioCtrl("RWR_AUDIO_CTRL", RWR_A_AUDIO);
DcsBios::Switch2Pos rwrBitBtn("RWR_BIT_BTN", BIT_SW);
DcsBios::Switch2Pos rwrDisplayBtn("RWR_DISPLAY_BTN", RWR_LIMIT_SW);
DcsBios::Potentiometer rwrDmrCtrl("RWR_DMR_CTRL", RWR_A_DMR);
DcsBios::Switch2Pos rwrOffsetBtn("RWR_OFFSET_BTN", RWR_OFFSET);
DcsBios::ActionButton rwrPowerBtnToggle("RWR_POWER_BTN", "TOGGLE", POWER_SW);
DcsBios::Switch2Pos rwrSpecialBtn("RWR_SPECIAL_BTN", SPECIAL);

const byte rwrDisTypeSwPins[5] = { DcsBios::PIN_NC, RWR_DIS_I, RWR_DIS_A, RWR_DIS_U, RWR_DIS_F };
/// @todo Replace with DCSBios version of DcsBios::SwitchMultiPos if/when fixed.
SwitchMultiPosDebounce rwrDisTypeSw("RWR_DIS_TYPE_SW", rwrDisTypeSwPins, 5, false, 100);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

   ///@todo If/When DCS Skunkworks fixes the multiposition switch remove the rwrDisTypeSw.pollThisInput(); call.
   rwrDisTypeSw.pollThisInput();
}