/**********************************
  Copyright (C) Rick Wong (Lick)
  http://licklick.wordpress.com/
***********************************/
#ifndef _ARM9_MAIN_H
#define _ARM9_MAIN_H


#include <nds.h>
#include <fat.h>
#include <cartreset.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "../../common/ndsx_brightness.h"   
#include "../../common/ndsx_firmware.h" 
//#include "../../common/ndsx_gbaslot.h"
//#include "../../common/ndsx_dsfont.h"
#include <sys/stat.h> 


#define BETWEEN(var, m1, m2) (var >= m1 && var < m2)
#define REGULATE_START(frame); {static u32 lastBeat = 0;\
                            if(lastBeat + frame < IPC->heartbeat){\
                            lastBeat = IPC->heartbeat;
#define REGULATE_END(); }}


#endif

