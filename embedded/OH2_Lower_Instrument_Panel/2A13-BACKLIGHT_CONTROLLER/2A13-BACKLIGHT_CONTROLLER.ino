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
 * @file 2A13-BACKLIGHT_CONTROLLER.ino
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Controls the backlighting (and some annunciators) for the entire pit.
 *
 * @details
 * 
 *  * **Reference Designator:** 2A13
 *  * **Intended Board:** ABSIS BACKLIGHT CONTROLLER
 *  * **RS485 Bus Address:** NA
 * 
 *  * **Intended Board:**
 * ABSIS BACKLIGHT CONTROLLER
 * 
 *  * ** Processor **
 *  Arduino Mega + Backlight Shield
 * 
 * **Wiring diagram:**
 * 
 * PIN | Function
 * --- | ---
 * 13  | J2 LIP_BL_CH1
 * 12  | J3 LIP_BL_CH2
 * 11  | J4 UIP_BL_CH1
 * 10  | J5 UIP_BL_CH2
 * 9   | J6 LC_BL_CH1
 * 8   | J7 LC_BL_CH2
 * 7   | J8 RC_BL_CH1
 * 6   | J9 RC_BL_CH2
 * 5   | J10 NC
 * 4   | J11 NC
 * 24  | J14 SIMPWR_BLM_A
 * 23  | J14 SIMPWR_BLM_B
 * 22  | J14 SIMPWR_PUSH
 * SDA | TEMP SNSR
 * SCL | TEMP SNSR
 * 2   | J12 & J13 Cooling fan headers.
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

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"


// Define pins for DCS-BIOS per interconnect diagram, and Bakclight control kicad sketch.
#define LIP_BL_CH1 13  ///< J2 LIP_BL_CH1
#define LIP_BL_CH2 12  ///< J3 LIP_BL_CH2
#define UIP_BL_CH1 11  ///< J4 UIP_BL_CH1
#define UIP_BL_CH2 10  ///< J5 UIP_BL_CH2
#define LC_BL_CH1 9    ///< J6 LC_BL_CH1
#define LC_BL_CH2 8    ///< J7 LC_BL_CH2
#define RC_BL_CH1 7    ///< J8 RC_BL_CH1
#define RC_BL_CH2 6    ///< J9 RC_BL_CH2
//#define 5   ///< J10 NC
//#define 4   ///< J11 NC
#define SIMPWR_BLM_A 24  ///< J14 SIMPWR_BLM_A
#define SIMPWR_BLM_B 23  ///< J14 SIMPWR_BLM_B
#define SIMPWR_PUSH 22   ///< J14 SIMPWR_PUSH
//#define SDA | TEMP SNSR
//#define SCL | TEMP SNSR
#define COOLING_FANS 2  ///< J12 & J13 Cooling fan headers.

#define BRIGHTNESS 50  ///< Brightness value used by the panels.

#include "2A13-BACKLIGHT_CONTROLLER.h"

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();
  OpenHornet::setup();
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
