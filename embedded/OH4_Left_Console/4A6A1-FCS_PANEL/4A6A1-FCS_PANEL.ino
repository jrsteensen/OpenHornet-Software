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
 * @file 4A6A1-FCS_PANEL.ino
 * @author Arribe
 * @date 03.04.2024
 * @version u.0.0.1 (partially tested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, but the Rudder trim speed is set to full-on.  
 * The rudder trim speed is untested, the test hardware was wired directly to always run at full power.
 * All other features are tested.
 * @brief Controls the FCS panel.
 *
 * @todo Implement the RUD_TRIM_SPD usage.  I found the motor was painfully slow even when wired to run at fullspeed, so never implemented a slower speed in code.
 * 
 * @details
 * 
 *  * **Reference Designator:** 4A6A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 7
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 15  | Take-Off Switch
 * 6   | Rudder Trim Speed - not used
 * 14  | Rudder Trim Direction - Counter-Clockwise.
 * 15  | Rudder Trim Direction - Clockwise.
 * 8   | Rudder Trim Potentiometer
 * 10  | FCS Reset
 * 9   | Gain Covered Switch
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 7 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#define TO_SW1 15          ///< Take-Off Switch
#define RUD_TRIM_SPD 6     ///< Rudder Trim Speed
#define RUD_TRIM_DIR_A 14  ///< Rudder Trim Direction - Counter-Clockwise.
#define RUD_TRIM_DIR_B 7   ///< Rudder Trim Direction - Clockwise.
#define RUD_TRIM_A 8       ///< Rudder Trim Potentiometer
#define RESET_SW1 10       ///< FCS Reset
#define GAIN_SW1 9         ///< Gain Covered Switch

//Declare variables for custom non-DCS logic <update comment as needed>
unsigned int rudTrimPosition = 0;  ///< Rudder trim position used to track position of the motorized potentiomter as it spins towards center.

// Connect switches to DCS-BIOS
DcsBios::Switch2Pos fcsResetBtn("FCS_RESET_BTN", RESET_SW1);
DcsBios::SwitchWithCover2Pos gainSwitch("GAIN_SWITCH", "GAIN_SWITCH_COVER", GAIN_SW1);
DcsBios::Potentiometer rudTrimPot("RUD_TRIM", RUD_TRIM_A);
DcsBios::Switch2Pos toTrimBtn("TO_TRIM_BTN", TO_SW1);

/**
* Helper function to run the motorized potentiometer counter-clockwise.
* 
* @param duration is the time in miliseconds to run the motor.
*/
void turnCounterClockwise(int duration) {
  digitalWrite(RUD_TRIM_DIR_B, LOW);  // Set one side low to spin.
  digitalWrite(RUD_TRIM_DIR_A, HIGH);
  delay(duration);
  digitalWrite(RUD_TRIM_DIR_B, HIGH);  // Set both pins to same state to STOP
}

/**
* Helper function to run the motorized potentiometer clockwise.
* 
* @param duration is the time in miliseconds to run the motor.
*/
void turnClockwise(int duration) {
  digitalWrite(RUD_TRIM_DIR_B, HIGH);
  digitalWrite(RUD_TRIM_DIR_A, LOW);  // Set one side low to spin.
  delay(duration);
  digitalWrite(RUD_TRIM_DIR_B, LOW);  // Set both pins to same state to STOP
}

/**
*  When the TO button is pressed (physically or virtually in the sim) run the motor in the correct direction to center potentiometer.
*  
*  Based on the rudder trim position in sim.
*  -# If the rudder trim is > 32767 run the motor **counter-clockwise** for 1 miliseconds at a time until the rudder trim potentiometer is centered.
*  -# If the rudder trim is < 32767 run the motor **clockwise** for 1 miliseconds at a time until the rudder trim potentiometer is centered.
*  @note the analogRead of the Rudder Trim is between 0 and 1023.  511 is close enough to center for the sim.
* 
*  @attention After the TO button is pushed inputs on the FCS panel to the sim are blocked until the rudder trim is centered.\n  *It will take time!*
*/
void onToTrimBtnChange(unsigned int newValue) {
  if (rudTrimPosition > 32767) {
    while (analogRead(RUD_TRIM_A) > 511)
      turnCounterClockwise(1);
    return;
  }
  if (rudTrimPosition < 32767) {
    while (analogRead(RUD_TRIM_A) < 511)
      turnClockwise(1);
    return;
  }
} DcsBios::IntegerBuffer toTrimBtnBuffer(0x74b4, 0x2000, 13, onToTrimBtnChange);

void onRudTrimChange(unsigned int newValue) {
  rudTrimPosition = newValue;
} DcsBios::IntegerBuffer rudTrimBuffer(0x7528, 0xffff, 0, onRudTrimChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*
* @warning The Rudder Trim Speed is untested, the motor is slow even at full power.  Input is blocked while the rudder trim is centering.
*
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(RUD_TRIM_DIR_A, OUTPUT);
  pinMode(RUD_TRIM_DIR_B, OUTPUT);
  pinMode(RUD_TRIM_SPD, OUTPUT);

  digitalWrite(RUD_TRIM_DIR_A, LOW);
  digitalWrite(RUD_TRIM_DIR_B, LOW);
  analogWrite(RUD_TRIM_SPD, 1023);
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