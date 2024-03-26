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
 * @file 1A2A1_MASTER_ARM_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the Master Arm panel backlighting /w annunciators to split out code for panel specific lighting support.
*/

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

#define READY_START 0 + MASTER_ARM_LED_START_INDEX
#define READY_COUNT 2
#define DISCH_START 2 + MASTER_ARM_LED_START_INDEX
#define DISCH_COUNT 2
#define MM_AG_START 25 + MASTER_ARM_LED_START_INDEX
#define MM_AG_COUNT 2
#define MM_AA_START 27 + MASTER_ARM_LED_START_INDEX
#define MM_AA_COUNT 2
#define BL_START 4 + MASTER_ARM_LED_START_INDEX
#define BL_LENGTH 21

/**
* @brief Setup maserArmPanel's neopixel and then set the DCS Bios methods to read sim state and set
* backlighting / annunciators accordingly.
*
*/

void onMcReadyChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, READY_START, READY_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), READY_START, READY_COUNT);  ///< Set light to Yellow
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer mcReadyBuffer(0x740c, 0x8000, 15, onMcReadyChange);

void onMcDischChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, DISCH_START, DISCH_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), DISCH_START, DISCH_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);

void onMasterModeAgLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, MM_AG_START, MM_AG_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), MM_AG_START, MM_AG_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

void onMasterModeAaLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, MM_AA_START, MM_AA_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), MM_AA_START, MM_AA_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

void onInstPnlDimmerChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, MASTER_ARM_LED_START_INDEX + BL_START, BL_LENGTH);  ///< Off
      uipBLCh1.show();
      break;
    default:
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), MASTER_ARM_LED_START_INDEX + BL_START, BL_LENGTH);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer instPnlDimmerBuffer(0x7546, 0xffff, 0, onInstPnlDimmerChange);