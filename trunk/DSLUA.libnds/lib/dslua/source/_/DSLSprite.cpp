#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include <assert.h>
}

#include "Utils.h"
#include "DSFileIO.h"
#include "DSLSprite.h"
#include "SimpleConsole.h"

struct _SpriteParams {
   u8    nWidth;
   u8    nHeight;
   u8    nParams[2];
} ValidSpriteParams[] = {
   {  8,  8, { 0, 0 } },
   { 16, 16, { 0, 1 } },
   { 32, 32, { 0, 2 } },
   { 64, 64, { 0, 3 } },
   { 16,  8, { 1, 0 } },
   { 32,  8, { 1, 1 } },
   { 32, 16, { 1, 2 } },
   { 64, 32, { 1, 3 } },
   {  8, 16, { 2, 0 } },
   {  8, 32, { 2, 1 } },
   { 16, 32, { 2, 2 } },
   { 32, 64, { 2, 3 } }
};

//------------------------------------------------------------
//----- FrameStrip class
//------------------------------------------------------------
UserdataStubs( FrameStrip, FrameStrip* );

FrameStrip::FrameStrip( int nScreen, int nWidth, int nHeight, int nMode )
:  m_nScreen( nScreen ),
   m_nWidth( nWidth ),
   m_nHeight( nHeight ),
   m_nMode( nMode ),
   m_nParamIndex( FrameStrip::getParamIndex( nWidth, nHeight ) ),
   m_nFrames( 0 ),
   m_gfxs( 16 )
{
}

FrameStrip::FrameStrip()
:  m_nScreen( 0 ),
   m_nWidth( 0 ),
   m_nHeight( 0 ),
   m_nMode( 0 ),
   m_nParamIndex( -1 ),
   m_nFrames( 0 ),
   m_gfxs( 4 )
{
}

//------------------------------------------------------------
// return the number of bytes required to store each frame
//------------------------------------------------------------
int FrameStrip::getFrameSize( int nWidth, int nHeight, int nMode )
{
   int nFrameSize  = ( ( nWidth * nHeight ) >> ( 1 - nMode ) );
   return nFrameSize;
}

//------------------------------------------------------------
// return the index to the obj shape/size array base on
// width and height
//------------------------------------------------------------
int FrameStrip::getParamIndex( int nWidth, int nHeight )
{
   int   nLoop,
         nCount   = countof( ValidSpriteParams ),
         nParam   = -1;
   for ( nLoop=0; nLoop<nCount; nLoop++ ) {
      if ( ( ValidSpriteParams[ nLoop ].nWidth == nWidth ) && ( ValidSpriteParams[ nLoop ].nHeight == nHeight ) ) {
         nParam   = nLoop;
         break;
      }
   }
   return nParam;
}

//------------------------------------------------------------
// return the number of bytes required to store each frame
// for this strip
//------------------------------------------------------------
const int FrameStrip::getFrameSize()
{
   return getFrameSize( m_nWidth, m_nHeight, m_nMode );
}

//------------------------------------------------------------
// return the number of frames stored in this graphics strip
//------------------------------------------------------------
const unsigned int FrameStrip::getTotalFrames()
{
   return m_nFrames;
}

//------------------------------------------------------------
// return the number of frames stored in this graphics strip
//------------------------------------------------------------
const u16 FrameStrip::getFrameGfx( unsigned int nIndex )
{
   if ( nIndex >= m_nFrames ) {
      return 0xFFFF;
   }
   return m_gfxs[ nIndex ];
}

//------------------------------------------------------------
// copy the sprite data into VRAM and keep track of the
// gfx index
//------------------------------------------------------------
bool FrameStrip::addFrame( char *cbTemp )
{
   int   nParam   = getParamIndex( m_nWidth, m_nHeight );
   if ( nParam >= 0 ) {
      waitUntilDMAFree( 3 );
      u16   gfx0     = PA_CreateGfx( m_nScreen, cbTemp, ValidSpriteParams[nParam].nParams[0], ValidSpriteParams[nParam].nParams[1], m_nMode );
      waitUntilDMAFree( 3 );
//assert( gfx0 != 0xFFFF );
      m_gfxs[ m_nFrames ]  = gfx0;
      ++m_nFrames;
      return true;
   }
   return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
void FrameStrip::freeAllFrames()
{
   for ( ; m_nFrames > 0; ) {
      --m_nFrames;
      waitUntilDMAFree( 3 );
      PA_DeleteGfx( m_nScreen, m_gfxs[ m_nFrames ] );
      waitUntilDMAFree( 3 );
      m_gfxs[ m_nFrames ]   = 0xFFFF;
   }
}

//------------------------------------------------------------
// destructor
//------------------------------------------------------------
FrameStrip::~FrameStrip()
{
}

const char* FrameStrip::toString()
{
   return "FrameStrip";
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_FrameStripCreate( lua_State* lState )
{
   const int      nScreen  = luaL_checkint( lState, 1 );
   const int      nWidth   = luaL_checkint( lState, 2 );
   const int      nHeight  = luaL_checkint( lState, 3 );
   const char     *szMode  = luaL_checkstring( lState, 4 );
   unsigned int   nParam;

   // check parameters
   if ( ( nScreen < 0 ) || ( nScreen > 1 ) ) {
      return luaL_error( lState, "Use 0 for bottom screen and 1 for top screen" );
   }

   // check to make sure the width/height passed in is valid sprite size
   nParam   = FrameStrip::getParamIndex( nWidth, nHeight );
   if ( nParam < 0 ) {
      return luaL_error( lState, "Invalid sprite width(%d) or height(%d)", nWidth, nHeight );
   }

   // check sprite color mode
   int   nMode = -1;
   if ( !strcmp( szMode, "16" ) ) {
      nMode = 0;
   }
   if ( !strcmp( szMode, "256" ) ) {
      nMode = 1;
   }
   if ( !stricmp( szMode, "16bit" ) ) {
      nMode = 2;
   }
   if ( nMode< 0 ) {
      return luaL_error( lState, "Invalid sprite color mode (%s)", szMode );
   }

   // create a new FrameStrip object
   FrameStrip**  ppssTemp = NULL;
   ppssTemp = pushFrameStrip( lState );
   *ppssTemp   = new FrameStrip( nScreen, nWidth, nHeight, nMode );

   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_FrameStripLoadBin( lua_State* lState )
{
	int            nArgC       = lua_gettop( lState );
   FrameStrip     **ppStrip   = checkFrameStrip( lState, 1 );
   const char     *szFName    = luaL_checkstring( lState, 2 );
   int            nLoop, nFrames  = 1;
   unsigned int   nSize, nWanted, nFrameSize;

   // check parameters
   if ( ( NULL == ppStrip ) || ( NULL == *ppStrip ) ) {
      return luaL_error( lState, "Invalid FrameStrip object" );
   }

   // get optional param
   if ( nArgC > 2 ) {
      nFrames  = luaL_checkint( lState, 3 );
      if ( nFrames <= 0 ) {
         return luaL_error( lState, "Number of frames has to be at least 1" );
      }
   }

   // open palette file in binary mode
   DS_FILE  *dsfImg  = DS_fopen( szFName, "rb" );
   if ( NULL == dsfImg ) {
      return luaL_error( lState, "Failed to open image file '%s'", szFName );
   }

   // load image into memroy
   nSize       = DS_getFileSize( dsfImg );
   nFrameSize  = ( *ppStrip )->getFrameSize();
   nWanted     = nFrames * nFrameSize;

   // make sure we have enough frame data
   if ( nSize < nWanted ) {
      DS_fclose( dsfImg );
      return luaL_error( lState, "Image file contains less than %d frames of sprite", nWanted );
   }

   // allocate memory to read frame data
//   char  *cbTemp     = ( char* )malloc( nFrameSize );
   AlignedMemory*    amMem = new AlignedMemory( 5, nFrameSize );
   char  *cbTemp     = ( char* )( amMem->vpAlignedMem );

#ifdef DEBUG
   char  szTemp[64];
   sprintf( szTemp, "F:%p (%p)", cbTemp, amMem->vpRealMem );
   SimpleConsolePrintStringCR( SCREEN_TOP, szTemp );
#endif // DEBUG

   // make sure we have a valid pointer
   if ( NULL == cbTemp ) {
      DS_fclose( dsfImg );
      return luaL_error( lState, "Unable to allocate %d bytes for sprite image data", nFrameSize );
   }

   // now loop, and read in all frames
   size_t   nRead;
   for ( nLoop = 0; nLoop < nFrames; ++nLoop ) {
      // read in a single frame of bin data
//      memset( cbTemp, 0, nFrameSize );
      nRead = DS_fread( cbTemp, 1, nFrameSize, dsfImg );

      // make sure we read in something
      if ( nRead != nFrameSize ) {
//         free( cbTemp );
         delete( amMem );
         DS_fclose( dsfImg );
         return luaL_error( lState, "Can only read in %d bytes of sprite data for frame %d", nRead, nLoop );
      }

      // add frame to strip
#ifdef DEBUG
      u32   nSum1;
      char  szDbgTemp[128];
      nSum1 = calcCheckSum( cbTemp, nFrameSize );
      sprintf( szDbgTemp, "PreAddFrame: %x", nSum1 );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif
      waitUntilDMAFree( 3 );
//      waitVBLDelay( 10 );
      ( *ppStrip )->addFrame( cbTemp );
      waitUntilDMAFree( 3 );
//      waitVBLDelay( 10 );

#ifdef DEBUG
      nSum1 = calcCheckSum( cbTemp, nFrameSize );
      sprintf( szDbgTemp, "PostAddFrame: %x", nSum1 );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif
   }

   // free mem, close the file when we are done
//   free( cbTemp );
//   delete [] cbTemp;
   delete( amMem );
   DS_fclose( dsfImg );

   return 0;
}

static int l_FrameStripFreeAll( lua_State *lState )
{
	FrameStrip**   pFrameStripTemp  = toFrameStrip( lState, 1 );
   if ( *pFrameStripTemp ) {
      ( *pFrameStripTemp )->freeAllFrames();
   }
	return 0;
}

static int l_FrameStripGC( lua_State *lState )
{
	FrameStrip**   pFrameStripTemp  = toFrameStrip( lState, 1 );

   if ( *pFrameStripTemp ) {
      ( *pFrameStripTemp )->freeAllFrames();
      delete( *pFrameStripTemp );
      *pFrameStripTemp = NULL;
   }
	return 0;
}

static int l_FrameStripToString( lua_State *lState )
{
	lua_pushstring( lState, ( *toFrameStrip( lState, 1 ) )->toString() );
	return 1;
}

static const luaL_reg FrameStrip_methods[] = {
   { "Create", l_FrameStripCreate },
   { "LoadBin", l_FrameStripLoadBin },
   { "FreeAll", l_FrameStripFreeAll },
	{ 0, 0 }
};
static const luaL_reg FrameStrip_meta[] = {
	{"__gc", l_FrameStripGC },
	{"__tostring", l_FrameStripToString },
	{ 0, 0 }
};

UserdataRegister( FrameStrip, FrameStrip_methods, FrameStrip_meta);

//------------------------------------------------------------
//----- Sprite class
//------------------------------------------------------------
UserdataStubs( Sprite, Sprite* );

int   Sprite::m_nFreeIndex   = -1;
u8    Sprite::m_naNextAvailableSprite[]  = {
         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
         0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
         0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
         0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
         0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
         0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
         0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
         0xFF
      };

Sprite::Sprite( FrameStrip* pfsStrip, unsigned int nFrame, u8 nPalette )
:  m_nScreen( pfsStrip->getScreen() ),
   m_nMode( pfsStrip->getMode() ),
   m_nX( HIDDEN_SPRITE_X ),
   m_nY( HIDDEN_SPRITE_Y ),
   m_nSpriteObj( 0xFF ),
   m_nWidth( pfsStrip->getWidth() ),
   m_nHeight( pfsStrip->getHeight() )
{
   u16   gfx      = pfsStrip->getFrameGfx( nFrame );
   int   nParam   = pfsStrip->getParamIndex();
   u8    nSprite;

   // init the available sprite data struct
   if ( m_nFreeIndex < 0 ) {
      freeAllSprites();
   }

   // create sprite
   nSprite        = getNextAvailableSprite();
   m_nSpriteObj   = nSprite;
   PA_CreateSpriteFromGfx( m_nScreen, nSprite, gfx,
                           ValidSpriteParams[nParam].nParams[0], ValidSpriteParams[nParam].nParams[1],
                           m_nMode, nPalette, m_nX, m_nY );
}

Sprite::Sprite()
:  m_nScreen( 0 ),
   m_nMode( 0 ),
   m_nX( HIDDEN_SPRITE_X ),
   m_nY( HIDDEN_SPRITE_Y ),
   m_nSpriteObj( 0xFF ),
   m_nWidth( 0 ),
   m_nHeight( 0 )
{
   assert( 0 );
}

//------------------------------------------------------------
// move the sprite object to specified location on screen
//------------------------------------------------------------
void  Sprite::setFrame( FrameStrip* pStrip, unsigned int nFrame )
{
   if ( 0xFF != m_nSpriteObj ) {
      if ( NULL != pStrip ) {
         PA_SetSpriteGfx( m_nScreen, m_nSpriteObj, pStrip->getFrameGfx( nFrame ) );
      }
   }
}

//------------------------------------------------------------
// move the sprite object to specified location on screen
//------------------------------------------------------------
void  Sprite::setXY( s16 nX, s16 nY )
{
   if ( 0xFF != m_nSpriteObj ) {
      m_nX  = nX;
      m_nY  = nY;

      PA_SetSpriteXY( m_nScreen, m_nSpriteObj, nX, nY );
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
void  Sprite::hide()
{
   if ( 0xFF != m_nSpriteObj ) {
      setXY( HIDDEN_SPRITE_X, HIDDEN_SPRITE_Y );
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
void  Sprite::free()
{
   if ( 0xFF != m_nSpriteObj ) {
      hide();
      PA_DeleteSprite( m_nScreen, m_nSpriteObj );
      freeSprite( m_nSpriteObj );
      m_nSpriteObj   = 0xFF;
   }
}

u8 Sprite::getNextAvailableSprite()
{
   u8 nResult;

   if ( ( m_nFreeIndex < 0 ) || ( m_nFreeIndex >= ( int )countof( m_naNextAvailableSprite ) ) ) {
      return 0xFF;
   }

   nResult   = m_naNextAvailableSprite[ m_nFreeIndex ];
   ++m_nFreeIndex;

   return nResult;
}

void Sprite::freeAllSprites()
{
   int   nLoop;
   // free all sprite one by one
   for ( nLoop = ( m_nFreeIndex - 1 ); nLoop >= 0; ++nLoop ) {
      freeSprite( m_naNextAvailableSprite[ nLoop ] );
   }

   // reinit data structure
   for ( nLoop = 0; nLoop < MAX_SPRITE_NUM ; ++nLoop ) {
      m_naNextAvailableSprite[ nLoop ] = nLoop;
   }
   m_naNextAvailableSprite[ MAX_SPRITE_NUM ] = 0xFF;
   m_nFreeIndex                              = 0;
}

void Sprite::freeSprite( u8 nSpriteNum )
{
   if ( ( m_nFreeIndex < 0 ) || ( m_nFreeIndex >= ( int )countof( m_naNextAvailableSprite ) ) ) {
      return;
   }
   if ( m_nFreeIndex == 0 ) {
      return;
   }

   --m_nFreeIndex;
   m_naNextAvailableSprite[ m_nFreeIndex ]   = nSpriteNum;
}

Sprite::~Sprite()
{
   free();
}

const char* Sprite::toString()
{
   return "Sprite";
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SpriteLoadPalette( lua_State* lState )
{
   const int      nScreenNum  = luaL_checkint( lState, 1 );
   const int      nPaletteNum = luaL_checkint( lState, 2 );
   const char     *szFName    = luaL_checkstring( lState, 3 );
   unsigned int   nSize;

   // check parameters
   if ( ( nScreenNum < 0 ) || ( nScreenNum > 1 ) ) {
      return luaL_error( lState, "Use 0 for bottom screen and 1 for top screen" );
   }
   if ( ( nPaletteNum < 0 ) || ( nPaletteNum > 15 ) ) {
      return luaL_error( lState, "Palette number has to be 0 through 15" );
   }

   // open palette file in binary mode
   DS_FILE  *dsfPal  = DS_fopen( szFName, "rb" );
   if ( NULL == dsfPal ) {
      return luaL_error( lState, "Failed to open palette file '%s'", szFName );
   }

   // check file size
   nSize = DS_getFileSize( dsfPal );
   if ( ( nSize < 1 ) || ( nSize % 2 ) ) {
      DS_fclose( dsfPal );
      return luaL_error( lState, "Incorrect palette file size: %d", nSize );
   }

   // load palette into memroy
//   char     *cbTemp   = new char[ nSize ];
//   char     *cbTemp   = ( char* )malloc( nSize );
   AlignedMemory*    amMem = new AlignedMemory( 5, nSize );
   char  *cbTemp     = ( char* )( amMem->vpAlignedMem );
   size_t   nRead;

#ifdef DEBUG
   char  szTemp[64];
   sprintf( szTemp, "P:%p (%p)", cbTemp, amMem->vpRealMem );
   SimpleConsolePrintStringCR( SCREEN_TOP, szTemp );
#endif //DEBUG

   // make sure we have a valid pointer
   if ( NULL == cbTemp ) {
      DS_fclose( dsfPal );
      return luaL_error( lState, "Unable to allocate %d bytes for palette memory", nSize );
   }

   // init the memory acquired and read in the data
//   memset( cbTemp, 0, nSize );
//   int   i;
//   cbTemp[0]  = 0;
//   cbTemp[1]  = 0;
//   for ( i=1;i<256; ++i ) {
//      cbTemp[i<<1]      = 0x7F;
//      cbTemp[(i<<1)+1]  = 0x7F;
//   }
   nRead = DS_fread( cbTemp, 1, nSize, dsfPal );

   // make sure we read in something
   if ( nRead != nSize ) {
//      free( cbTemp );
      delete( amMem );
      DS_fclose( dsfPal );
      return luaL_error( lState, "Can only read in %d bytes of palette data", nRead );
   }

   // set palette
//   PA_LoadSpritePal( nScreenNum, nPaletteNum, ( void* )( &tempBuffer[0] ) );
//   waitVBLDelay( 10 );
   waitUntilDMAFree( 3 );
   PA_LoadSpritePal( nScreenNum, nPaletteNum, ( void* ) cbTemp );
   waitUntilDMAFree( 3 );
//   for ( i=1;i<256; ++i ) {
//assert( cbTemp[i<<1] == 0x7F );
//assert( cbTemp[(i<<1)+1] == 0x7F );
//   }

   // free mem, close the file when we are done
//   free( cbTemp );
   delete( amMem );
   DS_fclose( dsfPal );

   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SpriteCreate( lua_State* lState )
{
   FrameStrip     **ppStrip   = checkFrameStrip( lState, 1 );
   const int      nFrame      = luaL_checkint( lState, 2 );
   const int      nPalette    = luaL_checkint( lState, 3 );
   const int      nX          = luaL_checkint( lState, 4 );
   const int      nY          = luaL_checkint( lState, 5 );

   // create a new FrameStrip object
   Sprite**  ppSprite = NULL;
   ppSprite  = pushSprite( lState );
   *ppSprite = new Sprite( *ppStrip, nFrame, nPalette );

   // set image
   if ( *ppSprite ) {
      ( *ppSprite )->setXY( nX, nY );
   }
//   PA_LoadSpritePal( nScreenNum, nPaletteNum, ( void* ) cbTemp );
//   u16 gfx0 = PA_CreateGfx( nScreen, cbTemp, ValidSpriteParams[nParam].nParams[0], ValidSpriteParams[nParam].nParams[1], nMode );
   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SpriteSetFrame( lua_State* lState )
{
	Sprite      **ppSprite  = toSprite( lState, 1 );
   FrameStrip  **ppStrip   = checkFrameStrip( lState, 2 );
   const int   nFrame      = luaL_checkint( lState, 3 );

   // set image
   if ( *ppSprite ) {
      if ( *ppStrip ) {
         if ( ( nFrame < 0 ) || ( nFrame >= ( ( int )( *ppStrip )->getTotalFrames() ) ) ) {
            return luaL_error( lState, "This FrameStrip object only contains %d frames", ( ( *ppStrip )->getTotalFrames() ) );
         }
         ( *ppSprite )->setFrame( ( *ppStrip ), nFrame );
      } else {
         return luaL_error( lState, "Invalid FrameStrip object" );
      }
   } else {
      return luaL_error( lState, "Invalid Sprite object" );
   }
   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SpriteMoveTo( lua_State* lState )
{
	Sprite**    ppSprite = toSprite( lState, 1 );
   const int   nX       = luaL_checkint( lState, 2 );
   const int   nY       = luaL_checkint( lState, 3 );

   // set image
   if ( *ppSprite ) {
      ( *ppSprite )->setXY( nX, nY );
   }
   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SpriteFree( lua_State* lState )
{
	Sprite**    ppSprite = toSprite( lState, 1 );

   // set image
   if ( *ppSprite ) {
      ( *ppSprite )->free();
   }
   return 0;
}

static int l_SpriteGC( lua_State *lState )
{
	Sprite**   pSpriteTemp  = toSprite( lState, 1 );
   if ( *pSpriteTemp ) {
      delete( *pSpriteTemp );
      *pSpriteTemp = NULL;
   }
	return 0;
}

static int l_SpriteToString( lua_State *lState )
{
	lua_pushstring( lState, ( *toSprite( lState, 1 ) )->toString() );
	return 1;
}

static const luaL_reg Sprite_methods[] = {
   { "LoadPalette", l_SpriteLoadPalette },
   { "Create", l_SpriteCreate },
   { "SetFrame", l_SpriteSetFrame },
   { "MoveTo", l_SpriteMoveTo },
   { "Free", l_SpriteFree },
	{ 0, 0 }
};
static const luaL_reg Sprite_meta[] = {
	{"__gc", l_SpriteGC },
	{"__tostring", l_SpriteToString },
	{ 0, 0 }
};

UserdataRegister( Sprite, Sprite_methods, Sprite_meta);

