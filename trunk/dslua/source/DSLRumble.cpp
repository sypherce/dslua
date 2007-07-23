#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLRumble.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_IsInserted(lua_State * lState)
{
	lua_pushboolean(lState, isRumbleInserted());
	return 1;
}

static int l_SetRumble(lua_State * lState)
{
	setRumble(luaL_checknumber(lState, 1));
	return 1;
}

static const struct luaL_reg DSLRumbleLib [] =
{
	{"IsInserted", l_IsInserted},
	{"SetRumble", l_SetRumble},
	{NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLRumbleLib(lua_State * lState)
{
	luaL_openlib(lState, "Rumble", DSLRumbleLib, 0);
	return 1;
}

