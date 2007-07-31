#include <nds.h>//NDS Header File - Included by default
#include <stdlib.h>//Standard C Library
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLSound.h"//This files own header file
#include "Utils.h"//Datatype helpers

UserdataStubs(Sound, Sound *);

static int l_SoundToString(lua_State * lState)
{
	lua_pushstring(lState, (char*)"__SOUND__");

	return 1;
}

static int l_SoundPlay(lua_State * lState)
{
	Sound ** sound = toSound(lState, 1);
	if(*sound)
		playGenericSound((*sound)->data, (*sound)->size);

	return 0;
}

static int l_SoundRelease(lua_State * lState)
{
	Sound ** sound = toSound(lState, 1);
	if(*sound)
	{
		free(*sound);
		*sound = NULL;
	}

	return 0;
}

static int l_SoundLoadRaw(lua_State * lState)
{
	const char *szFName = luaL_checkstring(lState, 1);
	unsigned int nSize;

	// open palette file in binary mode
	FILE *dsfSound = fopen(szFName, "rb");

	if(dsfSound == NULL)
	{
		return luaL_error(lState, "Failed to open raw sound file '%s'", szFName);
	}

	fseek(dsfSound, 0, SEEK_END);
	nSize = ftell(dsfSound);
	rewind (dsfSound);

	// create new sound object, Sound obj is responsible for delete memory
	char *data = (char*)malloc(nSize);

	// make sure we have a valid pointer
	if(data == NULL)
	{
		fclose(dsfSound);
		return luaL_error(lState, "Unable to allocate %d bytes for sound memory", nSize);
	}

	// init the memory acquired and read in the data
	size_t nRead = fread(data, 1, nSize, dsfSound);

	// close the file when we are done
	fclose(dsfSound);

	// make sure we read in something
	if(nRead != nSize)
	{
		free(data);
		return luaL_error(lState, "Can only read in %d bytes of tile sound data", nRead);
	}

	// create new sound object, Sound obj is responsible for delete memory
	Sound ** ppSound = NULL;
	ppSound = (Sound**)pushSound(lState);
	*ppSound = (Sound*)malloc(sizeof(Sound));
	if(*ppSound == NULL)
	{
		return luaL_error(lState, "Unable to allocate %d bytes for sound memory", nSize);
	}
	(*ppSound)->data = (void*)data;
	(*ppSound)->size = nSize;

	return 1;
}

static const luaL_reg Sound_methods[] = {
	{"LoadRaw", l_SoundLoadRaw},
	{"Play", l_SoundPlay},
	{"Free", l_SoundRelease},
	{0, 0}
};
static const luaL_reg Sound_meta[] = {
	{"__gc",l_SoundRelease},
	{"__tostring", l_SoundToString},
	{0, 0}
};

UserdataRegister(Sound, Sound_methods, Sound_meta);

