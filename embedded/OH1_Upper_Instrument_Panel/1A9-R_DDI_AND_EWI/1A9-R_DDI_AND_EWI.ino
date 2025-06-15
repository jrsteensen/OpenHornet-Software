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
 * @file 1A9-R_DDI_AND_EWI.ino
 * @author Peter Sawka, OH Community, Arribe
 * @date 02.29.2024
 * @version 0.0.3
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the right DDI & EWI module.
 * adapted from Peter Sawka's original Nano code
 *
 * @details
 * 
 *  * **Reference Designator:** 1A9
 *  * **Intended Board:** CONTROLLER_AMPCD DDI
 *  * **RS485 Bus Address:** 4
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A0  | RDDI Rotary - Day
 * A1  | RDDI Rotary - Night
 * A2  | RDDI Rotary - Off
 * A6  | RDDI Brightness Encoder A
 * 7   | RDDI Brightness Encoder B
 * 8   | RDDI Contrast Encoder A
 * A10 | RDDI Contrast Encoder B
 * 14  | REWI Fire
 * 16  | REWI Master Caution 
 * 9   | DDI Backlighting PWM, pin must be defined as digital #
 * 6   | DDI Interrupt Pin 
 * 
 *
 * @brief following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile
 *
 //#define DCSBIOS_RS485_SLAVE 4
 *
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif


/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5 ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"
#include "Wire.h"
#include "TCA9534.h"

// Define pins per the OH Interconnect. 
#define RDDI_ROT_DAY A0 ///< RDDI Rotary - Day
#define RDDI_ROT_NIGHT A1 ///< RDDI Rotary - Night
#define RDDI_ROT_OFF A2 ///< RDDI Rotary - Off
#define RDDI_BRT_A A6 ///< RDDI Brightness Encoder A
#define RDDI_BRT_B 7 ///< RDDI Brightness Encoder B
#define RDDI_CONT_A 8 ///< RDDI Contrast Encoder A
#define RDDI_CONT_B A10 ///< RDDI Contrast Encoder B
#define REWI_FIRE_SW 14 ///< REWI Fire
#define REWI_MC_SW 16 ///< REWI Master Caution
#define DDI_BACK_LIGHT 9 ///< DDI Backlighting PWM, pin must be defined as digital #
#define DDI_IRQ 6 ///< DDI Interrupt Pin

/**
* TCA9534 Chip Array
* Array for the 4 TCA9534 chips to read the DDI Buttons (indices): RIGHT = 0, Top = 1, Right = 2, Bottom = 3
*
*/
TCA9534 ddiButtons[4] = {
  TCA9534(0x23),  //RIGHT Row
  TCA9534(0x20),  //Top Row
  TCA9534(0x22),  // Right Row
  TCA9534(0x21)};   // Bottom Row



// Setup global variables for reading DDI button presses. 
bool lastBtnState[20]; ///< Array to hold the last state of the 20 DDI buttons.
bool buttonState[20]; ///< Array to hold the current state of the 20 DDI buttons.
uint8_t inputRegister[4]; ///< Input register for button read logic.
unsigned long lastDebounceTime[20]; ///< Array to hold last time of DDI button update for debounce.
unsigned long debounceDelay = 10;  ///< The debounce delay duration in ms, **increase if the output flickers**.

int index;

//Connect switches to DCS-BIOS 
DcsBios::RotaryEncoder rightDdiBrtCtl("RIGHT_DDI_BRT_CTL", "-3200", "+3200", RDDI_BRT_A, RDDI_BRT_B);
DcsBios::RotaryEncoder rightDdiContCtl("RIGHT_DDI_CONT_CTL", "-3200", "+3200", RDDI_CONT_A, RDDI_CONT_B);
DcsBios::Switch2Pos apuFireBtn("APU_FIRE_BTN", REWI_MC_SW);
DcsBios::SwitchWithCover2Pos rightFireBtn("RIGHT_FIRE_BTN", "RIGHT_FIRE_BTN_COVER", REWI_FIRE_SW);

/** 
* @brief Initialize pin array for DCSBios multi-position switch call per BORT command reference.
*
*/
const byte rightDdiBrtSelectPins[3] = { RDDI_ROT_OFF, RDDI_ROT_NIGHT, RDDI_ROT_DAY };
DcsBios::SwitchMultiPos rightDdiBrtSelect("RIGHT_DDI_BRT_SELECT", rightDdiBrtSelectPins, 3);

/**
 * @brief Setup DCS-BIOS control for DDI backlighting
 *
 */
void onInstrIntLtChange(unsigned int newValue) {
  analogWrite(DDI_BACK_LIGHT, map(newValue, 0, 65535, 0, 255));
} DcsBios::IntegerBuffer instrIntLtBuffer(0x7560, 0xffff, 0, onInstrIntLtChange);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(DDI_BACK_LIGHT, OUTPUT); // set backlighting pin mode to output
  analogWrite(DDI_BACK_LIGHT, 0); // turn off backlighting
  
  pinMode(DDI_IRQ, OUTPUT);  // Set IRQ pinmode to match sample code to avoid controls becomming unresponsive issue

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
* 
* @attention If DDI button output flickers increase debounceDelay.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

/**
* Read all the DDI button states and send DCSBios Commands in the following TCA9534 order: RIGHT, Top (buttons reversed), Right (buttons reversed), Bottom.
*
*/
  for (int i = 0; i < sizeof(ddiButtons) / sizeof(ddiButtons[0]); i++) { // RIGHT = 0, Top = 1, Right = 2, Bottom = 3
    inputRegister[i] = ddiButtons[i].ReadAll();

/**
* @brief Fix button index for Top and Right buttons to be in the same order as RIGHT and Bottom buttons.
*
*/
    for (int j = 0; j < 5; j++) {

      if (i == 1 || i == 2) {  //button order reversed, adjust the index accordingly.
        index = ((4 - j) + 5 * i);
      } else {
        index = (j + 5 * i);
      }

      /**
      * Set button current state from the inputRegister. If button state changed and longer than the debounce delay, then send the
      * corresponding DCSBios message by building the proper "RIGHT_DDI_PB_" + fixed index number string.
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
          sprintf(btnName, "RIGHT_DDI_PB_%02d", index + 1);
          DcsBios::tryToSendDcsBiosMessage(btnName, btnState == 1 ? "0" : "1");
        }
      }
      lastBtnState[index] = btnState;
    }
  }
}