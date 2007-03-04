#ifndef _DSLMUSIC_H_
#define _DSLMUSIC_H_

#include "lua.h"
#include "Utils.h"

//------------------------------------------------------------
//----- Music class
//------------------------------------------------------------
class Music
{
public:
   Music( AlignedMemory* );
   ~Music();

   void        play();
   void        pause( bool );
   void        stop();
   void        release();

   const char* toString();
private:
   AlignedMemory* m_apMusic;
   static u8      m_nNextChannel;

   Music();
};

ExternDeclare( Music, Music* );

#endif // _DSLMUSIC_H_
