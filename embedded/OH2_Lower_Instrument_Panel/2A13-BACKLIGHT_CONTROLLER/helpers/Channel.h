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
 * @file      Channel.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Channels are the logical representation of LED strips.
 * @details   Each channel maintains an array of LEDs, and a pointer to the first panel on the channel.
 *            The channel class provides methods to add panels to the channel and to update the backlights of all 
 *            panels in the channel. The channel class does not hold an array of panels. Instead, panels are organized 
 *            in a linked list within each channel. This conserves stack memory. This approach avoids allocating 
 *            fixed-size arrays for panels in each channel, which would exhaust the limited stack space on the 
 *            Arduino Mega 2560 (I tested it). Instead, this class provides a pointer to its first panel.
 *            Thus,the channels are still able to iterate through all of their panels.
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
    Panel* firstPanel;     // Pointer to first panel in the channel
    uint8_t panelCount;    // Number of panels in the channel
    
public:
    Channel(uint8_t p, const char* pcb, uint16_t count) {
        pin = p;
        pcbName = pcb;
        ledCount = count;
        leds = NULL;
        currentIndex = 0;  // Initialize currentIndex to 0
        firstPanel = nullptr;  // Initialize first panel pointer
        panelCount = 0;    // Initialize panel count
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
        
        // Add panel to linked list
        if (firstPanel == nullptr) {
            firstPanel = panel;  // First panel in the channel
        } else {
            // Find the last panel
            Panel* current = firstPanel;
            while (current->nextPanel != nullptr) {
                current = current->nextPanel;
            }
            current->nextPanel = panel;  // Add new panel at the end
        }
        
        panelCount++;
        currentIndex += panel->getLedCount();
    }

    // Getters
    uint8_t getPin() const { return pin; }
    const char* getPcbName() const { return pcbName; }
    uint16_t getLedCount() const { return ledCount; }
    CRGB* getLeds() const { return leds; }
    Panel* getFirstPanel() const { return firstPanel; }
    uint8_t getPanelCount() const { return panelCount; }

    // Update backlights for all panels in this channel (only used in manual mode)
    void updateBacklights(uint16_t brightness) {
        Panel* current = firstPanel;
        while (current != nullptr) {
            current->setBacklights(brightness);
            current = current->nextPanel;
        }
    }

};

#endif 