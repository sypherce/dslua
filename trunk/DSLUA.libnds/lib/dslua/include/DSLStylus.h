#ifndef _DSLSTYLUS_H_
#define _DSLSTYLUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nds.h>
#include "lua.h"

extern touchPosition touch;
int luaopen_DSLStylusLib( lua_State* );

#ifdef __cplusplus
}	   // extern "C"
#endif

#endif // _DSLSTYLUS_H_
