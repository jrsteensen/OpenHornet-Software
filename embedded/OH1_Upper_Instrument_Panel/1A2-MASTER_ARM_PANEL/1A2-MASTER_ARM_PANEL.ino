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
 * @file 1A2-MASTER_ARM_PANEL.ino
 * @author Sandra Carroll (<a href="mailto:smgvbest@gmail.com">smgvbest@gmail.com</a>), Arribe
 * @date 3.02.2024
 * @version 0.0.2
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the MASTER ARM panel. 
 *
 * @details This sketch is for the UIP Master Arm Panel.
 * 
 *  * **Reference Designator:** 1A2
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 1
 *  
 * ###Wiring diagram:
 * 
 * PIN | Function
 * --- | ---
 * A1  | Emergency Jettison Switch
 * A2  | Air to Ground Select
 * A3  | Ready/Discharge Switch
 * D2  | Air to Air Select
 * D3  | Master Arm Switch
 *
 *  
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
 *   #define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#include "Joystick.h"

//Declare pins for DCS-BIOS per interconnect diagram.
#define E_JETT_SW     A1 ///< Emergency Jettison Switch
#define AG_SW         A2 ///< Air to Ground Select
#define READY_SW      A3 ///< Ready/Discharge Switch
#define AA_SW         2 ///< Air to Air Select
#define MSTR_ARM_SW   3 ///< Master Arm Switch

bool inputsAsHID = false; ///< If false, initialize inputs as DCS-BIOS. If true, initialize them as joystick buttons.
                          ///< This is not a #define because we may want to change to change it via serial or similar
                          ///< in the future.

if (inputsAsHID = false) {
    // Connect switches to DCS-BIOS
    DcsBios::Switch2Pos masterArmSw("MASTER_ARM_SW", MSTR_ARM_SW);
    DcsBios::Switch2Pos masterModeAa("MASTER_MODE_AA", AA_SW);
    DcsBios::Switch2Pos masterModeAg("MASTER_MODE_AG", AG_SW);
    DcsBios::Switch2Pos emerJettBtn("EMER_JETT_BTN", E_JETT_SW);
    DcsBios::Switch2Pos fireExtBtn("FIRE_EXT_BTN", READY_SW);
} else {
    // Initialize inputs as a joystick device
    const int* inputPins[5]{ MASTR_ARM_SW, AA_SW, AG_SW, E_JETT_SW, READY_SW };


}
/**
 * @brief 
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();
  // Initialize the joystick.
  Joystick.begin();
  // @todo while (!Serial) {inputsAsHID = false}; ///< Do not allow inputs as HID to be activated if USB is not connected.
}

/**
* @brief Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}