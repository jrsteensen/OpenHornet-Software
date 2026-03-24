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
 * @file 4A5A2-APU_PANEL.ino
 * @author Arribe, Ash
 * @date 03.24.2026
 * @version 0.1.0
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the APU panel & L CIRCUIT BREAKERS.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A5A2
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 5
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 15  | APU Switch
 * 6   | APU Lamp
 * 14  | Engine Crank Right
 * 7   | Engine Crank Left
 * 16  | Circuit Breaker FCS Channel 1
 * 8   | Circuit Breaker FCS Channel 2
 * 10  | Circuit Breaker Speed Break
 * 9   | Circuit Breaker Launch Bar
 * 2   | APU Switch Magnet
 * 3   | Engine Crank Magnet
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
#define APU_SW1 15       ///< APU Mag Switch
#define APU_LAMP 6       ///< APU Lamp
#define CRANK_SW1 14     ///< Engine Crank Right
#define CRANK_SW2 7      ///< Engine Crank Left
#define FCS_CH1 16       ///< Circuit Breaker FCS Channel 1
#define FCS_CH2 8        ///< Circuit Breaker FCS Channel 2
#define LCSPDBRK 10      ///< Circuit Breaker Speed Break
#define LCLBAR 9         ///< Circuit Breaker Launch Bar
#define APU_SW_MAG 2     ///< APU Switch Magnet
#define ENG_CRANK_MAG 3  ///< Engine Crank Magnet


//Declare variables for custom non-DCS logic <update comment as needed>
bool apuState = LOW;      ///< Initializing for cold/ground start switch releases on power off.
byte engCrankState = 1;   ///< Three position switch, 1 is middle

// Connect switches to DCS-BIOS
DcsBios::Switch2Pos apuControlSw("APU_CONTROL_SW", APU_SW1);
DcsBios::Switch3Pos engineCrankSw("ENGINE_CRANK_SW", CRANK_SW2, CRANK_SW1);
DcsBios::Switch2Pos cbFcsChan1("CB_FCS_CHAN1", FCS_CH1);
DcsBios::Switch2Pos cbFcsChan2("CB_FCS_CHAN2", FCS_CH2);
DcsBios::Switch2Pos cbLaunchBar("CB_LAUNCH_BAR", LCLBAR);
DcsBios::Switch2Pos cbSpdBrk("CB_SPD_BRK", LCSPDBRK);

// DCSBios reads to save airplane state information.

/**
* @brief APU Ready Light LED
*
*/
DcsBios::LED apuReadyLt(FA_18C_hornet_APU_READY_LT_AM, APU_LAMP);

/**
* @brief DCSBios read back of APU switch position.  If the Switch is turned off virtually in the sim, then turn off the APU mag.
*
*/
void onApuControlSwChange(unsigned int newValue) {
  if (newValue == apuState) {
    return;  // No state change, no magnet update.
  } else {
    switch (newValue) {
      case 0:  // Switch turned off in SIM.
        digitalWrite(APU_SW_MAG, LOW);
        break;
      case 1:  // Switch is on in SIM.
        digitalWrite(APU_SW_MAG, HIGH);
        break;
      default:
        break;
    }
  }
  apuState = newValue;
} DcsBios::IntegerBuffer apuControlSwBuffer(FA_18C_hornet_APU_CONTROL_SW, onApuControlSwChange);

/**
* @brief DCSBios read back of Engine Crank switch position.  If the Switch is turned off virtually in the sim, 
* then turn off the Engine Crank mag.
* 
*/
void onEngineCrankSwChange(unsigned int newValue) {
  if (newValue == engCrankState) {
    return;  // no state change, no magnet update.
  } else {
    switch (newValue) {
      case 0:
        digitalWrite(ENG_CRANK_MAG, HIGH);  // switch turned on, either physically or virtually, engage magnet.
        break;
      case 1:  // switch turned off disengage magnet.
        digitalWrite(ENG_CRANK_MAG, LOW);
        break;
      case 2:
        digitalWrite(ENG_CRANK_MAG, HIGH);  // switch turned on, either physically or virtually, engage magnet.
        break;
      default:
        break;
    }
    engCrankState = newValue;
  }
} DcsBios::IntegerBuffer engineCrankSwBuffer(FA_18C_hornet_ENGINE_CRANK_SW, onEngineCrankSwChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(APU_SW_MAG, OUTPUT);
  pinMode(ENG_CRANK_MAG, OUTPUT);
  pinMode(APU_LAMP, OUTPUT);

  digitalWrite(APU_SW_MAG, LOW);
  digitalWrite(ENG_CRANK_MAG, LOW);
  digitalWrite(APU_LAMP, LOW);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();
}
