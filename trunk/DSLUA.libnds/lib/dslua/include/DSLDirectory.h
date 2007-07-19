#ifndef _DSLDIRECTORY_H_
#define _DSLDIRECTORY_H_

//INCLUDES
#include "lua.h"//LUA Header File - Included by default
#include <sys/dir.h>//system dir.h
#include <sys/unistd.h>//system unistd.h

//VARIABLES
extern DIR_ITER* dp;
extern struct stat fStat;

//FUNCTIONS
int luaopen_DSLDirectoryLib(lua_State *);//Enables the Directory Functions

#endif // _DSLDIRECTORY_H_
