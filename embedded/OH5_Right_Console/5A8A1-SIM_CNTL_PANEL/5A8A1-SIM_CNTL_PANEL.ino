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
 * @file 5A8A1-SIM_CNTL_PANEL.ino
 * @author Arribe
 * @date 03.08.2024
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the SIM CNTL panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 5A8A1
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 7
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | View Chase
 * 2   | View External
 * A2  | View Flyby
 * 3   | View Weapon
 * A1  | View Enemy
 * 4   | View Hud
 * A0  | View Map
 * 15  | Head Tracking Freeze
 * 6   | Head Tracking Center
 * 14  | Time Fast
 * 7   | Time Real
 * 16  | Toggle NVG
 * 8   | Toggle Labels
 * 10  | Game Pause
 * 9   | Game Freeze
 * No Pin | View Cockpit

 * @note This is a HID only panel.  The switches need to be mapped in the DCS controller setup for the views, and functions.
 * 
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
#include "Joystick.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define VIEW_CHASE A3  ///< View Chase
#define VIEW_EXT 2     ///< View External
#define VIEW_FLYBY A2  ///< View Flyby
#define VIEW_WPN 3     ///< View Weapon
#define VIEW_ENMY A1   ///< View Enemy
#define VIEW_HUD 4     ///< View Hud
#define VIEW_MAP A0    ///< View Map
#define HT_FRZE 15     ///< Head Tracking Freeze
#define HT_CTR 6       ///< Head Tracking Center
#define TIME_FAST 14   ///< Time Fast
#define TIME_REAL 7    ///< Time Real
#define TOG_NVG 16     ///< Toggle NVG
#define TOG_LABL 8     ///< Toggle Labels
#define GME_PAUSE 10   ///< Game Pause
#define GME_FREEZE 9   ///< Game Freeze

/// Array of pins to simplify the code working with the switches as joystick buttons.
const int* pins[15]{ VIEW_CHASE, VIEW_EXT, VIEW_FLYBY, VIEW_WPN, VIEW_ENMY, VIEW_HUD, VIEW_MAP, HT_FRZE, HT_CTR, TIME_FAST, TIME_REAL, TOG_NVG, TOG_LABL, GME_PAUSE, GME_FREEZE };

bool currentViewState[7]{ LOW, LOW, LOW, LOW, LOW, LOW, LOW };  ///< Initilize the array of view rotary state, for logic to determine if View Cockpit is selected.
const bool allOff[7]{ LOW, LOW, LOW, LOW, LOW, LOW, LOW };      ///< Initializes an array for allOff comparison.
unsigned int debounceDelay = 250;                               ///< View rotary debounce, if the view jumps to the Cockpit view while rotating increase the debounce delay.
unsigned long cockpitViewSteadyStateTime = 0;                   ///< Variable to hold the view steady state time  for debounce logic.
bool cockpitView = false;                                       ///< variable to hold if cockpit view is on or not

/// Joystick consiting of 16 buttons and no axis.
Joystick_ Joystick = Joystick_(
  0x5A81,
  JOYSTICK_TYPE_JOYSTICK,
  16,
  0,
  false,
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

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  for (int j = 0; j < 15; j++) {
    pinMode(pins[j], INPUT_PULLUP);

    Serial.begin(9600);

    Joystick.begin();
  }
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* The view rotary switch doesn't have position 1 connected to a pin on the Arduino.
* The logic loops over the pins, reading the button states.  The first 6 pins are for the view rotary.
* The view rotary position state is saved to an array.  If the current values is equal to the 'all off' values, then:
* -# It is assumed that the rotary is pointing to CKPT.
* -# While the rotary switch rotates all of the positions are not connected to ground.
*  To prevent false readings debounce logic is used to ensure that the view won't flip excessively.
* -# The switch needs to point to CKPT position for at least the debounceDelay duration before turning on.
*
* @attention Be deliberate in the view rotation, clicking one spot at a time at a smooth pace.
* Rapid movements, or delayed rotation with the knob pointing between the stops may lead to false readings
* that the switch is pointing to CKPT.
*/
void loop() {



  for (int j = 0; j < 15; j++) {
    if (j < 7) {                                    // If reading view rotary need to save state to later determine if the rotary is pointing to CKPT.
      currentViewState[j] = !digitalRead(pins[j]);  // Read the view rotary positions.
      Joystick.setButton(j, currentViewState[j]);   // Set the view rotary joystick button as read.
    } else {
      Joystick.setButton(j, !digitalRead(pins[j]));  // Not the view rotary, set the joystick button as read.
    }
    unsigned long now = millis();
    if (memcmp(currentViewState, allOff, 7) == 0) {                // Determine view rotary reads were all off
      if (cockpitView == false) {                                  // No view read, rotary pointing to CKPTstate, check if changing to true
        if ((now - cockpitViewSteadyStateTime) > debounceDelay) {  // Wait until the switch is certain to be in the CKPT position.
          Joystick.setButton(15, HIGH);                            // Set the CKPT button to on.
          cockpitView = true;                                      // Update the cockpitView to indicate the button is on.
        } else {
          //do nothing wait for the debounce time to elapse to esure it wasn't caused from rotating the switch.
        }
      } else {
        // cockpit view is already on, no change.
      }
    } else {
      Joystick.setButton(15, LOW);       // At least one view reads as on, rotary NOT pointing to CKPT.
      cockpitView = false;               // Set the cockpitView to false so it can be turned on next time.
      cockpitViewSteadyStateTime = now;  // Update the cockpitViewSteadyStateTime in prep for the next iteration.
    }
  }
}
