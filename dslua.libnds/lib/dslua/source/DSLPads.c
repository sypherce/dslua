#include <nds.h>//NDS Header File - Included by default
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLPads.h"//This files own header file

static int l_HeldA(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_A);

	return 1;
}

static int l_HeldB(lua_State* lState)
{
	lua_pushboolean( lState, keysHeld()&KEY_B);

	return 1;
}

static int l_HeldX(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_X);

	return 1;
}

static int l_HeldY(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_Y) ;

	return 1;
}

static int l_HeldR(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_R);

	return 1;
}

static int l_HeldL(lua_State* lState)
{
	lua_pushboolean( lState, keysHeld()&KEY_L );

	return 1;
}

static int l_HeldUp(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_UP);

	return 1;
}

static int l_HeldDown(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_DOWN);

	return 1;
}

static int l_HeldLeft(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_LEFT);

	return 1;
}

static int l_HeldRight(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_RIGHT);

	return 1;
}

static int l_HeldSelect(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_SELECT);

	return 1;
}

static int l_HeldStart(lua_State* lState)
{
	lua_pushboolean(lState, keysHeld()&KEY_START);

	return 1;
}

static int l_HeldAnyKey(lua_State* lState)
{
	bool bAnyKey  =	keysHeld()&KEY_A || keysHeld()&KEY_B || keysHeld()&KEY_X || keysHeld()&KEY_Y ||
					keysHeld()&KEY_R || keysHeld()&KEY_L ||
					keysHeld()&KEY_UP || keysHeld()&KEY_DOWN || keysHeld()&KEY_LEFT || keysHeld()&KEY_RIGHT ||
					keysHeld()&KEY_SELECT || keysHeld()&KEY_START;
	lua_pushboolean(lState, bAnyKey);

	return 1;
}

static const struct luaL_reg DSLPadsLib[] =
{
	{"A", l_HeldA},
	{"B", l_HeldB},
	{"X", l_HeldX},
	{"Y", l_HeldY},
	{"L", l_HeldL},
	{"R", l_HeldR},
	{"Up", l_HeldUp},
	{"Down", l_HeldDown},
	{"Left", l_HeldLeft},
	{"Right", l_HeldRight},
	{"Select", l_HeldSelect},
	{"Start", l_HeldStart},
	{"AnyKey", l_HeldAnyKey},
	{NULL, NULL}
};

int luaopen_DSLPadsLib(lua_State* lState)
{
	luaL_openlib(lState, "Pads", DSLPadsLib, 0);

	return 1;
}
