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
 * @file 4A7A2-OBOGS_PANEL.ino
 * @author Arribe
 * @date 03.05.2024
 * @version u.0.0.1 (partially untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram for the NUC Switch, but it was not yet tested on hardware.  The NUC Switch is a simple 2 position DCSBios switch. It is highly likely it will work fine.
 * All other switches were tested successfully.
 * @brief Controls the OBOGS panel, MC & HYD ISO panel, & NUC WPN panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A7A2
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 10
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Onboard Oxygenerator
 * 2   | Oxygen Flow Rate
 * A2  | Nuclear Weapon Consent (**NOT** implemented in DCS)
 * 3   | MC 1 OFF
 * A1  | MC 2 OFF
 * 4   | HYD Override
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 10 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__)  || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL       ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
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
#define OBOGS_SW1 A3    ///< Onboard Oxygenerator
#define OXY_FLOW_SW1 2  ///< Oxygen Flow Rate
#define NUC_SW1 A2      ///< Nuclear Weapon Consent Switch (**NOT** implemented in DCS).
#define MC_SW1 3        ///< MC 1 OFF
#define MC_SW2 A1       ///< MC 2 OFF
#define HYD_SW1 4       ///< HYD Override

//DCSBios OXY FLOW workaround
#define OXY_MSG "OXY_FLOW"     ///< DCSBios Message to move the OXY_FLOW knob.
#define MAX_FLOW "+65535"      ///< The OXY FLOW knob in the sim is a 90 degree pot, with max = 65535 we'll add the full on value.
#define MIN_FLOW "-65535"      ///< The OXY FLOW Knob in the sim is a 90 degree pot, with min = 0 we'll subtract the full on value.
#define REVERSE_OXY_FLOW true  ///< If OXY FLOW knob rotates in the opposite direction in the sim compared to the physical switch then change this to false.

bool lastBtnState = LOW;             ///< Last button state for oxy flow logic, initialize to off.
bool buttonDebounceState = LOW;      ///< Button debounce state for oxy flow logic.
unsigned long lastDebounceTime = 0;  ///< Variable to hold last button update time for Oxy flow logic, initialize to 0.
unsigned long debounceDelay = 10;    ///< The debounce delay duration in ms, **increase if the output flickers**.

// Connect switches to DCS-BIOS
//OBGS Panel
DcsBios::Switch2Pos obogsSw("OBOGS_SW", OBOGS_SW1);

//Mission Computer and Hydraulic Isolate Panel
DcsBios::Switch2Pos hydIsolateOverrideSw("HYD_ISOLATE_OVERRIDE_SW", HYD_SW1);
DcsBios::Switch3Pos mcSw("MC_SW", MC_SW1, MC_SW2);

/**
* @note The NUC Weapon Switch has no function in DCS.  This will only move the switch in the sim to match the physical cockpit.
*/
DcsBios::Switch2Pos nucWpnSw("NUC_WPN_SW", NUC_SW1);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(OXY_FLOW_SW1, INPUT_PULLUP);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
* 
* @note DCSBios control reference has 2 options for the OXY FLOW knob, Potentiometer or Rotary Encoder.  
* To use the Rotary Encoder built-in-option requires each postion to be connected to an Arduino Pin.
* It is possible to connect the spec'd knob to a second pin on the Arduino to use the Rotary Encoder sample code 
* with the step size set to "-65535", "+65535" for full-off and full-on.
* 
* ###OXY_FLOW Logic 
* To overcome the difference between physical switch and the sim's modeling of it, the custom logic follows DCSBios switch methodology for debounce.\n
* -# Read the current button position.
  -# If REVERSE_OXY_FLOW direction = true, then reverse current button position read.
  -# Then by tracking the current state versus an intermediary state and comparing the time difference from when the button last changed is longer than the debounce delay,
  send the DCSBios message to move the OXY FLOW knob to either full-on or full-off and update the variables to be ready for the next switch movement.
* 
*
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  bool buttonState = digitalRead(OXY_FLOW_SW1);
  if (REVERSE_OXY_FLOW == true) {  // if reverse the button position move is true
    buttonState = !buttonState;    // then set button state to opposite.
  }
  unsigned long now = millis();  // get current time.


  if (buttonState != buttonDebounceState) {  // If the button state changed from the button debounce state
    lastDebounceTime = now;                  // then save the current time.
    buttonDebounceState = buttonState;       // and save the current button state to the debounce state.
  }

  if ((now - lastDebounceTime) >= debounceDelay) {                                                 // if the difference in time between now and when the switch last changed is greater than the debounce time
    if (buttonDebounceState != lastBtnState) {                                                     // and if the button debounce state is not the same as the last button state
      if (DcsBios::tryToSendDcsBiosMessage(OXY_MSG, buttonState == HIGH ? MIN_FLOW : MAX_FLOW)) {  // then send the corresponding DCSBios Message to move the switch in the correct direction.
        lastBtnState = buttonDebounceState;                                                        // and then save the button's current state to the last state in preparation for the next time it's flipped.
      }
    }
  }
}
