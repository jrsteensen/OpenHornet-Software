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
 * A3  | Brake Pressure Stepper M1
 * 2   | Brake Pressure Stepper M2
 * A2  | Brake Pressure Stepper M3
 * 3   | Brake Pressure Adafruit LED Data In
 * A1  | Brake Pressure Stepper M4
 * 4   | Fire Switch 1
 * A0  | Fire Switch 2
 * 15  | Brake Handle Secure, pushed in.
 * 6   | Brake Handle Emerg
 * 14  | Brake Handle Park
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
#define BRAKE_PRESSURE_ZERO -60  ///< Zero position of brake pressure stepper to be bottom of red indicator line.
#define BRAKE_PRESSURE_MAX 160   ///< Max value for DCS Bios read's mapped value to get brake pressure stepper to 4.

// Define pins for DCS-BIOS per interconnect diagram.
#define BPKP_M1 A3      ///< Brake Pressure Stepper M1
#define BPKP_M2 2       ///< Brake Pressure Stepper M2
#define BPKP_M3 A2      ///< Brake Pressure Stepper M3
#define BRKP_DIN 3      ///< Brake Pressure Adafruit LED Data In
#define BPKP_M4 A1      ///< Brake Pressure Stepper M4
#define FIRE_SW1 4      ///< Fire Switch 1
#define FIRE_SW2 A0     ///< Fire Switch 2
#define BRKH_SECURE 15  ///< Brake Handle Secure, pushed in
#define BRKH_EMERG 14   ///< Brake Handle Emerg
#define BRKH_PARK 6     ///< Brake Handle Park

#define LED_COUNT 1    ///< Number of backlighting pixels for the brake pressure gauge.
#define BRIGHTNESS 50  ///< Brightness of the LED

Adafruit_NeoPixel brakePressureLight = Adafruit_NeoPixel(LED_COUNT, BRKP_DIN, NEO_GRB + NEO_KHZ800);  ///< lighting initialization

Stepper stepperBrakePressure(STEPS, BPKP_M1, BPKP_M2, BPKP_M3, BPKP_M4);  ///< Brake pressure stepper initializaiton

//Declare variables for custom non-DCS Bios logic
byte brakeState = 1;  ///< brakeState = 1:Park, 2:Emergency, or 0:Off

int brakePressureValue = 0;  ///< Used to hold the value from DCS as read during stepping logic.
int brakePressurePos = 0;    ///< variable to hold the brake pressure stepper position.

// Connect switches to DCS-BIOS
DcsBios::Switch3Pos fireTestSw("FIRE_TEST_SW", FIRE_SW1, FIRE_SW2);

/// Get DCS Sim state for brake pressure
void onHydIndBrakeChange(unsigned int newValue) {
  int temp = map(newValue, 0, 65535, 0, BRAKE_PRESSURE_MAX);
  if (brakePressureValue != temp) {
    brakePressureValue = temp;
  }
}
DcsBios::IntegerBuffer hydIndBrakeBuffer(0x7506, 0xffff, 0, onHydIndBrakeChange);

void onInstrIntLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      brakePressureLight.fill(0, 0, LED_COUNT);  ///< Off
      brakePressureLight.show();
      break;
    default:
      brakePressureLight.fill(brakePressureLight.Color(map(newValue, 0, 65535, 0, 255), 0, 0), 0, LED_COUNT);  ///< Set light to Green - GRB LED
      brakePressureLight.show();
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
* @note If the brake pressure needle doesn't end up at the zero position, you could remove power/usb connection to the pro-micro and then 
* plug it back in to re-run the initialization again prior to connecting the comport to DCS.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(BRKH_EMERG, INPUT_PULLUP);   ///< set emerg pulled pin mode for direct reads
  pinMode(BRKH_PARK, INPUT_PULLUP);    ///< set park pulled pin mode for direct reads
  pinMode(BRKH_SECURE, INPUT_PULLUP);  ///< set handle pushed in pin mode for direct reads

  brakePressureLight.begin();                    ///< begin brake light
  brakePressureLight.show();                     ///< Clear the LED
  brakePressureLight.setBrightness(BRIGHTNESS);  ///< set the initial brightness

  stepperBrakePressure.setSpeed(10);               ///< set the stepper speed
  stepperBrakePressure.step(240);                  // reset needle to far counter-clockwise position
  stepperBrakePressure.step(BRAKE_PRESSURE_ZERO);  // reset to 0%, bottom of red portion of the brake pressure position
  brakePressurePos = 0;                            ///< update needle position to 0
  brakePressureValue = 0;                          ///< update pressure value to 0
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* ## Brake Handle Logic
*
* By keeping track of the brake handle position via variable brakeState = 1:Park, 2:Emergency, or 0:Off
* Read the Emer and Parking brake lever switch states
*
* Based on brakeState, determine if the handle was moved into or out of the Park, Emer, or Off (pushed in position).
* Then send the corresponding rotate and pull DcsBios Messages to move from the prior brake handle position to the new brake handle position.
* 
* To get out of park requires a slight counter-clockwise rotation in the sim.  This is not implemented in the physical controls,
* but is required to be sent in a DcsBios message to push in the brake handle within the sim.  The logic accounts for this 'extra' rotation step.
*
* @note The brake handle rotate does not have any switches so it's impossible to know which way the handle is actually facing when it's pushed in.
*
* ## Brake Pressue Gauge
* Brake pressure gauge is updated based on the value returned from DCS Bios relative to the current position of the needle.
* 
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  // Brake Handle Logic:
  bool eBrkSwitchState = !digitalRead(BRKH_EMERG);  // read the emergency brake switch state
  bool pBrkSwitchState = !digitalRead(BRKH_PARK);   // read the parking brake switch state

  switch (brakeState) {                                           // brakeState = 1:Park, 2:Emergency, or 0:Off
    case 0:                                                       // brakeState == 0, brakes are off determine if pulled and which direction
      if (eBrkSwitchState == 1) {                                 // Emerg brake switch state == 1, Brake handle pulled for emergency brake
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");  // DcsBios message to pull handle
        brakeState = 2;                                           // Update brake state to emergency brakes applied
      }
      if (pBrkSwitchState == 1) {                                   // Parking brake switch state == 1, brake handle pulled for parking brake
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "1");  // Rotate parking brake handle to parking / vertical position
        delay(50);
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "0");  // DcsBios message to pull handle
        brakeState = 1;                                           // Update brake state to parking brakes applied
      }
      break;
    case 1:                                                         // brakeState == 1, Parking brake on
      if (pBrkSwitchState == 0) {                                   // Parking brake switch state == 0, brake handle pushed in
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "2");  // DcsBios message to rotate the brake handle to the 'release' position (not modeled in the physical pit)
        delay(50);                                                  // delay for the animation to complete
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");    // DcsBios message to push the brake handle in
        delay(50);                                                  // delay for the animation to complete
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_ROTATE", "0");  // DcsBios message to rotate the brake handle to the emergency / horizontal position
        brakeState = 0;                                             // Update brake state to off
      }
      break;
    case 2:                                                       // brakeState == 2, Emergency brake on
      if (eBrkSwitchState == 0) {                                 // Emergency brake swtich state == 0, brake handle pushed in
        sendDcsBiosMessage("EMERGENCY_PARKING_BRAKE_PULL", "1");  // DcsBios message to push the brake handle in
        brakeState = 0;                                           // Update brake state to off
      }
      break;
  }

  // Brake Pressue Gauge Logic:
  if (abs(brakePressureValue - brakePressurePos) > 2) {  // difference is greater than 2 steps.
    if ((brakePressureValue - brakePressurePos) > 0) {   // If value is greater than current position rotate needle clockwise
      stepperBrakePressure.step(-1);                     // Step one value clockwise
      brakePressurePos++;                                // update brake pressure position
    }
    if ((brakePressureValue - brakePressurePos) < 0) {  // If value is less than current position rotate needle counter-clockwise
      stepperBrakePressure.step(1);                     // Step one value counter-clockwise
      brakePressurePos--;                               // update brake pressure position
    }
  }
}
