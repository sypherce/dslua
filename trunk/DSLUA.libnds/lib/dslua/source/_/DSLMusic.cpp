#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "DSFileIO.h"
#include "DSLMusic.h"

//------------------------------------------------------------
//----- Music base class
//------------------------------------------------------------
UserdataStubs( Music, Music* );

Music::Music( AlignedMemory* apMusic )
:  m_apMusic( apMusic )
{
}

Music::Music()
:  m_apMusic( NULL )
{
}

void Music::play()
{
   if ( m_apMusic ) {
      PA_PlayMod( m_apMusic->vpAlignedMem );
   }
}

void Music::pause( bool bPause)
{
   if ( m_apMusic ) {
      PA_PauseMod( bPause );
   }
}

void Music::stop()
{
   if ( m_apMusic ) {
      PA_StopMod();
   }
}

void Music::release()
{
   if ( m_apMusic ) {
      stop();
      delete( m_apMusic );
      m_apMusic   = NULL;
   }
}

const char* Music::toString()
{
   return "__MUSIC__";
}

Music::~Music()
{
   release();
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicPlay( lua_State *lState )
{
	Music**   pMusicTemp  = toMusic( lState, 1 );
   if ( *pMusicTemp ) {
      ( *pMusicTemp )->play();
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicPause( lua_State *lState )
{
	Music**  pMusicTemp  = toMusic( lState, 1 );
   bool     bState      = lua_toboolean( lState, 2 );
   if ( *pMusicTemp ) {
      ( *pMusicTemp )->pause( bState );
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicStop( lua_State *lState )
{
	Music**   pMusicTemp  = toMusic( lState, 1 );
   if ( *pMusicTemp ) {
      ( *pMusicTemp )->stop();
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicRelease( lua_State *lState )
{
	Music**   pMusicTemp  = toMusic( lState, 1 );
   if ( *pMusicTemp ) {
      ( *pMusicTemp )->release();
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicLoadMod( lua_State* lState )
{
   const char*    szFName  = luaL_checkstring( lState, 1 );
   unsigned int   nSize;

   // open palette file in binary mode
   DS_FILE  *dsfMusic  = DS_fopen( szFName, "rb" );
   if ( NULL == dsfMusic ) {
      return luaL_error( lState, "Failed to open raw music file '%s'", szFName );
   }

   // load music into memroy
   nSize = DS_getFileSize( dsfMusic );
   AlignedMemory* amMem    = new AlignedMemory( 5, nSize );
   char*          cbTemp   = ( char* )( amMem->vpAlignedMem );
   size_t         nRead;

#ifdef DEBUG
   char  szTemp[64];
   sprintf( szTemp, "MP:%p (%p)", cbTemp, amMem->vpRealMem );
   SimpleConsolePrintStringCR( SCREEN_TOP, szTemp );
#endif //DEBUG

   // make sure we have a valid pointer
   if ( NULL == cbTemp ) {
      DS_fclose( dsfMusic );
      return luaL_error( lState, "Unable to allocate %d bytes for music memory", nSize );
   }

   // init the memory acquired and read in the data
   nRead = DS_fread( cbTemp, 1, nSize, dsfMusic );

   // make sure we read in something
   if ( nRead != nSize ) {
      delete( amMem );
      DS_fclose( dsfMusic );
      return luaL_error( lState, "Can only read in %d bytes of tile music data", nRead );
   }

   // close the file when we are done
   DS_fclose( dsfMusic );

   // create new music object, Music obj is responsible for delete memory
   Music**  ppMusic  = NULL;
   ppMusic  = pushMusic( lState );
   *ppMusic = new Music( amMem );

   return 1;
}

static int l_MusicGC( lua_State *lState )
{
	Music**   pMusicTemp  = toMusic( lState, 1 );
   if ( *pMusicTemp ) {
      delete( *pMusicTemp );
      *pMusicTemp = NULL;
   }
	return 0;
}

static int l_MusicToString( lua_State *lState )
{
	lua_pushstring( lState, ( *toMusic( lState, 1 ) )->toString() );
	return 1;
}

static const luaL_reg Music_methods[] = {
   { "LoadMod", l_MusicLoadMod },
   { "Play", l_MusicPlay },
   { "Pause", l_MusicPause },
   { "Stop", l_MusicStop },
   { "Free", l_MusicRelease },
	{0,0}
};
static const luaL_reg Music_meta[] = {
	{"__gc", l_MusicGC },
	{"__tostring", l_MusicToString },
	{0,0}
};

UserdataRegister( Music, Music_methods, Music_meta);

