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
 * @file 1A6A1_SPN_RCVY_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the Spin Recovery panel backlighting /w annunciators to split out code for panel specific lighting support.
*/

#ifndef __SPN_RCVY_BL_H
#define __SPN_RCVY_BL_H

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

#define SPN_START 29 + SPN_RCVY_LED_START_INDEX           ///< Index 1st SPN pixel
#define SPN_COUNT 1                                       ///< SPN pixel count
/// @todo remove ifdef for SPN_RCVY_REV3 as it's for my ease of testing
#ifdef SPN_RCVY_REV3
#define SPN2_START 36 + SPN_RCVY_LED_START_INDEX          ///< Index 2nd SPN pixel
#else
#define SPN2_START 32 + SPN_RCVY_LED_START_INDEX          ///< Index 2nd SPN pixel
#endif
#define SPN2_COUNT 1                                      ///< SPN2 pixel count

#define SPN_RCVY_BL_START 0 + SPN_RCVY_LED_START_INDEX    ///< Index for 1st block of SPN RCVY panel backlight pixels
#define SPN_RCVY_LENGTH 29                                ///< SPN RCVY panel backlight block 1 pixel count

#define SPN_RCVY2_BL_START 30 + SPN_RCVY_LED_START_INDEX  ///< Index for 2nd block of SPN RCVY panel backlight pixels

/// @todo remove ifdef for SPN_RCVY_REV3 as it's for my ease of testing
#ifdef SPN_RCVY_REV3
#define SPN_RCVY2_LENGTH 5                                ///< 2 SPN RCVY panel backlight block 2 pixel count
#else
#define SPN_RCVY2_LENGTH 2                                ///< 2 SPN RCVY panel backlight block 2 pixel count
#endif

/// @todo remove ifdef for SPN_RCVY_REV3 as it's for my ease of testing
#ifdef SPN_RCVY_REV3
#define SPN_RCVY3_BL_START 37 + SPN_RCVY_LED_START_INDEX  ///< 36 Index for 3nd block of SPN RCVY panel backlight pixels
#define SPN_RCVY3_LENGTH 26                               ///< 19 SPN RCVY panel backlight block 2 pixel count
#else
#define SPN_RCVY3_BL_START 36 + SPN_RCVY_LED_START_INDEX  ///< 36 Index for 3nd block of SPN RCVY panel backlight pixels
#define SPN_RCVY3_LENGTH 19                               ///< 19 SPN RCVY panel backlight block 2 pixel count
#endif

/**
* @brief Setup SPN RCVY panel's neopixel and then set the DCS Bios methods to read sim state and set
* backlighting / annunciators accordingly.
*
*/

void onSpinLtChange(unsigned int newValue) {
    switch (newValue) {
    case 0:
      uipBLCh1.fill(0, SPN_START, SPN_COUNT);  ///< Off
      uipBLCh1.fill(0, SPN2_START, SPN2_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), SPN_START, SPN_COUNT);  ///< Set light to Yellow
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), SPN2_START, SPN2_COUNT);  ///< Set light to Yellow
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer spinLtBuffer(0x742a, 0x0800, 11, onSpinLtChange);

#endif