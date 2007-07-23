#ifndef _DSLSYSTEM_H_
#define _DSLSYSTEM_H_

//DEFINES
//System configuration defines
#define INFO_COLOR *(u8*)0x027FFC82
#define INFO_BDAY_MONTH *(u8*)0x027FFC83
#define INFO_BDAY_DAY *(u8*)0x027FFC84
#define INFO_ALARM_HOUR *(u8*)0x027FFCD2
#define INFO_ALARM_MINUTE *(u8*)0x027FFCD3
#define INFO_NAME *(u8*)0x027FFC86
#define INFO_NAME_LENGTH *(u8*)0x027FFC9A
#define INFO_MESSAGE *(u8*)0x027FFC9C
#define INFO_MESSAGE_LENGTH *(u8*)0x027FFCD0
#define INFO_LANGUAGE *(u8*)(0x027FFCE4)

//INCLUDES
#include "lua.h"//LUA Header File - Included by default

//VARIABLES
extern int vblankCount;//Counter for VBlanks since program start

//FUNCTIONS
int luaopen_DSLSystemLib(lua_State*);//Enables the System Functions

#endif // _DSLSYSTEM_H_
