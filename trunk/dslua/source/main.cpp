// Includes
#include <stdlib.h>
#include <PA9.h>       // Include for PA_Lib
#include <fat.h>
#include <sys/dir.h>
#include <sys/unistd.h>
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
//#include "zlib.h"
#include "Utils.h"
#include "DSFileIO.h"
#include "SimpleConsole.h"
#include "DSLScreen.h"
#include "DSLPads.h"
#include "DSLStylus.h"
#include "DSLSound.h"
#include "DSLMic.h"
#include "DSLMusic.h"
#include "DSLRumble.h"
#include "DSLSystem.h"
#include "DSLWifi.h"
//#include "DSLKeyboard.h"
//#include "DSLGL.h"
#include "DSLDir.h"
#include "DSLImage.h"
//#include "resetmem.h"

#define buildPath(a,b,c) \
strcpy(a,b); \
strcat(a,c);

// global vars
static const luaL_reg lualibs[] = {
	//  LUA_BASELIBS
	{"base", luaopen_base},
	{"debug", luaopen_debug},
	{"table", luaopen_table},
	{"io", luaopen_io},
	{"string", luaopen_string},
	{"math", luaopen_math},
	//  LUA_EXTRALIBS
	{"text", luaopen_DSLConsoleLib},
	{"sound", Sound_register},
	{"music", Music_register},
	{"screen", luaopen_DSLScreenLib},
	{"pads", luaopen_DSLPadsLib},
	{"stylus", luaopen_DSLStylusLib},
	{"mic", luaopen_DSLMicLib},
	{"rumble", luaopen_DSLRumbleLib},
	{"dslua", luaopen_DSLSystemLib},
	{"wifi", luaopen_DSLWifiLib},
//	{"keyboard", luaopen_DSLKeyboardLib},
//	{"dsgl", luaopen_DSLGLLib},
	{"directory", luaopen_DSLDirectoryLib},
	{"image", luaopen_DSLImageLib},
	{NULL, NULL}
};

// func defs
bool  initializeEngine(void);
void  initDisplay(void);
void  cleanUpEngine(void);

static void openstdlibs(lua_State *);
//static int  report ( lua_State*, int );
static void l_message(const char *, const char *);
//static void stackDump ( lua_State* );
int         executeLuaScript(lua_State *, const char *);


//#define  DEBUG

//------------------------------------------------------------
//------------------------------------------------------------
int main(int argc, char * * argv)
{
	//static const char* myVersion = ZLIB_VERSION;
	//bool  bTest = ( zlibVersion()[0] == myVersion[0] );

	// init variables
	char szPath[ DS_MAX_FILENAME_LENGTH ] = "/scripts/";
	char szFile[ DS_MAX_FILENAME_LENGTH ] = "main.lua";

	// clear all memory before starting
	//ClearMemory();

	// main loop
	while(1)
	{
		// initialize everything
		if(!initializeEngine())
		{
			return 0;
		}

		while(1)
		{
			// show menu
			initDisplay();
			char szTarget[ DS_MAX_FILENAME_LENGTH + DS_MAX_FILENAME_LENGTH ];
			buildPath(szTarget, szPath, szFile);
			chdir(szPath);

#ifdef DEBUG
			SimpleConsolePrintStringCR(SCREEN_TOP, "Open LUA engine");
#endif
			// init the Lua state machine
			lua_State * lState = lua_open();   /* create state */
			if(lState == NULL)
			{
				// TODO: display error message
				l_message("DSLua", "cannot create state: not enough memory");
				return EXIT_FAILURE;
			}

#ifdef DEBUG
			SimpleConsolePrintStringCR(SCREEN_TOP, "LUA state initialized");
#endif
			// init the Lua state machine
			// execute script
			openstdlibs(lState);

			int nStatus;
#ifndef DEBUG
			PA_ResetSpriteSys();
			PA_ResetBgSys();
#endif
			initDisplay();
			nStatus = executeLuaScript(lState, szTarget);

			// pause for a bit
			if(nStatus)
			{
				waitUntilKeyPressed((bool *)&Pad.Held.Start);
				//            } else {
				//               waitVBLDelay( 60 );
			}

			// finished script, clean up
			//lua_setgcthreshold(lState, 0);
			lua_close(lState);
		}

		// clean up
		cleanUpEngine();
	}

	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   executeLuaScript(lua_State * lState, const char * szScriptName)
{
	int nResult;
	char caTemp[128];

	// load script into memory
	nResult = luaL_loadfile(lState, szScriptName);

	// if script loads properly, execute it
	if(nResult == 0)
	{
		// provide traceback function
		lua_pushstring(lState, "_TRACEBACK");
		lua_gettable(lState, LUA_GLOBALSINDEX);
		lua_insert(lState, -2);

		// execute Lua program
		nResult = lua_pcall(lState, 0, LUA_MULTRET, -2);
		if(nResult != 0)
		{
			initDisplay();
			PA_SetTextTileCol(SCREEN_TOP, PA_TEXT_RED);
			PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_RED);
			//         report( lState, nResult );
			sprintf(caTemp, "ERROR: script result %d\n", nResult);
			SimpleConsolePrintStringCR(SCREEN_BOTTOM, caTemp);
			stackDump(lState, SCREEN_BOTTOM);
		}
	}
	else
	{
		initDisplay();
		PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_RED);
		sprintf(caTemp, "Failed to load: %d", nResult);
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, caTemp);
		stackDump(lState, SCREEN_BOTTOM);
	}
	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
static void l_message(const char * pname, const char * msg)
{
	if(pname) fprintf(stderr, "%s: ", pname);
	fprintf(stderr, "%s\n", msg);
}


////------------------------------------------------------------
////------------------------------------------------------------
//static int report( lua_State* L, int status) {
//   const char *msg;
//   if ( status ) {
//      msg = lua_tostring( L, -1 );
//      if (msg == NULL) msg = "(error with no message)";
//      l_message( "DSLua", msg );
//      lua_pop(L, 1);
//   }
//   return status;
//}

//------------------------------------------------------------
//------------------------------------------------------------
static void openstdlibs(lua_State * l)
{
	const luaL_reg * lib = lualibs;

	for(; lib->func; lib++)
	{
		lib->func(l); /* open library */
		lua_settop(l, 0); /* discard any results */
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
void  initDisplay()
{
	// Initialize the text system on the bottom screen
	resetAllGraphics();
	PA_InitText(SCREEN_TOP, 0);
	PA_InitText(SCREEN_BOTTOM, 0);

	// clear screen and reset text color to white
	SimpleConsoleClearScreem(SCREEN_TOP);
	SimpleConsoleClearScreem(SCREEN_BOTTOM);
	PA_SetTextTileCol(SCREEN_TOP, PA_TEXT_WHITE);
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_WHITE);
	PA_SetScreenLight(SCREEN_TOP, 1);
	PA_SetScreenLight(SCREEN_BOTTOM, 1);
	PA_SetLedBlink(0,0);
}
u32 vblCounter = 0;

void vblFunc()
{
	vblCounter++;
	PA_vblFunc();
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  initializeEngine(void)
{
	irqInit();
	PA_Init();       // Initialize PA_Lib
	//PA_InitVBL();    // Initialize a standard VBL
	irqSet(IRQ_VBLANK, vblFunc); 
	PA_InitSound();  // initialize the sound system
	PA_InitWifi();

	// try to reset sound memory
	const u32 ubEmpty[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int nLoop;
	for(nLoop = 0; nLoop < 16; ++nLoop)
	{
		PA_PlaySound(nLoop, &ubEmpty, 16, 0, 11025);
	}

	// Initialize the text system on the bottom screen
	initDisplay();

	SimpleConsolePrintStringCR(SCREEN_BOTTOM, "Hello from DSLua");

	// initialize the disc
	if(!fatInitDefault())
	{
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "Failed to initialize IO devices...");
		return false;
	}

	// all done
	initDisplay();

	return true;
}

//------------------------------------------------------------
//------------------------------------------------------------
void eraseDirListing()
{
	initDisplay();
}

////------------------------------------------------------------
////------------------------------------------------------------
//static void stackDump( lua_State *L ) {
//   int   i;
//   int   top = lua_gettop(L);
//   char  caTemp[256];
//
//   for (i = 1; i <= top; i++) {  /* repeat for each level */
//      int t = lua_type(L, i);
//      switch (t) {
//
//      case LUA_TSTRING:  /* strings */
//         sprintf( caTemp, "`%s'", lua_tostring( L, i ) );
//         SimpleConsolePrintString( SCREEN_BOTTOM, caTemp );
//         break;
//
//      case LUA_TBOOLEAN:  /* booleans */
//         sprintf( caTemp, "%s", lua_toboolean( L, i ) ? "true" : "false" );
//         SimpleConsolePrintString( SCREEN_BOTTOM, caTemp );
//         break;
//
//      case LUA_TNUMBER:  /* numbers */
//         sprintf( caTemp, "%g", lua_tonumber( L, i ) );
//         SimpleConsolePrintString( SCREEN_BOTTOM, caTemp );
//         break;
//
//      default:  /* other values */
//         sprintf( caTemp, "%s", lua_typename( L, t ) );
//         SimpleConsolePrintString( SCREEN_BOTTOM, caTemp );
//         break;
//
//      }
//      SimpleConsolePrintString( SCREEN_BOTTOM, "  " );  /* put a separator */
//   }
//   SimpleConsolePrintStringCR( SCREEN_BOTTOM, caTemp );
//   printf("\n");  /* end the listing */
//}
//
//

