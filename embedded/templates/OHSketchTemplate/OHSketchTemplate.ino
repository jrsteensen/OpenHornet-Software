/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *
 *
 *   Project OpenHornet
 *
 *   The OpenHornet Project is a F/A-18C OFP 13C Lot 20 1:1 Replica Simulator,
 *   consisting of a physical structure and electrical/software interfaces to a PC
 *   to be driven by Digital Combat Simulator (DCS).
 *
 *   ---------------------------------------------------------------------------------
 *
 *   Copyright 2016-2023 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *   ---------------------------------------------------------------------------------
 *
 *   The OpenHornet Software is based on the FP Fork of DCS-BIOS
 *   You can find more information here: https://github.com/DCSFlightpanels/dcs-bios
 *
 *   DCS-BIOS is released under the following terms:
 *   https://github.com/DCSFlightpanels/dcs-bios/blob/master/DCS-BIOS-License.txt
 *
 *   ---------------------------------------------------------------------------------
 *
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxigen capable comments.
 *
 **************************************************************************************/

/**
 * @file OHSketchTemplate.ino
 * @author Balz Reber
 * @date 29.12.2019
 * @version u.0.0.1 (untested)
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware
 * @brief This is the OpenHornet Sketch Template
 *
 * @details This is the Open Hornet Sketch Template. It should be used as a starting point for every new sketch.
 * Please copy the whole OHSketchTemplate folder to start. As it also contains some test skip files needed for travis.
 * 
 *  * **Intended Board:**
 * ABSIS XYZ
 * 
 * **Wiring diagram:**
 * 
 * PIN | Function
 * --- | ---
 * 1   | function 1
 * 2   | function 2
 * 3   | function 3
 * 
 */

/**
 * Set DCS Bios to use irq serial
 */
#define DCSBIOS_IRQ_SERIAL

/**
 * DCS Bios library include
 */
#include "DcsBios.h"


/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}

/**
* A brief description on a single line, ended by a period or blank line.
* 
* A longer comment, which may stretch over several lines and may include other things like:
* Lists:
* - list points
* + nested list points
* - more list points
* 
* # Headers Level 1
* ## Headers Level 2
* ### Headers Level 3
* 
* > Block quotes
* 
* **Emphasis**
* _Emphasis_
* 
* `code()`
* 
* even Tables are supported:
* First Header  | Second Header
* ------------- | -------------
* Content Cell  | Content Cell 
* Content Cell  | Content Cell 
* 
* Links:
* [More about markdown support](http://www.doxygen.nl/manual/markdown.html)
* 
* @param myParam1 Description of 1st parameter.
* @param myParam2 Description of 2nd parameter.
* @returns Description of returned value.
*/
int sampleFunction(int myParam1, int myParam2) {
  int myReturn;
  return myReturn;
}