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
 * @file 1A2-MASTER_ARM_PANEL.ino
 * @author Sandra Carroll (<a href="mailto:smgvbest@gmail.com">smgvbest@gmail.com</a>)
 * @date 27.12.2022
 * @version u.0.0.1 (untested)
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware.
 * @brief ABSIS ALE MASTER ARM, RS485 BUS ADDRESS 1 
 *
 * @details This sketch is for the UIP Master Arm Panel.
 * 
 *  * **Reference Designator:** 1A2
 *  * **Intended Board:** ABSIS ALE
 *  * **RS485 Bus Address:** 1
 *  
 * **Wiring diagram:**
 * 
 * PIN | Function
 * --- | ---
 * A1  | Emergency Jettison Switch
 * A2  | Air to Ground Select
 * A3  | Ready/Discharge Switch
 * D2  | Air to Air Select
 * D3  | Master Arm Switch
 * D5  | TXENABLE_PIN for RS485 Communications
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
//#define DCSBIOS_RS485_SLAVE 1

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN        5
#define UART1_SELECT

/**
 * @brief DCS Bios library include
 */
#include "DcsBios.h"

/**
 * @brief Define Control I/O for DCS-BIOS 
 * 
 */
#define e_jett_sw     A1
#define ag_sw         A2
#define ready_sw      A3
#define aa_sw         2
#define mstr_arm_sw   3


/**
 * @brief Connect switches to DCS-BIOS 
 * 
 */
DcsBios::Switch2Pos masterArmSw("MASTER_ARM_SW", mstr_arm_sw);
DcsBios::Switch2Pos masterModeAa("MASTER_MODE_AA", aa_sw);
DcsBios::Switch2Pos masterModeAg("MASTER_MODE_AG", ag_sw);
DcsBios::Switch2Pos emerJettBtn("EMER_JETT_BTN", e_jett_sw);
DcsBios::Switch2Pos fireExtBtn("FIRE_EXT_BTN", ready_sw);

/**
 * @brief The following is for Notes for Backlight/Switch Lighting and will not be called in this code
 * PIXEL ORDER    D29-D32,D7-D27,D4-D6,D28
 * 
 * READY          D29-D30
 * DISCH          D31-D32
 * AA             D6,D28
 * AA_BL          D15
 * AG             D4,D5
 * AG_BL          D16
 * FIRE_EXGTH     D7-D14
 * MSTR_ARM_BL    D17-D22
 * EMERG_JETT_BL  D23-D27
 */

/**
 * @brief When ready_sw is Depressed D31/D32 are lit
 * 
 * @param newValue 
 */
void onMcDischChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer mcDischBuffer(0x740c, 0x4000, 14, onMcDischChange);

/**
 * @brief When aa_sw is Depressed D6/D28 are lit
 * 
 * @param newValue 
 */
void onMasterModeAaLtChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

/**
 * @brief When ag_sw is Despressed D4/D5 are lit
 * 
 * @param newValue 
 */
void onMasterModeAgLtChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer masterModeAgLtBuffer(0x740c, 0x0400, 10, onMasterModeAgLtChange);

/**
 * @brief When APU_FIRE_BUTTON is Depressed  
 * 
 * @param newValue 
 */
void onFireApuLtChange(unsigned int newValue) {
    /* your code here */
}
DcsBios::IntegerBuffer fireApuLtBuffer(0x740c, 0x0004, 2, onFireApuLtChange);


/**
 * @brief 
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

}

/**
* @brief Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}