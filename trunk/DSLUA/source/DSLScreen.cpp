#include <typeinfo>
#include <assert.h>
#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "SimpleConsole.h"
#include "DSLBackGround.h"
#include "DSLSprite.h"
#include "DSLScreen.h"

//------------------------------------------------------------
//------------------------------------------------------------
void screenWaitForVBL()
{
    PA_WaitForVBL();
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_createTextBackGround(lua_State * lState)
{
    TextBackGround * *   ppbgTemp = NULL;

    // get Lua to create an user data item first
    ppbgTemp = pushTextBackGround(lState);

    // now initialize it
    *ppbgTemp = new TextBackGround();

    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_createBuff8BitBackGround(lua_State * lState)
{
    Buff8BitBackGround * *   ppbgTemp = NULL;

    // get Lua to create an user data item first
    ppbgTemp = pushBuff8BitBackGround(lState);

    // now initialize it
    *ppbgTemp = new Buff8BitBackGround();

    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_createTileBackGround(lua_State * lState)
{
    TileBackGround * *   ppbgTemp = NULL;

    // get Lua to create an user data item first
    ppbgTemp = pushTileBackGround(lState);

    // now initialize it
    *ppbgTemp = new TileBackGround();

    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_ResetGraphics(lua_State * lState)
{
    resetAllGraphics();
    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_WaitForVBL(lua_State * lState)
{
    screenWaitForVBL();
    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool isBackGroundAvailable(lua_State * L, int nIndex)
{
    unsigned int nLoop;
    void * ud;
    char * saBGNames[] = {AVAILABLE_BGS};

    for(nLoop = 0; nLoop < countof(saBGNames); ++nLoop)
    {
        ud = luaL_checkudata(L, nIndex, saBGNames[ nLoop]);
        if(NULL != ud)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
void resetAllGraphics()
{
    // reset everything
    Sprite::freeAllSprites();
    PA_ResetSpriteSys();
#ifndef DEBUG
    PA_ResetBgSys();

    // attempting to clear VRAM
    memset(VRAM_A, 0, 0x20000);
    memset(VRAM_B, 0, 0x40000);
    memset(VRAM_C, 0, 0x60000);
    memset(VRAM_D, 0, 0x80000);
    memset(VRAM_E, 0, 0x90000);
    memset(VRAM_F, 0, 0x94000);
    memset(VRAM_G, 0, 0x98000);
    memset(VRAM_H, 0, 0xA0000);
    memset(VRAM_I, 0, 0x98000);
#endif
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_ScreenInitialize(lua_State * lState)
{
//SimpleConsolePrintString( SCREEN_BOTTOM, "Check args" );
// make sure we have the correct number of arguments
    int nArgc = lua_gettop(lState);

    if((nArgc < 2) || (nArgc > 5))
    {
        return luaL_error(lState, "wrong number of arguments");
    }

    // process all backgrounds
    int nScreen = luaL_checkint(lState, 1);
    int nBGLeft = nArgc - 1;

    int nLoop;
    int nIndex, nCurrentMin;
    bool baInit[] = {false, false, false, false, false};

    // make sure all other params are BackGrounds
    for(nLoop = 0; nLoop < nBGLeft; ++nLoop)
    {
        if(!isBackGroundAvailable(lState, 2 + nLoop))
        {
            luaL_argcheck(lState, false, 2 + nLoop, "Invalid background type");
        }
    }

    // loop through and init each BG with priority
    BackGround * *   ppBGTemp = NULL;
//   PA_ResetBgSys();
    while(1)
    {
        nIndex = -1;
        nCurrentMin = 999;
        // find the smallest
        for(nLoop = 2; nLoop <= nArgc; ++nLoop)
        {
            if(!baInit[ nLoop - 2 ])
            {
                ppBGTemp = toBackGround(lState, nLoop);
                if((*ppBGTemp) && ((*ppBGTemp)->m_bgType < nCurrentMin))
                {
                    nIndex = nLoop;
                    nCurrentMin = (*ppBGTemp)->m_bgType;
                }
            }
        }

        // found the smallest BG type, init it first
        assert(nIndex >= 0);
        ppBGTemp = toBackGround(lState, nIndex);
        (*ppBGTemp)->initializeOnScreen(nScreen, nIndex - 2);
        baInit[ nIndex - 2 ] = true;
        nBGLeft--;
        if(nBGLeft <= 0)
        {
            break;
        }
    }
//SimpleConsolePrintString( SCREEN_BOTTOM, "DoneInit" );
//waitVBLDelay( 1000 );

    return 0;
}

static const struct luaL_reg DSLScreenLib [] =
{
    {"WaitForVBL", l_WaitForVBL},
    {"LoadTextBG", l_createTextBackGround},
    {"Load8BitBG", l_createBuff8BitBackGround},
    {"LoadTileBG", l_createTileBackGround},
    {"Initialize", l_ScreenInitialize},
    {"ResetAll", l_ResetGraphics},
//   { "Capture", l_ScreenCap },
    {NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLScreenLib(lua_State * lState)
{
    // register the background library
//   BackGround_register( lState );
    TextBackGround_register(lState);
    Buff8BitBackGround_register(lState);
    TileBackGround_register(lState);

    // register other graphics library
    FrameStrip_register(lState);
    Sprite_register(lState);

    luaL_openlib(lState, "Screen", DSLScreenLib, 0);

    return 1;
}

//#include "gba_nds_fat.h"
//
//typedef struct {
//	unsigned short int type;                 /* Magic identifier            */
//	unsigned int size;                       /* File size in bytes          */
//	unsigned short int reserved1, reserved2;
//	unsigned int offset;                     /* Offset to image data, bytes */
//} PACKED HEADER;
//
//typedef struct {
//	unsigned int size;               /* Header size in bytes      */
//	int width,height;                /* Width and height of image */
//	unsigned short int planes;       /* Number of colour planes   */
//	unsigned short int bits;         /* Bits per pixel            */
//	unsigned int compression;        /* Compression type          */
//	unsigned int imagesize;          /* Image size in bytes       */
//	int xresolution,yresolution;     /* Pixels per meter          */
//	unsigned int ncolours;           /* Number of colours         */
//	unsigned int importantcolours;   /* Important colours         */
//} PACKED INFOHEADER;
//
////------------------------------------------------------------
////------------------------------------------------------------
//void write16(u16* address, u16 value)
//{
//	u8* first=(u8*)address;
//	u8* second=first+1;
//
//	*first=value&0xff;
//	*second=value>>8;
//}
//
////------------------------------------------------------------
////------------------------------------------------------------
//void write32(u32* address, u32 value)
//{
//	u8* first=(u8*)address;
//	u8* second=first+1;
//	u8* third=first+2;
//	u8* fourth=first+3;
//
//	*first=value&0xff;
//	*second=(value>>8)&0xff;
//	*third=(value>>16)&0xff;
//	*fourth=(value>>24)&0xff;
//}
//
////------------------------------------------------------------
////------------------------------------------------------------
//void screenshot(u8* buffer)
//{
//   // set up copy to VRAM_A
//	u8 vram_cr_temp   = VRAM_A_CR;
//	VRAM_A_CR         = VRAM_A_LCD;
//
//	u8* vram_temp     = (u8*) malloc( 128*1024 );
//	dmaCopy( VRAM_A, vram_temp, 128*1024 );
//
//	DISP_CAPTURE=DCAP_BANK(0)|DCAP_ENABLE|DCAP_SIZE(3);
//	while(DISP_CAPTURE & DCAP_ENABLE);
//
//	dmaCopy(VRAM_A, buffer, 256*192*2 );
//	dmaCopy(vram_temp, VRAM_A, 128*1024);
//
//	VRAM_A_CR=vram_cr_temp;
//
//	free(vram_temp);
//}
//
////------------------------------------------------------------
////------------------------------------------------------------
//void screenshotbmp2( const char* filename )
//{
//	FAT_InitFiles();
//	FAT_FILE* file=FAT_fopen(filename, "w");
//
//   // perform screen cap
//	u8* buffer  =(u8*)malloc( 256*192*2 );
//   screenshot( buffer );
//
//   // build header
//	u8* temp    =(u8*)malloc( 256*192*3+sizeof(INFOHEADER)+sizeof(HEADER) );
//
//	HEADER* header=(HEADER*)temp;
//	INFOHEADER* infoheader=(INFOHEADER*)(temp+sizeof(HEADER));
//
//	write16(&header->type, 0x4D42);
//	write32(&header->size, 256*192*3+sizeof(INFOHEADER)+sizeof(HEADER));
//	write32(&header->offset, sizeof(INFOHEADER)+sizeof(HEADER));
//	write16(&header->reserved1, 0);
//	write16(&header->reserved2, 0);
//
//	write16(&infoheader->bits, 24);
//	write32(&infoheader->size, sizeof(INFOHEADER));
//	write32(&infoheader->compression, 0);
//	write32((u32*)&infoheader->width, 256);
//	write32((u32*)&infoheader->height, 192);
//	write16(&infoheader->planes, 1);
//	write32((u32*)&infoheader->imagesize, 256*192*3);
//	write32((u32*)&infoheader->xresolution, 0);
//	write32((u32*)&infoheader->yresolution, 0);
//	write32((u32*)&infoheader->importantcolours, 0);
//	write32((u32*)&infoheader->ncolours, 0);
//
//	for(int y=0;y<192;y++)
//	{
//		for(int x=0;x<256;x++)
//		{
//			u16 color=buffer[256*192-y*256+x];
//
//			u8 b=(color&31)<<3;
//			u8 g=((color>>5)&31)<<3;
//			u8 r=((color>>10)&31)<<3;
//
//			temp[((y*256)+x)*3+sizeof(INFOHEADER)+sizeof(HEADER)]=r;
//			temp[((y*256)+x)*3+1+sizeof(INFOHEADER)+sizeof(HEADER)]=g;
//			temp[((y*256)+x)*3+2+sizeof(INFOHEADER)+sizeof(HEADER)]=b;
//		}
//	}
//
//	DC_FlushAll();
//	FAT_fwrite(temp, 1, 256*192*3+sizeof(INFOHEADER)+sizeof(HEADER), file);
//	FAT_fclose(file);
//	free(buffer);
//	free(temp);
//}
//
////------------------------------------------------------------
////------------------------------------------------------------
//void screenshotbmp( const char* filename )
//{
//	FAT_InitFiles();
//	FAT_FILE* file=FAT_fopen(filename, "w");
//
////	u8 vram_cr_temp   = VRAM_A_CR;
////	VRAM_A_CR         = VRAM_A_LCD;
////	DISP_CAPTURE   = DCAP_BANK(1)| DCAP_ENABLE|DCAP_SIZE(3);
//   DISP_CAPTURE   =
//      DCAP_ENABLE |
//      DCAP_MODE(2) |	// Capture Source    (0=Source A, 1=Source B, 2/3=Sources A+B blended)
//      DCAP_DST(0) |	// VRAM Write Offset (0=00000h, 0=08000h, 0=10000h, 0=18000h)
//      DCAP_SRC(0) |	// Source A          (0=Graphics Screen BG+3D+OBJ, 1=3D Screen)
//      DCAP_SIZE(3) |	// Capture Size      (0=128x128, 1=256x64, 2=256x128, 3=256x192 dots)
//      DCAP_OFFSET(0)|	// VRAM Read Offset  (0=00000h, 0=08000h, 0=10000h, 0=18000h)
//      DCAP_BANK(2) |	// VRAM Write Block  (0..3 = VRAM A..D) (VRAM must be allocated to LCDC)
//      DCAP_B(0) |	// EVB (0..16, or 0..31 ?) Blending Factor for Source A (or B ?)
//      DCAP_A(14);	// EVA (0..16, or 0..31 ?) Blending Factor for Source B (or A ?)
//	while(DISP_CAPTURE & DCAP_ENABLE);
//
//	u8* temp=(u8*)malloc(256*192*3+sizeof(INFOHEADER)+sizeof(HEADER));
//
//	HEADER* header=(HEADER*)temp;
//	INFOHEADER* infoheader=(INFOHEADER*)(temp+sizeof(HEADER));
//
//	write16(&header->type, 0x4D42);
//	write32(&header->size, 256*192*3+sizeof(INFOHEADER)+sizeof(HEADER));
//	write32(&header->offset, sizeof(INFOHEADER)+sizeof(HEADER));
//	write16(&header->reserved1, 0);
//	write16(&header->reserved2, 0);
//
//	write16(&infoheader->bits, 24);
//	write32(&infoheader->size, sizeof(INFOHEADER));
//	write32(&infoheader->compression, 0);
//	write32((u32*)&infoheader->width, 256);
//	write32((u32*)&infoheader->height, 192);
//	write16(&infoheader->planes, 1);
//	write32((u32*)&infoheader->imagesize, 256*192*3);
//	write32((u32*)&infoheader->xresolution, 0);
//	write32((u32*)&infoheader->yresolution, 0);
//	write32((u32*)&infoheader->importantcolours, 0);
//	write32((u32*)&infoheader->ncolours, 0);
//
//	for(int y=0;y<192;y++)
//	{
//		for(int x=0;x<256;x++)
//		{
//			u16 color=VRAM_C[256*192-y*256+x];
//
//			u8 b=(color&31)<<3;
//			u8 g=((color>>5)&31)<<3;
//			u8 r=((color>>10)&31)<<3;
//
//			temp[((y*256)+x)*3+sizeof(INFOHEADER)+sizeof(HEADER)]=r;
//			temp[((y*256)+x)*3+1+sizeof(INFOHEADER)+sizeof(HEADER)]=g;
//			temp[((y*256)+x)*3+2+sizeof(INFOHEADER)+sizeof(HEADER)]=b;
//		}
//	}
//	DC_FlushAll();
//	FAT_fwrite(temp, 1, 256*192*3+sizeof(INFOHEADER)+sizeof(HEADER), file);
//	FAT_fclose(file);
//	free(temp);
//}
//
////------------------------------------------------------------
////------------------------------------------------------------
//static int l_ScreenCap( lua_State* lState )
//{
//   const char* szFileName  = luaL_checkstring( lState, 1 );
//   screenshotbmp2( szFileName );
//   return 1;
//}
//

