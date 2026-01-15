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
 * @date      08.Nov 2025
 * @version   t 0.3.5
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     The board class is responsible for the physical input/output: catch rotary encoder commands, update LEDs.
 * @details   During setup, a singleton board object is created. It manages the physical update of the LEDs centrally.
 *            It is the only place from which the expensive physical update (FastLED.show() function) is called.
 *            Additionally, it provides the logic to catch rotary encoder commands to cycle between three modes:
 *            - Normal mode 1 (DCS-BIOS controlled)
 *            - Manual mode 2(control backlights with rotary encoder)
 *            - Rainbow test mode 3
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
#include "DcsRunningChecker.h"

class Board {

private:

    int updCountdown;                                                 //DCS Bios cycle countdown before invoking FastLED.show()
    static const int MAX_CHANNELS = 10;                               // Maximum number of channels
    static const int MODE_NORMAL = 1;                                 // Normal DCS-BIOS controlled mode
    static const int MODE_MANUAL = 2;                                 // Manual mode - control backlts with rotary encoder
    static const int MODE_RAINBOW = 3;                                // Rainbow test mode
    Channel* channels[MAX_CHANNELS];                                  // Array of channel pointers
    int channelCount;                                                 // Current number of channels
    int thisHue;                                                      // Current hue value for rainbow effect
    int deltaHue;                                                     // Hue change between LEDs for rainbow effect
    int currentMode;                                                  // Current operating mode
    int mode2_brightness;                                             // Current brightness level (0-255), for manual mode 2
    int mode3_brightness;                                             // Brightness level (0-255) for rainbow mode 3
    int dcs_brightness_console;                                        // Current brightness level (0-65535), for DCS-BIOS controlled mode
    int dcs_brightness_instrument;                                     // Current brightness level (0-65535), for DCS-BIOS controlled mode
    int dcs_brightness_flood;                                          // Current brightness level (0-65535), for DCS-BIOS controlled mode
    int encSwPin;                                                     // Encoder switch pin
    RotaryEncoder* encoder;                                           // Pointer to encoder instance
    int rotary_pos;                                                   // Current rotary encoder position
    static Board* instance;                                           // Static instance pointer to the Board class
    bool dcsWasRunning = true;                                        // Flag to track if DCS was running last time checkDcsRunning() was called
    
    /**
     * @brief Private constructor to enforce singleton pattern
     * @see This method is called by getInstance() when creating the singleton instance
     */
    Board() {
        updCountdown = 0;                                             // Initialize with 0
        channelCount = 0;                                             // Initialize with 0 channels
        thisHue = 0;                                                  // Initialize with 0
        deltaHue = 3;                                                 // Initialize with 3
        currentMode = MODE_NORMAL;                                    // Initialize to normal mode 1
        mode2_brightness = 64;                                        // Initialize manual mode 2 brightness to 25%
        mode3_brightness = 64;                                        // Initialize rainbow mode 3 brightness to 25%
        dcs_brightness_console = 0;                                   // Initialize DCS brightness to 0
        dcs_brightness_instrument = 0;                                // Initialize DCS brightness to 0
        dcs_brightness_flood = 0;                                     // Initialize DCS brightness to 0
        rotary_pos = 0;                                               // Initialize with 0
        encoder = nullptr;                                            // Initialize with nullptr
    }


public:

    /**
     * @brief Gets or createsthe singleton instance of the Board class
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    static Board* getInstance() {
        return instance ? instance : (instance = new Board()); }

    /**
     * @brief Sets up the rotary encoder with switch and encoder pins
     * @param encSwPin Pin number for the encoder switch
     * @param encAPin Pin number for encoder A
     * @param encBPin Pin number for encoder B
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void setupRotaryEncoder(int encSwPin, int encAPin, int encBPin) {
        this->encSwPin = encSwPin;
        pinMode(encSwPin, INPUT_PULLUP);                              // Initialize mode change pin
        encoder = new RotaryEncoder(encAPin, encBPin, RotaryEncoder::LatchMode::TWO03);
    }

    /**
     * @brief Registers a channel with the board
     * @param channel Pointer to the channel to register
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void registerChannel(Channel* channel) {
        channels[channelCount++] = channel;
    }

    /**
     * @brief Update the physical LED state
     * @see This method is called by loop() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void updateLeds() {                                               
        if (LedUpdateState::getInstance()->getUpdateFlag()) {
            updCountdown = (updCountdown == 0) ? 32 : updCountdown;   // Countdown logic allows to collect LED updates
            updCountdown--;                                           // from 32 loop() calls into one FastLED.show()
            if (updCountdown == 0) {                                  // Trigger FastLED.show() at end of countdown
                cli();
                FastLED.show();                                       
                LedUpdateState::getInstance()->setUpdateFlag(false);  // Reset update flag
                sei();
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
        static unsigned long lastButtonPressTime = 0;
        const unsigned long BUTTON_WAIT = 1000;                       // Wait time in milliseconds between button presses
        
        bool currentButtonState = digitalRead(encSwPin);              // Read the state of the encoder switch
        unsigned long currentTime = millis();                          // Get current time in milliseconds
        
        if (currentButtonState == LOW && lastButtonState == HIGH) {   // Button has just been pressed
            if (currentTime - lastButtonPressTime < BUTTON_WAIT) {    // Only process if 1 sec passed since last press
                lastButtonState = currentButtonState;
                return currentMode;
            }
            lastButtonPressTime = currentTime;                        // Update last press time
            int previousMode = currentMode;                           // Store previous mode
            currentMode = (currentMode % 3) + 1;                      // Cycle to next mode
            
            if (currentMode == MODE_NORMAL) {
                setAllLightsOff();
                sendDcsBiosMessage("CONSOLES_DIMMER", String(dcs_brightness_console).c_str());           // Send DCS-BIOS message to reset console dimmer
                sendDcsBiosMessage("INST_PNL_DIMMER", String(dcs_brightness_instrument).c_str());        // Send DCS-BIOS message to reset instrument lighting
                sendDcsBiosMessage("FLOOD_DIMMER", String(dcs_brightness_flood).c_str());                  // Send DCS-BIOS message to reset floodlights dimmer
            }
            if (currentMode == MODE_MANUAL) {
                mode2_brightness = 64;                                // Reset to 25% brightness
                fillSolid(NVIS_GREEN_A);                              // Apply the brightness immediately
            }
            if (currentMode == MODE_RAINBOW) {
                mode3_brightness = 64;                                // Reset to 25% brightness
                encoder->tick();                                      // Update encoder state
                rotary_pos = encoder->getPosition();                  // Sync encoder position to avoid false change detection
                setAllLightsOff();                                    // Clear any previous state
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
            case MODE_NORMAL:                                         // MODE 1: LEDs controlled by DCS BIOS
                {
                    bool dcsIsRunning = checkDcsRunning();
                    if (!dcsIsRunning && dcsWasRunning) {
                        for (int i = 0; i < channelCount; i++) {
                            channels[i]->updateInstrLights(0);
                            channels[i]->updateConsoleLights(0);
                            channels[i]->updateFloodLights(0);
                        }
                    } else if (dcsIsRunning && !dcsWasRunning) {
                        for (int i = 0; i < channelCount; i++) {
                            channels[i]->updateInstrLights(dcs_brightness_instrument);
                            channels[i]->updateConsoleLights(dcs_brightness_console);
                            channels[i]->updateFloodLights(dcs_brightness_flood);
                        }
                        LedUpdateState::getInstance()->setUpdateFlag(true);
                    }
                    dcsWasRunning = dcsIsRunning;
                    DcsBios::loop();
                }
                break;
            case MODE_MANUAL:                                         // MODE 2: LEDs controlled manually through BKLT switch
                encoder->tick();
                newPos = encoder->getPosition();
                if (newPos != rotary_pos) {
                    RotaryEncoder::Direction direction = encoder->getDirection();
                    if (direction == RotaryEncoder::Direction::CLOCKWISE) {
                        mode2_brightness = (mode2_brightness < 224) ? mode2_brightness + 32 : 255;  // Add 32 or cap at 255
                    } else {
                        mode2_brightness = (mode2_brightness > 32) ? mode2_brightness - 32 : 0;  // Subtract 32 or cap at 0
                    }
                    rotary_pos = newPos;
                    fillSolid(NVIS_GREEN_A);                          
                }
                break;   
            case MODE_RAINBOW:                                        // MODE 3: Rainbow test mode
                encoder->tick();
                newPos = encoder->getPosition();
                if (newPos != rotary_pos) {
                    RotaryEncoder::Direction direction = encoder->getDirection();
                    if (direction == RotaryEncoder::Direction::CLOCKWISE) {
                        mode3_brightness = (mode3_brightness < 224) ? mode3_brightness + 32 : 255;  // Add 32 or cap at 255
                    } else {
                        mode3_brightness = (mode3_brightness > 32) ? mode3_brightness - 32 : 0;  // Subtract 32 or cap at 0
                    }
                    rotary_pos = newPos;
                }
                for (int i = 0; i < channelCount; i++) {
                    fill_rainbow(channels[i]->getLeds(), channels[i]->getLedCount(), thisHue, deltaHue);
                    // Scale down brightness to reduce maximum brightness
                    nscale8_video(channels[i]->getLeds(), channels[i]->getLedCount(), mode3_brightness);
                }
                thisHue++;  // Increment the hue for the next frame
                LedUpdateState::getInstance()->setUpdateFlag(true);
                break;
        }
    }


    /**
     * @brief Fills all channels with a solid color
     * @param color The color to fill with
     * @param brightness Optional brightness value (0-255). If not provided, uses current brightness
     * @see This method is called by handleModeChange() and processMode() in Board.h, conditionally in MODE_MANUAL case
     */
    void fillSolid(const CRGB& color, int brightness = -1) {          // Fill all channels with a solid color
        int targetBrightness = (brightness >= 0) ? brightness : this->mode2_brightness;
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateInstrLights(map(targetBrightness, 0, 255, 0, 65535), color);
            channels[i]->updateConsoleLights(map(targetBrightness, 0, 255, 0, 65535), color);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Turns off all lights in all channels and resets brightness state
     * @see This method is called by handleModeChange() in Board.h
     */
    void setAllLightsOff() {                                         // Turn off all lights and reset brightness state
        for (int i = 0; i < channelCount; i++) {
            channels[i]->setAllLightsOff();
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Updates all channels with new instrument lighting value
     * @param newValue The new brightness value
     * @see This method is conditionally called by onInstrIntLtChange() in Board.h
     */
    void updateInstrumentLights(uint16_t newValue) {
        dcs_brightness_instrument = newValue;                         // In any mode, store the DCS-BIOS brightness value
        if (currentMode != MODE_NORMAL) return;                       // But only in normal mode, actually send update to channels
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateInstrLights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
        
    }

    /**
     * @brief Updates all channels with new console lighting value
     * @param newValue The new brightness value
     * @see This method is called by onConsolesDimmerChange() in Board.h
     */
    void updateConsoleLights(uint16_t newValue) {
        dcs_brightness_console = newValue;                            // In any mode, store the DCS-BIOS brightness value
        if (currentMode != MODE_NORMAL) return;                       // But only in normal mode, actually send update to channels
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateConsoleLights(newValue);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
        
    }

    /**
     * @brief Updates all channels with new flood lighting value
     * @param newValue The new brightness value
     * @see This method is called by onFloodDimmerChange() in Board.h
     */
    void updateFloodLights(uint16_t newValue) {
        dcs_brightness_flood = newValue;                              // In any mode, store the DCS-BIOS brightness value
        if (currentMode != MODE_NORMAL) return;                       // But only in normal mode, actually send update to channels
        for (int i = 0; i < channelCount; i++) {
            channels[i]->updateFloodLights(newValue);
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

    /**
     * @brief Callback for flood lighting changes from DCS-BIOS
     * @param newValue The new brightness value from DCS-BIOS
     * @see This method is called by DCS-BIOS when flood lighting changes
     */
    static void onFloodDimmerChange(unsigned int newValue) {
        if (instance) instance->updateFloodLights(newValue);
    }
    DcsBios::IntegerBuffer floodDimmerBuffer{FA_18C_hornet_FLOOD_DIMMER, onFloodDimmerChange};

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