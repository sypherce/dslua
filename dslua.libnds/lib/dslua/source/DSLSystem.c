#include <nds.h>//NDS Header File - Included by default
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLSystem.h"//This files own header file
#include "ndsx_brightness.h"//brightness\light declarations

static int l_WaitForAnyKey(lua_State* lState)
{
	while(1)
	{
		swiWaitForVBlank();
		if(	(keysHeld()&KEY_A) || (keysHeld()&KEY_B) || (keysHeld()&KEY_X) || (keysHeld()&KEY_Y) ||
			(keysHeld()&KEY_R) || (keysHeld()&KEY_L) ||
			(keysHeld()&KEY_UP) || (keysHeld()&KEY_DOWN) || (keysHeld()&KEY_LEFT) || (keysHeld()&KEY_RIGHT) ||
			(keysHeld()&KEY_SELECT) || (keysHeld()&KEY_START))
		break;
	}

	return 0;
}

static int l_VBCount(lua_State* lState)
{
	lua_pushnumber(lState, vblankCount);//NOTE: Test this

	return 1;
}

static int l_BDM( lua_State* lState)
{
	lua_pushnumber(lState, INFO_BDAY_MONTH);

	return 1;
}

static int l_BDD( lua_State* lState)
{
	lua_pushnumber(lState, INFO_BDAY_DAY);

	return 1;
}

static int l_RTCYear(lua_State* lState)
{
	lua_pushnumber(lState, IPC->time.rtc.year + 2000);

	return 1;
}

static int l_RTCHour(lua_State* lState)
{
	lua_pushnumber(lState, (IPC->time.rtc.hours < 12) ? IPC->time.rtc.hours : IPC->time.rtc.hours - 40);

	return 1;
}

static int l_RTCMin(lua_State* lState)
{
	lua_pushnumber(lState, IPC->time.rtc.minutes);

	return 1;
}

static int l_RTCSec(lua_State* lState)
{
	lua_pushnumber(lState, IPC->time.rtc.seconds);

	return 1;
}

static int l_UserName(lua_State* lState)
{
	int i = 0;
	u8 Name[21];
	u8 NameLength = INFO_NAME_LENGTH;
	for (i = 0; i < NameLength; i++)
		Name[i] = *(u8*)(0x027FFC86 + (i << 1));
	Name[i] = 0;
	lua_pushstring(lState, Name);

	return 1;
}

static int l_UserMessage(lua_State* lState)
{
	int i = 0;
	u8 Message[21];
	u8 MessageLength = INFO_MESSAGE_LENGTH;
	for (i = 0; i < MessageLength; i++)
		Message[i] = *(u8*)(0x027FFC9C + (i << 1));
	Message[i] = 0;
	lua_pushstring(lState, Message);

	return 1;
}

static int l_Language(lua_State* lState)
{
	lua_pushnumber(lState, INFO_LANGUAGE&7);

	return 1;
}

static int l_getehcolor(lua_State* lState)
{
	lua_pushnumber(lState, INFO_COLOR);

	return 1;
}

static int l_alarmhour(lua_State* lState)
{
	lua_pushnumber(lState, INFO_ALARM_HOUR);

	return 1;
}

static int l_alarmmin(lua_State* lState)
{
	lua_pushnumber(lState, INFO_ALARM_MINUTE);

	return 1;
}

static int l_DSLuaVersion(lua_State* lState)
{
	lua_pushstring(lState, "1.0");

	return 1;
}

static int l_SwitchScreens(lua_State* lState)
{
	lcdSwap();

	return 1;
}

static int l_LidClosed(lua_State* lState)
{
	lua_pushboolean(lState, (keysHeld()&KEY_LID));

	return 1;
}

static int l_SetScreenLight(lua_State* lState)
{
	int screen = luaL_checkint(lState, 1);
	int light = luaL_checkint(lState, 2);

	if(screen == 0)
	{
		if(light == 0)
			NDSX_SetBottomBacklight_Off();
		else if(light == 1)
			NDSX_SetBottomBacklight_On();
	}
	else if(screen == 1)
	{
		if(light == 0)
			NDSX_SetTopBacklight_Off();
		else if(light == 1)
			NDSX_SetTopBacklight_On();
	}

	return 1;
}

static int l_SetDSLBrightness(lua_State* lState)
{
	int brightness = luaL_checkint(lState, 1);
	if(brightness == 0)
		NDSX_SetBrightness_0();
	else if(brightness == 1)
		NDSX_SetBrightness_1();
	else if(brightness == 2)
		NDSX_SetBrightness_2();
	else if(brightness == 3)
		NDSX_SetBrightness_3();
	//PA_SetDSLBrightness(brightness);//lua_pushboolean( lState, PA_LidClosed() );

	return 1;
}

static int l_SetLedBlink(lua_State* lState)
{
	int blink = luaL_checkint(lState, 1);
	int speed = luaL_checkint(lState, 2);
	
	//PA_SetLedBlink(blink, speed);//lua_pushboolean( lState, PA_LidClosed() );

	return 1;
}

static const struct luaL_reg DSLSystemLib[] =
{
   {"WaitForAnyKey", l_WaitForAnyKey},
   {"VBlankCount", l_VBCount},
   {"Year", l_RTCYear},
   {"Hour", l_RTCHour},
   {"Min", l_RTCMin},
   {"AlarmHour", l_alarmhour},
   {"AlarmMin", l_alarmmin},
   {"Sec", l_RTCSec},
   {"UserName", l_UserName},
   {"UserMessage", l_UserMessage},
   {"BirthMonth", l_BDM},
   {"BirthDay", l_BDD},
   {"Color", l_getehcolor},
   {"Language", l_Language},
   {"Version", l_DSLuaVersion},
   {"SwitchScreens", l_SwitchScreens},
   {"LidClosed", l_LidClosed},
   {"SetDSLBrightness", l_SetDSLBrightness},
   {"SetScreenLight", l_SetScreenLight},
   {"SetLedBlink", l_SetLedBlink},
   {NULL, NULL}
};

int luaopen_DSLSystemLib(lua_State* lState)
{
	luaL_openlib(lState, "DSLua", DSLSystemLib, 0);

	return 1;
}
