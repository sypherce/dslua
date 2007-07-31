#include <nds.h>//NDS Header File - Included by default
#include <stdlib.h>//Standard C Library
#include "mikmod9.h"//MikMod arm9 Library
#include "mikmod.h"//MikMod Library
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLMusic.h"//This files own header file
#include "Utils.h"//Datatype helpers


void MikMod9_SendCommand(u32 data)
{
    while (REG_IPC_FIFO_CR & IPC_FIFO_SEND_FULL);
    if (REG_IPC_FIFO_CR & IPC_FIFO_ERROR)
    {
        REG_IPC_FIFO_CR |= IPC_FIFO_SEND_CLEAR;
    } 
    
    REG_IPC_FIFO_TX = data;
}

UserdataStubs(Music, Music *);

static int l_MusicToString(lua_State * lState)
{
	lua_pushstring(lState, (char*)"__MUSIC__");

	return 1;
}

static int l_MusicPlay(lua_State * lState)
{
	Music ** music = toMusic(lState, 1);
	if(*music)
	{
		Player_Start(*music);
		Player_SetPosition(0);
		modUpdate = true;
	}

	return 0;
}

static int l_MusicPause(lua_State * lState)
{
	Music ** music = toMusic(lState, 1);
	bool state = lua_toboolean(lState, 2);

	if(*music) modUpdate = !modUpdate;

	return 0;
}

static int l_MusicStop(lua_State * lState)
{
	Music ** music = toMusic(lState, 1);

	if(*music) Player_Stop();
	modUpdate = false;

	return 0;
}

static int l_MusicRelease(lua_State * lState)
{
	Music ** music = toMusic(lState, 1);
	if(*music)
	{
		l_MusicStop(lState);
		Player_Free(*music);
		*music = NULL;
	}

	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_MusicLoadMod(lua_State * lState)
{
	const char *szFName = luaL_checkstring(lState, 1);
	int nSize;

	// open palette file in binary mode
	FILE *dsfMusic = fopen(szFName, "rb");

	if(dsfMusic == NULL)
	{
		return luaL_error(lState, "Failed to open raw music file '%s'", szFName);
	}

	fseek(dsfMusic, 0, SEEK_END);
	nSize = ftell(dsfMusic);
	rewind (dsfMusic);

	// create new sound object, Sound obj is responsible for delete memory
	char *data = (char*)malloc(nSize);

	// make sure we have a valid pointer
	if(data == NULL)
	{
		fclose(dsfMusic);
		return luaL_error(lState, "Unable to allocate %d bytes for music memory", nSize);
	}

	// init the memory acquired and read in the data
	size_t nRead = fread(data, 1, nSize, dsfMusic);

	// close the file when we are done
	fclose(dsfMusic);

	// make sure we read in something
	if(nRead != nSize)
	{
		free(data);
		return luaL_error(lState, "Can only read in %d bytes of tile music data", nRead);
	}
	// create new sound object, Sound obj is responsible for delete memory
	Music ** ppMusic = NULL;
	ppMusic = (Music**)pushMusic(lState);
	//*ppMusic = (Music*)malloc(sizeof(Music));

	*ppMusic = Player_LoadMemory(data, nSize, 64, 0);
	if(*ppMusic == NULL)
	{
		return luaL_error(lState, "Unable to allocate %d bytes for music", nSize);
	}

	free(data);

	return 1;
}

static const luaL_reg Music_methods[] = {
	{"LoadMod", l_MusicLoadMod},
	{"Play", l_MusicPlay},
	{"Pause", l_MusicPause},
	{"Stop", l_MusicStop},
	{"Free", l_MusicRelease},
	{0, 0}
};
static const luaL_reg Music_meta[] = {
	{"__gc", l_MusicRelease},
	{"__tostring", l_MusicToString},
	{0, 0}
};

UserdataRegister(Music, Music_methods, Music_meta);

