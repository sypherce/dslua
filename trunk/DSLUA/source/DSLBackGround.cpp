#include <typeinfo>
#include <assert.h>
#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "DSFileIO.h"
#include "SimpleConsole.h"
#include "DSLBackGround.h"

//------------------------------------------------------------
//----- BackGround base class
//------------------------------------------------------------
UserdataStubs(BackGround, BackGround *);

BackGround::BackGround(const BGType bgType)
    :  m_bgType(bgType),
    m_nScreen(-1)
{
}

int BackGround::initializeOnScreen(const int nScreen, const int nBG)
{
    m_nScreen = nScreen;
    m_nPriority = nBG;
    return 0;
}

BackGround::~BackGround()
{
}

const char * BackGround::toString()
{
    return "__BACKGROUND__";
}

static int l_BackGroundGC(lua_State * lState)
{
    BackGround * *   pBGTemp = toBackGround(lState, 1);

    if(*pBGTemp)
    {
        delete (*pBGTemp);
        *pBGTemp = NULL;
    }
    return 0;
}

static int l_BackGroundToString(lua_State * lState)
{
    lua_pushstring(lState, (*toBackGround(lState, 1))->toString());
    return 1;
}

static const luaL_reg BackGround_methods[] = {
    {0, 0}
};
static const luaL_reg BackGround_meta[] = {
    {"__gc", l_BackGroundGC},
    {"__tostring", l_BackGroundToString},
    {0, 0}
};

UserdataRegister(BackGround, BackGround_methods, BackGround_meta);

//------------------------------------------------------------
//----- TextBackGround class
//------------------------------------------------------------
UserdataStubs(TextBackGround, TextBackGround *);

TextBackGround::TextBackGround()
    :  BackGround(BG_TEXT)
{
}

int TextBackGround::initializeOnScreen(const int nScreen, const int nBG)
{
    // init parent BG first
    BackGround::initializeOnScreen(nScreen, nBG);

    // init text mode
    PA_InitText(nScreen, nBG);
    return 0;
}

TextBackGround::~TextBackGround()
{
    // delete background
    if(m_nScreen >= 0)
    {
        PA_DeleteBg(m_nScreen, m_nPriority);
    }
}

const char * TextBackGround::toString()
{
    return "TextBackGround";
}

//------------------------------------------------------------
//------------------------------------------------------------
void TextBackGround::setColor(const int nColor)
{
    SimpleConsoleSetColor(m_nScreen, nColor);
}

//------------------------------------------------------------
//------------------------------------------------------------
void TextBackGround::printXY(const int nX, const int nY, const char * szString)
{
    // print the string at specified location
    PA_OutputSimpleText(m_nScreen, nX, nY, szString);
}

//------------------------------------------------------------
//------------------------------------------------------------
void TextBackGround::fillRectWithChar(const int nX1, const int nY1, const int nX2, const int nY2, const char * szString)
{
    SimpleConsoleFillRectWithChar(m_nScreen, nX1, nY1, nX2, nY2, szString[0]);
}

//------------------------------------------------------------
//------------------------------------------------------------
void TextBackGround::eraseScreen()
{
    SimpleConsoleClearScreem(m_nScreen);
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TBGSetTextColor(lua_State * lState)
{
    TextBackGround * *  ppTBG = checkTextBackGround(lState, 1);
    int nColor = luaL_checkint(lState, 2);

    (*ppTBG)->setColor(nColor);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TBGPrintAtScreenXY(lua_State * lState)
{
    // get all arguments
    TextBackGround * *  ppTBG = checkTextBackGround(lState, 1);
    int nX = luaL_checkint(lState, 2);
    int nY = luaL_checkint(lState, 3);
    const char * szString = luaL_checkstring(lState, 4);

    (*ppTBG)->printXY(nX, nY, szString);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TBGFillTextRectWithChar(lua_State * lState)
{
    // get all arguments
    TextBackGround * *  ppTBG = checkTextBackGround(lState, 1);
    int nX1 = luaL_checkint(lState, 2);
    int nY1 = luaL_checkint(lState, 3);
    int nX2 = luaL_checkint(lState, 4);
    int nY2 = luaL_checkint(lState, 5);
    const char * szString = luaL_checkstring(lState, 6);

    // print the string at specified location
    (*ppTBG)->fillRectWithChar(nX1, nY1, nX2, nY2, szString);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TBGEraseScreen(lua_State * lState)
{
    TextBackGround * *  ppTBG = checkTextBackGround(lState, 1);

    (*ppTBG)->eraseScreen();

    return 0;
}

static int l_TextBackGroundGC(lua_State * lState)
{
    TextBackGround * *   pBGTemp = toTextBackGround(lState, 1);

    if(*pBGTemp)
    {
        delete (*pBGTemp);
        *pBGTemp = NULL;
    }
    return 0;
}

static int l_TextBackGroundToString(lua_State * lState)
{
    lua_pushstring(lState, (*toTextBackGround(lState, 1))->toString());
    return 1;
}

static const luaL_reg TextBackGround_methods[] = {
    {"SetColor", l_TBGSetTextColor},
    {"PrintXY", l_TBGPrintAtScreenXY},
    {"FillRectWithChar", l_TBGFillTextRectWithChar},
    {"Clear", l_TBGEraseScreen},
    {0, 0}
};

static const luaL_reg TextBackGround_meta[] = {
    {"__gc", l_TextBackGroundGC},
    {"__tostring", l_TextBackGroundToString},
    {0, 0}
};

UserdataRegister(TextBackGround, TextBackGround_methods, TextBackGround_meta);

//------------------------------------------------------------
//----- Buff8BitBackGround class
//------------------------------------------------------------
UserdataStubs(Buff8BitBackGround, Buff8BitBackGround *);

Buff8BitBackGround::Buff8BitBackGround()
    :  BackGround(BG_BUFF8BIT)
{
}

int Buff8BitBackGround::initializeOnScreen(const int nScreen, const int nBG)
{
    // init parent BG first
    BackGround::initializeOnScreen(nScreen, nBG);

    // init 8bit mode
    PA_Init8bitBg(nScreen, nBG);
    return 0;
}

Buff8BitBackGround::~Buff8BitBackGround()
{
    // delete background
    if(m_nScreen >= 0)
    {
        PA_DeleteBg(m_nScreen, m_nPriority);
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void Buff8BitBackGround::setPaletteColor(const int nIndex, const int nR, const int nG, const int nB)
{
    PA_SetBgPalCol(m_nScreen, nIndex, PA_RGB(nR, nG, nB));
//   PA_SetBgPalNCol( m_nScreen, 3, 0, nIndex, PA_RGB( nR, nG, nB ) );
}

//------------------------------------------------------------
//------------------------------------------------------------
void Buff8BitBackGround::plot(const int nX, const int nY, const int nPaletteIndex)
{
    // Draws a pixel on screen
    PA_Put8bitPixel(m_nScreen, nX, nY, nPaletteIndex);
}

//------------------------------------------------------------
//------------------------------------------------------------
int Buff8BitBackGround::getPixel(const int nX, const int nY)
{
    // Draws a pixel on screen

    //return PA_Get8BitPixel(m_nScreen, nX, nY);
    u16 pos = (nX >> 1) + (nY << 7);

    if(nX & 1)
    {
        return(PA_DrawBg[m_nScreen][pos] >> 8);
    }
    else
    {
        return(PA_DrawBg[m_nScreen][pos]);
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void Buff8BitBackGround::line(const int nX1, const int nY1, const int nX2, const int nY2, const int nPaletteIndex)
{
    // Draws a pixel on screen
    PA_Draw8bitLine(m_nScreen, nX1, nY1, nX2, nY2, nPaletteIndex);
}

//------------------------------------------------------------
//------------------------------------------------------------
void Buff8BitBackGround::clear()
{
    // Draws a pixel on screen
    PA_Clear8bitBg(m_nScreen);
}

const char * Buff8BitBackGround::toString()
{
    return "Buff8BitBackGround";
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_8BGSetPaletteColor(lua_State * lState)
{
    Buff8BitBackGround * * ppTBG = checkBuff8BitBackGround(lState, 1);
    int nIndex = luaL_checkint(lState, 2);
    int nR = luaL_checkint(lState, 3);
    int nG = luaL_checkint(lState, 4);
    int nB = luaL_checkint(lState, 5);

    (*ppTBG)->setPaletteColor(nIndex, nR, nG, nB);

    return 0;
}

static int l_8BGPlot(lua_State * lState)
{
    Buff8BitBackGround * * ppTBG = checkBuff8BitBackGround(lState, 1);
    int nX = luaL_checkint(lState, 2);
    int nY = luaL_checkint(lState, 3);
    int nColor = luaL_checkint(lState, 4);

    (*ppTBG)->plot(nX, nY, nColor);

    return 0;
}

static int l_8BGGetPixel(lua_State * lState)
{
    Buff8BitBackGround * * ppTBG = checkBuff8BitBackGround(lState, 1);
    int nX = luaL_checkint(lState, 2);
    int nY = luaL_checkint(lState, 3);

    int retVal = (*ppTBG)->getPixel(nX, nY);

    lua_pushvalue(lState, retVal);
    return retVal;
}

static int l_8BGLine(lua_State * lState)
{
    Buff8BitBackGround * * ppTBG = checkBuff8BitBackGround(lState, 1);
    int nX1 = luaL_checkint(lState, 2);
    int nY1 = luaL_checkint(lState, 3);
    int nX2 = luaL_checkint(lState, 4);
    int nY2 = luaL_checkint(lState, 5);
    int nColor = luaL_checkint(lState, 6);

    (*ppTBG)->line(nX1, nY1, nX2, nY2, nColor);

    return 0;
}

static int l_8BGClear(lua_State * lState)
{
    Buff8BitBackGround * * ppTBG = checkBuff8BitBackGround(lState, 1);

    (*ppTBG)->clear();

    return 0;
}

static int l_Buff8BitBackGroundGC(lua_State * lState)
{
    Buff8BitBackGround * *   pBGTemp = toBuff8BitBackGround(lState, 1);

    if(*pBGTemp)
    {
        delete (*pBGTemp);
        *pBGTemp = NULL;
    }
    return 0;
}

static int l_Buff8BitBackGroundToString(lua_State * lState)
{
    lua_pushstring(lState, (*toBuff8BitBackGround(lState, 1))->toString());
    return 1;
}

static const luaL_reg Buff8BitBackGround_methods[] = {
    {"SetPaletteColor", l_8BGSetPaletteColor},
    {"Plot", l_8BGPlot},
    {"GetPixel", l_8BGGetPixel},
    {"Line", l_8BGLine},
    {"Clear", l_8BGClear},
    {0, 0}
};
static const luaL_reg Buff8BitBackGround_meta[] = {
    {"__gc", l_Buff8BitBackGroundGC},
    {"__tostring", l_Buff8BitBackGroundToString},
    {0, 0}
};

UserdataRegister(Buff8BitBackGround, Buff8BitBackGround_methods, Buff8BitBackGround_meta);

//------------------------------------------------------------
//----- TileBackGround class
//------------------------------------------------------------
UserdataStubs(TileBackGround, TileBackGround *);

TileBackGround::TileBackGround()
    :  BackGround(BG_BUFF8BIT),
    m_amMapData(NULL),
    m_amTileData(NULL),
    m_amPaletteData(NULL),
    m_nWidth(0),
    m_nHeight(0),
    m_nBGSize(0),
    m_bColorMode(false)
{
}

void TileBackGround::deletePalette()
{
    if(m_amPaletteData)
    {
        delete (m_amPaletteData);
        m_amPaletteData = NULL;
    }
}

void TileBackGround::deleteTiles()
{
    if(m_amTileData)
    {
        delete (m_amTileData);
        m_amTileData = NULL;
    }
    m_bColorMode = false;
}

void TileBackGround::deleteMap()
{
    if(m_amMapData)
    {
        delete (m_amMapData);
        m_amMapData = NULL;
    }
    m_nWidth = 0;
    m_nHeight = 0;
    m_nBGSize = 0;
}

void TileBackGround::setPalette(AlignedMemory * apPalData)
{
    if(apPalData)
    {
        // delete old palette data if there was one
        deletePalette();
        m_amPaletteData = apPalData;
    }
}

void TileBackGround::setTiles(AlignedMemory * apTileData, const bool bColorMode)
{
    if(apTileData)
    {
        // delete old tile data if there was one
        deleteTiles();
        m_amTileData = apTileData;
        m_bColorMode = bColorMode;
    }
}

void TileBackGround::setMap(AlignedMemory * apMapData, const u32 nWidth, const u32 nHeight)
{
    if(apMapData)
    {
        // delete old map data if there was one
        deleteMap();
        m_amMapData = apMapData;
        m_nWidth = nWidth;
        m_nHeight = nHeight;
        // TODO: use proper bg_size flag instead the one for LargeMap
        m_nBGSize = BG_512X256;
    }
}

int TileBackGround::getMapTile(const s16 nX, const s16 nY)
{
    // TODO: Large map func only
    u32 truex = nX & 31;
    u32 mapblock = (nX >> 5) << 11;     // Permet d'avoir le bon block...

    return *(unsigned short int *)(PA_BgInfo[m_nScreen][m_nPriority].Map + ((truex) << 1) + ((nY) << 6) + mapblock);
}

void TileBackGround::setMapTile(const s16 nX, const s16 nY, const s16 nTileInfo)
{
    // TODO: Large map func only
    PA_SetLargeMapTile(m_nScreen, m_nPriority, nX, nY, nTileInfo);
}

int TileBackGround::initializeOnScreen(const int nScreen, const int nBG)
{
    // init parent BG first
    BackGround::initializeOnScreen(nScreen, nBG);

    // init palette/tile/map
    waitUntilDMAFree(3);
    PA_LoadBgPal(nScreen, nBG, m_amPaletteData->vpAlignedMem);
    waitUntilDMAFree(3);
    PA_LoadBgTilesEx(nScreen, nBG, m_amTileData->vpAlignedMem, ((m_amTileData->m_nSize) >> 1));
    waitUntilDMAFree(3);
    PA_LoadBgMap(nScreen, nBG, m_amMapData->vpAlignedMem, m_nBGSize);
    waitUntilDMAFree(3);

    // init tile BG
    PA_InitBg(nScreen, nBG, m_nBGSize, 0, m_bColorMode);
    PA_BGScrollXY(nScreen, nBG, 0, 0);
    PA_InitLargeBg(nScreen, nBG, m_nWidth, m_nHeight, m_amMapData->vpAlignedMem);

    return 0;
}

void TileBackGround::scrollX(const int nX)
{
    PA_LargeScrollX(m_nScreen, m_nPriority, nX);
}

void TileBackGround::scrollY(const int nY)
{
    PA_LargeScrollY(m_nScreen, m_nPriority, nY);
}

TileBackGround::~TileBackGround()
{
    // delete all assets
    deleteMap();
    deleteTiles();
    deletePalette();

    // delete background
    if(m_nScreen >= 0)
    {
        PA_DeleteBg(m_nScreen, m_nPriority);
    }
}

const char * TileBackGround::toString()
{
    return "TileBackGround";
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileLoadPalette(lua_State * lState)
{
    TileBackGround * *  ppTBG = checkTileBackGround(lState, 1);
    const char *       szFName = luaL_checkstring(lState, 2);
    unsigned int nSize;

    // open palette file in binary mode
    DS_FILE * dsfPal = DS_fopen(szFName, "rb");

    if(NULL == dsfPal)
    {
        return luaL_error(lState, "Failed to open palette file '%s'", szFName);
    }

    // check file size
    nSize = DS_getFileSize(dsfPal);
    if((nSize < 1) || (nSize % 2))
    {
        DS_fclose(dsfPal);
        return luaL_error(lState, "Incorrect palette file size: %d", nSize);
    }

    // load palette into memroy
    AlignedMemory *    amMem = new AlignedMemory(5, nSize);
    char * cbTemp = (char *)(amMem->vpAlignedMem);
    size_t nRead;

#ifdef DEBUG
    char szTemp[64];
    sprintf(szTemp, "TP:%p (%p)", cbTemp, amMem->vpRealMem);
    SimpleConsolePrintStringCR(SCREEN_TOP, szTemp);
#endif //DEBUG

    // make sure we have a valid pointer
    if(NULL == cbTemp)
    {
        DS_fclose(dsfPal);
        return luaL_error(lState, "Unable to allocate %d bytes for tile palette memory", nSize);
    }

    // init the memory acquired and read in the data
    nRead = DS_fread(cbTemp, 1, nSize, dsfPal);

    // make sure we read in something
    if(nRead != nSize)
    {
        delete (amMem);
        DS_fclose(dsfPal);
        return luaL_error(lState, "Can only read in %d bytes of tile palette data", nRead);
    }

    // set palette
    (*ppTBG)->setPalette(amMem);

    // close the file when we are done
    DS_fclose(dsfPal);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileLoadTiles(lua_State * lState)
{
    TileBackGround * * ppTBG = checkTileBackGround(lState, 1);
    const char *    szFName = luaL_checkstring(lState, 2);
    const int nMode = luaL_checkint(lState, 3);
    unsigned int nSize;
    bool bColorMode;

    // validate parameters
    if((16 != nMode) && (256 != nMode))
    {
        return luaL_error(lState, "Tiles can only be 16 colors or 256 colors.  Not %d", nMode);
    }
    else
    {
        bColorMode = (256 == nMode);
    }

    // open tile file in binary mode
    DS_FILE * dsfTile = DS_fopen(szFName, "rb");
    if(NULL == dsfTile)
    {
        return luaL_error(lState, "Failed to open tile file '%s'", szFName);
    }

    // load tile into memroy
    nSize = DS_getFileSize(dsfTile);
    AlignedMemory *    amMem = new AlignedMemory(5, nSize);
    char * cbTemp = (char *)(amMem->vpAlignedMem);
    size_t nRead;

#ifdef DEBUG
    char szTemp[64];
    sprintf(szTemp, "TT:%p (%p)", cbTemp, amMem->vpRealMem);
    SimpleConsolePrintStringCR(SCREEN_TOP, szTemp);
#endif //DEBUG

    // make sure we have a valid pointer
    if(NULL == cbTemp)
    {
        DS_fclose(dsfTile);
        return luaL_error(lState, "Unable to allocate %d bytes for tile tile memory", nSize);
    }

    // init the memory acquired and read in the data
    nRead = DS_fread(cbTemp, 1, nSize, dsfTile);

    // make sure we read in something
    if(nRead != nSize)
    {
        delete (amMem);
        DS_fclose(dsfTile);
        return luaL_error(lState, "Can only read in %d bytes of tile tile data", nRead);
    }

    // set tile
    (*ppTBG)->setTiles(amMem, bColorMode);

    // close the file when we are done
    DS_fclose(dsfTile);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileLoadMap(lua_State * lState)
{
    TileBackGround * * ppTBG = checkTileBackGround(lState, 1);
    const char *    szFName = luaL_checkstring(lState, 2);
    const int nWidth = luaL_checkint(lState, 3);
    const int nHeight = luaL_checkint(lState, 4);
    unsigned int nSize;

    // open map file in binary mode
    DS_FILE * dsfMap = DS_fopen(szFName, "rb");

    if(NULL == dsfMap)
    {
        return luaL_error(lState, "Failed to open map file '%s'", szFName);
    }

    // load map into memroy
    nSize = DS_getFileSize(dsfMap);
    AlignedMemory *    amMem = new AlignedMemory(5, nSize);
    char * cbTemp = (char *)(amMem->vpAlignedMem);
    size_t nRead;

#ifdef DEBUG
    char szTemp[64];
    sprintf(szTemp, "TM:%p (%p)", cbTemp, amMem->vpRealMem);
    SimpleConsolePrintStringCR(SCREEN_TOP, szTemp);
#endif //DEBUG

    // make sure we have a valid pointer
    if(NULL == cbTemp)
    {
        DS_fclose(dsfMap);
        return luaL_error(lState, "Unable to allocate %d bytes for tile map memory", nSize);
    }

    // init the memory acquired and read in the data
    nRead = DS_fread(cbTemp, 1, nSize, dsfMap);

    // make sure we read in something
    if(nRead != nSize)
    {
        delete (amMem);
        DS_fclose(dsfMap);
        return luaL_error(lState, "Can only read in %d bytes of tile map data", nRead);
    }

    // set map
    (*ppTBG)->setMap(amMem, nWidth, nHeight);

    // close the file when we are done
    DS_fclose(dsfMap);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileScrollXY(lua_State * lState)
{
    TileBackGround * * ppTBG = checkTileBackGround(lState, 1);
    const int nNewX = luaL_checkint(lState, 2);
    const int nNewY = luaL_checkint(lState, 3);

    (*ppTBG)->scrollX(nNewX);
    (*ppTBG)->scrollY(nNewY);

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileGetMapTile(lua_State * lState)
{
    TileBackGround * * ppTBG = checkTileBackGround(lState, 1);
    const int nX = luaL_checkint(lState, 2);
    const int nY = luaL_checkint(lState, 3);

    int retVal = (*ppTBG)->getMapTile(nX, nY);

    lua_pushvalue(lState, retVal);

    return retVal;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TileSetMapTile(lua_State * lState)
{
    TileBackGround * * ppTBG = checkTileBackGround(lState, 1);
    const int nX = luaL_checkint(lState, 2);
    const int nY = luaL_checkint(lState, 3);
    const int nTileInf = luaL_checkint(lState, 4);

    (*ppTBG)->setMapTile(nX, nY, nTileInf);

    return 0;
}

static int l_TileBackGroundGC(lua_State * lState)
{
    TileBackGround * *   pBGTemp = toTileBackGround(lState, 1);

    if(*pBGTemp)
    {
        delete (*pBGTemp);
        *pBGTemp = NULL;
    }
    return 0;
}

static int l_TileBackGroundToString(lua_State * lState)
{
    lua_pushstring(lState, (*toTileBackGround(lState, 1))->toString());
    return 1;
}

static const luaL_reg TileBackGround_methods[] = {
    {"LoadPalette", l_TileLoadPalette},
    {"LoadTiles", l_TileLoadTiles},
    {"LoadMap", l_TileLoadMap},
    {"GetMapTile", l_TileGetMapTile},
    {"SetMapTile", l_TileSetMapTile},
    {"ScrollXY", l_TileScrollXY},
    {0, 0}
};
static const luaL_reg TileBackGround_meta[] = {
    {"__gc", l_TileBackGroundGC},
    {"__tostring", l_TileBackGroundToString},
    {0, 0}
};

UserdataRegister(TileBackGround, TileBackGround_methods, TileBackGround_meta);


