/**********************************
  Copyright (C) Rick Wong (Lick)
  http://licklick.wordpress.com/
***********************************/
#ifndef _NDSX_BRIGHTNESS_
#define _NDSX_BRIGHTNESS_


#include <nds/ipc.h>
#ifdef ARM7
#include <nds/arm7/serial.h>
#endif

#include "ndsx_firmware.h" // needed for hardware-stored brightnesslevel.


/*
    PowerManagement bits that libnds doesn't define (yet?).
    Check out "arm7/serial.h".
*/
#define PM_HARDWARE_REG     (4)     // SPI DATA 4
#define PM_BRIGHTNESS       (0x03)  // BIT(0)|BIT(1) of SPI DATA 4
#define PM_DSLITE           (0x40)  // BIT(6) of SPI DATA 4
#define PM_BACKLIGHTS       (PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP)

/*
    FIFO Message-IDs, shared between ARM7 and ARM9.
*/
#define IS_HARDWARE_LITE        (0x1211B207)

#define GET_BRIGHTNESS          (0x1211B210) // On Phatty DS: GET_BACKLIGHTS
#define SET_BRIGHTNESS_0        (0x1211B211) // On Phatty DS: SET_BACKLIGHTS_OFF
#define SET_BRIGHTNESS_1        (0x1211B212) // On Phatty DS: SET_BACKLIGHTS_ON
#define SET_BRIGHTNESS_2        (0x1211B213) // On Phatty DS: SET_BACKLIGHTS_ON
#define SET_BRIGHTNESS_3        (0x1211B214) // On Phatty DS: SET_BACKLIGHTS_ON 
#define SET_BRIGHTNESS_NEXT     (0x1211B215) // On Phatty DS: SET_BACKLIGHTS_TOGGLE
#define SET_BRIGHTNESS_PREVIOUS (0x1211B216) // On Phatty DS: SET_BACKLIGHTS_TOGGLE

#define SAVE_BRIGHTNESS         (0x1211B2FF) 

#define GET_BACKLIGHTS          (0x1211B217)
#define SET_BACKLIGHTS_ON       (0x1211B218)
#define SET_BACKLIGHTS_OFF      (0x1211B219)
#define SET_BACKLIGHTS_TOGGLE   (0x1211B220)


/*
    ARM9 functions to command the ARM7.

    NOTES:
  - swiWaitForVBlank() makes sure you don't interact with the ARM7
    while it's doing the writePM() and readPM() calls. (Those are
    SPI accessing functions and must run without being interrupted.)
  - When calling SPI-functions like writePM()/readPM(), you ACTUALLY
    REALLY have to DISABLE INTERRUPTS first. But swiWaitForVBlank()
    will suffice most of the times.
*/
#ifdef ARM9
inline bool NDSX_IsLite()
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = IS_HARDWARE_LITE;
    swiWaitForVBlank();
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (bool)REG_IPC_FIFO_RX;
}

inline u32  NDSX_GetBrightness()
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = GET_BRIGHTNESS;
    swiWaitForVBlank();
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (u32)REG_IPC_FIFO_RX;
}

inline void NDSX_SaveBrightness()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SAVE_BRIGHTNESS;
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_0()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_0;
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_1()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_1; 
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_2()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_2; 
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_3()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_3; 
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_Next()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_NEXT;
    swiWaitForVBlank();
}

inline void NDSX_SetBrightness_Previous()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BRIGHTNESS_PREVIOUS;
    swiWaitForVBlank();
}

inline u32  NDSX_GetBacklights()
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = GET_BACKLIGHTS;
    swiWaitForVBlank();
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (u32)REG_IPC_FIFO_RX;
}

inline void NDSX_SetBacklights_On()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BACKLIGHTS_ON;
    swiWaitForVBlank();
}

inline void NDSX_SetBacklights_Off()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BACKLIGHTS_OFF;
    swiWaitForVBlank();
}

inline void NDSX_SetBacklights_Toggle()
{ 
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = SET_BACKLIGHTS_TOGGLE;
    swiWaitForVBlank();
}
#endif // ARM9


/*
    ARM7 code.

    NOTES:
  - The BrightnessFifo should be called from an 
    ARM7-fifo-on-recv-interrupt-handler.
*/
#ifdef ARM7
static inline bool NDSX_ARM7_IsLite()
{
    if((bool)(readPowerManagement(PM_HARDWARE_REG) & PM_DSLITE))
        return true;
    return false;
}
void NDSX_ARM7_StoreBrightness(u8 level)
{
    u16 count0 = 0, count1 = 0;
    u16 version_always_5 = 0;
    u8  unused_always_0 = 0;
    u8  usersettings[256] = {0};

    if(level > 3)
        level = 3;

    if(NDSX_ARM7_IsLite() == false)
        return;


    /*
        UserSettings0 exists at [Normal] location?
    */
    Read_FlashFast(0x3FE00, (u8*)&version_always_5, 2);
    Read_FlashFast(0x3FE05, (u8*)&unused_always_0,  1);
    if(version_always_5 == 5 || unused_always_0 == 0)
    {
        /*
            UserSettings1 exists at [Normal] location?
        */
        Read_FlashFast(0x3FF00, (u8*)&version_always_5, 2);
        Read_FlashFast(0x3FF05, (u8*)&unused_always_0,  1);
        if(version_always_5 == 5 || unused_always_0 == 0)
        {
            Read_FlashFast(0x3FE70, (u8*)&count0, 2);
            Read_FlashFast(0x3FF70, (u8*)&count1, 2);

            if(count0 > count1)  // US0 is currently selected. Update it!
            {
                
                Read_FlashFast(0x3FE00, (u8*)usersettings, 0x100);
                u16 *p_language = (u16*)(usersettings + 0x64);
                u16 *p_count    = (u16*)(usersettings + 0x70);
                u16 *p_crc      = (u16*)(usersettings + 0x72);
                *p_language = ((*p_language) & 0xFFCF) | ((level&3) << 4);
                *p_count    = count0+1;
                *p_crc      = swiCRC16(0xFFFF, usersettings, 0x70);
                Write_Flash(0x3FE00, usersettings, 0x100);
            }
            else   // US1 is currently selected. Update it!
            {
                Read_FlashFast(0x3FF00, (u8*)usersettings, 0x100);
                u16 *p_language = (u16*)(usersettings + 0x64);
                u16 *p_count    = (u16*)(usersettings + 0x70);
                u16 *p_crc      = (u16*)(usersettings + 0x72);
                *p_language = ((*p_language) & 0xFFCF) | ((level&3) << 4);
                *p_count    = count1+1;
                *p_crc      = swiCRC16(0xFFFF, usersettings, 0x70);
                Write_Flash(0x3FF00, usersettings, 0x100);
            }
        }

        return;
    }


    /*
        UserSettings0 exists at [iQue] location?
    */
    Read_FlashFast(0x7FE00, (u8*)&version_always_5, 2);
    Read_FlashFast(0x7FE05, (u8*)&unused_always_0,  1);
    if(version_always_5 == 5 || unused_always_0 == 0)
    {
        /*
            UserSettings1 exists at [iQue] location?
        */
        Read_FlashFast(0x7FF00, (u8*)&version_always_5, 2);
        Read_FlashFast(0x7FF05, (u8*)&unused_always_0,  1);
        if(version_always_5 == 5 || unused_always_0 == 0)
        {
            Read_FlashFast(0x7FE70, (u8*)&count0, 2);
            Read_FlashFast(0x7FF70, (u8*)&count1, 2);

            if(count0 > count1)  // US0 is currently selected. Update it!
            {
                
                Read_FlashFast(0x7FE00, (u8*)usersettings, 0x100);
                u16 *p_language = (u16*)(usersettings + 0x64);
                u16 *p_count    = (u16*)(usersettings + 0x70);
                u16 *p_crc      = (u16*)(usersettings + 0x72);
                *p_language = ((*p_language) & 0xFFCF) | ((level&3) << 4);
                *p_count    = count0+1;
                *p_crc      = swiCRC16(0xFFFF, usersettings, 0x70);
                Write_Flash(0x7FE00, usersettings, 0x100);
            }
            else   // US1 is currently selected. Update it!
            {
                Read_FlashFast(0x7FF00, (u8*)usersettings, 0x100);
                u16 *p_language = (u16*)(usersettings + 0x64);
                u16 *p_count    = (u16*)(usersettings + 0x70);
                u16 *p_crc      = (u16*)(usersettings + 0x72);
                *p_language = ((*p_language) & 0xFFCF) | ((level&3) << 4);
                *p_count    = count1+1;
                *p_crc      = swiCRC16(0xFFFF, usersettings, 0x70);
                Write_Flash(0x7FF00, usersettings, 0x100);
            }
        }

        return;
    }
}

inline bool NDSX_ARM7_BrightnessFifo(u32 msg)
{
    if(msg == IS_HARDWARE_LITE)
    {
        REG_IPC_FIFO_TX = NDSX_ARM7_IsLite();
        return true;
    }

    // Is Lite?
    if(NDSX_ARM7_IsLite())
    {
        if(msg == GET_BRIGHTNESS)
        {
            REG_IPC_FIFO_TX = readPowerManagement(PM_HARDWARE_REG) & PM_BRIGHTNESS;
            return true;
        }    
        else if(msg == SAVE_BRIGHTNESS)
        {
            u8 level = readPowerManagement(PM_HARDWARE_REG) & PM_BRIGHTNESS;
            NDSX_ARM7_StoreBrightness(level);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_0)
        {
            writePowerManagement(PM_HARDWARE_REG, 0);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_1)
        {
            writePowerManagement(PM_HARDWARE_REG, 1);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_2)
        {
            writePowerManagement(PM_HARDWARE_REG, 2);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_3)
        {
            writePowerManagement(PM_HARDWARE_REG, 3);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_NEXT)
        {
            s32 nextlevel = 
                (readPowerManagement(PM_HARDWARE_REG) & PM_BRIGHTNESS) + 1;
            if(nextlevel > 3) 
                nextlevel = 0;
            writePowerManagement(PM_HARDWARE_REG, nextlevel);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_PREVIOUS)
        {
            s32 previouslevel = 
                (readPowerManagement(PM_HARDWARE_REG) & PM_BRIGHTNESS) - 1;
            if(previouslevel < 0)
                previouslevel = 3;
            writePowerManagement(PM_HARDWARE_REG, previouslevel);
            return true;
        }
        else if(msg == GET_BACKLIGHTS)
        {
            u32 backlight_bits = readPowerManagement(PM_CONTROL_REG) & PM_BACKLIGHTS;
            if(backlight_bits == 12)
                REG_IPC_FIFO_TX = 2;
            else if(backlight_bits == 8 || backlight_bits == 4)
                REG_IPC_FIFO_TX = 1;
            else
                REG_IPC_FIFO_TX = 0;
            return true;
        }
        else if(msg == SET_BACKLIGHTS_ON)
        {
            u32 reg_with_backlight = readPowerManagement(PM_CONTROL_REG) | PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_with_backlight);
            return true;
        }
        else if(msg == SET_BACKLIGHTS_OFF)
        {
            u32 reg_without_backlight = readPowerManagement(PM_CONTROL_REG) & ~PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_without_backlight & 255);
            return true;
        }
        else if(msg == SET_BACKLIGHTS_TOGGLE)
        {
            u32 oldreg = readPowerManagement(PM_CONTROL_REG);
            if(oldreg & PM_BACKLIGHTS) 
                oldreg &= ~PM_BACKLIGHTS;
            else
                oldreg |= PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, oldreg);
            return true;
        }
    } 
    else // Is Phatty!
    {
        if(msg == GET_BRIGHTNESS)
        {
            u32 backlight_bits = readPowerManagement(PM_CONTROL_REG) & PM_BACKLIGHTS;
            if(backlight_bits)
                REG_IPC_FIFO_TX = 1;
            else
                REG_IPC_FIFO_TX = 0;
            return true;
        }
        else if(msg == GET_BACKLIGHTS)
        {
            u32 backlight_bits = readPowerManagement(PM_CONTROL_REG) & PM_BACKLIGHTS;
            if(backlight_bits == 12)
                REG_IPC_FIFO_TX = 2;
            else if(backlight_bits == 8 || backlight_bits == 4)
                REG_IPC_FIFO_TX = 1;
            else
                REG_IPC_FIFO_TX = 0;
            return true;
        }    
        else if(msg == SET_BRIGHTNESS_0)
        {
            u32 reg_without_backlight = readPowerManagement(PM_CONTROL_REG) & ~PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_without_backlight);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_1 ||
            msg == SET_BRIGHTNESS_2 ||
            msg == SET_BRIGHTNESS_3)
        {
            u32 reg_with_backlight = readPowerManagement(PM_CONTROL_REG) | PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_with_backlight);
            return true;
        }
        else if(msg == SET_BRIGHTNESS_NEXT ||
            msg == SET_BRIGHTNESS_PREVIOUS ||
            msg == SET_BACKLIGHTS_TOGGLE)
        {
            u32 oldreg = readPowerManagement(PM_CONTROL_REG);
            if(oldreg & PM_BACKLIGHTS) 
                oldreg &= ~PM_BACKLIGHTS;
            else
                oldreg |= PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, oldreg);
            return true;
        }
        else if(msg == SET_BACKLIGHTS_ON)
        {
            u32 reg_with_backlight = readPowerManagement(PM_CONTROL_REG) | PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_with_backlight);
            return true;
        }
        else if(msg == SET_BACKLIGHTS_OFF)
        {
            u32 reg_without_backlight = readPowerManagement(PM_CONTROL_REG) & ~PM_BACKLIGHTS;
            writePowerManagement(PM_CONTROL_REG, reg_without_backlight);
            return true;
        }
    } 

    return false;
}
#endif // ARM7


#endif // _NDSX_BRIGHTNESS_
