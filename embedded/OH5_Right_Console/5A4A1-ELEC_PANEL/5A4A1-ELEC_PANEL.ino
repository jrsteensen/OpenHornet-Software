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
 * @file 5A4A1-ELEC_PANEL.ino
 * @author Lahiru
 * @date 10.14.2026
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Controls the ELEC panel & the BATT gauge.
 *
 * @details
 *
 *  * **Reference Designator:** 5A4A1
 *  * **Intended Board:** ABSIS ALE+
 *  * **RS485 Bus Address:** 3
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A3  | L GEN switch, NORM contact (J3-1)
 * 2   | BATT switch, ON contact (J3-2)
 * A2  | BATT switch, ORIDE contact (J3-3)
 * 3   | R GEN switch, NORM contact (J3-4)
 * 15  | BATT gauge, U VOLTS needle stepper COIL1 (J4-1, gauge DATA pin 1)
 * 14  | BATT gauge, U VOLTS needle stepper COIL2 (J4-3, gauge DATA pin 3)
 * 10  | BATT gauge, U VOLTS needle stepper COIL3 (J4-7, gauge DATA pin 7)
 * 16  | BATT gauge, U VOLTS needle stepper COIL4 (J4-5, gauge DATA pin 5)
 * 6   | BATT gauge, E VOLTS needle stepper COIL5 (J4-2, gauge DATA pin 2)
 * 7   | BATT gauge, E VOLTS needle stepper COIL6 (J4-4, gauge DATA pin 4)
 * 9   | BATT gauge, E VOLTS needle stepper COIL7 (J4-8, gauge DATA pin 8)
 * 8   | BATT gauge, E VOLTS needle stepper COIL8 (J4-6, gauge DATA pin 6)
 *
 * Pins follow OH-INTERCONNECT B v18, sheet 6 (5A4A1W1P3 plugs into ABSIS ALE+ J3, 5A4A1W2P4 into J4). The BATT
 * gauge is a GAUGE_Dual Needle Instruments PCB (X40 dual-shaft stepper). The 5A2W1P1 harness is a 2x4 housing
 * wired straight from J4-1..8 onto the gauge DATA header, and that header is interleaved
 * (pin 1 COIL1, 2 COIL5, 3 COIL2, 4 COIL6, 5 COIL4, 6 COIL8, 7 COIL3, 8 COIL7), so the two motors are split as
 * shown above rather than four-and-four. Both needles are homed against their low mechanical stops at power up.
 *
 * @note Needle 1 (COIL1..4) is driven by VOLT_U and needle 2 (COIL5..8) by VOLT_E. Swap the VOLT_U_* and
 *       VOLT_E_* pin defines if the gauge is built the other way round.
 * @note Switch values follow the DCS-BIOS F/A-18C definitions: L_GEN_SW and R_GEN_SW send 1 (NORM) while the
 *       contact is closed, BATTERY_SW is ON/OFF/ORIDE = 0..2. Verify every switch direction in DCS before
 *       removing the untested warning.
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
   #define DCSBIOS_RS485_SLAVE 3 ///DCSBios RS485 Bus Address
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
#define L_GEN A3          ///< L GEN switch, NORM contact
#define BATT_ON 2         ///< BATT switch, ON contact
#define BATT_ORIDE A2     ///< BATT switch, ORIDE contact
#define R_GEN 3           ///< R GEN switch, NORM contact
#define VOLT_U_COIL1 15   ///< U VOLTS needle stepper COIL1 (gauge DATA pin 1)
#define VOLT_U_COIL2 14   ///< U VOLTS needle stepper COIL2 (gauge DATA pin 3)
#define VOLT_U_COIL3 10   ///< U VOLTS needle stepper COIL3 (gauge DATA pin 7)
#define VOLT_U_COIL4 16   ///< U VOLTS needle stepper COIL4 (gauge DATA pin 5)
#define VOLT_E_COIL1 6    ///< E VOLTS needle stepper COIL5 (gauge DATA pin 2)
#define VOLT_E_COIL2 7    ///< E VOLTS needle stepper COIL6 (gauge DATA pin 4)
#define VOLT_E_COIL3 9    ///< E VOLTS needle stepper COIL7 (gauge DATA pin 8)
#define VOLT_E_COIL4 8    ///< E VOLTS needle stepper COIL8 (gauge DATA pin 6)

// BATT gauge tuning. Steps are counted from each needle's low mechanical stop.
#define VOLT_ZERO_POS 20       ///< Steps from the low mechanical stop to the dial zero.
#define VOLT_MAX_POS 600       ///< Steps from the low mechanical stop to the dial maximum. Tune on hardware.
#define VOLT_U_DIRECTION 1     ///< 1 if the U needle moves clockwise with increasing steps, -1 to reverse.
#define VOLT_E_DIRECTION 1     ///< 1 if the E needle moves clockwise with increasing steps, -1 to reverse.
#define VOLT_SPEED 300         ///< Normal needle speed in steps per second.
#define VOLT_ACCEL 600         ///< Normal needle acceleration in steps per second squared.
#define VOLT_HOMING_SPEED 40   ///< Slow speed used to find the mechanical stops.

/// Battery U volts needle. The coil order is swapped for a -1 direction so the dial runs the other way.
AccelStepper voltUNeedle(AccelStepper::FULL4WIRE,
                         (VOLT_U_DIRECTION == -1) ? VOLT_U_COIL3 : VOLT_U_COIL1,
                         (VOLT_U_DIRECTION == -1) ? VOLT_U_COIL4 : VOLT_U_COIL2,
                         (VOLT_U_DIRECTION == -1) ? VOLT_U_COIL1 : VOLT_U_COIL3,
                         (VOLT_U_DIRECTION == -1) ? VOLT_U_COIL2 : VOLT_U_COIL4);

/// Battery E volts needle.
AccelStepper voltENeedle(AccelStepper::FULL4WIRE,
                         (VOLT_E_DIRECTION == -1) ? VOLT_E_COIL3 : VOLT_E_COIL1,
                         (VOLT_E_DIRECTION == -1) ? VOLT_E_COIL4 : VOLT_E_COIL2,
                         (VOLT_E_DIRECTION == -1) ? VOLT_E_COIL1 : VOLT_E_COIL3,
                         (VOLT_E_DIRECTION == -1) ? VOLT_E_COIL2 : VOLT_E_COIL4);

// Connect switches to DCS-BIOS
DcsBios::Switch2Pos lGenSw("L_GEN_SW", L_GEN);
DcsBios::Switch3Pos batterySw("BATTERY_SW", BATT_ON, BATT_ORIDE);
DcsBios::Switch2Pos rGenSw("R_GEN_SW", R_GEN);

/**
* @brief DCS-BIOS read of the battery U volts. Maps the gauge value to a needle position.
*/
void onVoltUChange(unsigned int newValue) {
  voltUNeedle.moveTo(map(newValue, 0, 65535, VOLT_ZERO_POS, VOLT_MAX_POS));
}
DcsBios::IntegerBuffer voltUBuffer(FA_18C_hornet_VOLT_U, onVoltUChange);

/**
* @brief DCS-BIOS read of the battery E volts. Maps the gauge value to a needle position.
*/
void onVoltEChange(unsigned int newValue) {
  voltENeedle.moveTo(map(newValue, 0, 65535, VOLT_ZERO_POS, VOLT_MAX_POS));
}
DcsBios::IntegerBuffer voltEBuffer(FA_18C_hornet_VOLT_E, onVoltEChange);

/**
* @brief Home a needle against its low mechanical stop, then move it to the dial zero.
* @param needle The stepper to home.
* @note Blocks for a few seconds. Clicking at the stop is normal.
*/
void homeNeedle(AccelStepper &needle) {
  needle.setMaxSpeed(VOLT_HOMING_SPEED);
  needle.setAcceleration(VOLT_HOMING_SPEED);
  needle.setCurrentPosition(VOLT_MAX_POS);  // Assume the needle could be anywhere up to full scale.
  needle.runToNewPosition(0);               // Drive back against the mechanical stop.
  needle.setCurrentPosition(0);             // The stop is position 0.
  needle.runToNewPosition(VOLT_ZERO_POS);   // Park on the dial zero.
  needle.setMaxSpeed(VOLT_SPEED);
  needle.setAcceleration(VOLT_ACCEL);
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

  homeNeedle(voltUNeedle);
  homeNeedle(voltENeedle);
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

  voltUNeedle.run();  // Non-blocking, one step at most per call.
  voltENeedle.run();
}
