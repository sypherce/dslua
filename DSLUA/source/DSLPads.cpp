#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
	//#include "lualib.h"
}
#include "DSLPads.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldA(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.A);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldB(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.B);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldX(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.X);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldY(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Y);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldR(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.R);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldL(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.L);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldUp(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Up);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldDown(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Down);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldLeft(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Left);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldRight(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Right);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldSelect(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Select);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldStart(lua_State * lState)
{
	lua_pushboolean(lState, Pad.Held.Start);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_HeldAnyKey(lua_State * lState)
{
	bool bAnyKey = Pad.Held.A || Pad.Held.B || Pad.Held.X || Pad.Held.Y ||
	               Pad.Held.R || Pad.Held.L ||
	               Pad.Held.Up || Pad.Held.Down || Pad.Held.Left || Pad.Held.Right ||
	               Pad.Held.Select || Pad.Held.Start;

	lua_pushboolean(lState, bAnyKey);
	return 1;
}

static const struct luaL_reg DSLPadsLib [] =
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

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLPadsLib(lua_State * lState)
{
	luaL_openlib(lState, "Pads", DSLPadsLib, 0);
	return 1;
}

