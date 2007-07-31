#ifndef _DSLMUSIC_H_
#define _DSLMUSIC_H_

//INCLUDES
#include "lua.h"//LUA Header File - Included by default

//VARIABLES
bool modUpdate;

//TYPES
typedef MODULE Music;

Music *newMusic(unsigned int nBits, unsigned int nSize);

//FUNCTIONS
//u8		musicGetNextChannel(Music *music);
void	musicPlay(Music *music);
void	musicRelease(Music *music);

ExternDeclare(Music, Music *);

#endif//_DSLMUSIC_H_
