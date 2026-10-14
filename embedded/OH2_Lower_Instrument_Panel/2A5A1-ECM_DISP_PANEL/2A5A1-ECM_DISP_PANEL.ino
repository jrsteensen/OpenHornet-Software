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
 * @file 2A5A1-ECM_DISP_PANEL.ino
 * @author Lahiru
 * @date 10.14.2026
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Controls the ECM DISP panel & CABIN PRESSURE gauge.
 *
 * @details
 *
 *  * **Reference Designator:** 2A5A1
 *  * **Intended Board:** ABSIS ALE+
 *  * **RS485 Bus Address:** 4
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 15  | ECM JETT push button (J4-1)
 * 6   | ECM mode knob STBY (J4-2)
 * 14  | ECM mode knob BIT (J4-3)
 * 7   | ECM mode knob REC (J4-4)
 * 16  | ECM mode knob XMIT (J4-5)
 * 8   | DISPENSER switch BYPASS (J4-6)
 * 10  | DISPENSER switch OFF (J4-7)
 * 9   | AUX REL switch ENABLE (J4-8)
 * A3  | Cabin pressure gauge stepper COIL1 (J3-1)
 * 2   | Cabin pressure gauge stepper COIL2 (J3-2)
 * A2  | Cabin pressure gauge stepper COIL3 (J3-3)
 * A1  | Cabin pressure gauge stepper COIL4 (J3-5)
 * 3   | Cabin pressure gauge backlight, WS2812B data in (J3-4)
 *
 * Pins follow OH-INTERCONNECT B v18, sheet 3 (2A5A1W1P4 plugs into ABSIS ALE+ J4, 2A5A1W2P3 into J3).
 * The ECM mode knob OFF position has no wire and is reported when no other position pin is low.
 * The cabin pressure gauge is a GAUGE_Analog Instruments PCB (VID29-02 stepper and two WS2812B
 * backlight LEDs). Its zero-detect sensor is not wired in the interconnect, so the needle is homed
 * against the low mechanical stop at power up. The coil pins follow the 2A5A1W2P4 wire names COIL1..COIL4
 * in order, the same J3 positions the 4A3A3 brake pressure stepper uses. The gauge STEPPER header is
 * ordered COIL1, COIL4, COIL2, COIL3, so land each wire on the header pin of the same name; if the plug is
 * fitted positionally instead, use A3, A2, A1, D2 for CP_COIL1..CP_COIL4.
 *
 * @note Switch position order follows the DCS-BIOS F/A-18C definitions:
 *       ECM_MODE_SW = XMIT/REC/BIT/STBY/OFF (0..4), CMSD_DISPENSE_SW = BYPASS/ON/OFF (0..2),
 *       AUX_REL_SW sends 1 when the ENABLE contact is closed. Verify every switch direction in DCS
 *       before removing the untested warning.
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
   #define DCSBIOS_RS485_SLAVE 4 ///DCSBios RS485 Bus Address
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
#include <FastLED.h>
#include "2A5A1-ECM_DISP_PANEL.h"

// Define pins for DCS-BIOS per interconnect diagram.
#define ECM_JETT 15     ///< ECM JETT push button
#define ECM_STBY 6      ///< ECM mode knob STBY
#define ECM_BIT 14      ///< ECM mode knob BIT
#define ECM_REC 7       ///< ECM mode knob REC
#define ECM_XMIT 16     ///< ECM mode knob XMIT
#define DISP_BYPASS 8   ///< DISPENSER switch BYPASS
#define DISP_OFF 10     ///< DISPENSER switch OFF
#define AUXR_ENBL 9     ///< AUX REL switch ENABLE
#define CP_COIL1 A3     ///< Cabin pressure stepper COIL1
#define CP_COIL2 2      ///< Cabin pressure stepper COIL2
#define CP_COIL3 A2     ///< Cabin pressure stepper COIL3
#define CP_COIL4 A1     ///< Cabin pressure stepper COIL4
#define CP_BL_DIN 3     ///< Cabin pressure gauge backlight data in

// Cabin pressure gauge tuning. Steps are counted from the low mechanical stop.
#define CP_ZERO_POS 20        ///< Steps from the low mechanical stop to the dial zero.
#define CP_MAX_POS 600        ///< Steps from the low mechanical stop to the dial maximum. Tune on hardware.
#define CP_DIRECTION 1        ///< 1 if the needle moves clockwise with increasing steps, -1 to reverse.
#define CP_SPEED 300          ///< Normal needle speed in steps per second.
#define CP_ACCEL 600          ///< Normal needle acceleration in steps per second squared.
#define CP_HOMING_SPEED 40    ///< Slow speed used to find the mechanical stop.
#define CP_LED_COUNT 2        ///< Number of WS2812B backlight LEDs on the gauge PCB.

CRGB cabinPressLight[CP_LED_COUNT];  ///< Cabin pressure gauge backlight pixels.

/// Cabin pressure needle. The coil order is swapped for CP_DIRECTION = -1 so the dial runs the other way.
AccelStepper cabinPressNeedle(AccelStepper::FULL4WIRE,
                              (CP_DIRECTION == -1) ? CP_COIL3 : CP_COIL1,
                              (CP_DIRECTION == -1) ? CP_COIL4 : CP_COIL2,
                              (CP_DIRECTION == -1) ? CP_COIL1 : CP_COIL3,
                              (CP_DIRECTION == -1) ? CP_COIL2 : CP_COIL4);

// Connect switches to DCS-BIOS
const byte ecmModePins[5] = { ECM_XMIT, ECM_REC, ECM_BIT, ECM_STBY, DcsBios::PIN_NC };  ///< XMIT/REC/BIT/STBY/OFF per DCS-BIOS order.
/** @todo Replace SwitchMultiPosDebounce with DcsBios::SwitchMultiPos if/when DCS-Skunkworks adds debounce for PIN_NC positions.
 *  Without the debounce the knob would report OFF for a moment between every detent. */
SwitchMultiPosDebounce ecmModeSw("ECM_MODE_SW", ecmModePins, 5, false, 100);
DcsBios::Switch3Pos cmsdDispenseSw("CMSD_DISPENSE_SW", DISP_BYPASS, DISP_OFF);
DcsBios::Switch2Pos auxRelSw("AUX_REL_SW", AUXR_ENBL);
DcsBios::Switch2Pos cmsdJetSelBtn("CMSD_JET_SEL_BTN", ECM_JETT);

/**
* @brief DCS-BIOS read of the cabin pressure altimeter. Maps the gauge value to a needle position.
*/
void onPressureAltChange(unsigned int newValue) {
  cabinPressNeedle.moveTo(map(newValue, 0, 65535, CP_ZERO_POS, CP_MAX_POS));
}
DcsBios::IntegerBuffer pressureAltBuffer(FA_18C_hornet_PRESSURE_ALT, onPressureAltChange);

/**
* @brief DCS-BIOS read of the instrument lighting. Dims the gauge backlight, green like the other instrument backlights.
*/
void onInstrIntLtChange(unsigned int newValue) {
  fill_solid(cabinPressLight, CP_LED_COUNT, CRGB(0, map(newValue, 0, 65535, 0, 255), 0));
  FastLED.show();
}
DcsBios::IntegerBuffer instrIntLtBuffer(FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange);

/**
* @brief Home the needle against the low mechanical stop, then move it to the dial zero.
* @note Blocks for a few seconds. Clicking at the stop is normal.
*/
void homeCabinPressNeedle() {
  cabinPressNeedle.setMaxSpeed(CP_HOMING_SPEED);
  cabinPressNeedle.setAcceleration(CP_HOMING_SPEED);
  cabinPressNeedle.setCurrentPosition(CP_MAX_POS);  // Assume the needle could be anywhere up to full scale.
  cabinPressNeedle.runToNewPosition(0);             // Drive back against the mechanical stop.
  cabinPressNeedle.setCurrentPosition(0);           // The stop is position 0.
  cabinPressNeedle.runToNewPosition(CP_ZERO_POS);   // Park on the dial zero.
  cabinPressNeedle.setMaxSpeed(CP_SPEED);
  cabinPressNeedle.setAcceleration(CP_ACCEL);
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

  FastLED.addLeds<WS2812B, CP_BL_DIN, GRB>(cabinPressLight, CP_LED_COUNT);  ///< WS2812B on CP_BL_DIN, GRB wire order
  FastLED.clear(true);  ///< Backlight off until DCS sends the instrument light level

  homeCabinPressNeedle();
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

  ///@todo Remove ecmModeSw.pollThisInput() if/when the debounced class is replaced by DcsBios::SwitchMultiPos.
  ecmModeSw.pollThisInput();

  cabinPressNeedle.run();  // Non-blocking, one step at most per call.
}
