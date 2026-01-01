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
 * @file 5A10-DEFOG_PANEL.ino
 * @author Arribe
 * @date 03.06.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 **
 * @brief Controls the DEFOG panel, R CRKT BRKRs & CANOPY module.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A10
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 9
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 15  | Defog - Anti-ice
 * A7   | Defog Lever Potentiometer
 * 14  | Defog - Rain 
 * 7   | Canopy Open
 * 16  | Canopy Close
 * 8   | Canopy Aux 1 Buton
 * 10  | Canopy Aux 2 Buton
 * A3  | Right Circuit Breaker Landing Ger
 * 2   | Right Circuit Breaker Hook
 * A2  | Right Circuit Breaker FCS4
 * A1  | Right Circuit BreaKer FCS3
 * 4   | Right Circuit Breaker Panel FCSbit On
 * 2   | Canopy Open Mag-switch
 * 
 * @attention This sketch includes a Joystick for the 2 bonus auxiliary buttons on the under-side of the canopy switch housing, and as an x-axis for the defog lever handle.
 * In DCS the buttons must be mapped to the functions of your choice, and the defog lever must be mapped as an axis control.
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
 *
 * #define DCSBIOS_RS485_SLAVE 9 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#include "Joystick.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define DF_ANTIICE 15  ///< Defog - Anti-ice
#define DF_A A7         ///< 6 Defog Lever Potentiometer
#define DF_RAIN 14     ///< Defog - Rain
#define CN_OPEN 7      ///< Canopy Open
#define CN_CLOSE 16    ///< Canopy Close
#define CN_AUX1 8      ///< Canopy Aux 1 Buton
#define CN_AUX2 10     ///< Canopy Aux 2 Buton
#define RC_LG A3       ///< Right Circuit Breaker Landing Ger
#define RC_HOOK A2      ///< Right Circuit Breaket Hook
#define RC_FCS4 3     ///< Right Circuit Breaker FCS4
#define RC_FCS3 A1     ///< Right Circuit BreaKer FCS3
#define RC_FCSBIT 4    ///< Right Circuit Breaker Panel FCSbit On
#define CN_OPEN_MAG 2  ///< Canaopy Open Hold mag-switch

//Declare variables for custom non-DCS logic <update comment as needed>
bool wowLeft = true;          ///< Initializing weight-on-wheel value for cold/ground start.
bool wowRight = true;         ///< Initializing weight-on-wheel value for cold/ground start.
bool wowNose = true;          ///< Initializing weight-on-wheel value for cold/ground start.
bool canopyOpenState = true;  ///< Initializing canopy state as open for cold/ground start.
bool canopyMagHold = false;   ///< Initializing canopy mag hold to off.

/**
* Joystick Setup for defog lever on x-axis, and the 2 canopy aux buttons.
*/
Joystick_ Joystick = Joystick_(
  0x4910,
  JOYSTICK_TYPE_JOYSTICK,
  2,
  0,
  true,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false);


// Connect switches to DCS-BIOS
DcsBios::Switch3Pos wshieldAntiIceSw("WSHIELD_ANTI_ICE_SW", DF_ANTIICE, DF_RAIN);
DcsBios::Switch3Pos canopySw("CANOPY_SW", CN_OPEN, CN_CLOSE);
DcsBios::Switch2Pos cbFcsChan3("CB_FCS_CHAN3", RC_FCS3);
DcsBios::Switch2Pos cbFcsChan4("CB_FCS_CHAN4", RC_FCS4);
DcsBios::Switch2Pos cbHoook("CB_HOOOK", RC_HOOK);
DcsBios::Switch2Pos cbLg("CB_LG", RC_LG);
DcsBios::Switch2Pos fcsBitSw("FCS_BIT_SW", RC_FCSBIT);

// DCSBios reads to save airplane state information.

/**
*  If the Canopy Switch is moved to open with weight-on-wheels the mag-switch will hold. 
*  The program loop will release the mag-switch when the canopy is fully opened.
*/
void onCanopySwChange(unsigned int newValue) {
  if (newValue == 2) {
    if (wowRight == wowLeft == wowNose == true) {
      digitalWrite(CN_OPEN_MAG, HIGH);
      canopyMagHold = true;
    }
  } else {
    digitalWrite(CN_OPEN_MAG, LOW);
    canopyMagHold = false;
  }
} DcsBios::IntegerBuffer canopySwBuffer(0x74ce, 0x0300, 8, onCanopySwChange);

/**
* Determine the canopy open state by reading if the value is over 58,900 (determined by testing in DCS).
*/
void onCanopyPosChange(unsigned int newValue) {
  if (newValue >= 58900) {
    canopyOpenState = true;
  } else {
    canopyOpenState = false;
  }
} DcsBios::IntegerBuffer canopyPosBuffer(0x7552, 0xffff, 0, onCanopyPosChange);

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
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  Joystick.begin();
  Joystick.setXAxisRange(0, 1024);

  pinMode(CN_AUX1, INPUT_PULLUP);
  pinMode(CN_AUX2, INPUT_PULLUP);
  pinMode(DF_A, INPUT);

  pinMode(CN_OPEN_MAG, OUTPUT);
  digitalWrite(CN_OPEN_MAG, LOW);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* @note Defog lever didn't work well using DCSBios' potentiometer method.  The min and max range was too far off, could potentially work if values maped to a different range, 
* but the potentimeter's roation is much larger than the fog handle's range.  Found it easier within DCS to control it as a Joystick axis.
* Then one could calibrate it in the Window's Game Controllers utility, and map it as an axis in DCS.\n\n
* This is the command to use if one wants to try the pontentiometer approach - DcsBios::Potentiometer defogHandle("DEFOG_HANDLE", DF_A);
* 
* @details If the canopy mag-switch is held in the open positon, then when the canopy reaches the fully open position the mag-switch is released.
*
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  Joystick.setButton(0, !digitalRead(CN_AUX1)); // Set the aux 1 joystick button state.
  Joystick.setButton(1, !digitalRead(CN_AUX2)); // Set the aux 2 joystick button state.
  Joystick.setXAxis(analogRead(DF_A));  // Set the defog lever position.

  if(canopyMagHold && canopyOpenState){ // Release mag-switch when the canopy is open and the mag is on.
    digitalWrite(CN_OPEN_MAG, LOW);  // Release the mag-switch.
    canopyMagHold = false; // Set canopy mode to false.
  }
}

