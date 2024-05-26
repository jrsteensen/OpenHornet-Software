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

#include <DcsBios.h>
//#include "Adafruit_NeoPixel.h"
#include <RotaryEncoder.h>
#include <FastLED.h>

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

#define APU_LED_START_INDEX 0                   ///< APU panel is the first in the LC BL Channel 2 string.
#define AP_LED_COUNT 19

#define FCS_LED_START_INDEX 19                   ///< FCS panel is the second in the LC BL Channel 2 string.
#define FC_LED_COUNT 23

#define COMM_LED_START_INDEX 42                 ///< Comm panel is the third in the LC BL Channel 2 string.
#define CP_LED_COUNT 109

#define ANT_SEL_LED_START_INDEX 151             ///< Ant Sel panel is the fourth in the LC BL Channel 2 string.
#define AS_LED_COUNT 17

#define OBOGS_LED_START_INDEX 168               ///< Obogs panel is the fifth in the LC BL Channel 2 string.
#define OB_LED_COUNT 16

#define MC_HYD_ISOL_LED_START_INDEX 184         ///< MC/HYD Isol panel is the sixth in the LC BL Channel 2 string.
#define HI_LED_COUNT 24

#define LC_BL_CH1 9                             ///< Define the pin for Channel 1
#define LC_BL_CH2 8                             ///< Define the pin for Channel 2

#define LC1_START_INDEX 0                       ///< Define the start of Channel 1
#define LC1_LED_COUNT LG_LED_COUNT + SJ_LED_COUNT + FT_LED_COUNT + SP_LED_COUNT + GT_LED_COUNT + EL_LED_COUNT + FP_LED_COUNT      ///< The total number of LEDs in Channel 1

#define LC2_START_INDEX 0                       ///< Define the start of Channel 2
#define LC2_LED_COUNT AP_LED_COUNT + FC_LED_COUNT + CP_LED_COUNT + AS_LED_COUNT + OB_LED_COUNT + HI_LED_COUNT                     ///< The total number of LEDs in Channel 2

extern Adafruit_NeoPixel lcBLCh1;
extern Adafruit_NeoPixel lcBLCh2;

RotaryEncoder encoder(SIMPWR_BLM_A, SIMPWR_BLM_B, RotaryEncoder::LatchMode::TWO03);

#define LED_PIN1 LC_BL_CH1
#define LED_PIN2 LC_BL_CH2
#define NUM_LEDS1 LC1_LED_COUNT                 ///< Number of LC Channel 1 LEDs
#define NUM_LEDS2 LC2_LED_COUNT                 ///< Number of LC Channel 2 LEDs

CRGB leds1[NUM_LEDS1];                          ///< LED array for LC Channel 1
CRGB leds2[NUM_LEDS2];                          ///< LED array for LC Channel 2

/**
* @brief LC Variables
* The variables are defined here
*/
int currentMode = 5;
int brtns;

const int modeCount = 7; // Update this as you add more modes

void setBacklightBrightness(uint8_t brtns);
void checkButtonPress();
void applyMode(int mode);

namespace OpenHornet {
  void setup() {
    lcBLCh1.begin();
    lcBLCh2.begin();
    pinMode(SIMPWR_PUSH, INPUT_PULLUP);
    encoder.setPosition(0);
    fill_solid(leds1, NUM_LEDS1, CRGB::Green); // Initialize LEDs ON
    fill_solid(leds2, NUM_LEDS2, CRGB::Green);
    FastLED.show();
  }
}

/**
* @brief Backlight Brightness
* This function controls the brightness of the panels backlight
*/
void setBacklightBrightness(uint8_t brtns) {
  FastLED.setBrightness(brtns);
  FastLED.show();
}

void dynamicRainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CHSV(hue + (i * 10), 255, 255);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CHSV(hue + (i * 10), 255, 255);
  }
  FastLED.show();
  hue++;
  delay(1); // Adjust to control the speed of the rainbow
}

void confetti() {
  fadeToBlackBy(leds1, NUM_LEDS1, 10);
  int pos = random16(NUM_LEDS1);
  leds1[pos] += CHSV(random8(), 200, 255);

  fadeToBlackBy(leds2, NUM_LEDS2, 10);
  pos = random16(NUM_LEDS2);
  leds2[pos] += CHSV(random8(), 200, 255);

  FastLED.show();
}

void sinelon() {
  fadeToBlackBy(leds1, NUM_LEDS1, 20);
  int pos = beatsin16(13, 0, NUM_LEDS1 - 1);
  leds1[pos] += CHSV(0, 255, 192);

  fadeToBlackBy(leds2, NUM_LEDS2, 20);
  pos = beatsin16(13, 0, NUM_LEDS2 - 1);
  leds2[pos] += CHSV(0, 255, 192);

  FastLED.show();
}


void juggle() {
    fadeToBlackBy(leds1, NUM_LEDS1, 20);
    for (int i = 0; i < 8; i++) {
        leds1[beatsin16(i + 7, 0, NUM_LEDS1 - 1)] |= CHSV(i * 32, 200, 255);
    }

    fadeToBlackBy(leds2, NUM_LEDS2, 20);
    for (int i = 0; i < 8; i++) {
        leds2[beatsin16(i + 7, 0, NUM_LEDS2 - 1)] |= CHSV(i * 32, 200, 255);
    }

    FastLED.show();
}

void sequentialBootUpTest() {
  // Sequentially light up each LED
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CRGB::Green;
    FastLED.show();
    delay(50); // Adjust delay for desired speed
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CRGB::Green;
    FastLED.show();
    delay(5); // Adjust delay for desired speed
  }
  delay(50); // Hold for a moment

  // Turn off all LEDs
  fill_solid(leds1, NUM_LEDS1, CRGB::Black);
  fill_solid(leds2, NUM_LEDS2, CRGB::Black);
  FastLED.show();
}

void pulseWaveTest() {
  for (int j = 0; j < 3; j++) { // Repeat the pulse three times
    for (int brightness = 0; brightness <= 255; brightness += 5) {
      fill_solid(leds1, NUM_LEDS1, CRGB(brightness, brightness, brightness));
      fill_solid(leds2, NUM_LEDS2, CRGB(brightness, brightness, brightness));
      FastLED.show();
      delay(30);
    }
    for (int brightness = 255; brightness >= 0; brightness -= 5) {
      fill_solid(leds1, NUM_LEDS1, CRGB(brightness, brightness, brightness));
      fill_solid(leds2, NUM_LEDS2, CRGB(brightness, brightness, brightness));
      FastLED.show();
      delay(3);
    }
  }
}

void rapidFlashTest() {
  for (int i = 0; i < 5; i++) {
    fill_solid(leds1, NUM_LEDS1, CRGB::White);
    fill_solid(leds2, NUM_LEDS2, CRGB::White);
    FastLED.show();
    delay(100);
    fill_solid(leds1, NUM_LEDS1, CRGB::Black);
    fill_solid(leds2, NUM_LEDS2, CRGB::Black);
    FastLED.show();
    delay(100);
  }
}

void strobeLightEffect() {
  const int strobeDuration = 100; // Total duration of the strobe effect in milliseconds
  const int strobeInterval = 50;  // Interval between strobe flashes in milliseconds
  const int strobeCycles = strobeDuration / strobeInterval;
  const int delayBetweenStrobes = 3000; // Delay between strobe repetitions in milliseconds

  for (int repeat = 0; repeat < 3; repeat++) { // Repeat 3 times
    for (int i = 0; i < strobeCycles; i++) {
      // Turn on all LEDs to white
      fill_solid(leds1, NUM_LEDS1, CRGB::White);
      fill_solid(leds2, NUM_LEDS2, CRGB::White);
      FastLED.show();
      delay(strobeInterval);

      // Turn off all LEDs
      fill_solid(leds1, NUM_LEDS1, CRGB::Black);
      fill_solid(leds2, NUM_LEDS2, CRGB::Black);
      FastLED.show();
      delay(strobeInterval);
    }
    delay(delayBetweenStrobes); // Delay between repetitions
  }
  setBacklightBrightness(brtns);
  applyMode(0);
}

void fluorescentEffect() {
  const int flashCount = 4; // Number of random flashes
  const int minFlashDuration = 20; // Minimum duration of a flash in milliseconds
  const int maxFlashDuration = 200; // Maximum duration of a flash in milliseconds
  const int steadyDuration = 3000; // Duration for steady light in milliseconds

  // Random flashes
  for (int i = 0; i < flashCount; i++) {
    int flashDuration = random(minFlashDuration, maxFlashDuration);
    
    // Turn on all LEDs to white
    fill_solid(leds1, NUM_LEDS1, CRGB::Green);
    fill_solid(leds2, NUM_LEDS2, CRGB::Green);
    FastLED.show();
    delay(flashDuration);

    // Turn off all LEDs
    fill_solid(leds1, NUM_LEDS1, CRGB::Black);
    fill_solid(leds2, NUM_LEDS2, CRGB::Black);
    FastLED.show();
    delay(flashDuration);
  }

  // Steady light
  fill_solid(leds1, NUM_LEDS1, CRGB::Green);
  fill_solid(leds2, NUM_LEDS2, CRGB::Green);
  FastLED.show();
  delay(steadyDuration);

  // Turn off all LEDs
  fill_solid(leds1, NUM_LEDS1, CRGB::Black);
  fill_solid(leds2, NUM_LEDS2, CRGB::Black);
  FastLED.show();
}

void checkButtonPress() {
  static bool buttonWasPressed = false;
  bool buttonPressed = digitalRead(SIMPWR_PUSH) == LOW;
  if (buttonPressed && !buttonWasPressed) {
    applyMode(0);
    setBacklightBrightness(50);
    encoder.setPosition(0);
  }
  buttonWasPressed = buttonPressed;
}

void checkRotaryEncoder() {
  static int lastPos = -1;
  int newPos = encoder.getPosition();
  if (newPos != lastPos) {
    if (newPos > lastPos) {
      currentMode = (currentMode + 1) % modeCount;
    } else {
      currentMode = (currentMode - 1 + modeCount) % modeCount;
    }
    applyMode(currentMode);
    lastPos = newPos;
  }
}

void applyMode(int mode) {
  switch (mode) {
    case 0:
      fill_solid(leds1, NUM_LEDS1, CRGB::Green); // Default green mode
      fill_solid(leds2, NUM_LEDS2, CRGB::Green);
      FastLED.show();
      break;
    case 1:
      // Dynamic Rainbow mode handled in loop()
      break;
    case 2:
      confetti(); // Confetti mode
      break;
    case 3:
      sinelon(); // Sinelon mode
      break;
    case 4:
      juggle(); // Juggle mode
      break;
    // Add more cases as you implement additional modes
    case 5:
      fill_solid(leds1, NUM_LEDS1, CRGB::Black);
      fill_solid(leds2, NUM_LEDS2, CRGB::Black);
      FastLED.show();
      break;
  }
}

/**
* @brief DCS Bios Backlight Hook
* This callback function monitors DCS Bios for the backlight value
*/
void onConsoleIntLtChange(unsigned int newValue) {
  brtns = map(newValue, 0, 65535, 0, 255); // Map the DCS-BIOS value to 0-255 range
  setBacklightBrightness(brtns);

  if (brtns > 0 && currentMode == 5) {
    currentMode = 0;
    applyMode(currentMode);
  }
}

/**
* @brief Initialize DCS Bios buffers
*/
DcsBios::IntegerBuffer consoleIntLtBuffer(0x7558, 0xffff, 0, onConsoleIntLtChange);
// DcsBios::RotaryEncoder consolesDimmer("CONSOLES_DIMMER", "-4047", "+4047", SIMPWR_BLM_A, SIMPWR_BLM_B);