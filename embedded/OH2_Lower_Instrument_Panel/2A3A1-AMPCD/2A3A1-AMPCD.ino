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
 * @file 2A3A1-AMPCD.ino
 * @author Peter Sawka, OH Community, Arribe
 * @date 02.29.2024
 * @version 0.0.3
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the AMPCD.
 * Adapted from Peter Sawka's original Nano code
 *
 * @details
 * 
 *  * **Reference Designator:** 2A3A1
 *  * **Intended Board:** CONTROLLER_AMPCD DDI
 *  * **RS485 Bus Address:** 2
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A0  | AMPCD Rotary Brightness
 * A1  | AMPCD Off - No DCS Bios use
 * 8   | HDG +
 * 10  | HDG -
 * 4   | CRS +
 * 7   | CRS -
 *
 * 
 *
 * @brief following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile
 *
 //#define DCSBIOS_RS485_SLAVE 2
 *
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL  ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL  ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif


/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5  ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"
#include "TCA9534.h"

// Define pins per the OH Interconnect.
#define AMPCD_ROT_A A0       ///< AMPCD Rotary Brightness
#define AMPCD_ROT_OFF A1     ///< AMPCD Off  - No DCS Bios use.
#define HDG_P 8              ///< HDG +
#define HDG_M 10             ///< HDG -
#define CRS_P 4              ///< CRS +
#define CRS_M 7              ///< CRS -
#define AMPCD_BACK_LIGHT A9  ///< DDI Backlighting PWM



/**
* TCA9534 Chip Array
* Array for the 4 TCA9534 chips to read the DDI Buttons (indices): Left = 0, Top = 1, Right = 2, Bottom = 3
*
*/
TCA9534 ampcdButtons[4] = {
  TCA9534(0x23),  //Left Row
  TCA9534(0x20),  //Top Row
  TCA9534(0x22),  // Right Row
  TCA9534(0x21)
};  // Bottom Row



// Setup global variables for reading DDI button presses.
bool lastBtnState[28];               ///< Array to hold the last state of the 20 AMPCD buttons, and 4 rocker switches.
bool buttonState[28];                ///< Array to hold the current state of the 20 AMPCD buttons, and 4 rocker switches.
uint8_t inputRegister[4];            ///< Input register for button read logic.
unsigned long lastDebounceTime[28];  ///< Array to hold last time of AMPCD button update for debounce.
unsigned long debounceDelay = 10;    ///< The debounce delay duration in ms, **increase if the output flickers**.

/// DCS Bios Messages for the 4 AMPCD rocker switches.
char *AMPCD_Btns[] = { "AMPCD_GAIN_SW",
                       "AMPCD_NIGHT_DAY",
                       "AMPCD_SYM_SW",
                       "AMPCD_CONT_SW" };
char btnName[20];

//Connect switches to DCS-BIOS
DcsBios::Potentiometer ampcdBrtCtl("AMPCD_BRT_CTL", AMPCD_ROT_A);
DcsBios::Switch3Pos leftDdiCrsSw("LEFT_DDI_CRS_SW", CRS_P, CRS_M);
DcsBios::Switch3Pos leftDdiHdgSw("LEFT_DDI_HDG_SW", HDG_P, HDG_M);

/**
 * @brief Setup DCS-BIOS control for DDI backlighting
 *
 * @bug Potential bug with backlighting, the lights are either full on when DCSBios reports the intensity >50% or full off <50%. May be an electrical / PCB issue.
 * 
 */
void onInstrIntLtChange(unsigned int newValue) {
  analogWrite(AMPCD_BACK_LIGHT, map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
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
* @brief For each TCA9534 chip 'Begin', and set all of its AMPCD buttons to PinMode = INPUT
*
*/
  for (int i = 0; i < sizeof(ampcdButtons) / sizeof(ampcdButtons[0]); i++) {
    ampcdButtons[i].Begin();
    for (int j = 0; j < 7; j++) {
      ampcdButtons[i].PinMode(j, INPUT);
    }
  }
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
* 
* @attention If AMPCD button output flickers increase debounceDelay.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

  /**
* Read all the AMPCD button states and send DCSBios Commands in the following TCA9534 order: Left, Top (buttons reversed), Right (buttons reversed), Bottom.
*
*/

  for (int i = 0; i < sizeof(ampcdButtons) / sizeof(ampcdButtons[0]); i++) {
    inputRegister[i] = ampcdButtons[i].ReadAll();

    /**
* @brief Fix button index for Top and Right buttons to be in the same order as Left and Bottom buttons.
*
*/
    for (int j = 0; j < 7; j++) {

      int index = j + (7 * i);

      bool btnState = (inputRegister[i] >> (6 - j)) & 1;

      if (btnState != lastBtnState[index]) {
        lastDebounceTime[index] = millis();
      }

      if ((millis() - lastDebounceTime[index]) > debounceDelay) {
        if (btnState != buttonState[index]) {
          buttonState[index] = btnState;
          if (j > 4) {            // one of the AMPCD NIGHT, GAIN, SYMBOLOGY, CONTRAST rockers.
            if (btnState != 1) {  // If one of the AMPCD rocker buttons determine if value to send to DCS 0 or 2.
              if (i == 0 or i == 1) {
                if (j == 5) { DcsBios::sendDcsBiosMessage(AMPCD_Btns[i], "2"); }
                if (j == 6) { DcsBios::sendDcsBiosMessage(AMPCD_Btns[i], "0"); }
              } else {
                if (j == 5) { DcsBios::sendDcsBiosMessage(AMPCD_Btns[i], "0"); }
                if (j == 6) { DcsBios::sendDcsBiosMessage(AMPCD_Btns[i], "2"); }
              }
            } else {  // every AMPCD button has a "1" state.
              DcsBios::sendDcsBiosMessage(AMPCD_Btns[i], "1");
            }
          } else {  // One of the 20 push buttons.
            int btnNum = 4 - j + 5 * i;
            sprintf(btnName, "AMPCD_PB_%02d", btnNum + 1);
            DcsBios::sendDcsBiosMessage(btnName, btnState == 1 ? "0" : "1");
          }
        }
      }
      lastBtnState[index] = btnState;
    }
  }
}