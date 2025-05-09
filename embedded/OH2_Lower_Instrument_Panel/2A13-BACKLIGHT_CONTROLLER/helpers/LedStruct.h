#ifndef __LED_STRUCT_H
#define __LED_STRUCT_H

#include <stdint.h>
#include "LedRole.h"

// Maximum length for LED text description
const int MAX_LEN = 16;                // Maximum length for legend text (including null terminator)

/**********************************************************************************************************************
 * @brief   LED info structure.
 * @details Bundles properties of one LED.
 *********************************************************************************************************************/ 
struct Led {        
    uint16_t index;               // Local position of the LED on the panel, starts at 0
    LedRole  role;                // Role of LED as the enum defined above
    char     text[MAX_LEN];       // Free text to associate LED with legend text on a panel
};

#endif 