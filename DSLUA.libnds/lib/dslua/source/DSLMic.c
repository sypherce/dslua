#include <nds.h>
#include "lua.h"
#include "lauxlib.h"
#include "DSLMic.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_StartRecording( lua_State* lState )
{
   u8 *buffer = (u8*)luaL_checkstring(lState, 1);
   int length = luaL_checkint(lState, 2);
   //!PA_MicStartRecording (buffer, length);
   lua_pushboolean( lState, 1 );
   return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_GetVol( lua_State* lState )
{
	printf("hello");
   lua_pushnumber( lState, 0/*!PA_MicGetVol()*/ );
   return 1;
}

static int l_Replay( lua_State* lState )
{
   u8 channel = luaL_checkint(lState, 1);
   u8 *buffer = (u8*)luaL_checkstring(lState, 2);
   s32 length = luaL_checkint(lState, 3);
   //*PA_MicReplay(channel,buffer,length);
   lua_pushboolean( lState, 1 );
   return 1;
}

static const struct luaL_reg DSLMicLib [] = {
   { "StartRecording", l_StartRecording },
   { "GetVol", l_GetVol },
   { "Replay", l_Replay },
   { NULL, NULL }
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLMicLib( lua_State* lState )
{
   luaL_openlib( lState, "Mic", DSLMicLib, 0 );
   return 1;
}

