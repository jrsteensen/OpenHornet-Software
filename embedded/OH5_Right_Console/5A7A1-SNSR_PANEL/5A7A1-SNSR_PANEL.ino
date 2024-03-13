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
 * @file 5A7A1-SNSR_PANEL.ino
 * @author Arribe
 * @date 03.10.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the SNSR panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A7A1
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 6
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | FLIR On
 * A2  | FLIR Off
 * 3   | LTDR Arm
 * A1  | Radar Standby
 * 4   | Radar Operate
 * A0  | Radar Emergency
 * 15  | LST On
 * 6   | INS Carrier
 * 14  | INS Ground
 * 7   | INS Nav
 * 16  | INS In-Flight Alignment
 * 8   | INS Gyro
 * 10  | INS GB
 * 9   | INS Test
 * 2   | LTDR Arm Mag-switch
 *
 * @attention Due to limitations with DCSBios it's not possible to release the LTD/R mag-switch on weapon deployment like it does in the sim.
 * The switch will stay engaged in the ARM'ed position until one of the other release conditions are met.  In addition, in the sim the LTD/R mag-switch will not hold when
 * the landing gear has been oversped and the gear-bay doors damaged.  This is also not modeled in the logic, the LTD/R will hold.  Though in this scenario the sim won't
 * allow any A/G weapon deployments.
 *
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
 *
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
#include "5A7A1-SNSR_PANEL.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define FLIR_ON A3      ///< FLIR On
#define FLIR_OFF A2     ///< FLIR Off
#define LTDR_ARM 3      ///< LTDR Arm
#define RDR_STBY A1     ///< Radar Standby
#define RDR_OPR 4       ///< Radar Operate
#define RDR_EMERG A0    ///< Radar Emergency
#define LST_ON 15       ///< LST On
#define INS_CV 6        ///< INS Carrier
#define INS_GND 14      ///< INS Ground
#define INS_NAV 7       ///< INS Nav
#define INS_IFA 16      ///< INS In-Flight Alignment
#define INS_GYRO 8      ///< INS Gyro
#define INS_GB 10       ///< INS GB
#define INS_TEST 9      ///< INS Test
#define LTDR_ARM_MAG 2  ///< LTDR Arm Mag-switch

//Declare variables for custom non-DCS logic <update comment as needed>
bool leftGearDown = true;        ///< Initialize left landing gear state for the LTD/R switch hold logic.
bool rightGearDown = true;       ///< Initialize right landing gear state for the LTD/R switch hold logic.
bool noseGearDown = true;        ///< Initialize nose landing gear state for the LTD/R switch hold logic.
bool airToGroundLight = false;   ///< Initialize Air-to-Ground light for the LTD/R switch hold logic.
bool ltdrArmMagEngaged = false;  ///< Initialize LTD/R mag-swtich for hold logic.

const byte insSwPins[8] = { DcsBios::PIN_NC, INS_CV, INS_GND, INS_NAV, INS_IFA, INS_GYRO, INS_GB, INS_TEST };  ///< Off position doesn't have a pin.
const byte radarSwPins[4] = { DcsBios::PIN_NC, RDR_STBY, RDR_OPR, RDR_EMERG };                                 ///< Off position doesn't have a pin.

// Connect switches to DCS-BIOS
DcsBios::Switch3Pos flirSw("FLIR_SW", FLIR_ON, FLIR_OFF);
DcsBios::Switch2Pos lstNflrSw("LST_NFLR_SW", LST_ON, true);
DcsBios::Switch2Pos ltdRSw("LTD_R_SW", LTDR_ARM);

///@todo If/When https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/pull/56 is accepted by DCS Skunkworks change to DcsBios::SwitchMultiPos class.
SwitchMultiPosDebounce insSw("INS_SW", insSwPins, 8, false, 100);
SwitchRadar radarSw("RADAR_SW", "RADAR_SW_PULL", 3, radarSwPins, 4, false, 100);

// DCSBios reads to save airplane state information.
void onFlpLgLeftGearLtChange(unsigned int newValue) {
  leftGearDown = newValue;
}
DcsBios::IntegerBuffer flpLgLeftGearLtBuffer(0x7430, 0x1000, 12, onFlpLgLeftGearLtChange);

void onFlpLgRightGearLtChange(unsigned int newValue) {
  rightGearDown = newValue;
}
DcsBios::IntegerBuffer flpLgRightGearLtBuffer(0x7430, 0x2000, 13, onFlpLgRightGearLtChange);

void onFlpLgNoseGearLtChange(unsigned int newValue) {
  noseGearDown = newValue;
}
DcsBios::IntegerBuffer flpLgNoseGearLtBuffer(0x7430, 0x0800, 11, onFlpLgNoseGearLtChange);

/// A/G Master Mode light used to determine if the LTD/R mag-switch should be cancelled.  If the ight goes off the mag-swtich releases.
void onMasterModeAgLtChange(unsigned int newValue) {
  airToGroundLight = newValue;
  if (airToGroundLight == 0 && ltdrArmMagEngaged == true) {  // light off and the LTD/R mag-switch is on.
    digitalWrite(LTDR_ARM_MAG, LOW);                         // turn off the mag-switch.
    ltdrArmMagEngaged = false;                               // remember that the switch is released.
  }
}
DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

/// If the landing gear lever is lowered while the LTD/R mag-switch is held in the 'ARM' position the swtich is released.
void onGearLeverChange(unsigned int newValue) {
  if (newValue == 0 && ltdrArmMagEngaged == true) {  // landing gear lever lowered and mag-switch is on.
    digitalWrite(LTDR_ARM_MAG, LOW);                 // Landing gear handle lowered, turn off LTD/R Mag.
    ltdrArmMagEngaged = false;                       // remember that the switch is released.
  }
}
DcsBios::IntegerBuffer gearLeverBuffer(0x747e, 0x1000, 12, onGearLeverChange);

/// If the LTD/R switch is turned off virtually or physically update the mag-switch state.  The mag-switch will only hold if the landing gear is up, and the A/G master mode is on.
void onLtdRSwChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      digitalWrite(LTDR_ARM_MAG, LOW);  // switch turned off physically or virtually in the sim, turn off mag.
      ltdrArmMagEngaged = false;        // remember that the switch is released.
      break;
    case 1:
      if ((leftGearDown == rightGearDown == noseGearDown == false) && (airToGroundLight == true)) {  // if the gear is up and A/G light On the mag-switch will hold.
        digitalWrite(LTDR_ARM_MAG, HIGH);                                                            // hold the mag-swtich in the 'ARM' position.
        ltdrArmMagEngaged = true;                                                                    // remember that the mag-switch is holding.
        break;
      }
  }
}
DcsBios::IntegerBuffer ltdRSwBuffer(0x74c8, 0x4000, 14, onLtdRSwChange);

/**
* If the FLIR is turned off the LTD/R switch should be released.  Technically it shouldn't hold until after the FLIR is no longer timed-out.  But no way to get that state from DCS.
*
* @bug The code to check the FLIR switch state should work, but for some unkown reason it's not releasing the mag-switch like the landing gear or the A/G master mode does.  Potential DCSBios bug.
*/
void onFlirSwChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      if (ltdrArmMagEngaged == true) {
        digitalWrite(LTDR_ARM_MAG, LOW);  // flir switch turned off, turn off LTD/R Mag.
        ltdrArmMagEngaged = false;        // remember that the switch is released.
      }
      break;
    case 1:
      break;
    case 2:
      break;
  }
}
DcsBios::IntegerBuffer flirSwBuffer(0x74c8, 0x3000, 12, onFlirSwChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  flirSw.resetThisState();

  pinMode(LTDR_ARM_MAG, OUTPUT);
  digitalWrite(LTDR_ARM_MAG, LOW);
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

  radarSw.pollThisInput();

  ///@todo If/When https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/pull/56 is accepted by DCS Skunkworks remove the insSw.pollThisInput(); call.
  insSw.pollThisInput();
}