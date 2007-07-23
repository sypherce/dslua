#ifndef _DSLDIRECTORY_H_
#define _DSLDIRECTORY_H_

#include <sys/dir.h>
#include <sys/unistd.h>
#include "lua.h"
#include "Utils.h"



#ifdef __cplusplus
extern "C" {
#endif

extern DIR_ITER* dp;
extern struct stat fStat;

int luaopen_DSLDirectoryLib(lua_State *);

#ifdef __cplusplus
}          // extern "C"
#endif

#endif // _DSLDIRECTORY_H_
