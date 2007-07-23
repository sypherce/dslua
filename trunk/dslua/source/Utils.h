#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"

#define  countof(__x)   (sizeof(__x) / sizeof(__x[0]))

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

char const * const getSystemVersion();
#ifndef __SOCKET__
bool                 strStartsWith(const char *, const char *, bool bCaseSensitive = true);
bool                 strEndsWith(const char *, const char *, bool bCaseSensitive = true);
void                 waitVBLDelay(const int);
void                 waitUntilKeyPressed(bool *);
void                 waitUntilDMAFree(int u8);
unsigned int         calcCheckSum(const void *, const int);

class AlignedMemory {
public:
AlignedMemory(unsigned int, unsigned int);
~AlignedMemory();
void * vpAlignedMem;
#ifdef DEBUG
private:
#endif
void *          vpRealMem;
unsigned int m_nSize;
unsigned int m_nRealSize;
private:
AlignedMemory();
};
#endif

#ifdef __cplusplus
}          // extern "C"
#endif

#endif // _UTILS_H_

