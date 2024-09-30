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
 * @author ViajanDee
 * @date 05.21.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was tested working on a simple version of the hardware. (Remove this line once fully tested on a complete hardware set.)
 * @brief Controls the backlight for the pit.
 * 
 * @todo
 * 
 * @details
 * 
 *  * **Reference Designator:** 2A13
 *  * **Intended Board:** ABSIS BACKLIGHT CONTROLLER
 *  * **RS485 Bus Address:** NA
 * 
 *  * **Intended Board:**
 * ABSIS BACKLIGHT CONTROLLER
 * 
 *  * ** Processor **
 *  Arduino Mega + Backlight Shield
 * 
 * **Wiring Diagram:**
 * 
 * PIN | Function
 * --- | ---
 * 13  | J2 LIP_BL_CH1
 * 12  | J3 LIP_BL_CH2
 * 11  | J4 UIP_BL_CH1
 * 10  | J5 UIP_BL_CH2
 * 9   | J6 LC_BL_CH1
 * 8   | J7 LC_BL_CH2
 * 7   | J8 RC_BL_CH1
 * 6   | J9 RC_BL_CH2
 * 5   | J10 NC
 * 4   | J11 NC
 * 24  | J14 SIMPWR_BLM_A
 * 23  | J14 SIMPWR_BLM_B
 * 22  | J14 SIMPWR_PUSH
 * SDA | TEMP SNSR
 * SCL | TEMP SNSR
 * 2   | J12 & J13 Cooling fan headers.
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
*/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL ///< This enables iterrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL  ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

#include "DcsBios.h"
#include "Adafruit_NeoPixel.h"

// Define pins for DCS-BIOS per interconnect diagram, and Backlight control kicad sketch.
#define LIP_BL_CH1 13  ///< J2 LIP_BL_CH1
#define LIP_BL_CH2 12  ///< J3 LIP_BL_CH2
#define UIP_BL_CH1 11  ///< J4 UIP_BL_CH1
#define UIP_BL_CH2 10  ///< J5 UIP_BL_CH2
#define LC_BL_CH1 9    ///< J6 LC_BL_CH1
#define LC_BL_CH2 8    ///< J7 LC_BL_CH2
#define RC_BL_CH1 7    ///< J8 RC_BL_CH1
#define RC_BL_CH2 6    ///< J9 RC_BL_CH2
//#define 5   ///< J10 NC
//#define 4   ///< J11 NC
#define SIMPWR_BLM_A 22  ///< J14 SIMPWR_BLM_A
#define SIMPWR_BLM_B 23  ///< J14 SIMPWR_BLM_B
#define SIMPWR_PUSH 24   ///< J14 SIMPWR_PUSH
//#define SDA | TEMP SNSR
//#define SCL | TEMP SNSR
#define COOLING_FANS 2  ///< J12 & J13 Cooling fan headers.

#define BRIGHTNESS 50  ///< Brightness value used by the panels.

#include "2A13_BACKLIGHT_CONTROLLER.h"

Adafruit_NeoPixel lcBLCh1 = Adafruit_NeoPixel(LC1_LED_COUNT, LC_BL_CH1, NEO_GRB + NEO_KHZ800);    ///< Setup the LC BL Channel 1 string.
Adafruit_NeoPixel lcBLCh2 = Adafruit_NeoPixel(LC2_LED_COUNT, LC_BL_CH2, NEO_GRB + NEO_KHZ800);    ///< Setup the LC BL Channel 2 string.

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code that should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {
  DcsBios::setup();     // Initialize DCS-BIOS
  OpenHornet::setup();  // Initialize OpenHornet from header
  FastLED.addLeds<NEOPIXEL, LED_PIN1>(leds1, NUM_LEDS1);
  FastLED.addLeds<NEOPIXEL, LED_PIN2>(leds2, NUM_LEDS2);
  
  FastLED.setBrightness(BRIGHTNESS);

  strobeLightEffect();

  FastLED.setBrightness(brtns);

  //currentMode = 5;
  //applyMode(currentMode);         // Initialize in the default mode
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code that should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {
  DcsBios::loop();      //Run DCS-BIOS loop
  encoder.tick();
  checkRotaryEncoder();
  checkButtonPress();

  if (currentMode == 5 && brtns > 0) {
    checkButtonPress();
  }

  switch (currentMode) {
        case 1:
            dynamicRainbow();
            break;
        case 2:
            confetti();
            break;
        case 3:
            sinelon();
            break;
        case 4:
            juggle();
            break;
        case 5:
            fill_solid(leds1, NUM_LEDS1, CRGB::Black);
            fill_solid(leds2, NUM_LEDS2, CRGB::Black);
            FastLED.show();
            break;
    }
}