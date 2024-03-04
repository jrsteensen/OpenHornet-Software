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
 * @file 4A5A1-FUEL_PANEL.ino
 * @author Arribe
 * @date 03.03.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the FUEL panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A5A1
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 6
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 15  | PROBE Emergency Extend
 * 6   | PROBE Extend
 * 14  | External Tanks Wing Stop
 * 7   | External Tanks Wing Override
 * 16  | External Tanks Center Stop
 * 8   | External Tanks Center Override
 * 10  | Fuel Dump mag-switch
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 6 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#define PROB_SW1 15  ///< PROBE Emergency Extend
#define PROB_SW2 6   ///< PROBE Extend
#define WING_SW1 14  ///< External Tanks Wing Stop
#define WING_SW2 7   ///< External Tanks Wing Override
#define CTR_SW1 16   ///< External Tanks Center Stop
#define CTR_SW2 8    ///< External Tanks Center Override
#define DUMP 10      ///< Fuel Dump switch
#define DUMP_MAG 2   ///< Fuel Dump mag

//Declare variables for custom non-DCSBios logic for fuel dump mag-switch.
bool fuelDumpState = LOW;  ///< Fuel dump switch state
int bingo = 0;             ///< Bingo quantity, initialize to cold/dark state of 0.
int fuelQty = 0;           ///< Fuel quantity variable for fuel dump logic.
bool dumpHold = LOW;       ///< Fuel dump mag-switch hold
bool wowLeft = true;       ///< Initializing weight-on-wheel value for cold/ground start.
bool wowRight = true;      ///< Initializing weight-on-wheel value for cold/ground start.
bool wowNose = true;       ///< Initializing weight-on-wheel value for cold/ground start.

// Connect switches to DCS-BIOS
DcsBios::Switch2Pos fuelDumpSw("FUEL_DUMP_SW", DUMP);
DcsBios::Switch3Pos probeSw("PROBE_SW", PROB_SW1, PROB_SW2);
DcsBios::Switch3Pos extWngTankSw("EXT_WNG_TANK_SW", WING_SW1, WING_SW2);
DcsBios::Switch3Pos extCntTankSw("EXT_CNT_TANK_SW", CTR_SW1, CTR_SW2);

/**
* @brief DCSBios read back of fuel dump switch position.  If the Switch is turned off virtually in the sim, then turn off the fuel dump mag.
* @note The Fuel Dump mag-switch will not hold with weight on wheels.
*/
void onFuelDumpSwChange(unsigned int newValue) {
  if (newValue == fuelDumpState) {
    return;  // no state change, no magnet update.
  } else {
    switch (newValue) {
      case 0:  // switch turned off manually in game...disengage magnet.
        digitalWrite(DUMP_MAG, LOW);
        dumpHold = LOW;
        break;
      case 1:
        if(wowLeft == wowRight == wowNose == false){
        digitalWrite(DUMP_MAG, HIGH);  // switch turned on, either physically or virtually, engage magnet.  May get overriden by bingo logic in loop.
        dumpHold = HIGH;               // set dumpHold flag to HIGH for bingo logic.
        }
        break;
      default:
        break;
    }
    fuelDumpState = newValue;
  }
} DcsBios::IntegerBuffer fuelDumpSwBuffer(0x74b4, 0x0100, 8, onFuelDumpSwChange);

/// @brief Read the IFEI panel's BINGO setting and convert value to int for fuel dump mag-switch logic.
void onIfeiBingoChange(char* newValue) {
  bingo = atoi(newValue);
} DcsBios::StringBuffer<5> ifeiBingoBuffer(0x7468, onIfeiBingoChange);

void onExtWowLeftChange(unsigned int newValue) {
  wowLeft = newValue;
} DcsBios::IntegerBuffer extWowLeftBuffer(0x74d8, 0x0100, 8, onExtWowLeftChange);

void onExtWowNoseChange(unsigned int newValue) {
  wowNose = newValue;
} DcsBios::IntegerBuffer extWowNoseBuffer(0x74d6, 0x4000, 14, onExtWowNoseChange);

void onExtWowRightChange(unsigned int newValue) {
  wowRight = newValue;
} DcsBios::IntegerBuffer extWowRightBuffer(0x74d6, 0x8000, 15, onExtWowRightChange);

/**
* @brief Read the IFEI panel's Fuel state value when it's going down, and convert to int for fueldump logic.
* @note Fuel dump logic doesn't care if the fuel quantity is going up.
*/
void onIfeiFuelDownChange(char* newValue) {
  fuelQty = atoi(newValue);
} DcsBios::StringBuffer<6> ifeiFuelDownBuffer(0x748a, onIfeiFuelDownChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
  void setup() {

    // Run DCS Bios setup function
    DcsBios::setup();

    pinMode(DUMP_MAG, OUTPUT);

    digitalWrite(DUMP_MAG, LOW);
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

/**
*   ### Fuel Dump mag-switch cancel logic:
*   If BINGO setting value is greater than current fuel quantity, or if fuel level below critical cutoff floor of 1,950 lbs, then:
    -# If the fuel dump mag-switch is already off nothing happens. \n
*   -# Turn off the fuel dump mag-switch. \n
*   @note If the fuel quantity is lower than BINGO or lower than the 1,950 lbs safety floor the fuel dump mag-switch will not hold.
* 
*/  
    if (bingo >= fuelQty || fuelQty <= 1950) {  //if bingo greater than fuel quantity, or if fuel quantity under 1,950 lbs then deactivate the magnet.
      switch (dumpHold) {
        case LOW:
          break;  // mag-switch already off no action.
        case HIGH:
          digitalWrite(DUMP_MAG, LOW);  //magnet is high, turn off as fuel state reached bingo or is below 1,950lb safety margin.
          dumpHold = LOW;               //set magnet flag to LOW to match that it was just turned off.
          break;
        default:
          break;
      }
    }
  }
