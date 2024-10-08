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
 * @file 5A1A1-RC_ABSIS_BUS_MASTER.ino
 * @author John Steensen (<a href="mailto:john.steensen@openhornet.com">john.steensen@openhornet.com</a>)
 * @date 10.07.2024
 * @version 1.0.0
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Functions as the DCS-BIOS RS485 Master of the UIP.
 *
 * @details This sketch is for the UIP ABSIS Master.
 * _Based off DCS-BIOS Skunkworks `RS485Master.ino` sample code._
 *
 *  * **Reference Designator:** 5A1A1
 *  * **Intended Board:** ABSIS Bus Master
 *  * **RS485 Bus Address:** N/A
 *
 * ###Wiring diagram:
 *
 * PIN | Function
 * --- | ---
 * 2  | UART1_TXENABLE_PIN (ABSIS BUS #1)
 * 3  | UART2_TXENABLE_PIN (ABSIS BUS #2)
 * 4  | UART3_TXENABLE_PIN (ABSIS BUS #3)
 *
 */

// The following #define tells DCS-BIOS that this is a RS-485 master device.
#define DCSBIOS_RS485_MASTER

// Define where the TX_ENABLE signals are connected.
#define UART1_TXENABLE_PIN 2
#define UART2_TXENABLE_PIN 3
#define UART3_TXENABLE_PIN 4

#include "DcsBios.h"

/**
 * @brief
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {
    DcsBios::setup();
}

/**
* @brief Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {
    DcsBios::loop();
}