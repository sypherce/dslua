#ifndef _DSLWIFISOCKET_H_
#define _DSLWIFISOCKET_H_

//DEFINES
#define isdigit(c) (c >= '0' && c <= '9')
#define IS_DIGIT_OR_DOT(c) (isdigit(c) || (c == '.'))
#define IS_INETADDR(s) (IS_DIGIT_OR_DOT(s[0]) && IS_DIGIT_OR_DOT(s[1]) && IS_DIGIT_OR_DOT(s[2]) && IS_DIGIT_OR_DOT(s[3]) && IS_DIGIT_OR_DOT(s[4]) && IS_DIGIT_OR_DOT(s[5]) && IS_DIGIT_OR_DOT(s[6]))

//INCLUDES
#include "lua.h"//LUA Header File - Included by default

//TYPES
typedef struct 
{
	int m_nSocket;
	char *m_nHost;
	int m_nPort;
	int m_nMode;
	struct sockaddr_in m_nLocal;
} WifiSocket;

//-------------------------
//-------------------------
ExternDeclare(WifiSocket, WifiSocket*);


#endif//_DSLWIFISOCKET_H_
