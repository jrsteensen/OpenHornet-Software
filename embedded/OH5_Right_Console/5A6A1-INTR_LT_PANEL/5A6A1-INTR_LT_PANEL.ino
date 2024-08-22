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
 * @file 5A6A1-INTR_LT_PANEL.ino
 * @author Arribe
 * @date 03.11.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the INTR LT panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A6A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 5
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Light Test
 * 2   | NVG Mode
 * 3   | Day Mode
 * A2  | Warning / Caution Brightness
 * A1  | Chart Brightness
 * A7   | Console Brightness
 * 8   | Intrument Panel Brightness
 * A10  | Flood Brightness
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 5 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#define TEST A3      ///< Light Test
#define NVG 2        ///< NVG Mode
#define DAY 3        ///< Day Mode
#define WAR_CAUT A2  ///< Warning / Caution Brightness
#define CHART A1     ///< Chart Brightness
#define CONSOLES A7   ///< Console Brightness
#define INST_PNL 8   ///< Intrument Panel Brightness
#define FLOOD A10     ///< Flood Brightness

// Connect switches to DCS-BIOS
DcsBios::Potentiometer chartDimmer("CHART_DIMMER", CHART);
DcsBios::Switch3Pos cockkpitLightModeSw("COCKKPIT_LIGHT_MODE_SW", NVG, DAY);
DcsBios::Potentiometer consolesDimmer("CONSOLES_DIMMER", CONSOLES);
DcsBios::Potentiometer floodDimmer("FLOOD_DIMMER", FLOOD);
DcsBios::Potentiometer instPnlDimmer("INST_PNL_DIMMER", INST_PNL);
DcsBios::Switch2Pos lightsTestSw("LIGHTS_TEST_SW", TEST);
DcsBios::Potentiometer warnCautionDimmer("WARN_CAUTION_DIMMER", WAR_CAUT);

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
