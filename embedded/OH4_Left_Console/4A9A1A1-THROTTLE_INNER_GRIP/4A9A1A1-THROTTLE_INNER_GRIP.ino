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
 * @file 4A9A1A1-THROTTLE_INNER_GRIP.ino
 * @author Arribe
 * @date 04.01.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the THROTTLE INNER GRIP
 *
 * @details
 * 
 *  * **Reference Designator:** 4A9A1A1
 *  * **Intended Board:** THROTTLE_INNER_GRIP
 *  * **RS485 Bus Address:** NA
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 2   | CAGE_UNCAGE
 * 3   | SPEEDBREAK_RETRACT
 * 4   | SPEEDBREAK_EXTEND
 * 7   | COUNTERMEASURES_AFT
 * 8   | COUNTERMEASURES_FWD
 * 9   | PUSH - coms switch
 * A3  | JOY_X
 * A2  | JOY_Y
 * A1  | JOY_SW
 * A0  | ANTENNA_ELEVATION
 * 13  | D - coms switch
 * 12  | C - coms switch
 * 11  | B - coms switch
 * 10  | A - coms switch
 * SCL | I2C_SCK
 * SDA | I2C_DATA
 * 
 */

#include <Wire.h>

// Define pins inner grip pro-mini
#define CAGE_UNCAGE 2          ///< CAGE_UNCAGE
#define SPEEDBREAK_RETRACT 3   ///< SPEEDBREAK_RETRACT
#define SPEEDBREAK_EXTEND 4    ///< SPEEDBREAK_EXTEND
#define COUNTERMEASURES_AFT 7  ///< COUNTERMEASURES_AFT
#define COUNTERMEASURES_FWD 8  ///< COUNTERMEASURES_FWD
#define PUSH 9                 ///< PUSH - coms switch
#define JOY_X A3               ///< JOY_X
#define JOY_Y A2               ///< JOY_Y
#define JOY_SW A1              ///< JOY_SW
#define ANTENNA_ELEVATION A0   ///< ANTENNA_ELEVATION
#define D 13                   ///< D - coms switch
#define C 12                   ///< C - coms switch
#define B 11                   ///< B - coms switch
#define A 10                   ///< A - coms switch

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*
* Joins I2C bus with address #49, registering requestEvent()
*
* @todo Add error handling as needed.
*
*/
void setup() {
  Wire.begin(49);                // join i2c bus with address #49
  Wire.onRequest(requestEvent);  // register requestEvent


  pinMode(CAGE_UNCAGE, INPUT_PULLUP);
  pinMode(SPEEDBREAK_RETRACT, INPUT_PULLUP);
  pinMode(SPEEDBREAK_EXTEND, INPUT_PULLUP);
  pinMode(COUNTERMEASURES_AFT, INPUT_PULLUP);
  pinMode(COUNTERMEASURES_FWD, INPUT_PULLUP);
  pinMode(ANTENNA_ELEVATION, INPUT_PULLUP);
  pinMode(D, INPUT_PULLUP);
  pinMode(C, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(A, INPUT_PULLUP);
  pinMode(PUSH, INPUT_PULLUP);
  pinMode(JOY_SW, INPUT_PULLUP);
  pinMode(JOY_X, INPUT_PULLUP);
  pinMode(JOY_Y, INPUT_PULLUP);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
* 
* @note The loop is empty
*/
void loop() {
}

/**
* Function that returns the sate of the inboard throttle grip controls when prompted by the throttle controller.
* Writes each button / analog read back to the throttle controller in the same order each time.
* The Wire library only sends one byte at a time.  The analog read values need to be split into low and high bytes
* so the controller can properly reassemble the 2 byte numeric value.
*
* Passes inner grip reads to throttle controller in this order: 
* CAGE_UNCAGE, SPEEDBREAK_RETRACT, SPEEDBREAK_EXTEND, COUNTERMEASURES_AFT, COUNTERMEASURES_FWD, D, C, B, A, PUSH, JOY_SW,
* JOY_X, JOY_Y, ANTENNA_ELEVATION
*
*  @note There is minimal logic for the inner grip pro-mini given the extra steps for reprogramming.
*/

void requestEvent() {

  Wire.write(!digitalRead(CAGE_UNCAGE));
  Wire.write(!digitalRead(SPEEDBREAK_RETRACT));
  Wire.write(!digitalRead(SPEEDBREAK_EXTEND));
  Wire.write(!digitalRead(COUNTERMEASURES_AFT));
  Wire.write(!digitalRead(COUNTERMEASURES_FWD));
  Wire.write(!digitalRead(D));
  Wire.write(!digitalRead(C));
  Wire.write(!digitalRead(B));
  Wire.write(!digitalRead(A));
  Wire.write(!digitalRead(PUSH));
  Wire.write(!digitalRead(JOY_SW));

  int temp = analogRead(JOY_X);  // get the analog read of the TDC X azis.
  Wire.write(lowByte(temp));     // write out the low byte to the controller
  Wire.write(highByte(temp));    // write out the high byte to the controller

  temp = analogRead(JOY_Y);    // get the analog read of the TDC Y azis.
  Wire.write(lowByte(temp));   // write out the low byte to the controller
  Wire.write(highByte(temp));  // write out the high byte to the controller

  temp = analogRead(ANTENNA_ELEVATION);  // get the analog read of the Antenna Elevation azis.
  Wire.write(lowByte(temp));             // write out the low byte to the controller
  Wire.write(highByte(temp));            // write out the high byte to the controller
}
