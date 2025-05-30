/**********************************************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *  
 * @file      Board.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     The board class is responsible for the physical interaction: catch rotary encoder commands, update LEDs.
 * @details   During setup, a single board object is created. It manages the update of the LEDs centrally.
 *            It is the only place from which the expensive physical update (FastLED.show() function) is called.
 *            Additionally, it provides the logic to catch rotary encoder commands to cycle between three modes:
 *            - Normal mode (DCS-BIOS controlled)
 *            - Manual mode (control backlights with rotary encoder)
 *            - Rainbow test mode
 *********************************************************************************************************************/



#ifndef __BOARD_H
#define __BOARD_H

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
  #define DCSBIOS_IRQ_SERIAL
#else
  #define DCSBIOS_DEFAULT_SERIAL
#endif

#include "FastLED.h"
#include "Channel.h"
#include "DcsBios.h"
#include "Colors.h"
#include "LedUpdateState.h"
#include "RotaryEncoder.h"

class Board {
public:
    static const int MODE_NORMAL = 1;                                 // Normal DCS-BIOS controlled mode
    static const int MODE_MANUAL = 2;                                 // Manual mode - control backlts with rotary encoder
    static const int MODE_RAINBOW = 3;                                // Rainbow test mode

    static Board* getInstance() {                                     // Singleton pattern
        return instance ? instance : (instance = new Board()); }

    void initAndRegisterChannel(Channel* channel) {                   // Initialize and register a channel in one step
        channel->initialize();
        channels[channelCount++] = channel;
    }

    void initialize(uint8_t encSwPin, uint8_t encAPin, uint8_t encBPin) {
        this->encSwPin = encSwPin;
        pinMode(encSwPin, INPUT_PULLUP);                             // Initialize mode change pin
        encoder = new RotaryEncoder(encAPin, encBPin, RotaryEncoder::LatchMode::TWO03);
    }

    void updateLeds() {                                                // Method to update LEDs if needed
        if (LedUpdateState::getInstance()->getUpdateFlag()) {
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    // If countdown is 0, this is the first cycle, so set to 8
            updCountdown--;
            if (updCountdown == 0) {                                  // Trigger FastLED.show() at end of countdown
                FastLED.show();                                       // This countdown logic allows to capture multiple
                LedUpdateState::getInstance()->setUpdateFlag(false);  // DCS-BIOS loop cycles into one FastLED.show() call
            }
        }
    }

    void fillSolid(const CRGB& color) {                               // Fill backlights of all channels with a solid color
        if (currentMode != MODE_MANUAL) return;                       // Only applicable in manual mode
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateBacklights(map(brightness, 0, 255, 0, 65535));
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    void fillBlack() {                                                // Fill all channels with black (works in any mode)
        for (int i = 0; i < channelCount; i++) {
            fill_solid(channels[i]->getLeds(), channels[i]->getLedCount(), NVIS_BLACK);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    void fillRainbow() {                                              // Fill all channels with a rainbow pattern
        for (int i = 0; i < channelCount; i++) {
            fill_rainbow(channels[i]->getLeds(), channels[i]->getLedCount(), thisHue, deltaHue);
        }
        thisHue++;  // Increment the hue for the next frame
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    // Handle mode change button press and return current mode
    int handleModeChange() {
        static bool lastButtonState = HIGH;
        bool currentButtonState = digitalRead(encSwPin);
        if (currentButtonState == LOW && lastButtonState == HIGH) {   // Button has just been pressed
            int previousMode = currentMode;                           // Store previous mode
            currentMode = (currentMode % 3) + 1;                      // Cycle to next mode
            
            if (currentMode == MODE_MANUAL) {
                brightness = 128;  // Reset to 50% brightness
                fillSolid(NVIS_GREEN_A);                              // Apply the brightness immediately
                LedUpdateState::getInstance()->setUpdateFlag(true);   // Register an LED update is needed
            }
            if (currentMode == MODE_NORMAL) {
                // Reset brightness to a known good state for DCS-BIOS
                brightness = 128;  // Reset to 50% brightness
                fillBlack();
                LedUpdateState::getInstance()->setUpdateFlag(true);   // Register an LED update is needed
            }
            if (currentMode == MODE_RAINBOW) {
                fillBlack();  // Clear any previous state
                LedUpdateState::getInstance()->setUpdateFlag(true);
            }

            lastButtonState = currentButtonState;
            delay(10);                                                // Small delay to debounce
        } else {
            lastButtonState = currentButtonState;
        }
        return currentMode;
    }

    // Process the current mode
    void processMode() {
        int newPos = 0;  
        switch(currentMode) {
            case MODE_NORMAL:                                      // LEDs controlled by DCS BIOS
                DcsBios::loop();
                break;
            case MODE_MANUAL:                                      // LEDs controlled manually through BKLT switch
                encoder->tick();
                newPos = encoder->getPosition();
                if (newPos != rotary_pos) {
                    RotaryEncoder::Direction direction = encoder->getDirection();
                    if (direction == RotaryEncoder::Direction::CLOCKWISE) {
                        incrBrightness();
                    } else {
                        decrBrightness();
                    }
                    rotary_pos = newPos;
                    fillSolid(NVIS_GREEN_A);                       // Only call fillSolid when brightness changes
                }
                break;   
            case MODE_RAINBOW:                                     //Rainbow test mode
                fillRainbow();
                break;
        }
    }

    // Get current mode
    int getCurrentMode() const {
        return currentMode;
    }

    // Increase brightness by 32 levels (0-255)
    void incrBrightness() {
        if (currentMode != MODE_MANUAL) return;  // Only allow in manual mode
        if (brightness < 224) {  // Leave room for 32 more steps
            brightness += 32;
        } else {
            brightness = 255;    // Cap at maximum
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    // Decrease brightness by 32 levels (0-255)
    void decrBrightness() {
        if (currentMode != MODE_MANUAL) return;  // Only allow in manual mode
        if (brightness > 32) {   // Leave room for 32 more steps
            brightness -= 32;
        } else {
            brightness = 0;      // Cap at minimum
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    // Update all channels with new instrument lighting value
    void updateInstrumentLights(uint16_t newValue) {
        if (currentMode != MODE_NORMAL) return;  // Only update in normal mode
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateBacklights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    // Update all channels with new console lighting value
    void updateConsoleLights(uint16_t newValue) {
        if (currentMode != MODE_NORMAL) return;  // Only update in normal mode
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateConsoleLights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    // Static callback functions for DCS-BIOS
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->updateInstrumentLights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    static void onConsolesDimmerChange(unsigned int newValue) {
        if (instance) instance->updateConsoleLights(newValue);
    }
    DcsBios::IntegerBuffer consolesDimmerBuffer{FA_18C_hornet_CONSOLES_DIMMER, onConsolesDimmerChange};


private:
    // Private constructor to enforce singleton pattern
    Board() {
        updCountdown = 0;
        channelCount = 0;
        thisHue = 0;      // Starting hue
        deltaHue = 3;     // Hue change between LEDs
        currentMode = MODE_NORMAL;  // Initialize to normal mode
        brightness = 128;  // Initialize manual mode brightness to 50%
        rotary_pos = 0;   // Initialize rotary encoder position
        encoder = nullptr;
    }

    // LED update state
    int updCountdown;

    // Channel management
    static const int MAX_CHANNELS = 10;  // Maximum number of channels
    Channel* channels[MAX_CHANNELS];     // Array of channel pointers
    int channelCount;                    // Current number of channels

    // Rainbow effect variables
    uint8_t thisHue;     // Current hue value
    uint8_t deltaHue;    // Hue change between LEDs

    // Mode management
    int currentMode;     // Current operating mode

    // Brightness control
    uint8_t brightness;  // Current brightness level (0-255)

    // Encoder management
    uint8_t encSwPin;    // Encoder switch pin
    RotaryEncoder* encoder;  // Pointer to encoder instance
    int rotary_pos;      // Current rotary encoder position

    // Static instance pointer
    static Board* instance;
};

// Initialize static instance pointer
Board* Board::instance = nullptr;

#endif 





    // TODO: the following lines contain code snippets for the future PREFLT mode that is not yet implemented

    //    static void onAcftNameChange(char* newValue) {
    //    if (!strcmp(newValue, "FA-18C_hornet")) {
    //        //cl_F18C.MakeCurrent();
    //    }
    //}

    // DCS-BIOS callbacks and buffers
    //static void onAcftNameChange(char* newValue) {
    //    if (!strcmp(newValue, "FA-18C_hornet")) {
    //        //cl_F18C.MakeCurrent();
    //    }
    //}
    //DcsBios::StringBuffer<16> AcftNameBuffer{0x0000, onAcftNameChange};

    /*
            // This is the recommended approach and the ideal if we can work out all the kinks:
        // If the mission time changes backwards, we have entered a new aircraft; Resync everything

        unsigned long uLastModelTimeS = 0xFFFFFFFF; // Start big, to ensure the first step triggers a resync

        void onModTimeChange(char* newValue) {
        unsigned long currentModelTimeS = atol(newValue);

        if( currentModelTimeS < uLastModelTimeS )
        {
            if( currentModelTimeS > 20 )// Delay to give time for DCS to finish loading and become stable and responsive
            {
            DcsBios::resetAllStates();
            uLastModelTimeS = currentModelTimeS;
            }
        }
        else
        {
            uLastModelTimeS = currentModelTimeS;
        }
        }
        DcsBios::StringBuffer<5> modTimeBuffer(0x043e, onModTimeChange);

        // Also we can check on weight on wheels change:
        void onExtWowLeftChange(unsigned int newValue) {
            // your code here 
        }
        DcsBios::IntegerBuffer extWowLeftBuffer(FA_18C_hornet_EXT_WOW_LEFT, onExtWowLeftChange);
        
    */