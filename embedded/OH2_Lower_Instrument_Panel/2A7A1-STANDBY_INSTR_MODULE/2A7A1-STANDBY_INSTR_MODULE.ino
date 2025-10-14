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
*   Copyright 2016-2024 OpenHornet
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
* @file 2A7A1-STANDBY_INSTR_MODULE
* @author Thib-O with the help of @Circuit, @BnepeThomas, @No1Sonuk, @Ulukaii and @Sandra
* @date 14.10.2025
* @version t.0.0.9 (tested on USB, and with bus RS485)
* @brief Code for standby controller to drive the standby the standby intrument panel.
*/

/**
* Check if we're on a Mega328 or Mega2560 and define the correct
* serial interface
* 
*/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
  #define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
  #include <avr/power.h>
#endif

/**
* The Arduino pin that is connected to the
* RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 2 ///< Sets TXENABLE_PIN to Arduino Pin 2
#define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

/**
* 32U4 now works as a slave only.   uncomment below to make this device a RS485 SLAVE
* Be sure to set slave address per INTERCONNECT
*/
//#define DCSBIOS_RS485_SLAVE 5 

/**
* If using a SERVO Motor (only used in Radar Altimeter) leave un-commented
* otherwise comment out to not include the SERVO.H library
*/
#define DCSBIOS_DISABLE_SERVO

#define FASTLED_INTERRUPT_RETRY_COUNT 1     // Define the number of retries for FastLED update


#include "DcsBios.h"
#include "Arduino.h"
#include <Stepper.h>
#include <MultiMap.h>              // by Rob Tillart
#include <FastLED.h>
#include <SPI.h>
#include <Wire.h>
extern "C" {
#include "utility/twi.h"           // from Wire library, so we can do bus scanning
}
#include <U8g2lib.h>               // by Oli Kraus


/////////////////////////////////////////////////////////////////////////////////////////////////////

// BACKLIGHTING CONFIGURATION (WHEN JUMPER ARE MOVED TO PIN 2 AND 3)
#define BACKLIGHT_PIN 11
#define BACKLIGHT_COUNT 5

// Define the array of ws2812
CRGB ws2812[BACKLIGHT_COUNT];

// COMMON STEPPER CONFIGURATION
#define STEPS 720

// AIRSPEED STEPPER CONFIGURATIION
#define AIR_ZERO_SENSE_PIN A0
#define AIR_OFFSET_TO_ZERO_POINT 703 // CHANGE VALUE BETWEEN 0 TO 720 TO SET NEEDLE TO 0 KNOT

#define COIL_STANDBY_AIR_A1 46
#define COIL_STANDBY_AIR_A2 48
#define COIL_STANDBY_AIR_A3 50
#define COIL_STANDBY_AIR_A4 52

Stepper stepperSTANDBY_AIR(STEPS, COIL_STANDBY_AIR_A1, COIL_STANDBY_AIR_A2, COIL_STANDBY_AIR_A3, COIL_STANDBY_AIR_A4);

int valAIR = 0;
int posAIR = 0;
int AIR = 0;

// ALTIMETER STEPPER CONFIGURATIION
#define ALT_ZERO_SENSE_PIN A1
#define ALT_OFFSET_TO_ZERO_POINT 0 // CHANGE VALUE BETWEEN 0 TO 720 TO SET NEEDLE TO 0 FT

#define COIL_STANDBY_ALT_A1 42
#define COIL_STANDBY_ALT_A2 44
#define COIL_STANDBY_ALT_A3 38
#define COIL_STANDBY_ALT_A4 40

Stepper stepperSTANDBY_ALT(STEPS, COIL_STANDBY_ALT_A1, COIL_STANDBY_ALT_A2, COIL_STANDBY_ALT_A3, COIL_STANDBY_ALT_A4);

int valALT = 0;
int posALT = 0;
int ALT = 0;
int DiffvalposALT = 0;
unsigned long LastALTAutoRecalTime;
bool GAUGE_RECAL_READY = false;
bool ALT_ZERO_LAST_STATE = HIGH;

// OLED ALTIMETER CONFIGURATION
#define TCAADDR 0x70

/**
 * @brief Selects a specific channel on the TCA9548A I2C multiplexer
 * @param i Channel number to select (0-7)
 * @see This function is called by setup() and updateBARO() and updateALT() functions
 */
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

#define BARO_OLED_Port 0
#define ALT_OLED_Port 1

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_BARO(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2_ALT(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

String BaroOnes = "2";
String BaroTens = "9";
String BaroHundreds = "9";
String BaroThousands = "2";
bool BaroUpdated = true; 

String Alt1000s = "0";
String LastAlt1000s = "";
String Alt10000s = "0";
String LastAlt10000s = "";
bool AltUpdated = true;

// VVI STEPPER CONFIGURATIION
#define VVI_ZERO_SENSE_PIN A2
#define VVI_OFFSET_TO_ZERO_POINT 360 // CHANGE VALUE BETWEEN 0 TO 720 TO SET NEEDLE TO 0 FT/MIN

#define COIL_STANDBY_VVI_A1 32
#define COIL_STANDBY_VVI_A2 30
#define COIL_STANDBY_VVI_A3 36
#define COIL_STANDBY_VVI_A4 34

Stepper stepperSTANDBY_VVI(STEPS, COIL_STANDBY_VVI_A1, COIL_STANDBY_VVI_A2, COIL_STANDBY_VVI_A3, COIL_STANDBY_VVI_A4);

int valVVI = 0;
int posVVI = 0;
int VVI = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////

// AIRSPEED CONFIGURATION TO DCS
//NO LINEAR MAPPING TO KEEP ACCURACY
float AIRInputArray[29] = {
  0,2680,3980,5760,7560,9400,13050,16000,19510,22850,25390,28560,31750,34870,38400,41720,44160,46900,49060,51200,53630,55600,56980,58800,60380,62200,63930,65500,65535
};
float AIROutputArray[29] = {
  0,30,48,65,83,100,112,166,198,230,266,302,338,374,410,445,480,504,527,549,571,588,604,620,637,656,675,690,720
};
/**
 * @brief Callback function for standby airspeed indicator changes from DCS-BIOS
 * @param newValue New airspeed value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyAsiAirspeedBuffer
 */
void onStbyAsiAirspeedChange(unsigned int newValue) {
  AIR = multiMapBS<float>(newValue, AIRInputArray, AIROutputArray, 29);
}
DcsBios::IntegerBuffer stbyAsiAirspeedBuffer(FA_18C_hornet_STBY_ASI_AIRSPEED, onStbyAsiAirspeedChange);

// VVI CONFIGURATION TO DCS
//NO LINEAR MAPPING TO KEEP ACCURACY
float VVIInputArray[13] = {
  0,5500,9000,13000,19900,26000,32767,40000,45800,52600,56500,59800,65535
};
float VVIOutputArray[13] = {
  20,110,160,210,260,310,360,410,460,510,560,610,700
};

/**
 * @brief Callback function for vertical velocity indicator changes from DCS-BIOS
 * @param newValue New VVI value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer vsiBuffer
 */
void onVsiChange(unsigned int newValue) {
  VVI = multiMapBS<float>(newValue, VVIInputArray, VVIOutputArray, 13);
}
DcsBios::IntegerBuffer vsiBuffer(FA_18C_hornet_VSI, onVsiChange);

// ALTIMETER CONFIGURATION TO DCS
/**
 * @brief Updates the barometric pressure display on the OLED screen
 * @param strnewValue String containing the new barometric pressure value to display
 * @see This function is called by buildBAROString() and setup() functions
 */
void updateBARO(String strnewValue) {
  const char* newValue = strnewValue.c_str();
  tcaselect(BARO_OLED_Port);
  u8g2_BARO.setFontMode(0);
  u8g2_BARO.setDrawColor(0);
  u8g2_BARO.drawBox(0, 0, 128, 32);
  u8g2_BARO.setDrawColor(1);
  u8g2_BARO.setFontDirection(2);
  u8g2_BARO.drawStr(115, 0, newValue);
  u8g2_BARO.sendBuffer();
}

/**
 * @brief Builds and displays the complete barometric pressure string from individual digits
 * @see This function is called by loop() function when BaroUpdated is true
 */
void buildBAROString() {
  updateBARO(BaroThousands + BaroHundreds + BaroTens + BaroOnes);
  BaroUpdated = false;
}

/**
 * @brief Updates the altitude display on the OLED screen with ten-thousands and thousands digits
 * @param strTenThousands String containing the ten-thousands digit of altitude
 * @param strThousands String containing the thousands digit of altitude
 * @see This function is called by loop() function when AltUpdated is true
 */
void updateALT(String strTenThousands, String strThousands) {
  if (strTenThousands == LastAlt10000s && strThousands == LastAlt1000s) {
    return;
  }

  const char* newTenThousandsValue = strTenThousands.c_str();
  const char* newThousandsValue = strThousands.c_str();

  int End_X_Pos = 46;
  int End_Y_Pos = 28;
  int Start_Y_Pos = 13;
  int Start_X_Pos = 27;
  int Box_Width = 20;

  tcaselect(ALT_OLED_Port);
  u8g2_ALT.setFontMode(0);
  u8g2_ALT.setDrawColor(0);
  u8g2_ALT.drawBox(0, 0, 128, 32);
  u8g2_ALT.setDrawColor(1);

  if (strTenThousands == "0") {
    u8g2_ALT.drawBox(Start_X_Pos, 13, Box_Width, 20);
    u8g2_ALT.setDrawColor(0);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos, End_X_Pos, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 1, End_X_Pos - 1, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 2, End_X_Pos - 2, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 3, End_X_Pos - 3, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 4, End_X_Pos - 4, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 5, End_X_Pos - 5, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 6, End_X_Pos - 6, 32);

    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 11, End_X_Pos - 11, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 12, End_X_Pos - 12, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 13, End_X_Pos - 13, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 14, End_X_Pos - 14, 32);
    u8g2_ALT.drawLine(Start_X_Pos, Start_Y_Pos + 15, End_X_Pos - 15, 32);

    u8g2_ALT.drawLine(Start_X_Pos + 4, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos + 1);
    u8g2_ALT.drawLine(Start_X_Pos + 5, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos);
    u8g2_ALT.drawLine(Start_X_Pos + 6, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 1);
    u8g2_ALT.drawLine(Start_X_Pos + 7, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 2);
    u8g2_ALT.drawLine(Start_X_Pos + 8, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 3);
    u8g2_ALT.drawLine(Start_X_Pos + 9, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 4);
    u8g2_ALT.drawLine(Start_X_Pos + 10, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 5);

    u8g2_ALT.drawLine(Start_X_Pos + 15, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 10);
    u8g2_ALT.drawLine(Start_X_Pos + 16, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 11);
    u8g2_ALT.drawLine(Start_X_Pos + 17, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 12);
    u8g2_ALT.drawLine(Start_X_Pos + 18, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 13);
    u8g2_ALT.drawLine(Start_X_Pos + 19, Start_Y_Pos, Start_X_Pos + Box_Width, End_Y_Pos - 14);

    u8g2_ALT.setDrawColor(1);

  } else {
    u8g2_ALT.drawStr(32, 32, newTenThousandsValue);
  }
  u8g2_ALT.drawStr(65, 32, newThousandsValue);
  u8g2_ALT.sendBuffer();

  LastAlt10000s = strTenThousands;
  LastAlt1000s = strThousands;

  AltUpdated = false;
}

/**
 * @brief Callback function for standby altimeter 100ft pointer changes from DCS-BIOS
 * @param newValue New altimeter 100ft pointer value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyAlt100FtPtrBuffer
 */
void onStbyAlt100FtPtrChange(unsigned int newValue) {
  ALT = map(newValue, 0, 65535, 0, 720);
}
DcsBios::IntegerBuffer stbyAlt100FtPtrBuffer(FA_18C_hornet_STBY_ALT_100_FT_PTR, onStbyAlt100FtPtrChange);

/**
 * @brief Callback function for standby altimeter thousands digit changes from DCS-BIOS
 * @param newValue New altimeter thousands digit value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyAlt1000FtCntBuffer
 */
void onStbyAlt1000FtCntChange(unsigned int newValue) {
  if (newValue < 6553) Alt1000s = "0";
  else if (newValue < 13106) Alt1000s = "1";
  else if (newValue < 19660) Alt1000s = "2";
  else if (newValue < 26214) Alt1000s = "3";
  else if (newValue < 32767) Alt1000s = "4";
  else if (newValue < 39321) Alt1000s = "5";
  else if (newValue < 45874) Alt1000s = "6";
  else if (newValue < 52428) Alt1000s = "7";
  else if (newValue < 58981) Alt1000s = "8";
  else Alt1000s = "9";
  AltUpdated = true;
}
DcsBios::IntegerBuffer stbyAlt1000FtCntBuffer(0x74f8,0xffff,0, onStbyAlt1000FtCntChange);

/**
 * @brief Callback function for standby altimeter ten-thousands digit changes from DCS-BIOS
 * @param newValue New altimeter ten-thousands digit value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyAlt10000FtCntBuffer
 */
void onStbyAlt10000FtCntChange(unsigned int newValue) {
  if (newValue < 7280) Alt10000s = "0";
  else if (newValue < 14560) Alt10000s = "1";
  else if (newValue < 21843) Alt10000s = "2";
  else if (newValue < 29122) Alt10000s = "3";
  else if (newValue < 36400) Alt10000s = "4";
  else if (newValue < 43684) Alt10000s = "5";
  else if (newValue < 50967) Alt10000s = "6";
  else if (newValue < 58245) Alt10000s = "7";
  else Alt10000s = "8";
  AltUpdated = true;
}
DcsBios::IntegerBuffer stbyAlt10000FtCntBuffer(0x74f6,0xffff,0, onStbyAlt10000FtCntChange);

/**
 * @brief Callback function for standby barometric pressure ones digit changes from DCS-BIOS
 * @param newValue New barometric pressure ones digit value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyPressSet0Buffer
 */
void onStbyPressSet0Change(unsigned int newValue) {
  if (newValue < 6553) BaroOnes = "0";
  else if (newValue < 13106) BaroOnes = "1";
  else if (newValue < 16301) BaroOnes = "2";
  else if (newValue < 19660) BaroOnes = "3";
  else if (newValue < 29918) BaroOnes = "4";
  else if (newValue < 36727) BaroOnes = "5";
  else if (newValue < 43536) BaroOnes = "6";
  else if (newValue < 50345) BaroOnes = "7";
  else if (newValue < 53284) BaroOnes = "8";
  else BaroOnes = "9";
  BaroUpdated = true;
}
DcsBios::IntegerBuffer stbyPressSet0Buffer(0x74fa, 0xffff, 0, onStbyPressSet0Change);

/**
 * @brief Callback function for standby barometric pressure tens digit changes from DCS-BIOS
 * @param newValue New barometric pressure tens digit value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyPressSet1Buffer
 */
void onStbyPressSet1Change(unsigned int newValue) {
  if (newValue < 6553) BaroTens = "0";
  else if (newValue < 13106) BaroTens = "1";
  else if (newValue < 16301) BaroTens = "2";
  else if (newValue < 19660) BaroTens = "3";
  else if (newValue < 29918) BaroTens = "4";
  else if (newValue < 36727) BaroTens = "5";
  else if (newValue < 43536) BaroTens = "6";
  else if (newValue < 50345) BaroTens = "7";
  else if (newValue < 53284) BaroTens = "8";
  else BaroTens = "9";
  BaroUpdated = true;
}
DcsBios::IntegerBuffer stbyPressSet1Buffer(0x74fc, 0xffff, 0, onStbyPressSet1Change);

/**
 * @brief Callback function for standby barometric pressure thousands and hundreds digits changes from DCS-BIOS
 * @param newValue New barometric pressure thousands/hundreds digit value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer stbyPressSet2Buffer
 */
void onStbyPressSet2Change(unsigned int newValue) {  
  if (newValue < 39321) BaroThousands = "2",BaroHundreds = "8" ;
  else if (newValue < 52428) BaroThousands = "2",BaroHundreds = "9" ;
  else if (newValue < 65535) BaroThousands = "3",BaroHundreds = "0" ;
  else BaroThousands = "3",BaroHundreds = "1" ;
  BaroUpdated = true;
}
DcsBios::IntegerBuffer stbyPressSet2Buffer(0x74fe, 0xffff, 0, onStbyPressSet2Change);

//////////////////////////////////////////////////////////////////////////////////////////////////////

// SAI-RWR-ALT BUTTONs/ENCODERs CONFIGURATION

//SAI
DcsBios::RotaryEncoder saiSet("SAI_SET", "-800", "+800", 24, 26);  // SAI ENCODER CONFIG
DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", 28);                // SAI TEST BUTTON CONFIG
DcsBios::Switch2Pos saiCage("SAI_CAGE", 22);                       // SAI BUTTON ON ENCODER CONFIG
//RWR
DcsBios::Potentiometer rwrRwrIntesity("RWR_RWR_INTESITY", A3);
//ALTIMETER
DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-800", "+800", 13, 12);

//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Callback function for backlighting control -only relevant if jumper is moved to pin 2 and 3
 * @param newValue New instrument lighting intensity value from DCS-BIOS (0-65535)
 * @see This function is called by DcsBios::IntegerBuffer instrIntLtBuffer
 */
void onInstrIntLtChange(unsigned int newValue) {
FastLED.setBrightness(map(newValue, 0, 65535, 0, 255));
FastLED.show();
//ws2812[0] = CRGB(0, map(newValue, 0, 65535, 0, 255), 0);
}
DcsBios::IntegerBuffer instrIntLtBuffer(FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange);

//////////////////////////////////////////////////////////////////////////////////////////////////////

int MAX487_TX = 2;  //AVOID MAX487 INTERACTION WHILE WORKING IN USB

/**
 * @brief Initializes the standby instrument module with OLED displays, stepper motors, and DCS-BIOS communication
 * @see This function is called automatically by Arduino framework at startup
 */
void setup() {
  #if (not defined( DCSBIOS_RS485_SLAVE) || not defined(DCSBIOS_RS485_MASTER ))   // Disable RS485 TRANSCEIVER IF NOT BEING USED
    pinMode(TXENABLE_PIN, OUTPUT);    // WILL DISABLE TX ONLY
    digitalWrite(TXENABLE_PIN, HIGH); // SET TX HIGH TO DISABLE TX ON TRANSCEIVER
  #endif

  //OLEDS ALTIMETER SETUP
  Wire.begin();  
  for (uint8_t t = 0; t < 8; t++) {
    tcaselect(t);

    for (uint8_t addr = 0; addr <= 127; addr++) {
      //if (addr == TCAADDR) continue;

      uint8_t data;
      if (!twi_writeTo(addr, &data, 0, 1, 1)) {
        // Had to comment out these debugging messages as they created a conflict with the IRQ definition in DCS BIOS
      }
    }
  }

  tcaselect(BARO_OLED_Port);
  u8g2_BARO.begin();
  u8g2_BARO.clearBuffer();
  u8g2_BARO.setFont(u8g2_font_fub14_tr);
  u8g2_BARO.sendBuffer();

  tcaselect(ALT_OLED_Port);
  u8g2_ALT.begin();
  u8g2_ALT.clearBuffer();
  u8g2_ALT.setFont(u8g2_font_fub20_tr);
  u8g2_ALT.sendBuffer();

  updateALT("0", "0");
  updateBARO("2992");

  //AIRSPEED STEPPER HOMING
  pinMode(AIR_ZERO_SENSE_PIN, INPUT_PULLUP);
  stepperSTANDBY_AIR.setSpeed(60);
  stepperSTANDBY_AIR.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_AIR.step(1);
    if (digitalRead(AIR_ZERO_SENSE_PIN) == 0) {
      delay(500);
      stepperSTANDBY_AIR.step(AIR_OFFSET_TO_ZERO_POINT);
      break;
    }
  }

  posAIR - 0;
  AIR = map(0, 0, 65535, 0, 720);

  //ALTIMETER STEPPER HOMING
  pinMode(ALT_ZERO_SENSE_PIN, INPUT_PULLUP);
  stepperSTANDBY_ALT.setSpeed(60);
  stepperSTANDBY_ALT.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_ALT.step(1);
    if (digitalRead(ALT_ZERO_SENSE_PIN) == 0) {
      delay(500);
      stepperSTANDBY_ALT.step(ALT_OFFSET_TO_ZERO_POINT);
      break;
    }
  }

  posALT = 0;
  ALT = 0;

  //VVI STEPPER HOMING
  pinMode(VVI_ZERO_SENSE_PIN, INPUT_PULLUP);
  stepperSTANDBY_VVI.setSpeed(60);
  stepperSTANDBY_VVI.step(1000);
  for (int i = 0; i <= 2000; i++) {
    delay(1);
    stepperSTANDBY_VVI.step(1);
    if (digitalRead(VVI_ZERO_SENSE_PIN) == 0) {
      delay(500);
      stepperSTANDBY_VVI.step(VVI_OFFSET_TO_ZERO_POINT);
      break;
    }
  }

  posVVI - 0;
  VVI = map(0, 0, 65535, 0, 720);

  FastLED.addLeds<WS2812B, BACKLIGHT_PIN, RGB>(ws2812, BACKLIGHT_COUNT);  // GRB ordering is typical
  fill_solid(ws2812, BACKLIGHT_COUNT, CRGB::Black); 
  FastLED.show();
  fill_solid(ws2812, BACKLIGHT_COUNT, CRGB::LightGreen); 

  DcsBios::setup();
}

/**
 * @brief Main program loop that handles DCS-BIOS communication, stepper motor control, and OLED updates
 * @see This function is called continuously by Arduino framework after setup()
 */
void loop() {
  DcsBios::loop();

  //UPDATE THE OLEDS EVERY 100 MS TO AVOID LAG WITH STEPPERS
  static unsigned long lastOLEDUpdate = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastOLEDUpdate >= 100) { //EVERY 100 MS
    lastOLEDUpdate = currentMillis;

    if (BaroUpdated == true) buildBAROString();
    if (AltUpdated == true) updateALT(Alt10000s, Alt1000s);
  }

  //MOVE AIRSPEED STEPPER
  valAIR = AIR;
  if (abs(valAIR - posAIR) > 2) {  //IF DIFFERENCE IS GREATER THAN 2 STEPS 
    if ((valAIR - posAIR) > 0) {
      stepperSTANDBY_AIR.step(1); //MOVE CLOCKWISE
      posAIR++;
    }
    if ((valAIR - posAIR) < 0) {
      stepperSTANDBY_AIR.step(-1); //MOVE COUNTERCLOCKWISE
      posAIR--;
    }
  }

  //MOVE ALTIMETER STEPPER
  valALT = ALT;
  DiffvalposALT = valALT - posALT;

  if (DiffvalposALT > 359) {
    DiffvalposALT -= 720; //ALLOW LOOPBACK
  }
  if (DiffvalposALT < -359) {
    DiffvalposALT += 720; //ALLOW LOOPBACK
  }

  if (DiffvalposALT != 0) {
    if (DiffvalposALT > 0) {
      stepperSTANDBY_ALT.step(1); //MOVE CLOCKWISE
      posALT++;
      if (posALT >= 720) posALT = 0; //LOOPBACK TO KEEP posALT BETWEEN 0 AND 720 WHEN THE NEEDLE IS PASSING 0 FEET
    } else {
      stepperSTANDBY_ALT.step(-1); //MOVE COUNTERCLOCKWISE
      posALT--;
      if (posALT < 0) posALT = 719; //LOOPBACK TO KEEP posALT BETWEEN 0 AND 720 WHEN THE NEEDLE IS PASSING 0 FEET
    }
  }

  //AUTO RECALIBRATION IF WE PASS THROUGH PHYSICAL ZERO EVERY 2 MINUTES TO AVOID PERMANENT OFFSET
  unsigned long now = millis();
  if (now - LastALTAutoRecalTime >= 120000UL) {  //2 MINUTES = 120000 MS
    GAUGE_RECAL_READY = true;  //READY TO RECALIBRATE AT NEXT ZERO
  }

  bool ALT_ZERO_CURRENT_STATE = digitalRead(ALT_ZERO_SENSE_PIN);

  if (GAUGE_RECAL_READY && ALT_ZERO_LAST_STATE == HIGH && ALT_ZERO_CURRENT_STATE == LOW) {
    posALT = 0;
    LastALTAutoRecalTime = now;
    GAUGE_RECAL_READY = false;  //WAIT NEXT TWO MINUTES BEFORE ANOTHER RECAL
  }
  
  ALT_ZERO_LAST_STATE = ALT_ZERO_CURRENT_STATE;

  //MOVE VVI STEPPER
  valVVI = VVI;
  if (abs(valVVI - posVVI) > 2) { //IF DIFFERENCE IS GREATER THAN 2 STEPS 
    if ((valVVI - posVVI) > 0) {
      stepperSTANDBY_VVI.step(1); //MOVE CLOCKWISE
      posVVI++;
    }
    if ((valVVI - posVVI) < 0) {
      stepperSTANDBY_VVI.step(-1); //MOVE COUNTERCLOCKWISE
      posVVI--;
    }
  }
}
