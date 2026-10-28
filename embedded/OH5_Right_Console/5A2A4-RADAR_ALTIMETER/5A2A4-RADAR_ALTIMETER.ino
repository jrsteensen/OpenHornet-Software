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
 * @file 5A2A4-RADAR_ALTIMETER.ino
 * @author Lahiru
 * @date 10.14.2026
 * @version u.0.0.1 (untested)
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. (Remove this line once tested on hardware and in system.)
 * @brief Controls the RADAR ALTIMETER gauge.
 *
 * @details
 *
 *  * **Reference Designator:** 5A2A4
 *  * **Intended Board:** CONTROLLER_Radar Altimeter (SparkFun Pro Micro)
 *  * **RS485 Bus Address:** 1
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 16  | Altitude needle stepper COIL1 (J5-2, gauge STEPPER header pin 1)
 * 10  | Altitude needle stepper COIL2 (J5-1, gauge STEPPER header pin 3)
 * 15  | Altitude needle stepper COIL3 (J5-9, gauge STEPPER header pin 4)
 * 14  | Altitude needle stepper COIL4 (J5-10, gauge STEPPER header pin 2)
 * 3   | Gauge backlight, WS2812B data (J5-4, when JP1 BL_DIN_SEL selects D3)
 * 9   | Green lamp (J13)
 * 6   | Red LOW ALT warning lamp (J14)
 * 5   | OFF flag servo signal (J15-3, through Q1)
 * 2   | Push to test button (J16)
 * A8  | WARN HT set knob potentiometer wiper (J17-4)
 * 7   | WARN HT set knob encoder channel A (J17-2, encoder option only)
 * 8   | WARN HT set knob encoder channel B (J17-4, encoder option only)
 * 4   | RS-485 transceiver RE/DE (on the controller PCB, not a connector pin)
 *
 * Pins follow the CONTROLLER_Radar Altimeter schematic and OH-INTERCONNECT B v18, sheet 6. The gauge is a
 * GAUGE_Analog Instruments PCB (VID29-02 stepper, two WS2812B backlight LEDs); its zero-detect sensor is not
 * wired, so the needle is homed against the low mechanical stop at power up. The coil pins follow the
 * 5A2A4W6P2 harness table, which lands each controller COILn wire on the gauge header pin of the same name
 * (header order COIL1, COIL4, COIL2, COIL3). The gauge symbol on the same sheet is drawn positionally and
 * disagrees with that table; if the needle only buzzes, set RA_COIL1..RA_COIL4 to 16, 15, 14, 10.
 *
 * @note The OFF flag servo header J15 is driven through Q1 (2N7002) with a pull-up to 3.3 V, which inverts the
 *       signal from pin 5. With RADALT_OFF_FLAG_INVERTED set to 1 (default) Timer3 generates an inverted servo
 *       pulse on pin 5 so the servo sees a normal pulse. Set it to 0 if the board is modified to drive the servo
 *       directly; the sketch then uses DcsBios::ServoOutput.
 * @note The DCS-BIOS RADALT_HEIGHT control only accepts variable steps, so the pot is synced to the sim value
 *       (DcsBios::RotarySyncingPotentiometer). The interconnect draws the pot wiper on J17-2 (D7), which has no
 *       ADC on the ATmega32U4; wire the wiper to J17-4 (D8/A8) instead. Set RADALT_HEIGHT_ENCODER to 1 to use a
 *       rotary encoder on J17-2 / J17-4 instead of the pot.
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
   #define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address
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
#define TXENABLE_PIN 4  ///< Sets TXENABLE_PIN to Arduino Pin 4. The Radar Altimeter controller uses D4, not D5 like the ABSIS ALE.
#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#define RADALT_OFF_FLAG_INVERTED 1  ///< 1: Timer3 inverted pulse on pin 5 (stock PCB with Q1). 0: DcsBios::ServoOutput on pin 5.
#define RADALT_HEIGHT_ENCODER 0     ///< 0: potentiometer on A8 synced to the sim. 1: rotary encoder on pins 7 and 8.

#if RADALT_OFF_FLAG_INVERTED
#define DCSBIOS_DISABLE_SERVO  ///< The Servo library is not needed when Timer3 drives the OFF flag.
#endif

#include "DcsBios.h"
#include <AccelStepper.h>
#include <FastLED.h>

// Define pins for DCS-BIOS per interconnect diagram.
#define RA_COIL1 16       ///< Altitude needle stepper COIL1 (J5-2)
#define RA_COIL2 10       ///< Altitude needle stepper COIL2 (J5-1)
#define RA_COIL3 15       ///< Altitude needle stepper COIL3 (J5-9)
#define RA_COIL4 14       ///< Altitude needle stepper COIL4 (J5-10)
#define RA_BL_DIN 3       ///< Gauge backlight data in
#define RA_GRN_LAMP 9     ///< Green lamp
#define RA_RED_LAMP 6     ///< Red LOW ALT warning lamp
#define RA_OFF_FLAG 5     ///< OFF flag servo signal (OC3A)
#define RA_TEST_SW 2      ///< Push to test button
#define RA_HEIGHT_POT A8  ///< WARN HT knob potentiometer wiper
#define RA_ENC_A 7        ///< WARN HT knob encoder channel A
#define RA_ENC_B 8        ///< WARN HT knob encoder channel B

// Altitude needle tuning. Steps are counted from the low mechanical stop.
#define RA_ZERO_POS 20        ///< Steps from the low mechanical stop to the dial zero.
#define RA_MAX_POS 600        ///< Steps from the low mechanical stop to the dial maximum. Tune on hardware.
#define RA_DIRECTION 1        ///< 1 if the needle moves clockwise with increasing steps, -1 to reverse.
#define RA_SPEED 300          ///< Normal needle speed in steps per second.
#define RA_ACCEL 600          ///< Normal needle acceleration in steps per second squared.
#define RA_HOMING_SPEED 40    ///< Slow speed used to find the mechanical stop.
#define RA_LED_COUNT 2        ///< Number of WS2812B backlight LEDs on the gauge PCB.

// OFF flag servo tuning. Pulse widths in microseconds; swap the two values to reverse the flag travel.
#define RA_OFF_FLAG_HIDDEN_US 1000  ///< Servo pulse with the OFF flag out of view (DCS value 0).
#define RA_OFF_FLAG_SHOWN_US 2000   ///< Servo pulse with the OFF flag in view (DCS value 65535).

#define RA_HEIGHT_DEADBAND 700      ///< Pot vs sim difference (0..65535) below which no correction is sent, about 1 percent.
#define RA_HEIGHT_MAX_STEP 3200     ///< Largest correction sent per message, the DCS-BIOS suggested step.

CRGB radaltLight[RA_LED_COUNT];  ///< Gauge backlight pixels.

/// Altitude needle. The coil order is swapped for RA_DIRECTION = -1 so the dial runs the other way.
AccelStepper radaltNeedle(AccelStepper::FULL4WIRE,
                          (RA_DIRECTION == -1) ? RA_COIL3 : RA_COIL1,
                          (RA_DIRECTION == -1) ? RA_COIL4 : RA_COIL2,
                          (RA_DIRECTION == -1) ? RA_COIL1 : RA_COIL3,
                          (RA_DIRECTION == -1) ? RA_COIL2 : RA_COIL4);

// Connect switches and lamps to DCS-BIOS
DcsBios::Switch2Pos radaltTestSw("RADALT_TEST_SW", RA_TEST_SW);
DcsBios::LED radaltGreenLamp(FA_18C_hornet_RADALT_GREEN_LAMP_AM, RA_GRN_LAMP);
DcsBios::LED lowAltWarnLt(FA_18C_hornet_LOW_ALT_WARN_LT_AM, RA_RED_LAMP);

#if RADALT_HEIGHT_ENCODER
DcsBios::RotaryEncoder radaltHeight("RADALT_HEIGHT", "-3200", "+3200", RA_ENC_A, RA_ENC_B);
#else
/**
* @brief Correction to send to DCS so the sim knob follows the physical pot.
* @param potValue Pot position scaled to 0..65535.
* @param dcsValue Knob position reported by DCS-BIOS, 0..65535.
* @returns The variable step to send, 0 when the two are close enough.
*/
int radaltHeightSync(unsigned int potValue, unsigned int dcsValue) {
  long diff = (long)potValue - (long)dcsValue;
  if (diff > -RA_HEIGHT_DEADBAND && diff < RA_HEIGHT_DEADBAND) {
    return 0;
  }
  return (int)constrain(diff, -(long)RA_HEIGHT_MAX_STEP, (long)RA_HEIGHT_MAX_STEP);
}
/// Use DcsBios::InvertedRotarySyncingPotentiometer instead if the knob runs backwards.
DcsBios::RotarySyncingPotentiometer radaltHeight("RADALT_HEIGHT", RA_HEIGHT_POT, FA_18C_hornet_RADALT_HEIGHT, radaltHeightSync);
#endif

#if RADALT_OFF_FLAG_INVERTED
/**
* @brief Start Timer3 as a 50 Hz inverted servo pulse generator on pin 5 (OC3A).
* @details Fast PWM mode 14 (TOP = ICR3) with prescaler 8 gives 0.5 us per tick and a 20 ms frame. In inverting
*          compare mode the pin is LOW for OCR3A ticks and HIGH for the rest of the frame; Q1 on the controller
*          PCB turns that into a normal HIGH servo pulse of OCR3A ticks at J15. Timer3 is otherwise unused on the
*          Pro Micro (the Servo library uses Timer1), so nothing else is disturbed.
*/
void offFlagServoBegin() {
  pinMode(RA_OFF_FLAG, OUTPUT);
  TCCR3A = _BV(COM3A1) | _BV(COM3A0) | _BV(WGM31);  // Inverting compare output on OC3A, fast PWM
  TCCR3B = _BV(WGM33) | _BV(WGM32) | _BV(CS31);     // Fast PWM mode 14, prescaler 8
  ICR3 = 40000;                                     // 20 ms frame
  OCR3A = (unsigned int)RA_OFF_FLAG_SHOWN_US * 2;   // Show the OFF flag until DCS says otherwise
}

/**
* @brief Set the OFF flag servo pulse width.
* @param pulseUs Pulse width in microseconds.
*/
void offFlagServoWrite(unsigned int pulseUs) {
  OCR3A = pulseUs * 2;  // 0.5 us per tick
}

/**
* @brief DCS-BIOS read of the OFF flag position, mapped to a servo pulse width.
*/
void onRadaltOffFlagChange(unsigned int newValue) {
  offFlagServoWrite(map(newValue, 0, 65535, RA_OFF_FLAG_HIDDEN_US, RA_OFF_FLAG_SHOWN_US));
}
DcsBios::IntegerBuffer radaltOffFlagBuffer(FA_18C_hornet_RADALT_OFF_FLAG, onRadaltOffFlagChange);
#else
DcsBios::ServoOutput radaltOffFlag(FA_18C_hornet_RADALT_OFF_FLAG_A, RA_OFF_FLAG, RA_OFF_FLAG_HIDDEN_US, RA_OFF_FLAG_SHOWN_US);
#endif

/**
* @brief DCS-BIOS read of the altitude pointer. Maps the gauge value to a needle position.
*/
void onRadaltAltPtrChange(unsigned int newValue) {
  radaltNeedle.moveTo(map(newValue, 0, 65535, RA_ZERO_POS, RA_MAX_POS));
}
DcsBios::IntegerBuffer radaltAltPtrBuffer(FA_18C_hornet_RADALT_ALT_PTR, onRadaltAltPtrChange);

/**
* @brief DCS-BIOS read of the instrument lighting. Dims the gauge backlight, green like the other instrument backlights.
*/
void onInstrIntLtChange(unsigned int newValue) {
  fill_solid(radaltLight, RA_LED_COUNT, CRGB(0, map(newValue, 0, 65535, 0, 255), 0));
  FastLED.show();
}
DcsBios::IntegerBuffer instrIntLtBuffer(FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange);

/**
* @brief Home the needle against the low mechanical stop, then move it to the dial zero.
* @note Blocks for a few seconds. Clicking at the stop is normal.
*/
void homeRadaltNeedle() {
  radaltNeedle.setMaxSpeed(RA_HOMING_SPEED);
  radaltNeedle.setAcceleration(RA_HOMING_SPEED);
  radaltNeedle.setCurrentPosition(RA_MAX_POS);  // Assume the needle could be anywhere up to full scale.
  radaltNeedle.runToNewPosition(0);             // Drive back against the mechanical stop.
  radaltNeedle.setCurrentPosition(0);           // The stop is position 0.
  radaltNeedle.runToNewPosition(RA_ZERO_POS);   // Park on the dial zero.
  radaltNeedle.setMaxSpeed(RA_SPEED);
  radaltNeedle.setAcceleration(RA_ACCEL);
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

  #if RADALT_OFF_FLAG_INVERTED
  offFlagServoBegin();
  #endif

  FastLED.addLeds<WS2812B, RA_BL_DIN, GRB>(radaltLight, RA_LED_COUNT);  ///< WS2812B on RA_BL_DIN, GRB wire order
  FastLED.clear(true);  ///< Backlight off until DCS sends the instrument light level

  homeRadaltNeedle();
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

  radaltNeedle.run();  // Non-blocking, one step at most per call.
}
