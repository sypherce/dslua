#include <nds.h>
#include <stdlib.h>
#include <dswifi7.h>

void startSound(int sampleRate, const void* data, u32 bytes, u8 channel, u8 vol,  u8 pan, u8 format)
{
	SCHANNEL_TIMER(channel)  = SOUND_FREQ(sampleRate);
	SCHANNEL_SOURCE(channel) = (u32)data;
	SCHANNEL_LENGTH(channel) = bytes >> 2 ;
	SCHANNEL_CR(channel)     = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(vol) | SOUND_PAN(pan) | (format==1?SOUND_8BIT:SOUND_16BIT);
}


s32 getFreeSoundChannel()
{
	int i;
	for(i=0; i<16; i++)
	{
		if((SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0) return i;
	}
	return -1;
}

int vcount;
touchPosition first,tempPos;

void VcountHandler()
{
	static int lastbut = -1;
	uint8 ct[sizeof(IPC->time.rtc)];
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if(!((but ^ lastbut) & (1<<6)))
	{
		tempPos = touchReadXY();
		if(tempPos.x == 0 || tempPos.y == 0)
		{
			but |= (1 <<6);
			lastbut = but;
		}
		else
		{
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
	}
	else
	{
		lastbut = but;
		but |= (1 <<6);
	}

	if(vcount == 80)
	{
		first = tempPos;
	}
	else
	{
		if(abs(xpx - first.px) > 10 || abs(ypx - first.py) > 10 || (but & (1<<6)))
		{
			but |= (1 <<6);
			lastbut = but;
		}
		else
		{ 	
			IPC->mailBusy = 1;
			IPC->touchX			= x;
			IPC->touchY			= y;
			IPC->touchXpx		= xpx;
			IPC->touchYpx		= ypx;
			IPC->touchZ1		= z1;
			IPC->touchZ2		= z2;
			IPC->mailBusy = 0;
		}
	}
    // Read the time
    rtcGetTime((uint8 *)ct);
    BCDToInteger((uint8 *)&(ct[1]), 7); 
	u8 i;
	u8 *temp;
	temp = (u8*)&IPC->time.rtc;
	for(i=0; i<sizeof(ct); i++)
	{
		temp[i] = ct[i];
	} 

	IPC->buttons		= but;
	vcount ^= (80 ^ 130);
	SetYtrigger(vcount);
}

void VblankHandler(void)
{
	u32 i;

	//sound code  :)
	TransferSound *snd = IPC->soundData;
	IPC->soundData = 0;

	if(0 != snd)
	{
		for(i=0; i<snd->count; i++)
		{
			s32 chan = getFreeSoundChannel();

			if(chan >= 0)
			{
				startSound(snd->data[i].rate, snd->data[i].data, snd->data[i].len, chan, snd->data[i].vol, snd->data[i].pan, snd->data[i].format);
			}
		}
	}

	Wifi_Update();
}

void arm7_synctoarm9()//send fifo message
{
	REG_IPC_FIFO_TX = 0x87654321;
}

inline bool NDSX_ARM7_WifiSync(u32 msg)
{
	if(msg==0x87654321)
	{
		Wifi_Sync();
		return true;
	}
	return false;
}

void arm7_fifo()//check incoming fifo messages
{
    while(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY))
    {
        u32 msg = REG_IPC_FIFO_RX;  

        // Pass message through handlers --Lick
		if(NDSX_ARM7_WifiSync(msg)) continue;
        //if(NDSX_ARM7_BrightnessFifo(msg)) continue;
    }
}


int main(int argc, char ** argv)
{
	u32 fifo_temp;   
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

	// Reset the clock if needed
	rtcReset();

	//enable sound
	powerON(POWER_SOUND);
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);
	IPC->soundData = 0;

	irqInit();
	irqSet(IRQ_VBLANK, VblankHandler);
	SetYtrigger(80);
	vcount = 80;
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);

	irqSet(IRQ_WIFI, Wifi_Interrupt);
	irqEnable(IRQ_WIFI);



	// trade some mail, to get a pointer from arm9
	while(1)
	{
		while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
		fifo_temp=REG_IPC_FIFO_RX;
		if(fifo_temp==0x12345678) break;
	}
	while(REG_IPC_FIFO_CR&IPC_FIFO_RECV_EMPTY) swiWaitForVBlank();
	fifo_temp=REG_IPC_FIFO_RX;
	Wifi_Init(fifo_temp);

	irqSet(IRQ_FIFO_NOT_EMPTY,arm7_fifo);
	irqEnable(IRQ_FIFO_NOT_EMPTY);
	REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

	Wifi_SetSyncHandler(arm7_synctoarm9);

	// Keep the ARM7 idle
	while(1) swiWaitForVBlank();
}
