#ifndef _DSLSPRITE_H_
#define _DSLSPRITE_H_

#include <vector>

typedef std::vector<u16> GFXS;

#define  HIDDEN_SPRITE_X   256
#define  HIDDEN_SPRITE_Y   192
#define  MAX_SPRITE_NUM    128

//------------------------------------------------------------
//----- FrameStrip class
//------------------------------------------------------------
class FrameStrip
{
public:
FrameStrip(int, int, int, int);
~FrameStrip();

static int  getFrameSize(int, int, int);
static int  getParamIndex(int, int);

const unsigned int   getTotalFrames();
const int            getFrameSize();
const u16            getFrameGfx(unsigned int);
const u8             getScreen()
{
    return m_nScreen;
};
const u8             getWidth()
{
    return m_nWidth;
};
const u8             getHeight()
{
    return m_nHeight;
};
const u8             getMode()
{
    return m_nMode;
};
const int            getParamIndex()
{
    return m_nParamIndex;
};

bool           addFrame(char *);
void           freeAllFrames();

const char * toString();
private:
FrameStrip();
u8 m_nScreen;
u8 m_nWidth;
u8 m_nHeight;
u8 m_nMode;
int m_nParamIndex;
unsigned int m_nFrames;
GFXS m_gfxs;
};

//------------------------------------------------------------
//----- Sprite class
//------------------------------------------------------------
class Sprite
{
public:
Sprite(FrameStrip *, unsigned int, u8);
~Sprite();

void  hide();
void  setFrame(FrameStrip *, unsigned int);
void  setXY(s16, s16);
void  free();

const char * toString();
static void freeAllSprites();

private:
Sprite();

u8 m_nScreen;
u8 m_nMode;
u16 m_nX;
u16 m_nY;
u8 m_nSpriteObj;
u8 m_nWidth;
u8 m_nHeight;

static int m_nFreeIndex;
static u8 m_naNextAvailableSprite[ MAX_SPRITE_NUM + 1 ];

static u8   getNextAvailableSprite();
static void freeSprite(u8);
};

//-------------------------
//-------------------------
ExternDeclare(FrameStrip, FrameStrip *);
ExternDeclare(Sprite, Sprite *);


#endif   // _DSLSPRITE_H_

