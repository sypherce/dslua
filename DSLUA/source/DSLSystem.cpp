#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "DSLScreen.h"
#include "DSLSystem.h"
#include "DSLPads.h"
#include "DSLVersion.h"
#include "Utils.h"

//------------------------------------------------------------
//------------------------------------------------------------
static int l_WaitForAnyKey(lua_State * lState)
{
    bool bAnyKey;

    while(1)
    {
        screenWaitForVBL();
        bAnyKey = Pad.Held.A || Pad.Held.B || Pad.Held.X || Pad.Held.Y ||
                  Pad.Held.R || Pad.Held.L ||
                  Pad.Held.Up || Pad.Held.Down || Pad.Held.Left || Pad.Held.Right ||
                  Pad.Held.Select || Pad.Held.Start;
        if(bAnyKey)
        {
            break;
        }
    }

    return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_VBCount(lua_State * lState)
{
    lua_pushnumber(lState, 0 /************88IPC->heartbeat*********/);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCYear(lua_State * lState)
{
//   PA_UpdateRTC();
//   screenWaitForVBL();
//   screenWaitForVBL();
//   PA_UpdateRTC();
//   lua_pushnumber( lState, IPC->curtime[1] + IPC->curtime[2] + IPC->curtime[1] );
//   lua_pushnumber( lState, PA_RTC.Year );
    lua_pushnumber(lState, PA_RTC.Year);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCMonth(lua_State * lState)
{
    lua_pushnumber(lState, PA_RTC.Month);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCDay(lua_State * lState)
{
    lua_pushnumber(lState, PA_RTC.Day);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCHour(lua_State * lState)
{
    lua_pushnumber(lState, PA_RTC.Hour);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCMin(lua_State * lState)
{
    lua_pushnumber(lState, PA_RTC.Minutes);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_RTCSec(lua_State * lState)
{
    lua_pushnumber(lState, PA_RTC.Seconds);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UserName(lua_State * lState)
{
    char szName[32];

    strncpy(szName, ((char *)(&PA_UserInfo.Name[0])), PA_UserInfo.NameLength);
    szName[ PA_UserInfo.NameLength ] = '\0';
    lua_pushstring(lState, szName);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UserMsg(lua_State * lState)
{
    char szMsg[64];

    strncpy(szMsg, ((char *)(&PA_UserInfo.Message[0])), PA_UserInfo.MessageLength);
    szMsg[ PA_UserInfo.MessageLength ] = '\0';
    lua_pushstring(lState, szMsg);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UserLang(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.Language);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UserColor(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.Color);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_BirthMonth(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.BdayMonth);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_BirthDay(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.BdayDay);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_AlarmHour(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.AlarmHour);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_AlarmMin(lua_State * lState)
{
    lua_pushnumber(lState, PA_UserInfo.AlarmMinute);
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_DSLuaVersion(lua_State * lState)
{
    lua_pushstring(lState, getSystemVersion());
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SwitchScreens(lua_State * lState)
{
    PA_SwitchScreens(); //lua_pushboolean( lState, PA_LidClosed() );
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_LidClosed(lua_State * lState)
{
    lua_pushboolean(lState, PA_LidClosed());
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SetScreenLight(lua_State * lState)
{
    int screen = luaL_checkint(lState, 1);
    int light = luaL_checkint(lState, 2);

    PA_SetScreenLight(screen, light); //lua_pushboolean( lState, PA_LidClosed() );
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SetDSLBrightness(lua_State * lState)
{
    int brightness = luaL_checkint(lState, 1);

    PA_SetDSLBrightness(brightness); //lua_pushboolean( lState, PA_LidClosed() );
    return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SetLedBlink(lua_State * lState)
{
    int blink = luaL_checkint(lState, 1);
    int speed = luaL_checkint(lState, 2);

    PA_SetLedBlink(blink, speed); //lua_pushboolean( lState, PA_LidClosed() );
    return 1;
}

static const struct luaL_reg DSLSystemLib [] =
{
    {"WaitForAnyKey", l_WaitForAnyKey},
    {"VBlankCount", l_VBCount},
    {"Year", l_RTCYear},
    {"Month", l_RTCMonth},
    {"Day", l_RTCDay},
    {"Hour", l_RTCHour},
    {"Min", l_RTCMin},
    {"AlarmHour", l_AlarmHour},
    {"AlarmMin", l_AlarmMin},
    {"Sec", l_RTCSec},
    {"UserName", l_UserName},
    {"UserMessage", l_UserMsg},
    {"BirthMonth", l_BirthMonth},
    {"BirthDay", l_BirthDay},
    {"Color", l_UserColor},
    {"Language", l_UserLang},
    {"Version", l_DSLuaVersion},
    {"SwitchScreens", l_SwitchScreens},
    {"LidClosed", l_LidClosed},
    {"SetDSLBrightness", l_SetDSLBrightness},
    {"SetScreenLight", l_SetScreenLight},
    {"SetLedBlink", l_SetLedBlink},
    {NULL, NULL}
};

//------------------------------------------------------------
//------------------------------------------------------------
int luaopen_DSLSystemLib(lua_State * lState)
{
    luaL_openlib(lState, "DSLua", DSLSystemLib, 0);

    return 1;
}

