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
    /**
     * @brief Constructor for the Channel class
     * @param p Pin number for the LED strip
     * @param pcb Name of the PCB this channel is connected to
     * @param ledArray Pointer to the LED array
     * @param count Number of LEDs in the strip
     */
    Channel(uint8_t p, const char* pcb, CRGB* ledArray, uint16_t count) {
        pin = p;
        pcbName = pcb;
        leds = ledArray;   // Store pointer to the static array
        ledCount = count;
        currentIndex = 0;  // Initialize currentIndex to 0
        firstPanel = nullptr;  // Initialize first panel pointer
        panelCount = 0;    // Initialize panel count
    }

    /**
     * @brief Initializes the LED strip with FastLED
     * @see This method is called during setup in 2A13-BACKLIGHT_CONTROLLER.ino
     */
    void initialize() {
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

    /**
     * @brief Adds a panel to the channel's linked list
     * @tparam PanelType The type of panel to add
     * @see This method is called by setup() in 2A13-BACKLIGHT_CONTROLLER.ino
     */
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
            Panel* currentPanel = firstPanel;
            while (currentPanel->nextPanel != nullptr) {
                currentPanel = currentPanel->nextPanel;
            }
            currentPanel->nextPanel = panel;  // Add new panel at the end
        }
        
        panelCount++;
        currentIndex += panel->getLedCount();
    }

    // Getters
    /**
     * @brief Gets the pin number for this channel
     * @return The pin number
     */
    uint8_t getPin() const { return pin; }

    /**
     * @brief Gets the PCB name for this channel
     * @return The PCB name
     */
    const char* getPcbName() const { return pcbName; }

    /**
     * @brief Gets the number of LEDs in this channel
     * @return The LED count
     */
    uint16_t getLedCount() const { return ledCount; }

    /**
     * @brief Gets the LED array for this channel
     * @return Pointer to the LED array
     */
    CRGB* getLeds() const { return leds; }

    /**
     * @brief Gets the first panel in the channel's linked list
     * @return Pointer to the first panel
     */
    Panel* getFirstPanel() const { return firstPanel; }

    /**
     * @brief Gets the number of panels in this channel
     * @return The panel count
     */
    uint8_t getPanelCount() const { return panelCount; }

    /**
     * @brief Updates backlights for all panels in this channel
     * @param brightness The brightness value to set
     * @param color The color to set (defaults to NVIS_GREEN_A)
     * @see This method is called by Board::fillSolid() and Board::updateInstrumentLights()
     */
    void updateInstrLights(uint16_t brightness, const CRGB& color = NVIS_GREEN_A) {
        Panel* current = firstPanel;
        while (current != nullptr) {
            current->setInstrLights(brightness, color);
            current = current->nextPanel;
        }
    }

    /**
     * @brief Updates console lights for all panels in this channel
     * @param brightness The brightness value to set
     * @param color The color to set (defaults to NVIS_GREEN_A)
     * @see This method is called by Board::updateConsoleLights() 
     */
    void updateConsoleLights(uint16_t brightness, const CRGB& color = NVIS_GREEN_A) {
        Panel* current = firstPanel;
        while (current != nullptr) {
            current->setConsoleLights(brightness, color);
            current = current->nextPanel;
        }
    }

    /**
     * @brief Updates flood lights for all panels in this channel
     * @param brightness The brightness value to set
     * @see This method is called by Board::updateFloodLights() 
     */
    void updateFloodLights(uint16_t brightness) {
        Panel* current = firstPanel;
        while (current != nullptr) {
            current->setFloodlights(brightness);
            current = current->nextPanel;
        }
    }

    /**
     * @brief Turns off all lights in all panels of this channel and resets brightness state
     * @see This method is called by Board::setAllLightsOff()
     */
    void setAllLightsOff() {
        // Clear all LEDs in the entire channel array (not just panel-tracked ones)
        fill_solid(leds, ledCount, NVIS_BLACK);
        
        // Also clear panel-tracked LEDs and reset brightness state
        Panel* current = firstPanel;
        while (current != nullptr) {
            current->setAllLightsOff();
            current = current->nextPanel;
        }
    }

};

#endif 