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
 *   GitHub repository is released under the Creative Commons Atribution - 
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/

/**
 * @file 1A6-SPIN_RECOVERY_PANEL.ino
 * @author Arribe
 * @date 02.26.2024
 * @version 0.0.2
 * @brief ABSIS ALE SPIN RECOVERY, RS485 BUS ADDRESS 5
 *
 * @details This sketch is for the UIP Spin Recovery Panel.
 * 
 *  * **Reference Designator:** 1A6
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 6
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A3   | HMD Brightness
 * A2   | IR Off
 * D2   | IR Override
 * D3   | Spin Recovery, with cover
 *
 */

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL
#else
#define DCSBIOS_DEFAULT_SERIAL
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * @brief following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile
*/
//#define DCSBIOS_RS485_SLAVE 6

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5
#define UART1_SELECT

/**
 * DCS Bios library include.
 */
#include "DcsBios.h"

/**
 * @brief Define Control I/O for DCS-BIOS. 
 * 
 */
#define hmdA A3
#define irOff A2
#define irOride 2
#define spinRcvy 3

/**
 * @brief Connect switches to DCS-BIOS 
 * 
 */
DcsBios::Potentiometer hmdOffBrt("HMD_OFF_BRT", A3);
DcsBios::Switch3Pos irCoolSw("IR_COOL_SW", irOride, irOff);
DcsBios::SwitchWithCover2Pos spinRecoverySw("SPIN_RECOVERY_SW", "SPIN_RECOVERY_COVER", spinRcvy);

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