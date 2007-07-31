#include <nds.h>//NDS Header File - Included by default
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLMic.h"//This files own header file

static int l_StartRecording(lua_State* lState)
{
	u16 *buffer = (u16*)luaL_checkstring(lState, 1);
	int length = luaL_checkint(lState, 2);
	CommandStartRecording(buffer, length);
	lua_pushboolean(lState, 1);

	return 1;
}

static int l_GetVol(lua_State* lState)
{
	printf("hello");
	lua_pushnumber(lState, 0/*!PA_MicGetVol()*/);

	return 1;
}

static int l_Replay(lua_State* lState)
{
	u8 channel = luaL_checkint(lState, 1);
	u16 *buffer = (u16*)luaL_checkstring(lState, 2);
	int length = luaL_checkint(lState, 3);
	CommandPlayOneShotSample(0, 16384, buffer, length*2, 0x7f);
	//playGenericSound(buffer, length);
	lua_pushboolean(lState, 1);

	return 1;
}

static const struct luaL_reg DSLMicLib[] =
{
	{"StartRecording", l_StartRecording},
	{"GetVol", l_GetVol},
	{"Replay", l_Replay},
	{NULL, NULL}
};

int luaopen_DSLMicLib(lua_State* lState)
{
	luaL_openlib(lState, "Mic", DSLMicLib, 0);

	return 1;
}
