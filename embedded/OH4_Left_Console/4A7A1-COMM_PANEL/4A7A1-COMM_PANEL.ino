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
 * @file 4A7A1-COMM_PANEL.ino
 * @author Arribe
 * @date 03.03.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief @brief Controls the COMM panel & ANT SEL panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A7A1
 *  * **Intended Board:** ABSIS_Mega
 *  * **RS485 Bus Address:** 8
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A0  | VOX MIC COLD - HOT
 * A15 | ICS Volume
 * A1  | RWR Volume
 * A14 | WPN Volume
 * A2  | MIDSB Volume
 * A13 | MIDSA Volume
 * A3  | TCN Volume
 * A12 | AUX Volume
 * A6  | ILS Channel 1
 * A9  | ILS Channel 2
 * A7  | ILS Channel 3
 * A8  | ILS Channel 4
 * 52  | RLY PLAIN
 * 53  | RLY CIPHER
 * 50  | GXMT COMM2
 * 51  | GXMT COMM1
 * 48  | ILS Manual or UFC
 * 49  | IFF CRYPTO ZERO
 * 46  | IFF CRYPTO HOLD
 * 47  | IFFMAS NORM or EMERG
 * 44  | MODE4 OFF
 * 45  | MODE4 DIS/AUD
 * 43  | ANT SEL Panel COMM 1 LOWER
 * 40  | ANT SEL Panel COMM 1 UPPER
 * 41  | ANT SEL Panel IFF LOWER
 * 38  | ANT SEL Panel IFF UPPER
 * 36  | ILS Channel 5
 * 37  | ILS Channel 6
 * 34  | ILS Channel 7
 * 35  | ILS Channel 8
 * 32  | ILS Channel 9
 * 33  | ILS Channel 10
 * 30  | ILS Channel 11
 * 31  | ILS Channel 12
 * 28  | ILS Channel 13
 * 29  | ILS Channel 14
 * 26  | ILS Channel 15
 * 27  | ILS Channel 16
 * 24  | ILS Channel 17
 * 25  | ILS Channel 18
 * 22  | ILS Channel 19
 * 23  | ILS Channel 20
 * 
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug RS485 currently does not work with the Pro Micro (32U4), Fails to compile.
   @todo When the RS485 is resolved for the Pro Micro finish coding for RS485 on the Comm Panel's Mega.

   #define DCSBIOS_RS485_SLAVE 8 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
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
#define UART1_TXENABLE_PIN 1 ///< Sets TXENABLE_PIN to Arduino Mega pin Tx0
#define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"

// Define pins for DCS-BIOS per interconnect diagram.
 #define VOX_A A0  ///< VOX MIC COLD - HOT
 #define ICS_A A15 ///< ICS Volume
 #define RWR_A A1  ///< RWR Volume
 #define WPN_A A14 ///< WPN Volume
 #define MIDSB_A A2  ///< MIDSB Volume
 #define MIDSA_A A13 ///< MIDSA Volume
 #define TCN_A A3  ///< TCN Volume
 #define AUX_A A12 ///< AUX Volume
 #define ILS_SW1 A6  ///< ILS Channel 1
 #define ILS_SW2 A9  ///< ILS Channel 2
 #define ILS_SW3 A7  ///< ILS Channel 3
 #define ILS_SW4 A8  ///< ILS Channel 4
 #define RLY_SW1 52  ///< RLY PLAIN
 #define RLY_SW2 53  ///< RLY CIPHER
 #define GXMT_SW1 50  ///< GXMT COMM2
 #define GXMT_SW2 51  ///< GXMT COMM1
 #define ILSUFC_SW1 48  ///< ILS Manual or UFC
 #define IFFCRY_SW1 49  ///< IFF CRYPTO ZERO
 #define IFFCRY_SW2 46  ///< IFF CRYPTO HOLD
 #define IFFMAS_SW1 47  ///< IFFMAS NORM or EMERG
 #define MODE4_SW1 44  ///< MODE4 OFF
 #define MODE4_SW2 45  ///< MODE4 DIS/AUD
 #define COMANT_SW1 43  ///< ANT SEL Panel COMM 1 LOWER
 #define COMANT_SW2 40  ///< ANT SEL Panel COMM 1 UPPER
 #define IFFANT_SW1 41  ///< ANT SEL Panel IFF LOWER
 #define IFFANT_SW2 38  ///< ANT SEL Panel IFF UPPER
 #define ILS_SW5 36  ///< ILS Channel 5
 #define ILS_SW6 37  ///< ILS Channel 6
 #define ILS_SW7 34  ///< ILS Channel 7
 #define ILS_SW8 35  ///< ILS Channel 8
 #define ILS_SW9 32  ///< ILS Channel 9
 #define ILS_SW10 33  ///< ILS Channel 10
 #define ILS_SW11 30  ///< ILS Channel 11
 #define ILS_SW12 31  ///< ILS Channel 12
 #define ILS_SW13 28  ///< ILS Channel 13
 #define ILS_SW14 29  ///< ILS Channel 14
 #define ILS_SW15 26  ///< ILS Channel 15
 #define ILS_SW16 27  ///< ILS Channel 16
 #define ILS_SW17 24  ///< ILS Channel 17
 #define ILS_SW18 25  ///< ILS Channel 18
 #define ILS_SW19 22  ///< ILS Channel 19
 #define ILS_SW20 23  ///< ILS Channel 20

// Connect switches to DCS-BIOS 

//Volume Knobs
DcsBios::Potentiometer comAux("COM_AUX", AUX_A);
DcsBios::Potentiometer comIcs("COM_ICS", 69);
DcsBios::Potentiometer comMidsA("COM_MIDS_A", MIDSA_A);
DcsBios::Potentiometer comMidsB("COM_MIDS_B", MIDSB_A);
DcsBios::Potentiometer comRwr("COM_RWR", RWR_A);
DcsBios::Potentiometer comTacan("COM_TACAN", TCN_A);
DcsBios::Potentiometer comVox("COM_VOX", VOX_A);
DcsBios::Potentiometer comWpn("COM_WPN", WPN_A);

//SWITCHES
DcsBios::Switch3Pos comCommGXmtSw("COM_COMM_G_XMT_SW", GXMT_SW1, GXMT_SW2);
DcsBios::Switch3Pos comCommRelaySw("COM_COMM_RELAY_SW", RLY_SW1, RLY_SW2);
DcsBios::Switch3Pos comCryptoSw("COM_CRYPTO_SW", IFFCRY_SW1, IFFCRY_SW2);
DcsBios::Switch2Pos comIffMasterSw("COM_IFF_MASTER_SW", IFFMAS_SW1);
DcsBios::Switch3Pos comIffMode4Sw("COM_IFF_MODE4_SW", MODE4_SW1, MODE4_SW2);
DcsBios::Switch2Pos comIlsUfcManSw("COM_ILS_UFC_MAN_SW", ILSUFC_SW1);

/**
* @brief ILS Rotary pin assignments for DCSBios Multi-position Switch.
*
*/
const byte comIlsChannelSwPins[20] = 
{ILS_SW1, ILS_SW2, ILS_SW3, ILS_SW4, ILS_SW5, ILS_SW6, ILS_SW7, 
ILS_SW8, ILS_SW9, ILS_SW10, ILS_SW11, ILS_SW12, ILS_SW13, ILS_SW14,
ILS_SW15, ILS_SW16, ILS_SW17, ILS_SW18, ILS_SW19, ILS_SW20};
/**
* @attention It is possible to spin the ILS rotary faster than the DCSBios debounce time, 
* causing some channels to be skipped over in the sim.  If you want to see each channel's click you may
* need to slow down.
* 
*/
DcsBios::SwitchMultiPos comIlsChannelSw("COM_ILS_CHANNEL_SW", comIlsChannelSwPins, 20);
 
//ANT SEL PANEL
DcsBios::Switch3Pos comm1AntSelectSw("COMM1_ANT_SELECT_SW", COMANT_SW1, COMANT_SW2);
DcsBios::Switch3Pos iffAntSelectSw("IFF_ANT_SELECT_SW", IFFANT_SW1, IFFANT_SW2);

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
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
