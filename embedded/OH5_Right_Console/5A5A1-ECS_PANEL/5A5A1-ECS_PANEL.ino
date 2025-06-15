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
 * @file 5A5A1-ECS_PANEL.ino
 * @author Arribe
 * @date 03.22.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the ECS panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A5A1
 *  * **Intended Board:** ABSIS ALE w&#x2F; Relay Module
 *  * **RS485 Bus Address:** 4
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | ECS Mode Auto
 * 2   | PITOT mag-switch
 * A2  | ECS Mode Off
 * 3   | Bleed Air Solenoid
 * A1  | Cabin Pressure NORM
 * 4   | Cabin Pressure RAM
 * A0  | PITOT On
 * 15  | Engine Anti-Ice On
 * 6   | Engine Anti-Ice Test
 * 14  | Bleed Air AUG - Pull
 * 7   | Bleed Air Right Off
 * 16  | Bleed Air Off
 * 8   | Bleed Air Left Off
 * 10  | Suit Temperature
 * 9   | Cabin Temperature
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 4 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#include "5A5A1-ECS_PANEL.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define M_AUTO A3      ///< ECS Mode Auto
#define PITOT_MAG 2    ///< PITOT mag-switch
#define M_OFF A2       ///< ECS Mode Off
#define BA_SOLENOID 3  ///< Bleed Air Solenoid
#define CP_NORM A1     ///< Cabin Pressure NORM
#define CP_RAM 4       ///< Cabin Pressure RAM
#define PITOT_ON A0    ///< PITOT On
#define ENG_ON 15      ///< Engine Anti-Ice On
#define ENG_TEST 6     ///< Engine Anti-Ice Test
#define BA_AUG 14      ///< Bleed Air AUG - Pull
#define BA_ROFF 7      ///< Bleed Air Right Off
#define BA_OFF 16      ///< Bleed Air Off
#define BA_LOFF 8      ///< Bleed Air Left Off
#define TS_A 10        ///< Suit Temperature
#define TC_A 9         ///< Cabin Temperature

//Declare variables for custom non-DCS logic <update comment as needed>
bool wowLeft = true;    ///< Initializing weight-on-wheel value for cold/ground start.
bool wowRight = true;   ///< Initializing weight-on-wheel value for cold/ground start.
bool wowNose = true;    ///< Initializing weight-on-wheel value for cold/ground start.
unsigned int rpmL = 0;  ///< Initializing engine RPM for cold start
unsigned int rpmR = 0;  ///< Initializing engine RPM for cold start

bool apuSwitchOn = 0;       ///< Variable for Bleed Air logic.
bool bleedAirAugState = 0;  ///< Variable for Bleed Air logic.
bool leftGenState = 0;      ///< Variable for PITOT magswitch logic.
bool rightGenState = 0;     ///< Variable for PITOT magswitch logic.
bool pitotHold = false;     ///< Variable for PITOT magswitch logic.

// Connect switches to DCS-BIOS
const byte bleedAirKnobPins[4] = { BA_LOFF, BA_OFF, BA_ROFF, DcsBios::PIN_NC };
/** @todo Remove SwitchMultiPosDebounce if/When DCS-Skunkworks fixes the DcsBios::SwitchMultiPos to work with PIN_NC.
*   @note You may want to play with the debounce delay to see if a longer period of time will work better for you or not.
*   In tesing a longer delay could potentially reduce the bouncing back to the NORM position, but also made the knob feel less responsive or stuck in sim.
*   @bug The Bleed Air knob rotation bounces to NORM when rotated, potentially needs a hardware resolution to connect the NORM position to an Arduino pin.
*/
SwitchMultiPosDebounce bleedAirKnob("BLEED_AIR_KNOB", bleedAirKnobPins, 4, false, 100);

DcsBios::Switch2Pos bleedAirPull("BLEED_AIR_PULL", BA_AUG);
DcsBios::Switch3Pos cabinPressSw("CABIN_PRESS_SW", CP_RAM, CP_NORM);
DcsBios::Switch3Pos ecsModeSw("ECS_MODE_SW", M_AUTO, M_OFF);
DcsBios::Switch3Pos engAntiiceSw("ENG_ANTIICE_SW", ENG_ON, ENG_TEST);
DcsBios::Switch2Pos pitotHeatSw("PITOT_HEAT_SW", PITOT_ON);
DcsBios::Potentiometer suitTemp("SUIT_TEMP", TS_A);
DcsBios::Potentiometer cabinTemp("CABIN_TEMP", TC_A);

//Engine RPM needed for Aug bleed air switch.  The solenoid needs to pull down the AUG bleed air with both engines are > MIL
void onIfeiRpmLChange(char* newValue) {
  rpmL = atoi(newValue);
}
DcsBios::StringBuffer<3> ifeiRpmLBuffer(0x749e, onIfeiRpmLChange);

void onIfeiRpmRChange(char* newValue) {
  rpmR = atoi(newValue);
}
DcsBios::StringBuffer<3> ifeiRpmRBuffer(0x74a2, onIfeiRpmRChange);

void onApuControlSwChange(unsigned int newValue) {
  apuSwitchOn = newValue;
  if (apuSwitchOn == 0 && bleedAirAugState == 1) { // If APU switch turned off and Bleed Air Augmentation up, then pull solenoid.
    bleedAirAugState = 0;
    digitalWrite(BA_SOLENOID, HIGH);  // pull down.
    delay(200);  //wait a moment
    digitalWrite(BA_SOLENOID, LOW); // turn off solenoid.
  }
}
DcsBios::IntegerBuffer apuControlSwBuffer(0x74c2, 0x0100, 8, onApuControlSwChange);

void onBleedAirPullChange(unsigned int newValue) {
  if (bleedAirAugState != newValue) {
    bleedAirAugState = newValue;
    switch (newValue) {
      case 0:  // If BA pushed virtually within sim, pull down in pit
        digitalWrite(BA_SOLENOID, HIGH); // pull down.
        delay(200);  //wait a moment
        digitalWrite(BA_SOLENOID, LOW); // turn off solenoid.
        break;
      case 1:
        /// @note It's physically possible to overcome the solenoid by manually holding the bleed air knob up.
        if (apuSwitchOn == 0 || (wowLeft == wowRight == wowNose == 0)) {  //then pull down, ELSE do nothing.
          digitalWrite(BA_SOLENOID, HIGH); // pull down.
          delay(200);  //wait a moment
          digitalWrite(BA_SOLENOID, LOW); // turn off solenoid.
        }
        break;
    }
  }
}
DcsBios::IntegerBuffer bleedAirPullBuffer(0x74c4, 0x8000, 15, onBleedAirPullChange);

void onLGenSwChange(unsigned int newValue) {
  leftGenState = newValue;
}
DcsBios::IntegerBuffer lGenSwBuffer(0x74c4, 0x2000, 13, onLGenSwChange);

void onRGenSwChange(unsigned int newValue) {
  rightGenState = newValue;
}
DcsBios::IntegerBuffer rGenSwBuffer(0x74c4, 0x4000, 14, onRGenSwChange);

void onPitotHeatSwChange(unsigned int newValue) {
  if (newValue == 0) {  // switch turned off physically or virtually in sim, turn off mag.
    digitalWrite(PITOT_MAG, LOW);
    pitotHold = false;
  } else {
    if (leftGenState == 1 || rightGenState == 1)  // at least one generator needs to be running to hold PITOT mag-switch in on position.
      digitalWrite(PITOT_MAG, HIGH);
    pitotHold = true;
  }
}
DcsBios::IntegerBuffer pitotHeatSwBuffer(0x74c8, 0x0100, 8, onPitotHeatSwChange);



/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(BA_SOLENOID, OUTPUT);
  pinMode(PITOT_MAG, OUTPUT);

  digitalWrite(BA_SOLENOID, LOW);
  digitalWrite(PITOT_MAG, LOW);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* Bleed air APU augmentation logic to hold in up position:
*  1. Weight-on-wheels is true.
*  2. Engine rpm < 95% ... pull if throttles advanced to to MIL.
*  3. APU switch on...onApuControlSwChange will pull if switched off.
*  4. at least 1 gen running...if both off pull.
*
*/
void loop() {
  DcsBios::loop();

  ///@todo Remove bleedAirKnob.pollThisInput() if/When DCS-Skunkworks fixes the DcsBios::SwitchMultiPos to work with PIN_NC.
  bleedAirKnob.pollThisInput();

  if (bleedAirAugState == 1) {                                                      // If bleed air aug is on...
    if ((rpmL >= 95 && rpmR >= 95) || (leftGenState == 0 && rightGenState == 0)) {  // If over 95% rpm or both generators off pull down on bleed air aug
      digitalWrite(BA_SOLENOID, HIGH); // pull down.
      delay(200);  //wait a moment
      digitalWrite(BA_SOLENOID, LOW); // turn off solenoid.
      bleedAirAugState = 0;
    }
  }
  if (pitotHold == true) { // If PITOT switch is on...
    if (leftGenState == 0 && rightGenState == 0) { // IF both generators are off release the mag-switch.
      digitalWrite(PITOT_MAG, LOW);
      pitotHold = false;
    }
  }
}
