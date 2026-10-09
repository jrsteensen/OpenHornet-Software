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
 * @date      18.Jul 2026
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
#include "DCS_State_Checker.h"

class Board {

private:

    int updCountdown;                                                 //Loop countdown before invoking FastLED.show() (MODE_MANUAL / MODE_RAINBOW only)
    unsigned long lastShowMs;                                         //millis() at the end of the last FastLED.show() (MODE_NORMAL only)
    static const unsigned long SHOW_INTERVAL_NORMAL_MS = 150;         // Min. gap between FastLED.show() calls in MODE_NORMAL. Interrupts are
                                                                      // off during show(), so DCS-BIOS data can ONLY be received in this gap.
                                                                      // Larger = fewer missed updates, but more LED latency. See below.
    static const unsigned long MAX_SHOW_STARVATION_MS = 250;                                                                  static const int MAX_CHANNELS = 10;                               // Maximum number of channels
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
    uint16_t dcs_brightness_console;                                   // Current brightness level (0-65535), for DCS-BIOS controlled mode
    uint16_t dcs_brightness_instrument;                                // Current brightness level (0-65535), for DCS-BIOS controlled mode
    uint16_t dcs_brightness_flood;                                     // Current brightness level (0-65535), for DCS-BIOS controlled mode
    uint16_t pending_instr, pending_console, pending_flood;           // Parked suspicious values
    bool     pend_instr_v = false;
    bool     pend_console_v = false;
    bool     pend_flood_v = false;
    static const uint16_t DIMMER_JUMP_LIMIT = 8192;                  // ~12.5% of full scale
    int encSwPin;                                                     // Encoder switch pin
    RotaryEncoder* encoder;                                           // Pointer to encoder instance
    int rotary_pos;                                                   // Current rotary encoder position
    static Board* instance;                                           // Static instance pointer to the Board class
    DcsState prevDcsState = DcsState::EXITED;                         // Previous DCS state for transition detection

    uint8_t  nextChannel;                                             // Round-robin pointer for per-channel updates
    uint32_t maxPowerMW;                                              // Power budget for scale calc; 0 = no limit
    static const uint16_t LED_BUDGET_PER_GAP = 350;                   // Max LEDs shown per frame gap (~18 ms)
    static const unsigned long DCS_SILENT_MS = 500;                   // No frame end for this long = DCS not streaming
        
    /**
     * @brief Private constructor to enforce singleton pattern
     * @see This method is called by getInstance() when creating the singleton instance
     */
    Board() {
        updCountdown = 0;                                             // Initialize with 0
        lastShowMs = 0;                                               // Initialize with 0
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
        nextChannel = 0;                                              // Initialize with 0
        maxPowerMW = 0;                                               // Initialize with 0
    }

    /**
     * @brief Computes scaled CRGB targets for instrument backlights and pushes them to all channels.
     *        Uses the cached dcs_brightness_instrument value. No gating - always applies.
     * @see Called by updateInstrumentLights() (after gate) and from mode-entry / DCS-resume paths
     *      that need to force-push the cached value regardless of whether it changed.
     */
    void applyInstrumentTargets() {
        uint8_t scale = map(dcs_brightness_instrument, 0, 65535, 0, 255);
        CRGB instrTarget = NVIS_GREEN_A;       instrTarget.nscale8_video(scale);
        CRGB cgrbTarget  = NVIS_CGRB_GREEN_A;  cgrbTarget.nscale8_video(scale);
        for (int i = 0; i < channelCount; i++) {
            channels[i]->applyInstrLights(instrTarget, cgrbTarget);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Computes scaled CRGB target for console backlights and pushes it to all channels.
     *        Uses the cached dcs_brightness_console value. No gating - always applies.
     */
    void applyConsoleTargets() {
        uint8_t scale = map(dcs_brightness_console, 0, 65535, 0, 255);
        CRGB consoleTarget = NVIS_GREEN_A;     consoleTarget.nscale8_video(scale);
        for (int i = 0; i < channelCount; i++) {
            channels[i]->applyConsoleLights(consoleTarget);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
    }

    /**
     * @brief Computes scaled CRGB target for floodlights and pushes it to all channels.
     *        Uses the cached dcs_brightness_flood value. No gating - always applies.
     */
    void applyFloodTargets() {
        uint8_t scale = map(dcs_brightness_flood, 0, 65535, 0, 255);
        CRGB floodTarget = NVIS_WHITE;         floodTarget.nscale8_video(scale);
        for (int i = 0; i < channelCount; i++) {
            channels[i]->applyFloodlights(floodTarget);
        }
        LedUpdateState::getInstance()->setUpdateFlag(true);
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
     * @brief Registers a channel with the board and with the LedUpdateState singleton
     * @param channel Pointer to the channel to register
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void registerChannel(Channel* channel) {
        LedUpdateState::getInstance()->registerArray(channel->getLeds());
        channels[channelCount++] = channel;
    }

    /**
     * @brief Update the physical LED state
     * @details FastLED.show() takes ~51 ms for ~1700 LEDs and runs with interrupts disabled,
     *          so no DCS-BIOS data can be received while it runs. A too long FastLED update 
     *          will cause a missed DCS-BIOS message, and subsequently flickering lights. 
     * 
     *          On the upside, between two DCS-Bios message frames, there is a silence 
     *          period which we can use to update part of the LEDs. 
     * 
     *          Therefore, we implement a per-channel update system. We check if a channel 
     *          needs an update (is dirty) and if it fits in the current gap
     *          
     *          MODE_MANUAL / MODE_RAINBOW do not call
     *          DcsBios::loop(), so they have nothing to listen for and keep the loop countdown.
     * @see This method is called by loop() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void updateLeds() {
        LedUpdateState* upd = LedUpdateState::getInstance();
        if (!upd->getUpdateFlag()) return;                            // Nothing dirty
    
        if (currentMode != MODE_NORMAL) {                             // MANUAL / RAINBOW: no DCS listening,
            updCountdown = (updCountdown == 0) ? 8 : updCountdown;    // keep legacy batching and global show
            if (--updCountdown != 0) return;
            cli();
            FastLED.show();
            upd->setUpdateFlag(false);
            sei();
            return;
        }
    
        // MODE_NORMAL: show only in the idle gap after a DCS-BIOS frame
        bool dcsSilent = (millis() - lastFrameEndMs) > DCS_SILENT_MS;
        bool starved   = (millis() - lastShowMs) > MAX_SHOW_STARVATION_MS;
        if (!dcsSilent && !starved) {
            if (!frameJustEnded) return;                              // Wait for end-of-frame marker
            if (!dcsStreamIdle()) return;                             // Bytes pending: retry next loop
            if (millis() - lastFrameEndMs > 3) {                      // Gap already stale:
                frameJustEnded = false;                               // wait for the next one
                return;
            }
        }
    
        uint8_t scale = 255;
        if (maxPowerMW) {                                             // Same computation FastLED.show() uses
            scale = calculate_max_brightness_for_power_mW(255, maxPowerMW);
        }
    
        uint16_t budget = LED_BUDGET_PER_GAP;
        for (uint8_t n = 0; n < channelCount; n++) {                  // Visit each channel at most once,
            uint8_t i = nextChannel;                                  // starting where the last gap ended
            nextChannel = (nextChannel + 1) % channelCount;
            if (!(upd->getDirtyMask() & (1 << i))) continue;          // Clean channel
            if (channels[i]->getLedCount() > budget) continue;        // Doesn't fit this gap; bit stays set
            if (!dcsSilent && !starved && !dcsStreamIdle()) break;    // Next frame started early: stop
    
            channels[i]->showNow(scale);
            lastShowMs = millis();                                    // Feed the starvation timer
    
            cli();                                                    // processChar is not reentrant:
            while (UCSR0A & (1 << RXC0)) {                            // no RX interrupt may interleave
                volatile uint8_t d = UDR0; (void)d;                   // Drop stale bytes, clear overrun
            }
            for (uint8_t k = 0; k < 6; k++) {
                DcsBios::parser.processChar(0x55);                    // Force resync wait, now atomic
            }
            sei();
            upd->clearBit(i);
            budget -= channels[i]->getLedCount();
        }
        frameJustEnded = false;                                       // Gap consumed
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
                // Force-restore the cached brightness directly (bypasses central gate).
                // The DCS-BIOS messages below also keep the sim in sync, but we no longer
                // rely on their echo to repaint our LEDs.
                applyInstrumentTargets();
                applyConsoleTargets();
                applyFloodTargets();
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
                    DcsState currentDcsState = getDcsState();
                    if (currentDcsState == DcsState::EXITED && prevDcsState != DcsState::EXITED) {
                        setAllLightsOff();                            // DCS just exited: turn off all lights
                    } else if (prevDcsState == DcsState::EXITED &&
                               currentDcsState != DcsState::EXITED &&
                               currentDcsState != DcsState::PAUSED) {
                        // DCS became active again: force-push the cached brightness
                        // (helpers bypass the central gate, which would otherwise short-circuit)
                        applyInstrumentTargets();
                        applyConsoleTargets();
                        applyFloodTargets();
                    }
                    // PAUSED: do nothing - keep current light state
                    prevDcsState = currentDcsState;
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
     * @brief Fills all channels with a solid color (used by MODE_MANUAL)
     * @param color The color to fill instrument and console backlights with
     * @param brightness Optional brightness value (0-255). If not provided, uses mode2_brightness.
     * @details The CGRB instrument LEDs (e.g. radar altimeter) always render as NVIS_CGRB_GREEN_A
     *          scaled by the same brightness, matching the original behavior.
     * @see This method is called by handleModeChange() and processMode() in Board.h, conditionally in MODE_MANUAL case
     */
    void fillSolid(const CRGB& color, int brightness = -1) {
        uint8_t scale = (uint8_t)((brightness >= 0) ? brightness : this->mode2_brightness);
        CRGB instrTarget   = color;              instrTarget.nscale8_video(scale);
        CRGB cgrbTarget    = NVIS_CGRB_GREEN_A;  cgrbTarget.nscale8_video(scale);
        CRGB consoleTarget = color;              consoleTarget.nscale8_video(scale);
        for (int i = 0; i < channelCount; i++) {
            channels[i]->applyInstrLights(instrTarget, cgrbTarget);
            channels[i]->applyConsoleLights(consoleTarget);
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
        if (newValue == dcs_brightness_instrument) { pend_instr_v = false; return; }
        uint16_t diff = (newValue > dcs_brightness_instrument)
                      ? newValue - dcs_brightness_instrument
                      : dcs_brightness_instrument - newValue;
        if (diff > DIMMER_JUMP_LIMIT) {                            // Suspicious jump:
            uint16_t pdiff = (newValue > pending_instr)
                           ? newValue - pending_instr
                           : pending_instr - newValue;
            if (!pend_instr_v || pdiff > DIMMER_JUMP_LIMIT) {      // Not near the parked value: (re)park
                pending_instr = newValue;
                pend_instr_v = true;
                return;
            }                                                      // Second value near the parked one: genuine
        }
        pend_instr_v = false;
        dcs_brightness_instrument = newValue;
        if (currentMode != MODE_NORMAL) return;
        applyInstrumentTargets();
    }

    /**
     * @brief Updates all channels with new console lighting value
     * @param newValue The new brightness value
     * @see This method is called by onConsolesDimmerChange() in Board.h
     */
    void updateConsoleLights(uint16_t newValue) {
        if (newValue == dcs_brightness_console) { pend_console_v = false; return; }
        uint16_t diff = (newValue > dcs_brightness_console)
                      ? newValue - dcs_brightness_console
                      : dcs_brightness_console - newValue;
        if (diff > DIMMER_JUMP_LIMIT) {                            // Suspicious jump:
            uint16_t pdiff = (newValue > pending_console)
                           ? newValue - pending_console
                           : pending_console - newValue;
            if (!pend_console_v || pdiff > DIMMER_JUMP_LIMIT) {    // Not near the parked value: (re)park
                pending_console = newValue;
                pend_console_v = true;
                return;
            }                                                      // Second value near the parked one: genuine
        }
        pend_console_v = false;
        dcs_brightness_console = newValue;
        if (currentMode != MODE_NORMAL) return;
        applyConsoleTargets();
    }

    /**
     * @brief Updates all channels with new flood lighting value
     * @param newValue The new brightness value
     * @see This method is called by onFloodDimmerChange() in Board.h
     */
    void updateFloodLights(uint16_t newValue) {
        if (newValue == dcs_brightness_flood) { pend_flood_v = false; return; }
        uint16_t diff = (newValue > dcs_brightness_flood)
                      ? newValue - dcs_brightness_flood
                      : dcs_brightness_flood - newValue;
        if (diff > DIMMER_JUMP_LIMIT) {                            // Suspicious jump:
            uint16_t pdiff = (newValue > pending_flood)
                           ? newValue - pending_flood
                           : pending_flood - newValue;
            if (!pend_flood_v || pdiff > DIMMER_JUMP_LIMIT) {      // Not near the parked value: (re)park
                pending_flood = newValue;
                pend_flood_v = true;
                return;
            }                                                      // Second value near the parked one: genuine
        }
        pend_flood_v = false;
        dcs_brightness_flood = newValue;
        if (currentMode != MODE_NORMAL) return;
        applyFloodTargets();
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