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
 * @file 2A13-BACKLIGHT_CONTROLLER.ino
 * @author Arribe, Ulukaii, Higgins
 * @date May 01, 2025
 * @version V 0.1 (tested)
 * @warning This sketch is based on OH-Interconnect. You may need to adapt it to your actual wiring and panel versions.
 * @brief Controls backlights & most annunciators of the cockpit. Coded for Arduino MEGA 2560 + ABSIS BACKLIGHT SHIELD.
 * @details
 * **Wiring Diagram:** 
 * 
 * | Pin | Function                      |
 * |-----|-------------------------------|
 * | 13  | J2 LIP_CH1                    |
 * | 12  | J3 LIP_CH2                    |
 * | 11  | J4 UIP_CH1                    |
 * | 10  | J5 UIP_CH2                    |
 * | 9   | J6 LC_CH1                     |
 * | 8   | J7 LC_CH2                     |
 * | 7   | J8 RC_CH1                     |
 * | 6   | J9 RC_CH2                     |
 * | 5   | J10 NC                        |
 * | 4   | J11 NC                        |
 * | 24  | J14 SIMPWR_BLM_A              |
 * | 23  | J14 SIMPWR_BLM_B              |
 * | 22  | J14 SIMPWR_BLM_PUSH           |
 * | SDA | TEMP SNSR                     |
 * | SCL | TEMP SNSR                     |
 * | 2   | J12 & J13 Cooling fan headers |
 */

/**********************************************************************************************************************
 * @brief Preprocessor directives 
 * @remark In particular:
 *         1. Check if we're on a Mega328 or Mega2560 and choose serial interface (Pro Micro: Default)
 *         2. Include external libraries FastLED and DcsBios
 *         3. Include the abstract Panel base class and other panel classes
 *********************************************************************************************************************/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "FastLED.h"
#include "DcsBios.h"
#include "panels/Panel.h"  
#include "panels/1A2A1_MASTER_ARM.h"
#include "panels/1A4_L_EWI.h"
#include "panels/1A7A1_HUD_PANEL_REV4.h"            //Make sure to uncomment the correct HUD panel header file
//#include "panels/1A7A1_HUD_PANEL_REV3.h"          //Make sure to uncomment the correct include statement for your HUD panel
#include "panels/1A5_R_EWI.h"
#include "panels/1A6A1_SPN_RCVY.h"
#include "panels/2A2A1A6_ECM.h"
#include "panels/Colors.h"

/********************************************************************************************************************
 * @brief Set up global variables: set up pins according to (YOUR!) wiring; set up FastLED arrays.
 * @remark The written number of LEDs per strip might exceed the actual number. This can be ignored.
 ********************************************************************************************************************/
const uint8_t pin_LIP_CH1  =     13;
const uint8_t pin_LIP_CH2  =     12;
const uint8_t pin_UIP_CH1  =     11;
const uint8_t pin_UIP_CH2  =      9;              // Ulukaii deviation. Standard is pin 10
const uint8_t pin_LC_CH1   =     10;             // Ulukaii deviation. Standard is pin 9
const uint8_t pin_LC_CH2   =      8;
const uint8_t pin_RC_CH1   =      7;
const uint8_t pin_RC_CH2   =      6;
const uint8_t pin_CH9      =      5;
const uint8_t pin_CH10     =      4;
const uint8_t pin_EncoderSw = 24;              // Ulukaii deviation. Standard is 22
const uint8_t pin_EncoderA  = 22;              // Ulukaii deviation. Standard is 24
const uint8_t pin_EncoderB  = 23;                            

// LED counts per channel
const int LED_COUNT_LIP_1 = 100;
const int LED_COUNT_LIP_2 = 120;
const int LED_COUNT_UIP_1 = 210;
const int LED_COUNT_LC_1 = 200;
const int LED_COUNT_LC_2 = 233;
const int LED_COUNT_RC_1 = 250;
const int LED_COUNT_RC_2 = 380;

// Define arrays for each channel using the named constants
CRGB LIP_1[LED_COUNT_LIP_1];
CRGB LIP_2[LED_COUNT_LIP_2];
CRGB UIP_1[LED_COUNT_UIP_1];
CRGB LC_1[LED_COUNT_LC_1];
CRGB LC_2[LED_COUNT_LC_2];
CRGB RC_1[LED_COUNT_RC_1];
CRGB RC_2[LED_COUNT_RC_2];

/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 ********************************************************************************************************************/
void setup() {
  // Initialize FastLED
  FastLED.addLeds<WS2812B, pin_LIP_CH1, GRB>(LIP_1, LED_COUNT_LIP_1);
  FastLED.addLeds<WS2812B, pin_LIP_CH2, GRB>(LIP_2, LED_COUNT_LIP_2);
  FastLED.addLeds<WS2812B, pin_UIP_CH1, GRB>(UIP_1, LED_COUNT_UIP_1);
  FastLED.addLeds<WS2812B, pin_LC_CH1,  GRB>(LC_1,  LED_COUNT_LC_1);
  FastLED.addLeds<WS2812B, pin_LC_CH2,  GRB>(LC_2,  LED_COUNT_LC_2);
  FastLED.addLeds<WS2812B, pin_RC_CH1,  GRB>(RC_1,  LED_COUNT_RC_1);
  FastLED.addLeds<WS2812B, pin_RC_CH2,  GRB>(RC_2,  LED_COUNT_RC_2);

  // Initialize all LED strips to BLACK
  fill_solid(LIP_1, LED_COUNT_LIP_1, COLOR_BLACK);
  fill_solid(LIP_2, LED_COUNT_LIP_2, COLOR_BLACK);
  fill_solid(UIP_1, LED_COUNT_UIP_1, COLOR_BLACK);
  fill_solid(LC_1,  LED_COUNT_LC_1,  COLOR_BLACK);
  fill_solid(LC_2,  LED_COUNT_LC_2,  COLOR_BLACK);
  fill_solid(RC_1,  LED_COUNT_RC_1,  COLOR_BLACK);
  fill_solid(RC_2,  LED_COUNT_RC_2,  COLOR_BLACK);
  FastLED.show();

  // Initialize panels. Adapt the order of the panels according to your physical panel order.
  int currentIndex = 0;
  
  // UIP_1 Channel Panels
  // 1. Master Arm Panel (29 LEDs)
  MasterArmPanel* masterArmPanel = MasterArmPanel::getInstance(currentIndex, UIP_1);
  currentIndex += masterArmPanel->getLedCount();
  
  // 2. Left EWI Panel (30 LEDs)
  EwiPanel* ewiPanel = EwiPanel::getInstance(currentIndex, UIP_1);
  currentIndex += ewiPanel->getLedCount();
  
  // 3. HUD Panel (50 LEDs)
  //HudPanel* hudPanel = HudPanel::getInstance(currentIndex, UIP_1);
  //currentIndex += hudPanel->getLedCount();
  
  // 4. Right EWI Panel (30 LEDs)
  REwiPanel* rEwiPanel = REwiPanel::getInstance(currentIndex, UIP_1);
  currentIndex += rEwiPanel->getLedCount();
  
  // 5. Spin Recovery Panel (63 LEDs)
  SpnRcvyPanel* spnRcvyPanel = SpnRcvyPanel::getInstance(currentIndex, UIP_1);
  currentIndex += spnRcvyPanel->getLedCount();

  // LIP_2 Channel Panels
  currentIndex = 0;  // Reset index for new channel
  
  // 1. ECM Panel (79 LEDs)
  EcmPanel* ecmPanel = EcmPanel::getInstance(currentIndex, LIP_2);
  currentIndex += ecmPanel->getLedCount();

  // Run DCS Bios setup function
  DcsBios::setup();
}

void loop() {
  //Run DCS Bios loop function
  DcsBios::loop();
  
  // Call the updateLeds() method of the Panel class, which will update the LEDs if any changes are pending
  Panel::updateLeds();
}
