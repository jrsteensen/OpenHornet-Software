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
 * @warning This sketch is based on OH-Interconnect. Adapt it to your actual wiring and 
 *          panel versions.
 * @brief   Controls backlights & most annunciators. 
 * @details **Intended Board**
 *          For Arduino MEGA 2560 + ABSIS BACKLIGHT SHIELD.

 *          **Wiring Diagram:** 
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
 *   
 *          **How to use**
 *          If you are building according to spec, you only need to review this file:
 *          - In the setup() function, add only those panels that you are using.
 *          - Make sure to add the panels to the channel and in the order that you have physically connected them.
 *          - There is no need to adapt any indexes or LED counts, anyhwere.
 *          If you want to change the colors:
 *          - Open the Colors.h file and change the color definitions.
 *          - Colors are according MIL-STD-3099. Adapt individually as needed for your build and LEDs in use.
 *          - Note that methods that are dimming these LEDs are using FastLED's nscale8_video() function.
 *            This function provides a more color-preserving dimming effect than pure RGB value recalculation.
 *          If you have an non-standard wiring / pinout of your backlight controller:
 *          - Adapt the pinout in the "Define pinouts and channels" section in this file.
 *          If you are using custom panels: 
 *          - Adapt or create a new panel class in the "panels" folder. You may use the 1A2A1_MASTER_ARM.h as template. 
 *          - Make sure to use the same class structure (Inherit from panel class) as the other panels.
 *          - Add the panel to the setup() function.
 *          - Adapt the max LED count of the affected channel as needed in the "Define pinouts and channels" section.
 * 
 *          **Technical Background**
 *          This sketch / codebase is result of matching OH requirements into a constrained Arduino environment.
 *          It addresses the following requirements:
 *          1. Enable control of OH backlights and indicators with DCS-BIOS.
 *          2. Enable control of OH backlights manually with a rotary encoder, if no DCS-BIOS connection is available.
 *          3. Provide a function to test all backlights and indicators with a rainbow pattern.
 *          4. Backlights and indicators share the same LED strip.
 *          5. Not all panels are used in all builds; the code must be modular enough to allow for this.
 *          6. Fast code execution to avoid LED flickering (output side) or dropping DCS-BIOS updates (input side).
 *          7. Use shall not need to recalculate LED indices or LED counts as their builds evolve.
 *          8. DCS-BIOS callbacks expect to call 'static' functions - no dynamic allocation is allowed.
 *          9. Extensible for a future use of the PREFLT function (feasibility study ongoing).
 *          Solutions:
 *          - An OOP programming paradigm is used, as many functions are repeating across panels.
 *          - Panels classes adhere to a singleton pattern to make them compatible with DCS-BIOS callbacks (addresses: rqrmt 8)
 *          - Panels classes store their LED role info in PROGMEM to save SRAM, of which only 8KB are available
 *          - Channels class implements logical LED strip mgmt and automatic indices calculation (addresses: rqrmt 4, 5, 7)
 *          - Board class implements the main logic and handles mode changes (addresses: rqrmt 1, 2, 3)
 *          - LedUpdateState class implements central point for LED update calls, ensuring high performance (addresses: rqrmt 6)
 *          - LedStruct class prepares LedText struct for future PREFLT function (addresses: rqrmt 9)
 */

 /*************************************************************************************/

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


/********************************************************************************************************************
 * @brief   Define pinouts and channels 
 * @remark  Check that the pinout corresponds to (YOUR!) wiring.
 * @details Syntax: Channel <Name as on Interconnect>(hardware pin, "Channel name as on PCB", expected max. led count);
 *          When adapting below code, observe memory constraints. Each LED uses 3 bytes of SRAM. 8KB are available.
 ********************************************************************************************************************/

// Hardware pin definitions
const uint8_t encSw =    22;              
const uint8_t encA  =    24;              
const uint8_t encB  =    23;  

// Define channel objects (and create them)
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
    board->initAndRegisterChannel(&LIP_1);                            // Initializing channels will block RAM according 
    board->initAndRegisterChannel(&LIP_2);                            //   to max LED count
    board->initAndRegisterChannel(&UIP_1);
    board->initAndRegisterChannel(&UIP_2);
    board->initAndRegisterChannel(&LC_1);
    board->initAndRegisterChannel(&LC_2);
    board->initAndRegisterChannel(&RC_1);
    board->initAndRegisterChannel(&RC_2);
    board->initAndRegisterChannel(&AUX_1);
    board->initAndRegisterChannel(&AUX_2);

    pinMode(encSw, INPUT_PULLUP);                                     // Initialize mode change pin

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
    int currentMode = board->handleModeChange(encSw);                 // Handle mode changes and get current mode
    static int rotary_pos = 0;                                        // Static to persist between loop iterations
    int newPos = 0;                                                   // Declare outside switch
    
    switch(currentMode) {
        case Board::MODE_NORMAL:                                      // LEDs controlled by DCS BIOS
            DcsBios::loop();
            break;
        case Board::MODE_MANUAL:                                      // LEDs controlled manually through BKLT switch
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
                board->fillSolid(NVIS_GREEN_A);                       // Only call fillSolid when brightness changes
            }
            break;   
        case Board::MODE_RAINBOW:                                     //Rainbow test mode
            board->fillRainbow();
            break;
    }

    board->updateLeds();                                              // Update LEDs as needed
}
