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
 * @file      LedStruct.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     LED info structures.
 * @details   struct Led:     Bundles properties "index" and "role" of one LED.
 *            struct LedText: Bundles properties "index" and "text" of one LED.
 * @remark    LedText associates text with specific LEDs. This is intended for the PREFLT function.
 *            We use this in a separate struct because the 16 byte LED text is not always needed and we need to 
 *            save memory and access time. Putting both in the same struct could lead to memory problems.
 *********************************************************************************************************************/

#ifndef __LED_STRUCT_H
#define __LED_STRUCT_H

#include <stdint.h>
#include "LedRole.h"

// Maximum length for LED text description
const int MAX_LEN = 16;                // Maximum length for legend text (including null terminator)


struct Led {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    LedRole  role;                // Role of LED as the enum defined above
};


struct LedText {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    char     text[MAX_LEN];       // Free text to associate LED with legend text on a panel
};

#endif 