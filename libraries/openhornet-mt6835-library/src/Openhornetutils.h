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
* @file openhornet-mt6835-library.zip
* @author Sandra
* @date 04.21.2024
* @version u.0.6.3
* @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
* @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. The throttle solenoids weren't working. (Remove this line once tested on hardware and in system.)
* @brief Controls the THROTTLE QUADRANT.
*
* @details
* 
* @attention contains parts of Simple FOC libraries with license avialable here: https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
* 
*/

#if not defined(FOCUTILS_LIB_H)
#define FOCUTILS_LIB_H

#include "Arduino.h"

// sign function
#define _sign(a) ( ( (a) < 0 )  ?  -1   : ( (a) > 0 ) )

#if not defined(_round)
    #define _round(x) ((x)>=0?(long)((x)+0.5f):(long)((x)-0.5f))
#endif

#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define _sqrt(a) (_sqrtApprox(a))
#define _isset(a) ( (a) != (NOT_SET) )
#define _UNUSED(v) (void) (v)

// utility defines
#define _2_SQRT3 1.15470053838f
#define _SQRT3 1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2 1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI 3.14159265359f
#define _PI_2 1.57079632679f
#define _PI_3 1.0471975512f
#define _2PI 6.28318530718f
#define _3PI_2 4.71238898038f
#define _PI_6 0.52359877559f
#define _RPM_TO_RADS 0.10471975512f

#define NOT_SET -12345.0
#define _HIGH_IMPEDANCE 0
#define _HIGH_Z _HIGH_IMPEDANCE
#define _ACTIVE 1
#define _NC (NOT_SET)

#define MIN_ANGLE_DETECT_MOVEMENT (_2PI/101.0f)

/** 
 * Function implementing delay() function in milliseconds 
 * - blocking function
 * - hardware specific

 * @param ms number of milliseconds to wait
 */
void _delay(unsigned long ms);

/** 
 * Function implementing timestamp getting function in microseconds
 * hardware specific
 */
unsigned long _micros();

// dq current structure
struct DQCurrent_s
{
    float d;
    float q;
};
// phase current structure
struct PhaseCurrent_s
{
    float a;
    float b;
    float c;
};
// dq voltage structs
struct DQVoltage_s
{
    float d;
    float q;
};


/**
 *  Function approximating the sine calculation by using fixed size array
 * - execution time ~40us (Arduino UNO)
 *
 * @param a angle in between 0 and 2PI
 */
float _sin(float a);
/**
 * Function approximating cosine calculation by using fixed size array
 * - execution time ~50us (Arduino UNO)
 *
 * @param a angle in between 0 and 2PI
 */
float _cos(float a);

/**
 * normalizing radian angle to [0,2PI]
 * @param angle - angle to be normalized
 */
float _normalizeAngle(float angle);


/**
 * Electrical angle calculation
 *
 * @param shaft_angle - shaft angle of the motor
 * @param pole_pairs - number of pole pairs
 */
float _electricalAngle(float shaft_angle, int pole_pairs);

/**
 * Function approximating square root function
 *  - using fast inverse square root
 *
 * @param value - number
 */
float _sqrtApprox(float value);

#endif