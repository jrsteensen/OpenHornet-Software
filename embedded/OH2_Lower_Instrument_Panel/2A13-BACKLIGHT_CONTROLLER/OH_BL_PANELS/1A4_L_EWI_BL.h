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
 * @file 1A4_L_EWI_BL.h
 * @author Arribe
 * @date 03.25.2024
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Header file for the Left EWI panel annunciators to split out code for panel specific lighting support.
*/

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

#define FIRE_START 0 + L_EWI_LED_START_INDEX
#define FIRE_COUNT 4
#define MC_START 4 + L_EWI_LED_START_INDEX
#define MC_COUNT 4
#define GO_START 8 + L_EWI_LED_START_INDEX
#define GO_COUNT 2
#define NO_GO_START 10 + L_EWI_LED_START_INDEX
#define NO_GO_COUNT 2
#define R_BLEED_START 12 + L_EWI_LED_START_INDEX
#define R_BLEED_COUNT 2
#define L_BLEED_START 14 + L_EWI_LED_START_INDEX
#define L_BLEED_COUNT 2
#define SPD_BRK_START 16 + L_EWI_LED_START_INDEX
#define SPD_BRK_COUNT 2
#define STBY_START 18 + L_EWI_LED_START_INDEX
#define STBY_COUNT 2
#define REC_START 20 + L_EWI_LED_START_INDEX
#define REC_COUNT 2
#define L_BAR1_START 22 + L_EWI_LED_START_INDEX
#define L_BAR1_COUNT 2
#define L_BAR2_START 24 + L_EWI_LED_START_INDEX
#define L_BAR2_COUNT 2
#define XMIT_START 26 + L_EWI_LED_START_INDEX
#define XMIT_COUNT 2
#define ASPJ_ON_START 28 + L_EWI_LED_START_INDEX
#define ASPJ_ON_COUNT 2


void onFireLeftLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, FIRE_START, FIRE_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), FIRE_START, FIRE_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer fireLeftLtBuffer(0x7408, 0x0040, 6, onFireLeftLtChange);

void onMasterCautionLtChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, MC_START, MC_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), MC_START, MC_COUNT);  ///< Set light to Yellow
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer masterCautionLtBuffer(0x7408, 0x0200, 9, onMasterCautionLtChange);

void onLhAdvAspjOhChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, ASPJ_ON_START, ASPJ_ON_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), ASPJ_ON_START, ASPJ_ON_COUNT);  ///< Set light to Yellow
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvAspjOhBuffer(0x740a, 0x0008, 3, onLhAdvAspjOhChange);

void onLhAdvGoChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, GO_START, GO_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), GO_START, GO_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvGoBuffer(0x740a, 0x0010, 4, onLhAdvGoChange);

void onLhAdvLBarGreenChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BAR2_START, L_BAR2_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), L_BAR2_START, L_BAR2_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvLBarGreenBuffer(0x740a, 0x0002, 1, onLhAdvLBarGreenChange);

void onLhAdvLBarRedChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BAR1_START, L_BAR1_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), L_BAR1_START, L_BAR1_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvLBarRedBuffer(0x7408, 0x8000, 15, onLhAdvLBarRedChange);

void onLhAdvLBleedChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, L_BLEED_START, L_BLEED_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), L_BLEED_START, L_BLEED_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvLBleedBuffer(0x7408, 0x0800, 11, onLhAdvLBleedChange);

void onLhAdvNoGoChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, NO_GO_START, NO_GO_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 255, 0), NO_GO_START, NO_GO_COUNT);  ///< Set light to Yellow
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvNoGoBuffer(0x740a, 0x0020, 5, onLhAdvNoGoChange);

void onLhAdvRBleedChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, R_BLEED_START, R_BLEED_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(255, 0, 0), R_BLEED_START, R_BLEED_COUNT);  ///< Set light to Red
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvRBleedBuffer(0x7408, 0x1000, 12, onLhAdvRBleedChange);

void onLhAdvRecChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, REC_START, REC_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), REC_START, REC_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvRecBuffer(0x740a, 0x0001, 0, onLhAdvRecChange);

void onLhAdvSpdBrkChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, SPD_BRK_START, SPD_BRK_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), SPD_BRK_START, SPD_BRK_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvSpdBrkBuffer(0x7408, 0x2000, 13, onLhAdvSpdBrkChange);

void onLhAdvStbyChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, STBY_START, STBY_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), STBY_START, STBY_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvStbyBuffer(0x7408, 0x4000, 14, onLhAdvStbyChange);

void onLhAdvXmitChange(unsigned int newValue) {
  switch (newValue) {
    case 0:
      uipBLCh1.fill(0, XMIT_START, XMIT_COUNT);  ///< Off
      uipBLCh1.show();
      break;
    case 1:
      uipBLCh1.fill(uipBLCh1.Color(0, 255, 0), XMIT_START, XMIT_COUNT);  ///< Set light to Green
      uipBLCh1.show();
      break;
  }
}
DcsBios::IntegerBuffer lhAdvXmitBuffer(0x740a, 0x0004, 2, onLhAdvXmitChange);