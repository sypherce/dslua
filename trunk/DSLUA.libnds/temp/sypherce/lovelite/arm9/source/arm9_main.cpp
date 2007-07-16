/**********************************
  Copyright (C) Rick Wong (Lick)
  http://licklick.wordpress.com/
***********************************/
#ifndef ARM9
#define ARM9
#endif

#include "arm9_main.h"

#include "Top_img_bin.h"
#include "Top_pal_bin.h"
#include "Bottom_img_bin.h"
#include "Bottom_pal_bin.h"


void Mem16CopyTwice8(u16 *dst, u8* src, u32 halfwords)
{
    for(u32 i=0; i<halfwords; i++)
    {
        dst[i] = src[i*2] | (src[i*2+1] << 8);
    }
}

void SetBrightnessIcon(u8 brightnessLevel)
{
    // Make all icons empty
    Mem16CopyTwice8((u16*)BG_BMP_RAM(0), (u8*)(Bottom_img_bin), 256*64 / 2);

    // Set the right icon
    u16 *dst = (u16*)BG_BMP_RAM(0);
    u8 *src = (u8*)(Bottom_img_bin + 256*64);

    u32 start_x = 30/2 + (brightnessLevel * 51/2);
    u32 start_y = 0;

    for(u32 y=start_y; y<start_y+64; y++)
    {
        for(u32 x=start_x; x<start_x+32; x++)
        {
            u32 i2 = (y * 128 + x) * 2;
            dst[y * 128 + x] = src[i2] | (src[i2+1] << 8);
        }
    }
}


int main(void) 
{
    //  Standard NDS setup
    //
    powerON(POWER_ALL);
    lcdMainOnBottom();

    irqInit();
    irqEnable(IRQ_VBLANK);
	irqSet(IRQ_VBLANK, 0);
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;
    
    //  Both Main & Sub: Mode5 with extended rotation bg's
    //
    videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE/* | DISPLAY_BG3_ACTIVE*/);
    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE);

    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankE(VRAM_E_MAIN_SPRITE);

    memset(BG_GFX,      0, 256*1024);
    memset(BG_GFX_SUB,  0, 128*1024);
    //  Top screen setup
    //
	SUB_BG2_CR = BG_BMP8_256x256 | BG_BMP_BASE(0) | BG_PRIORITY_1;
	SUB_BG2_XDX = 1 << 8;
	SUB_BG2_XDY = 0;
	SUB_BG2_YDX = 0;
	SUB_BG2_YDY = 1 << 8;

	SUB_BG3_CR = BG_BMP8_256x256 | BG_BMP_BASE(4) | BG_PRIORITY_0;
	SUB_BG3_XDX = 1 << 8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1 << 8;

    u32 angle = 0;
    SUB_BG2_CX = -15 * (SIN[angle+180 & 0x1FF]>>4);
    SUB_BG2_CY = ((-32) << 8) + 5 * (SIN[angle & 0x1FF]>>4);
    SUB_BG3_CX = 15 * (SIN[angle+90 & 0x1FF]>>4);
    SUB_BG3_CY = ((-120) << 8) + 5 * (SIN[angle & 0x1FF]>>4);

    memcpy(BG_PALETTE_SUB, Top_pal_bin, Top_pal_bin_size);
    BG_PALETTE_SUB[0] = RGB15(31, 31, 31);
    Mem16CopyTwice8((u16*)BG_BMP_RAM_SUB(0), (u8*)(Top_img_bin),          256*80 / 2);
    Mem16CopyTwice8((u16*)BG_BMP_RAM_SUB(4), (u8*)(Top_img_bin + 256*80), 256*32 / 2);
    

    //  Bottom screen setup
    //
	BG2_CR = BG_BMP8_256x256 | BG_BMP_BASE(0) | BG_PRIORITY_1;
	BG2_XDX = 1 << 8;
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = 1 << 8;
	BG2_CX = 0;
	BG2_CY = (-50) << 8;

    memcpy(BG_PALETTE, Bottom_pal_bin, Bottom_pal_bin_size);
    BG_PALETTE[0] = RGB15(31, 31, 31);
    Mem16CopyTwice8((u16*)BG_BMP_RAM(0), (u8*)(Bottom_img_bin), 256*64 / 2);
    Mem16CopyTwice8((u16*)(BG_BMP_RAM(0) + 256*90), (u8*)(Bottom_img_bin + 256*128), 256*32 / 2);

/*    
    BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(6) | BG_PRIORITY_0;
    BG3_XDX = 1 << 8;
    BG3_XDY = 0;
    BG3_YDX = 0;
    BG3_YDY = 1 << 8;
    BG3_CX = 0;
    BG3_CY = 0;

    dsfontMemory = (u16*)BG_BMP_RAM(6);
*/

    //
    //  Done with all the graphics !!
    //

    u8 brightnessLevel = 0;
    brightnessLevel = NDSX_GetBrightness();
    SetBrightnessIcon(brightnessLevel);

    swiWaitForVBlank();
    swiWaitForVBlank();
    fatInitDefault();

    while(1)
    {
        angle++;
        SUB_BG2_CX = -15 * (SIN[angle+180 & 0x1FF]>>4);
	    SUB_BG2_CY = ((-32) << 8) + 5 * (SIN[angle & 0x1FF]>>4);
        SUB_BG3_CX = 15 * (SIN[angle+90 & 0x1FF]>>4);
	    SUB_BG3_CY = ((-120) << 8) + 5 * (SIN[angle & 0x1FF]>>4);

        scanKeys();
        touchPosition tp = touchReadXY();
        if(tp.py >= 0)
        {
            if(BETWEEN(tp.py, 60, 100))
            {
                REGULATE_START(15); // 1/4 second
                if(BETWEEN(tp.px, 30, 30 +40))
                {
                    if(brightnessLevel != 0) {
                        brightnessLevel = 0;
                        SetBrightnessIcon(brightnessLevel);
                        NDSX_SetBrightness_0();
                    }
                }
                else if(BETWEEN(tp.px, 81, 81 +40))
                {
                    if(brightnessLevel != 1) {
                        brightnessLevel = 1;
                        SetBrightnessIcon(brightnessLevel);
                        NDSX_SetBrightness_1();
                    }
                }
                else if(BETWEEN(tp.px, 132, 132 +40))
                {
                    if(brightnessLevel != 2) {
                        brightnessLevel = 2;
                        SetBrightnessIcon(brightnessLevel);
                        NDSX_SetBrightness_2();
                    }
                }
                else if(BETWEEN(tp.px, 183, 183 +40))
                {
                    if(brightnessLevel != 3) {
                        brightnessLevel = 3;
                        SetBrightnessIcon(brightnessLevel);
                        NDSX_SetBrightness_3();
                    }
                }
                REGULATE_END();
            }

            if(BETWEEN(tp.py, 144, 144 +24))
            {
                if(BETWEEN(tp.px, 70, 183))
                {
                    if(NDSX_IsLite())
                        NDSX_SaveBrightness();

                    if(cartSetMenuMode(DEVICE_TYPE_AUTO))
                    {
                        // Card recognized, "Menu" mode ready
                        passmeloopEnter();
                    }
                    else
                    {
                        // Card not recognized, trying without "Menu" mode
                        BG_PALETTE_SUB[0] = RGB15(31, 0, 0);
                        passmeloopEnter();
                    }
                }
            }
        }

        swiWaitForVBlank();
    }
    return 0;
}

