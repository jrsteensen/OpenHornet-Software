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
 * @file 2A15A1-PIT_MANAGEMENT_SYSTEM.ino
 * @author Adam Apell (AdamA)
 * @date 5.10.2026
 * @version 1.0.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls ABSIS Pit Management System Board for use with SIM PWR Panel
 * @details This sketch is for the LIP ABSIS Pit Management System
 * @todo The following functions are not yet implemented: Preflight Mode, Maintanance Mode, PSU Status beyond "Not Ready State"
 * 
 *  * **Reference Designator:** 2A15A1
 *  * **Intended Board:** ABSIS Pit Management System
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 4   | EXT_ACC1 (External Connection)
 * 5   | EXT_ACC2 (External Connection)
 * 6   | EXT_ACC3 (External Connection)
 * 7   | EXT_ACC4 (External Connection)
 * 8   | PS_ON (Active Low Power Supply Turn On Control)
 * 9   | PWR_OK (Active High Power Supply Stable Feedback)
 * 14  | SIMPWR_MAINT_MODE
 * 15  | SIMPWR_PFLT_MODE
 * 18  | SIMPWR_DISPON
 * 19  | SIMPWR_MASTERON (Sim Pit Master Power)
 * 20  | SIMPWR_PCRESET
 * 21  | SIMPWER_PCPWR
 *
 */

// Includes
#ifdef __AVR__
#include <avr/power.h>
#endif

// Define pins per interconnect diagram.
#define EXT_ACC1 4            // D4/A6, PD4, ADC8
#define EXT_ACC2 5            // D5,    PC6
#define EXT_ACC3 6            // D6/A7, PD7, ADC10
#define EXT_ACC4 7            // D7,    PE6, HW Interrupt (INT6)
#define PS_ON 8               // D8/A8, PB4, ADC11, Pin Change Interrupt (PCINT4)
#define PWR_OK 9              // D9#/A9, PB5, ADC12, Pin Change Interrupt (PCINT5)
#define SIMPWR_MAINT_MODE 14  // D14,   PB3, MISO,  Pin Change Interrupt (PCINT3)
#define SIMPWR_PFLT_MODE 15   // D15,   PB1, SCK,   Pin Change Interrupt (PCINT1)
#define SIMPWR_DISPON 18      // A0,    PF7, ADC7
#define SIMPWR_MASTERON 19    // A1,    PF6, ADC6
#define SIMPWR_PCRESET 20     // A2,    PF5, ADC5
#define SIMPWR_PCPWR 21       // A3,    PF4, ADC4

// Initialize Global Variables at MCU Power-On
volatile int CurrentSwitchState_SIMPWR_MAINT_MODE;  // Initialize global variable for Maintanance Mode Switch Current State
volatile int CurrentSwitchState_SIMPWR_PFLT_MODE;   // Initialize global variable for Maintanance Mode Switch Current State
volatile int CurrentSwitchState_SIMPWR_DISPON;      // Initialize global variable for Maintanance Mode Switch Current State
volatile int CurrentSwitchState_SIMPWR_MASTERON;    // Initialize global variable for Master Switch Monitoring Current State
volatile int CurrentSwitchState_SIMPWR_PCRESET;     // Initialize global variable for PC Reset Switch Current State
volatile int CurrentSwitchState_SIMPWR_PCPWR;       // Initialize global variable for PC Power Switch Current State
volatile int CurrentPowerSupplyState;               // Initialize global variable for PSU monitoring Current State

volatile int LastSwitchState_SIMPWR_MAINT_MODE;  // Initialize global variable for Maintanance Mode Switch Last State
volatile int LastSwitchState_SIMPWR_PFLT_MODE;   // Initialize global variable for Maintanance Mode Switch Last State
volatile int LastSwitchState_SIMPWR_DISPON;      // Initialize global variable for Maintanance Mode Switch Last State
volatile int LastSwitchState_SIMPWR_MASTERON;    // Initialize global variable for Master Switch Monitoring Last State
volatile int LastSwitchState_SIMPWR_PCRESET;     // Initialize global variable for PC Reset Switch Last State
volatile int LastSwitchState_SIMPWR_PCPWR;       // Initialize global variable for PC Power Switch Last State
volatile int LastPowerSupplyState;               // Initialize global variable for PSU monitoring Last State

// Function Prototypes
void Maint_Mode();
void Preflight_Mode();
void Display();
void Master();
void PC_Reset();
void PC_Power();
void PSU_State();

/**
* @brief Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {
  // Configure Input Pins
  pinMode(SIMPWR_MAINT_MODE, INPUT_PULLUP);  // Not Yet Implemented
  pinMode(SIMPWR_PFLT_MODE, INPUT_PULLUP);   // Not Yet Implemented
  pinMode(SIMPWR_DISPON, INPUT_PULLUP);      // Latching Switch to power on External Display System
  pinMode(SIMPWR_MASTERON, INPUT_PULLUP);    // Latching Switch to power on Main SimPit Power Supply
  pinMode(SIMPWR_PCRESET, INPUT_PULLUP);     // Momentary Switch to Activate PC Motherboad Reset Switch Function
  pinMode(SIMPWR_PCPWR, INPUT_PULLUP);       // Momentary Switch to Activate PC Motherboad Power Switch Function
  pinMode(PWR_OK, INPUT);                    // Feedback circuit from SimPit ATX Power Supply

  // Configure Output Pins
  pinMode(PS_ON, OUTPUT);     // Switch controlled digital output to turn on power supply
  pinMode(EXT_ACC1, OUTPUT);  // Momentary Output for PC Power Switch function (requires external connection to PC motherboard supplied by end-user)
  pinMode(EXT_ACC2, OUTPUT);  // Momentary Output for PC Reset Switch function (requires external connection to PC motherboard supplied by end-user)
  pinMode(EXT_ACC3, OUTPUT);  // Latching Output for Display On Switch Function (requires external connection supplied by end-user)
  pinMode(EXT_ACC4, OUTPUT);  // Not Yet Implemented

  // Setup Pinchange Interupt for Switch Pins
  PCICR |= 0b00000001;  // Enable Pin Change Interupts
  //PCMSK0 |= 0b00010000;  // Select Pin 8 (PS_ON) as interrupt trigger on Interrupt PCINT4"
  PCMSK0 |= 0b00100000;  // Select Pin 9 (PWR_OK) as interrupt trigger on Interrupt PCINT5"
  //PCMSK0 |= 0b00001000;  // Select Pin 14 (Maint_Mode) as interrupt trigger on Interrupt PCINT3"
  //PCMSK0 |= 0b00000010;  // Select Pin 15 (PFLT_MODE) as interrupt trigger on Interrupt PCINT1"

  // Configure Initial Power Supply State Variables prior to first read
  CurrentSwitchState_SIMPWR_MASTERON = HIGH;
  CurrentPowerSupplyState = LOW;

  // Setup Internal Controls to Default Off State
  digitalWrite(PS_ON, HIGH);  // Default to off - Power Supply Control

  // Setup External Accessories to Default Off State
  digitalWrite(EXT_ACC1, LOW);  // Default to off - External Accessory #1
  digitalWrite(EXT_ACC2, LOW);  // Default to off - External Accessory #2
  digitalWrite(EXT_ACC3, LOW);  // Default to off - External Accessory #3
  digitalWrite(EXT_ACC4, LOW);  // Default to off - External Accessory #4

  //Serial.begin(9600);  //This pipes to the serial monitor
}

/**
* @brief Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {
  // Read the state of the switches
  CurrentSwitchState_SIMPWR_MAINT_MODE = digitalRead(SIMPWR_MAINT_MODE);  // Get current state of Maintanance Mode Switch, Low = On & High = Off
  CurrentSwitchState_SIMPWR_PFLT_MODE = digitalRead(SIMPWR_PFLT_MODE);    // Get current state of Preflight Mode Switch, Low = On & High = Off
  CurrentSwitchState_SIMPWR_DISPON = digitalRead(SIMPWR_DISPON);          // Get current state of Display On Switch, Low = On & High = Off
  CurrentSwitchState_SIMPWR_MASTERON = digitalRead(SIMPWR_MASTERON);      // Get current state of Master Power Switch, Low = On & High = Off
  CurrentSwitchState_SIMPWR_PCRESET = digitalRead(SIMPWR_PCRESET);        // Get current state of PC Reset Switch, Low = On & High = Off
  CurrentSwitchState_SIMPWR_PCPWR = digitalRead(SIMPWR_PCPWR);            // Get current state of PC Power Switch, Low = On & High = Off
  CurrentPowerSupplyState = digitalRead(PWR_OK);                          // Get Current State of Power Supply Status, High = "Power Supply Good!" & Low = "Power Supply Not Ready!"

  delay(20);  // Switch Debounce - 20ms

  Maint_Mode();
  Preflight_Mode();
  Display();
  Master();
  PC_Reset();
  PC_Power();
  PSU_State();
}

/**
* @brief Maintenance Mode Function
*
* This function controls the operation of Maintenance Mode.
* @todo The mechanical switch is functional, however not yet implemented in software function.
*/
void Maint_Mode() {
  if (CurrentSwitchState_SIMPWR_MAINT_MODE != LastSwitchState_SIMPWR_MAINT_MODE) {
    if (CurrentSwitchState_SIMPWR_MAINT_MODE == LOW) {
      // Not Yet Implemented
    } else {
      // Not Yet Implemented
    }
    LastSwitchState_SIMPWR_MAINT_MODE = CurrentSwitchState_SIMPWR_MAINT_MODE;
  }
}

/**
* @brief Preflight Mode Function
*
* This function controls the operation of Preflight Mode.
* @todo The mechanical switch is functional, however not yet implemented in software function.
*/
void Preflight_Mode() {
  if (CurrentSwitchState_SIMPWR_PFLT_MODE != LastSwitchState_SIMPWR_PFLT_MODE) {
    if (CurrentSwitchState_SIMPWR_PFLT_MODE == LOW) {
      // Not Yet Implemented
    } else {
      // Not Yet Implemented
    }
    LastSwitchState_SIMPWR_PFLT_MODE = CurrentSwitchState_SIMPWR_PFLT_MODE;
  }
}

/**
* @brief Delay On Function
*
* This function controls the operation and use of the "Display On" Switch on the Sim Power Panel.
* The mechanical switch is functional and activates/deactivates the External Accessory Port #3 on the rear of the Lowe rInstrument Panel.
*/
void Display() {
  if (CurrentSwitchState_SIMPWR_DISPON != LastSwitchState_SIMPWR_DISPON) {
    if (CurrentSwitchState_SIMPWR_DISPON == LOW) {
      digitalWrite(EXT_ACC3, HIGH);  // Turn on External Acccessory Port (Latching Switch for External display Subsystem)
    } else {
      digitalWrite(EXT_ACC3, LOW);  // Turn off External Acccessory Port (Latching Switch for External display Subsystem)
    }
    LastSwitchState_SIMPWR_DISPON = CurrentSwitchState_SIMPWR_DISPON;
  }
}

/**
* @brief Master Function
*
* This function controls the operation and use of the "Master" Switch on the Sim Power Panel.
* The mechanical switch is functional and activates/deactivates the ATX Power Supply which powers the entire SimPit.
* This is a latching switch maintaining the control state to the ATX powersupply.
*/
void Master() {
  if (CurrentSwitchState_SIMPWR_MASTERON != LastSwitchState_SIMPWR_MASTERON) {
    if (CurrentSwitchState_SIMPWR_MASTERON == LOW) {
      digitalWrite(PS_ON, LOW);  // Turn on Main Power Supply
    } else {
      digitalWrite(PS_ON, HIGH);  // Turn off Main Power Supply
    }
    LastSwitchState_SIMPWR_MASTERON = CurrentSwitchState_SIMPWR_MASTERON;
  }
}

/**
* @brief PC Reset Switch Function
*
* This function controls the operation and use of the "PC Reset" Switch on the Sim Power Panel.
* The mechanical switch is functional and activates/deactivates the External Accessory Port #2 on the rear of the Lower Instrument Panel.
* This switch is a momentary function serving as a remote Reset Button for the conputer controlling the simulator.
*/
void PC_Reset() {
  if (CurrentSwitchState_SIMPWR_PCRESET != LastSwitchState_SIMPWR_PCRESET) {
    if (CurrentSwitchState_SIMPWR_PCRESET == LOW) {
      digitalWrite(EXT_ACC2, HIGH);  // Turn on External Acccessory Port (Momentary pushbutton emulation for PC Reset Switch)
    } else {
      digitalWrite(EXT_ACC2, LOW);  // Turn off External Acccessory Port (Momentary pushbutton emulation for PC Reset Switch)
    }
    LastSwitchState_SIMPWR_PCRESET = CurrentSwitchState_SIMPWR_PCRESET;
  }
}

/**
* @brief PC Power Switch Function
*
* This function controls the operation and use of the "PC Power" Switch on the Sim Power Panel.
* The mechanical switch is functional and activates/deactivates the External Accessory Port #2 on the rear of the Lower Instrument Panel.
* This switch is a momentary function serving as a remote Power button for the computer controlling the simulator.
*/
void PC_Power() {
  if (CurrentSwitchState_SIMPWR_PCPWR != LastSwitchState_SIMPWR_PCPWR) {
    if (CurrentSwitchState_SIMPWR_PCPWR == LOW) {
      digitalWrite(EXT_ACC1, HIGH);  // Turn on External Acccessory Port (Momentary pushbutton emulation for PC Power Switch)
    } else {
      digitalWrite(EXT_ACC1, LOW);  // Turn off External Acccessory Port (Momentary pushbutton emulation for PC Power Switch)
    }
    LastSwitchState_SIMPWR_PCPWR = CurrentSwitchState_SIMPWR_PCPWR;
  }
}

/**
* @brief Power Supply Status Function
*
* This function allows for status feedback from the ATX Power supply.
* @todo Use of this function is not yet implemented.
*/
void PSU_State() {
  if (CurrentPowerSupplyState != LastPowerSupplyState) {
    if (CurrentPowerSupplyState == HIGH) {
      // Not Yet Implemented
    } else {
      // Not Yet Implemented
    }
    LastPowerSupplyState = CurrentPowerSupplyState;
  }
}

/**
* @brief Power Supply "Failed State" Function
*
* This function provides realtime feedback from the ATX Power Supply.  In the event of a failed PSU, this function will trigger on the loss of the PWR_OK signal from the PSU.
* The triggered interupt disables the PSU to assist in protecting the simulator from an unhealthy PSU state.
*/
ISR(PCINT0_vect) {
  if (CurrentPowerSupplyState != LOW) {
    digitalWrite(PS_ON, HIGH);  // Turn off PSU, will not reset until Master Switch is cycled.
  }
}