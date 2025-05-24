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
 *   Copyright 2016-2025 OpenHornet
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
 * @file    2A13-BACKLIGHT_CONTROLLER.ino
 * @author  Ulukaii, Arribe, Higgins
 * @date    May 22, 2025
 * @version V 0.3.2 ( partially tested)
 * @warning This sketch is based on OH-Interconnect. Adapt it to your actual wiring and panel versions.
 * @brief   Controls backlights & most annunciators. Coded for Arduino MEGA 2560 + ABSIS BACKLIGHT SHIELD.
 * @details **Wiring Diagram:** 
 * 
 *          | Pin | Function                      |
 *          |-----|-------------------------------|
 *          | 13  | J2 LIP_CH1                    |
 *          | 12  | J3 LIP_CH2                    |
 *          | 11  | J4 UIP_CH1                    |
 *          | 10  | J5 UIP_CH2                    |
 *          | 9   | J6 LC_CH1                     |
 *          | 8   | J7 LC_CH2                     |
 *          | 7   | J8 RC_CH1                     |
 *          | 6   | J9 RC_CH2                     |
 *          | 5   | J10 NC                        |
 *          | 4   | J11 NC                        |
 *          | 24  | J14 SIMPWR_BLM_A              |
 *          | 23  | J14 SIMPWR_BLM_B              |
 *          | 22  | J14 SIMPWR_BLM_PUSH           |
 *          | SDA | TEMP SNSR                     |
 *          | SCL | TEMP SNSR                     |
 *          | 2   | J12 & J13 Cooling fan headers |
 */

/**********************************************************************************************************************
 * @brief  Preprocessor directives & library includes

 *********************************************************************************************************************/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)        // Check if we're on a Mega328 or Mega2560
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif
#ifdef __AVR__                                                        // Include AVR power library if we're on an AVR
  #include <avr/power.h>
#endif
#define FASTLED_INTERRUPT_RETRY_COUNT 1                               // Define the number of retries for the interrupt

#include "FastLED.h"
#include "DcsBios.h"
#include "RotaryEncoder.h"
#include "helpers/Panel.h"
#include "helpers/Channel.h"
#include "helpers/Colors.h"
#include "helpers/Board.h"
#include "panels/1A2A1_MASTER_ARM.h"
#include "panels/1A4_L_EWI.h"
#include "panels/1A7A1_HUD_PANEL_REV4.h"            
#include "panels/1A7A1_HUD_PANEL_REV3.h"          
#include "panels/1A5_R_EWI.h"
#include "panels/1A6A1_SPN_RCVY.h"
#include "panels/2A2A1A6_ECM.h"
#include "panels/2A2A1A7_RWR_CONTROL.h"
#include "panels/2A2A1A8_STANDBY_INSTRUMENT.h"
#include "panels/4A2A1_LDG_GEAR_PANEL.h"
#include "panels/4A3A1_SELECT_JETT_PANEL.h"
#include "panels/4A1_LC_ALL_PANELS.h"
#include "panels/4A1_LC_Flood.h"
#include "panels/5A2A7_LDG_CHECKLIST.h"
#include "panels/5A2A4_RADAR_ALT.h"
#include "panels/5A4A1_HYD_PRESS.h"
#include "panels/5A3A1_CAUTION.h"
#include "panels/5A2A3_RC1_ALL_REMAINING_PANELS.h"
#include "panels/5A5_RC2_ALL_PANELS.h"

// Hardware pin definitions
const uint8_t encSw =    22;              
const uint8_t encA  =    24;              
const uint8_t encB  =    23;  

// Color definitions according MIL-STD-3099. Adapt to your visual preferences.
#define NVIS_YELLOW  CRGB(172, 144, 0)                                // Yellow Indicators
#define NVIS_RED     CRGB(158, 4, 4)                                  // Red Indicators
#define NVIS_GREEN_A CRGB(51, 102, 0)                                 // Green Backlighting
#define NVIS_GREEN_B CRGB(85, 138, 0)                                 // Green indicators
#define NVIS_WHITE   CRGB(40, 40, 30)                                 // Dimmed white, e.g. for Jett Station Select toggle light
#define NVIS_BLACK   CRGB(0, 0, 0)                                    // No colour / OFF


/********************************************************************************************************************
 * @brief   Create the channel objects. 
 * @remark  Check that the pinout corresponds to (YOUR!) wiring.
 * @details Syntax: Channel <Name as on Interconnect>(hardware pin, "Channel name as on PCB", expected max. led count);
 *          When adapting below code, observe memory constraints. Each LED uses 3 bytes of SRAM.
 ********************************************************************************************************************/

Channel LIP_1(13, "Channel 1", 100);
Channel LIP_2(12, "Channel 2", 120);
Channel UIP_1(11, "Channel 3", 210);
Channel UIP_2(9, "Channel 4", 210);                                   // Ulukaii deviation. Specification: pin 10
Channel LC_1(10, "Channel 5", 304);              
Channel LC_2(5, "Channel 6", 150);
Channel RC_1(7, "Channel 7", 170);               
Channel RC_2(6, "Channel 8", 266);               
Channel AUX_1(8, "Channel 9", 100);                                   //Spare channel
Channel AUX_2(4, "Channel 10", 100);                                  //Spare channel

Board* board;                                                         // Pointer to singleton board instance
RotaryEncoder encoder(encA, encB, RotaryEncoder::LatchMode::TWO03);


/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 ********************************************************************************************************************/
void setup() {
    board = Board::getInstance();                                      // Get board instance
    board->initAndRegisterChannel(&LIP_1);                            // Initialize channels and register them
    board->initAndRegisterChannel(&LIP_2);
    board->initAndRegisterChannel(&UIP_1);
    board->initAndRegisterChannel(&UIP_2);
    board->initAndRegisterChannel(&LC_1);
    board->initAndRegisterChannel(&LC_2);
    board->initAndRegisterChannel(&RC_1);
    board->initAndRegisterChannel(&RC_2);
    board->initAndRegisterChannel(&AUX_1);
    board->initAndRegisterChannel(&AUX_2);

    pinMode(encSw, INPUT_PULLUP);                             // Initialize mode change pin

    UIP_1.addPanel<MasterArmPanel>();                                 // Instantiate the panels;
    UIP_1.addPanel<EwiPanel>();                                       // Adapt order according to your physical wiring; 
    //UIP_1.addPanel<HudPanel>();                                     // Do not exceed the channel's LED count defined above.
    UIP_1.addPanel<REwiPanel>();
    UIP_1.addPanel<SpnRcvyPanel>();
    LIP_2.addPanel<EcmPanel>();
    LIP_2.addPanel<RwrControlPanel>();
    LIP_2.addPanel<StandbyInstrumentPanel>();
    LC_1.addPanel<LdgGearPanel>();
    LC_1.addPanel<SelectJettPanel>();
    LC_1.addPanel<LcAllPanels>();
    LC_2.addPanel<LcFloodPanel>();
    RC_1.addPanel<LdgChecklistPanel>();
    RC_1.addPanel<RadarAltPanel>();
    RC_1.addPanel<HydPressGauge>();
    RC_1.addPanel<CautionPanel>();
    RC_1.addPanel<Rc1AllRemainingPanels>();
    RC_2.addPanel<Rc2AllPanels>();

    FastLED.show();                                                   // Show the LEDs
    DcsBios::setup();                                                 // Run DCS Bios setup function
}

void loop() {
    int currentMode = board->handleModeChange(encSw);         // Handle mode changes and get current mode
    static int rotary_pos = 0;                                        // Static to persist between loop iterations
    int newPos = 0;                                                  // Declare outside switch
    
    switch(currentMode) {
        case Board::MODE_NORMAL:                                      //LEDs controlled by DCS BIOS
            DcsBios::loop();
            break;
        case Board::MODE_MANUAL:                                      //LEDs controlled manually through BKLT switch
            board->fillSolid(NVIS_GREEN_A);
            encoder.tick();
            newPos = encoder.getPosition();
            if (newPos != rotary_pos) {
                RotaryEncoder::Direction direction = encoder.getDirection();
                if (direction == RotaryEncoder::Direction::CLOCKWISE) {
                    board->incrBrightness();
                } else {
                    board->decrBrightness();
                }
                rotary_pos = newPos;
            }
            break;   
        case Board::MODE_RAINBOW:                                     //Rainbow test mode
            board->fillRainbow();
            break;
    }

    board->updateLeds();                                             // Update LEDs as needed
}
