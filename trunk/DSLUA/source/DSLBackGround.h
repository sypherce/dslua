#ifndef _DSLBACKGROUND_H_
#define _DSLBACKGROUND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Utils.h"

//------------------------------------------------------------
//----- BackGround base class
//------------------------------------------------------------
class BackGround
{
public:
enum           BGType {BG_3D, BG_LARGE, BG_EXT_ROTATE, BG_ROTATE, BG_BUFF8BIT, BG_TEXT, BG_KEYBOARD};

public:
const BGType m_bgType;

virtual int          initializeOnScreen(const int, const int) = 0;
virtual const char *toString();
virtual ~BackGround() = 0;

protected:
int m_nScreen;
int m_nPriority;

BackGround(const BGType bgType);

private:
BackGround();
};

//------------------------------------------------------------
//----- TextBackGround class
//------------------------------------------------------------
class TextBackGround : public BackGround
{
public:
TextBackGround();
~TextBackGround();

void setColor(const int);
void printXY(const int, const int, const char *);
void fillRectWithChar(const int, const int, const int, const int, const char *);
void eraseScreen();

const char * toString();
int         initializeOnScreen(const int, const int);
};



//------------------------------------------------------------
//----- KeyboardBackGround class
//------------------------------------------------------------
class KeyboardBackGround : public BackGround
{
public:
KeyboardBackGround();
~KeyboardBackGround();

void setColor(const int, const int);

const char * toString();
int         initializeOnScreen(const int, const int);
};

//------------------------------------------------------------
//----- Buff8BitBackGround class
//------------------------------------------------------------
class Buff8BitBackGround : public BackGround
{
public:
Buff8BitBackGround();
~Buff8BitBackGround();

void  setPaletteColor(const int, const int, const int, const int);
void  plot(const int, const int, const int);
int  getPixel(const int, const int);
void  line(const int, const int, const int, const int, const int);
void  clear();

const char * toString();
int         initializeOnScreen(const int, const int);
};

//------------------------------------------------------------
//----- TileBackGround class
//------------------------------------------------------------
class TileBackGround : public BackGround
{
public:
TileBackGround();
~TileBackGround();

const char * toString();
int         initializeOnScreen(const int, const int);

void        setPalette(AlignedMemory *);
void        setTiles(AlignedMemory *, const bool);
void        setMap(AlignedMemory *, const u32, const u32);
int         getMapTile(const s16, const s16);
void        setMapTile(const s16, const s16, const s16);

void        scrollX(const int);
void        scrollY(const int);
private:
AlignedMemory * m_amMapData;
AlignedMemory * m_amTileData;
AlignedMemory * m_amPaletteData;

u32 m_nWidth;
u32 m_nHeight;
u8 m_nBGSize;
bool m_bColorMode;

void  deletePalette();
void  deleteTiles();
void  deleteMap();
};

//-------------------------
//-------------------------
ExternDeclare(BackGround, BackGround *);
ExternDeclare(TextBackGround, TextBackGround *);
ExternDeclare(KeyboardBackGround, KeyboardBackGround *);
ExternDeclare(Buff8BitBackGround, Buff8BitBackGround *);
ExternDeclare(TileBackGround, TileBackGround *);

#define AVAILABLE_BGS   "TextBackGround", "KeyboardBackGround", "Buff8BitBackGround", "TileBackGround"

#ifdef __cplusplus
}          // extern "C"
#endif

#endif // _DSLBACKGROUND_H_
