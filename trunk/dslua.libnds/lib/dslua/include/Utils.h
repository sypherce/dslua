#ifndef _UTILS_H_
#define _UTILS_H_

#include "lua.h"

#define ExternDeclare(HANDLE, DATATYPE) \
    extern DATATYPE * check ## HANDLE(lua_State *, int); \
    extern DATATYPE * to ## HANDLE(lua_State *, int); \
    extern DATATYPE * push ## HANDLE(lua_State *); \
    extern int HANDLE ## _register(lua_State *);

#define UserdataStubs(HANDLE, DATATYPE) \
    DATATYPE * check ## HANDLE(lua_State * L, int nIndex) {\
		void * ud = luaL_checkudata(L, nIndex, # HANDLE); \
		luaL_argcheck(L, ud != NULL, nIndex, "`##HANDLE' expected"); \
		return (DATATYPE *)ud; \
	} \
    DATATYPE * to ## HANDLE(lua_State * L, int index) \
	{\
		DATATYPE * handle = (DATATYPE *)lua_touserdata(L, index); \
		if(handle == NULL) luaL_typerror(L, index, # HANDLE);\
		return handle; \
	} \
    DATATYPE * push ## HANDLE(lua_State * L) {\
		DATATYPE * newvalue = (DATATYPE *)lua_newuserdata(L, sizeof(DATATYPE)); \
		luaL_getmetatable(L, # HANDLE); \
		lua_pushvalue(L, -1); \
		lua_setmetatable(L, -3); \
		lua_pushstring(L, "__index"); \
		lua_pushstring(L, # HANDLE); \
		lua_gettable(L, LUA_GLOBALSINDEX); \
		lua_settable(L, -3); \
		lua_pop(L, 1); \
		return newvalue; \
	}

#define UserdataRegister(HANDLE, METHODS, METAMETHODS) \
    int HANDLE ## _register(lua_State * L) {\
		luaL_newmetatable(L, # HANDLE);  /* create new metatable */ \
		lua_pushliteral(L, "__index"); \
		lua_pushvalue(L, -2);  /* push metatable */ \
		lua_rawset(L, -3);  /* metatable.__index = metatable */ \
        \
		luaL_openlib(L, 0, METAMETHODS, 0); \
		luaL_openlib(L, # HANDLE, METHODS, 0); \
        \
		lua_pushstring(L, # HANDLE); \
		lua_gettable(L, LUA_GLOBALSINDEX); \
		luaL_getmetatable(L, # HANDLE); \
		lua_setmetatable(L, -2); \
		return 1; \
	}

#endif//_UTILS_H_

