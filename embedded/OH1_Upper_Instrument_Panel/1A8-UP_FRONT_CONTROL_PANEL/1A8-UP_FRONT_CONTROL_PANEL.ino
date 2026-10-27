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
 * @file 1A8-UP_FRONT_CONTROL_PANEL.ino
 * @author Sandra Carroll, sandra
 *         Christian Thalhammer, scuba
 *         B. Murphy, Murtle
 * @date 01.10.2026
 * @version 1.0.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the Up-Front Control Panel
 *
 * @details
 * 
 *  * **Reference Designator:** 1A8
 *  * **Intended Board:** UFC_MAIN v6.3.1
 *  * **RS485 Bus Address:** N/A
 * 
 * *##Wiring diagram (ESP32-S3 pin assignments; see the "Pin Assignments" section below):
 * PIN | Function
 * --- | ---
 *  35	|	SDApin
 *  36	|	SCLpin
 *  15	|	IO_INT
 *  13	|	NOGOLED (UFC_NOGO, via TCA8418)
 *  14	|	GOLED (UFC_GO, via TCA8418)
 *  13	|	BL_DATA_PIN
 *  33	|	ENC1PB
 *  37	|	ENC1A
 *  38	|	ENC1B
 *  34	|	ENC2PB
 *  21	|	ENC2A
 *  17	|	ENC2B
 *  12	|	DIO_3_3V
 *   5	|	OP1_CLK_3_3V
 *   4	|	OP2_CLK_3_3V
 *   6	|	OP3_CLK_3_3V
 *   7	|	OP4_CLK_3_3V
 *  18	|	OP5_CLK_3_3V
 *  44	|	CUE_CLK_3_3V
 *  16	|	VCC_SNS_3_3V
 *  43	|	COM1_CLK_3_3V
 *   8	|	COM2_CLK_3_3V
 *   1	|	ADC_RDY_3_3V
 *  14	|	SCRCS_3_3V
 *   9	|	SCRWR_3_3V
 *  10	|	SCRDATA_3_3V
 *  11	|	SCRBL_3_3V
 *  A0	|	UFC_COMM1_VOL (via ADS1115)
 *  A1	|	UFC_COMM2_VOL (via ADS1115)
 *  A2	|	UFC_BRT_POT (via ADS1115)
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 */

// ══════════════════════════════════════════════════════════════════════════════
// Compile-Time Configuration
// ══════════════════════════════════════════════════════════════════════════════

// Use the ESP32 core's native TinyUSB CDC Serial transport, enabled by the
// board build flags. Do not load a second, incompatible TinyUSB stack.

/**
 * Enable or Disable usage of Interrupt processing
 * comment out to disable
*/
//#define USE_ADC_RDY 
//#define USE_IO_INT

/**
 * Check if we're on a Mega328 or Mega2560 and not on a ESP32 to define the correct
 * serial interface
 * 
 */
#if (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)) && !defined(ARDUINO_ARCH_ESP32)
  #define DCSBIOS_IRQ_SERIAL // < This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
  #define DCSBIOS_DEFAULT_SERIAL // < This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

// ══════════════════════════════════════════════════════════════════════════════
// Library Includes
// ══════════════════════════════════════════════════════════════════════════════

/**
 * @brief Main Includes
 */
#include <Adafruit_TCA8418.h>           //Key Matrix driver
#include <Adafruit_TCA8418_registers.h> //Key Matrix driver
#include <TM1640Anode.h>                //TM1640 for OP Display
#include <TM1640.h>                     //TM1640 for COMM Display
#include <TM16xxMatrix.h>               //TM1640 matrix for COMM Display
#include <TM16xxMatrix16.h>             //TM1640 matrix for COMM Displa
#include "HT1621_OH.h"                  //modified HT1621 lib used for scratchpad
#include "CHARSET.h"                    //Custom charset
#include <PCA95x5.h>                    //PCA9555 AP Keyboard
#include <FastLED.h>                    //WS2812B Driver
#include <Adafruit_ADS1X15.h>           //ADS1115 4CH 16bit ADC

/**
 * @brief DCS-BIOS Includes
 */
//#define USE_MATRIX_SWITCHES  // were not using matrix switches so disable them
#define DCSBIOS_DISABLE_SERVO  // we're not using servo's so disable them
#include "DCS_State_Checker.h" // DCS State Checker for monitoring DCS status and aircraft telemetry
#include "DcsBios.h"
#include "MatPotentiometers.h"  // Mat Potentiometer input class that accepts a 16bit value for the pot position, with optional hysteresis and EWMA smoothin
#include "UFC_State_Checker.h"  // UFC power-source detection and operational state machine

/**
 * @brief Pin Assignments 
 */
// ══════════════════════════════════════════════════════════════════════════════
// Pin Assignments
// ══════════════════════════════════════════════════════════════════════════════
#define SDApin          35
#define SCLpin          36
#define IO_INT          15
#define BL_DATA_PIN     13
#define ENC1PB          33
#define ENC1A           37
#define ENC1B           38
#define ENC2PB          34
#define ENC2A           21
#define ENC2B           17
#define DIO_3_3V        12
#define OP1_CLK_3_3V    5
#define OP2_CLK_3_3V    4
#define OP3_CLK_3_3V    6
#define OP4_CLK_3_3V    7
#define OP5_CLK_3_3V    18
#define CUE_CLK_3_3V    44
#define VCC_SNS_3_3V    16
#define COM1_CLK_3_3V   43
#define COM2_CLK_3_3V   8
#define ADC_RDY_3_3V    1
#define SCRCS_3_3V      14
#define SCRWR_3_3V      9
#define SCRDATA_3_3V    10
#define SCRBL_3_3V      11

// ══════════════════════════════════════════════════════════════════════════════
// TCA8418 / PCA9555 GPIO Pin Numbers
// ══════════════════════════════════════════════════════════════════════════════
/**
 * @brief TCA8418 GPIO PINS USED OR NOT FOR COMPLETENESS
 */
#define UFC_IPSW          5
#define UFC_BITSW         12
#define UFC_EMCON         17
#define UFC_GO            14
#define UFC_NOGO          13
#define UFC_ADF1          15
#define UFC_ADF2          16
#define UFC_PB_0          0
#define UFC_PB_1          1
#define UFC_PB_2          2
#define UFC_PB_3          3
#define UFC_PB_4          4
#define UFC_PB_5          5
#define UFC_PB_6          6
#define UFC_PB_7          7
#define UFC_PB_8          8
#define UFC_PB_9          9
#define UFC_PB_CLR        10
#define UFC_PB_ENT        11
#define UFC_PB_OP1        12
#define UFC_PB_OP2        13
#define UFC_PB_OP3        14
#define UFC_PB_OP4        15
#define UFC_PB_OP5        16
#define UFC_PB_IP         17
#define UFC_PB_BIT        18
#define UFC_SW_ADF1       19
#define UFC_SW_ADF2       20
#define UFC_SW_ADFOFF     21
#define UFC_PB_COMM1      22
#define UFC_PB_COMM2      23
#define UFC_PB_COMM1_INC  24  
#define UFC_PB_COMM2_INC  25
#define UFC_PB_COMM1_DEC  26
#define UFC_PB_COMM2_DEC  27

/**
 * @brief TCA95xx GPIO PINS
 */
#define AP_PB_AP          28
#define AP_PB_IFF         29
#define AP_PB_TCN         30
#define AP_PB_ILS         31
#define AP_PB_DL          32
#define AP_PB_BCN         33
#define AP_PB_ONOFF       34

// ══════════════════════════════════════════════════════════════════════════════
// Runtime & Peripheral State
// ══════════════════════════════════════════════════════════════════════════════
/**
 * @brief ADS1115 channel assignments — maps each physical pot to an ADS1115 input pin and
 * the corresponding MUX constant used in startADCReading(). The _CH values are the
 * state-machine indices; the _MUX values are the Adafruit library constants.
 *
 * @note  Needed in BOTH interrupt-driven (USE_ADC_RDY) and polling modes, so this
 *        block is intentionally NOT guarded by #if defined(USE_ADC_RDY).
 */
#define ADS_CH_COMM1_VOL   1    // A0 = COMM1 volume pot
#define ADS_CH_COMM2_VOL   0    // A1 = COMM2 volume pot
#define ADS_CH_UFC_BRT     2    // A2 = UFC brightness pot

#define ADS_MUX_COMM1_VOL  ADS1X15_REG_CONFIG_MUX_SINGLE_0  // A0
#define ADS_MUX_COMM2_VOL  ADS1X15_REG_CONFIG_MUX_SINGLE_1  // A1
#define ADS_MUX_UFC_BRT    ADS1X15_REG_CONFIG_MUX_SINGLE_2  // A2

#if defined ( USE_ADC_RDY )
/**
 * @brief ADS1115 I2C Analog port resolution and width
 */
#define ADS1115_RESULUTION  16
#define ADS1115_WIDTH       65535
#define ADS1115_MIN_WIDTH   0
#endif

 /**
 * @brief State of DCS
 */
#define DCSRUNNING true
#define DSCSTOPPED false

/**
 * @brief WS2812 Backlighting
 */
#define UFC_BL_LEDS 117
#define AP_BL_LEDS (4*7)
#define BACKLIGHT_LEDS (UFC_BL_LEDS + AP_BL_LEDS)
// Define the array of leds
CRGB ws2812b[BACKLIGHT_LEDS];

/** @brief Raw ADS1115 reading for the COMM1 volume potentiometer (A0). Written by the ADS1115 ISR state machine; read by the MatPotentiometer DCS-BIOS input. */
uint16_t comm1Vol  = 0;
/** @brief Raw ADS1115 reading for the COMM2 volume potentiometer (A1). Written by the ADS1115 ISR state machine; read by the MatPotentiometer DCS-BIOS input. */
uint16_t comm2Vol  = 0;
/** @brief Raw ADS1115 reading for the UFC brightness potentiometer (A2). Renamed from ufcBrt to avoid a name collision with the MatPotentiometer instance declared later. */
uint16_t ufcBrtVal = 0;

/** @brief Convert a signed ADS1115 reading to unsigned, flooring small negative readings at 0. */
static inline uint16_t clampAdc(int16_t raw) {
  return (raw < 0) ? 0 : (uint16_t)raw;
}

/** @brief Simple local logging over USB serial only -- no WiFi/syslog dependency. */
#define _UFC_LOG(fmt, ...)  Serial.printf("[UfcState] " fmt "\n", ##__VA_ARGS__)

/**
 * @brief  When true, scan the I2C bus during setup() and print all found addresses.
 *
 * @details Set to true temporarily during board bring-up or fault diagnosis to
 *          enumerate attached I2C devices.  Leave false for normal operation to
 *          avoid adding setup latency.
 */
bool I2C_SCAN = false;

/** @brief Strings shown on the five option displays during the boot splash screen. Indices map to OP1-OP5. */
static const char* const BOOT_OP_MESSAGES[]        = {"MODE", "BIOS", "BLD", "H631", "V1.0"};
/** @brief Strings shown on the COMM1 and COMM2 displays during the boot splash screen. */
static const char* const BOOT_COMM_MESSAGES[]      = {" 1", " 2"};
/** @brief Strings shown on the scratchpad string and number regions during the boot splash screen. */
static const char* const BOOT_SCRATCHPAD_MESSAGES[] = {" O H", " FA18-C"};

/**
 * @brief  DCS-BIOS common metadata received from the simulator.
 *
 * @details Populated by DCS-BIOS callbacks and used throughout the firmware
 *          to track simulator state, aircraft identity, and communication health.
 */
typedef struct
{
  uint8_t  MissionStart;       ///< DCSRUNNING or DSCSTOPPED; set by DCS-BIOS aircraft-name callback.
  char     AircraftName[24];   ///< Null-terminated aircraft identifier string (e.g. "FA-18C_hornet").
  uint16_t AltMslFtChange;     ///< Reserved for future altitude change tracking.
  char     DCSBIOSVersion[6];  ///< DCS-BIOS library version string received from the simulator.
  char     PilotName[24];      ///< Null-terminated pilot name string from DCS-BIOS metadata.
  uint8_t  UpdateCounter;      ///< Rolling counter incremented each DCS-BIOS export frame (wraps at 255).
  uint8_t  SkipCounter;        ///< Number of DCS-BIOS frames skipped since last update.
} DCSCommondata;

/** @brief Global instance of the DCS-BIOS metadata structure. Populated by DCS-BIOS callbacks in loop(). */
DCSCommondata DCSMetaData;

/** @brief Current brightness level (0-7) applied to all TM1640 option and COMM displays. Updated by UFC_BRTChange(). */
int  UCF_brightness = 7;
/** @brief True when the UFC display subsystem is active (brightness > 0). Used by TM1640 setupDisplay() calls. */
bool UFC_active     = true;

/** @brief UFC power-source and operational state monitor. Owns VCC_SNS pin and replaces the former powerSource() function. */
UfcState         ufcState(VCC_SNS_3_3V);

/** @brief TCA8418 keyboard matrix controller for the main UFC keypad (9×10 matrix + GPIO expander). */
Adafruit_TCA8418 UFCkeypad;
/** @brief PCA9555 16-bit GPIO expander for the autopilot pushbutton panel. */
PCA9555          APkeypad;
/** @brief HT1621 scratchpad LCD driver (number + string display with PWM backlight). */
HT1621_OH        scratchpad;
/** @brief ADS1115 16-bit 4-channel I2C ADC used for COMM1/COMM2 volume and UFC brightness pots. */
Adafruit_ADS1115 ads;

/** @brief Set true by TCA8418_irq() when the keypad signals a new event. Cleared in loop() before processing. */
volatile bool TCA8418_event = false;
/** @brief Cumulative count of TCA8418 interrupts received (diagnostic / overflow detection). */
uint32_t      _tca8418_cnt  = 0;

/** @brief Set true by ADS1115_irq() when a conversion result is ready. Cleared in loop() before reading. */
volatile bool ADS1115_event = false;
/** @brief Cumulative count of ADS1115 DRDY interrupts received (diagnostic). */
uint32_t      _ads1115_cnt  = 0;
/** @brief Pin alias for the ADS1115 DRDY interrupt line; mirrors ADC_RDY_3_3V. */
constexpr int READY_PIN     = ADC_RDY_3_3V;

/**
 * @brief  Debounced state cache for the autopilot pushbutton panel.
 *
 * @details apkeys[0][0..6] correspond to: AP, IFF, TCN, ILS, D/L, BCN, ON/OFF.
 *          Written by processAPKeypad(); read by the DcsBios::MatActionButtonSet instances.
 */
uint8_t apkeys[1][10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/** @brief Number of columns driven by each COMM display TM1640 matrix. */
#define OP_DIGITS       4
#define COM1_DIGITS     1
#define COM2_DIGITS     1
#define COM_DIGITS      4

/** @brief Enumeration of the five CUE indicator positions (CUE1 = left-most). */
enum { CUE1, CUE2, CUE3, CUE4, CUE5 };

/**
 * @brief  Physical column order mapping for TM1640 option display digits.
 *
 * @details The TM1640Anode driver addresses columns 0-3.  The PCB routes
 *          the segments so that the logical digit order is {3,0,1,2}.
 *          sendAsciiChar_OH() indexes this array to find the correct column.
 */
uint8_t op_order[4] = {3, 0, 1, 2};

/** @brief Enumeration of the five UFC option display positions (OP1 = top). */
enum { OP1, OP2, OP3, OP4, OP5 };

/**
 * @brief  TM1640Anode driver array for the five option displays and the CUE indicator.
 *
 * @details Indices 0-4 drive option displays OP1-OP5; index 5 drives the CUE
 *          indicator row.  All share the same DIO_3_3V data line; each has its
 *          own clock line.
 */
TM1640Anode GN1640[6] {
  TM1640Anode(DIO_3_3V, OP1_CLK_3_3V, OP_DIGITS,  true, UCF_brightness), ///< OP Display 1
  TM1640Anode(DIO_3_3V, OP2_CLK_3_3V, OP_DIGITS,  true, UCF_brightness), ///< OP Display 2
  TM1640Anode(DIO_3_3V, OP3_CLK_3_3V, OP_DIGITS,  true, UCF_brightness), ///< OP Display 3
  TM1640Anode(DIO_3_3V, OP4_CLK_3_3V, OP_DIGITS,  true, UCF_brightness), ///< OP Display 4
  TM1640Anode(DIO_3_3V, OP5_CLK_3_3V, OP_DIGITS,  true, UCF_brightness), ///< OP Display 5
  TM1640Anode(DIO_3_3V, CUE_CLK_3_3V, COM_DIGITS, true, UCF_brightness)  ///< CUE indicators
};

/**
 * @brief  Per-display content and cue state for the five option displays.
 *
 * @details Updated by UFC_OPTION_DISPLAY_xChange() and UFC_OPTION_CUEING_xChange()
 *          callbacks.  update_op_display() and update_op_cue() read from here to
 *          refresh the hardware.
 */
struct op_display_content {
  bool cueing;        ///< True when the cue (arrow) indicator for this option is active.
  char content[5];    ///< 4-character display string plus null terminator.
};

/** @brief Content/cue state array for all five option displays (OP1-OP5). */
op_display_content op_display[5];

/** @brief TM1640Anode driver for the COMM1 single-digit radio channel display. */
TM1640Anode COMM1(DIO_3_3V, COM1_CLK_3_3V, COM1_DIGITS, true, UCF_brightness);
/** @brief TM1640Anode driver for the COMM2 single-digit radio channel display. */
TM1640Anode COMM2(DIO_3_3V, COM2_CLK_3_3V, COM2_DIGITS, true, UCF_brightness);

/**
 * @brief  TM1640 base-layer drivers for the COMM matrix displays.
 *
 * @details These two TM1640 instances provide the underlying hardware interface
 *          shared by the TM16xxMatrix / TM16xxMatrix16 wrappers.  Index 0 = COMM1,
 *          index 1 = COMM2.
 */
TM1640 GN1640_COMM[2] {
  TM1640(DIO_3_3V, COM1_CLK_3_3V),
  TM1640(DIO_3_3V, COM2_CLK_3_3V)
};

/**
 * @brief  4×4 matrix display wrappers for COMM1 and COMM2.
 *
 * @details Used by display_comm_char() to write column data for the 14-segment
 *          radio channel characters.
 */
TM16xxMatrix matrix_COMM[2] {
  TM16xxMatrix(&GN1640_COMM[0], 4, 4),
  TM16xxMatrix(&GN1640_COMM[1], 4, 4)
};

/** @brief Dedicated 4×4 matrix wrapper for COMM1 (same underlying TM1640 as matrix_COMM[0]). */
TM16xxMatrix   matrix_COMM1(&GN1640_COMM[0], 4, 4);
/** @brief 8×16 extended matrix wrapper for COMM2, used to drive CUE4/CUE5 segments. */
TM16xxMatrix16 matrix_COMM2(&GN1640_COMM[1], 8, 16);

/** @brief Last COMM2 character string passed to display_comm_char(); used by cue() to refresh COMM2 after drawing a CUE segment. */
static char COMM2_CHAR[3];

/** @brief TM1640Anode driver for CUE indicators 1-3 (shares CUE_CLK_3_3V with GN1640[5]). */
TM1640Anode CUE123(DIO_3_3V, CUE_CLK_3_3V, 8, true, UCF_brightness);

// ── GO / NOGO LED state ──────────────────────────────────────────────────────
/** @brief millis() timestamp when the GO or NOGO LED was last turned on; used for the 10-second auto-off timeout. */
static unsigned long ledOnTime       = 0;
/** @brief True while the GO LED is on (DCS mission running indicator). */
static bool goLedOn                  = false;
/** @brief True while the NOGO LED is on (DCS not running indicator). */
static bool noGoLedOn                = false;
/** @brief Mission state observed on the previous updateGoNoGoLedNonBlocking() call; detects state transitions. */
static uint8_t lastMissionState      = DSCSTOPPED;

// ── NOGO breathing effect ────────────────────────────────────────────────────
/** @brief millis() timestamp of the last breatheNogoLed() LED toggle. */
static unsigned long lastBreathUpdate = 0;
/** @brief Minimum milliseconds between breatheNogoLed() LED updates (~60 fps). */
static const unsigned long BREATH_INTERVAL = 16;
/** @brief Full period of the NOGO LED square-wave breathing cycle in milliseconds. */
static const float BREATH_PERIOD     = 2000.0f;

/** @brief True when no DCS data has been received and all UFC displays are blank. Controls breatheNogoLed() activation. */
static bool displayBlank             = true;

/** @brief GO/NOGO LED on-time in milliseconds before auto-off. */
#define TIMEOUT 10000

/** @brief millis() timestamp when the current boot message splash was shown; used for the 10-second display timeout. */
static unsigned long bootMessageStartTime = 0;
/** @brief True while the boot message splash is being displayed on the UFC screens. */
static bool bootMessageActive             = false;
/** @brief Length of the version splash, counted from when it is drawn. */
#define BOOT_SPLASH_MS 10000
/** @brief Length of each of the two built-in test periods (outer segments, then inner segments). */
#define BIT_PHASE_MS 5000
/** @brief Wait after power-up before the built-in test first lights the displays, so they have settled. */
#define BIT_START_DELAY_MS 2000

/** @brief Built-in test period now showing: 0 = not running, 1 = outer segments, 2 = inner segments. */
static uint8_t bitPhase                   = 0;
/** @brief True once the first test period has been drawn (false while waiting BIT_START_DELAY_MS). */
static bool bitShown                      = false;
/** @brief millis() when the current test period started (before the first one: when the wait began). */
static unsigned long bitPhaseStart        = 0;
/** @brief True from setup() until the first mission-state check: the power-on test was armed in setup(). */
static bool powerOnSequence               = false;


// ════════════════════════════════════════════════════════════════════════════
// Interrupt Service Routines
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief  TCA8418 keypad controller interrupt service routine.
 *
 * @details Triggered on a CHANGE edge of IO_INT (pin 15) when the TCA8418
 *          detects a key press/release event or a GPIO state change on one of
 *          its expander pins.  Sets the TCA8418_event flag so that loop() can
 *          call processUFCKeypad() and processUFCIO() at the next opportunity,
 *          and increments the diagnostic counter _tca8418_cnt.
 *
 * @note    Placed in IRAM (IRAM_ATTR) so the ISR executes from fast internal
 *          RAM rather than slower flash on the ESP32-S3.
 */
void IRAM_ATTR TCA8418_irq() {
  TCA8418_event = true;
  _tca8418_cnt++;
}

#if defined ( USE_ADC_RDY )

/**
 * @brief  ADS1115 DRDY (data-ready) interrupt service routine.
 *
 * @details Triggered on a FALLING edge of ADC_RDY_3_3V (pin 1) when the
 *          ADS1115 completes a single-shot conversion.  Sets ADS1115_event so
 *          that loop() reads the result and starts the next channel conversion,
 *          and increments the diagnostic counter _ads1115_cnt.
 *
 * @note    Only compiled when USE_ADC_RDY is defined.
 *          Placed in IRAM (IRAM_ATTR) for fast execution.
 */
void IRAM_ATTR ADS1115_irq() {
  ADS1115_event = true;
}
#endif

// ════════════════════════════════════════════════════════════════════════════
// Input Processing Functions
// ════════════════════════════════════════════════════════════════════════════

/** @brief Last ADF switch position handled by processUFCIO(); 0xFF forces the next poll to send it again. */
static uint8_t lastadf = 0xFF;

/**
 * @brief  Read UFC controls that are not part of the TCA8418 key matrix.
 *
 * @details Reads three GPIO pins through the TCA8418 expander (UFC_IPSW,
 *          UFC_EMCON, UFC_BITSW) and writes their states to UFCkeys and
 *          HUDkeys arrays.  Also reads the two ADF selector switch pins,
 *          encodes their combined state into a 3-bit value, and sends the
 *          appropriate "UFC_ADF" DCS-BIOS message when the state changes.
 *
 *          ADF encoding:
 *          - adfsw == 2 (ADF1=0, ADF2=1) → ADF position 2
 *          - adfsw == 4 (ADF1=1, ADF2=0) → ADF position 0 (off)
 *          - adfsw == 6 (ADF1=1, ADF2=1) → ADF position 1
 *
 * @note    Called from loop() after a TCA8418 interrupt event is detected.
 */
void processUFCIO() {

  // Poll gate: at most once per millisecond, same as processUFCKeypad(). Longer gates
  // (3 ms) made the buttons feel delayed.
  static unsigned long lastIoPollTime = 0;
  const unsigned long currentPollTime = millis();
  if (currentPollTime - lastIoPollTime < 1) return;
  lastIoPollTime = currentPollTime;

  // Direct GPIO reads (not part of the key matrix). Investigated as a possible cause of
  // pushbuttons not responding in-game: Adafruit_TCA8418 has no readGPIO() method (confirmed
  // against the library source), and digitalRead() below already performs a live I2C read of
  // TCA8418_REG_GPIO_DAT_STAT_* on every call, so no separate refresh is needed — this code was
  // not the cause of that symptom.
  UFCkeys[0][0] = UFCkeypad.digitalRead(UFC_IPSW);  // IP Switch
  UFCkeys[0][1] = UFCkeypad.digitalRead(UFC_EMCON); // EMCON Switch
  HUDkeys[0][0] = UFCkeypad.digitalRead(UFC_BITSW); // HUD Video Bit

  // Process ADF switch: two GPIO pins encode a 3-position rotary switch.
  const uint8_t adfsw = (UFCkeypad.digitalRead(UFC_ADF1) << 2) | (UFCkeypad.digitalRead(UFC_ADF2) << 1);

  if (lastadf != adfsw) {
    // Lookup table instead of switch/case for a direct, branch-free translation.
    static const char* const adf_values[] = {"0", "INVALID", "2", "INVALID", "0", "INVALID", "1", "INVALID"};
    if (adfsw == 2 || adfsw == 4 || adfsw == 6) {
      // If DCS-BIOS cannot take the message yet, keep lastadf as it was so the next poll tries again.
      if (!DcsBios::tryToSendDcsBiosMessage("UFC_ADF", adf_values[adfsw])) return;
    }
    lastadf = adfsw;
  }
}

/**
 * @brief  Service a pending TCA8418 keypad interrupt.
 *
 * @details Reads the TCA8418 interrupt status register to determine whether a
 *          GPIO change or a key-matrix event occurred:
 *          - GPIO interrupt (bit 1): reads and clears all three GPIO interrupt
 *            status registers, then clears the GPIO IRQ flag.
 *          - Key event (bit 0): retrieves the key code, extracts press/release
 *            state (bit 7), converts the raw code to (row, col) coordinates in
 *            the UFCkeys matrix, and clears the EVENT IRQ flag.
 *
 *          UFCkeys[row][col] is set to 1 on press and 0 on release.  These
 *          values are read by the DcsBios::MatActionButtonSet instances.
 *
 * @note    Called from loop() after TCA8418_event is detected and cleared.
 */
void processUFCKeypad() {
  // Poll gate: at most once per millisecond (~1 kHz) instead of running flat-out on
  // every loop() iteration. This keeps keys responsive; a 3 ms gate made the panel
  // feel delayed. Still well inside the TCA8418's 10-entry event FIFO.
  static unsigned long lastKeypadPollTime = 0;
  const unsigned long currentPollTime = millis();
  if (currentPollTime - lastKeypadPollTime < 1) return;
  lastKeypadPollTime = currentPollTime;

  const uint8_t intStat = UFCkeypad.readRegister(TCA8418_REG_INT_STAT);

  // Handle GPIO interrupts
  if (intStat & 0x02) {
    // Read all GPIO status registers in sequence to clear them
    UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_1);
    UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_2);
    UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_3);
    // Clear GPIO IRQ flag
    UFCkeypad.writeRegister(TCA8418_REG_INT_STAT, 2);
  }

  // Handle keypad events
  if (intStat & 0x01) {
    const uint8_t keyCode = UFCkeypad.getEvent();
    const bool state = keyCode > 127;
    const uint8_t cleanKeyCode = (keyCode & 0x7F) - 1;
    
    // Direct matrix calculation
    const uint8_t row = cleanKeyCode / 10;
    const uint8_t col = cleanKeyCode % 10;
    
    // Ignore events outside the key matrix (switch GPIO events, empty FIFO read).
    if (row < TCA8418_ROWS && col < TCA8418_COLS) {
      UFCkeys[row][col] = state ? 1 : 0;
      if (state) {
        // O/S BRG only: tell the scratchpad which key was pressed (see HT1621_OH::noteKey).
        // Matrix rows 1-3 are digits 1-9; row 4 is CLR, 0, ENT.
        if (col < 3) {
          if (row >= 1 && row <= 3)      scratchpad.noteKey('1' + (row - 1) * 3 + col);
          else if (row == 4)             scratchpad.noteKey(col == 0 ? 'C' : col == 1 ? '0' : 'E');
        }
      }
    }

    // Clear the EVENT IRQ flag
    UFCkeypad.writeRegister(TCA8418_REG_INT_STAT, 1);
  }
}

/**
 * @brief  Poll the PCA9555 autopilot keypad and update the apkeys state cache.
 *
 * @details Reads all 16 PCA9555 pins in a single I2C transaction, masks off
 *          the seven monitored pins (P03, P04, P05, P13, P14, P15, P16), and
 *          applies a 10 ms software debounce before writing the stable states
 *          to apkeys[0][0..6] (AP, IFF, TCN, ILS, D/L, BCN, ON/OFF).
 *
 *          A 1 ms poll gate prevents redundant I2C reads on every loop()
 *          tick; the gate and debounce timers share the same captured millis()
 *          value to avoid calling millis() twice.
 *
 *          apkeys values are read by the DcsBios::MatActionButtonSet instances
 *          for the autopilot panel.
 *
 * @note    Called unconditionally from loop() (poll-driven, no interrupt).
 */
 void processAPKeypad() {
  // APkeypad.read() is a full I2C transaction. The poll gate is 1 ms; the 10 ms debounce below
  // still filters contact bounce.
  static unsigned long lastPollTime = 0;
  const unsigned long currentPollTime = millis();
  if (currentPollTime - lastPollTime < 1) return;
  lastPollTime = currentPollTime;

  static uint16_t lastPortState = 0;
  static unsigned long lastChangeTime = 0;
  static const unsigned long DEBOUNCE_DELAY = 10; // 10ms debounce
  
  // Read all 16 pins in a single I2C transaction
  const uint16_t currentPortState = APkeypad.read();
  
  // Check if any of our monitored pins have changed
  // Pins we monitor: P03, P04, P05, P13, P14, P15, P16
  const uint16_t monitoredPinsMask = (1 << PCA95x5::Port::P03) | 
                                     (1 << PCA95x5::Port::P04) | 
                                     (1 << PCA95x5::Port::P05) | 
                                     (1 << PCA95x5::Port::P13) | 
                                     (1 << PCA95x5::Port::P14) | 
                                     (1 << PCA95x5::Port::P15) | 
                                     (1 << PCA95x5::Port::P16);
  
  const uint16_t relevantCurrentState = currentPortState & monitoredPinsMask;
  const uint16_t relevantLastState = lastPortState & monitoredPinsMask;
  
  // Only process if there's a change in monitored pins
  if (relevantCurrentState != relevantLastState) {
    // Reuse already-captured poll time instead of calling millis() again
    if (currentPollTime - lastChangeTime >= DEBOUNCE_DELAY) {
      // Extract individual pin states using bit operations
      apkeys[0][0] = (currentPortState >> PCA95x5::Port::P13) & 1;  // AP
      apkeys[0][1] = (currentPortState >> PCA95x5::Port::P14) & 1;  // IFF
      apkeys[0][2] = (currentPortState >> PCA95x5::Port::P15) & 1;  // TCN
      apkeys[0][3] = (currentPortState >> PCA95x5::Port::P16) & 1;  // ILS
      apkeys[0][4] = (currentPortState >> PCA95x5::Port::P03) & 1;  // D/L
      apkeys[0][5] = (currentPortState >> PCA95x5::Port::P04) & 1;  // BCN
      apkeys[0][6] = (currentPortState >> PCA95x5::Port::P05) & 1;  // ON/OFF
      
      lastPortState = currentPortState;
      lastChangeTime = currentPollTime;
    }
  }
}

// ════════════════════════════════════════════════════════════════════════════
// Display Helper Functions
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief  Write a single ASCII character to one digit position of a TM1640 option display.
 *
 * @details Looks up the 16-bit segment pattern for @p c in the OP_DISPLAY_FONT
 *          PROGMEM table (indexed as c - 32, so space = index 0).  Characters
 *          below ASCII 32 are clamped to space to prevent a font-table underflow.
 *          The segment pattern is then sent to GN1640[op_display] at the physical
 *          column address given by op_order[pos].
 *
 * @param op_display  Index into GN1640[] (0-4 for OP1-OP5; 5 for the CUE display).
 * @param pos         Logical digit position within the display (0 = leftmost).
 *                    Translated to a physical column via op_order[].
 * @param c           ASCII character to render.  Characters < 32 are treated as space.
 */
void sendAsciiChar_OH(int op_display, uint8_t pos, char c) {
  if (c < 32) c = 32; 
  uint16_t uSegments = pgm_read_word(OP_DISPLAY_FONT + (c - 32));
  GN1640[op_display].setSegments16(uSegments, op_order[pos]);
}

/**
 * @brief  Refresh all four digit positions of one option display from its content buffer.
 *
 * @details Iterates over op_display[disp_num].content[0..3] and calls
 *          sendAsciiChar_OH() for each digit, translating each ASCII character
 *          to the appropriate 16-bit TM1640 segment pattern.  Called whenever
 *          DCS-BIOS delivers a new string for that display position.
 *
 * @param disp_num  Option display index to refresh (0 = OP1, 1 = OP2, … 4 = OP5).
 */
void update_op_display(int disp_num) {
  for (int digit_num = 0; digit_num < OP_DIGITS; digit_num++) {
    sendAsciiChar_OH(disp_num, digit_num, op_display[disp_num].content[digit_num]);
  }

  // The scratchpad backlight is only on while something is lit on the scratchpad or on an option
  // window. Tell the scratchpad driver whether any option window now has a visible character.
  bool optionLit = false;
  for (int i = 0; i < 5 && !optionLit; i++) {
    for (int d = 0; d < OP_DIGITS; d++) {
      if (op_display[i].content[d] > ' ') { optionLit = true; break; }
    }
  }
  scratchpad.setOptionContent(optionLit);
}

/**
 * @brief Displays a character on the COMM display
 * @param comm_char Pointer to the character string to display
 * @param n_display Display number (0 or 1)
 * @details Handles both single characters and special sequences for the COMM display.
 * The function supports:
 * - Single digits (1-9)
 * - Special characters (M, G, C, S)
 * - Two-digit numbers (10-20)
 * - Special sequences (<>, ==, ++)
 */
// Optimized: previously used a CharMapping struct + 14-entry SPECIAL_CHARS PROGMEM table scanned
// with strcmp_P on every call. The table is unnecessary — every index is directly computable from
// the two input characters with simple comparisons and arithmetic. Also fixes incorrect PROGMEM
// usage: the struct stored const char* pointers in flash, but strcmp_P treated those pointer values
// as flash addresses rather than reading them via pgm_read_ptr first (broken on AVR, silent on ESP32).
// See display_comm_char2() for the original implementation.
void display_comm_char(char* comm_char, int n_display) {
  if (!comm_char || n_display < 0 || n_display > 1) return;

  uint8_t array_pos = 0;

  if (comm_char[0] != '\0') {
    const char c0 = comm_char[0];
    const char c1 = comm_char[1];

    if (c1 != '\0') {
      if      (c0 == '1' && c1 >= '0' && c1 <= '9') array_pos = 10 + (uint8_t)(c1 - '0'); // "10"-"19"
      else if (c0 == '2' && c1 == '0')               array_pos = 20;                        // "20"
      else if (c0 == '<' && c1 == '>')               array_pos = 25;                        // "<>"
      else if (c0 == '=' && c1 == '=')               array_pos = 27;                        // "=="
      else if (c0 == '+' && c1 == '+')               array_pos = 28;                        // "++"
      else {
        switch (c1) {
          case '1': case '2': case '3': case '4': case '5':
          case '6': case '7': case '8': case '9':
            array_pos = (uint8_t)(c1 - '0'); break;
          case 'M': array_pos = 21; break;
          case 'G': array_pos = 22; break;
          case 'C': array_pos = 23; break;
          case 'S': array_pos = 24; break;
        }
      }
    }
  }

  matrix_COMM[n_display].setAll(false);
  const uint8_t* char_set = (n_display == 1) ? COMM_CHAR2[array_pos] : COMM_CHAR1[array_pos];
  for (uint8_t i = 0; i < 4; i++) {
    matrix_COMM[n_display].setColumn(i, pgm_read_byte(&char_set[i]));
  }
  // Remember COMM2's text only; cue() redraws COMM2 from it.
  // strncpy + terminator keeps it within COMM2_CHAR[3].
  if (n_display == 1) {
    strncpy(COMM2_CHAR, comm_char, sizeof(COMM2_CHAR) - 1);
    COMM2_CHAR[sizeof(COMM2_CHAR) - 1] = '\0';
  }
}

/**
 * @brief  Segment pattern and display column address for each CUE indicator.
 *
 * @details CUE1-3 are driven through the CUE123 TM1640Anode display;
 *          CUE4-5 share the COMM2 display area.  Each entry provides the
 *          16-bit segment mask to set (or 0x0000 to clear) and the column
 *          address passed to setSegments16().
 */
static const struct {
  uint16_t segment;  ///< TM1640 segment bit mask for this CUE indicator.
  uint8_t  position; ///< TM1640 column (grid) address for this CUE indicator.
} CUE_CONFIG[] = {
  {0x0001, 0b0100},      ///< CUE1 — SEG1 active, grid 3
  {0x0002, 0b0011},      ///< CUE2 — SEG2 active, grid 4
  {0x0004, 0b0010},      ///< CUE3 — SEG3 active, grid 5
  {0x4000, 0b00000111},  ///< CUE4 — uses COMM2 display, SEG15
  {0x8000, 0b00000110}   ///< CUE5 — uses COMM2 display, SEG16
};

/**
 * @brief  Turn a single CUE indicator on or off.
 *
 * @details Looks up the segment mask and column address from CUE_CONFIG[pos].
 *          CUE1-3 are written directly to the CUE123 TM1640Anode display.
 *          CUE4-5 share the COMM2 TM1640Anode display and require a
 *          display_comm_char() refresh after writing the CUE segment so that
 *          the channel character is not erased.
 *
 * @param pos    CUE position to control (CUE1 … CUE5).  Values > CUE5 are ignored.
 * @param state  true = illuminate the CUE arrow; false = extinguish it.
 */
void cue(const uint8_t pos, const bool state) {
  // Input validation
  if (pos > CUE5) return;

  // Get configuration for this CUE
  const uint16_t segment = CUE_CONFIG[pos].segment;
  const uint8_t position = CUE_CONFIG[pos].position;

  // Set segments based on state
  const uint16_t value = state ? segment : 0x0000;

  // CUE1-3 use CUE123 display
  if (pos <= CUE3) {
    CUE123.setSegments16(value, position);
  }
  // CUE4-5 use COMM2 display
  else {
    COMM2.setSegments16(value, position);
    display_comm_char(COMM2_CHAR, 1); // Refresh COMM2 display
  }
}

/**
 * @brief  Refresh the CUE indicator for one option display from its cueing flag.
 *
 * @details Reads op_display[disp_num].cueing and drives the corresponding CUE
 *          segment on GN1640[5] (CUE indicators 1-3) or matrix_COMM2 (CUE indicators 4-5).
 *          Called by bootMessage() during the splash sequence.  For runtime updates
 *          driven by DCS-BIOS, cue() is used directly via handleOptionCueingChange().
 *
 * @param disp_num  Option display index whose CUE should be updated (1-5).
 *                  Case 0 has no CUE and is ignored.
 */
void update_op_cue(int disp_num) {
  cue(disp_num, op_display[disp_num].cueing);
}


// ════════════════════════════════════════════════════════════════════════════
// DCS-BIOS Callbacks
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief  Common handler for all five UFC option cueing state changes.
 *
 * @details DCS-BIOS sends a single character for each option cue indicator:
 *          ':' = cue active (arrow lit), any other character = cue inactive.
 *          Calls cue() with the decoded boolean state.
 *
 * @param newValue   Pointer to the 1-character DCS-BIOS cueing string.
 * @param cueNumber  CUE position to update (CUE1 … CUE5).
 */
inline void handleOptionCueingChange(const char* newValue, const uint8_t cueNumber) {
  cue(cueNumber, *newValue == ':');
}

/** @brief DCS-BIOS callback: UFC option 1 cue indicator state change. @param newValue Single character; ':' = cue on. */
void UFC_OPTION_CUEING_1Change(char* newValue) { handleOptionCueingChange(newValue, CUE1); }
DcsBios::StringBuffer<1> UFC_OPTION_CUEING_1(FA_18C_hornet_UFC_OPTION_CUEING_1_A, UFC_OPTION_CUEING_1Change);

/** @brief DCS-BIOS callback: UFC option 2 cue indicator state change. @param newValue Single character; ':' = cue on. */
void UFC_OPTION_CUEING_2Change(char* newValue) { handleOptionCueingChange(newValue, CUE2); }
DcsBios::StringBuffer<1> UFC_OPTION_CUEING_2(FA_18C_hornet_UFC_OPTION_CUEING_2_A, UFC_OPTION_CUEING_2Change);

/** @brief DCS-BIOS callback: UFC option 3 cue indicator state change. @param newValue Single character; ':' = cue on. */
void UFC_OPTION_CUEING_3Change(char* newValue) { handleOptionCueingChange(newValue, CUE3); }
DcsBios::StringBuffer<1> UFC_OPTION_CUEING_3(FA_18C_hornet_UFC_OPTION_CUEING_3_A, UFC_OPTION_CUEING_3Change);

/** @brief DCS-BIOS callback: UFC option 4 cue indicator state change. @param newValue Single character; ':' = cue on. */
void UFC_OPTION_CUEING_4Change(char* newValue) { handleOptionCueingChange(newValue, CUE4); }
DcsBios::StringBuffer<1> UFC_OPTION_CUEING_4(FA_18C_hornet_UFC_OPTION_CUEING_4_A, UFC_OPTION_CUEING_4Change);

/** @brief DCS-BIOS callback: UFC option 5 cue indicator state change. @param newValue Single character; ':' = cue on. */
void UFC_OPTION_CUEING_5Change(char* newValue) { handleOptionCueingChange(newValue, CUE5); }
DcsBios::StringBuffer<1> UFC_OPTION_CUEING_5(FA_18C_hornet_UFC_OPTION_CUEING_5_A, UFC_OPTION_CUEING_5Change);

//-------------- UFC OP Displays ---------------

/**
 * @brief  Common handler for all five UFC option display content changes.
 *
 * @details Validates inputs, copies the 4-character string into the
 *          op_display content buffer using memcpy, then calls
 *          update_op_display() to push the new content to the TM1640 hardware.
 *
 * @param displayNum  Option display index (0 = OP1 … 4 = OP5).
 *                    Returns false immediately if >= 5.
 * @param newValue    Pointer to a 4-character string from DCS-BIOS.
 *                    Returns false immediately if nullptr.
 *
 * @return @c true if the display was updated successfully.
 * @return @c false if @p displayNum is out of range or @p newValue is nullptr.
 */
inline bool updateUFCOptionDisplay(uint8_t displayNum, const char* newValue) {
  // Input validation
  if (!newValue || displayNum >= 5) {
    return false;
  }

  // Update display content using memcpy for efficiency
  memcpy(op_display[displayNum].content, newValue, 4);
  update_op_display(displayNum);
  return true;
}

/** @brief DCS-BIOS callback: UFC option display 1 content (4 chars). @param newValue 4-char option string. */
void UFC_OPTION_DISPLAY_1Change(char* newValue) { updateUFCOptionDisplay(OP1, newValue); }
DcsBios::StringBuffer<4> UFC_OPTION_DISPLAY_1(FA_18C_hornet_UFC_OPTION_DISPLAY_1_A, UFC_OPTION_DISPLAY_1Change);

/** @brief DCS-BIOS callback: UFC option display 2 content (4 chars). @param newValue 4-char option string. */
void UFC_OPTION_DISPLAY_2Change(char* newValue) { updateUFCOptionDisplay(OP2, newValue); }
DcsBios::StringBuffer<4> UFC_OPTION_DISPLAY_2(FA_18C_hornet_UFC_OPTION_DISPLAY_2_A, UFC_OPTION_DISPLAY_2Change);

/** @brief DCS-BIOS callback: UFC option display 3 content (4 chars). @param newValue 4-char option string. */
void UFC_OPTION_DISPLAY_3Change(char* newValue) { updateUFCOptionDisplay(OP3, newValue); }
DcsBios::StringBuffer<4> UFC_OPTION_DISPLAY_3(FA_18C_hornet_UFC_OPTION_DISPLAY_3_A, UFC_OPTION_DISPLAY_3Change);

/** @brief DCS-BIOS callback: UFC option display 4 content (4 chars). @param newValue 4-char option string. */
void UFC_OPTION_DISPLAY_4Change(char* newValue) { updateUFCOptionDisplay(OP4, newValue); }
DcsBios::StringBuffer<4> UFC_OPTION_DISPLAY_4(FA_18C_hornet_UFC_OPTION_DISPLAY_4_A, UFC_OPTION_DISPLAY_4Change);

/** @brief DCS-BIOS callback: UFC option display 5 content (4 chars). @param newValue 4-char option string. */
void UFC_OPTION_DISPLAY_5Change(char* newValue) { updateUFCOptionDisplay(OP5, newValue); }
DcsBios::StringBuffer<4> UFC_OPTION_DISPLAY_5(FA_18C_hornet_UFC_OPTION_DISPLAY_5_A, UFC_OPTION_DISPLAY_5Change);

/** @brief Last COMM channel text sent by DCS-BIOS, per display (0 = COMM1, 1 = COMM2). Empty until DCS sends something. */
static char dcsCommValue[2][3] = { "", "" };

/** @brief Store a COMM value from DCS-BIOS and draw it. */
static void storeAndDrawComm(char* newValue, int n_display) {
  if (!newValue) return;
  strncpy(dcsCommValue[n_display], newValue, 2);
  dcsCommValue[n_display][2] = '\0';
  display_comm_char(newValue, n_display);
}

/**
 * @brief  Redraw both COMM displays from the last values DCS-BIOS sent.
 *
 * @details Called on mission start. DCS-BIOS only calls back when a value changes, so channels
 *          unchanged since the last mission would otherwise stay blank after the splash clear.
 */
static void redrawCommFromDcs() {
  for (int n = 0; n < 2; n++) {
    if (dcsCommValue[n][0] != '\0') {
      display_comm_char(dcsCommValue[n], n);
    }
  }
}

/**
 * @brief  DCS-BIOS callback: COMM1 radio channel display content.
 *
 * @details Receives a 2-character string representing the currently selected
 *          COMM1 channel symbol (e.g. " 1", "15", " M") and draws it via
 *          storeAndDrawComm().
 *
 * @param newValue  Pointer to a 2-character null-terminated string from DCS-BIOS.
 */
void UFC_COMM_DISPLAY_1Change(char* newValue) {
  storeAndDrawComm(newValue, 0);
}
DcsBios::StringBuffer < 2 > UFC_COMM_DISPLAY_1(FA_18C_hornet_UFC_COMM1_DISPLAY_A, UFC_COMM_DISPLAY_1Change);

/**
 * @brief  DCS-BIOS callback: COMM2 radio channel display content.
 *
 * @details Same as UFC_COMM_DISPLAY_1Change() for display index 1 (COMM2).
 *
 * @param newValue  Pointer to a 2-character null-terminated string from DCS-BIOS.
 */
void UFC_COMM_DISPLAY_2Change(char* newValue) {
  storeAndDrawComm(newValue, 1);
}
DcsBios::StringBuffer < 2 > UFC_COMM_DISPLAY_2(FA_18C_hornet_UFC_COMM2_DISPLAY_A, UFC_COMM_DISPLAY_2Change);

/**
 * @brief  DCS-BIOS callback: UFC scratchpad numeric display content.
 *
 * @details Receives an 8-character string (leading space + 7 digits/chars) and
 *          forwards it to scratchpad.print_num() which encodes each character
 *          as a 7-segment pattern and updates the HT1621 hardware.
 *
 * @param newValue  8-character null-terminated scratchpad number string from DCS-BIOS.
 */
void onUfcScratchpadNumberDisplayChange(char* newValue) {
  scratchpad.print_num(newValue, false);
}
DcsBios::StringBuffer<8> UFC_SCRATCHPAD_NUMBER_DISPLAY(FA_18C_hornet_UFC_SCRATCHPAD_NUMBER_DISPLAY_A, onUfcScratchpadNumberDisplayChange);

/**
 * @brief  DCS-BIOS callback: UFC scratchpad alphanumeric string display (left region).
 *
 * @details Receives a 4-character string and forwards it to scratchpad.print_str()
 *          which encodes each character pair as a 16-segment pattern and updates
 *          the HT1621 hardware (the two large alphanumeric characters on the left
 *          of the scratchpad display).
 *
 * @param newValue  4-character null-terminated scratchpad string from DCS-BIOS.
 */
void onUfcScratchpadString1DisplayChange(char* newValue) {
  scratchpad.print_str(newValue, false);
}
DcsBios::StringBuffer<4> onUfcScratchpadString1DisplayBuffer(FA_18C_hornet_UFC_SCRATCHPAD_STRING_1_DISPLAY_A, onUfcScratchpadString1DisplayChange);

/**
 * @brief  DCS-BIOS callback: UFC scratchpad string display 2 (2-char field between
 *         the primary string display and the main 8-digit number field).
 *
 * @details This is a real, actively-populated DCS-BIOS control (confirmed in the
 *          FA-18C module source) — not a placeholder. It was previously discarded
 *          by this callback, so its content (e.g. the tightly-bundled middle pair
 *          of digits in a 10-digit grid coordinate entry, such as "59" in
 *          "8 59 4389771") never reached the display. Forwarded to
 *          scratchpad.print_str2(), which writes it into the digit "8"
 *          alphanumeric cell (_buffer16[2..3] / HT1621 addresses 4 and 6) — the
 *          physical cell that sits idle because STRING_1 (above) only ever
 *          supplies 2 real characters and drives just the digit "9" cell.
 *
 * @param newValue  Up to 4-character null-terminated string from DCS-BIOS (real
 *                  content is 2 characters; buffer sized to match print_str2()'s
 *                  read pattern, mirroring UFC_SCRATCHPAD_STRING_1_DISPLAY above).
 */
void onUfcScratchpadString2DisplayChange(char* newValue) {
  scratchpad.print_str2(newValue, false);
}
DcsBios::StringBuffer<4> ufcScratchpadString2DisplayBuffer(FA_18C_hornet_UFC_SCRATCHPAD_STRING_2_DISPLAY_A, onUfcScratchpadString2DisplayChange);

/**
 * @brief  DCS-BIOS callback: UFC display brightness control.
 *
 * @details Receives the in-simulator UFC brightness knob value (0-65535).
 *          Maps it to 0-255 for PWM and 0-7 for TM1640 display intensity,
 *          then updates all six option display drivers, both COMM display
 *          drivers, and the scratchpad backlight.  A static cache prevents
 *          redundant updates when the value has not changed.
 *
 * @param newValue  UFC_BRT value from DCS-BIOS (0 = off, 65535 = maximum brightness).
 */
void UFC_BRTChange(unsigned int newValue) {
  static unsigned int lastValue = 65536; // Initialised to an impossible value to force the first update
  
  // Early return if no change
  if (newValue == lastValue) {
    return;
  }
  lastValue = newValue;

  // Single mapping operation
  const uint8_t mappedBrightness = map(newValue, 0, 65535, 0, 255);
  
  // Update cached values
  UCF_brightness = mappedBrightness;
  UFC_active = (mappedBrightness > 0);
  
  // Combined display updates in a single loop
  for (int k = 0; k < 8; k++) {
    if (k < 6) {
      GN1640[k].setupDisplay(UFC_active, map(mappedBrightness,0,255,0,7));
    }
    if (k < 2) {
      GN1640_COMM[k].setupDisplay(UFC_active, map(mappedBrightness,0,255,0,7));
    }
  }
  // Update scratchpad using already mapped brightness
  scratchpad.set_backlight_brightness(mappedBrightness);
  // Record in ufcState so the value is available to save() / load()
  ufcState.setBrightness(mappedBrightness);
}
DcsBios::IntegerBuffer UFC_BRT(FA_18C_hornet_UFC_BRT, UFC_BRTChange);

/**
 * @brief  DCS-BIOS callback: instrument interior lighting brightness.
 *
 * @details Controls the WS2812B LED backlight strip brightness.  Sets all LEDs
 *          to green and scales the master FastLED brightness to the DCS value.
 *          The green colour matches the FA-18C cockpit night-vision compatible
 *          interior lighting tone.
 *
 * @param newValue  INSTR_INT_LT value from DCS-BIOS (0 = off, 65535 = full brightness).
 */
void onInstrIntLtChange(unsigned int newValue) {
  fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
  FastLED.setBrightness(map(newValue, 0, 65535, 0, 255));
  FastLED.show();
}
DcsBios::IntegerBuffer instrIntLtBuffer(FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange);

/**
 * @brief  DCS-BIOS callback: DCS-BIOS library version string.
 *
 * @details Stores the version string reported by DCS-BIOS into
 *          DCSMetaData.DCSBIOSVersion for diagnostics and logging.
 *
 * @param newValue  Version string up to 5 characters + null terminator.
 */
void onDcsBiosChange(char* newValue) {
  strcpy(DCSMetaData.DCSBIOSVersion, newValue);
}
DcsBios::StringBuffer<6> dcsBiosBuffer(CommonData_DCS_BIOS_A, onDcsBiosChange);

/**
 * @brief  DCS-BIOS callback: pilot name string.
 *
 * @details Stores the current pilot name from DCS metadata into
 *          DCSMetaData.PilotName.  Available for display or logging purposes.
 *
 * @param newValue  Pilot name string, up to 23 characters + null terminator.
 */
void onPilotnameChange(char* newValue) {
  strcpy(DCSMetaData.PilotName, newValue);
}
DcsBios::StringBuffer<24> pilotnameBuffer(CommonData_PILOTNAME_A, onPilotnameChange);

/**
 * @brief  DCS-BIOS callback: export frame update counter.
 *
 * @details Stores the rolling per-frame counter into DCSMetaData.UpdateCounter.
 *          Used to monitor DCS-BIOS communication health.  Truncates silently
 *          from unsigned int to uint8_t (wraps at 255).
 *
 * @param newValue  Current update counter value (0-65535, typically 0-255).
 */
void onUpdateCounterChange(unsigned int newValue) {
  DCSMetaData.UpdateCounter = newValue;
}
DcsBios::IntegerBuffer UpdateCounterBuffer(MetadataEnd_UPDATE_COUNTER, onUpdateCounterChange);

/**
 * @brief  DCS-BIOS callback: export frame skip counter.
 *
 * @details Stores the number of frames skipped since the last update into
 *          DCSMetaData.SkipCounter.  A consistently non-zero value indicates
 *          USB bandwidth or processing bottlenecks.
 *
 * @param newValue  Frame skip count (0-65535, typically 0-255).
 */
void onUpdateSkipCounterChange(unsigned int newValue) {
  DCSMetaData.SkipCounter = newValue;
  
}
DcsBios::IntegerBuffer UpdateSkipCounterBuffer(MetadataEnd_UPDATE_SKIP_COUNTER, onUpdateSkipCounterChange);
/**
 * @brief  DCS-BIOS callback: aircraft name string (mission running indicator).
 *
 * @details DCS-BIOS sends an empty string here when no mission is loaded or
 *          DCS is at the main menu, and a non-empty aircraft identifier
 *          (e.g. "FA-18C_hornet") once a mission is actively running. This
 *          drives DCSMetaData.MissionStart, which the NOGO/GO LED logic reads
 *          to determine whether DCS is actually connected and running. When a mission
 *          starts it also forces all polled inputs to re-send their state.
 *
 * @param newValue  Aircraft name string, or empty/nullptr if no mission active.
 */
void onMissionAcftNameChange(char* newValue) {
  const bool wasRunning = (DCSMetaData.MissionStart == DCSRUNNING);
  DCSMetaData.MissionStart = (newValue != nullptr && newValue[0] != '\0') ? DCSRUNNING : DSCSTOPPED;
  // Mission just started (stopped -> running, once): make every polled input (COMM1/COMM2 volume, UFC
  // brightness, pull switches) and the ADF switch send its current position, so the cockpit matches the
  // panel without anything being moved. Not repeated while the mission keeps running.
  if (!wasRunning && DCSMetaData.MissionStart == DCSRUNNING) {
    DcsBios::PollingInput::resetAllStates();
    lastadf = 0xFF;
  }
}
DcsBios::StringBuffer<24> missionAcftNameBuffer(MetadataStart_ACFT_NAME_A, onMissionAcftNameChange);
// ════════════════════════════════════════════════════════════════════════════
// Boot, LED and System Utility Functions
// ════════════════════════════════════════════════════════════════════════════

/**
 * @brief  Display or clear the OpenHornet boot splash screen across all UFC displays.
 *
 * @details When called to show the splash (noClear = true), writes BOOT_OP_MESSAGES
 *          to the five option displays, BOOT_COMM_MESSAGES to the COMM displays,
 *          and BOOT_SCRATCHPAD_MESSAGES to the scratchpad.  Optionally activates
 *          all CUE indicators.
 *
 *          When called to clear (noClear = false), overwrites every display region
 *          with spaces and deactivates all CUE indicators.
 *
 *          Typical usage:
 *          - bootMessage(true, true, true)  — show splash, keep it visible
 *          - bootMessage(true, false, false) — clear all displays after the timeout
 *
 * @param noPause  Unused parameter retained for API compatibility.
 * @param noClear  If false, clear all displays after writing the splash content.
 *                 If true, leave the splash content on screen.
 * @param noCue    If true, skip driving the CUE indicators (both on and off).
 *                 If false, turn all CUE indicators on (or off when clearing).
 */
void bootMessage(bool noPause, bool noClear, bool noCue) {
  // Set COMM displays
  // Draw the splash directly so it is not recorded as a DCS-BIOS value.
  display_comm_char(const_cast<char*>(BOOT_COMM_MESSAGES[0]), 0);
  display_comm_char(const_cast<char*>(BOOT_COMM_MESSAGES[1]), 1);

  // Set option displays and cueing
  for (int i = 0; i < 5; i++) {
    strncpy(op_display[i].content, BOOT_OP_MESSAGES[i], 4);
    op_display[i].cueing = true;
    update_op_display(i);
    update_op_cue(i);
    if (!noCue) {
      cue(i, true);
    }
  }

  // Set scratchpad content
  scratchpad.print_str(const_cast<char*>(BOOT_SCRATCHPAD_MESSAGES[0]), false);
  // print_str() drives only the first 16-segment cell; draw the "H" in the second.
  scratchpad.print_str2(" H", false);
  scratchpad.print_num(const_cast<char*>(BOOT_SCRATCHPAD_MESSAGES[1]), false);

  // Clear displays if requested
  if (!noClear) {
    static const char BLANK_DISPLAY[] = "    ";
    static const char BLANK_COMM[] = "  ";
    static const char BLANK_SCRATCHPAD[] = "      ";
    static const char BLANK_STRING2[] = "  ";

    scratchpad.print_str(const_cast<char*>(BLANK_SCRATCHPAD), false);
    scratchpad.print_num(const_cast<char*>(BLANK_SCRATCHPAD), false);
    // String2 now shares the real, visible digit "8" cell (_buffer16[2]/[3]) per the
    // schematic-verified fix, so it must be explicitly blanked here too — previously this call
    // was unnecessary because print_str2() only wrote to a hardware no-connect register. Without
    // it, the last STRING_2 value (e.g. the merged scratchpad digit) stayed lit on the glass after
    // the boot-splash-then-clear cycle ran on DCS mission stop / disconnect.
    scratchpad.print_str2(const_cast<char*>(BLANK_STRING2), false);

    // Clear option displays
    for (int i = 0; i < 5; i++) {
      strncpy(op_display[i].content, BLANK_DISPLAY, 4);
      op_display[i].cueing = false;
      update_op_display(i);
      if (!noCue) {
        cue(i, false);
      }
    }
    // Clear COMM displays
    display_comm_char(const_cast<char*>(BLANK_COMM), 0);
    display_comm_char(const_cast<char*>(BLANK_COMM), 1);
  }
}

/**
 * @brief  Show one period of the UFC built-in test (NATOPS figure 2-48) on every display.
 *
 * @details Period 1 lights the outer segments of the alphanumeric displays (option windows, COMM
 *          windows and the two scratchpad cells), period 2 the inner segments. Every numeric
 *          segment and all five option cues are lit in both periods.
 *
 *          Segment masks (from OP_DISPLAY_FONT and the COMM layout in CHARSET.h):
 *          - Option windows (16-bit): outer 0x071F, inner 0xF8E0; together they are all 16 segments.
 *          - COMM windows (4 columns of 4 rows, row 0 = A, 1 = B, 2 = C, 3 = D): outer = rows A and B
 *            (0x03), inner = rows C and D (0x0C).
 *
 * @param phase  1 = outer segments, 2 = inner segments.
 */
static void bitTestShow(uint8_t phase) {
  const uint16_t opMask   = (phase == 1) ? 0x071F : 0xF8E0;
  const uint8_t  commMask = (phase == 1) ? 0x03   : 0x0C;

  for (int i = 0; i < 5; i++) {
    for (uint8_t pos = 0; pos < OP_DIGITS; pos++) {
      GN1640[i].setSegments16(opMask, op_order[pos]);
    }
    cue(i, true);   // CUE4/CUE5 redraw COMM2, so the COMM windows are drawn after the cues
  }
  for (int n = 0; n < 2; n++) {
    for (uint8_t col = 0; col < 4; col++) {
      matrix_COMM[n].setColumn(col, commMask);
    }
  }
  scratchpad.bitTest(phase);
}

/**
 * @brief  Scan the I2C bus and print all responding device addresses to Serial.
 *
 * @details Iterates through all valid 7-bit I2C addresses (1-126), issues a
 *          zero-byte transmission to each, and prints the address in hex when
 *          a device acknowledges (error == 0).  Unknown errors (error == 4) are
 *          also reported.  Called from setup() only when I2C_SCAN is true.
 *
 * @note    Runs synchronously and adds significant startup latency; leave
 *          I2C_SCAN = false for normal operation.
 */
void i2cscan() {
  uint8_t nDevices = 0;

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      if (address < 0x10) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 0x10) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("Done");
  }
}

// ════════════════════════════════════════════════════════════════════════════
// DCS-BIOS Input Mappings
// Global instances read by DcsBios::loop() on every cycle.
// ════════════════════════════════════════════════════════════════════════════

// ---------- UFC keypad buttons (main key matrix) ----------
DcsBios::MatActionButtonSet ufc1("UFC_1", &UFCkeys[1][0], HIGH);
DcsBios::MatActionButtonSet ufc2("UFC_2", &UFCkeys[1][1], HIGH);
DcsBios::MatActionButtonSet ufc3("UFC_3", &UFCkeys[1][2], HIGH);
DcsBios::MatActionButtonSet ufc4("UFC_4", &UFCkeys[2][0], HIGH);
DcsBios::MatActionButtonSet ufc5("UFC_5", &UFCkeys[2][1], HIGH);
DcsBios::MatActionButtonSet ufc6("UFC_6", &UFCkeys[2][2], HIGH);
DcsBios::MatActionButtonSet ufc7("UFC_7", &UFCkeys[3][0], HIGH);
DcsBios::MatActionButtonSet ufc8("UFC_8", &UFCkeys[3][1], HIGH);
DcsBios::MatActionButtonSet ufc9("UFC_9", &UFCkeys[3][2], HIGH);
DcsBios::MatActionButtonSet ufc0("UFC_0", &UFCkeys[4][1], HIGH);

DcsBios::MatActionButtonSet ufcclr("UFC_CLR", &UFCkeys[4][0], HIGH);
DcsBios::MatActionButtonSet ufcent("UFC_ENT", &UFCkeys[4][2], HIGH);
DcsBios::MatActionButtonSet ufcip("UFC_IP", &UFCkeys[0][0], LOW);
DcsBios::MatActionButtonSet ufcencon("UFC_EMCON", &UFCkeys[0][1], LOW);

DcsBios::MatActionButtonSet ufcos1("UFC_OS1", &UFCkeys[0][3], HIGH);
DcsBios::MatActionButtonSet ufcos2("UFC_OS2", &UFCkeys[1][3], HIGH);
DcsBios::MatActionButtonSet ufcos3("UFC_OS3", &UFCkeys[2][3], HIGH);
DcsBios::MatActionButtonSet ufcos4("UFC_OS4", &UFCkeys[3][3], HIGH);
DcsBios::MatActionButtonSet ufcos5("UFC_OS5", &UFCkeys[4][3], HIGH);

DcsBios::MatActionButtonSet hud_bit("HUD_VIDEO_BIT", &HUDkeys[0][0], LOW);

// ---------- COMM channel encoders and pull switches ----------
typedef DcsBios::RotaryEncoderT<POLL_EVERY_TIME, DcsBios::TWO_STEPS_PER_DETENT> OHRotaryEncoder;

OHRotaryEncoder ufcComm1ChannelSelect("UFC_COMM1_CHANNEL_SELECT", "DEC", "INC", ENC1A, ENC1B);
OHRotaryEncoder ufcComm2ChannelSelect("UFC_COMM2_CHANNEL_SELECT", "DEC", "INC", ENC2A, ENC2B);
DcsBios::Switch2Pos ufcComm1Pull("UFC_COMM1_PULL", ENC1PB);
DcsBios::Switch2Pos ufcComm2Pull("UFC_COMM2_PULL", ENC2PB);

// ---------- Autopilot panel buttons (PCA9555) ----------
DcsBios::MatActionButtonSet UFC_AP("UFC_AP", &apkeys[0][0], HIGH);
DcsBios::MatActionButtonSet UFC_IFF("UFC_IFF", &apkeys[0][1], HIGH);
DcsBios::MatActionButtonSet UFC_TCN("UFC_TCN", &apkeys[0][2], HIGH);
DcsBios::MatActionButtonSet UFC_ILS("UFC_ILS", &apkeys[0][3], HIGH);
DcsBios::MatActionButtonSet UFC_DL("UFC_DL", &apkeys[0][4], HIGH);
DcsBios::MatActionButtonSet UFC_BCN("UFC_BCN", &apkeys[0][5], HIGH);
DcsBios::MatActionButtonSet UFC_ONOFF("UFC_ONOFF", &apkeys[0][6], HIGH);

// ---------- Potentiometers (COMM1/COMM2 volume, UFC brightness) ----------
// Fed by the ADS1115 raw readings (comm1Vol / comm2Vol / ufcBrtVal), not the ESP32
// native ADC — reverse = true and input_max = 32767 match GAIN_TWO's raw ADC range
// and the physical (reversed) pot wiring on the PCB.
DcsBios::MatPotentiometer ufcComm1Vol("UFC_COMM1_VOL", nullptr, &comm1Vol, true, 0, 32767);
DcsBios::MatPotentiometer ufcComm2Vol("UFC_COMM2_VOL", nullptr, &comm2Vol, true, 0, 32767);
DcsBios::MatPotentiometer ufcBrt("UFC_BRT", nullptr, &ufcBrtVal, true, 0, 32767);


/**
 * @brief  Produce a slow square-wave blink on the NOGO LED when DCS is idle.
 *
 * @details Active only when all of the following are true:
 *          - DCS mission is not running (MissionStart == DSCSTOPPED)
 *          - All UFC displays are blank (displayBlank == true)
 *          - Neither the GO nor NOGO LED is in its 10-second on phase
 *
 *          Toggles UFC_NOGO at approximately 0.25 Hz (BREATH_PERIOD = 2000 ms
 *          half-period) using a cheap integer modulo, avoiding any float or
 *          trigonometric operations.  Called from loop() every iteration.
 */
  void breatheNogoLed() {
    // Last level written (-1 = unknown, forces a write when breathing restarts).
    static int8_t lastBreathLevel = -1;

    // Only breathe when DCS is stopped and display is blank
    if (DCSMetaData.MissionStart != DSCSTOPPED || !displayBlank || goLedOn || noGoLedOn) {
        lastBreathLevel = -1;
        return;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastBreathUpdate >= BREATH_INTERVAL) {
        lastBreathUpdate = currentMillis;

        // Optimized: previous code computed sin() + 3 float multiplications only to threshold the result
        // at > 0.5, which is equivalent to a plain square wave. Replaced with a single integer modulo.
        bool ledState = (currentMillis % (unsigned long)BREATH_PERIOD) < ((unsigned long)BREATH_PERIOD / 2);
        // Only write to the I2C bus when the level flips.
        if ((int8_t)ledState != lastBreathLevel) {
          UFCkeypad.digitalWrite(UFC_NOGO, ledState);
          lastBreathLevel = (int8_t)ledState;
        }
    }
}

/**
 * @brief  Drive the GO / NOGO indicator LEDs based on DCS mission state, non-blocking.
 *
 * @details Compares DCSMetaData.MissionStart against the last observed state.
 *          On a transition:
 *          - DSCSTOPPED → DCSRUNNING : GO LED on, NOGO LED off; timer starts.
 *          - DCSRUNNING → DSCSTOPPED : NOGO LED on, GO LED off; timer starts.
 *
 *          Both LEDs auto-extinguish TIMEOUT (10 s) after they were turned on,
 *          preventing them from staying lit indefinitely when the state persists.
 *          All GPIO writes go through the TCA8418 over I2C.
 *
 * @note    Called from loop() every iteration; returns immediately in the common
 *          case where no state change has occurred and no timeout has expired.
 */
void updateGoNoGoLedNonBlocking() {
  unsigned long currentMillis = millis();

  if (DCSMetaData.MissionStart != lastMissionState) {
    // Mission state changed
    lastMissionState = DCSMetaData.MissionStart;
    ledOnTime = currentMillis;

    if (DCSMetaData.MissionStart == DCSRUNNING) {
      // Turn on GO LED, turn off NOGO LED
      UFCkeypad.digitalWrite(UFC_GO, HIGH);
      UFCkeypad.digitalWrite(UFC_NOGO, LOW);
      goLedOn = true;
      noGoLedOn = false;
    } else if (DCSMetaData.MissionStart == DSCSTOPPED) {
      // Turn on NOGO LED, turn off GO LED
      UFCkeypad.digitalWrite(UFC_NOGO, HIGH);
      UFCkeypad.digitalWrite(UFC_GO, LOW);
      noGoLedOn = true;
      goLedOn = false;
    }
  }

  // Check if 10 seconds have passed since LED turned on
  if (goLedOn && (currentMillis - ledOnTime >= TIMEOUT)) {
    UFCkeypad.digitalWrite(UFC_GO, LOW);
    goLedOn = false;
  }

  if (noGoLedOn && (currentMillis - ledOnTime >= TIMEOUT)) {
    UFCkeypad.digitalWrite(UFC_NOGO, LOW);
    noGoLedOn = false;
  }
}

/**
 * @brief  Manage the boot splash screen display lifecycle, non-blocking.
 *
 * @details Watches for DCS mission-state transitions:
 *          - DSCSTOPPED → DCSRUNNING : resets the shown-flag so the splash can
 *            appear again on the next stop.
 *          - DCSRUNNING → DSCSTOPPED (first time only): calls bootMessage() to
 *            show the splash and starts a 10-second timer.
 *
 *          At power-on setup() arms the UFC built-in test. BIT_START_DELAY_MS later this function
 *          lights it (bitTestShow(): BIT_PHASE_MS of outer segments, then BIT_PHASE_MS of inner
 *          segments), then shows the version splash. The test runs once per power-up and stops at
 *          once if a mission starts. After BOOT_SPLASH_MS the splash is cleared via a second
 *          bootMessage() call. A 500 ms poll gate on the splash timeout check avoids calling
 *          millis() on every loop iteration while the splash is active.
 *
 *          millis() is only called when a state change or an active boot message
 *          timeout actually needs to be evaluated, minimising overhead.
 *
 * @note    Called from loop() every iteration.
 */
void handleBootMessage() {
  static uint8_t prevMissionState = 255;
  static bool bootMessageShownForCurrentStop = false;
  static unsigned long lastTimeoutCheck = 0;

  const uint8_t currentMissionState = DCSMetaData.MissionStart;
  const bool stateChanged = (prevMissionState != currentMissionState);

  // Optimized: previously millis() was called unconditionally on every loop() iteration just to
  // decide whether to early-return. Now cheap state comparisons gate the millis() call — millis()
  // is only called when there is actually a state change or an active boot message to time out.
  if (!stateChanged && !bootMessageActive) return;

  unsigned long currentMillis = millis();

  // Handle mission-state transitions immediately when they occur
  if (stateChanged) {
    prevMissionState = currentMissionState;
    // The power-on test was started in setup(), before the mission state was known.
    const bool poweredOnJustNow = powerOnSequence;
    powerOnSequence = false;
    if (currentMissionState == DCSRUNNING) {
      bootMessageShownForCurrentStop = false;
      if (poweredOnJustNow && bitPhase != 0) {
        // The sim was already running at power-on: drop the test (and blank the displays if it had
        // already lit them); DCS-BIOS fills them in as values change.
        bitPhase = 0;
        bootMessageActive = false;
        if (bitShown) bootMessage(true, false, false);
      }
      // Mission started: redraw the COMM channels last sent by DCS-BIOS.
      redrawCommFromDcs();
    } else if (currentMissionState == DSCSTOPPED &&
               !bootMessageShownForCurrentStop &&
               !bootMessageActive) {
      bootMessage(true, true, true);
      bootMessageActive = true;
      bootMessageStartTime = currentMillis;
      bootMessageShownForCurrentStop = true;
    }
  }

  // Power-on built-in test: outer segments for BIT_PHASE_MS, then inner segments for BIT_PHASE_MS,
  // then the version splash. If a mission starts meanwhile the test stops and DCS-BIOS takes over
  // the displays (it sends every value at mission start).
  if (bitPhase != 0) {
    if (currentMissionState == DCSRUNNING) {
      bitPhase = 0;
      bootMessageActive = false;
    } else if (!bitShown) {
      // Power-up wait: leave the displays alone until they have settled, then light the first period.
      if (currentMillis - bitPhaseStart >= BIT_START_DELAY_MS) {
        bitShown = true;
        bitPhaseStart = currentMillis;
        bitTestShow(1);
      }
    } else if (currentMillis - bitPhaseStart >= BIT_PHASE_MS) {
      if (bitPhase == 1) {
        bitPhase = 2;
        bitPhaseStart = currentMillis;
        bitTestShow(2);
      } else {
        // Test over: show the version splash (bootMessageActive stays true; the timeout below clears it).
        bitPhase = 0;
        bootMessage(true, true, true);
        bootMessageStartTime = currentMillis;
      }
    }
    return;
  }

  // Poll the 10-second timeout at 500ms resolution — coarse enough for a 10s timer,
  // avoids calling bootMessage() on every loop iteration while the message is active
  if (bootMessageActive && (currentMillis - lastTimeoutCheck >= 500)) {
    lastTimeoutCheck = currentMillis;
    if (currentMillis - bootMessageStartTime >= BOOT_SPLASH_MS) {
      bootMessage(true, false, false);
      bootMessageActive = false;
      // Splash cleared during a running mission: redraw the COMM channels.
      if (currentMissionState == DCSRUNNING) {
        redrawCommFromDcs();
      }
    }
  }
}

/**
 * @brief  Arduino one-time hardware and peripheral initialisation.
 *
 * @details Executed once at power-on or after a reset.  Performs the following
 *          in order:
 *          1. Starts Serial at 250000 baud and waits for connection.
 *          2. Initialises the I2C bus at 400 kHz (fast mode).
 *          3. Optionally scans the I2C bus (I2C_SCAN == true).
 *          4. Initialises the TCA8418 UFC keypad controller, configures the
 *             key matrix, GPIO pins, debounce, and attaches the CHANGE interrupt.
 *          5. Initialises the PCA9555 autopilot keypad expander.
 *          6. (USE_ADC_RDY) Initialises the ADS1115 ADC, attaches the DRDY
 *             interrupt, and starts the first single-shot conversion on COMM1_VOL.
 *          7. Initialises the WS2812B FastLED backlight strip.
 *          8. Initialises the HT1621 scratchpad LCD.
 *          9. (not USE_ADC_RDY) Configures the ESP32 native ADC.
 *         10. Starts the TM1640 display drivers and arms the power-on built-in test and version
 *             splash (run by handleBootMessage()).
 *         11. Turns off the FastLED strip (setup complete indication).
 *         12. Reads power source via powerSource().
 *         13. Tests both GO and NOGO LEDs for 2 seconds.
 *         14. Calls DcsBios::setup() to initialise the DCS-BIOS serial protocol.
 */
void setup() {
  Serial.begin(250000);
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT && defined(ARDUINO_USB_MODE) && ARDUINO_USB_MODE == 1
  // Hardware CDC and JTAG: by default every write waits up to 100 ms when the PC is not reading fast
  // enough, which makes the knobs and keys feel sluggish (worst with DTR off in the DCS-BIOS Bridge).
  Serial.setTxTimeoutMs(0);
#endif
  Serial.printf("Openhornet Up-Front Controller HW Version %s, FW Version %s\n", BOOT_OP_MESSAGES[3], BOOT_OP_MESSAGES[4]);

  ufcState.begin();  // configures VCC_SNS_3_3V as INPUT_PULLUP
  ufcState.load();   // restore previously saved runtime options from NVS (uses defaults on first boot)

  // Set runtime configuration states that reflect the current firmware build.
  // These are applied after load() so they always reflect actual compiled behaviour
  // rather than a potentially stale saved value.

  // irqmode: driven by compile-time defines — IRQ if USE_ADC_RDY or USE_IO_INT is defined, else POLLING.
  #if defined(USE_ADC_RDY) || defined(USE_IO_INT)
    ufcState.setIrqMode(true);
  #else
    ufcState.setIrqMode(false);
  #endif

  // bootmode: boot splash screen is always shown in this firmware build.
  ufcState.setBootMode(true);

  // ---------- UFC Keyboard Setup
  Wire.begin(SDApin, SCLpin);
  Wire.setClock(400000);
  //Wire.setTimeOut(30);
  delay(1000);

  if (I2C_SCAN) i2cscan();

  if (!UFCkeypad.begin(TCA8418_DEFAULT_ADDR, & Wire)) {
    _UFC_LOG("UFCkeypad not found, check wiring & pullups!");
  } else {
    _UFC_LOG("UFCkeypad found at 0x%02x", TCA8418_DEFAULT_ADDR);
  }

  UFCkeypad.matrix(TCA8418_ROWS, TCA8418_COLS);

  // Optimized for S3: TCA8418_irq() is only meaningful in IRQ mode (USE_IO_INT).
  // In polling mode (the confirmed-working configuration on this chip — the S3
  // was found to miss edges via attachInterrupt() on this pin), leaving this
  // attached still fires an ISR on every real key edge for no consumer, since
  // TCA8418_event/_tca8418_cnt are never read by the polling code path. Gating
  // it behind USE_IO_INT (matching how the ADS1115 DRDY interrupt is already
  // gated behind USE_ADC_RDY below) removes that spurious interrupt overhead.
  #if defined(USE_IO_INT)
    pinMode(IO_INT, INPUT);
    attachInterrupt(digitalPinToInterrupt(IO_INT), TCA8418_irq, CHANGE);
  #endif
  UFCkeypad.pinMode(UFC_NOGO, OUTPUT); // NOGO LED
  UFCkeypad.pinMode(UFC_GO, OUTPUT); // GO LED
  
  UFCkeypad.pinMode(UFC_IPSW, INPUT_PULLUP); //5
  UFCkeypad.pinMode(UFC_BITSW, INPUT_PULLUP); //12
  UFCkeypad.pinMode(UFC_EMCON, INPUT_PULLUP); //17
  UFCkeypad.flush();
  UFCkeypad.enableDebounce();
  UFCkeypad.enableInterrupts();
  
  // ----------- AP Keyboard Setup

  APkeypad.attach(Wire);
  if (APkeypad.i2c_error()) {
    _UFC_LOG("APkeypad not found, check wiring & pullups!");
  } else {
    _UFC_LOG("APkeypad found");
  }
  APkeypad.polarity(PCA95x5::Polarity::INVERTED_ALL);
  APkeypad.direction(PCA95x5::Direction::IN_ALL);
  APkeypad.write(PCA95x5::Level::L_ALL);

  // ---------- ADS1115 ADC Setup ----------
  // Must be initialised regardless of IRQ vs polling mode.
  // The ADC input range (gain) can be changed via the following functions, but
  // be careful never to exceed VDD +0.3V max, or the upper/lower input limits.
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.setGain(GAIN_TWO);           // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  // 860 samples/s keeps the three-channel poll to ~4 ms (library default 128 SPS is ~24 ms).
  ads.setDataRate(RATE_ADS1115_860SPS);

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
  }

  #if defined(USE_ADC_RDY)
    // ----------- ADC Interrupt Setup (IRQ mode only) -----------
    pinMode(ADC_RDY_3_3V, INPUT);
    // DRDY goes LOW when conversion is complete, so FALLING is the correct edge.
    attachInterrupt(digitalPinToInterrupt(ADC_RDY_3_3V), ADS1115_irq, FALLING);

    // Passing 0x8000 (32767) as the threshold flag forces the library to write
    // 1 to the MSB of the High Threshold register and 0 to the Low Threshold
    // register, which tells the chip to pulse the ALERT/RDY line on every read.
    ads.startComparator_SingleEnded(ADS_CH_COMM1_VOL, 32767);

    // Kick off the first single-shot read on COMM1_VOL (A0); state machine
    // cycles through all channels from here on.
    ads.startADCReading(ADS_MUX_COMM1_VOL, /*continuous=*/false);
  #endif

  // ---------- Backlight Setup
  FastLED.addLeds < WS2812B, BL_DATA_PIN, GRB > (ws2812b, BACKLIGHT_LEDS); // GRB ordering is typical
  FastLED.clear();
  fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
  FastLED.setBrightness(128);
  FastLED.show();

  // ---------- Scratchpad Setup

  scratchpad.begin(SCRCS_3_3V, SCRWR_3_3V, SCRDATA_3_3V, SCRBL_3_3V); // (cs, wr, Data, backlight)
  scratchpad.clear(); // clear the screen

  // ---------- TM1640 display drivers: explicit begin()
  // The TM16xx library runs begin() (which clears the display) by itself the first time a driver sends
  // data. For the TM1640Anode drivers that happens in the middle of the first setSegments16() call,
  // after the new segments were stored, so that call's segments are wiped: the first position written
  // after power-up (the leftmost cell of each option window) stayed dark. Running begin() here, before
  // anything is drawn, makes every first write behave like the rest.
  for (int k = 0; k < 6; k++) {
    GN1640[k].begin(UFC_active, UCF_brightness);   // OP1-OP5 and the CUE row
  }
  CUE123.begin(UFC_active, UCF_brightness);
  COMM1.begin(UFC_active, UCF_brightness);
  COMM2.begin(UFC_active, UCF_brightness);
  GN1640_COMM[0].begin(UFC_active, UCF_brightness);
  GN1640_COMM[1].begin(UFC_active, UCF_brightness);

  // Optimized for S3: removed the unused ESP32 native-ADC setup that used to live
  // here (analogSetAttenuation / analogReadResolution). All three pots are read
  // exclusively through the ADS1115 external ADC (see the loop() ADC section);
  // the native ADC was configured but never actually read via analogRead().
  // Leaving it out also avoids the S3's ADC2/Wi-Fi-Bluetooth coexistence quirk
  // entirely, since the native ADC peripheral is never touched.

  //  ---------- turn off backlight to show we're done with setup
  FastLED.clear();
  FastLED.show();

  ufcState.detectPower();  // replaces powerSource() — result cached in ufcState for later queries

  UFCkeypad.digitalWrite(UFC_GO, HIGH); 
  UFCkeypad.digitalWrite(UFC_NOGO, HIGH);
  delay(2000);
  UFCkeypad.digitalWrite(UFC_GO, LOW); 
  UFCkeypad.digitalWrite(UFC_NOGO, LOW);

  DcsBios::setup();

  //  ---------- Boot sequence. BIT_START_DELAY_MS (2 s) after setup() ends, handleBootMessage() runs the
  //  built-in test (5 s outer segments, 5 s inner segments), shows the version splash for 10 s, then
  //  clears the displays. The NOGO LED then flashes until a mission starts.
  bitPhase            = 1;
  bitShown            = false;
  bitPhaseStart       = millis();
  bootMessageActive   = true;
  powerOnSequence     = true;
}

/**
 * @brief  Arduino main execution loop — called repeatedly after setup() completes.
 *
 * @details Each iteration performs the following work in order:
 *
 *          1. **UFC keypad**: If a TCA8418 interrupt event is pending,
 *             detaches the interrupt, clears the flag, calls processUFCKeypad()
 *             and processUFCIO() to decode the event, then re-attaches the
 *             interrupt.  If no event is pending, still calls processUFCKeypad()
 *             and processAPKeypad() for polling fallback.
 *
 *          2. **ADS1115 ADC** (USE_ADC_RDY only): If a DRDY interrupt fired,
 *             reads the completed channel, stores the result in the appropriate
 *             comm1Vol / comm2Vol / ufcBrtVal variable, and starts the next
 *             single-shot conversion (state machine cycling A0→A1→A2→A0).
 *
 *          3. **GO / NOGO LEDs**: Calls updateGoNoGoLedNonBlocking() to manage
 *             the mission-state indicator LEDs.
 *
 *          4. **NOGO breathing**: Calls breatheNogoLed() for the idle slow-blink
 *             effect when DCS is not running.
 *
 *          5. **Boot message**: Calls handleBootMessage() to show or clear the
 *             splash screen on DCS state transitions.
 *
 *          6. **DCS-BIOS**: Calls DcsBios::loop() to process incoming USB serial
 *             data, fire registered callbacks, and send queued output messages.
 */
void loop() {
  // ── Keypad input: always polled — TCA8418 IO_INT interrupt is unreliable on S3 ──
  processUFCKeypad();
  processUFCIO();
  processAPKeypad();  // AP keypad always polled (PCA9555 has no IRQ line)

  // ── ADC input ──────────────────────────────────────────────────────────────
  #if defined(USE_ADC_RDY)
    // Interrupt-driven: ADS1115 DRDY pulses ADC_RDY_3_3V, ADS1115_irq() sets ADS1115_event.
    if (ADS1115_event) {
      ADS1115_event = false;
      static uint8_t adsChannel = ADS_CH_COMM1_VOL;
      switch (adsChannel) {
        case ADS_CH_COMM1_VOL:
          comm1Vol  = clampAdc(ads.getLastConversionResults());
          ads.startADCReading(ADS_MUX_COMM2_VOL, /*continuous=*/false);
          adsChannel = ADS_CH_COMM2_VOL;
          break;
        case ADS_CH_COMM2_VOL:
          comm2Vol  = clampAdc(ads.getLastConversionResults());
          ads.startADCReading(ADS_MUX_UFC_BRT, /*continuous=*/false);
          adsChannel = ADS_CH_UFC_BRT;
          break;
        case ADS_CH_UFC_BRT:
          ufcBrtVal = clampAdc(ads.getLastConversionResults());
          ads.startADCReading(ADS_MUX_COMM1_VOL, /*continuous=*/false);
          adsChannel = ADS_CH_COMM1_VOL;
          break;
      }
    }
  #else
    // Polling: read all three channels directly every 100ms.
    static unsigned long lastAdcPollTime = 0;
    const unsigned long  adcPollInterval = 100;
    if (millis() - lastAdcPollTime >= adcPollInterval) {
      lastAdcPollTime = millis();
      // Negative readings are floored at 0 by clampAdc().
      comm1Vol  = clampAdc(ads.readADC_SingleEnded(ADS_CH_COMM1_VOL));
      comm2Vol  = clampAdc(ads.readADC_SingleEnded(ADS_CH_COMM2_VOL));
      ufcBrtVal = clampAdc(ads.readADC_SingleEnded(ADS_CH_UFC_BRT));
    }
  #endif

  // ── GO / NOGO indicator LEDs ─────────────────────────────────────────────
  // GO   = mission running, shown for TIMEOUT ms then auto-off.
  // NOGO = mission not running, shown for TIMEOUT ms then breathes slowly.
  updateGoNoGoLedNonBlocking();
  breatheNogoLed();

  // Handle boot message display
  if (ufcState.getBootMode()) {
    handleBootMessage();
  }

  // Main DCS-BIOS loop
  DcsBios::loop();
}
