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
 * @file 2A13_BACKLIGHT_CONTROLLER.h
 * @author ViajanDee
 * @date 05.21.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Header file for the Sim Pit panel backlighting.
 */

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

/**
* @brief LC Backlights
*
* The Panels are listed in the order they appear according to the interconnect guide.
* If any panels are not connected, change the associtated LED COUNT to 0, comment out its header, 
* and adjust the other panels' start index accordingly.
*
*/
#define LANDING_GEAR_LED_START_INDEX 0          ///< Landing Gear panel is the first in the LC BL Channel 1 string.
#define LG_LED_COUNT 23

#define SELECT_JETT_LED_START_INDEX 23          ///< Select Jett panel is the second in the LC BL Channel 1 string.
#define SJ_LED_COUNT 81

#define FIRE_TEST_LED_START_INDEX 105           ///< Fire Test panel is the third in the LC BL Channel 1 string.
#define FT_LED_COUNT 10

#define SIM_POWER_LED_START_INDEX 115           ///< Sim Power panel is the fourth in the LC BL Channel 1 string.
#define SP_LED_COUNT 35

#define GEN_TIE_LED_START_INDEX 150             ///< Gen Tie panel is the fifth in the LC BL Channel 1 string.
#define GT_LED_COUNT 9

#define EXTERNAL_LT_LED_START_INDEX 159         ///< External Lights panel is the sixth in the LC BL Channel 1 string.
#define EL_LED_COUNT 44

#define FUEL_LED_START_INDEX 203                ///< Fuel panel is the seventh in the LC BL Channel 1 string.
#define FP_LED_COUNT 32



#define LC_BL_CH1 9                             ///< Define the pin for Channel 1

#define LC_START_INDEX 0                        ///< Define the start of Channel 1
#define LC_LED_COUNT LG_LED_COUNT + SJ_LED_COUNT + FT_LED_COUNT + SP_LED_COUNT + GT_LED_COUNT + EL_LED_COUNT + FP_LED_COUNT ///< The total number of LEDs in Channel 1

Adafruit_NeoPixel lcBLCh1 = Adafruit_NeoPixel(LC_LED_COUNT, LC_BL_CH1, NEO_GRB + NEO_KHZ800);    ///< Setup the LC BL Channel 1 string.

/**
* @brief LC Variables
* The variables are defined here
*/
unsigned int consoleIntLt = 0;              ///< Track the internal lights status

namespace OpenHornet {
  void setup() {
    lcBLCh1.begin();
    lcBLCh1.fill(lcBLCh1.Color(0, 0, 0), LC_START_INDEX, LC_LED_COUNT);                         ///< Initialize LEDs Off
    lcBLCh1.show();
  }
}

/**
* @brief Backlight Brightness
* This function controls the brightness of the panels backlight
*/
void setBacklightBrightness(uint8_t brtns) {
  uint32_t consoleIntLt = lcBLCh1.Color(0, brtns, 0);
  for (int i = 0; i < LC_LED_COUNT; i++) {
    lcBLCh1.setPixelColor(i, consoleIntLt);
  }
  lcBLCh1.show();
}

/**
* @brief DCS Bios Backlight Hook
* This callback function monitors DCS Bios for the backlight value
*/
void onConsoleIntLtChange(unsigned int newValue) {
  uint8_t brtns = map(newValue, 0, 65535, 0, 255); // Map the DCS-BIOS value to 0-255 range
  setBacklightBrightness(brtns);
}

/**
* @brief Initialize DCS Bios buffers
*/
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);
DcsBios::RotaryEncoder consolesDimmer("CONSOLES_DIMMER", "-4047", "+4047", SIMPWR_BLM_A, SIMPWR_BLM_B);