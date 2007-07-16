#include <typeinfo>
#include <assert.h>
#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "DSLConnection.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_ConnectWFC( lua_State* lState )
{
   int returnVal = PA_ConnectWifiWFC();
   lua_pushvalue( lState, returnVal ) ;
   return returnVal;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_Disconnect( lua_State* lState )
{
   int returnVal = Wifi_DisconnectAP();
   Wifi_DisableWifi();
   lua_pushvalue( lState, returnVal ) ;
   return returnVal;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_GetHTTP( lua_State* lState )
{
   char *address = (char*)luaL_checkstring( lState, 1 );
   char *buffer = (char*)luaL_checkstring( lState, 2 );

   PA_GetHTTP(buffer, address);
   lua_pushboolean( lState, 1) ;
   return 1;
}
//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionInitSocket( lua_State* lState )
{
   SocketConnection**  ppTConnection    = (SocketConnection**)new SocketConnection();
   const char*  	   host			    = luaL_checkstring( lState, 1 );
   int 				   port 		    = luaL_checkint( lState, 2 );
	 
   ppTConnection = pushSocketConnection( lState );

   // now initialize it
   *ppTConnection   = new SocketConnection();
   int returnVal    = (*ppTConnection)->connect( (char*)host,port );

   return returnVal;
}
//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionInitUDP( lua_State* lState )
{
   UDPConnection**  ppTConnection    = (UDPConnection**)new UDPConnection();
   int				   port    			= luaL_checkint( lState, 1 );
   int				   mode    			= luaL_checkint( lState, 2 );
	 
   ppTConnection = pushUDPConnection( lState );

   // now initialize it
   *ppTConnection   = new UDPConnection();
   int returnVal    = (*ppTConnection)->connect(port,mode);

   return returnVal;
}
static const struct luaL_reg DSLWifiLib [] = {
   { "ConnectWFC", l_ConnectWFC },
   { "GetHTTP", l_GetHTTP },
   { "InitTCP", l_TConnectionInitSocket },
   { "InitUDP", l_TConnectionInitUDP },
   { "Disconnect", l_Disconnect },
   { NULL, NULL }
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLWifiLib( lua_State* lState )
{
   // register the wifi library
   SocketConnection_register( lState );
   UDPConnection_register( lState );
   luaL_openlib( lState, "Wifi", DSLWifiLib, 0 );
   return 1;
}
