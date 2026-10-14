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
 *   Copyright 2016-2026 OpenHornet
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
 * @file 5A2-RIGHT_AUX_PANEL.ino
 * @author Lahiru
 * @date 10.14.2026
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Controls the HOOK lever, WING FOLD lever, AV COOL panel & HYD PRESS gauge.
 *
 * @details
 *
 *  * **Reference Designator:** 5A2
 *  * **Intended Board:** ABSIS ALE+
 *  * **RS485 Bus Address:** 2
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | HYD PRESS gauge, needle 1 stepper COIL1 (J3-1, gauge DATA pin 1)
 * A2  | HYD PRESS gauge, needle 1 stepper COIL2 (J3-3, gauge DATA pin 3)
 * A0  | HYD PRESS gauge, needle 1 stepper COIL3 (J3-7, gauge DATA pin 7)
 * A1  | HYD PRESS gauge, needle 1 stepper COIL4 (J3-5, gauge DATA pin 5)
 * 2   | HYD PRESS gauge, needle 2 stepper COIL5 (J3-2, gauge DATA pin 2)
 * 3   | HYD PRESS gauge, needle 2 stepper COIL6 (J3-4, gauge DATA pin 4)
 * 15  | HYD PRESS gauge, needle 2 stepper COIL7 (J4-1, gauge DATA pin 8)
 * 4   | HYD PRESS gauge, needle 2 stepper COIL8 (J3-6, gauge DATA pin 6)
 * 6   | HOOK lever switch (J4-2)
 * 14  | HOOK lever light, LED 1 (J4-3)
 * 7   | HOOK lever light, LED 2 (J4-4)
 * 16  | AV COOL switch (J4-5)
 * 8   | WING FOLD handle pull / unlock switch (J4-6)
 * 10  | WING FOLD handle FOLD position (J4-7)
 * 9   | WING FOLD handle SPREAD position (J4-8)
 *
 * Pins follow OH-INTERCONNECT B v18, sheet 6 (5A2W1P3 plugs into ABSIS ALE+ J3, 5A2W1P4 into J4). The HYD PRESS
 * gauge is a GAUGE_Dual Needle Instruments PCB (X40 dual-shaft stepper). The 5A2W1P6 harness is a 2x4 housing
 * wired straight from J3-1..7 and J4-1 onto the gauge DATA header, and that header is interleaved
 * (pin 1 COIL1, 2 COIL5, 3 COIL2, 4 COIL6, 5 COIL4, 6 COIL8, 7 COIL3, 8 COIL7), so the two motors are split as
 * shown above rather than four-and-four. Both needles are homed against their low mechanical stops at power up.
 *
 * @note Needle 1 (COIL1..4) is driven by HYD_IND_LEFT and needle 2 (COIL5..8) by HYD_IND_RIGHT. Swap the
 *       HYD_LEFT_* and HYD_RIGHT_* pin defines if the gauge is built the other way round.
 * @note Switch values follow the DCS-BIOS F/A-18C definitions: HOOK_LEVER and WING_FOLD_PULL send 1 while
 *       their contact is closed, AV_COOL_SW sends 1 (NORM) while the contact is closed, WING_FOLD_ROTATE is
 *       FOLD/HOLD/UNFOLD = 0..2. Verify every switch direction in DCS before removing the untested warning.
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
   #define DCSBIOS_RS485_SLAVE 2 ///DCSBios RS485 Bus Address
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 *
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL  ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL  ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5  ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#define DCSBIOS_DISABLE_SERVO  ///< No servo on this panel, keep the Servo library out of the build.

#include "DcsBios.h"
#include <AccelStepper.h>

// Define pins for DCS-BIOS per interconnect diagram.
#define HYD_LEFT_COIL1 A3   ///< HYD PRESS needle 1 stepper COIL1 (gauge DATA pin 1)
#define HYD_LEFT_COIL2 A2   ///< HYD PRESS needle 1 stepper COIL2 (gauge DATA pin 3)
#define HYD_LEFT_COIL3 A0   ///< HYD PRESS needle 1 stepper COIL3 (gauge DATA pin 7)
#define HYD_LEFT_COIL4 A1   ///< HYD PRESS needle 1 stepper COIL4 (gauge DATA pin 5)
#define HYD_RIGHT_COIL1 2   ///< HYD PRESS needle 2 stepper COIL5 (gauge DATA pin 2)
#define HYD_RIGHT_COIL2 3   ///< HYD PRESS needle 2 stepper COIL6 (gauge DATA pin 4)
#define HYD_RIGHT_COIL3 15  ///< HYD PRESS needle 2 stepper COIL7 (gauge DATA pin 8)
#define HYD_RIGHT_COIL4 4   ///< HYD PRESS needle 2 stepper COIL8 (gauge DATA pin 6)
#define HOOK_SW 6           ///< HOOK lever switch
#define HOOK_IND_1 14       ///< HOOK lever light, LED 1
#define HOOK_IND_2 7        ///< HOOK lever light, LED 2
#define AVC_SW 16           ///< AV COOL switch
#define WF_UNLOCK 8         ///< WING FOLD handle pull / unlock switch
#define WF_FOLD 10          ///< WING FOLD handle FOLD position
#define WF_SPREAD 9         ///< WING FOLD handle SPREAD position

// HYD PRESS gauge tuning. Steps are counted from each needle's low mechanical stop.
#define HYD_ZERO_POS 20        ///< Steps from the low mechanical stop to the dial zero.
#define HYD_MAX_POS 600        ///< Steps from the low mechanical stop to the dial maximum. Tune on hardware.
#define HYD_LEFT_DIRECTION 1   ///< 1 if needle 1 moves clockwise with increasing steps, -1 to reverse.
#define HYD_RIGHT_DIRECTION 1  ///< 1 if needle 2 moves clockwise with increasing steps, -1 to reverse.
#define HYD_SPEED 300          ///< Normal needle speed in steps per second.
#define HYD_ACCEL 600          ///< Normal needle acceleration in steps per second squared.
#define HYD_HOMING_SPEED 40    ///< Slow speed used to find the mechanical stops.

/// Left hydraulic pressure needle. The coil order is swapped for a -1 direction so the dial runs the other way.
AccelStepper hydLeftNeedle(AccelStepper::FULL4WIRE,
                           (HYD_LEFT_DIRECTION == -1) ? HYD_LEFT_COIL3 : HYD_LEFT_COIL1,
                           (HYD_LEFT_DIRECTION == -1) ? HYD_LEFT_COIL4 : HYD_LEFT_COIL2,
                           (HYD_LEFT_DIRECTION == -1) ? HYD_LEFT_COIL1 : HYD_LEFT_COIL3,
                           (HYD_LEFT_DIRECTION == -1) ? HYD_LEFT_COIL2 : HYD_LEFT_COIL4);

/// Right hydraulic pressure needle.
AccelStepper hydRightNeedle(AccelStepper::FULL4WIRE,
                            (HYD_RIGHT_DIRECTION == -1) ? HYD_RIGHT_COIL3 : HYD_RIGHT_COIL1,
                            (HYD_RIGHT_DIRECTION == -1) ? HYD_RIGHT_COIL4 : HYD_RIGHT_COIL2,
                            (HYD_RIGHT_DIRECTION == -1) ? HYD_RIGHT_COIL1 : HYD_RIGHT_COIL3,
                            (HYD_RIGHT_DIRECTION == -1) ? HYD_RIGHT_COIL2 : HYD_RIGHT_COIL4);

// Connect switches and lamps to DCS-BIOS
DcsBios::Switch2Pos hookLever("HOOK_LEVER", HOOK_SW);
DcsBios::LED arrestingHookLt1(FA_18C_hornet_ARRESTING_HOOK_LT_AM, HOOK_IND_1);
DcsBios::LED arrestingHookLt2(FA_18C_hornet_ARRESTING_HOOK_LT_AM, HOOK_IND_2);
DcsBios::Switch2Pos avCoolSw("AV_COOL_SW", AVC_SW);
DcsBios::Switch2Pos wingFoldPull("WING_FOLD_PULL", WF_UNLOCK);
DcsBios::Switch3Pos wingFoldRotate("WING_FOLD_ROTATE", WF_FOLD, WF_SPREAD);

/**
* @brief DCS-BIOS read of the left hydraulic pressure. Maps the gauge value to a needle position.
*/
void onHydIndLeftChange(unsigned int newValue) {
  hydLeftNeedle.moveTo(map(newValue, 0, 65535, HYD_ZERO_POS, HYD_MAX_POS));
}
DcsBios::IntegerBuffer hydIndLeftBuffer(FA_18C_hornet_HYD_IND_LEFT, onHydIndLeftChange);

/**
* @brief DCS-BIOS read of the right hydraulic pressure. Maps the gauge value to a needle position.
*/
void onHydIndRightChange(unsigned int newValue) {
  hydRightNeedle.moveTo(map(newValue, 0, 65535, HYD_ZERO_POS, HYD_MAX_POS));
}
DcsBios::IntegerBuffer hydIndRightBuffer(FA_18C_hornet_HYD_IND_RIGHT, onHydIndRightChange);

/**
* @brief Home a needle against its low mechanical stop, then move it to the dial zero.
* @param needle The stepper to home.
* @note Blocks for a few seconds. Clicking at the stop is normal.
*/
void homeNeedle(AccelStepper &needle) {
  needle.setMaxSpeed(HYD_HOMING_SPEED);
  needle.setAcceleration(HYD_HOMING_SPEED);
  needle.setCurrentPosition(HYD_MAX_POS);  // Assume the needle could be anywhere up to full scale.
  needle.runToNewPosition(0);              // Drive back against the mechanical stop.
  needle.setCurrentPosition(0);            // The stop is position 0.
  needle.runToNewPosition(HYD_ZERO_POS);   // Park on the dial zero.
  needle.setMaxSpeed(HYD_SPEED);
  needle.setAcceleration(HYD_ACCEL);
}

/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the programm start, belongs in this function.
*/
void setup() {
  #if !defined(DCSBIOS_RS485_SLAVE) && !defined(DCSBIOS_RS485_MASTER)   // Disable RS485 TRANSCEIVER IF NOT BEING USED
    pinMode(TXENABLE_PIN, OUTPUT);   // WILL DISABLE TX ONLY
    digitalWrite(TXENABLE_PIN, LOW); // SET TX LOW TO DISABLE TX ON TRANSCEIVER (HIGH enables the driver, see DCS-BIOS set_txen())
  #endif

  // Run DCS Bios setup function
  DcsBios::setup();

  homeNeedle(hydLeftNeedle);
  homeNeedle(hydRightNeedle);
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

  hydLeftNeedle.run();   // Non-blocking, one step at most per call.
  hydRightNeedle.run();
}
