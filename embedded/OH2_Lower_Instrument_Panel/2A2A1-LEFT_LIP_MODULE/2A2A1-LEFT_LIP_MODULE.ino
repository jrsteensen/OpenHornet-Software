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
 * @file 2A2A1-LEFT_LIP_MODULE.ino
 * @author Arribe
 * @date 03.21.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the LEFT LIP module.
 *
 * @details
 * 
 *  * **Reference Designator:** 2A2A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 1
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | LI Switch
 * 2   | LO Switch
 * A2  | CTR Switch
 * A1  | RI Switch
 * 4   | RO Switch
 * 3   | UP Switch
 * A0  | DOWN Switch
 * 15  | QTY Switch
 * 6   | ZONE Switch
 * 14  | MODE Switch
 * 7   | ET Switch
 * 16  | VR MAN
 * 8   | VR Auto
 * 10  | VR LDDI - switch in sim is 3 position
 * A9   | VR IFEI Brightness
 * 
 * @todo When Video Record panel's two remaining 3 positon switches are incorporated into OpenHornet, the sketch should be updated to include them.  Currently VR LDDI not programmed.
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#define TXENABLE_PIN 5  ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define LI_SW A3      ///< LI Switch
#define LO_SW 2      ///< LO Switch
#define CTR_SW A2    ///< CTR Switch
#define RI_SW A1     ///< RI Switch
#define RO_SW 4      ///< RO Switch
#define UP_SW 3      ///< UP Switch
#define DOWN_SW A0   ///< DOWN Switch
#define QTY_SW 15    ///< QTY Switch
#define ZONE_SW 6    ///< ZONE Switch
#define MODE_SW 14   ///< MODE Switch
#define ET_SW 7      ///< ET Switch
#define VR_MAN 16    ///< VR MAN
#define VR_AUTO 8    ///< VR Auto
#define VR_LDDI 10   ///< VR LDDI - Switch in SIM is 3 position
#define VR_A_IFEI A9  ///< VR IFEI Brightness

// Connect switches to DCS-BIOS
DcsBios::Potentiometer ifei("IFEI", VR_A_IFEI);
DcsBios::Switch3Pos modeSelectorSw("MODE_SELECTOR_SW", VR_AUTO, VR_MAN);

DcsBios::Switch2Pos ifeiDwnBtn("IFEI_DWN_BTN", DOWN_SW);
DcsBios::Switch2Pos ifeiEtBtn("IFEI_ET_BTN", ET_SW);
DcsBios::Switch2Pos ifeiModeBtn("IFEI_MODE_BTN", MODE_SW);
DcsBios::Switch2Pos ifeiQtyBtn("IFEI_QTY_BTN", QTY_SW);
DcsBios::Switch2Pos ifeiUpBtn("IFEI_UP_BTN", UP_SW);
DcsBios::Switch2Pos ifeiZoneBtn("IFEI_ZONE_BTN", ZONE_SW);

DcsBios::ActionButton sjCtrToggle("SJ_CTR", "TOGGLE", CTR_SW);
DcsBios::ActionButton sjLiToggle("SJ_LI", "TOGGLE", LI_SW);
DcsBios::ActionButton sjLoToggle("SJ_LO", "TOGGLE", LO_SW);
DcsBios::ActionButton sjRiToggle("SJ_RI", "TOGGLE", RI_SW);
DcsBios::ActionButton sjRoToggle("SJ_RO", "TOGGLE", RO_SW);

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
}