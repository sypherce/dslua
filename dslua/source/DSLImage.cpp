#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLImage.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageLoadBMP(lua_State * lState)
{
	// Note: This function doesn't seem to work...
	int screen = luaL_checkint(lState, 1);
	int x = luaL_checkint(lState, 2);
	int y = luaL_checkint(lState, 3);
	char * buffer = (char *)luaL_checkstring(lState, 4);

	PA_LoadBmpEx(screen, x, y, buffer);

	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageBMPHeight(lua_State * lState)
{
	char * buffer = (char *)luaL_checkstring(lState, 1);

	lua_pushnumber(lState, PA_GetBmpHeight(buffer));
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageBMPWidth(lua_State * lState)
{
	char * buffer = (char *)luaL_checkstring(lState, 1);

	lua_pushnumber(lState, PA_GetBmpWidth(buffer));
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageLoadJPG(lua_State * lState)
{
	int screen = luaL_checkint(lState, 1);
	char * buffer = (char *)luaL_checkstring(lState, 2);

	PA_LoadJpeg(screen, buffer);

	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageLoadGIF(lua_State * lState)
{
	int screen = luaL_checkint(lState, 1);
	char * buffer = (char *)luaL_checkstring(lState, 2);

	PA_LoadGif(screen, buffer);

	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageGIFWidth(lua_State * lState)
{
	char * buffer = (char *)luaL_checkstring(lState, 1);

	lua_pushnumber(lState, PA_GetGifWidth(buffer));
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_ImageGIFHeight(lua_State * lState)
{
	char * buffer = (char *)luaL_checkstring(lState, 1);

	lua_pushnumber(lState, PA_GetGifHeight(buffer));
	return 1;
}


static const struct luaL_reg DSLImageLib [] =
{
	{"LoadBMP", l_ImageLoadBMP},
	{"BMPHeight", l_ImageBMPHeight},
	{"BMPWidth", l_ImageBMPWidth},	{"LoadJPG", l_ImageLoadJPG},
	{"LoadGIF", l_ImageLoadGIF},
	{"GIFHeight", l_ImageGIFHeight},
	{"GIFWidth", l_ImageGIFWidth},
	{NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLImageLib(lua_State * lState)
{
	luaL_openlib(lState, "Image", DSLImageLib, 0);
	return 1;
}

