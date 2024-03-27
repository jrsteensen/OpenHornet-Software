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
 * @file 1A5_R_EWI_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the Right EWI panel annunciators to split out code for panel specific lighting support.
*/

#ifndef __R_EWI_BL_H
#define __R_EWI_BL_H

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

#define R_FIRE_START 0 + R_EWI_LED_START_INDEX      ///< Starting index of the pixel string, FIRE push button switch
#define R_FIRE_COUNT 4                              ///< Number of pixels under the Fire push button.
#define APU_FIRE_START 4 + R_EWI_LED_START_INDEX    ///< Index for pixel for APU Fire pushbutton switch.
#define APU_FIRE_COUNT 4                            ///< Number of pixels under the APU Fire pushbutton.
#define DISP_START 8 + R_EWI_LED_START_INDEX        ///< Index for 1st DISP pixel
#define DISP_COUNT 2                                ///< DISP pixel count
#define RCDRON_ON_START 10 + R_EWI_LED_START_INDEX  ///< Index for 1st RCDRON ON pixel
#define RCDRON_ON_COUNT 2                           ///< RCDRON ON pixel count
#define L_BLANK_START 12 + R_EWI_LED_START_INDEX    ///< Index for 1st BLANK pixel
#define L_BLANK_COUNT 2                             ///< R Bleed pixel count
#define R_BLANK_START 14 + R_EWI_LED_START_INDEX    ///< Index for 1st BLANK pixel
#define R_BLANK_COUNT 2                             ///< BLANK pixel count
#define R_BLANK2_START 16 + R_EWI_LED_START_INDEX   ///< Index for 1st BLANK pixel
#define R_BLANK2_COUNT 2                            ///< BLANK pixel count
#define L_BLANK2_START 18 + R_EWI_LED_START_INDEX   ///< Index for 1st BLANK pixel
#define L_BLANK2_COUNT 2                            ///< BLANK pixel count
#define L_BLANK3_START 20 + R_EWI_LED_START_INDEX   ///< Index for 1st BLANK pixel
#define L_BLANK3_COUNT 2                            ///< BLANK pixel count
#define SAM_START 22 + R_EWI_LED_START_INDEX        ///< Index for 1st SAM pixel
#define SAM_COUNT 2                                 ///< SAM pixel count
#define AAA_START 24 + R_EWI_LED_START_INDEX        ///< Index for 1st AAA pixel
#define AAA_COUNT 2                                 ///< AAA pixel count
#define AI_START 26 + R_EWI_LED_START_INDEX         ///< Index for 1st AI pixel
#define AI_COUNT 2                                  ///< AI pixel count.
#define CW_START 28 + R_EWI_LED_START_INDEX         /// Index for 1st CW pixel
#define CW_COUNT 2                                  ///< CW pixel count.

void onFireRightLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, R_FIRE_START, R_FIRE_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), R_FIRE_START, R_FIRE_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer fireRightLtBuffer(0x740c, 0x0010, 4, onFireRightLtChange);

void onFireApuLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, APU_FIRE_START, APU_FIRE_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), APU_FIRE_START, APU_FIRE_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer fireApuLtBuffer(0x740c, 0x0004, 2, onFireApuLtChange);

void onRhAdvAaaChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, AAA_START, AAA_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), AAA_START, AAA_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvAaaBuffer(0x740a, 0x0800, 11, onRhAdvAaaChange);

void onRhAdvAiChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, AI_START, AI_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), AI_START, AI_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvAiBuffer(0x740a, 0x0400, 10, onRhAdvAiChange);

void onRhAdvCwChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, CW_START, CW_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), CW_START, CW_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvCwBuffer(0x740a, 0x1000, 12, onRhAdvCwChange);

void onRhAdvDispChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, DISP_START, DISP_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), DISP_START, DISP_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvDispBuffer(0x740a, 0x0100, 8, onRhAdvDispChange);

void onRhAdvRcdrOnChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, RCDRON_ON_START, RCDRON_ON_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), RCDRON_ON_START, RCDRON_ON_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvRcdrOnBuffer(0x740a, 0x0080, 7, onRhAdvRcdrOnChange);

void onRhAdvSamChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, SAM_START, SAM_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), SAM_START, SAM_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSamBuffer(0x740a, 0x0200, 9, onRhAdvSamChange);

void onRhAdvSpareRh1Change(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BLANK_START, L_BLANK_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), L_BLANK_START, L_BLANK_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh1Buffer(0x740a, 0x2000, 13, onRhAdvSpareRh1Change);

void onRhAdvSpareRh2Change(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, R_BLANK_START, R_BLANK_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), R_BLANK_START, R_BLANK_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh2Buffer(0x740a, 0x4000, 14, onRhAdvSpareRh2Change);

void onRhAdvSpareRh3Change(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, R_BLANK2_START, R_BLANK2_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), R_BLANK2_START, R_BLANK2_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh3Buffer(0x740a, 0x8000, 15, onRhAdvSpareRh3Change);

void onRhAdvSpareRh4Change(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BLANK2_START, L_BLANK2_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), L_BLANK2_START, L_BLANK2_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh4Buffer(0x740c, 0x0001, 0, onRhAdvSpareRh4Change);

void onRhAdvSpareRh5Change(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BLANK3_START, L_BLANK3_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), L_BLANK3_START, L_BLANK3_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer rhAdvSpareRh5Buffer(0x740c, 0x0002, 1, onRhAdvSpareRh5Change);

#endif