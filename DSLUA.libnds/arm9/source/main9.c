#include <fat.h>
#include <nds.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <dswifi9.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "DSLPads.h"
#include "DSLStylus.h"
#include "DSLMic.h"
#include "DSLSystem.h"

#define VCOUNT (*((u16 volatile *) 0x04000006))

void *sgIP_malloc(int size) { return malloc(size); }
void sgIP_free(void * ptr) { free(ptr); }
void sgIP_dbgprint(char * txt, ...) {}
void Timer_50ms(void)
{
	Wifi_Timer(50);
}

void arm9_synctoarm7()//send fifo message
{
	REG_IPC_FIFO_TX=0x87654321;
}

inline bool NDSX_ARM9_WifiSync(u32 msg)
{
	if(msg==0x87654321)
	{
		Wifi_Sync();
		return true;
	}
	return false;
}

void arm9_fifo()//check incoming fifo messages
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
    {
        u32 msg = REG_IPC_FIFO_RX;  

        // Pass message through handlers --Lick
		if(NDSX_ARM9_WifiSync(msg)) continue;
        //if(NDSX_ARM7_BrightnessFifo(msg)) continue;
    }
}

//extern int luaopen_DSLMicLib(lua_State *lState);
//extern int luaopen_DSLPadsLib(lua_State *lState);

#define __dolibrary__
#ifdef __dolibrary__
static int dolibrary (lua_State *L, const char *name)
{
	if(0) return 1;
	//if(strcmp(name, "text") == 0) luaopen_(L);
	//else if(strcmp(name, "sound") == 0) luaopen_(L);
	//else if(strcmp(name, "music") == 0) luaopen_(L);
	//else if(strcmp(name, "screen") == 0) luaopen_(L);
	//else if(strcmp(name, "pads") == 0) luaopen_(L);
	//else if(strcmp(name, "stylus") == 0) luaopen_(L);
	else if(strcmp(name, "mic") == 0) luaopen_DSLMicLib(L);
	else if(strcmp(name, "pads") == 0) luaopen_DSLPadsLib(L);
	else if(strcmp(name, "stylus") == 0) luaopen_DSLStylusLib(L);
	else if(strcmp(name, "dslua") == 0) luaopen_DSLSystemLib(L);
	//else if(strcmp(name, "dslua") == 0) luaopen_(L);
	//else if(strcmp(name, "wifi") == 0) luaopen_(L);
	else return 1;
	return 0;
}
#else
static int dolibrary (lua_State *L, const char *name)
{
	lua_getglobal(L, "require");
	lua_pushstring(L, name);
	return report(L, lua_pcall(L, 1, 0, 0));
}
#endif//zero

touchPosition touch;
int vblankCount = 0;

void vblFunction()
{
	scanKeys();
	touch=touchReadXY();
	vblankCount++;//Test this
}

/*
** $Id: luac.c,v 1.54 2006/06/02 17:37:11 lhf Exp $
** Lua compiler (saves bytecodes to files; also list bytecodes)
** See Copyright Notice in lua.h
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define luac_c
#define LUA_CORE

#include "lua.h"
#include "lauxlib.h"

#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"

static void fatal(const char* message)
{
 fprintf(stderr,"%s: %s\n","DSLua 0.8",message);
 while(1);//exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	videoSetMode(0);//not using the main screen
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);//sub bg 0 will be used to print text
	vramSetBankC(VRAM_C_SUB_BG);

	SUB_BG0_CR = BG_MAP_BASE(31);

	BG_PALETTE_SUB[255] = RGB15(31,31,31);//by default font will be rendered with color 255

	//consoleInit() is a lot more flexible but this gets you up and running quick
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

	iprintf("Syncing CPUs\n");

	{//send fifo message to initialize the arm7 wifi
		REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;//enable & clear FIFO

		u32 Wifi_pass= Wifi_Init(WIFIINIT_OPTION_USELED);
		REG_IPC_FIFO_TX=0x12345678;
		REG_IPC_FIFO_TX=Wifi_pass;

		*((volatile u16 *)0x0400010E) = 0;//disable timer3

		irqInit(); 
		irqSet(IRQ_TIMER3, Timer_50ms);//setup timer IRQ
		irqEnable(IRQ_TIMER3);
		irqSet(IRQ_FIFO_NOT_EMPTY, arm9_fifo);//setup fifo IRQ
		irqEnable(IRQ_FIFO_NOT_EMPTY);

		REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;//enable FIFO IRQ

		Wifi_SetSyncHandler(arm9_synctoarm7);//tell wifi lib to use our handler to notify arm7

		//set timer3
		*((volatile u16 *)0x0400010C) = -6553;//6553.1 * 256 cycles = ~50ms;
		*((volatile u16 *)0x0400010E) = 0x00C2;//enable, irq, 1/256 clock

		while(Wifi_CheckInit()==0)//wait for arm7 to be initted successfully
		{
			while(VCOUNT>192);//wait for vblank
			while(VCOUNT<192);
		}

	}//wifi init complete - wifi lib can now be used!
	
	iprintf("Initializing Fat Device\n");
	fatInitDefault();
		irqSet(IRQ_VBLANK, vblFunction);//setup timer IRQ
		irqEnable(IRQ_VBLANK);



 //struct Smain s;
 //int i=doargs(argc,argv);
 //argc-=i; argv+=i;
 //if (argc<=0) usage("no input files given");
 lua_State *L = lua_open();  /* create state */
	if (L == NULL)
	{
		//l_message("DSLUA 0.7", "cannot create state: not enough memory");
		return EXIT_FAILURE;
	}
	lua_gc(L, LUA_GCSTOP, 0);  /* stop collector during initialization */
	luaL_openlibs(L);  /* open libraries */
	//dolibrary(L, "mic");
	dolibrary(L, "mic");
	dolibrary(L, "pads");
	dolibrary(L, "stylus");
	dolibrary(L, "dslua");
	lua_gc(L, LUA_GCRESTART, 0);
 if (L==NULL) fatal("not enough memory for state");
 //s.argc=argc;
 //s.argv=argv;
 //if (lua_cpcall(L,pmain,&s)!=0) fatal(lua_tostring(L,-1));
	iprintf("Executing Lua Script\n");

 if (luaL_dofile(L,"/main.lua")!=0) fatal(lua_tostring(L,-1));
 lua_close(L);
 return EXIT_SUCCESS;
}

