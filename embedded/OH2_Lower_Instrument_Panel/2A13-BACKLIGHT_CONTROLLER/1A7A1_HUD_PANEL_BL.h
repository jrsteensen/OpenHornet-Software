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
 * @file 1A7A1_HUD_PANEL_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the HUD panel backlighting.
*/

#ifndef __HUD_PANEL_BL_H
#define __HUD_PANEL_BL_H

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

#define HUD_REJECT_START 0 + HUD_PANEL_START_INDEX    ///< Index 1st HUD REJECT pixel
#define HUD_REJECT_COUNT 6                            ///< HUD REJECT pixel count
#define HUD_BRT_START 6 + HUD_PANEL_START_INDEX       ///< Index 1st HUD BRT pixel
#define HUD_BRT_COUNT 4                               ///< HUD BRT pixel count
#define HUD_MODE_START 10 + HUD_PANEL_START_INDEX     ///< Index 1st HUD MODE pixel
#define HUD_MODE_COUNT 5                              ///< HUD MODE pixel count
#define BLK_LVL_START 15 + HUD_PANEL_START_INDEX      ///< Index 1st BLK LVL pixel
#define BLK_LVL_COUNT 6                               ///< BLK LVL pixel count
#define WB_START 21 + HUD_PANEL_START_INDEX           ///< Index 1st WB pixel
#define WB_COUNT 5                                    ///< WB pixel count
#define BAL_START 26 + HUD_PANEL_START_INDEX          ///< Index 1st BAL pixel
#define BAL_COUNT 5                                   ///< BAL pixel count.
#define ATT_START 31 + HUD_PANEL_START_INDEX          ///< Index 1st ATT pixel
#define ATT_COUNT 6                                   ///< ATT pixel count.
#define ALT_START 37 + HUD_PANEL_START_INDEX          ///< Index 1st ALT pixel
#define ALT_COUNT 7                                   ///< ALT pixel count.
#define AOA_START 44 + HUD_PANEL_START_INDEX          ///< Index 1st AOA pixel
#define AOA_COUNT 5                                   ///< AOA pixel count
#define HUD_PANEL_BL_START 0 + HUD_PANEL_START_INDEX  ///< Index of 1st pixel in the backlighting string
#define HUD_PANEL_BL_LENGTH 50                        ///< Number of pixels in the backlighting string

#endif