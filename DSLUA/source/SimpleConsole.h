#ifndef _SIMPLECONSOLE_H_
#define _SIMPLECONSOLE_H_

#define  SCREEN_BOTTOM     0
#define  SCREEN_TOP        1

#define  DSLUA_MAX_TEXT_COLUMNS        32
#define  DSLUA_MAX_TEXT_ROWS           24
#define  DSLUA_MAX_TEXT_COUNT    ( DSLUA_MAX_TEXT_COLUMNS * DSLUA_MAX_TEXT_ROWS )

#define  PA_TEXT_WHITE        0
#define  PA_TEXT_RED          1
#define  PA_TEXT_GREEN        2
#define  PA_TEXT_BLUE         3
#define  PA_TEXT_MAGENTA      4
#define  PA_TEXT_CYAN         5
#define  PA_TEXT_YELLOW       6
#define  PA_TEXT_LIGHT_GRAY   7
#define  PA_TEXT_DARK_GRAY    8
#define  PA_TEXT_BLACK        9

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"

extern int   g_nSimpleConsoleNextX[2];
extern int   g_nSimpleConsoleNextY[2];

void  SimpleConsoleFillRectWithChar( int, int, int, int, int, const char );
void  SimpleConsoleClearScreem( int );
void  SimpleConsoleSetColor( int, int );
int   SimpleConsolePrintString( int, const char* );
int   SimpleConsolePrintStringCR( int, const char* );
int   SimpleConsolePrintLine( int );
void  stackDump( lua_State*, int );

int luaopen_DSLConsoleLib( lua_State* );

#ifdef __cplusplus
}	   // extern "C"
#endif

#endif  // _SIMPLECONSOLE_H_
