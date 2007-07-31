/*
  Functions for the ARM7 to process the commands from the ARM9.Based
  on code from the MOD player example posted to the GBADEV forums.
  Chris Double (chris.double@double.co.nz)
  http://www.double.co.nz/nintendo_ds
*/
#include <nds.h>
#include "command.h"
#include "tobmic.h"

static void CommandPlayOneShotSample(PlaySampleSoundCommand* ps)
{
	int channel = ps->channel;

	SCHANNEL_CR(channel) = 0;
	SCHANNEL_TIMER(channel) = SOUND_FREQ(ps->frequency);
	SCHANNEL_SOURCE(channel) = (uint32)ps->data;
	SCHANNEL_LENGTH(channel) = ps->length >> 2;
	SCHANNEL_PAN(channel) = 100;
	SCHANNEL_CR(channel) =
	    SCHANNEL_ENABLE |
	    SOUND_ONE_SHOT |
	    SOUND_16BIT |
	    SOUND_PAN(64) |
	    SOUND_VOL(ps->volume);
}

static void CommandStartRecording(StartRecordingCommand* sr)
{
	tob_StartRecording(sr->buffer, sr->length);
	commandControl->return_data = 0;
}

static void CommandStopRecording()
{
	commandControl->return_data = tob_StopRecording();
}

void CommandProcessCommands()
{
	static int currentCommand = 0;
	while(currentCommand != commandControl->currentCommand) {
		Command* command = &commandControl->command[currentCommand];

		switch(command->commandType) {
		case PLAY_ONE_SHOT_SAMPLE:
			CommandPlayOneShotSample(&command->playSample);
			break;
		case START_RECORDING:
			CommandStartRecording(&command->startRecording);
			break;
		case STOP_RECORDING:
			CommandStopRecording();
		}
		currentCommand++;
		currentCommand &= MAX_COMMANDS-1;
	}
}
