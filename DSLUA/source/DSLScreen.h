#ifndef _DSLSCREEN_H_
#define _DSLSCREEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"

//------------------------------------------------------------
//------------------------------------------------------------
void screenWaitForVBL();
void resetAllGraphics();
int luaopen_DSLScreenLib( lua_State* );

#ifdef __cplusplus
}	   // extern "C"
#endif

#endif // _DSLSCREEN_H_
