#include <PA9.h>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "Utils.h"
#include "SimpleConsole.h"

int g_nSimpleConsoleNextX[] = {0, 0};
int g_nSimpleConsoleNextY[] = {0, 0};

//------------------------------------------------------------
//------------------------------------------------------------
void  SimpleConsoleFillRectWithChar(int nScreen, int nX1, int nY1, int nX2, int nY2, const char cFill)
{
	int nW, nH;
	char caTemp[ 4 ];

	caTemp[0] = cFill;
	caTemp[1] = '\0';

	for(nH = nY1; nH <= nY2; ++nH)
	{
		for(nW = nX1; nW <= nX2; ++nW)
		{
			PA_OutputSimpleText(nScreen, nW, nH, caTemp);
		}
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
void SimpleConsoleClearScreem(int nScreen)
{
	PA_ClearTextBg(nScreen);
	g_nSimpleConsoleNextX[ nScreen ] = 0;
	g_nSimpleConsoleNextY[ nScreen ] = 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
void  SimpleConsoleSetColor(int nScreen, int nColor)
{
	PA_SetTextTileCol(nScreen, nColor);
}

//DSLUA_GetLastLetterX & DSLUA_GetLastLetterY are untested
#define DSLUA_GetLastLetterX(screen) (g_nSimpleConsoleNextX[ screen ] + strlen(szString))
#define DSLUA_GetLastLetterY(screen) (g_nSimpleConsoleNextY[ screen ] + (strlen(szString) / DSLUA_MAX_TEXT_COLUMNS)) //might have problems with remaining fraction rounding
//------------------------------------------------------------
//------------------------------------------------------------
int SimpleConsolePrintString(int nScreen, const char * szString)
{
	int nResult;
	int nNext;

	nResult = PA_OutputSimpleText(nScreen, g_nSimpleConsoleNextX[ nScreen ], g_nSimpleConsoleNextY[ nScreen ], szString);
	nNext = DSLUA_GetLastLetterY(nScreen) * DSLUA_MAX_TEXT_COLUMNS + DSLUA_GetLastLetterX(nScreen) + 1;
	// TODO: calculate using bits since width is 32
	if(nNext < DSLUA_MAX_TEXT_COUNT)
	{
		g_nSimpleConsoleNextY[ nScreen ] = nNext / DSLUA_MAX_TEXT_COLUMNS;
		g_nSimpleConsoleNextX[ nScreen ] = nNext % DSLUA_MAX_TEXT_COLUMNS;
	}
	else
	{
		// TODO: scroll screen
		//      g_nSimpleConsoleNextX[ nScreen ]  = 0;
		//      g_nSimpleConsoleNextY[ nScreen ]  = DSLUA_MAX_TEXT_ROWS - 1;
		SimpleConsoleClearScreem(nScreen);
	}
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
int SimpleConsolePrintStringCR(int nScreen, const char * szString)
{
	int nResult = SimpleConsolePrintString(nScreen, szString);

	SimpleConsolePrintLine(nScreen);
	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int SimpleConsolePrintLine(int nScreen)
{
	g_nSimpleConsoleNextX[ nScreen ] = 0;
	if(g_nSimpleConsoleNextY[ nScreen ] >= (DSLUA_MAX_TEXT_ROWS - 1))
	{
		// TODO: implement scrolling
#ifdef DEBUG
		waitVBLDelay(20);
		waitUntilKeyPressed(&Pad.Held.Start);
#endif
		SimpleConsoleClearScreem(nScreen);
		//      g_nSimpleConsoleNextY[ nScreen ]   = ( DSLUA_MAX_TEXT_ROWS - 1 );
	}
	else
	{
		g_nSimpleConsoleNextY[ nScreen ]++;
	}
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
void stackDump(lua_State * L, int nScreen)
{
	int i;
	int top = lua_gettop(L);
	char caTemp[256];

	for(i = 1; i <= top; i++)
	{                            /* repeat for each level */
		int t = lua_type(L, i);
		switch(t)
		{
		case LUA_TSTRING:/* strings */
			sprintf(caTemp, "`%s'", lua_tostring(L, i));
			SimpleConsolePrintString(nScreen, caTemp);
			break;

		case LUA_TBOOLEAN:/* booleans */
			sprintf(caTemp, "%s", lua_toboolean(L, i) ? "true" : "false");
			SimpleConsolePrintString(nScreen, caTemp);
			break;

		case LUA_TNUMBER:/* numbers */
			sprintf(caTemp, "%g", lua_tonumber(L, i));
			SimpleConsolePrintString(nScreen, caTemp);
			break;

		default:/* other values */
			sprintf(caTemp, "%s", lua_typename(L, t));
			SimpleConsolePrintString(nScreen, caTemp);
			break;
		}
		SimpleConsolePrintStringCR(nScreen, ""); /* put a separator */
	}
	//   SimpleConsolePrintStringCR( nScreen, caTemp );
	printf("\n"); /* end the listing */
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SetTextColor(lua_State * lState)
{
	int nScreen = luaL_checkint(lState, 1);
	int nColor = luaL_checkint(lState, 2);

	SimpleConsoleSetColor(nScreen, nColor);
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_PrintAtScreenXY(lua_State * lState)
{
	// get all arguments
	int nScreen = luaL_checkint(lState, 1);
	int nX = luaL_checkint(lState, 2);
	int nY = luaL_checkint(lState, 3);
	const char * szString = luaL_checkstring(lState, 4);

	// print the string at specified location
	PA_OutputSimpleText(nScreen, nX, nY, szString);

	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_FillTextRectWithChar(lua_State * lState)
{
	// get all arguments
	int nScreen = luaL_checkint(lState, 1);
	int nX1 = luaL_checkint(lState, 2);
	int nY1 = luaL_checkint(lState, 3);
	int nX2 = luaL_checkint(lState, 4);
	int nY2 = luaL_checkint(lState, 5);
	const char * szString = luaL_checkstring(lState, 6);

	// print the string at specified location
	SimpleConsoleFillRectWithChar(nScreen, nX1, nY1, nX2, nY2, szString[0]);

	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_EraseScreen(lua_State * lState)
{
	int nScreen = luaL_checkint(lState, 1);

	SimpleConsoleClearScreem(nScreen);

	return 0;
}

static const struct luaL_reg DSLConsoleLib [] =
{
	{"SetColor", l_SetTextColor},
	{"PrintXY", l_PrintAtScreenXY},
	{"FillRectWithChar", l_FillTextRectWithChar},
	{"EraseScreen", l_EraseScreen},
	{NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLConsoleLib(lua_State * lState)
{
	luaL_openlib(lState, "Text", DSLConsoleLib, 0);
	return 1;
}
