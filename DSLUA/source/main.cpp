// Includes
#include <stdlib.h>
#include <PA9.h>       // Include for PA_Lib
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
#include "DSLSystem.h"
#include "DSLWifi.h"
#include "DSLKeyboard.h"
#include "DSLGL.h"
//#include "resetmem.h"

// global vars
static const luaL_reg lualibs[] = {
	{"base", luaopen_base},
	{"debug", luaopen_debug},
	{"table", luaopen_table},
	{"io", luaopen_io},
	{"string", luaopen_string},
	{"math", luaopen_math},
	{"text", luaopen_DSLConsoleLib},
	{"sound", Sound_register},
	{"music", Music_register},
	{"screen", luaopen_DSLScreenLib},
	{"pads", luaopen_DSLPadsLib},
	{"stylus", luaopen_DSLStylusLib},
	{"mic", luaopen_DSLMicLib},
	{"dslua", luaopen_DSLSystemLib},
	{"wifi", luaopen_DSLWifiLib},
	{"keyboard", luaopen_DSLKeyboardLib},
	{"dsgl", luaopen_DSLGLLib},
	//  { "loadlib", luaopen_loadlib },
/* add your libraries here */
	//  LUA_EXTRALIBS
	{NULL, NULL}
};

// func defs
bool  initializeEngine(void);
void  initDisplay(void);
void  cleanUpEngine(void);
bool  showFilePickerMenu(char *, char *);

static void openstdlibs(lua_State *);
//static int  report ( lua_State*, int );
static void l_message(const char *, const char *);
//static void stackDump ( lua_State* );
int         executeLuaScript(lua_State *, const char *);

void  showDirListing(const DIR_LIST *, const int);
void  eraseDirListing();

//#define  DEBUG

//------------------------------------------------------------
//------------------------------------------------------------
int main(int argc, char * * argv)
{
	//static const char* myVersion = ZLIB_VERSION;
	//bool  bTest = ( zlibVersion()[0] == myVersion[0] );

	// init variables
	char szPath[ DS_MAX_FILENAME_LENGTH ] = "/";
	char szFile[ DS_MAX_FILENAME_LENGTH ] = "";

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
			SimpleConsoleClearScreem(SCREEN_TOP);
			SimpleConsoleClearScreem(SCREEN_BOTTOM);
			bool bSuccess = false;
			strcpy(szPath, "___FOLDER______________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________");
			strcpy(szFile, "main.lua");
			if(szPath[3] == '/' || szPath[5] == '/')
				bSuccess = true;
			if(!bSuccess)
			{
				strcpy(szPath, "/");
				strcpy(szFile, "");
				bSuccess = showFilePickerMenu(szPath, szFile);
				eraseDirListing();
			}

			// do something with the file
			if(bSuccess)
			{
				char szTarget[ DS_MAX_FILENAME_LENGTH + DS_MAX_FILENAME_LENGTH ];
				buildPath(szTarget, szPath, szFile);
				setCurrentWorkingDir(szPath);

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
				nStatus = executeLuaScript(lState, szTarget);

				// pause for a bit
				if(nStatus)
				{
					waitUntilKeyPressed((bool *)&Pad.Held.Start);
					//            } else {
					//               waitVBLDelay( 60 );
				}

				// finished script, clean up
				lua_setgcthreshold(lState, 0);
				lua_close(lState);
			}
			else
			{
				break;
			}
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
			SimpleConsoleClearScreem(SCREEN_TOP);
			SimpleConsoleClearScreem(SCREEN_BOTTOM);
			PA_InitText(SCREEN_TOP, 0);
			PA_InitText(SCREEN_BOTTOM, 0);
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
		SimpleConsoleClearScreem(SCREEN_TOP);
		SimpleConsoleClearScreem(SCREEN_BOTTOM);
		PA_InitText(SCREEN_BOTTOM, 0);
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
void drawCenteredTextBox(const int nWidth, const int nHeight)
{
	int nX1 = (DSLUA_MAX_TEXT_COLUMNS - nWidth) / 2;
	int nY1 = (DSLUA_MAX_TEXT_ROWS - nHeight) / 2;
	int nX2 = nX1 + nWidth - 1;
	int nY2 = nY1 + nHeight - 1;

	SimpleConsoleFillRectWithChar(SCREEN_BOTTOM, nX1, nY1, nX2, nY2, 'X');
	SimpleConsoleFillRectWithChar(SCREEN_BOTTOM, nX1 + 1, nY1 + 1, nX2 - 1, nY2 - 1, ' ');
}

//------------------------------------------------------------
//------------------------------------------------------------
bool showConfigMenu()
{
	bool bOrigStatus = true;
	bool bStatus = true;

	// wait for user to release SELECT button
	while(Pad.Held.Select)
	{
		PA_WaitForVBL();
	}

	// get current cf status
	DS_DEVICE_INFO * ddiTemp = getDeviceInfo("/cf");
	bStatus = ddiTemp->bAvailable;
	bOrigStatus = bStatus;

	// deal with config menu
	while(1)
	{
		PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_WHITE);
		drawCenteredTextBox(27, 8);

		PA_OutputSimpleText(SCREEN_BOTTOM, 7, 10, "FAT currently    ");
		PA_OutputSimpleText(SCREEN_BOTTOM, 4, 12, "A to toggle B to cancel");
		PA_OutputSimpleText(SCREEN_BOTTOM, 8, 13, "SELECT to return");

		PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_YELLOW);
		PA_OutputSimpleText(SCREEN_BOTTOM, 21, 10, (bStatus ? "ON" : "OFF"));
		PA_OutputSimpleText(SCREEN_BOTTOM, 4, 12, "A");
		PA_OutputSimpleText(SCREEN_BOTTOM, 16, 12, "B");
		PA_OutputSimpleText(SCREEN_BOTTOM, 8, 13, "SELECT");

		if(Pad.Newpress.A)
		{
			bStatus = !bStatus;
		}

		if(Pad.Newpress.B)
		{
			return false;
		}

		if(Pad.Newpress.Select)
		{
			break;
		}

		PA_WaitForVBL();
	}

	// set the proper status
	if(bOrigStatus != bStatus)
	{
		if(bOrigStatus)
		{
			DSFATDevice_CleanUp();
		}
		else
		{
			DSFATDevice_Init();
		}
	}
	ddiTemp->bAvailable = bStatus;

	return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool showFilePickerMenu(char * szSelectedPath, char * szSelectedFile)
{
	// read in directory list
	int nSelected = 0;
	int nRetries = 0;
	int nLoop;
	char szNewPath[ DS_MAX_FILENAME_LENGTH ];
	char szPath[ DS_MAX_FILENAME_LENGTH ] = "/";
	char szFile[ DS_MAX_FILENAME_LENGTH ] = "";
	bool bSuccess;


	// init dir variables
	// TODO: check for possible error where we can't read the directory
	if(strcmp(szSelectedPath, ""))
	{
		strcpy(szPath, szSelectedPath);
	}
	for(nLoop = 0; nLoop < g_dlList.nEntries; ++nLoop)
	{
		if(!stricmp(szSelectedFile, g_dlList.cdeDirList[ nLoop ].szFileName))
		{
			nSelected = nLoop;
			strcpy(szFile, szSelectedFile);
			break;
		}
	}
	strcpy(szFile, "");
	while(true)
	{
		bSuccess = readDirectoryList(szPath, DS_MAX_FILES_PER_DIRECTORY, &g_dlList);
		if(bSuccess)
		{
			nRetries = 0;
			break;
		}
		else
		{
			strcpy(szPath, "/");
			++nRetries;
			if(nRetries > 3)
			{
				// error: can not read root device
				return false;
			}
		}
	}

	// display dir listing on screen
	int nDelay = 0;
	showDirListing(&g_dlList, nSelected);
	while(1)
	{
		if(nDelay > 6)
		{
			// process the up and down key
			nDelay = 0;
			if(Pad.Held.Up)
			{
				if(nSelected > 0)
				{
					nSelected--;
					showDirListing(&g_dlList, nSelected);
				}
			}
			if(Pad.Held.Down)
			{
				if(nSelected < (g_dlList.nEntries - 1))
				{
					nSelected++;
					showDirListing(&g_dlList, nSelected);
				}
			}
		}

		if(Pad.Newpress.Select)
		{
			// user wants to set up configurations
			showConfigMenu();
			nSelected = 0;
			setCurrentWorkingDir("/");
			readDirectoryList("/", DS_MAX_FILES_PER_DIRECTORY, &g_dlList);
			showDirListing(&g_dlList, nSelected);
			//         if ( bReset ) {
			//            break;
			//         }
		}

		if(Pad.Newpress.A)
		{
			// wait until user releases A
			while(Pad.Held.A)
			{
				PA_WaitForVBL();
			}
			;

			// user has selected something, figure out what to do
			if(FT_FILE == g_dlList.cdeDirList[ nSelected ].ftType)
			{
				strcpy(szFile, g_dlList.cdeDirList[ nSelected ].szFileName);
				strcpy(szSelectedPath, szPath);
				strcpy(szSelectedFile, szFile);
				return true;
			}
			else
			{
				if(!strcmp(g_dlList.cdeDirList[ nSelected ].szFileName, "."))
				{
				}
				else if(!strcmp(g_dlList.cdeDirList[ nSelected ].szFileName, ".."))
				{
					// parent directory
					strcpy(szNewPath, szPath);
					stripLastDirFromPath(szNewPath);
				}
				else
				{
					buildPath(szNewPath, szPath, g_dlList.cdeDirList[ nSelected ].szFileName);
					buildPath(szNewPath, szNewPath, "");
				}
				bSuccess = readDirectoryList(szNewPath, DS_MAX_FILES_PER_DIRECTORY, &g_dlList);
				if(bSuccess)
				{
					strcpy(szPath, szNewPath);
				}
				else
				{
					bSuccess = readDirectoryList(szPath, DS_MAX_FILES_PER_DIRECTORY, &g_dlList);
				}
			}
			nSelected = 0;
			showDirListing(&g_dlList, nSelected);
		}
		nDelay++;
		PA_WaitForVBL();
	}
	return false;
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
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  initializeEngine(void)
{
	PA_Init();       // Initialize PA_Lib
	PA_InitVBL();    // Initialize a standard VBL
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
	if(!DSIODevice_Init())
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
void  cleanUpEngine(void)
{
	DSIODevice_CleanUp();
}

//------------------------------------------------------------
//------------------------------------------------------------
void eraseDirListing()
{
	initDisplay();
}

//------------------------------------------------------------
//------------------------------------------------------------
void showDirListing(const DIR_LIST * dlList, const int nSelected)
{
	int nLoop, nStartIndex;
	int nHeight = 21;
	int nWidth = 32;
	char szTemp[16];

	// erase box
	sprintf(szTemp, " DSLua %s ", getSystemVersion());
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_WHITE);
	PA_InitTextBorders(SCREEN_BOTTOM, 0, 0, (nWidth - 1), (nHeight - 1));
	PA_EraseTextBox(SCREEN_BOTTOM);
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_GREEN);
	PA_OutputSimpleText(SCREEN_BOTTOM, (nWidth - 11) / 2, 0, szTemp);

	// now show file listing
	nStartIndex = dlList->nEntries - (nHeight - 2);
	if(nStartIndex < 0)
	{
		nStartIndex = 0;
	}
	else if(nStartIndex > nSelected)
	{
		nStartIndex = nSelected;
	}

	// draw file selection arrow
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_BLUE);
	PA_OutputSimpleText(SCREEN_BOTTOM, 1, nSelected - nStartIndex + 1, ">");

	// display directory listing
	for(nLoop = 0; nLoop < (nHeight - 2); nLoop++)
	{
		if((nStartIndex + nLoop) < dlList->nEntries)
		{
			if(dlList->cdeDirList[ nStartIndex + nLoop ].ftType == FT_FILE)
			{
				int nNameLen = strlen(dlList->cdeDirList[ nLoop + nStartIndex ].szFileName);
				if((nNameLen >= 4) && (!stricmp(&dlList->cdeDirList[ nLoop + nStartIndex ].szFileName[ nNameLen - 4], ".lua")))
				{
					PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_GREEN);
				}
				else
				{
					PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_CYAN);
				}
				PA_BoxText(SCREEN_BOTTOM, 2, 1 + nLoop, (nWidth - 2), 1 + nLoop, dlList->cdeDirList[ nLoop + nStartIndex ].szFileName, (nWidth - 2 - 2));
			}
			else
			{
				PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_YELLOW);
				PA_BoxText(SCREEN_BOTTOM, 2, 1 + nLoop, (nWidth - 2), 1 + nLoop, dlList->cdeDirList[ nLoop + nStartIndex ].szFileName, (nWidth - 2 - 2));
			}
		}
		else
		{
			break;
		}
	}
	// show some instructions
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_WHITE);
	PA_OutputSimpleText(SCREEN_BOTTOM, (nWidth - 18) / 2 - 1, 21, "UP + DOWN to select");
	PA_OutputSimpleText(SCREEN_BOTTOM, (nWidth - 26) / 2 - 1, 22, "A to enter dir or run script");
	PA_OutputSimpleText(SCREEN_BOTTOM, (nWidth - 18) / 2 - 1, 23, "SELECT for configs");
	PA_SetTextTileCol(SCREEN_BOTTOM, PA_TEXT_YELLOW);
	PA_OutputSimpleText(SCREEN_BOTTOM, 6, 21, "UP");
	PA_OutputSimpleText(SCREEN_BOTTOM, 11, 21, "DOWN");
	PA_OutputSimpleText(SCREEN_BOTTOM, 2, 22, "A");
	PA_OutputSimpleText(SCREEN_BOTTOM, 6, 23, "SELECT");
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

