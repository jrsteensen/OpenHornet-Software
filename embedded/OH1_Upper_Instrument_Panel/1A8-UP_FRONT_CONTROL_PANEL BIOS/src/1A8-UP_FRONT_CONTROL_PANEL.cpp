#pragma message "Board macro: " ARDUINO_BOARD
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
 * @date 09.05.2026
 * @version 1.1.0 alpha
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the Up-Front Control Panel
 *
 * @details
 * 
 *  * **Reference Designator:** 1A8
 *  * **Intended Board:** UFC_Main PCB1
 *  * **RS485 Bus Address:** N/A
 * 
 * *##Wiring diagram:
 * PIN | Function
 * --- | ---
 *  33	|	SDApin       
 *  35	|	SCLpin       
 *  15	|	IO_INT       
 *  13	|	NOGOLED      
 *  14	|	GOLED        
 *   9	|	BL_DATA_PIN  
 *  40	|	ENC1PB       
 *  38	|	ENC1A        
 *  36	|	ENC1B        
 *  34	|	ENC2PB       
 *  21	|	ENC2A        
 *  17	|	ENC2B        
 *    7	|	DIO_3_3V     
 *    4	|	OP1_CLK_3_3V 
 *    5	|	OP2_CLK_3_3V 
 *    6	|	OP3_CLK_3_3V 
 *    8	|	OP4_CLK_3_3V 
 *   18	|	OP5_CLK_3_3V 
 *   37	|	CUE_CLK_3_3V 
 *   16	|	VCC_SNS_3_3V 
 *   39	|	COM1_CLK_3_3V
 *   10	|	COM2_CLK_3_3V 
 *   A0	|	UFC_COMM1_VOL
 *   A1	|	UFC_COMM2_VOL
 *   A2	|	UFC_BRT_POT  
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 */

 /**
  * OVERRIDE TINYUSB Library for CDC overflow 
  */
//#define USE_TINYUSB

/**
 * Enable or Disable usage of Interrupt processing
 * comment out to disable
*/
#define USE_ADC_RDY 
#define USE_IO_INT

#if defined(USE_TINYUSB)
  #include <Adafruit_TinyUSB.h>
#endif
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

/**
 * @brief Main Includes
 */
#include <Adafruit_TCA8418.h>           //Key Matrix driver
#include <adafruit_TCA8418_registers.h> //Key Matrix driver
#include <TM1640Anode.h>                //TM1640 for OP Display
#include <TM1640.h>                     //TM1640 for COMM Display
#include <TM16xxMatrix.h>               //TM1640 matrix for COMM Display
#include <TM16xxMatrix16.h>             //TM1640 matrix for COMM Displa
#include "HT1621_OH.h"                  //modified HT1621 lib used for scratchpad
#include "CHARSET.h"                    //Custom charset
#include <PCA95x5.h>                    //PCA9555 AP Keyboard
#include <FastLED.h>                    //WS2812B Driver
#include <Adafruit_ADS1X15.h>           //ADS1115 4CH 16bit ADC
#include "driver/ledc.h"
#include "WebOtaUpdate.h"

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
#define SDApin          33
#define SCLpin          35
#define IO_INT          15
#define BL_DATA_PIN     9
#define ENC1PB          40
#define ENC1A           38
#define ENC1B           36
#define ENC2PB          34
#define ENC2A           21
#define ENC2B           17
#define DIO_3_3V        7
#define OP1_CLK_3_3V    4
#define OP2_CLK_3_3V    5
#define OP3_CLK_3_3V    6
#define OP4_CLK_3_3V    8
#define OP5_CLK_3_3V    18
#define CUE_CLK_3_3V    37
#define VCC_SNS_3_3V    16
#define COM1_CLK_3_3V   39
#define COM2_CLK_3_3V   10
#define ADC_RDY_3_3V    1
#define SCRCS_3_3V      14
#define SCRWR_3_3V      13
#define SCRDATA_3_3V    12
#define SCRBL_3_3V      11

#define OP_DIGITS       4
#define COM1_DIGITS     1
#define COM2_DIGITS     1
#define UFC_COMM1_VOL   A0
#define UFC_COMM2_VOL   A1
#define UFC_BRT_POT     A2

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

/**
 * @brief Analog port resolution and width
 */
#define ANALOG_RESOLUTION   13
#define ANALOG_WIDTH        8192
#define ANALOG_MIN_WIDTH    0

#if defined ( USE_ADC_RDY )
/**
 * @brief ADS1115 I2C Analog port resolution and width
 */
#define ADS1115_RESULUTION  16
#define ADS1115_WIDTH       65535
#define ADS1115_MIN_WIDTH   0

/**
 * @brief ADS1115 channel assignments — maps each physical pot to an ADS1115 input pin and
 * the corresponding MUX constant used in startADCReading(). The _CH values are the
 * state-machine indices; the _MUX values are the Adafruit library constants.
 */
#define ADS_CH_COMM1_VOL   0                              // A0 = COMM1 volume pot
#define ADS_CH_COMM2_VOL   1                              // A1 = COMM2 volume pot
#define ADS_CH_UFC_BRT     2                              // A2 = UFC brightness pot

#define ADS_MUX_COMM1_VOL  ADS1X15_REG_CONFIG_MUX_SINGLE_0  // A0
#define ADS_MUX_COMM2_VOL  ADS1X15_REG_CONFIG_MUX_SINGLE_1  // A1
#define ADS_MUX_UFC_BRT    ADS1X15_REG_CONFIG_MUX_SINGLE_2  // A2
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

/**
 * @brief USE ELOG FOR REMOTE LOGGING
 * MAINLY USED FOR MONITOR CODE DURING DEVELOPEMENT
 * REQUIRES A SYSLOG SERVER RUNNING
 * 
 */
#define ENABLE_ELOG

#if defined(ENABLE_ELOG)
  #define _UFC_LOG(fmt, ...)  Logger.log(UFC_STATE_LOG_CH, ELOG_LEVEL_INFO, "[UfcState] " fmt, ##__VA_ARGS__)
#else
  #define _UFC_LOG(fmt, ...)  Serial.printf("[UfcState] " fmt "\n", ##__VA_ARGS__)
#endif

#if defined(ENABLE_ELOG)
// ELOG CONFIGURATION
// for arduino IDE you must edit the elogConfig.h file in the Elog library to enable the features you want.
// Located at: <Arduino libraries folder>/Elog/src/ElogConfig.h

// #define ELOG_SD_ENABLE        // uncomment to enable SD card logging
// #define ELOG_SPIFFS_ENABLE    // uncomment to enable SPIFFS/LittleFS logging
// #define ELOG_SYSLOG_ENABLE    // uncomment to enable Syslog logging
// #define ELOG_TIMER_ENABLE     // uncomment to enable the LogTimer utility

#include <WiFi.h> 
#include <secrets.h>                    // Create a secrets.h file with your WiFi credentials (define WIFI_SSID and WIFI_PASS)
#include <Elog.h>

/**
*  @brief Setup ELOG for sending messages to a SYSLOG server
*/

/** @brief Elog channel identifier used for all UFC log messages. */
#define MYLOG 0

/** @brief Wi-Fi SSID sourced from secrets.h (used by Elog syslog). */
const char* ssid     = WIFI_SSID;
/** @brief Wi-Fi password sourced from secrets.h. */
const char* password = WIFI_PASS;
/** @brief NTP server hostname sourced from secrets.h for Elog timestamps. */
const char* ntpServer = NTP_SERVER;
/** @brief UTC offset in seconds for configTime() (-18000 = UTC-5, Eastern Standard Time). */
const long gmtOffset_sec = -18000;
/** @brief Daylight saving time offset in seconds (-14400 = UTC-4, Eastern Daylight Time). */
const int daylightOffset_sec = -14400;

/** @brief Raw ADS1115 reading for the COMM1 volume potentiometer (A0). Written by the ADS1115 ISR state machine; read by the MatPotentiometer DCS-BIOS input. */
uint16_t comm1Vol  = 0;
/** @brief Raw ADS1115 reading for the COMM2 volume potentiometer (A1). Written by the ADS1115 ISR state machine; read by the MatPotentiometer DCS-BIOS input. */
uint16_t comm2Vol  = 0;
/** @brief Raw ADS1115 reading for the UFC brightness potentiometer (A2). Renamed from ufcBrt to avoid a name collision with the MatPotentiometer instance declared later. */
uint16_t ufcBrtVal = 0;

/**
 * @brief  Connect to Wi-Fi and synchronise the system clock via NTP.
 *
 * @details Blocks until a WL_CONNECTED status is obtained, logging progress
 *          to the Elog syslog channel.  After connection, calls configTime()
 *          to set the local timezone offsets so that Elog timestamps are
 *          correct.  Only compiled when ENABLE_ELOG is defined.
 */
void connect_wifi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Logger.log(MYLOG, ELOG_LEVEL_INFO, "Connecting to WiFi..");
    }
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Connected to the WiFi network");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}
#endif

/**
 * @brief  When true, scan the I2C bus during setup() and print all found addresses.
 *
 * @details Set to true temporarily during board bring-up or fault diagnosis to
 *          enumerate attached I2C devices.  Leave false for normal operation to
 *          avoid adding setup latency.
 */
bool I2C_SCAN = false;

/** @brief Strings shown on the five option displays during the boot splash screen. Indices map to OP1-OP5. */
static const char* const BOOT_OP_MESSAGES[]        = {"MODE", "BIOS", "BLD", "H620", "F110"};
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
#define COM_DIGITS 4

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
/** @brief True when OTA mode was enabled at boot (ENC1PB held during power-on). */
static bool otaModeActive            = false;

/** @brief GO/NOGO LED on-time in milliseconds before auto-off. */
#define TIMEOUT 10000

/** @brief millis() timestamp when the current boot message splash was shown; used for the 10-second display timeout. */
static unsigned long bootMessageStartTime = 0;
/** @brief True while the boot message splash is being displayed on the UFC screens. */
static bool bootMessageActive             = false;


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
 *          RAM rather than slower flash on the ESP32S2.
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
  _ads1115_cnt++;
}
#endif

// ════════════════════════════════════════════════════════════════════════════
// Input Processing Functions
// ════════════════════════════════════════════════════════════════════════════

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
//uint8_t adf1, adf2;
static uint8_t lastadf;

 // UFCkeypad.readGPIO(); // Read all GPIO states to update the internal state of the TCA8418 driver
  UFCkeys[0][0] = UFCkeypad.digitalRead(UFC_IPSW);  // IP Switch
  UFCkeys[0][1] = UFCkeypad.digitalRead(UFC_EMCON); // EMCON Switch
  HUDkeys[0][0] = UFCkeypad.digitalRead(UFC_BITSW); // HUD Video Bit

  // Process ADF switch
  const uint8_t adfsw = (UFCkeypad.digitalRead(UFC_ADF1) << 2) | (UFCkeypad.digitalRead(UFC_ADF2) << 1);
  
  if (lastadf != adfsw) {
  // Use lookup table instead of switch for better efficiency
  static const char* const adf_values[] = {"0", "INVALID", "2", "INVALID", "0", "INVALID", "1", "INVALID"};
  if (adfsw == 2 || adfsw == 4 || adfsw == 6) {
    DcsBios::tryToSendDcsBiosMessage("UFC_ADF", adf_values[adfsw]);
     }
  }
  lastadf = adfsw;
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
    
    // Direct assignment instead of switch statement
    UFCkeys[row][col] = state ? 1 : 0;

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
 *          A 10 ms poll gate prevents redundant I2C reads on every loop()
 *          tick; the gate and debounce timers share the same captured millis()
 *          value to avoid calling millis() twice.
 *
 *          apkeys values are read by the DcsBios::MatActionButtonSet instances
 *          for the autopilot panel.
 *
 * @note    Called unconditionally from loop() (poll-driven, no interrupt).
 */
 void processAPKeypad() {
  // Optimized: APkeypad.read() is a full I2C transaction; previously called on every loop() iteration.
  // A 10ms poll interval is imperceptible for button presses but eliminates continuous I2C bus traffic.
  static unsigned long lastPollTime = 0;
  const unsigned long currentPollTime = millis();
  if (currentPollTime - lastPollTime < 10) return;
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
  int disp_pos = 0;

  for (int digit_num = 0; digit_num < OP_DIGITS; digit_num++) {
    sendAsciiChar_OH(disp_num, digit_num, op_display[disp_num].content[digit_num]);
  }
}

/**
 * @breif 
 */
// Lookup table for special character sequences
struct CharMapping {
    const char* str;
    uint8_t pos;
};

static const CharMapping SPECIAL_CHARS[] PROGMEM = {
    {"10", 10}, {"11", 11}, {"12", 12}, {"13", 13}, {"14", 14},
    {"15", 15}, {"16", 16}, {"17", 17}, {"18", 18}, {"19", 19},
    {"20", 20}, {"<>", 25}, {"==", 27}, {"++", 28}
};

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
    strcpy(COMM2_CHAR, comm_char);
}

// Backup of original display_comm_char before optimization — kept for reference/rollback
// void display_comm_char2(char* comm_char, int n_display) {
//     if (!comm_char || n_display < 0 || n_display > 1) {
//         return;
//     }

//     uint8_t array_pos = 0;

//     if (comm_char[0] != '\0') {
//         for (const auto& mapping : SPECIAL_CHARS) {
//             if (strcmp_P(comm_char, mapping.str) == 0) {
//                 array_pos = mapping.pos;
//                 break;
//             }
//         }

//         if (array_pos == 0) {
//             switch (comm_char[1]) {
//                 case '1': case '2': case '3': case '4': case '5':
//                 case '6': case '7': case '8': case '9':
//                     array_pos = comm_char[1] - '0';
//                     break;
//                 case 'M': array_pos = 21; break;
//                 case 'G': array_pos = 22; break;
//                 case 'C': array_pos = 23; break;
//                 case 'S': array_pos = 24; break;
//             }
//         }
//     }

//     matrix_COMM[n_display].setAll(false);
//     const uint8_t* char_set = (n_display == 1) ? COMM_CHAR2[array_pos] : COMM_CHAR1[array_pos];
//     for (uint8_t i = 0; i < 4; i++) {
//         matrix_COMM[n_display].setColumn(i, pgm_read_byte(&char_set[i]));
//     }
//     strcpy(COMM2_CHAR, comm_char);
// }

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
  switch (disp_num) {
  case 1: //OP Display 1 CUE
    op_display[disp_num].cueing ? GN1640[5].setSegments16(0x0001, 0b0100) : GN1640[5].setSegments16(0x0000, 0b0100); //0x10 -> SEG5 active, 0 -> GRID 1
    break;
  case 2: //OP Display 2 CUE
    op_display[disp_num].cueing ? GN1640[5].setSegments16(0x0002, 0b0011) : GN1640[5].setSegments16(0x0000, 0b0011); //0x08 -> SEG4 active, 1 -> GRID 2
    break;
  case 3: //OP Display 3 CUE
    op_display[disp_num].cueing ? GN1640[5].setSegments16(0x0004, 0b0010) : GN1640[5].setSegments16(0x0000, 0b0010); //0x04 -> SEG3 active, 2 -> GRID 3
    break;
  case 4: //OP Display 4 CUE
    op_display[disp_num].cueing ? matrix_COMM2.setColumn(14, 0x80, true) : matrix_COMM2.setColumn(14, 0, true); //0x80 -> SEG8 active, 14 -> GRID 15
    break;
  case 5: //OP Display 5 CUE
    op_display[disp_num].cueing ? matrix_COMM2.setColumn(15, 0x64, true) : matrix_COMM2.setColumn(15, 0, true); //0x64 -> SEG7 active, 15 -> GRID 16
    break;
  }
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

/**
 * @brief  DCS-BIOS callback: COMM1 radio channel display content.
 *
 * @details Receives a 2-character string representing the currently selected
 *          COMM1 channel symbol (e.g. " 1", "15", " M").  Caches the last
 *          transmitted value and only calls display_comm_char() when the
 *          content actually changes, avoiding unnecessary I2C traffic.
 *
 * @param newValue  Pointer to a 2-character null-terminated string from DCS-BIOS.
 */
void UFC_COMM_DISPLAY_1Change(char* newValue) {
    static char lastValue[3] = {0}; // Cache for last displayed value
    
    // Input validation
    if (!newValue) {
        return;
    }
    
    // Check if the new value is different from the cached value
    // Only update if there's a change to avoid unnecessary display updates
    if (strcmp(lastValue, newValue) != 0) {
        // Store new value in cache
        strncpy(lastValue, newValue, 2);
        lastValue[2] = '\0'; // Ensure null termination
        
        // Update display
        display_comm_char(newValue, 0);
    }
}
DcsBios::StringBuffer < 2 > UFC_COMM_DISPLAY_1(FA_18C_hornet_UFC_COMM1_DISPLAY_A, UFC_COMM_DISPLAY_1Change);

/**
 * @brief  DCS-BIOS callback: COMM2 radio channel display content.
 *
 * @details Identical caching behaviour to UFC_COMM_DISPLAY_1Change(); sends
 *          the 2-character channel symbol to display_comm_char() for display
 *          index 1 (COMM2) only when the value differs from the last update.
 *
 * @param newValue  Pointer to a 2-character null-terminated string from DCS-BIOS.
 */
void UFC_COMM_DISPLAY_2Change(char* newValue) {
    static char lastValue[3] = {0}; // Cache for last displayed value
    
    // Input validation
    if (!newValue) {
        return;
    }
    
    // Check if the new value is different from the cached value
    // Only update if there's a change to avoid unnecessary display updates
    if (strcmp(lastValue, newValue) != 0) {
        // Store new value in cache
        strncpy(lastValue, newValue, 2);
        lastValue[2] = '\0'; // Ensure null termination
        
        // Update display
        display_comm_char(newValue, 1);
    }
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
 * @brief  DCS-BIOS callback: UFC scratchpad string display 2 (right region, 2 chars).
 *
 * @details Placeholder for the right-side 2-character scratchpad region.
 *          Currently unused; reserved for future implementation.
 *
 * @param newValue  2-character null-terminated string from DCS-BIOS.
 */
void onUfcScratchpadString2DisplayChange(char* newValue) {
  (void)newValue; // Reserved for future use
}
DcsBios::StringBuffer<2> ufcScratchpadString2DisplayBuffer(FA_18C_hornet_UFC_SCRATCHPAD_STRING_2_DISPLAY_A, onUfcScratchpadString2DisplayChange);

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
    UFC_COMM_DISPLAY_1Change(const_cast<char*>(BOOT_COMM_MESSAGES[0]));
    UFC_COMM_DISPLAY_2Change(const_cast<char*>(BOOT_COMM_MESSAGES[1]));

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
    scratchpad.print_num(const_cast<char*>(BOOT_SCRATCHPAD_MESSAGES[1]), false);

    // Clear displays if requested
    if (!noClear) {
        static const char BLANK_DISPLAY[] = "    ";
        static const char BLANK_COMM[] = "  ";
        static const char BLANK_SCRATCHPAD[] = "      ";

        scratchpad.print_str(const_cast<char*>(BLANK_SCRATCHPAD), false);
        scratchpad.print_num(const_cast<char*>(BLANK_SCRATCHPAD), false);

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

  for (uint8_t address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0)
    {
      //Logger.log(MYLOG, ELOG_LEVEL_INFO, "I2C device found at address 0x%02x",address );   

      //Serial.print("I2C device found at address 0x");
      if (address < 0x10)
      Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Done");
}

// ════════════════════════════════════════════════════════════════════════════
// DCS-BIOS Input Mappings
// Global instances read by DcsBios::loop() on every cycle.
// ════════════════════════════════════════════════════════════════════════════

  DcsBios::MatActionButtonSet ufc1("UFC_1", & UFCkeys[1][0], LOW);
  DcsBios::MatActionButtonSet ufc2("UFC_2", & UFCkeys[1][1], LOW);
  DcsBios::MatActionButtonSet ufc3("UFC_3", & UFCkeys[1][2], LOW);

  DcsBios::MatActionButtonSet ufc4("UFC_4", & UFCkeys[2][0], HIGH);
  DcsBios::MatActionButtonSet ufc5("UFC_5", & UFCkeys[2][1], HIGH);
  DcsBios::MatActionButtonSet ufc6("UFC_6", & UFCkeys[2][2], HIGH);

  DcsBios::MatActionButtonSet ufc7("UFC_7", & UFCkeys[3][0], HIGH);
  DcsBios::MatActionButtonSet ufc8("UFC_8", & UFCkeys[3][1], HIGH);
  DcsBios::MatActionButtonSet ufc9("UFC_9", & UFCkeys[3][2], HIGH);
  DcsBios::MatActionButtonSet ufc0("UFC_0", & UFCkeys[4][1], HIGH);

  DcsBios::MatActionButtonSet ufcclr("UFC_CLR", & UFCkeys[4][0], HIGH);
  DcsBios::MatActionButtonSet ufcent("UFC_ENT", & UFCkeys[4][2], HIGH);
  DcsBios::MatActionButtonSet ufcip("UFC_IP", & UFCkeys[0][0], LOW);
  DcsBios::MatActionButtonSet ufcencon("UFC_EMCON", & UFCkeys[0][1], LOW);

  DcsBios::MatActionButtonSet ufcos1("UFC_OS1", & UFCkeys[0][3], HIGH);
  DcsBios::MatActionButtonSet ufcos2("UFC_OS2", & UFCkeys[1][3], HIGH);
  DcsBios::MatActionButtonSet ufcos3("UFC_OS3", & UFCkeys[2][3], HIGH);
  DcsBios::MatActionButtonSet ufcos4("UFC_OS4", & UFCkeys[3][3], HIGH);
  DcsBios::MatActionButtonSet ufcos5("UFC_OS5", & UFCkeys[4][3], HIGH);

  DcsBios::MatActionButtonSet hud_bit("HUD_VIDEO_BIT", & HUDkeys[0][0], LOW);

  typedef DcsBios::RotaryEncoderT < POLL_EVERY_TIME, DcsBios::TWO_STEPS_PER_DETENT > OHRotaryEncoder;

  OHRotaryEncoder ufcComm1ChannelSelect("UFC_COMM1_CHANNEL_SELECT", "DEC", "INC", ENC1A, ENC1B);
  OHRotaryEncoder ufcComm2ChannelSelect("UFC_COMM2_CHANNEL_SELECT", "DEC", "INC", ENC2A, ENC2B);
  DcsBios::Switch2Pos ufcComm1Pull("UFC_COMM1_PULL", ENC1PB);
  DcsBios::Switch2Pos ufcComm2Pull("UFC_COMM2_PULL", ENC2PB);

  DcsBios::MatActionButtonSet UFC_AP("UFC_AP", & apkeys[0][0], HIGH);
  DcsBios::MatActionButtonSet UFC_IFF("UFC_IFF", & apkeys[0][1], HIGH);
  DcsBios::MatActionButtonSet UFC_TCN("UFC_TCN", & apkeys[0][2], HIGH);
  DcsBios::MatActionButtonSet UFC_ILS("UFC_ILS", & apkeys[0][3], HIGH);
  DcsBios::MatActionButtonSet UFC_DL("UFC_DL", & apkeys[0][4], HIGH);
  DcsBios::MatActionButtonSet UFC_BCN("UFC_BCN", & apkeys[0][5], HIGH);
  DcsBios::MatActionButtonSet UFC_ONOFF("UFC_ONOFF", & apkeys[0][6], HIGH);

  //template <unsigned long pollIntervalMs = POLL_EVERY_TIME, unsigned int hysteresis = 128, unsigned int ewma_divisor = 5>
  // typedef DcsBios::PotentiometerEWMA < POLL_EVERY_TIME, 256, 10 > OHPotentiometer;
  // OHPotentiometer ufcBrt("UFC_BRT", A2, false, ANALOG_MIN_WIDTH, ANALOG_WIDTH);
  // OHPotentiometer ufccom1vol("UFC_COMM1_VOL", A0, false, ANALOG_MIN_WIDTH, ANALOG_WIDTH);
  // OHPotentiometer ufccom2vol("UFC_COMM2_VOL", A1, false, ANALOG_MIN_WIDTH, ANALOG_WIDTH);

  DcsBios::MatPotentiometer ufcComm1Vol("UFC_COMM1_VOL", nullptr, &comm1Vol);
  DcsBios::MatPotentiometer ufcComm2Vol("UFC_COMM2_VOL", nullptr, &comm2Vol);
  DcsBios::MatPotentiometer ufcBrt("UFC_BRT", nullptr, &ufcBrtVal); // fixed: was passing &ufcBrt (self-reference, wrong type, missing 3rd arg)


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
    // Only breathe when DCS is stopped and display is blank
    if (DCSMetaData.MissionStart != DSCSTOPPED || !displayBlank || goLedOn || noGoLedOn) {
        return;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - lastBreathUpdate >= BREATH_INTERVAL) {
        lastBreathUpdate = currentMillis;

        // Optimized: previous code computed sin() + 3 float multiplications only to threshold the result
        // at > 0.5, which is equivalent to a plain square wave. Replaced with a single integer modulo.
        bool ledState = (currentMillis % (unsigned long)BREATH_PERIOD) < ((unsigned long)BREATH_PERIOD / 2);
        UFCkeypad.digitalWrite(UFC_NOGO, ledState);
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
 *          After 10 seconds the splash is cleared via a second bootMessage() call.
 *          A 500 ms poll gate on the timeout check avoids calling millis() on
 *          every loop iteration while the splash is active.
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
    if (currentMissionState == DCSRUNNING) {
      bootMessageShownForCurrentStop = false;
    } else if (currentMissionState == DSCSTOPPED &&
               !bootMessageShownForCurrentStop &&
               !bootMessageActive) {
      bootMessage(true, true, true);
      bootMessageActive = true;
      bootMessageStartTime = currentMillis;
      bootMessageShownForCurrentStop = true;
    }
  }

  // Poll the 10-second timeout at 500ms resolution — coarse enough for a 10s timer,
  // avoids calling bootMessage() on every loop iteration while the message is active
  if (bootMessageActive && (currentMillis - lastTimeoutCheck >= 500)) {
    lastTimeoutCheck = currentMillis;
    if (currentMillis - bootMessageStartTime >= 10000) {
      bootMessage(true, false, false);
      bootMessageActive = false;
    }
  }
}

// Create USB CDC (Serial) and HID instances
#if defined(USE_TINYUSB)
  Adafruit_USBD_CDC usb_serial;
  Adafruit_USBD_HID usb_hid;
#endif
/**
 * @brief  Arduino one-time hardware and peripheral initialisation.
 *
 * @details Executed once at power-on or after a reset.  Performs the following
 *          in order:
 *          1. Starts Serial at 250000 baud and waits for connection.
 *          2. (ENABLE_ELOG) Configures Elog syslog and connects to Wi-Fi.
 *          3. Configures ENC1PB as INPUT_PULLUP for boot-time OTA detection.
 *          4. Initialises the I2C bus at 400 kHz (fast mode).
 *          5. Optionally scans the I2C bus (I2C_SCAN == true).
 *          6. Initialises the TCA8418 UFC keypad controller, configures the
 *             key matrix, GPIO pins, debounce, and attaches the CHANGE interrupt.
 *          7. Initialises the PCA9555 autopilot keypad expander.
 *          8. (USE_ADC_RDY) Initialises the ADS1115 ADC, attaches the DRDY
 *             interrupt, and starts the first single-shot conversion on COMM1_VOL.
 *          9. Initialises the WS2812B FastLED backlight strip.
 *         10. Initialises the HT1621 scratchpad LCD.
 *         11. (not USE_ADC_RDY) Configures the ESP32 native ADC.
 *         12. Shows the boot splash message.
 *         13. Turns off the FastLED strip (setup complete indication).
 *         14. Reads power source via powerSource().
 *         15. Tests both GO and NOGO LEDs for 2 seconds.
 *         16. Calls DcsBios::setup() to initialise the DCS-BIOS serial protocol.
 *         17. Checks ENC1PB for OTA mode activation; if held, starts WebOtaUpdate.
 */
void setup() {
  // Initial the TinyUSB Device
  #if defined(USE_TINYUSB)
    TinyUSBDevice.begin();
  #endif
  
  // Serial.begin(250000);
  // while (!Serial) delay(10);
  // Serial.println("Openhornet Up-Front Controller");

  #if defined (ENABLE_ELOG)
    Logger.configureSyslog("192.168.1.12", 514, "UFC"); // Syslog server IP, port and device name
    Logger.registerSyslog(MYLOG, ELOG_LEVEL_DEBUG, ELOG_FAC_USER, "counter"); // ...and syslog. Set the facility to user
    connect_wifi();
    delay(10);
    Logger.log(MYLOG, ELOG_LEVEL_INFO , "Openhornet Up-Front Controller HW Version %s, FW Version %s", BOOT_OP_MESSAGES[3], BOOT_OP_MESSAGES[4]);
  #endif
  
  // Ensure ENC1PB is readable early for boot-time OTA mode selection
  pinMode(ENC1PB, INPUT_PULLUP);
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

  // otamode: set later in setup() after ENC1PB is read (see end of setup).

  // ---------- UFC Keyboard Setup
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Wire.begin");
  #endif
  Wire.begin(SDApin, SCLpin);
  Wire.setClock(400000);
  //Wire.setTimeOut(30);
  delay(1000);

  if (I2C_SCAN) i2cscan();

  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "UFC Keyboard Setup");
  #endif
  if (!UFCkeypad.begin(TCA8418_DEFAULT_ADDR, & Wire)) {
    #if defined (ENABLE_ELOG)
      Logger.log(MYLOG, ELOG_LEVEL_INFO,"UFCkeypad not found, check wiring & pullups!");
    #endif
  } else {
    #if defined (ENABLE_ELOG)
      Logger.log(MYLOG, ELOG_LEVEL_INFO,"UFCkeypad found at %02x\n", TCA8418_DEFAULT_ADDR);
    #endif
  }

  UFCkeypad.matrix(TCA8418_ROWS, TCA8418_COLS);
 
  // ------------------ setup gpio interupt
  pinMode(IO_INT,INPUT);
  attachInterrupt(digitalPinToInterrupt(IO_INT), TCA8418_irq, CHANGE);
  UFCkeypad.pinMode(UFC_NOGO, OUTPUT); // NOGO LED
  UFCkeypad.pinMode(UFC_GO, OUTPUT); // GO LED
  
  UFCkeypad.pinMode(UFC_IPSW, INPUT_PULLUP); //5
  UFCkeypad.pinMode(UFC_BITSW, INPUT_PULLUP); //12
  UFCkeypad.pinMode(UFC_EMCON, INPUT_PULLUP); //17
  UFCkeypad.flush();
  UFCkeypad.enableDebounce();
  UFCkeypad.enableInterrupts();
  
  // ----------- AP Keyboard Setup
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "AP Keyboard Setup");
  #endif

  APkeypad.attach(Wire);
  if (APkeypad.i2c_error()) {
    #if defined (ENABLE_ELOG)
      Logger.log(MYLOG, ELOG_LEVEL_INFO, "APkeypad not found, check wiring & pullups!");
    #endif
  } else {
    #if defined (ENABLE_ELOG)
	    Logger.log(MYLOG, ELOG_LEVEL_INFO, "APkeypad found at ");
    #endif
  }
  APkeypad.polarity(PCA95x5::Polarity::INVERTED_ALL);
  APkeypad.direction(PCA95x5::Direction::IN_ALL);
  APkeypad.write(PCA95x5::Level::L_ALL);

  #if defined ( USE_ADC_RDY )
  // ----------- setup ADC Interupt

  pinMode(ADC_RDY_3_3V,INPUT);
  // Fixed: DRDY goes LOW when conversion is complete, so FALLING is the correct edge
  attachInterrupt(digitalPinToInterrupt(ADC_RDY_3_3V), ADS1115_irq, FALLING);
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
  }

  // Kick off the first single-shot read on COMM1_VOL (A0); state machine cycles through all channels.
  ads.startADCReading(ADS_MUX_COMM1_VOL, /*continuous=*/false);
  #endif 

  // ---------- Backlight Setup
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Backlight Setup");
  #endif
  FastLED.addLeds < WS2812B, BL_DATA_PIN, GRB > (ws2812b, BACKLIGHT_LEDS); // GRB ordering is typical
  FastLED.clear();
  fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
  FastLED.setBrightness(128);
  FastLED.show();

  // ---------- Scratchpad Setup
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Scratchpad Setup");
  #endif

  scratchpad.begin(SCRCS_3_3V, SCRWR_3_3V, SCRDATA_3_3V, SCRBL_3_3V); // (cs, wr, Data, backlight)
  scratchpad.clear(); // clear the screen

  #if not defined ( USE_ADC_RDY )
  //  --------- Analog Port Setup  
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Analog Port Setup");
  #endif
  analogSetAttenuation(ADC_11db);
  analogReadResolution(ANALOG_RESOLUTION);
  //analogSetClockDiv(15);
  #endif

  //  ---------- Shoow the boot message and keep it on display until sim connects
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Boot Message");
  #endif
  bootMessage(true, true, true);

  //  ---------- turn off backlight to show we're done with setup
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Turning off Backlight");
  #endif
  FastLED.clear();
  FastLED.show();

  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Check Power Source");
  #endif
  ufcState.detectPower();  // replaces powerSource() — result cached in ufcState for later queries

  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Test Go/NoGo LED");
  #endif
  UFCkeypad.digitalWrite(UFC_GO, HIGH); 
  UFCkeypad.digitalWrite(UFC_NOGO, HIGH);
  delay(2000);
  UFCkeypad.digitalWrite(UFC_GO, LOW); 
  UFCkeypad.digitalWrite(UFC_NOGO, LOW);

  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Start DCS-BIOS::setup()");
  #endif
  DcsBios::setup();
  #if defined (ENABLE_ELOG)
    Logger.log(MYLOG, ELOG_LEVEL_INFO, "Starting Arduino DCS-BIOS::loop()");
  #endif 

  // Enable OTA mode only if ENC1PB is held during boot (active-low with INPUT_PULLUP)
  otaModeActive = (digitalRead(ENC1PB) == LOW);
  ufcState.setOtaMode(otaModeActive);  // record in UfcState and persist via save() if desired
  if (otaModeActive) {
    Serial.println("[WebOTA] Boot button detected on COM1. OTA mode ENABLED.");
    WebOtaUpdate::begin();
  } else {
    Serial.println("[WebOTA] OTA mode skipped (COM1 not pushed at boot).");
  }
}

/**
 * @brief  Arduino main execution loop — called repeatedly after setup() completes.
 *
 * @details Each iteration performs the following work in order:
 *
 *          1. **OTA**: If OTA mode is active, services the HTTP server for
 *             firmware upload requests (WebOtaUpdate::handle()).
 *
 *          2. **UFC keypad**: If a TCA8418 interrupt event is pending,
 *             detaches the interrupt, clears the flag, calls processUFCKeypad()
 *             and processUFCIO() to decode the event, then re-attaches the
 *             interrupt.  If no event is pending, still calls processUFCKeypad()
 *             and processAPKeypad() for polling fallback.
 *
 *          3. **ADS1115 ADC** (USE_ADC_RDY only): If a DRDY interrupt fired,
 *             reads the completed channel, stores the result in the appropriate
 *             comm1Vol / comm2Vol / ufcBrtVal variable, and starts the next
 *             single-shot conversion (state machine cycling A0→A1→A2→A0).
 *
 *          4. **GO / NOGO LEDs**: Calls updateGoNoGoLedNonBlocking() to manage
 *             the mission-state indicator LEDs.
 *
 *          5. **NOGO breathing**: Calls breatheNogoLed() for the idle slow-blink
 *             effect when DCS is not running.
 *
 *          6. **Boot message**: Calls handleBootMessage() to show or clear the
 *             splash screen on DCS state transitions.
 *
 *          7. **DCS-BIOS**: Calls DcsBios::loop() to process incoming USB serial
 *             data, fire registered callbacks, and send queued output messages.
 */
void loop() {

  // Handle HTTP OTA page and upload requests only in boot-selected OTA mode
  if (ufcState.getOtaMode()) {
    WebOtaUpdate::handle();
  }

  // ── Keyboard and ADC input ────────────────────────────────────────────────
  if (ufcState.getIrqMode()) {
    // IRQ mode: TCA8418 keypad driven by TCA8418_event; ADS1115 driven by ADS1115_event.
    // Keypad and ADC are checked independently — a fired ADC IRQ must not skip keypad.

    if (TCA8418_event) {
      detachInterrupt(digitalPinToInterrupt(IO_INT));
      TCA8418_event = false;
      processUFCKeypad();
      processUFCIO();
      attachInterrupt(digitalPinToInterrupt(IO_INT), TCA8418_irq, CHANGE);
    }
    processAPKeypad();  // AP keypad always polled (PCA9555 has no IRQ line)

    // ADS1115 state machine: 2 I2C transactions per DRDY interrupt (read + start next channel)
    if (ADS1115_event) {
      ADS1115_event = false;
      static uint8_t adsChannel = ADS_CH_COMM1_VOL;
      switch (adsChannel) {
        case ADS_CH_COMM1_VOL:
          comm1Vol  = (uint16_t)ads.getLastConversionResults();
          ads.startADCReading(ADS_MUX_COMM2_VOL, /*continuous=*/false);
          adsChannel = ADS_CH_COMM2_VOL;
          break;
        case ADS_CH_COMM2_VOL:
          comm2Vol  = (uint16_t)ads.getLastConversionResults();
          ads.startADCReading(ADS_MUX_UFC_BRT, /*continuous=*/false);
          adsChannel = ADS_CH_UFC_BRT;
          break;
        case ADS_CH_UFC_BRT:
          ufcBrtVal = (uint16_t)ads.getLastConversionResults();
          ads.startADCReading(ADS_MUX_COMM1_VOL, /*continuous=*/false);
          adsChannel = ADS_CH_COMM1_VOL;
          break;
      }
    }
  } else {
    // Polling mode: unconditionally call keyboard and ADC reads each loop iteration.
    processUFCKeypad();
    processAPKeypad();

    static unsigned long lastAdcPollTime = 0;
    const unsigned long  adcPollInterval = 100;  // ms between ADS1115 reads
    if (millis() - lastAdcPollTime >= adcPollInterval) {
      lastAdcPollTime = millis();
      comm1Vol  = (uint16_t)ads.readADC_SingleEnded(ADS_CH_COMM1_VOL);
      comm2Vol  = (uint16_t)ads.readADC_SingleEnded(ADS_CH_COMM2_VOL);
      ufcBrtVal = (uint16_t)ads.readADC_SingleEnded(ADS_CH_UFC_BRT);
    }
  }

  //  hud bit leds showing if mission is running or not and show for 10 seconds then turn off
  //  GO = MISSION Running
  //  NOGO = MISSION NOT RUNNING

  updateGoNoGoLedNonBlocking();
  breatheNogoLed(); // Add breathing effect for NOGO LED


  // Handle boot message display
  if (ufcState.getBootMode()) {
  handleBootMessage();
  }
  
  // Main DCS-BIOS loop
  DcsBios::loop();
}
