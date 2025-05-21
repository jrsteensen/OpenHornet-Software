#ifndef LED_ROLE_H
#define LED_ROLE_H

/**********************************************************************************************************************
 * @brief   LED role enumeration.
 * @details Defines the different roles that LEDs can have in the panels.
 * @remark  This enum is used for memory efficiency and type safety.
 *********************************************************************************************************************/
enum LedRole {
    LED_BACKLIGHT,
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
    LED_BLANK1,  
    LED_BLANK2,  
    LED_BLANK3,  
    LED_BLANK4,  
    LED_BLANK5,  
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
    LED_SPARE1,
    LED_FUEL_LO,
    LED_FCES,
    LED_SPARE2,
    LED_L_GEN,
    LED_R_GEN,
    LED_SPARE3
};

#endif 