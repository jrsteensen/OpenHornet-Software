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
 * @date    October 12, 2025
 * @version V 0.4.5 (tested)
 * @warning This sketch is based on OH-Interconnect. Adapt it to your actual wiring and 
 *          panel versions.
 * @brief   Controls backlights & most annunciators. 
 * @details **Intended Board**
 *          For Arduino MEGA 2560 + ABSIS BACKLIGHT SHIELD.

 *          **Wiring Diagram:** 
 * 
 *          | Pin | Function                      | Channel on PCB   |
 *          |-----|-------------------------------|------------------|
 *          | 13  | J2 LIP_CH1                    | Channel 1        |
 *          | 12  | J3 LIP_CH2                    | Channel 2        |
 *          | 11  | J4 UIP_CH1                    | Channel 3        |
 *          | 10  | J5 UIP_CH2                    | Channel 4        |
 *          | 9   | J6 LC_CH1                     | Channel 5        |
 *          | 8   | J7 LC_CH2                     | Channel 6        |
 *          | 7   | J8 RC_CH1                     | Channel 7        |
 *          | 6   | J9 RC_CH2                     | Channel 8        |
 *          | 5   | J10 LC_FLOOD                  | Channel 9        |
 *          | 4   | J11 RC_FLOOD                  | Channel 10       |
 *          | 22  | J14 SIMPWR_BLM_A              |                  |
 *          | 23  | J14 SIMPWR_BLM_B              |                  |
 *          | 24  | J14 SIMPWR_BLM_PUSH           |                  |
 *          | SDA | TEMP SNSR                     |                  |
 *          | SCL | TEMP SNSR                     |                  |
 *          | 2   | J12 & J13 Cooling fan headers |                  |
 *  
 * 
 *          **User guide**
 *          The BLM will start in mode 1 (DCS-BIOS) mode by default. This means you should
 *          see only dark panels. By pressing the rotary encoder, you can switch to mode 2
 *          (manual mode) and see manually dimmable backlights in green. Pressing again, 
 *          you should see a rainbow pattern. Pressing again, you get back to mode 1.
 *
 *
 *          **Configuration guide**
 *          (1) If you are building according to spec:
 *              ...you only need to work with this file (the main .ino). Generally, the 
 *              code in works as follows: at power up, three kinds of objects 
 *              are created: one Board() object -the top-level object - that 
 *              represents the BLM board itself. Ten Channel() objects, each of which 
 *              represent on of the ten BL channels. And many Panel() objects, each of 
 *              which represents a specific panel in the pit.
 *              The board object handles the change between the three modes (normal, 
 *              manual, rainbow) that you can invoke with the rotary encoder. It also 
 *              handles the brightness control for the normal mode. Therefore, 
 *              it is the interface to the user.
 *              Each channel object handles one LED strip and FastLED commands. Therefore, 
 *              they are the interface to the hardware.
 *              The panel objects represent cockpit panels and 'know' which of their LED
 *              has which role (e.g. backlighting or specific indicators such as "FIRE").
 *              They also handle DCS-Bios commands. Therefore, they are the interface to 
 *              DCS-BIOS.
 *              In the setup() function, add only those panels that you are using. When 
 *              adding, adjust the panel order according to your physical connections. 
 *              Comment out the panels that you do not have connected. You do not need to 
 *              adapt any LED indices or LED counts, or other files, anywhere. Also make 
 *              sure that the pins defined below match your physical wiring.
 *          (2) If you add panels in a non-standard order: 
 *              Adapt the panel order in the setup() function as described in (1). If you
 *              intend to add more panels to a channel than the default, make sure that you
 *              increase the LED count of the affected channel (e.g. LIP_1_LED_COUNT) in 
 *              the "Define pinouts and channels" section appropriately.
 *          (3) If you want to change the colors: 
 *              Open the Colors.h file and change the color definitions. Observe that in
 *              this file, the colours according to MIL-STD-3099 are documented. However,
 *              these may not fit exactly to the visual output that your LEDs provide. 
 *              Therefore, adapt colors as needed for your build and LEDs in use.
 *              Note: LEDs dimming uses FastLED's nscale8_video() function. It provides a
 *              more color-preserving dimming effect than pure RGB value recalculation.
 *          (4) If you have an non-standard wiring / pinout of your backlight controller:
 *              Adapt the pinout in the "Define pinouts and channels" section in this file.
 *          (5) If you are using custom panels: 
 *              Adapt or create a new panel class in the "panels" folder. You may use the 
 *              1A2A1_MASTER_ARM.h as template. Make sure to follow its structure closely.
 * 
 *          **Troubleshooting**
 *          (!) If the panels stay dark:
 *              First confirm the correct wiring of the rotary  encoder. Second, observe
 *              if the onboard LED is blinking fast. If so, this means that the power-on
 *              self-test has detected an exceedance of  panels added to a channel. In 
 *              this case, you need to increase the LED count of the affected channel in 
 *              the "Define pinouts and channels" section.
 *              If the onboard LED is not blinking, the code failed to initialize and 
 *              start up properly. Revert the last changes.
 *          (!) If the BLM does not react after a while of operation: 
 *              I observed that a faulty LED on a panel can cause the BLM to stop working 
 *              after a few mins of operation. The faulty LED is usually the first LED on 
 *              a channel that stays dark. Replacing the faulty LED should fix the issue.
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
 *          6. As the users' build progresses, and more panels are added, the user shall 
 *             not need to recalculate LED indices/counts. She/he shall not need to 
 *             reprogram this BL controller (if build to spec)
 *          7. Code execution must be fast to avoid LED flickering (output side) or loss 
 *              of updates coming from DCS-BIOS (input side).
 *          8. Extensible for a future use of the PREFLT function (feasibility TBD)
 *          This sketch respects the following technical  limitations:
 *          A. DCS-BIOS callbacks expect 'static' functions; dynamic alloc not allowed.
 *          B. Arduino MEGA 2560 has 8KB of SRAM.
 *          C. FastLED.show() command is time-sensitive 
 *          Solutions:
 *          - An OOP programming paradigm is used, as functionalities repeat across panels
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
#define FASTLED_INTERRUPT_RETRY_COUNT 1                               // Define the number of retries for FastLED update
#define DCSBIOS_DISABLE_SERVO                                         // Disable DCS-BIOS servo support (not used)

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
#include "panels/4A4A1_FIRE_TEST_PANEL.h"
#include "panels/10A1A1_SIM_PWR_PANEL.h"
#include "panels/4A4A2_GEN_TIE_PANEL.h"
#include "panels/4A4A2_EXT_LTS_PANEL.h"
#include "panels/4A5A1_FUEL_PANEL.h"
#include "panels/4A1_LC2_ALL_PANELS.h"
#include "panels/4A1_LC_Flood.h"
#include "panels/5A1_RC_Flood.h"
#include "panels/5A2A7_LDG_CHECKLIST.h"
#include "panels/5A2A4_RADAR_ALT.h"
#include "panels/5A4A1_HYD_PRESS.h"
#include "panels/5A3A1_CAUTION.h"
#include "panels/5A2A3_RC1_ALL_REMAINING_PANELS.h"
#include "panels/5A5_RC2_ALL_PANELS.h"
#include "panels/2A2A1A3_IFEI_Panel.h"
#include "panels/2A2A1A3_Video_Record_Panel.h"
#include "panels/2A2A1A1_Jett_Station_Panel.h"
#include "panels/2A2A1A4_Jett_Placard_Panel.h"


/********************************************************************************************************************
 * @brief   Define pinouts, LED counts and LED channels 
 * @remark  Check that the pinout corresponds to (YOUR!) wiring.
 * @details Syntax: Channel <Name as on Interconnect>(hardware pin, "Channel name as on PCB", expected max. led count);
 *          When adapting below code, observe memory constraints. Each LED uses 3 bytes of SRAM. 8KB are available.
 ********************************************************************************************************************/


// Voltage and current definitions. Adapt to your ATX PSU specs.
const int VOLTAGE = 5;
const int MAX_MILLIAMPS = 20000;

// Hardware pin definitions
const int encSw =    24;              
const int encA  =    22;              
const int encB  =    23;  

// LED counts for each channel
const int LIP_1_LED_COUNT = 100;
const int LIP_2_LED_COUNT = 120;
const int UIP_1_LED_COUNT = 210;
const int UIP_2_LED_COUNT = 210;
const int LC_1_LED_COUNT = 250;
const int LC_2_LED_COUNT = 215;
const int RC_1_LED_COUNT = 171;
const int RC_2_LED_COUNT = 266;
const int LC_FLOOD_LED_COUNT = 100;
const int RC_FLOOD_LED_COUNT = 100;

// Static LED arrays for each channel, using the LED counts defined abv
CRGB LIP_1_leds[LIP_1_LED_COUNT];    
CRGB LIP_2_leds[LIP_2_LED_COUNT];    
CRGB UIP_1_leds[UIP_1_LED_COUNT];    
CRGB UIP_2_leds[UIP_2_LED_COUNT];    
CRGB LC_1_leds[LC_1_LED_COUNT];     
CRGB LC_2_leds[LC_2_LED_COUNT];     
CRGB RC_1_leds[RC_1_LED_COUNT];     
CRGB RC_2_leds[RC_2_LED_COUNT];     
CRGB LC_FLOOD_leds[LC_FLOOD_LED_COUNT];    // 100 LEDs
CRGB RC_FLOOD_leds[RC_FLOOD_LED_COUNT];    // 100 LEDs

// Create objects of "channel" class (with the right LED count)
Channel LIP_1(13, "Channel 1", LIP_1_leds, LIP_1_LED_COUNT);
Channel LIP_2(12, "Channel 2", LIP_2_leds, LIP_2_LED_COUNT);
Channel UIP_1(11, "Channel 3", UIP_1_leds, UIP_1_LED_COUNT);
Channel UIP_2(10, "Channel 4", UIP_2_leds, UIP_2_LED_COUNT);                                   
Channel LC_1(9, "Channel 5", LC_1_leds, LC_1_LED_COUNT);                                      
Channel LC_2(8, "Channel 6", LC_2_leds, LC_2_LED_COUNT);
Channel RC_1(7, "Channel 7", RC_1_leds, RC_1_LED_COUNT);               
Channel RC_2(6, "Channel 8", RC_2_leds, RC_2_LED_COUNT);               
Channel LC_FLOOD(5, "Channel 9", LC_FLOOD_leds, LC_FLOOD_LED_COUNT);           
Channel RC_FLOOD(4, "Channel 10", RC_FLOOD_leds, RC_FLOOD_LED_COUNT);          

// Create pointer to singleton board instance
Board* board;                                                         


/********************************************************************************************************************
 * @brief Standard Arduino setup and loop functions.
 * @remark Setup runs once, loop runs continuously. Conversion CRGB --> GRB is done by FastLED.
 *         Note that the call to DCS-Bios::loop() is done in the Board.h, where the mode logic resides.
 ********************************************************************************************************************/
void setup() {
    board = Board::getInstance();                                     // Get board instance
    board->setupRotaryEncoder(encSw, encA, encB);                    // Set up rotary encoder with switch and encoder pins
    
    // Initialize all channels
    LIP_1.initialize();                                               // Calling .initialize() on a channel object will
    LIP_2.initialize();                                               // trigger FastLED.addLeds() with pin and led count
    UIP_1.initialize();                                               
    UIP_2.initialize();                                               
    LC_1.initialize();
    LC_2.initialize();
    RC_1.initialize();
    RC_2.initialize();
    LC_FLOOD.initialize();
    RC_FLOOD.initialize();

    board->registerChannel(&LIP_1);                                   // Register channels with the board, so they are
    board->registerChannel(&LIP_2);                                   // accessible by the board object
    board->registerChannel(&UIP_1);
    board->registerChannel(&UIP_2);
    board->registerChannel(&LC_1);
    board->registerChannel(&LC_2);
    board->registerChannel(&RC_1);
    board->registerChannel(&RC_2);
    board->registerChannel(&LC_FLOOD);
    board->registerChannel(&RC_FLOOD);

    UIP_1.addPanel<MasterArmPanel>();                                 // Instantiate the panels;
    UIP_1.addPanel<EwiPanel>();                                       // Adapt order according to your physical wiring; 
    //UIP_1.addPanel<HudPanel>();                                     // Do not exceed the channel's LED count defined above.
    //UIP_1.addPanel<HudPanelRev3>();
    UIP_1.addPanel<REwiPanel>();
    UIP_1.addPanel<SpnRcvyPanel>();

    LIP_1.addPanel<IfeiPanel>();
    LIP_1.addPanel<VideoRecordPanel>();
    LIP_1.addPanel<JettStationPanel>();
    LIP_1.addPanel<JettPlacardPanel>();

    LIP_2.addPanel<EcmPanel>();
    LIP_2.addPanel<RwrControlPanel>();
    LIP_2.addPanel<StandbyInstrumentPanel>();

    LC_1.addPanel<LdgGearPanel>();
    LC_1.addPanel<SelectJettPanel>();
    LC_1.addPanel<FireTestPanel>();
    LC_1.addPanel<SimPwrPanel>();
    LC_1.addPanel<GenTiePanel>();
    LC_1.addPanel<ExtLtsPanel>();
    LC_1.addPanel<FuelPanel>();
    
    LC_2.addPanel<Lc2AllPanels>();

    RC_1.addPanel<LdgChecklistPanel>();
    RC_1.addPanel<RadarAltPanel>();
    RC_1.addPanel<HydPressGauge>();
    RC_1.addPanel<CautionPanel>();
    RC_1.addPanel<Rc1AllRemainingPanels>();
    RC_2.addPanel<Rc2AllPanels>();

    LC_FLOOD.addPanel<LcFloodLights>();
    RC_FLOOD.addPanel<RcFloodLights>();

    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTAGE, MAX_MILLIAMPS);   // Set the maximum power in volts and milliamps
    FastLED.setMaxRefreshRate(100);                                   // Set the maximum refresh rate to 100 Hz instead of std. 400 Hz. Slightly reduces CPU load.
    FastLED.show();                                                   // Show the LEDs
    DcsBios::setup();                                                 // Run DCS Bios setup function
}

void loop() {
    board->handleModeChange();                                        // Handle mode changes
    board->processMode();                                             // Process current mode, incl. DCS-Bios::loop()
    board->updateLeds();                                              // Update LEDs as needed
}
