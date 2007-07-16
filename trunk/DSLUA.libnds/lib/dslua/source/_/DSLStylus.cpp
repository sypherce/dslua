#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLStylus.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusNewPress( lua_State* lState )
{
   lua_pushboolean( lState, Stylus.Newpress ) ;
   return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusDown( lua_State* lState )
{
   lua_pushboolean( lState, Stylus.Held ) ;
   return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusReleased( lua_State* lState )
{
   lua_pushboolean( lState, Stylus.Released ) ;
   return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_PosX( lua_State* lState )
{
   lua_pushnumber( lState, Stylus.X ) ;
   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_PosY( lua_State* lState )
{
   lua_pushnumber( lState, Stylus.Y ) ;
   return 1;
}

static const struct luaL_reg DSLStylusLib [] = {
   { "NewPress", l_StylusNewPress },
   { "Down", l_StylusDown },
   { "Released", l_StylusReleased },
   { "X", l_PosX },
   { "Y", l_PosY },
   { NULL, NULL }
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLStylusLib( lua_State* lState )
{
   luaL_openlib( lState, "Stylus", DSLStylusLib, 0 );
   return 1;
}

