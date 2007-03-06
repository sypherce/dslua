#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLKeyboard.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardInit(lua_State * lState)
{
	int background = luaL_checkint(lState, 1);
	PA_InitKeyboard(background);
	// Scroll it off-screen
	PA_ScrollKeyboardXY(20, 256);
	lua_pushboolean(lState, true);
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardSetColor(lua_State * lState)
{
	int color1 = luaL_checkint(lState, 1);
	int color2 = luaL_checkint(lState, 2);

	if(color1 >= 0 && color1 <= 3 && color2 >= 0 && color2 <= 3)
	{
		PA_SetKeyboardColor(color1, color2);
		lua_pushboolean(lState, false);
	}
	else
	{
		lua_pushboolean(lState, false);
	}
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardGetChar(lua_State * lState)
{
	char character = PA_CheckKeyboard();

	lua_pushnumber(lState, int (character));
	PA_EraseLastKey();
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardScrollXY(lua_State * lState)
{
	int x = luaL_checkint(lState, 1);
	int y = luaL_checkint(lState, 2);

	PA_ScrollKeyboardXY(x, y);
	lua_pushboolean(lState, true);
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardIn(lua_State * lState)
{
	int x = luaL_checkint(lState, 1);
	int y = luaL_checkint(lState, 2);

	PA_KeyboardIn(x, y);
	lua_pushboolean(lState, true);
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_KeyboardOut(lua_State * lState)
{
	PA_KeyboardOut();
	lua_pushboolean(lState, true);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static const struct luaL_reg DSLKeyboardLib [] =
{
	{"Init", l_KeyboardInit},
	{"Color", l_KeyboardSetColor},
	{"GetChar", l_KeyboardGetChar},
	{"ScrollXY", l_KeyboardScrollXY},
	{"In", l_KeyboardIn},
	{"Out", l_KeyboardOut},
	{NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLKeyboardLib(lua_State * lState)
{
	luaL_openlib(lState, "Keyboard", DSLKeyboardLib, 0);
	return 1;
}

