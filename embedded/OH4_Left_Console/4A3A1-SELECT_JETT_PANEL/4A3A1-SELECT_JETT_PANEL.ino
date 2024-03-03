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
 * @file 4A3A1-SELECT_JETT_PANEL.ino
 * @author Arribe
 * @date 03.01.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the SELECT JETT panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A3A1
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 2
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A0  | Hook Bypass Switch
 * A1  | Launch Bar Switch
 * A2  | Flaps Switch Position 2
 * A3  | Flaps Switch Position 1
 * 2   | Launch Bar Mag Switch
 * 3   | Hook Bypass Mag Switch
 * 4   | Landing Taxi Light
 * 6   | Selector Jettision Position 1
 * 7   | Selector Jettision Position 3
 * 8   | Selector Jettision Position 8
 * 10  | Antiskid Switch
 * 14  | Selector Jettision Position 2
 * 15  | Selector Jettision Push
 * 16  | Selector Jettision Position 4
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
 *
//#define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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

/**
* @brief Pilots may want the launch bar to automatically release when the throttles advance to MIL power.
* The sim's version of the F/A-18 does *NOT* have this feature.  The define allows setting it as desired, default is *false*.
*
*/
#define LBAR_SW_AUTORETRACT false

/**
* @brief Delay for Hook Bypass mag-switch auto-off when hook lever is lowered
*/
#define HOOK_DELAY 3200

// Define pins for DCS-BIOS per interconnect diagram.
#define HOOKB_SW A0 ///< Hook Bypass Switch
#define LBAR_SW A1 ///< Launch Bar Switch
#define FLAPS_SW2 A2 ///< Flaps Switch Position 2
#define FLAPS_SW1 A3 ///< Flaps Switch Position 1
#define LBAR_RET 2    ///< Launch Bar Switch Mag
#define HOOK_FIELD 3  ///< Hook Bypass Switch Mag
#define LADG_SW 4 ///< Landing Taxi Light
#define SJET_SW1 6 ///< Selector Jettision Position 1
#define SJET_SW3 7 ///< Selector Jettision Position 3
#define SJET_SW5 8 ///< Selector Jettision Position 8
#define ASKID_SW 10 ///< Antiskid Switch
#define SJET_SW2 14 ///< Selector Jettision Position 2
#define SJET_PUSH 15 ///< Selector Jettision Push
#define SJET_SW4 16 ///< Selector Jettision Position 4

//Declare variables for custom non-DCSBios logic for mag-switches
bool hookBypassMagState = LOW;    ///< Initializing for correct cold/ground start position as switch releases on power off.
bool hookBypassState = LOW;       ///< Initializing for correct cold/ground start position as switch releases on power off.
bool launchBarMagState = LOW;     ///< Initializing for correct cold/ground start position as switch releases on power off.
bool launchBarState = LOW;        ///< Initializing for correct cold/ground start position as switch releases on power off.
bool hookLeverState = LOW;        ///< Initializing for correct cold/ground start position as switch releases on power off.
unsigned long hookLeverTime = 0;  ///< Initializing variable to hold time of last hook change.
bool wowLeft = true;              ///< Initializing weight-on-wheel value for cold/ground start.
bool wowRight = true;             ///< Initializing weight-on-wheel value for cold/ground start.
bool wowNose = true;              ///< Initializing weight-on-wheel value for cold/ground start.
unsigned int rpmL = 0;            ///< Initializing engine RPM for cold start
unsigned int rpmR = 0;            ///< Initializing engine RPM for cold start

// Connect switches to DCS-BIOS
DcsBios::Switch2Pos antiSkidSw("ANTI_SKID_SW", ASKID_SW);
DcsBios::Switch3Pos flapSw("FLAP_SW", FLAPS_SW1, FLAPS_SW2);
DcsBios::Switch2Pos hookBypassSw("HOOK_BYPASS_SW", HOOKB_SW);
DcsBios::Switch2Pos launchBarSw("LAUNCH_BAR_SW", LBAR_SW);
DcsBios::Switch2Pos ldgTaxiSw("LDG_TAXI_SW", LADG_SW);
DcsBios::Switch2Pos selJettBtn("SEL_JETT_BTN", SJET_PUSH);
const byte selJettKnobPins[5] = { SJET_SW5, SJET_SW4, SJET_SW3, SJET_SW2, SJET_SW1 };
DcsBios::SwitchMultiPos selJettKnob("SEL_JETT_KNOB", selJettKnobPins, 5);

// DCSBios reads to save airplane state information.

/**
* @brief DCSBios read back of Hook Bypass position.  If the Switch is turned off virtually in the sim, then turn off the hook bypass mag.
*/
void onHookBypassSwChange(unsigned int newValue) {
  if (newValue == hookBypassState) {
    return;  //no state change, no change to magnet.
  } else {
    switch (newValue) {
      case 0:  // Switch is down in carrier position turn off mag.
        digitalWrite(HOOK_FIELD, LOW);
        hookBypassMagState = LOW;
        break;
      case 1:  // Switch is up in field position, turn on mag.
        digitalWrite(HOOK_FIELD, HIGH);
        hookBypassMagState = HIGH;
        break;
      default:
        break;
    } hookBypassState = newValue;
  }
} DcsBios::IntegerBuffer hookBypassSwBuffer(0x7480, 0x4000, 14, onHookBypassSwChange);

/**
* @brief Need to save the hook lever state to test turning off the hook bypass mag-switch in the main loop.
*  
*/
void onHookLeverChange(unsigned int newValue) {
  hookLeverState = newValue;
  hookLeverTime = millis();
} DcsBios::IntegerBuffer hookLeverBuffer(0x74a0, 0x0200, 9, onHookLeverChange);

/**
* @brief DCSBios read back of Hook Bypass position.  If the Switch is turned off virtually in the sim, then turn off the hook bypass mag.
  The Launch bar switch is only held down when there is weight-on-wheels.
*/
void onLaunchBarSwChange(unsigned int newValue) {
  if (newValue == launchBarState) {
    return;  //no state change, no launch bar magnet state update.
  } else {
    switch (newValue) {
      case 0:  // Launch bar switch turned off
        digitalWrite(LBAR_RET, LOW);
        launchBarMagState = LOW;
        break;
      case 1:  // Launch bar switch turned on, only lock launch bar down if weight on wheels
        if (wowLeft == wowRight == wowNose == true) {
          digitalWrite(LBAR_RET, HIGH);
          launchBarMagState = HIGH;
        } else {
          digitalWrite(LBAR_RET, LOW);
          launchBarMagState = LOW;
        }
        break;
      default:
        break;
    }
  }
  launchBarState = newValue;
} DcsBios::IntegerBuffer launchBarSwBuffer(0x7480, 0x2000, 13, onLaunchBarSwChange);

//Engine RPM needed for launch bar mag-switch
void onIfeiRpmLChange(char* newValue) {
  rpmL = atoi(newValue);
} DcsBios::StringBuffer<3> ifeiRpmLBuffer(0x749e, onIfeiRpmLChange);

void onIfeiRpmRChange(char* newValue) {
  rpmR = atoi(newValue);
} DcsBios::StringBuffer<3> ifeiRpmRBuffer(0x74a2, onIfeiRpmRChange);

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

  pinMode(LBAR_RET, OUTPUT);
  pinMode(HOOK_FIELD, OUTPUT);

  digitalWrite(LBAR_RET, LOW);
  digitalWrite(HOOK_FIELD, LOW);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
*
*
*
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

/**
 ### Launch Bar Auto Retract Logic
*  If the launch bar mag-switch is held in extend position, then: \n
*  -# if no weight on wheels retract the launch bar. \n
*  -# if the launch bar auto-retract is defined as true and both engines' RPM is >85 then retract launch bar. \n
*   @note If launch bar auto-retract is true, when connecting to the catapault it may be easier to keep one engine under 80% while advancing the other with enough power get over the shuttle.
* 
*/
  if (launchBarMagState == HIGH) {
    switch (launchBarState) {
      case LOW:  //launch bar switch in retract
        //do nothing the state change method will retract the launch bar
        break;
      case 1:   //launch bar switch in extend
        if (wowLeft == wowRight == wowNose == false){ // no wheight on wheels retract launch bar
          digitalWrite(LBAR_RET, LOW);
          launchBarMagState = LOW;  // mag is off
          break;
        }        
        if ((LBAR_SW_AUTORETRACT == true) && (rpmL >= 85 && rpmR >= 85)) {  //If Launch bar auto retract on throttle is true and both engines over 80% rpm turn off mag
          digitalWrite(LBAR_RET, LOW);
          launchBarMagState = LOW;  // mag is off
        }
        break;
    }
  }

/**
 ### Hook bypass override cancel logic
*  If the hook bypass mag-switch is held in field position with no weight on wheels and hook lever is down, then: \n
*  -# if hook lever state change time is longer than the hook delay, turn off hook bypass back to carrier. \n
*  -# else continue until either the time is reached or the hook lever is raised prior to cancelling the hook bypass switch.
*  @bug Potential bug, the hook bypass autocancel delay time varies when running sketch.  The sim's the autocancel delay is ~3.8 seconds.  In testing the sketch the autocancel will be at least 3.2 seconds, but can be as long as ~8 seconds.  This is likely due to delays in DCSBios sending state updates.
* 
*/
  if ((hookBypassMagState == HIGH) && (wowLeft == wowRight == wowNose == false) && (hookLeverState == LOW)) {  // If the hook bypass mag is on, with weight off wheels, and the hook lever is down.
    if ((millis() - hookLeverTime ) > HOOK_DELAY) {                                                               // if longer than the hook override auto cancel delay
      digitalWrite(HOOK_FIELD, LOW);
      hookBypassMagState = LOW;
    } else {
      //wait for time to pass to ensure hook lever isn't raised before the autocancel time is met.
    }
  }
}
