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
 * @file      LedRole.h
 * @author    Ulukaii
 * @date      24.05.2025
 * @version   t 0.3.2
 * @copyright Copyright 2016-2025 OpenHornet. See 2A13-BACKLIGHT_CONTROLLER.ino for details.
 * @brief     Defines the role of the LEDs in the panels.
 * @details   Each panel holds a table of LEDs. These contain the role of the LED, which is one of the values
 *            defined in this enum. When LEDs update commands are processed, the table of LEDs is scanned through
 *            and only those LEDs that match the desired role are updated.
 *********************************************************************************************************************/

#ifndef LED_ROLE_H
#define LED_ROLE_H

/**********************************************************************************************************************
 * @brief   LED role enumeration.
 * @details Defines the different roles that LEDs can have in the panels.
 * @remark  This enum is used for memory efficiency and type safety.
 *********************************************************************************************************************/
enum LedRole {
    LED_INSTR_BL,
    LED_CONSOLE_BL,
    LED_INSTR_BL_CGRB,
    LED_READY,
    LED_DISCH,
    LED_AG,
    LED_AA,
    // EWI Panel specific types
    LED_FIRE,
    LED_CAUTION,
    LED_GO,
    LED_NO_GO,
    LED_R_BLEED,
    LED_L_BLEED,
    LED_SPD_BRK,
    LED_STBY,
    LED_REC,
    LED_L_BAR1,
    LED_L_BAR2,
    LED_XMIT,
    LED_ASPJ_ON,
    // Right EWI Panel specific types
    LED_R_FIRE,
    LED_APU_FIRE,
    LED_DISP,
    LED_RCDRON,
    LED_SPARE1,  
    LED_SPARE2,  
    LED_SPARE3,  
    LED_SPARE4,  
    LED_SPARE5,  
    LED_SAM,
    LED_AAA,
    LED_AI,
    LED_CW,
    // Spin Recovery Panel specific types
    LED_SPIN,
    // ECM Panel specific types
    LED_ECM_JETT_SEL,
    LED_ECM_BACKLIGHT,
    // RWR Panel specific types
    LED_RWR_BIT,
    LED_RWR_BIT_FAIL,
    LED_RWR_OFFSET,
    LED_RWR_OFFSET_EN,
    LED_RWR_SPECIAL,
    LED_RWR_SPECIAL_EN,
    LED_RWR_DISPLAY,
    LED_RWR_LIMIT,
    LED_RWR_POWER,
    LED_RWR_NONE,
    // Flood Lights specific types
    LED_FLOOD,
    // Caution Panel specific types
    LED_CK_SEAT,
    LED_APU_ACC,
    LED_BATT_SW,
    LED_FCS_HOT,
    LED_GEN_TIE,
    LED_CSPARE1,
    LED_FUEL_LO,
    LED_FCES,
    LED_CSPARE2,
    LED_L_GEN,
    LED_R_GEN,
    LED_CSPARE3,
    // Jett Station Panel specific types
    LED_JETT_RO_1,
    LED_JETT_RO_2,
    LED_JETT_RI_1,
    LED_JETT_RI_2,
    LED_JETT_CTR_1,
    LED_JETT_CTR_2,
    LED_JETT_LI_1,
    LED_JETT_LI_2,
    LED_JETT_LO_1,
    LED_JETT_LO_2,
    LED_JETT_NOSE,
    LED_JETT_LEFT,
    LED_JETT_RIGHT,
    LED_JETT_HALF,
    LED_JETT_FULL,
    LED_JETT_FLAPS
};

#endif 