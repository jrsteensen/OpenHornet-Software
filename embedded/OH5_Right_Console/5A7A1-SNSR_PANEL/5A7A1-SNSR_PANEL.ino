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

const byte insSwPins[8] = { DcsBios::PIN_NC, INS_CV, INS_GND, INS_NAV, INS_IFA, INS_GYRO, INS_GB, INS_TEST };  ///< Off position doesn't have a pin.
const byte radarSwPins[4] = { DcsBios::PIN_NC, RDR_STBY, RDR_OPR, RDR_EMERG };                                 ///< Off position doesn't have a pin.

// Connect switches to DCS-BIOS
DcsBios::Switch3Pos flirSw("FLIR_SW", FLIR_ON, FLIR_OFF);
DcsBios::Switch2Pos lstNflrSw("LST_NFLR_SW", LST_ON, true);
DcsBios::Switch2Pos ltdRSw("LTD_R_SW", LTDR_ARM);

///@todo If/When https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/pull/56 is accepted by DCS Skunkworks change to DcsBios::SwitchMultiPos class.
SwitchMultiPosDebounce insSw("INS_SW", insSwPins, 8, false, 100);
SwitchRadar radarSw("RADAR_SW", "RADAR_SW_PULL", 3, radarSwPins, 4, false, 100);

/// If the LTD/R switch is turned off virtually or physically update the mag-switch state.
void onLtdRSwChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      digitalWrite(LTDR_ARM_MAG, LOW);  // switch turned off physically or virtually in the sim, turn off mag.
      break;
    case 1:
      digitalWrite(LTDR_ARM_MAG, HIGH);                                                            // hold the mag-swtich in the 'ARM' position.
  }
}
DcsBios::IntegerBuffer ltdRSwBuffer(FA_18C_hornet_LTD_R_SW, onLtdRSwChange);


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