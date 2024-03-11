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
 * @note The INS and RADAR rotary off positions are flakey at best.  If we want solid rotation movements, then the 'off' position pins need to be connected to the controller.
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

const byte insSwPins[7] = { INS_CV, INS_GND, INS_NAV, INS_IFA, INS_GYRO, INS_GB, INS_TEST };  ///< Off position doesn't have a pin.
const byte radarSwPins[3] = { RDR_STBY, RDR_OPR, RDR_EMERG };                                 ///< Off position doesn't have a pin.
byte currentInsSwState[7];                                                                    ///< Array to hold the digital reads of the INS switch.
byte insDebounceState[7];                                                                     ///< Array to hold the debounce state for the INS.
byte lastInsSwState[8] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW };                          ///< Array to hold last INS switch state, 'off' is in the last position.
byte currentRadarSwSate[3];                                                                   ///< Array to hold the digital reads of the Radar switch.
byte radarDebounceSate[3];                                                                    ///< Array to hold the debounce state for the radar.
byte lastRadarSwState[4] = { LOW, LOW, LOW, LOW };                                            ///< Array to hold last radar switch state, 'off' is in the last position.
const bool allOff[7] = { LOW, LOW, LOW, LOW, LOW, LOW, LOW };                                 ///< Array used to test if the rotary reads were all LOW.
bool radarRotaryPulled = false;

const unsigned int offDelay = 200;      ///< delay for the INS and Radar switches' logic to move to off position.
const unsigned int debounceDelay = 10;  ///< debounce delay for the rotary swtichtes.
unsigned long lastRadarTime = 0;        ///< last time Radar switch was updated.
unsigned long radarDebounceTime[3];     ///< Array to hold the debounce times for each pin position.
unsigned long lastInsTime = 0;          ///< last time INS switch was updated.
unsigned long insDebounceTime[7];       ///< Array to hold the debounce times for each pin position.
unsigned long now;                      //variable to hold current time.

// Connect switches to DCS-BIOS
DcsBios::Switch3Pos flirSw("FLIR_SW", FLIR_ON, FLIR_OFF);
DcsBios::Switch2Pos lstNflrSw("LST_NFLR_SW", LST_ON);
DcsBios::Switch2Pos ltdRSw("LTD_R_SW", LTDR_ARM);


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

  for (int j = 0; j < 7; j++) {
    pinMode(insSwPins[j], INPUT_PULLUP);
  }

  for (int j = 0; j < 3; j++) {
    pinMode(radarSwPins[j], INPUT_PULLUP);
  }

  pinMode(LTDR_ARM_MAG, OUTPUT);
  digitalWrite(LTDR_ARM_MAG, LOW);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* For the LTD/R switch to hold the A/G master mode needs to be on, and the landing gear raised.
*
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  /**
* INS Switch Logic:
* -# Loop over the INS pins, if the INS switch state changed from last read and is pointing at a non-off position send INS rotation update message.
* -# Test if the INS rotary isn't pointing to any non-off position.  If longer than the off delay we deduce that the switch is pointing to off, send the INS rotation pointing to off message.
*
*/
  for (int j = 0; j < 7; j++) {                         // loop over the INS pins.
    currentInsSwState[j] = !digitalRead(insSwPins[j]);  // Read the INS rotary positions and hold to test if swtich is off.
    now = millis();                                     // refresh now

    if (currentInsSwState[j] != insDebounceState[j]) {  // Test if the INS switch state changed.
      insDebounceTime[j] = now;                         // save debounce time for this position.
      insDebounceState[j] = currentInsSwState[j];       // set debounce state to current read.
    }

    if ((now - insDebounceTime[j]) >= debounceDelay) {               // If the debounceDelay has passed.
      if (insDebounceState[j] != lastInsSwState[j]) {                // If the debounce state is not the same as the last state.
        if (insDebounceState[j] == HIGH) {                           // if the swith is pointing to the position send DCS Bios message.
          char positionName[3];                                      ///< string to hold the position name.
          sprintf(positionName, "%01d", j + 1);                      // create position name: j + 1 because the off pin (position 0) isn't hooked up.
          DcsBios::tryToSendDcsBiosMessage("INS_SW", positionName);  // send DCS Bios message to rotate the knob.
        }
        lastInsSwState[j] = insDebounceState[j];  // debounce conditions met, set last state = to debounce state.
      }
    }
  }

  now = millis();                                         // refresh time
  if (memcmp(lastInsSwState, allOff, 7) == 0) {           // Determine INS rotary debounced reads were all LOW, to deduce if INS pointing to 'off' position
    if (lastInsSwState[8] == LOW) {                       // INS rotary pointing to OFF, check if changing position in sim.
      if ((now - lastInsTime) > offDelay) {               // Wait until the switch is certain to be in the OFF position.
        DcsBios::tryToSendDcsBiosMessage("INS_SW", "0");  // Send INS off position to DCS.
        lastInsSwState[8] = HIGH;                         // Update the INS Off position to indicate the rotary is pointing to off.
      } else {
        //do nothing wait for the off delay time to elapse to esure it wasn't caused from the rotating switch.
      }
    } else {
      // INS off position already set, no change.
    }
  } else {                    // INS rotary pointing to something other than OFF.
    lastInsSwState[8] = LOW;  // Set the INS off position state to LOW.
    lastInsTime = now;        // Update the last INS update time to reset the time delta for calculating the time delta for moving to off.
  }


  /**
* Radar Switch Logic:
* -# Loop over the radar pins, if the radar switch state changed from last read and is pointing at a non-off position send radar rotation update message.
* -# Test if the radar rotary isn't pointing to any non-off position.  If longer than the off delay we deduce that the rotary is pointing to off, send the radar rotation pointing to off message.
*
*/
  for (int j = 0; j < 3; j++) {  // loop over the radar pins.

    currentRadarSwSate[j] = !digitalRead(radarSwPins[j]);  // Read the radar rotary positions and hold to test if swtich is off.
    now = millis();                                        // refresh the time.

    if (currentRadarSwSate[j] != radarDebounceSate[j]) {  // Test if the radar switch state changed.
      radarDebounceTime[j] = now;                         // set radar debounce time to now.
      radarDebounceSate[j] = currentRadarSwSate[j];       // set the debounce state to current state.
    }

    if ((now - radarDebounceTime[j]) >= debounceDelay) {             // if the debounce delay was met.
      if (radarDebounceSate[j] != lastRadarSwState[j]) {             // if the debounce state not the last state.
        if (radarDebounceSate[j] == HIGH) {                          // if the swith is pointing to the position send DCS Bios message.
          if (j == 2) {                                              // Pointing to Emer, need to send pull message first
            DcsBios::tryToSendDcsBiosMessage("RADAR_SW_PULL", "1");  // pull the radar rotary.
            radarRotaryPulled = true;                                // remember if the rotary is pulled.
            delay(300);                                              // Delay determined via trial and error.  The rotation below has to be made after the slow pull animation is complete.
          } else {
            if (radarRotaryPulled == true) {
              DcsBios::tryToSendDcsBiosMessage("RADAR_SW_PULL", "1");  // make sure radar knob isn't pulled, may be a DCS bug to pull again i.e. rotate mouse wheel back.
              radarRotaryPulled = false;                               // remember that the rotary is NOT pulled.
              delay(300);                                              // need to delay so the rotation below will work.
            }
          }
          char positionName[3];                                        // string to hold the postion name.
          sprintf(positionName, "%01d", j + 1);                        // set the postion name, j + 1 because the off pin (position 0) isn't hooked up.
          DcsBios::tryToSendDcsBiosMessage("RADAR_SW", positionName);  // sent rotation message.
        }
        lastRadarSwState[j] = radarDebounceSate[j];  // debounce conditions complete save debounce state to last state.
      }
    }
  }

  now = millis();                                           // refresh time
  if (memcmp(lastRadarSwState, allOff, 3) == 0) {           // Determine radar rotary debounced reads were all LOW, to deduce if radar pointing to 'off' position
    if (lastRadarSwState[4] == LOW) {                       // radar rotary pointing to OFF, check if changing position in sim.
      if ((now - lastRadarTime) > offDelay) {               // Wait until the switch is certain to be in the OFF position.
        DcsBios::tryToSendDcsBiosMessage("RADAR_SW", "0");  // Send radar off position to DCS.
        lastRadarSwState[4] = HIGH;                         // Update the radar Off position to indicate the rotary is pointing to off.
      } else {
        //do nothing wait for the off delay time to elapse to esure it wasn't caused from the rotating switch.
      }
    } else {
      // radar off position already set, no change.
    }
  } else {                      // radar rotary pointing to something other than OFF.
    lastRadarSwState[4] = LOW;  // Set the radar off position state to LOW.
    lastRadarTime = now;        // Update the last radar update time to reset the time delta for calculating the time delta for moving to off.
  }
}
