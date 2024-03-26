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
 * @file 2A13-BACKLIGHT_CONTROLLER.H
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the backlighting to split out code for panel specific lighting support.
*/

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

/**
* @brief UIP Backlights
* @todo Add if defined statements here to test if the panels are included prior to including them in the main LED counts.
*/
#define MASTER_ARM_LED_START_INDEX 0
#define MA_LED_COUNT 29

#define L_EWI_LED_START_INDEX 29
#define L_EWI_LED_COUNT 30

#define UIP_LED_COUNT MA_LED_COUNT + L_EWI_LED_COUNT

Adafruit_NeoPixel uipBLCh1 = Adafruit_NeoPixel(UIP_LED_COUNT, UIP_BL_CH1, NEO_GRB + NEO_KHZ800);

#include ".\OH_BL_PANELS\1A2A1_MASTER_ARM_BL.h"
#include ".\OH_BL_PANELS\1A4_L_EWI_BL.h"

namespace OpenHornet {
void setup() {
  uipBLCh1.begin();
  uipBLCh1.show();
  uipBLCh1.setBrightness(BRIGHTNESS);
}
}
