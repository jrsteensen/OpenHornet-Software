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
 *          - When adding, adjust the panel order according to your physical connections.
 *          - Make sure that the pins defined below match your physical wiring.
 *          - There is no need to adapt any indexes or LED counts, anyhwere.
 *          If you want to change the colors:
 *          - Open the Colors.h file and change the color definitions.
 *          - Adapt individually as needed for your build and LEDs in use.
 *          - Colors are according MIL-STD-3099.
 *          - Note: LEDs dimming uses FastLED's nscale8_video() function. It provides a
 *            more color-preserving dimming effect than pure RGB value recalculation.
 *          If you have an non-standard wiring / pinout of your backlight controller:
 *          - Adapt the pinout in the "Define pinouts and channels" section in this file.
 *          If you are using custom panels: 
 *          - Adapt or create a new panel class in the "panels" folder; 
 *            you may use the 1A2A1_MASTER_ARM.h as template. 
 *          - Make sure to use the same concepts (Inherit from panel class, singleton, 
 *            PROGMEM etc.) as the other panels. Creating of a LED_TEXT table is optional.
 *          - Add the panel to a channel in the setup() function.
 *          - Adapt the max LED count of the affected channel as needed in the 
 *            "Define pinouts and channels" section.
 * 
 *          **Technical Background**
 *          This sketch addresses the following functional OH requirements:
 *          1. Enable control of OH backlights and indicators with DCS-BIOS.
 *          2. Enable control of OH backlights manually, if DCS-BIOS is not available.
 *          3. Enable test of all backlights and indicators with a rainbow pattern.
 *          4. Backlights and indicators share the same LED strip.
 *          5. Different users may build a different subset of panels and connect them 
 *             in varying orders as their build progresses; the code must be modular
 *             enough to allow this. 
 *          6. As the users' build progresses:
 *             - She/he shall not needc to recalculate LED indices/counts
 *             - She/he shall not need to reprogram this BL controller (if build to spec)
 *          7. Code execution must be fast to avoid LED flickering (output side) or loss 
 *              of updates coming from DCS-BIOS (input side).
 *          8. Extensible for a future use of the PREFLT function (feasibility TBD)
 *          This sketch respects the following technical  limitations:
 *          A. DCS-BIOS callbacks expect 'static' functions; dynamic alloc not allowed.
 *          B. Arduino MEGA 2560 has 8KB of SRAM.
 *          C. FastLED.show() command is time-sensitive 
 *          Solutions:
 *          - An OOP programming paradigm is used, as functions may repeat across panels
 *          - The generic Panel class (Panel.h) is the base class for all panels 
 *            Each panel must be inherit from it.
 *          - For each panel in the pit, one panel class exists in the panel folder.
 *          - These panels classes encapsulate all there is to know (LED roles, DCS-BIOS
 *            methods) about the panel in one spot. In particular, they:
 *            (1) inherit the common behaviour from the generic Panel class;
 *            (2) contain a table with LED indices (panel-local) and their roles;
 *            (3) provide a method to recalculate panel-local indices to strip indices;
 *            (4) additionally implement DCS-BIOS callbacks unique to this panel;
 *            (5) are singletons for compatibility with DCS-BIOS callbacks (rqrmt 5, 6)
 *          - Channels class implements logical LED strip mgmt (rqrmt 4, 7)
 *          - Board class implements mode change logic and mode control (= steer what 
 *            happens during a loop of the Arduino) (rqrmt 1, 2, 3)
 *          - LedUpdateState class implements central point for LED update calls,
 *            ensuring high performance (rqrmt 6)
 *          - LedStruct class prepares LedText struct for future PREFLT function (rqrmt 8)
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
const int encSw =    22;              
const int encA  =    24;              
const int encB  =    23;  

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
Channel LIP_1(12, "Channel 1", LIP_1_leds, 100);
Channel LIP_2(13, "Channel 2", LIP_2_leds, 120);
Channel UIP_1(11, "Channel 3", UIP_1_leds, 210);
Channel UIP_2(9, "Channel 4", UIP_2_leds, 210);                                   
Channel LC_1(10, "Channel 5", LC_1_leds, 304);                        //Ulukaii deviation. Standard is 9.              
Channel LC_2(5, "Channel 6", LC_2_leds, 150);
Channel RC_1(7, "Channel 7", RC_1_leds, 170);               
Channel RC_2(6, "Channel 8", RC_2_leds, 266);               
Channel AUX_1(8, "Channel 9", AUX_1_leds, 100);                       //Spare channel
Channel AUX_2(4, "Channel 10", AUX_2_leds, 100);                      //Spare channel

Board* board;                                                         // Pointer to singleton board instance


/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 *         Note that the call to DCS-Bios::loop() is done in the Board.h, where the mode logic resides.
 ********************************************************************************************************************/
void setup() {
    board = Board::getInstance();                                     // Get board instance
    board->initializeBoard(encSw, encA, encB);                        // Initialize board with encoder pins
    
    // Initialize all channels
    LIP_1.initialize();                                               // Initialize channels with FastLED
    LIP_2.initialize();
    UIP_1.initialize();
    UIP_2.initialize();
    LC_1.initialize();
    LC_2.initialize();
    RC_1.initialize();
    RC_2.initialize();
    AUX_1.initialize();
    AUX_2.initialize();

    // Register all channels with the board
    board->registerChannel(&LIP_1);                                   // Register channels with the board
    board->registerChannel(&LIP_2);
    board->registerChannel(&UIP_1);
    board->registerChannel(&UIP_2);
    board->registerChannel(&LC_1);
    board->registerChannel(&LC_2);
    board->registerChannel(&RC_1);
    board->registerChannel(&RC_2);
    board->registerChannel(&AUX_1);
    board->registerChannel(&AUX_2);

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

    RC_1.addPanel<LdgChecklistPanel>();
    RC_1.addPanel<RadarAltPanel>();
    RC_1.addPanel<HydPressGauge>();
    RC_1.addPanel<CautionPanel>();
    RC_1.addPanel<Rc1AllRemainingPanels>();
    RC_2.addPanel<Rc2AllPanels>();

    AUX_1.addPanel<LcFloodPanel>();

    FastLED.show();                                                   // Show the LEDs
    DcsBios::setup();                                                 // Run DCS Bios setup function
}

void loop() {
    board->handleModeChange();                                        // Handle mode changes
    board->processMode();                                             // Process current mode, incl. DCS-Bios::loop()
    board->updateLeds();                                              // Update LEDs as needed
}
