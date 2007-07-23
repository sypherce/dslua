/**********************************
  Copyright (C) Rick Wong (Lick)
  http://licklick.wordpress.com/
***********************************/
#ifndef _NDSX_FIRMWARE_
#define _NDSX_FIRMWARE_


#ifdef ARM7
#include <nds/arm7/serial.h>
#endif

/*
    FIFO Message-IDs, shared between ARM7 and ARM9.
*/
#define GET_BYTE_VALUE            (0x1211FE00)
#define GET_SHORT_VALUE           (0x1211FE01)
#define GET_LONG_VALUE            (0x1211FE02)
#define GET_BLOCK_BYTE_VALUE      (0x1211FE03)
#define GET_BLOCK_SHORT_VALUE     (0x1211FE03)
#define GET_BLOCK_LONG_VALUE      (0x1211FE03)

#define SET_BYTE_VALUE            (0x1211FE10)
#define SET_SHORT_VALUE           (0x1211FE11)
#define SET_LONG_VALUE            (0x1211FE12)


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
/////////////////////
//
//  Single read:
//
static inline s8 NDSX_Firmware_GetByte(u32 address)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY));
    REG_IPC_FIFO_TX = GET_BYTE_VALUE;
    REG_IPC_FIFO_TX = address;
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (s8)REG_IPC_FIFO_RX;
}

static inline s16 NDSX_Firmware_GetShort(u32 address)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_SHORT_VALUE;
    REG_IPC_FIFO_TX = address;
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (s16)REG_IPC_FIFO_RX;
}

static inline s32 NDSX_Firmware_GetLong(u32 address)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_LONG_VALUE;
    REG_IPC_FIFO_TX = address;
    while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
    return (s32)REG_IPC_FIFO_RX;
}

/////////////////////
//
//  Mass read:
//
static inline void NDSX_Firmware_ReadBlock8(u32 address, u8 *destination, u32 numBytes)
{
    numBytes = (numBytes>512)?512:numBytes;
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BLOCK_BYTE_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = numBytes;
	u32 i;
    for(i=0; i<numBytes; i++) {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        destination[i] = REG_IPC_FIFO_RX;
    }
}

static inline void NDSX_Firmware_ReadBlock16(u32 address, u16 *destination, u32 numShorts)
{
    numShorts = (numShorts>256)?256:numShorts;
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BLOCK_SHORT_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = numShorts;
	u32 i;
    for(i=0; i<numShorts; i++) {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        destination[i] = REG_IPC_FIFO_RX;
    }
}

static inline void NDSX_Firmware_ReadBlock32(u32 address, u32 *destination, u32 numLongs)
{
    numLongs = (numLongs>128)?128:numLongs;
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BLOCK_LONG_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = numLongs;
	u32 i;
    for(i=0; i<numLongs; i++) {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        destination[i] = REG_IPC_FIFO_RX;
    }
}

/////////////////////
//
//  Single write:
//
static inline void NDSX_Firmware_SetByte(u32 address, u8 value)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BYTE_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = value;
}

static inline void NDSX_Firmware_SetShort(u32 address, u16 value)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BYTE_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = value;
}

static inline void NDSX_Firmware_SetLong(u32 address, u32 value)
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
    REG_IPC_FIFO_TX = GET_BYTE_VALUE;
    REG_IPC_FIFO_TX = address;
    REG_IPC_FIFO_TX = value;
}

/////////////////////
//
//  Mass write:
//

#endif // ARM9


/*
    ARM7 code.

    NOTES:
  - The following function must be called from within
    an ARM7-fifo-on-recv-interrupt-handler.
*/
#ifdef ARM7
void Read_FlashFast(u32 address, u8 *destination, u32 length) {
    u32 i;

    // Set SPI command
    //
    while(REG_SPICNT & SPI_BUSY);
    REG_SPICNT  = 0x8900;
    REG_SPIDATA = 0x0B;             // 0B = fast read, 03 = normal read
    while(REG_SPICNT & SPI_BUSY);

    // Command parameters
    // 
    REG_SPIDATA=(address>>16)&255;  // Part 1 of address
    while(REG_SPICNT & SPI_BUSY);
    REG_SPIDATA=(address>>8)&255;   // Part 2 of address
    while(REG_SPICNT & SPI_BUSY);
    REG_SPIDATA=(address)&255;      // Part 3 of address
    while(REG_SPICNT & SPI_BUSY);
    REG_SPIDATA=0;                  // Dummy write, only when using SPI{0B}
    while(REG_SPICNT & SPI_BUSY);

    for(i=0; i<length; i++) 
    {
        REG_SPIDATA = 0;
        while(REG_SPICNT & SPI_BUSY);
        destination[i] = REG_SPIDATA & 255;
    }

    REG_SPICNT = 0;
}

bool Write_Flash(u32 address, u8 *source, u32 length) {
    u32 i;
    u8 lock;

    if(length > 256) // write is 256size
        return false;

    // Set SPI command
    //
    while(REG_SPICNT & SPI_BUSY);
    REG_SPICNT  = 0x8900;
    REG_SPIDATA = 0x06;             // 06 = WRiteENable
    while(REG_SPICNT & SPI_BUSY);

    REG_SPICNT = 0;

    // Set SPI command
    //
    REG_SPICNT  = 0x8900;
    REG_SPIDATA = 0x05;             // 05 = ReaDStatusRegister
    while(REG_SPICNT & SPI_BUSY);

    lock = 0;
    while(lock == 0)
    {
        REG_SPIDATA = 0;
        while(REG_SPICNT & SPI_BUSY);
        lock = REG_SPIDATA & 2;
    }

    REG_SPICNT = 0;

    // Set SPI command
    //
    REG_SPICNT  = 0x8900;
    REG_SPIDATA = 0x0A;             // 0A = PageWrite
    while(REG_SPICNT & SPI_BUSY);

    // Command parameters
    // 
    REG_SPIDATA=(address>>16)&255;  // Part 1 of address
    while(REG_SPICNT & SPI_BUSY);
    REG_SPIDATA=(address>>8)&255;   // Part 2 of address
    while(REG_SPICNT & SPI_BUSY);
    REG_SPIDATA=(address)&255;      // Part 3 of address
    while(REG_SPICNT & SPI_BUSY);

    for(i=0; i<length; i++) 
    {
        REG_SPIDATA = source[i];
        while(REG_SPICNT & SPI_BUSY);
    }

    REG_SPICNT = 0;

    REG_SPICNT  = 0x8900;
    REG_SPIDATA = 0x05;             // 05 = ReaDStatusRegister
    while(REG_SPICNT & SPI_BUSY);

    lock = 1;
    while(lock != 0)
    {
        REG_SPIDATA = 0;
        while(REG_SPICNT & SPI_BUSY);
        lock = REG_SPIDATA & 1;
    }

    REG_SPICNT = 0;
    return true;
}

bool NDSX_ARM7_FirmwareAccessFifo(u32 msg)
{
    u32 addr = 0;
    u32 size = 0;
    u32 i=0;
    u8 block[512]; // maximum bytes

    if(msg == GET_BYTE_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        u8 value = 1;
        Read_FlashFast(addr, (u8*)&value, 1);
        while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
        REG_IPC_FIFO_TX = value;
        return true;
    }
    else if(msg == GET_SHORT_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        u16 value = 2;
        Read_FlashFast(addr, (u8*)&value, 2);
        while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
        REG_IPC_FIFO_TX = value;
        return true;
    }
    else if(msg == GET_LONG_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        u32 value = 4;
        Read_FlashFast(addr, (u8*)&value, 4);
        while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
        REG_IPC_FIFO_TX = value;
        return true;
    }
    else if(msg == GET_BLOCK_BYTE_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        size = REG_IPC_FIFO_RX;
        size = (size>512)?512:size;

        Read_FlashFast(addr, (u8*)block, size);

        for(i=0; i<size; i++) {
            while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
            REG_IPC_FIFO_TX = ((uint8*)block)[i];
        }
        return true;
    }
    else if(msg == GET_BLOCK_SHORT_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        size = REG_IPC_FIFO_RX;
        size = (size>256)?256:size;

        Read_FlashFast(addr, (u8*)block, size*2);

        for(i=0; i<size; i++) {
            while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
            REG_IPC_FIFO_TX = ((uint16*)block)[i];
        }
        return true;
    }
    else if(msg == GET_BLOCK_LONG_VALUE)
    {
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        addr = REG_IPC_FIFO_RX;
        while(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY); 
        size = REG_IPC_FIFO_RX;
        size = (size>128)?128:size;

        Read_FlashFast(addr, (u8*)block, size*4);

        for(i=0; i<size; i++) {
            while(!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)); 
            REG_IPC_FIFO_TX = ((uint32*)block)[i];
        }
        return true;
    }
    return false;
}
#endif // ARM7


#endif // _NDSX_FIRMWARE_
