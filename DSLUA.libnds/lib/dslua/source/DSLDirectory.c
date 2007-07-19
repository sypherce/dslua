#include <nds.h>//NDS Header File - Included by default
#include <errno.h>//errno declaration
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLDirectory.h"//This files own header file

DIR_ITER* dp;
struct stat fStat;

static int l_DirectoryOpen(lua_State * lState)
{
	const char *    szFName = luaL_checkstring(lState, 1);
	int returnVal = 1;
	dp = diropen (szFName);
	if(dp == NULL)
		returnVal = errno;

	lua_pushnumber(lState, returnVal);

	return 1;
}

static int l_DirectoryNext(lua_State * lState)
{
	char *    szFName = (char*)malloc(256+16);
	int returnVal = dirnext(dp, (char*)szFName, &fStat);
	if(returnVal != 0)
		lua_pushstring(lState, "");
	else
		lua_pushstring(lState, szFName);
	free(szFName);

	return 1;
}

static int l_DirectoryReset(lua_State * lState)
{
	int returnVal = dirreset(dp);
	if(returnVal == -1)
		returnVal = errno;
	lua_pushnumber(lState, returnVal);

	return 1;
}

static int l_DirectoryClose(lua_State * lState)
{
	int returnVal = dirclose(dp);
	if(returnVal == -1)
		returnVal = errno;
	lua_pushnumber(lState, returnVal);

	return 1;
}

static int l_DirectoryChdir(lua_State * lState)
{
	const char *    szFName = luaL_checkstring(lState, 1);
	lua_pushnumber(lState, chdir (szFName));

	return 1;
}

static int l_DirectoryIsDir(lua_State * lState)
{
	lua_pushnumber(lState, (int)(fStat.st_mode & S_IFDIR));

	return 1;
}

static int l_DirectoryIsFile(lua_State * lState)
{
	lua_pushnumber(lState, !(int)(fStat.st_mode & S_IFDIR));

	return 1;
}

static const struct luaL_reg DSLDirectoryLib[] =
{
	{"Open", l_DirectoryOpen},
	{"Next", l_DirectoryNext},
	{"Reset", l_DirectoryReset},
	{"Close", l_DirectoryClose},
	{"Chdir", l_DirectoryChdir},
	{"IsDir", l_DirectoryIsDir},
	{"IsFile", l_DirectoryIsFile},
	{NULL, NULL}
};

int luaopen_DSLDirectoryLib(lua_State * lState)
{
	luaL_openlib(lState, "Directory", DSLDirectoryLib, 0);

	return 1;
}
