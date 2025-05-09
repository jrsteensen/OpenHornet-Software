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
 * @author Arribe, Ulukaii, Higgins
 * @date May 01, 2025
 * @version V 0.1 (tested)
 * @warning This sketch is based on OH-Interconnect. You may need to adapt it to your actual wiring and panel versions.
 * @brief Controls backlights & most annunciators of the cockpit. Coded for Arduino MEGA 2560 + ABSIS BACKLIGHT SHIELD.
 * @details
 * **Wiring Diagram:** 
 * 
 * | Pin | Function                      |
 * |-----|-------------------------------|
 * | 13  | J2 LIP_CH1                    |
 * | 12  | J3 LIP_CH2                    |
 * | 11  | J4 UIP_CH1                    |
 * | 10  | J5 UIP_CH2                    |
 * | 9   | J6 LC_CH1                     |
 * | 8   | J7 LC_CH2                     |
 * | 7   | J8 RC_CH1                     |
 * | 6   | J9 RC_CH2                     |
 * | 5   | J10 NC                        |
 * | 4   | J11 NC                        |
 * | 24  | J14 SIMPWR_BLM_A              |
 * | 23  | J14 SIMPWR_BLM_B              |
 * | 22  | J14 SIMPWR_BLM_PUSH           |
 * | SDA | TEMP SNSR                     |
 * | SCL | TEMP SNSR                     |
 * | 2   | J12 & J13 Cooling fan headers |
 */

/**********************************************************************************************************************
 * @brief Preprocessor directives 
 * @remark In particular:
 *         1. Check if we're on a Mega328 or Mega2560 and choose serial interface (Pro Micro: Default)
 *         2. Include external libraries FastLED and DcsBios
 *         3. Include the helper files (Panel, Colors, Channel...) and include the specific panel classes
 *********************************************************************************************************************/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "FastLED.h"
#include "DcsBios.h"
#include "helpers/Panel.h"
#include "helpers/Colors.h" 
#include "helpers/Channel.h"
#include "panels/1A2A1_MASTER_ARM.h"
#include "panels/1A4_L_EWI.h"
#include "panels/1A7A1_HUD_PANEL_REV4.h"            //Make sure to uncomment the correct HUD panel header file
//#include "panels/1A7A1_HUD_PANEL_REV3.h"          //Make sure to uncomment the correct include statement for your HUD panel
#include "panels/1A5_R_EWI.h"
#include "panels/1A6A1_SPN_RCVY.h"
#include "panels/2A2A1A6_ECM.h"
#include "panels/2A2A1A7_RWR_CONTROL.h"
#include "panels/2A2A1A8_STANDBY_INSTRUMENT.h"
#include "panels/4A1_LC_ALL_PANELS.h"
#include "panels/4A1_LC_Flood.h"


/********************************************************************************************************************
 * @brief Create the channel objects. Carefully check pinout corresponds to (YOUR!) wiring.
 * @remark The written number of LEDs per strip might exceed the actual number. This can be ignored.
 * @details Syntax: Channel <Name as on Interconnect>(hardware pin, "Channel name as on PCB", expected maxled count);
 ********************************************************************************************************************/

Channel LIP_1(13, "Channel 1", 100);
Channel LIP_2(12, "Channel 2", 120);
Channel UIP_1(11, "Channel 3", 210);
Channel UIP_2(10, "Channel 4", 210);
Channel LC_1(9, "Channel 5", 200);
Channel LC_2(5, "Channel 6", 233);
Channel RC_1(7, "Channel 7", 250);
Channel RC_2(6, "Channel 8", 380);
Channel AUX_1(8, "Channel 9", 300);
Channel AUX_2(4, "Channel 10", 300);

//Set up other variables
const uint8_t pin_EncoderSw =    24;              // Ulukaii deviation. Standard is 22
const uint8_t pin_EncoderA  =    22;              // Ulukaii deviation. Standard is 24
const uint8_t pin_EncoderB  =    23;                            


/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 ********************************************************************************************************************/
void setup() {
    // Initialize the LED strips
    LIP_1.initialize();
    LIP_2.initialize();
    UIP_1.initialize();
    UIP_2.initialize();
    LC_1.initialize();
    LC_2.initialize();
    RC_1.initialize();
    RC_2.initialize();
    AUX_1.initialize();
    AUX_2.initialize();
    FastLED.show();

    // Panel instantiations
    UIP_1.addPanel<MasterArmPanel>();
    UIP_1.addPanel<EwiPanel>();
    //UIP_1.addPanel<HudPanel>();
    UIP_1.addPanel<REwiPanel>();
    UIP_1.addPanel<SpnRcvyPanel>();

    LIP_2.addPanel<EcmPanel>();
    LIP_2.addPanel<RwrControlPanel>();
    LIP_2.addPanel<StandbyInstrumentPanel>();

    LC_1.addPanel<LcAllPanels>();
    LC_2.addPanel<LcFloodPanel>();

    // Run DCS Bios setup function
    DcsBios::setup();
}

void loop() {
  //Run DCS Bios loop function
  DcsBios::loop();
  
  // Call the updateLeds() method of the Panel class, which will update the LEDs if and only if any changes are pending
  Panel::updateLeds();
}
