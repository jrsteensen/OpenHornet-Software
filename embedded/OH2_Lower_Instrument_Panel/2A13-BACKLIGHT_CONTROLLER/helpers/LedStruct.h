#ifndef __LED_STRUCT_H
#define __LED_STRUCT_H

#include <stdint.h>
#include "LedRole.h"

// Maximum length for LED text description
const int MAX_LEN = 16;                // Maximum length for legend text (including null terminator)

/**********************************************************************************************************************
 * @brief   LED info structure.
 * @details Bundles properties "index" and "role" of one LED.
 *********************************************************************************************************************/ 
struct Led {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    LedRole  role;                // Role of LED as the enum defined above
};

/**********************************************************************************************************************
 * @brief   LED text structure.
 * @details Bundles properties "index" and "text" of one LED.
 * @remark  Associates text with specific LEDs. This is intended for the PREFLT function.
 *          This structure is used to associate text with specific LEDs. We use this in a separate struct because
 *          the 16 byte LED text is not always needed and we need to save memory and access time.
 *          Putting both in the same struct could lead to memory problems.
 *********************************************************************************************************************/ 
struct LedText {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    char     text[MAX_LEN];       // Free text to associate LED with legend text on a panel
};

#endif 