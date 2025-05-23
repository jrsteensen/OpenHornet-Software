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
 * @file Channel.h
 * @author Ulukaii
 * @date 09.05.2025
 * @version u 0.3.1
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief Defines the Channel class for managing LED strips.
 * @details Each channel represents a physical LED strip connected to a specific pin.
 *          The class manages the LED array and provides methods for initialization and control.
 *********************************************************************************************************************/

#ifndef __CHANNEL_H
#define __CHANNEL_H

#include "FastLED.h"
#include "Panel.h"
#include "Colors.h"


class Channel {

private:
    uint8_t pin;           // Hardware pin number
    const char* pcbName;   // Changed from char* to const char*
    uint16_t ledCount;     // Number of LEDs
    CRGB* leds;            // Pointer to LED array
    uint16_t currentIndex; // Index of the next available LED
    
public:
    Channel(uint8_t p, const char* pcb, uint16_t count) {
        pin = p;
        pcbName = pcb;
        ledCount = count;
        leds = NULL;
        currentIndex = 0;  // Initialize currentIndex to 0
    }

    void initialize() {
        leds = new CRGB[ledCount];

        // Use a switch statement to overcome strange behaviour of FastLED to have a pin number at compile time
        switch(pin) {
            case 4:  FastLED.addLeds<WS2812B, 4, GRB>(leds, ledCount); break;
            case 5:  FastLED.addLeds<WS2812B, 5, GRB>(leds, ledCount); break;
            case 6:  FastLED.addLeds<WS2812B, 6, GRB>(leds, ledCount); break;
            case 7:  FastLED.addLeds<WS2812B, 7, GRB>(leds, ledCount); break;
            case 8:  FastLED.addLeds<WS2812B, 8, GRB>(leds, ledCount); break;
            case 9:  FastLED.addLeds<WS2812B, 9, GRB>(leds, ledCount); break;
            case 10: FastLED.addLeds<WS2812B, 10, GRB>(leds, ledCount); break;
            case 11: FastLED.addLeds<WS2812B, 11, GRB>(leds, ledCount); break;
            case 12: FastLED.addLeds<WS2812B, 12, GRB>(leds, ledCount); break;
            case 13: FastLED.addLeds<WS2812B, 13, GRB>(leds, ledCount); break;
            default: break; // Handle invalid pin
        }
        
        fill_solid(leds, ledCount, NVIS_BLACK);
    }

    //Add panel to channel
    template<typename PanelType>
    void addPanel() {
        PanelType* panel = PanelType::getInstance(currentIndex, leds);
        
        // Safety check: Ensure we don't exceed the channel's LED capacity
        if (currentIndex + panel->getLedCount() > ledCount) {
            // Halt execution and indicate error with LED pattern
            while(1) {
                digitalWrite(LED_BUILTIN, HIGH);
                delay(100);
                digitalWrite(LED_BUILTIN, LOW);
                delay(100);
            }
        }
        
        currentIndex += panel->getLedCount();
    }


    // Getters
    uint8_t getPin() const { return pin; }
    const char* getPcbName() const { return pcbName; }
    uint16_t getLedCount() const { return ledCount; }
    CRGB* getLeds() const { return leds; }

};

#endif 