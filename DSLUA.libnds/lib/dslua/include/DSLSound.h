#ifndef _DSLSOUND_H_
#define _DSLSOUND_H_

//INCLUDES
#include "lua.h"//LUA Header File - Included by default

//TYPES
typedef struct 
{
	void *data;
	unsigned int size;
} Sound;

Sound *newSound(unsigned int nBits, unsigned int nSize);

//FUNCTIONS
//u8		soundGetNextChannel(Sound *sound);
void	soundPlay(Sound *sound);
void	soundRelease(Sound *sound);

ExternDeclare(Sound, Sound *);

#endif//_DSLSOUND_H_
