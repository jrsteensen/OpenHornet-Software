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
 *   Copyright 2016-2024 OpenHornet
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
 @file      HornetStepper.h
 @author    Ulukaii
 @date      13.01.2026
 @version   0.3.0
 @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 @brief     Common class to control stepper-run gauges in OpenHornet. 
 @details   Uses the AccelStepper library to control the stepper motor.

            Usage:
            (1/4) 
            In the main .ino, before setup(), create a HornetStepper object for each gauge:
            HornetStepper myStepper(COIL1, COIL2, COIL3, COIL4, ZERO_POS, MAX_POS, 
                                   DIRECTION, CAP_VALUE, normalSpeed, normalAccel,
                                   mapPoints, numMapPoints);
            where:
            - COIL1, COIL2, COIL3, COIL4 are the pins for the coils of the stepper motor
            - ZERO_POS is the position of the zero on the dial, in steps from low mech stop
            - MAX_POS is the maximum position of the needle, in steps from low mech stop
            - DIRECTION is the direction of the needle, 1 for forward, -1 for backward

            (2/4) 
            In your main .ino, before setup(), link the DCS-BIOS to the gauge:
            void func(unsigned int newVal) {
                myStepper.setTarget(newVal);
            }
            DcsBios::IntegerBuffer radaltAltPtrBuffer(FA_18C_hornet_RADALT_ALT_PTR, func);

            (3/4)
            In your main .ino, in setup(), optionally call the findZero() and the
            testFullRange() functions to zero and test the range of the gauge:
            myStepper.findZero();
            myStepper.testFullRange();

            (4/4) 
            In your main .ino, in the loop(), call the run() function like this:
            myStepper.run();

            Optional functionalities:
            - Adapt speed and acceleration by changing normalSpeed and normalAccel
            - Enable non.linear mapping with the optional mapPoints parameter  
            - Press CLR and ENT buttons on UFC at the same time to manually trigger homing.

*/

#ifndef HORNET_STEPPER_H
#define HORNET_STEPPER_H

#include <AccelStepper.h>
#include <Arduino.h>
#include <stdlib.h>
#include "DcsBios.h"
#include <MultiMap.h>                                                 // by Rob Tillart

/**
 * @brief Only in case of non-linear gauges, this struct is used. 
 *        It represents a value-position mapping pair. 
 */
struct MapPoint {
    unsigned int value;                                               // DCS-BIOS input value (0-65535)
    unsigned int position;                                            // Stepper step position (e.g. 0-720)
    };

/**
 * @brief   Common class to control stepper-run gauges in OpenHornet. 
 * @details Uses the AccelStepper library to control the stepper motor.
 */
class HornetStepper {
private:
    const int mechZero = 0;                                           // Mechanical stop position (is always 0)
    int       zeroPos;                                                // Dial zero position (e.g., 20 steps from mech 0)
    int       maxPos;                                                 // Maximum position (e.g., 720 steps from mech 0)
    int       dirForward;                                             // Direction for forward movement (1 or -1)
    unsigned int capValue;                                            // DCS-BIOS value to cap at max mech stop
    int       normalSpeed;                                            // Normal operating speed
    int       normalAccel;                                            // Normal operating acceleration  

    MapPoint* mapPoints;                                              // Array of value-position mapping pairs (optional)
    uint8_t numMapPoints;                                             // The size of mapPoints array
    unsigned int* inputVals;                                          // Extracted input values array for multiMapBS
    unsigned int* outputPos;                                          // Extracted output positions array for multiMapBS
    bool useMultiMap;                                                 // Flag to use multiMap vs linear mapping

    AccelStepper stepper;                                             // Stepper motor object
    bool      homingInProgress;                                       // Track if homing is in progress
    bool      testInProgress;                                         // Track if test is in progress

public:
                                                                      // Track button states to manually trigger homing
                                                                      // DCS-Bios expects these to be static
    static bool ufcEntPressed;                                        // UFC ENTER button state (static; shared across instances)
    static bool ufcClrPressed;                                        // UFC CLEAR button state (static; shared across instances)


    /**
     * @brief Constructor for HornetStepper
     * @param coil1 Pin for coil 1
     * @param coil2 Pin for coil 2
     * @param coil3 Pin for coil 3
     * @param coil4 Pin for coil 4
     * @param zeroPos Position of the zero on the dial, in steps from low mechanical stop (absolute position, e.g., 20)
     * @param maxPos Maximum number of steps from low mech stop to high mech stop
     * @param dirForward Direction for forward movement (1 or -1)
     * @param capValue DCS-BIOS value to cap at (default 65535). This is needed if mech stop is lower than game maximum. 
     *                Values above this will be auto-capped, preventing the needle from exceeding maxPos position.
     * @param normalSpeed Normal operating speed
     * @param normalAccel Normal operating acceleration
     * @param mapPoints Optional array of MapPoint structs for non-linear mapping (nullptr for linear mapping)
     * @param numMapPoints Number of mapping points (required if mapPoints is provided)

     */
    HornetStepper(int coil1, int coil2, int coil3, int coil4,
                  int zeroPos, int maxPos, int dirForward, 
                  unsigned int capValue = 65535,
                  int normalSpeed = 300,
                  int normalAccel = 600,
                  MapPoint* mapPoints = nullptr,
                  uint8_t numMapPoints = 0)
        : stepper(AccelStepper::FULL4WIRE,
                 (dirForward == -1) ? coil3 : coil1,
                 (dirForward == -1) ? coil4 : coil2,
                 (dirForward == -1) ? coil1 : coil3,
                 (dirForward == -1) ? coil2 : coil4)
    {
        this->zeroPos = zeroPos;
        this->maxPos = maxPos;
        this->dirForward = dirForward;
        this->capValue = capValue;
        this->normalSpeed = normalSpeed;
        this->normalAccel = normalAccel;
        this->homingInProgress = false;
        this->testInProgress = false;
        
        stepper.setMaxSpeed(normalSpeed);
        stepper.setAcceleration(normalAccel);

        // Handle mapping array - if nullptr passed, use linear mapping
        if (mapPoints != nullptr && numMapPoints > 0) {
            // Use provided mapping array for non-linear mapping
            this->mapPoints = mapPoints;
            this->numMapPoints = numMapPoints;
            this->useMultiMap = true;
            
            // Extract input values and output positions arrays for multiMapBS
            this->inputVals = new unsigned int[numMapPoints];
            this->outputPos = new unsigned int[numMapPoints];
            for (uint8_t i = 0; i < numMapPoints; i++) {
                this->inputVals[i] = mapPoints[i].value;
                this->outputPos[i] = mapPoints[i].position;
            }
        } else {
            // Use standard linear mapping (no array needed)
            this->mapPoints = nullptr;
            this->inputVals = nullptr;
            this->outputPos = nullptr;
            this->numMapPoints = 0;
            this->useMultiMap = false;
        }
    }

    /**
     * @brief Destructor to clean up allocated arrays
     */
    ~HornetStepper() {
        if (useMultiMap && inputVals != nullptr) {
            delete[] inputVals;
            delete[] outputPos;
        }
    }

    
    /**
     * @brief   findZero() is a function to zero the gauge
     * @note    The gauge is zeroed by slowly moving the needle to mechanical zero), then to 
     *          the dial zero position. You may hear clocking sounds. This is normal. 
     * @details Coordinate system:
     *          - Position 0 = mechanical zero (physical stop)
     *          - Position zeroPos (e.g., 20) = dial zero (where gauge shows "0")
     */
    void findZero() {
        int zeroingSpeed = 20;                                         // Slow speed for zeroing operation
        int zeroingAccel = 10;
        stepper.setMaxSpeed(zeroingSpeed);
        stepper.setAcceleration(zeroingAccel);

        stepper.setCurrentPosition(maxPos);                           // Assume needle at max position
        stepper.runToNewPosition(mechZero);                           // Move backwards to mech stop
        stepper.setCurrentPosition(mechZero);                         // At mech stop, set coordinate sys to 0
        stepper.runToNewPosition(zeroPos);                            // Move forward to position that is 0 on dial
        
        stepper.setMaxSpeed(normalSpeed);                             // Resume normal speed and acceleration
        stepper.setAcceleration(normalAccel);
    }
    

    /**
     * @brief testFullRange() is a function to test the range of the gauge
     * @note  During the test, the gauge is moved all the way. 
     *        runToNewPosition() blocks the CPU during test.
     */
    void testFullRange(int testSpeed = 20, int testAccel = 10) {
        testInProgress = true;
        stepper.setMaxSpeed(testSpeed);
        stepper.setAcceleration(testAccel);

        stepper.runToNewPosition(maxPos);
        delay(2000);
        stepper.runToNewPosition(zeroPos);
        
        stepper.setMaxSpeed(normalSpeed);
        stepper.setAcceleration(normalAccel);
        testInProgress = false;
    }
    

    /**
     * @brief   setTarget() is a function to set the target position of the gauge
     * @param   targetVal The target value as pure DCS BIOS value (value range: 0-65535)
     * @details Maps DCS BIOS value to stepper position:
     *          1) cap targetVal at capValue as needed
     *          2) map capped value to stepper position, 
     *             using multiMapBS if useMultiMap is true, otherwise use standard linear mapping
     *          3) call the moveTo() function of AccelStepper to pass the new target position.
     */
    void setTarget(unsigned int targetVal) {
        // 1) Cap targetVal at capValue as needed
        unsigned int trimmedVal = min(targetVal, capValue);
        
        // 2) map capped value to stepper position
        long targetPos;
        if (useMultiMap) {
            // Use multiMap with pre-extracted arrays
            targetPos = multiMapCache<unsigned int>(trimmedVal, inputVals, outputPos, numMapPoints);
        } else {
            targetPos = map(trimmedVal, 0, capValue, zeroPos, maxPos);
        }
        
        // 3) call the moveTo() function of AccelStepper to pass the new target position.
        stepper.moveTo(targetPos);
    }
    

    /**
     * @brief run() is a function to run the stepper motor (non-blocking)
     * @note This method must be called repeatedly in the main loop to perform movement.
     *       It also checks for button-triggered homing and startup test.
     */
    void run() {
        bool shouldHome = ufcEntPressed && ufcClrPressed;
        
        if (shouldHome && !homingInProgress && !testInProgress) {
            homingInProgress = true;
            findZero();
            testFullRange();
            homingInProgress = false;
        }
        
        stepper.run();
    }
    
};

// Static member definitions
bool HornetStepper::ufcEntPressed = false;
bool HornetStepper::ufcClrPressed = false;

// DCS-BIOS callbacks for homing trigger buttons
void onUfcEntChange(unsigned int newVal) {
    // newVal is 0 when not pressed, non-zero when pressed
    HornetStepper::ufcEntPressed = (newVal != 0);
}
DcsBios::IntegerBuffer ufcEntBuffer(FA_18C_hornet_UFC_ENT, onUfcEntChange);

void onUfcClrChange(unsigned int newVal) {
    // newVal is 0 when not pressed, non-zero when pressed
    HornetStepper::ufcClrPressed = (newVal != 0);
}
DcsBios::IntegerBuffer ufcClrBuffer(FA_18C_hornet_UFC_CLR, onUfcClrChange);

#endif 