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
 * @brief Header file for the DCSBios backlighting to split out code for panel specific lighting support.
*/

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

/// @todo remove ifdef for HUD_PANEL_REV3 as it's for my ease of testing
#define HUD_PANEL_REV3

/// @todo remove ifdef for SPN_RCVY_REV3 as it's for my ease of testing
#define SPN_RCVY_REV3

/**
* @brief UIP Backlights
*
* The Panels are listed in the order they appear according to the interconnect guide.
* If any panels are not connected, change the associtated LED COUNT to 0, comment out its header, 
* and adjust the other panels' start index accordingly.
*
*/
#define MASTER_ARM_LED_START_INDEX 0 ///< Master Arm panel is the first in the UIP BL Channel 1 string.
#define MA_LED_COUNT 29 ///< Master Arm panel has 29 pixels with backlighting.

#define L_EWI_LED_START_INDEX 29 ///< L_EWI is the second panel in the UIP BL Channel 1 string.
#define L_EWI_LED_COUNT 30 ///< L EWI panel has 30 pixels, no backlighting.

#define HUD_PANEL_START_INDEX 59 ///< HUD_PANEL is the 3rd panel in the UIP BL Channel 1 string.

/// @todo remove ifdef for HUD_PANEL_REV3 as it's for my ease of testing
#ifdef HUD_PANEL_REV3 
#define HUD_PANEL_LED_COUNT 56 ///< HUD_PANEL rev4 has 50 pixels, all for backlighting. If using an older revision may be 56.
#else
#define HUD_PANEL_LED_COUNT 50 ///< HUD_PANEL rev4 has 50 pixels, all for backlighting. If using an older revision may be 56.
#endif

/// @todo remove ifdef for HUD_PANEL_REV3 as it's for my ease of testing
#ifdef HUD_PANEL_REV3 
#define R_EWI_LED_START_INDEX 115 ///< R_EWI is the 4th panel in the UIP BL Channel 1 string.
#else
#define R_EWI_LED_START_INDEX 109 ///< R_EWI is the 4th panel in the UIP BL Channel 1 string.
#endif

#define R_EWI_LED_COUNT 30 ///< R EWI panel has 30 pixels, no backlighting.

/// @todo remove ifdef for HUD_PANEL_REV3 as it's for my ease of testing
#ifdef HUD_PANEL_REV3 
#define SPN_RCVY_LED_START_INDEX 145  ///< SPN RCVY is the 5th and final panel in the UIP BL Channel 1 string.
#else

#define SPN_RCVY_LED_START_INDEX 139  ///< SPN RCVY is the 5th and final panel in the UIP BL Channel 1 string.
#endif
/// @todo remove ifdef for SPN_RCVY_REV3 as it's for my ease of testing
#ifdef SPN_RCVY_REV3
#define SPN_RCVY_LED_COUNT 63 ///< SPN RCVY rev3 panel had 63 pixels with backlighting.
#else
#define SPN_RCVY_LED_COUNT 52 ///< SPN RCVY panel has 52 pixels with backlighting.
#endif

#define UIP_LED_COUNT MA_LED_COUNT + L_EWI_LED_COUNT + HUD_PANEL_LED_COUNT + R_EWI_LED_COUNT + SPN_RCVY_LED_COUNT ///< add up the connected panels pixel count for the Adafruit_NeoPixel string.

Adafruit_NeoPixel uipBLCh1 = Adafruit_NeoPixel(UIP_LED_COUNT, UIP_BL_CH1, NEO_GRB + NEO_KHZ800); ///< Setup the UIP BL Ch1 string.

// Include the connected panels.  If any panel not connected comment out its header, and change its LED count above along with updating downstream panel(s)' start index.
#include "1A2A1_MASTER_ARM_BL.h" 
#include "1A4_L_EWI_BL.h"
#include "1A7A1_HUD_PANEL_BL.h"
#include "1A5_R_EWI_BL.h"
#include "1A6A1_SPN_RCVY_BL.h"

/**
* @brief LIP Backlights
* @todo all the backlight code / headers.
*/

#define LIP_LED_COUNT 10 ///< place holder, replace with actual count method similar to above.

Adafruit_NeoPixel lipBLCh1 = Adafruit_NeoPixel(LIP_LED_COUNT, LIP_BL_CH1, NEO_GRB + NEO_KHZ800); ///< Setup the LIP BL Ch1 string.


namespace OpenHornet { 
/// @todo If we don't want the namespace rename the setup function and use it in 2A13-BACKLIGHT_CONTROLLER.ino's setup loop, so there is only one setup() per Arduino skectch.
void setup() { 
  uipBLCh1.begin();
  uipBLCh1.show(); // Clear the LEDs
  uipBLCh1.setBrightness(BRIGHTNESS);

  lipBLCh1.begin();
  lipBLCh1.show(); // Clear the LEDs
  lipBLCh1.setBrightness(BRIGHTNESS);
}
}

/**
* This is the DcsBios hook into the Internal Light panel's Instrument Panel Dimmer pot.
* The use of headers and ifdefs should help avoid users from having to manually update the code for any unconnected panels if the associated headers are commented out above.
*
* @todo Add all the backlighting channels and associated panels that run off the onInstPnlDimmerChange pot to light up all the connected panels' backlighting pixels.
*/
void onInstPnlDimmerChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      #ifdef __MASTER_ARM_BL_H
      uipBLCh1.fill(0, MA_BL_START, MA_BL_LENGTH);  ///< Off
      #endif
      #ifdef __HUD_PANEL_BL_H
      uipBLCh1.fill(0, HUD_PANEL_BL_START, HUD_PANEL_BL_LENGTH);  ///< Off
      #endif
      uipBLCh1.show();
      break;
    default:
      #ifdef __MASTER_ARM_BL_H
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), MA_BL_START, MA_BL_LENGTH);  ///< Set light to Green
      #endif
      #ifdef __HUD_PANEL_BL_H
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), HUD_PANEL_BL_START, HUD_PANEL_BL_LENGTH);  ///< Set light to Green
      #endif
      #ifdef __SPN_RCVY_BL_H
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), SPN_RCVY_BL_START, SPN_RCVY_LENGTH);  ///< Set light to Green
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), SPN_RCVY2_BL_START, SPN_RCVY2_LENGTH);  ///< Set light to Green
      uipBLCh1.fill(uipBLCh1.Color(0, map(newValue, 0, 65535, 0, 255), 0), SPN_RCVY3_BL_START, SPN_RCVY3_LENGTH);  ///< Set light to Green
      #endif
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer instPnlDimmerBuffer(0x7546, 0xffff, 0, onInstPnlDimmerChange);
