/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *
 *
 *   Project OpenHornet
 *
 *   The OpenHornet Project is a F/A-18C OFP 13C Lot 20 1:1 Replica Simulator,
 *   consisting of a physical structure and electrical/software interfaces to a PC
 *   to be driven by Digital Combat Simulator (DCS).
 *
 *   ---------------------------------------------------------------------------------
 *
 *   Copyright 2016-2023 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *   ---------------------------------------------------------------------------------
 *
 *   The OpenHornet Software is based on the FP Fork of DCS-BIOS
 *   You can find more information here: https://github.com/DCSFlightpanels/dcs-bios
 *
 *   DCS-BIOS is released under the following terms:
 *   https://github.com/DCSFlightpanels/dcs-bios/blob/master/DCS-BIOS-License.txt
 *
 *   ---------------------------------------------------------------------------------
 *
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxigen capable comments.
 *
 **************************************************************************************/

/**
 * @file 1A2-MASTER_ARM_PANEL.ino
 * @author Balz Reber
 * @date 29.12.2019
 * @version u.0.0.1 (untested)
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware
 * @brief UIP Master Arm Panel Sketch
 *
 * @details Sketch for the Upper Instrument Panel, Master Arm Panel
 * 
 * **Intended Board:**
 * ABSIS NANO
 * 
 * **Wiring diagram:**
 * 
 * PIN | Function
 * --- | ---
 * 3   | .55 SQ INDICATOR DISCH Light
 * 4   | .55 SQ INDICATOR READY Light
 * 6   | MASTER MODE A/A Light
 * 7   | MASTER MODE A/A Switch
 * 8   | MASTER MODE A/G Switch
 * 9   | MASTER MODE A/G Light
 * 10  | MASTER ARM Toggle Switch
 * 11  | EMERGENCY JETT Button 
 * 
 */

/**
 * DCS BIOS Slave ID
 * 
 * The following \#define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. Since this is for the 1A2A Master Arm Panel
 * the slave address is 2, according to 1A2A.
 */
#define DCSBIOS_RS485_SLAVE 2

/**
 * TX Enable Pin
 * 
 * The Arduino pin that is connected to the
 * /RE and DE pins on the RS-485 transceiver.
 * This is **Pin 2** in all ABSIS NANO boards.
*/
#define TXENABLE_PIN 2

/**
 * DCS Bios library include
 */
#include "DcsBios.h"


/**
 * .55 SQ INDICATOR READY Light on **Pin 4**
 * @return todo
 */
DcsBios::LED mcReady(0x740c, 0x8000, 4);

/**
 * .55 SQ INDICATOR DISCH Light on **Pin 3**
 * @return todo
 */
DcsBios::LED mcDisch(0x740c, 0x4000, 3);


/**
 * .55 SQ INDICATOR Button Functionality not implemented
 * in the F/A-18 DCS Module yet.
 */

/**
 * MASTER ARM Toggle Switch on *Pin 10*
 * @return todo
 */
DcsBios::Switch2Pos masterArmSw("MASTER_ARM_SW", 10);


/**
 * MASTER MODE A/A Switch on **Pin 7**
 * @return todo
 */
DcsBios::Switch2Pos masterModeAa("MASTER_MODE_AA", 7);

/**
 * MASTER MODE A/A Light on **Pin 6**
 * @return todo
 */
DcsBios::LED masterModeAaLt(0x740c, 0x0200, 6);

/**
 * MASTER MODE A/G Switch on **Pin 8**
 * @return todo
 */
DcsBios::Switch2Pos masterModeAg("MASTER_MODE_AG", 8);

/**
 * MASTER MODE A/G Light on **Pin 9**
 * @return todo
 */
DcsBios::LED masterModeAgLt(0x740c, 0x0400, 9);


/**
 * EMERGENCY JETTISON BUTTON on **Pin 11**
 * @return todo
 */
DcsBios::Switch2Pos emerJettBtn("EMER_JETT_BTN", 11);



/**
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
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}
