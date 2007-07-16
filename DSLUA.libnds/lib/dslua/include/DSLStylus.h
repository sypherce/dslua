#ifndef _DSLSTYLUS_H_
#define _DSLSTYLUS_H_

//INCLUDES
#include <nds.h>//Included for the touchPosition Declaration
#include "lua.h"//LUA Header File - Included by default

//VARIABLES
extern touchPosition touch;//Declaration of the main stylus variable

//FUNCTIONS
int luaopen_DSLStylusLib( lua_State* );//Enables the Stylus Functions

#endif // _DSLSTYLUS_H_
