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
 * @file 4A3A3-FIRE_TEST_PANEL.ino
 * @author Arribe
 * @date 03.30.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the BRK PRESS gauge, BRAKE handle &amp; FIRE TEST panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A3A3
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 3
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | Break Pressure Stepper M1
 * 2   | Break Pressure Stepper M2
 * A2  | Break Pressure Stepper M3
 * 3   | Break Pressure Adafruit LED Data In
 * A1  | Break Pressure Stepper M4
 * 4   | Fire Switch 1
 * A0  | Fire Switch 2
 * 15  | Break Handle Secure, pushed in.
 * 6   | Break Handle Emerg
 * 14  | Break Handle Park
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 

   #define DCSBIOS_RS485_SLAVE 3 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#include <Stepper.h>
#include "Adafruit_NeoPixel.h"

// May need to play with steps, zero value, and max to get the needle to be close to the needle position in sim.
#define STEPS 310                ///< Set steps per revolution
#define BREAK_PRESSURE_ZERO -60  ///< Zero position of break pressure stepper to be bottom of red indicator line.
#define BREAK_PRESSURE_MAX 160   ///< Max value for DCS Bios read's mapped value to get break pressure stepper to 4.

// Define pins for DCS-BIOS per interconnect diagram.
#define BPKP_M1 A3      ///< Break Pressure Stepper M1
#define BPKP_M2 2       ///< Break Pressure Stepper M2
#define BPKP_M3 A2      ///< Break Pressure Stepper M3
#define BRKP_DIN 3      ///< Break Pressure Adafruit LED Data In
#define BPKP_M4 A1      ///< Break Pressure Stepper M4
#define FIRE_SW1 4      ///< Fire Switch 1
#define FIRE_SW2 A0     ///< Fire Switch 2
#define BRKH_SECURE 15  ///< Break Handle Secure, pushed in
#define BRKH_EMERG 14   ///< Break Handle Emerg
#define BRKH_PARK 6     ///< Break Handle Park

#define LED_COUNT 1    ///< Number of backlighting pixels for the break pressure gauge.
#define BRIGHTNESS 50  ///< Brightness of the LED

Adafruit_NeoPixel breakPressureLight = Adafruit_NeoPixel(LED_COUNT, BRKP_DIN, NEO_GRB + NEO_KHZ800);  ///< lighting initialization

Stepper stepperBreakPressure(STEPS, BPKP_M1, BPKP_M2, BPKP_M3, BPKP_M4);  ///< Break pressure stepper initializaiton

//Declare variables for custom non-DCS Bios logic
byte breakState = 1;  ///< breakState = 1:Park, 2:Emergency, or 0:Off

int breakPressureValue = 0;  ///< Used to hold the value from DCS as read during stepping logic.
int breakPressurePos = 0;    ///< variable to hold the break pressure stepper position.

// Connect switches to DCS-BIOS
DcsBios::Switch3Pos fireTestSw("FIRE_TEST_SW", FIRE_SW1, FIRE_SW2);

/// Get DCS Sim state for break pressure
void onHydIndBrakeChange(unsigned int newValue) {
  int temp = map(newValue, 0, 65535, 0, BREAK_PRESSURE_MAX);
  if (breakPressureValue != temp) {
    breakPressureValue = temp;
  }
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);

void onInstrIntLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      breakPressureLight.fill(0, 0, LED_COUNT);  ///< Off
      breakPressureLight.show();
      break;
    default:
      breakPressureLight.fill(breakPressureLight.Color(map(newValue, 0, 65535, 0, 255), 0, 0), 0, LED_COUNT);  ///< Set light to Green - GRB LED
      breakPressureLight.show();
      break;
  }
}
DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*
* @note If the break pressure needle doesn't end up at the zero position, you could remove power/usb connection to the pro-micro and then 
* plug it back in to re-run the initialization again prior to connecting the comport to DCS.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(BRKH_EMERG, INPUT_PULLUP);   ///< set emerg pulled pin mode for direct reads
  pinMode(BRKH_PARK, INPUT_PULLUP);    ///< set park pulled pin mode for direct reads
  pinMode(BRKH_SECURE, INPUT_PULLUP);  ///< set handle pushed in pin mode for direct reads

  breakPressureLight.begin();                    ///< begin break light
  breakPressureLight.show();                     ///< Clear the LED
  breakPressureLight.setBrightness(BRIGHTNESS);  ///< set the initial brightness

  stepperBreakPressure.setSpeed(10);               ///< set the stepper speed
  stepperBreakPressure.step(240);                  // reset needle to far counter-clockwise position
  stepperBreakPressure.step(BREAK_PRESSURE_ZERO);  // reset to 0%, bottom of red portion of the break pressure position
  breakPressurePos = 0;                            ///< update needle position to 0
  breakPressureValue = 0;                          ///< update pressure value to 0
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* ## Break Handle Logic
*
* By keeping track of the break handle position via variable breakState = 1:Park, 2:Emergency, or 0:Off
* Read the Emer and Parking break lever switch states
*
* Based on breakState, determine if the handle was moved into or out of the Park, Emer, or Off (pushed in position).
* Then send the corresponding rotate and pull DcsBios Messages to move from the prior break handle position to the new break handle position.
* 
* To get out of park requires a slight counter-clockwise rotation in the sim.  This is not implemented in the physical controls,
* but is required to be sent in a DcsBios message to push in the break handle within the sim.  The logic accounts for this 'extra' rotation step.
*
* @note The break handle rotate does not have any switches so it's impossible to know which way the handle is actually facing when it's pushed in.
*
* ## Break Pressue Gauge
* Break pressure gauge is updated based on the value returned from DCS Bios relative to the current position of the needle.
* 
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  // Break Handle Logic:
  bool eBrkSwitchState = !digitalRead(BRKH_EMERG);  // read the emergency break switch state
  bool pBrkSwitchState = !digitalRead(BRKH_PARK);   // read the parking break switch state

  switch (breakState) {                                           // breakState = 1:Park, 2:Emergency, or 0:Off
    case 0:                                                       // breakState == 0, breaks are off determine if pulled and which direction
      if (eBrkSwitchState == 1) {                                 // Emerg break switch state == 1, Break handle pulled for emergency break
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");  // DcsBios message to pull handle
        breakState = 2;                                           // Update break state to emergency breaks applied
      }
      if (pBrkSwitchState == 1) {                                   // Parking break switch state == 1, break handle pulled for parking break
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "1");  // Rotate parking break handle to parking / vertical position
        delay(50);
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");  // DcsBios message to pull handle
        breakState = 1;                                           // Update break state to parking breaks applied
      }
      break;
    case 1:                                                         // breakState == 1, Parking break on
      if (pBrkSwitchState == 0) {                                   // Parking break switch state == 0, break handle pushed in
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "2");  // DcsBios message to rotate the break handle to the 'release' position (not modeled in the physical pit)
        delay(50);                                                  // delay for the animation to complete
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");    // DcsBios message to push the break handle in
        delay(50);                                                  // delay for the animation to complete
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "0");  // DcsBios message to rotate the break handle to the emergency / horizontal position
        breakState = 0;                                             // Update break state to off
      }
      break;
    case 2:                                                       // breakState == 2, Emergency break on
      if (eBrkSwitchState == 0) {                                 // Emergency break swtich state == 0, break handle pushed in
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");  // DcsBios message to push the break handle in
        breakState = 0;                                           // Update break state to off
      }
      break;
  }

  // Break Pressue Gauge Logic:
  if (abs(breakPressureValue - breakPressurePos) > 2) {  // difference is greater than 2 steps.
    if ((breakPressureValue - breakPressurePos) > 0) {   // If value is greater than current position rotate needle clockwise
      stepperBreakPressure.step(-1);                     // Step one value clockwise
      breakPressurePos++;                                // update break pressure position
    }
    if ((breakPressureValue - breakPressurePos) < 0) {  // If value is less than current position rotate needle counter-clockwise
      stepperBreakPressure.step(1);                     // Step one value counter-clockwise
      breakPressurePos--;                               // update break pressure position
    }
  }
}
