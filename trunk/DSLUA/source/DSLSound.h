#ifndef _DSLSOUND_H_
#define _DSLSOUND_H_

#include "lua.h"
#include "Utils.h"

//------------------------------------------------------------
//----- Sound class
//------------------------------------------------------------
class Sound
{
public:
Sound(AlignedMemory *);
~Sound();

void        play();
void        release();

const char * toString();
private:
AlignedMemory * m_apSound;
static u8 m_nNextChannel;

Sound();
u8          getNextChannel();
};

ExternDeclare(Sound, Sound *);

#endif // _DSLSOUND_H_
