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
 * @date      16.05.2025
 * @version   t 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Defines the Board class for managing LED strips.
 * @details   During setup, a single board object is created. It manages the update of the LEDs centrally.
 *            It is the only place from which the expensive FastLED.show() function is called.
 *********************************************************************************************************************/



#ifndef __BOARD_H
#define __BOARD_H

#include "FastLED.h"
#include "Channel.h"
#include "DcsBios.h"
#include "Colors.h"

class Board {
public:
    // Mode definitions
    static const int MODE_NORMAL = 1;   // Normal DCS-BIOS controlled mode
    static const int MODE_MANUAL = 2;   // Manual mode with all backlights at 100% green
    static const int MODE_RAINBOW = 3;  // Rainbow test mode

    static Board* getInstance() {
        if (!instance) {
            instance = new Board();
        }
        return instance;
    }

    // Method to check if LEDs need updating
    bool isLedsNeedUpdate() const {
        return ledsNeedUpdate;
    }

    // Method to indicate that LEDs need updating
    void setLedsNeedUpdate() {
        ledsNeedUpdate = true;
    }

    // Method to update LEDs if needed
    void updateLeds() {
        if (ledsNeedUpdate) {
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    // If countdown is 0, this is the first cycle, so set to 8
            updCountdown--;
            if (updCountdown == 0) {                                  // Trigger FastLED.show() at end of countdown
                FastLED.show();                                       // This countdown logic allows to capture multiple
                ledsNeedUpdate = false;                               // DCS-BIOS loop cycles into one FastLED.show() call
            }
        }
    }

    // Add a channel to the board
    void addChannel(Channel* channel) {
        channels[channelCount++] = channel;
    }

    // Initialize and register a channel in one step
    void initAndRegisterChannel(Channel* channel) {
        channel->initialize();
        addChannel(channel);
    }

    // Fill all channels with a solid color
    void fillSolid(const CRGB& color) {
        for (int i = 0; i < channelCount; i++) {
            fill_solid(channels[i]->getLeds(), channels[i]->getLedCount(), color);
        }
        setLedsNeedUpdate();
    }

    // Fill all channels with a rainbow pattern
    void fillRainbow() {
        for (int i = 0; i < channelCount; i++) {
            fill_rainbow(channels[i]->getLeds(), channels[i]->getLedCount(), thisHue, deltaHue);
        }
        thisHue++;  // Increment the hue for the next frame
        setLedsNeedUpdate();
    }

    // Handle mode change button press and return current mode
    int handleModeChange(uint8_t buttonPin) {
        static bool lastButtonState = HIGH;
        bool currentButtonState = digitalRead(buttonPin);
        if (currentButtonState == LOW && lastButtonState == HIGH) {       // Button has just been pressed
            fillSolid(NVIS_BLACK);                                       // Reset the LEDs
            currentMode = (currentMode % 3) + 1;                          // Cycle to next mode
            lastButtonState = currentButtonState;
            delay(10);                                                    // Small delay to debounce
        } else {
            lastButtonState = currentButtonState;
        }
        return currentMode;
    }

    // Get current mode
    int getCurrentMode() const {
        return currentMode;
    }

private:
    // Private constructor to enforce singleton pattern
    Board() {
        ledsNeedUpdate = false;
        updCountdown = 0;
        channelCount = 0;
        thisHue = 0;      // Starting hue
        deltaHue = 3;     // Hue change between LEDs
        currentMode = MODE_NORMAL;  // Initialize to normal mode
    }

    // Private methods
    static void onAcftNameChange(char* newValue) {
        if (!strcmp(newValue, "FA-18C_hornet")) {
            //cl_F18C.MakeCurrent();
        }
    }

    // LED update state
    bool ledsNeedUpdate;
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

    // Static instance pointer
    static Board* instance;
};

// Initialize static instance pointer
Board* Board::instance = nullptr;

#endif 