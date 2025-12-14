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

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "OpenhornetMT6835.h"  
#include "Joystick_ESP32S2.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define OUTBD_CSX 12         ///< J2 OUTBD_CSx
#define INBD_CSY 5           ///< J3 INBD_CSy

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

SPISettings MT6835SPI(1000000, MT6835_BITORDER, SPI_MODE3);
OpenhornetMT6835 outBoardThrottle = OpenhornetMT6835(OUTBD_CSX, MT6835SPI);
OpenhornetMT6835 inBoardThrottle = OpenhornetMT6835(INBD_CSY, MT6835SPI);

/**
*  Rewrite of the Arduino map function to set inputs and values used to be unsigned long long
*  The standard map function was overflowing back to negative values when the mapped values reached 2800.
*
*/
long mapHallSensor(unsigned long long x, unsigned long long in_min, unsigned long long in_max, unsigned long long out_min, unsigned long long out_max) {
const unsigned long long run = in_max - in_min;
  if(run == 0){
    log_e("map(): Invalid input range, min == max");
    return -1; // AVR returns -1, SAM returns 0
  }
  const unsigned long long  rise = out_max - out_min;
  const unsigned long long delta = x - in_min;
return (delta * rise) / run + out_min;
}

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*  
*
*/
void setup() {

  outBoardThrottle.init();
  outBoardThrottle.setRotationDirection(1); 
  inBoardThrottle.init();
  inBoardThrottle.setRotationDirection(-1);  // reverse hall sensor rotation so both throttles read as increasing when moving forward

  //throttle needs to be all the way back to ground idle when plugged into the computer to set the zero value.
  outBoardThrottle.setZeroFromCurrentPosition();
  inBoardThrottle.setZeroFromCurrentPosition();

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
*/
void loop() {

  outBoardThrottle.update();                          // update the outboard hall sensor to prep for read.
  Joystick.setRxAxis(mapHallSensor(outBoardThrottle.readRawAngle21(), 0, 750000, 0, 65535));  //0 and 750000 came from reading the Serial Monitor for the min/max values to then plug into this line.

  inBoardThrottle.update();                           // update the inboard hall sensor to prep for read
  Joystick.setRyAxis(mapHallSensor(inBoardThrottle.readRawAngle21(), 0, 750000, 0, 65535));   //0 and 740000 came from reading the Serial Monitor for the min/max values to then plug into this line.

}