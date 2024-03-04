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
 * @file 1A7-HUD_PANEL.ino
 * @author Arribe
 * @date 02.29.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the HUD panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 1A7
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 3
 * 
 * *##Wiring diagram:
 * PIN | Function
 * --- | ---
 * A0   | AOA Indexer
 * A1   | HUD Balance
 * A2   | HUD Black Level
 * A3   | HUD Brightness
 * 6    | HUD Symbology REJ2
 * 7    | Video Control W/B
 * 8    | ALT BARO 
*  9    | ATT STBY
*  10   | ATT INS
*  14   | Day Switch
*  15   | HUD Symbology NORM
*  16   | Video Control OFF
*
* @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
* It also sets the address of this slave device. The slave address should be
* between 1 and 126 and must be unique among all devices on the same bus.
*
* @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
*
*  #define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
 #define AOA_A A0 ///< AOA Indexer
 #define BAL_A A1 ///< HUD Balance
 #define BLK_A A2 ///< HUD Black Level
 #define BRT_A A3 ///< HUD Brightness
 #define REJ_2 6 ///< HUD Symbology REJ2
 #define WB_WB 7 ///< Video Control W/B
 #define ALT_BARO 8 ///< ALT BARO
 #define ATT_STBY 9 ///< ATT STBY
 #define ATT_INS 10 ///< ATT INS
 #define DAY_SW 14 ///< Day Switch
 #define REJ_NORM 15 ///< HUD Symbology NORM
 #define WB_OFF 16 ///< Video Control OFF

// Connect switches to DCS-BIOS 
DcsBios::Switch2Pos hudAltSw("HUD_ALT_SW", ALT_BARO);
DcsBios::Potentiometer hudAoaIndexer("HUD_AOA_INDEXER", AOA_A);
DcsBios::Switch3Pos hudAttSw("HUD_ATT_SW", ATT_STBY, ATT_INS);
DcsBios::Potentiometer hudBalance("HUD_BALANCE", BAL_A);
DcsBios::Potentiometer hudBlackLvl("HUD_BLACK_LVL", BLK_A);
DcsBios::Potentiometer hudSymBrt("HUD_SYM_BRT", BRT_A);
DcsBios::Switch2Pos hudSymBrtSelect("HUD_SYM_BRT_SELECT", DAY_SW);
DcsBios::Switch3Pos hudSymRejSw("HUD_SYM_REJ_SW", REJ_2, REJ_NORM);
DcsBios::Switch3Pos hudVideoControlSw("HUD_VIDEO_CONTROL_SW", WB_OFF, WB_WB);

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