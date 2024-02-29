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
 * @file 4A2A1-LDG_GEAR_PANEL.ino
 * @author Arribe
 * @date 2/28/2024
 * @version 0.0.2
 * @brief Controls the LDG GEAR panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A2A1
 *  * **Intended Board:** ABSIS ALE /w Relay Module
 *  * **RS485 Bus Address:** 1
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A1  | Landing Gear Emergency Rotate and Pull
 * A2  | Landing Gear Down Lock Override Button
 * A3  | Landing Gear Warning Silence Button
 * 2   | Landing Gear Down Lock Soleniod
 * 3   | Landing Gear Limit Switch (handle raise / lower)
 * 4   | Landing Gear Lollipop LED
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
//#define DCSBIOS_RS485_SLAVE 1

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5
#define UART1_SELECT

/**
 * DCS Bios library include
 */
#include "DcsBios.h"

/**
 * @brief Define Control I/O for DCS-BIOS. 
 * 
 */
const int lgEmergSw = A1;
const int lgOrideSw = A2;
const int lgWarnSw =  A3;
const int lgLeverSol = 2;
const int lgLimitSw = 3;
const int lgLed = 4;

/**
* @brief Declare variables for down lock logic
* initializing values for weight on wheels under a cold / onground start and down lock off.
*
*/
int wowLeft = 1;
int wowRight = 1;
int wowNose = 1;
int downLockOverride = 0;

/**
 * @brief Connect switches to DCS-BIOS 
 * 
 */
DcsBios::Switch2Pos emergencyGearRotate("EMERGENCY_GEAR_ROTATE", lgEmergSw);
DcsBios::Switch2Pos gearDownlockOverrideBtn("GEAR_DOWNLOCK_OVERRIDE_BTN", lgOrideSw);
DcsBios::Switch2Pos gearLever("GEAR_LEVER", lgLimitSw);
DcsBios::Switch2Pos gearSilenceBtn("GEAR_SILENCE_BTN", lgWarnSw);
DcsBios::LED landingGearHandleLt(0x747e, 0x0800, lgLed);

/**
* @brief DCSBios reads to save airplane state information.
*
*/
void onExtWowLeftChange(unsigned int newValue) {
  wowLeft = newValue;
}
DcsBios::IntegerBuffer extWowLeftBuffer(0x74d8, 0x0100, 8, onExtWowLeftChange);

void onExtWowNoseChange(unsigned int newValue) {
  wowNose = newValue;
}
DcsBios::IntegerBuffer extWowNoseBuffer(0x74d6, 0x4000, 14, onExtWowNoseChange);

void onExtWowRightChange(unsigned int newValue) {
  wowRight = newValue;
}
DcsBios::IntegerBuffer extWowRightBuffer(0x74d6, 0x8000, 15, onExtWowRightChange);

void onGearDownlockOverrideBtnChange(unsigned int newValue) {
  downLockOverride = newValue;
}
DcsBios::IntegerBuffer gearDownlockOverrideBtnBuffer(0x747e, 0x4000, 14, onGearDownlockOverrideBtnChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(lgLeverSol, OUTPUT);
  digitalWrite(lgLeverSol, LOW);  //initialize solenoid to off
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
* If landing gear handle in down position and lock override pushed, then activate soleniod to unlock handle.
* If landing gear handle in down position and NO weight on wheels, then activate soleniod to unlock handle.
* IF landing gear handle is down and there is weight on at least one wheel, then turn off soleniod to lock handle down.
* If landing gear handle is up turn off soleniod, handle cannot be locked in up position.
* 
* Note: digital reads of switch state will allow the landing gear handle to operate using the downlock override button
* without needing to have the sim running.
*/
  if (digitalRead(lgLimitSw) == 1) {  //Switch closed, gear handle is down
    if (downLockOverride == 1 || !digitalRead(lgOrideSw) == 1) {  // Override switched pushed virually in sim or physcially in pit, turn on soleniod to unlock gear handle.
      digitalWrite(lgLeverSol, HIGH);
    } else if (wowLeft == wowRight == wowNose == 0) {  //No weight on any wheel, turn on soleniod to unlock the gear handle
      digitalWrite(lgLeverSol, HIGH);
    } else {  // gear handle is down and there is weight on at least one wheel, turn off soleniod to lock the handle down.
      digitalWrite(lgLeverSol, LOW);
    }
  } else {  //gear handle up, turn off soleniod
    digitalWrite(lgLeverSol, LOW);
  }
}
