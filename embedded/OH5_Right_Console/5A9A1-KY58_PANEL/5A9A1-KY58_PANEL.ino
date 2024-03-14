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
 * @file 5A9A1-KY58_PANEL.ino
 * @author Arribe
 * @date 03.10.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the KY58 panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A9A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 8
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Mode - Plaintext
 * 2   | Mode - En(C)rypted - comm 2 radio
 * A2  | Mode - Load cryptographic key (no practical purpose in the sim).
 * 3   | Mode - Recieve Variable (no practical purpose in the sim).
 * A1  | TD
 * 4   | Off
 * 15  | Zero CH 1 - 5 (not implemented in the sim).
 * 6   | CH 1
 * 14  | CH 2
 * 7   | CH 3
 * 16  | CH 4
 * 8   | CH 5
 * 10  | CH 6
 * 9   | Zero All (not implemented in the sim).

 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 8 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
 #define MODE_P A3  ///< Mode - Plaintext
 #define MODE_C 2   ///< Mode - En(C)rypted - comm 2 radio
 #define MODE_LD A2  ///< Mode - Load cryptographic key (no practical purpose in the sim).
 #define MODE_RV 3   ///< Mode - Recieve Variable (no practical purpose in the sim).
 #define TD A1  ///< Power TD
 #define OFF 4   ///< Power Off
 #define KY_VOL A0 ///< KY58 Volume
 #define CH_Z1_5 15  ///< Zero CH 1 - 5
 #define CH_1 6   ///< CH 1
 #define CH_2 14  ///< CH 2
 #define CH_3 7   ///< CH 3
 #define CH_4 16  ///< CH 4
 #define CH_5 8   ///< CH 5
 #define CH_6 10  ///< CH 6
 #define CH_ZALL 9   ///< Zero All

// Connect switches to DCS-BIOS 
/// KY-58 Fill Select Knob pins, Z 1-5/1/2/3/4/5/6/Z ALL - 5A9A1SW2 CHANNEL 
const byte ky58FillSelectPins[8] = {CH_Z1_5, CH_1, CH_2, CH_3, CH_4, CH_5, CH_6, CH_ZALL};
/// @note The sim does not allow rotating the KY58 Fill Select knob to Z all or Z 1-5, and if it did there isn't a way to reload the cryptographic keys.
DcsBios::SwitchMultiPos ky58FillSelect("KY58_FILL_SELECT", ky58FillSelectPins, 8);

///KY-58 Mode Select Knob, P/C/LD/RV - 5A9A1SW1 MODE
const byte ky58ModeSelectPins[4] = {MODE_P, MODE_C, MODE_LD, MODE_RV};
DcsBios::SwitchMultiPos ky58ModeSelect("KY58_MODE_SELECT", ky58ModeSelectPins, 4);

DcsBios::Switch3Pos ky58PowerSelect("KY58_POWER_SELECT", TD, OFF);
DcsBios::Potentiometer ky58Volume("KY58_VOLUME", KY_VOL);

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
