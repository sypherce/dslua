#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "DSFileIO.h"
#include "DSLSound.h"

#define _EXTRA_SOUND_SIZE  32

//------------------------------------------------------------
//----- Sound base class
//------------------------------------------------------------
UserdataStubs( Sound, Sound* );

u8   Sound::m_nNextChannel = 0;

Sound::Sound( AlignedMemory* apSound )
:  m_apSound( apSound )
{
}

Sound::Sound()
:  m_apSound( NULL )
{
}

u8 Sound::getNextChannel()
{
   u8 nResult  = m_nNextChannel;

   m_nNextChannel = ( ( m_nNextChannel + 1 ) & 0x07 );

   return nResult;
}

void Sound::play()
{
   if ( m_apSound ) {
      u8 nChannel = getNextChannel() + 4;
      PA_PlaySoundEx( nChannel,
                    m_apSound->vpAlignedMem, ( m_apSound->m_nSize - _EXTRA_SOUND_SIZE ),
                    PA_SoundOption.volume, PA_SoundOption.freq, 1 );
   }
}

void Sound::release()
{
   if ( m_apSound ) {
      delete( m_apSound );
      m_apSound   = NULL;
   }
}

const char* Sound::toString()
{
   return "__SOUND__";
}

Sound::~Sound()
{
   release();
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SoundPlay( lua_State *lState )
{
	Sound**   pSoundTemp  = toSound( lState, 1 );
   if ( *pSoundTemp ) {
      ( *pSoundTemp )->play();
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SoundRelease( lua_State *lState )
{
	Sound**   pSoundTemp  = toSound( lState, 1 );
   if ( *pSoundTemp ) {
      ( *pSoundTemp )->release();
   }
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SoundLoadRaw( lua_State* lState )
{
   const char*    szFName  = luaL_checkstring( lState, 1 );
   unsigned int   nSize;
   int            nLoop;

   // open palette file in binary mode
   DS_FILE  *dsfSound  = DS_fopen( szFName, "rb" );
   if ( NULL == dsfSound ) {
      return luaL_error( lState, "Failed to open raw sound file '%s'", szFName );
   }

   // load sound into memroy
   nSize = DS_getFileSize( dsfSound );
   AlignedMemory* amMem    = new AlignedMemory( 5, nSize + _EXTRA_SOUND_SIZE );
   char*          cbTemp   = ( char* )( amMem->vpAlignedMem );
   for ( nLoop = 0; nLoop < _EXTRA_SOUND_SIZE; ++nLoop ) {
      cbTemp[ nSize + nLoop ] = 0x00;
   }
   size_t         nRead;

#ifdef DEBUG
   char  szTemp[64];
   sprintf( szTemp, "SP:%p (%p)", cbTemp, amMem->vpRealMem );
   SimpleConsolePrintStringCR( SCREEN_TOP, szTemp );
#endif //DEBUG

   // make sure we have a valid pointer
   if ( NULL == cbTemp ) {
      DS_fclose( dsfSound );
      return luaL_error( lState, "Unable to allocate %d bytes for sound memory", nSize );
   }

   // init the memory acquired and read in the data
   nRead = DS_fread( cbTemp, 1, nSize, dsfSound );

   // make sure we read in something
   if ( nRead != nSize ) {
      delete( amMem );
      DS_fclose( dsfSound );
      return luaL_error( lState, "Can only read in %d bytes of tile sound data", nRead );
   }

   // close the file when we are done
   DS_fclose( dsfSound );

   // create new sound object, Sound obj is responsible for delete memory
   Sound**  ppSound  = NULL;
   ppSound  = pushSound( lState );
   *ppSound = new Sound( amMem );

   return 1;
}

static int l_SoundGC( lua_State *lState )
{
	Sound**   pSoundTemp  = toSound( lState, 1 );
   if ( *pSoundTemp ) {
      delete( *pSoundTemp );
      *pSoundTemp = NULL;
   }
	return 0;
}

static int l_SoundToString( lua_State *lState )
{
	lua_pushstring( lState, ( *toSound( lState, 1 ) )->toString() );
	return 1;
}

static const luaL_reg Sound_methods[] = {
   { "LoadRaw", l_SoundLoadRaw },
   { "Play", l_SoundPlay },
   { "Free", l_SoundRelease },
	{0,0}
};
static const luaL_reg Sound_meta[] = {
	{"__gc", l_SoundGC },
	{"__tostring", l_SoundToString },
	{0,0}
};

UserdataRegister( Sound, Sound_methods, Sound_meta);

