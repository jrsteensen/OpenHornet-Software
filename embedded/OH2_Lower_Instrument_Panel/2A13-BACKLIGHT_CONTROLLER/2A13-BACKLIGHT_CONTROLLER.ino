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
 *          If you are building according to spec, you only need to work with this file:
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
 *          - Make sure to use the same concepts (Inherit from panel class, singleton, PROGMEM etc.) as the other panels.
 *          - Add the panel to the setup() function.
 *          - Adapt the max LED count of the affected channel as needed in the "Define pinouts and channels" section.
 * 
 *          **Technical Background**
 *          This sketch addresses the following OH requirements:
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
 *          - The generic Panel class (Panel.h) is the base class for all panels. Each panel must be inherit from it.
 *          - For each panel in the pit, there is one specific panel class in the panels folder.
 *          - These panels classes (1) inherit the common behaviour from the generic Panel class;
 *                                 (2) additionally implement DCS-BIOS callbacks that are unique to this panel
 *                                 (3) adhere to a singleton pattern for compatibility with DCS-BIOS callbacks (rqrmt 8)
 *          - Panels classes store their LED role info in PROGMEM to save SRAM, of which only 8KB are available
 *          - Panel classes encapsulate all there is to know (LED roles, DCS-BIOS methods) about them in one spot
 *          - Channels class implements logical LED strip mgmt and automatic indices calculation (rqrmt 4, 5, 7)
 *          - Board class implements the main logic and handles mode changes (rqrmt 1, 2, 3)
 *          - LedUpdateState class implements central point for LED update calls, ensuring high performance (rqrmt 6)
 *          - LedStruct class prepares LedText struct for future PREFLT function (rqrmt 9)
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
#include "panels/2A2A1A3_IFEI_Panel.h"
#include "panels/2A2A1A3_Video_Record_Panel.h"
#include "panels/2A2A1A1_Jett_Station_Panel.h"


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

// Static LED arrays for each channel
CRGB LIP_1_leds[100];    // 100 LEDs
CRGB LIP_2_leds[120];    // 120 LEDs
CRGB UIP_1_leds[210];    // 210 LEDs
CRGB UIP_2_leds[210];    // 210 LEDs
CRGB LC_1_leds[304];     // 304 LEDs
CRGB LC_2_leds[150];     // 150 LEDs
CRGB RC_1_leds[170];     // 170 LEDs
CRGB RC_2_leds[266];     // 266 LEDs
CRGB AUX_1_leds[100];    // 100 LEDs
CRGB AUX_2_leds[100];    // 100 LEDs

// Define channel objects (and create them)
Channel LIP_1(13, "Channel 1", LIP_1_leds, 100);
Channel LIP_2(12, "Channel 2", LIP_2_leds, 120);
Channel UIP_1(11, "Channel 3", UIP_1_leds, 210);
Channel UIP_2(9, "Channel 4", UIP_2_leds, 210);                                   
Channel LC_1(10, "Channel 5", LC_1_leds, 304);                                   //Ulukaii deviation. Standard is 9.              
Channel LC_2(5, "Channel 6", LC_2_leds, 150);
Channel RC_1(7, "Channel 7", RC_1_leds, 170);               
Channel RC_2(6, "Channel 8", RC_2_leds, 266);               
Channel AUX_1(8, "Channel 9", AUX_1_leds, 100);                                   //Spare channel
Channel AUX_2(4, "Channel 10", AUX_2_leds, 100);                                  //Spare channel

Board* board;                                                         // Pointer to singleton board instance


/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 ********************************************************************************************************************/
void setup() {
    board = Board::getInstance();                                      // Get board instance
    board->initialize(encSw, encA, encB);                             // Initialize board with encoder pins
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

    UIP_1.addPanel<MasterArmPanel>();                                 // Instantiate the panels;
    UIP_1.addPanel<EwiPanel>();                                       // Adapt order according to your physical wiring; 
    //UIP_1.addPanel<HudPanel>();                                     // Do not exceed the channel's LED count defined above.
    UIP_1.addPanel<REwiPanel>();
    UIP_1.addPanel<SpnRcvyPanel>();

    LIP_1.addPanel<IfeiPanel>();
    LIP_1.addPanel<VideoRecordPanel>();
    LIP_1.addPanel<JettStationPanel>();

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
    board->handleModeChange();                                        // Handle mode changes
    board->processMode();                                             // Process current mode
    board->updateLeds();                                              // Update LEDs as needed
}
