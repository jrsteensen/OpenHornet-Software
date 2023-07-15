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
 * @file 1A1A1-RS485_BUS_MASTER_UIP.ino
 * @author Balz Reber
 * @date 17.12.2019
 * @brief Sketch for the RS485 Bus Master which controlls a chain of RS485 Slaves
 *
 * @details This is the Sketch for the RS485 Bus Master. The RS485 bus Master has to be
 * an Arduino Mega. This Arduino Mega is connected to the PC running DCS-bios via USB. 
 * There can't be anything connected to that Arduino except the RS485 Bus.
 * Preferably this runs on an ABSIS MEGA with a Bus termination resistor installed on the RS485 Bus.
 * You can find more information about the RS485 Bus System Architecture in the OH documentation.
 */

/**
 * Tells DCS Bios that this is the RS485 Master
 */
#define DCSBIOS_RS485_MASTER

/**
 * Define where the UART1 TX_ENABLE of the RS485 chip is connected. 
 */
#define UART1_TXENABLE_PIN 2

/**
 * DCS Bios library include
 */
#include "DcsBios.h"


/**
* Arduino Setup Function
*
* Arduino standard Setup Function with DcsBios setup finction included.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function with DcsBios loop function included
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}
