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
 * @file 3A2A1-SEAT_CONTROLS.ino
 * @author Arribe
 * @date 03.13.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet fully tested on hardware.\n  *The ejection handle pull, seat arm/disarm, and manual release override have been tested on hardware.
 * The seat up/down, and shoulder harness lock/unlock was validated via BORT by observing DCS output values and ensuring those inputs line up with the DCS Bios Switch codes' inputs.*
 *
 * @brief Controls all seat controls and handles.
 *
 * @details
 * 
 *  * **Reference Designator:** 3A2A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 9
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Seat Harness Lock - forward position
 * 2   | Seat Harness Unlock - aft position
 * A2  | Seat Up - switch aft 
 * 3   | Seat Down - switch forward 
 * A1  | Eject
 * 15  | Seat Arm
 * 6   | Harness Release
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 9 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5 ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"

// Define pins for DCS-BIOS per interconnect diagram.
 #define SEAT_HARNESS_LOCK A3  ///< Seat Harness Lock - forward position
 #define SEAT_HARNESS_UNLOCK 2   ///< Seat Harness Unlock - aft position
 #define SEAT_UP A2  ///< Seat Up - switch aft
 #define SEAT_DOWN 3   ///< Seat Down - switch forward 
 #define EJECT A1  ///< Eject
 #define SEAT_ARM 15  ///< Seat Arm
 #define HARNESS_RELEASE 6   ///< Harness Release

// Connect switches to DCS-BIOS 
DcsBios::Switch2Pos ejectionHandleSw("EJECTION_HANDLE_SW", EJECT);
DcsBios::Switch2Pos ejectionSeatArmed("EJECTION_SEAT_ARMED", SEAT_ARM, true); ///< For proper control alignment the swith movement needs to be inverted. The seat is armed when DCS Bios value = 0, but physical switch is normally open.
DcsBios::Switch2Pos ejectionSeatMnlOvrd("EJECTION_SEAT_MNL_OVRD", HARNESS_RELEASE);
DcsBios::Switch3Pos seatHeightSw("SEAT_HEIGHT_SW", SEAT_UP, SEAT_DOWN ); ///< Verified order via BORT by observing DCS output value

const byte shldrHarnessSwPins[2] = {SEAT_HARNESS_UNLOCK, SEAT_HARNESS_LOCK}; ///< Verified order via BORT by observing DCS output value
DcsBios::SwitchMultiPos shldrHarnessSw("SHLDR_HARNESS_SW", shldrHarnessSwPins, 2);


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
