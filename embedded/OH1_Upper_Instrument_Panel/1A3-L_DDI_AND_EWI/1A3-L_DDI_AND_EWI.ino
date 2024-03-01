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
 *   GitHub repository is released under the Creative Commons Atribution - 
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/

/**
 * @file 1A3-L_DDI_AND_EWI.ino
 * @author Peter Sawka, OH Community, Arribe
 * @date 02.29.2024
 * @version u.0.0.3
 * @brief Controls the left DDI & EWI module.
 * adapted from Peter Sawka's original Nano code
 * @details
 * 
 *  * **Reference Designator:** 1A3
 *  * **Intended Board:** CONTROLLER_AMPCD DDI
 *  * **RS485 Bus Address:** 2
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A0  | LDDI Rotary - Day
 * A1  | LDDI Rotary - Night
 * A2  | LDDI Rotary - Off
 * A6  | LDDI Brightness Encoder A
 * 7   | LDDI Brightness Encoder B
 * 8   | LDDI Contrast Encoder A
 * A10 | LDDI Contrast Encoder B
 * 14  | LEWI Fire
 * 16  | LEWI Master Caution 
 * A9  | DDI Backlighting PWM 
 * 
 */

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL
#else
#define DCSBIOS_DEFAULT_SERIAL
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * @brief following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile
*/
//#define DCSBIOS_RS485_SLAVE 2

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5
#define UART1_SELECT

/**
 * DCS Bios library include
 */
#include "DcsBios.h"

/**
* @brief- Use library from @Balse on Discord - https://github.com/balzreber/TCA9534
*
*/
#include "TCA9534.h"

/**
 * @brief Define Control I/O for DCS-BIOS 
 * 
 */
#define LDDI_ROT_DAY A0
#define LDDI_ROT_NIGHT A1
#define LDDI_ROT_OFF A2
#define LDDI_BRT_A A6
#define LDDI_BRT_B 7
#define LDDI_CONT_A 8
#define LDDI_CONT_B A10
#define LEWI_FIRE_SW 14
#define LEWI_MC_SW 16
#define DDI_BACK_LIGHT A9

/**
* Setup the DDI Buttons as an array with indexes: Left = 0, Top = 1, Right = 2, Bottom = 3
*
*/
TCA9534 ddiButtons[4] = {
  TCA9534(0x23),  //Left Row
  TCA9534(0x20),  //Top Row
  TCA9534(0x22),  //Right Row
  TCA9534(0x21)   //Bottom Row
};

/**
* @brief Setup global variables for reading DDI button presses.  If output flickers increase debounceDelay.
*
*/
bool lastBtnState[20];
bool buttonState[20];
uint8_t inputRegister[4];
unsigned long lastDebounceTime[20];
unsigned long debounceDelay = 10;  // the debounce time; increase if the output flickers

/**
 * @brief Connect switches to DCS-BIOS 
 * 
 */
DcsBios::RotaryEncoder leftDdiBrtCtl("LEFT_DDI_BRT_CTL", "-3200", "+3200", LDDI_BRT_A, LDDI_BRT_B);
DcsBios::RotaryEncoder leftDdiContCtl("LEFT_DDI_CONT_CTL", "-3200", "+3200", LDDI_CONT_A, LDDI_CONT_B);
DcsBios::Switch2Pos masterCautionResetSw("MASTER_CAUTION_RESET_SW", LEWI_MC_SW);
DcsBios::SwitchWithCover2Pos leftFireBtn("LEFT_FIRE_BTN", "LEFT_FIRE_BTN_COVER", LEWI_FIRE_SW);

/** 
* @brief Initialize pin array for DCSBios multiposition switch call per BORT command reference.
*
*/
const byte leftDdiBrtSelectPins[3] = { LDDI_ROT_OFF, LDDI_ROT_NIGHT, LDDI_ROT_DAY };
DcsBios::SwitchMultiPos leftDdiBrtSelect("LEFT_DDI_BRT_SELECT", leftDdiBrtSelectPins, 3);

/**
 * @brief Setup DCS-BIOS control for DDI backlighting
 * 
 */
void onInstrIntLtChange(unsigned int newValue) {
  analogWrite(DDI_BACK_LIGHT, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  /**
* @brief Initialize last button state array to all 0's.
*
*/
  for (int i = 0; i < sizeof(lastBtnState) / sizeof(lastBtnState[0]); i++) {
    lastBtnState[i] = 0;
  }

  /**
* @brief For each TCA9534 chip 'Begin', and set all of its DDI buttons to PinMode = INPUT
*
*/
  for (int i = 0; i < sizeof(ddiButtons) / sizeof(ddiButtons[0]); i++) {
    ddiButtons[i].Begin();
    for (int j = 0; j < 5; j++) {
      ddiButtons[i].PinMode(j, INPUT);
    }
  }
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  /**
* Read all the DDI button states and send DCSBios Commands in the following TCA9534 order: Left, Top (buttons reversed), Right (buttons reversed), Bottom.
*
*/
  for (int i = 0; i < sizeof(ddiButtons) / sizeof(ddiButtons[0]); i++) { // Left = 0, Top = 1, Right = 2, Bottom = 3
    inputRegister[i] = ddiButtons[i].ReadAll();

    /**
* @brief Fix button index for Top and Right buttons to be in the same order as Left and Bottom buttons.
*
*/
    for (int j = 0; j < 5; j++) {
      int index;
      if (i == 1 || i == 2) {  //button order reversed, adjust the index accordingly.
        index = ((4 - j) + 5 * i);
      } else {
        index = (j + 5 * i);
      }

      /**
      * Set button current state from the inputRegister. If button state changed and longer than the debounce delay, then send the
      * corresponding DCSBios message by building the proper "LEFT_DDI_PB_" + fixed index number string.
      *
      */
      bool btnState = (inputRegister[i] >> (4 - j)) & 1;

      if (btnState != lastBtnState[index]) {
        lastDebounceTime[index] = millis();
      }

      if ((millis() - lastDebounceTime[index]) > debounceDelay) {
        if (btnState != buttonState[index]) {
          buttonState[index] = btnState;
          char btnName[14];
          sprintf(btnName, "LEFT_DDI_PB_%02d", index + 1);
          DcsBios::tryToSendDcsBiosMessage(btnName, btnState == 1 ? "0" : "1");
        }
      }
      lastBtnState[index] = btnState;
    }
  }
}