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
 * @details   During setup, a single board object is created. It manages the physicalupdate of the LEDs centrally.
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

private:

    /**
     * @brief Private constructor to enforce singleton pattern
     * @see This method is called by getInstance() when creating the singleton instance
     */
    Board() {
        updCountdown = 0;
        channelCount = 0;
        thisHue = 0;                                                  
        deltaHue = 3;                                                 
        currentMode = MODE_NORMAL;  // Initialize to normal mode
        brightness = 128;  // Initialize manual mode brightness to 50%
        rotary_pos = 0;   // Initialize rotary encoder position
        encoder = nullptr;
    }

    int updCountdown;                                                 //DCS Bios cycle countdown before invoking FastLED.show()
    static const int MAX_CHANNELS = 10;                               // Maximum number of channels
    Channel* channels[MAX_CHANNELS];                                  // Array of channel pointers
    int channelCount;                                                 // Current number of channels
    uint8_t thisHue;                                                  // Current hue value for rainbow effect
    uint8_t deltaHue;                                                 // Hue change between LEDs for rainbow effect
    int currentMode;                                                  // Current operating mode
    uint8_t brightness;                                               // Current brightness level (0-255)

    uint8_t encSwPin;                                                  // Encoder switch pin
    RotaryEncoder* encoder;                                            // Pointer to encoder instance
    int rotary_pos;                                                    // Current rotary encoder position

    // Static instance pointer
    static Board* instance;


public:
    static const int MODE_NORMAL = 1;                                 // Normal DCS-BIOS controlled mode
    static const int MODE_MANUAL = 2;                                 // Manual mode - control backlts with rotary encoder
    static const int MODE_RAINBOW = 3;                                // Rainbow test mode

    /**
     * @brief Gets the singleton instance of the Board class
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    static Board* getInstance() {
        return instance ? instance : (instance = new Board()); }

    /**
     * @brief Initializes the board with encoder pins
     * @param encSwPin Pin number for the encoder switch
     * @param encAPin Pin number for encoder A
     * @param encBPin Pin number for encoder B
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void initializeBoard(uint8_t encSwPin, uint8_t encAPin, uint8_t encBPin) {
        this->encSwPin = encSwPin;
        pinMode(encSwPin, INPUT_PULLUP);                              // Initialize mode change pin
        encoder = new RotaryEncoder(encAPin, encBPin, RotaryEncoder::LatchMode::TWO03);
    }

    /**
     * @brief Initializes and registers a channel with the board
     * @param channel Pointer to the channel to register
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void initAndRegisterChannel(Channel* channel) {                   // Initialize and register a channel in one step
        channel->initialize();
        channels[channelCount++] = channel;
    }

    /**
     * @brief Updates the physical LED state if needed
     * @see This method is called by loop() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void updateLeds() {                                               // Triggers physical LED update (FastLED.show())
        if (LedUpdateState::getInstance()->getUpdateFlag()) {
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    // Countdown logic allows to collect data
            updCountdown--;                                           // from multiple DCS Bios cycles
            if (updCountdown == 0) {                                  // Trigger FastLED.show() at end of countdown
                FastLED.show();                                       
                LedUpdateState::getInstance()->setUpdateFlag(false);  
            }
        }
    }


    /**
     * @brief Handles mode change button press and returns current mode
     * @return The current mode after handling the button press
     * @see This method is called by loop() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    int handleModeChange() {
        static bool lastButtonState = HIGH;
        bool currentButtonState = digitalRead(encSwPin);
        if (currentButtonState == LOW && lastButtonState == HIGH) {   // Button has just been pressed
            int previousMode = currentMode;                           // Store previous mode
            currentMode = (currentMode % 3) + 1;                      // Cycle to next mode
            
            if (currentMode == MODE_MANUAL) {
                brightness = 128;  // Reset to 50% brightness
                fillSolid(NVIS_GREEN_A);                              // Apply the brightness immediately
                LedUpdateState::getInstance()->setUpdateFlag(true);   // Activate "update is needed" flag
            }
            if (currentMode == MODE_NORMAL) {
                // Reset brightness to a known good state for DCS-BIOS
                brightness = 128;  // Reset to 50% brightness
                fillBlack();
                LedUpdateState::getInstance()->setUpdateFlag(true);   // Activate "update is needed" flag
            }
            if (currentMode == MODE_RAINBOW) {
                fillBlack();  // Clear any previous state
                LedUpdateState::getInstance()->setUpdateFlag(true);   // Activate "update is needed" flag
            }

            lastButtonState = currentButtonState;
            delay(10);                                                // Small delay to debounce switch
        } else {
            lastButtonState = currentButtonState;
        }
        return currentMode;
    }

    /**
     * @brief Processes the current mode
     * @see This method is called by loop() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void processMode() {
        int newPos = 0;  
        switch(currentMode) {
            case MODE_NORMAL:                                         // LEDs controlled by DCS BIOS
                DcsBios::loop();
                break;
            case MODE_MANUAL:                                         // LEDs controlled manually through BKLT switch
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
                    fillSolid(NVIS_GREEN_A);                          
                }
                break;   
            case MODE_RAINBOW:                                     //Rainbow test mode
                fillRainbow();
                break;
        }
    }


    /**
     * @brief Fills all channels with a solid color
     * @param color The color to fill with
     * @see This method is called by handleModeChange() and processMode() in Board.h, conditionally in MODE_MANUAL case
     * @todo Remove parameter, check if deprecated, merge with incrBrightness() and decrBrightness()
     */
    void fillSolid(const CRGB& color) {                               // Fill backlights of all channels with a solid color
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateBacklights(map(brightness, 0, 255, 0, 65535));
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Fills all channels with black
     * @see This method is called by handleModeChange() in Board.h
     */
    void fillBlack() {                                                // Fill all channels with black (works in any mode)
        for (int i = 0; i < channelCount; i++) {
            fill_solid(channels[i]->getLeds(), channels[i]->getLedCount(), NVIS_BLACK);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Fills all channels with a rainbow pattern
     * @see This method is called by processMode() in Board.h
     */
    void fillRainbow() {                                              // Fill all channels with a rainbow pattern
        for (int i = 0; i < channelCount; i++) {
            fill_rainbow(channels[i]->getLeds(), channels[i]->getLedCount(), thisHue, deltaHue);
        }
        thisHue++;  // Increment the hue for the next frame
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }


    /**
     * @brief Increases brightness by 32 levels
     * @see This method is called by processMode() in Board.h
     */
    void incrBrightness() {
        if (currentMode != MODE_MANUAL) return;  // Only allow in manual mode
        if (brightness < 224) {  // Leave room for 32 more steps
            brightness += 32;
        } else {
            brightness = 255;    // Cap at maximum
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Decreases brightness by 32 levels
     * @see This method is conditionallycalled by processMode() in Board.h
     */
    void decrBrightness() {
        if (currentMode != MODE_MANUAL) return;  // Only allow in manual mode
        if (brightness > 32) {   // Leave room for 32 more steps
            brightness -= 32;
        } else {
            brightness = 0;      // Cap at minimum
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Updates all channels with new instrument lighting value
     * @param newValue The new brightness value
     * @see This method is conditionallycalled by onInstrIntLtChange() in Board.h
     */
    void updateInstrumentLights(uint16_t newValue) {
        if (currentMode != MODE_NORMAL) return;  // Only update in normal mode
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateBacklights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Updates all channels with new console lighting value
     * @param newValue The new brightness value
     * @see This method is called by onConsolesDimmerChange() in Board.h
     */
    void updateConsoleLights(uint16_t newValue) {
        if (currentMode != MODE_NORMAL) return;  // Only update in normal mode
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateConsoleLights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }


    /**
     * @brief Callback for instrument lighting changes from DCS-BIOS
     * @param newValue The new brightness value from DCS-BIOS
     * @see This method is called by DCS-BIOS when instrument lighting changes
     */
    static void onInstrIntLtChange(unsigned int newValue) {
        if (instance) instance->updateInstrumentLights(newValue);
    }
    DcsBios::IntegerBuffer instrIntLtBuffer{FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange};

    /**
     * @brief Callback for console dimmer changes from DCS-BIOS
     * @param newValue The new brightness value from DCS-BIOS
     * @see This method is called by DCS-BIOS when console dimmer changes
     */
    static void onConsolesDimmerChange(unsigned int newValue) {
        if (instance) instance->updateConsoleLights(newValue);
    }
    DcsBios::IntegerBuffer consolesDimmerBuffer{FA_18C_hornet_CONSOLES_DIMMER, onConsolesDimmerChange};

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