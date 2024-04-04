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
 * @file 4A9A1-THROTTLE_CONTROLLER.ino
 * @author Arribe
 * @date 04.01.2024
 * @version u.0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. The throttle solenoids weren't working. (Remove this line once tested on hardware and in system.)
 * @brief Controls the THROTTLE QUADRANT.
 *
 * @details
 * 
 * @attention Uses Simple FOC libraries with license avialable here: https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
 * @todo Review Simple FOC library MIT copyright for compatibility.  Update copy right notice above as needed for the usage as a git-submodule.
 *
 * @todo Extract just the code for MT6935 hall sensors from the Simple FOC library.
 *
 *  * **Reference Designator:** 4A9A1
 *  * **Intended Board:** CONTROLLER_Throttle
 *  * **RS485 Bus Address:** NA
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 12  | J2 OUTBD_CSx
 * 5   | J3 INBD_CSy
 * 40  | INBD_MAX_LIMIT
 * 38  | INBD_IDLE_LIMIT
 * 36  | OUTBD_MAX_LIMIT
 * 34  | OUTBD_IDLE_LIMIT
 * 21  | EXT_LTS
 * 17  | RAID_FLIR
 * 15  | ATC_ENGAGE
 * 39  | MAX_SOL_SIG
 * 37  | IDLE_SOL_SIG
 * 
*
* ## Hall Sensor Zero via #define SET_THROTTLE_ZERO
* It may be helpful to zero the hall sensors when both throttles are at ground idle position.
* If the Hall sensors are not zeroed out the reads may increase when advancing the throttles forward, and then loop around to a 
* lower value which is confusing for the Window's game controller and its calibration utility.
* 
* ## Disabling the max limit switches via #define DISABLE_MAX_LIMIT_SWITCHES 1
* The max limit switches make running the window's game controller calibarion challenging, and prevents moving the throttles to max
* without pushing a 'button' on the controller advancing the calibarion to the next screen.
*
* ## Reverse External Light Switch Movement
* Reversing the external lights switch may be needed by some (like me) who inadvertently put the switch / wires on backwards.
* Gave an option in code to reverse instead of pulling the outer grip apart to fix physically.
*
*/

#define DISABLE_MAX_SOL_SIG         ///< If defined the solenoid will be disabled in code, comment out line if the solenoid is used
#define DISABLE_IDLE_SOL_SIG        ///< If defined the solenoid will be disabled in code, comment out line if the solenoid is used
#define SET_THROTTLE_ZERO           ///< Define to determine if code should allow the hall sensors to set its zero position based on initial position
//#define DISABLE_MAX_LIMIT_SWITCHES  ///< disables the max limit switches, comment out line if the max limit switches should be used

#define DCSBIOS_DISABLE_SERVO  ///< So the code will compile with an ESP32
#define DCSBIOS_DEFAULT_SERIAL

#include <Wire.h>
#include <SPI.h>
#include "SimpleFOC.h"                             // https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
#include "SimpleFOCDrivers.h"                      // https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
#include "encoders/mt6835/MagneticSensorMT6835.h"  // https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
#include "DcsBios.h"
#include "Joystick_ESP32S2.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define OUTBD_CSX 12         ///< J2 OUTBD_CSx
#define INBD_CSY 5           ///< J3 INBD_CSy
#define INBD_MAX_LIMIT 40    ///< INBD_MAX_LIMIT
#define INBD_IDLE_LIMIT 38   ///< INBD_IDLE_LIMIT
#define OUTBD_MAX_LIMIT 36   ///< OUTBD_MAX_LIMIT
#define OUTBD_IDLE_LIMIT 34  ///< OUTBD_IDLE_LIMIT
#define EXT_LTS 21           ///< EXT_LTS
#define RAID_FLIR 17         ///< RAID_FLIR
#define ATC_ENGAGE 15        ///< ATC_ENGAGE
#define MAX_SOL_SIG 39       ///< MAX_SOL_SIG
#define IDLE_SOL_SIG 37      ///< IDLE_SOL_SIG

/** 
* Inner grip control with index 
* CAGE_UNCAGE, SPEEDBREAK_RETRACT, SPEEDBREAK_EXTEND, COUNTERMEASURES_AFT, COUNTERMEASURES_FWD, D, C, B, A, PUSH, JOY_SW, JOY_X, JOY_Y, ANTENNA_ELEVATION
*
*/
#define CAGE_UNCAGE 0
#define SPEEDBREAK_RETRACT 1
#define SPEEDBREAK_EXTEND 2
#define COUNTERMEASURES_AFT 3
#define COUNTERMEASURES_FWD 4
#define D 5
#define C 6
#define B 7
#define A 8
#define PUSH 9
#define JOY_SW 10
#define JOY_X 11
#define JOY_Y 12
#define ANTENNA_ELEVATION 13


//Declare variables for custom non-DCS Bios logic
bool wowLeft = false;            ///< Weight-on-wheels for solenoid logic.
bool wowRight = false;           ///< Weight-on-wheels for solenoid logic.
bool wowNose = false;            ///< Weight-on-wheels for solenoid logic.
bool launchBarExtended = false;  ///< launch bar used for Max solenoid logic.
bool arrestingHookUp = true;     ///< hook up used for Max solenoid logic.


Joystick_ Joystick = Joystick_(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  18,
  0,
  true,
  true,
  true,
  true,
  true,
  false,
  false,
  false,
  false,
  false,
  false);

const bool testAutoSendMode = false;

SPISettings myMT6835SPISettings(1000000, MT6835_BITORDER, SPI_MODE3);
MagneticSensorMT6835 outboardThrottle = MagneticSensorMT6835(OUTBD_CSX, myMT6835SPISettings);
MagneticSensorMT6835 inboardThrottle = MagneticSensorMT6835(INBD_CSY, myMT6835SPISettings);

// DCSBios reads to save airplane state information. <update comment as needed>
void onExtWowLeftChange(unsigned int newValue) {
  wowLeft = newValue;
}
DcsBios::IntegerBuffer extWowLeftBuffer(0x74d8, 0x0100, 8, onExtWowLeftChange);

void onExtWowRightChange(unsigned int newValue) {
  wowRight = newValue;
}
DcsBios::IntegerBuffer extWowRightBuffer(0x74d6, 0x8000, 15, onExtWowRightChange);

void onExtWowNoseChange(unsigned int newValue) {
  wowNose = newValue;
}
DcsBios::IntegerBuffer extWowNoseBuffer(0x74d6, 0x4000, 14, onExtWowNoseChange);

void onHookLeverChange(unsigned int newValue) {
  arrestingHookUp = newValue;  // 1 is hook up
}
DcsBios::IntegerBuffer hookLeverBuffer(0x74a0, 0x0200, 9, onHookLeverChange);

void onLaunchBarSwChange(unsigned int newValue) {
  launchBarExtended = newValue;  // 1 is extended
}
DcsBios::IntegerBuffer launchBarSwBuffer(0x7480, 0x2000, 13, onLaunchBarSwChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*  
* @note If SET_THROTTLE_ZERO defined the code will read the hall sensors' initial position at power on to set that as zero.
* If used ensure that the throttles are pulled back to the ground idle (not fuel-cut-off/min position).
*
* ### Flight Idle Detent Solenoid
* If weight-on-wheels is true, pull flight idle detent down to allow throttle movement to ground idle position.
* 
* ### Afterburner Lockout Detent Solenoid
* If weight-on-wheels is true, and launch bar is extended or the hook is down then raise the afterburner 
* lockout detent to prevent inadvertent selection of afterburner. Otherwise pull the afterburner lockout detent down.
*
* @note If DISABLE_MAX_SOL_SIG or DISABLE_IDLE_SOL_SIG are not defined the solenoids will be used based on the logic from DCS state.
*
* @todo When flight idle detent is fixed mechanically for more consistent usage, explore using the aircraft's g-load to simiulate high-G
* maneuvers causing the flight idle stop to retract and allow inadvertent selection of ground idle.
*
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  Wire.begin(SDA, SCL);
  Serial.begin(115200);

  outboardThrottle.init();
  inboardThrottle.init();
  inboardThrottle.setRotationDirection(-1);  // reverse hall sensor rotation so both throttles read as increasing when moving forward

#ifdef SET_THROTTLE_ZERO
  //throttle needs to be all the way back to ground idle when plugged into the computer to set the zero value.
  outboardThrottle.setZeroFromCurrentPosition();
  inboardThrottle.setZeroFromCurrentPosition();
#endif

  pinMode(OUTBD_MAX_LIMIT, INPUT_PULLUP);
  pinMode(OUTBD_IDLE_LIMIT, INPUT_PULLUP);
  pinMode(INBD_MAX_LIMIT, INPUT_PULLUP);
  pinMode(INBD_IDLE_LIMIT, INPUT_PULLUP);
  pinMode(EXT_LTS, INPUT_PULLUP);
  pinMode(RAID_FLIR, INPUT_PULLUP);
  pinMode(ATC_ENGAGE, INPUT_PULLUP);

  pinMode(IDLE_SOL_SIG, OUTPUT);
  pinMode(MAX_SOL_SIG, OUTPUT);

  digitalWrite(IDLE_SOL_SIG, LOW);
  digitalWrite(MAX_SOL_SIG, LOW);

  Joystick.setXAxisRange(0, 1024);    // TDC X-axis
  Joystick.setYAxisRange(0, 1024);    // TDC Y-axis
  Joystick.setZAxisRange(0, 1024);    // Radar Elevation
  Joystick.setRxAxisRange(0, 65535);  // Outboard Throttle Arm
  Joystick.setRyAxisRange(0, 65535);  // Inboard Throttle Arm
  Joystick.begin();
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*
* @attention To setup the throttle reads, uncomment the serial print lines in the loop section.  
* Then open a serial monitor to observe the min and max values for the outboard and inboard throttles.
* Set the 3rd parametr (max input) of the map function to be at least as large if not a little larger than the observed max values.
* If the max value isn't set high enough the values can loop back around to 0 prior to throttle advancing all the way forward.
* Windows game controller calibration may be used to adjust the upper and lower bounds as long as the mapped values don't loop back to 0.
*
* @todo If it's determined that users won't have to tweak the mapped value maxes, 
* remove the temp reads for the hall sensors and combine the outboard and inboard throttle lines like:
* 
*  `temp = outboardThrottle.readRawAngle21();           // read outboard hall sensor` \n
*  `Joystick.setRxAxis(mapHallSensor(temp, 0, 750000, 0, 65535));`
*  
* Into one line:
*  `Joystick.setRxAxis(mapHallSensor(outboardThrottle.readRawAngle21(), 0, 750000, 0, 65535));`
*
* @note Inner grip values sent to throttle controller in this order: 
* CAGE_UNCAGE, SPEEDBREAK_RETRACT, SPEEDBREAK_EXTEND, COUNTERMEASURES_AFT, COUNTERMEASURES_FWD, D, C, B, A, PUSH, JOY_SW,
* JOY_X, JOY_Y, ANTENNA_ELEVATION
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  uint32_t temp;  // temp value to hold the analog reads in preparation of doing logic.

  outboardThrottle.update();                                     // update the outboard hall sensor to prep for read.
  temp = outboardThrottle.readRawAngle21();                      // read outboard hall sensor
  Joystick.setRxAxis(mapHallSensor(temp, 0, 750000, 0, 65535));  //0 and 750000 came from reading the Serial Monitor for the min/max values to then plug into this line.
  // Uncomment the code below if you wish to pass the outboard throttle's raw values to the serial monitor
  //Serial.print("outbThrottle: ");
  //Serial.print(temp);

  inboardThrottle.update();                                      // update the inboard hall sensor to prep for read
  temp = inboardThrottle.readRawAngle21();                       // read inboard hall sensor
  Joystick.setRyAxis(mapHallSensor(temp, 0, 750000, 0, 65535));  //0 and 740000 came from reading the Serial Monitor for the min/max values to then plug into this line.
// Uncomment the code below if you wish to pass the inboard throttle's raw values to the serial monitor
//Serial.print("  inbThrottle: ");
//Serial.print(temp);
//Serial.print("\n");

// determine if max limit switches are included or not
#ifndef DISABLE_MAX_LIMIT_SWITCHES
  Joystick.setButton(0, !digitalRead(OUTBD_MAX_LIMIT));
  Joystick.setButton(1, !digitalRead(INBD_MAX_LIMIT));
#endif

  Joystick.setButton(2, !digitalRead(OUTBD_IDLE_LIMIT));
  Joystick.setButton(3, !digitalRead(INBD_IDLE_LIMIT));
  Joystick.setButton(4, !digitalRead(EXT_LTS));
  Joystick.setButton(5, !digitalRead(RAID_FLIR));
  Joystick.setButton(6, !digitalRead(ATC_ENGAGE));

  int index = 0;  // Initialize the index to keep track of which inner grip button we're reading
  byte tempLow;   //variable for holding low byte of inner grip's analog reads
  byte tempHi;    //variable for holding high byte of inner grip's analog reads

  Wire.requestFrom(49, 17);  // request inner grip pro-mini return current button state info

  while (Wire.available()) {  // While something to read from the inner grip
    switch (index) {
      case CAGE_UNCAGE:
        Joystick.setButton(7, Wire.read());
        break;
      case SPEEDBREAK_RETRACT:
        Joystick.setButton(8, Wire.read());
        break;
      case SPEEDBREAK_EXTEND:
        Joystick.setButton(9, Wire.read());
        break;
      case COUNTERMEASURES_AFT:
        Joystick.setButton(10, Wire.read());
        break;
      case COUNTERMEASURES_FWD:
        Joystick.setButton(11, Wire.read());
        break;
      case D:
        Joystick.setButton(12, Wire.read());
        break;
      case C:
        Joystick.setButton(13, Wire.read());
        break;
      case B:
        Joystick.setButton(14, Wire.read());
        break;
      case A:
        Joystick.setButton(15, Wire.read());
        break;
      case PUSH:
        Joystick.setButton(16, Wire.read());
        break;
      case JOY_SW:
        Joystick.setButton(17, Wire.read());
        break;
      case JOY_X:
        tempLow = Wire.read();      // read low byte
        tempHi = Wire.read();       // read high byte
        temp = (tempHi << 8) + tempLow;  //rebuild the TDC X-axis value
        Joystick.setXAxis(map(temp, 0, 65000, 0, 1024));
        break;
      case JOY_Y:
        tempLow = Wire.read();      // read low byte
        tempHi = Wire.read();       // read high byte
        temp = (tempHi << 8) + tempLow;  //rebuild the TDC Y-axis value
        Joystick.setYAxis(map(temp, 0, 65000, 0, 1024));
        break;
      case ANTENNA_ELEVATION:
        tempLow = Wire.read();                       // read low byte
        tempHi = Wire.read();                        // read high byte
        temp = (tempHi << 8) + tempLow;                   //rebuild the Antenna axis value
        Joystick.setZAxis(map(temp, 754, 978, 0, 1024));  // mapped values determined by reading the retured elevation on the serial monitor
        // enable the lines below to see the Elevation knob's analog read values.
        //Serial.print(" Elevation: ");
        //Serial.print((tempHi << 8) + tempLow);
        //Serial.print("\n");
        break;
    }
    index++;  // increment the index for the next iteration through the while loop
  }

#ifndef DISABLE_IDLE_SOL_SIG
  if (wowLeft == wowRight == wowNose == true) {
    digitalWrite(IDLE_SOL_SIG, HIGH);
  } else {
    digitalWrite(IDLE_SOL_SIG, LOW);
  }
#endif

#ifndef DISABLE_MAX_SOL_SIG
  if ((wowLeft == wowRight == wowNose == true) && ((launchBarExtended == true) || (arrestingHookUp == false))) {
    digitalWrite(MAX_SOL_SIG, LOW);  // With weight-on-wheels, if the lanuch bar is extended or the hook is down then raise afterburner lockout / detent to prevent inadvertent afterburner selection.
  } else {
    digitalWrite(MAX_SOL_SIG, HIGH);  // Otherwise pull the afterburner lockout / detent.
  }
#endif
}

/**
*  Rewrite of the Arduino map function to set inputs and values used to be unsigned long long
*  The standard map function was overflowing back to negative values when the mapped values reached 2800.
*
*/

long mapHallSensor(unsigned long long x, unsigned long long in_min, unsigned long long in_max, unsigned long long out_min, unsigned long long out_max) {
  const unsigned long long run = in_max - in_min;
  if (run == 0) {
    log_e("map(): Invalid input range, min == max");
    return -1;  // AVR returns -1, SAM returns 0
  }
  const unsigned long long rise = out_max - out_min;
  const unsigned long long delta = x - in_min;
  return (delta * rise) / run + out_min;
}
