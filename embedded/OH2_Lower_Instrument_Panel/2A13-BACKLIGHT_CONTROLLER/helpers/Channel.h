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
#include "Colors.h"


class Channel {
public:
    Channel(uint8_t p, const char* pcb, uint16_t count) {
        pin = p;
        pcbName = pcb;
        ledCount = count;
        leds = NULL;
    }

    void initialize() {
        leds = new CRGB[ledCount];
        if (leds == NULL) {
            // Handle allocation failure
            return;
        }
        
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
        
        fill_solid(leds, ledCount, COLOR_BLACK);
    }

    // Getters
    uint8_t getPin() const { return pin; }
    const char* getPcbName() const { return pcbName; }
    uint16_t getLedCount() const { return ledCount; }
    CRGB* getLeds() const { return leds; }

private:
    uint8_t pin;           // Hardware pin number
    char* pcbName;         // Name on PCB ("Channel 1", "Channel 2")
    uint16_t ledCount;     // Number of LEDs
    CRGB* leds;            // Pointer to LED array
};

#endif 