#include <nds.h>
#include "lua.h"
#include "lauxlib.h"
#include "DSLStylus.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusNewPress( lua_State* lState )
{
   lua_pushboolean( lState, keysDown()&KEY_TOUCH ) ;
   return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusDown( lua_State* lState )
{
   lua_pushboolean( lState, keysHeld()&KEY_TOUCH ) ;
   return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_StylusReleased( lua_State* lState )
{
   lua_pushboolean( lState, keysUp()&KEY_TOUCH ) ;
   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_PosX( lua_State* lState )
{
   lua_pushnumber( lState, touch.px ) ;
   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_PosY( lua_State* lState )
{
   lua_pushnumber( lState, touch.py ) ;
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

