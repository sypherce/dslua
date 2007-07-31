#include <nds.h>//NDS Header File - Included by default
#include <dswifi9.h>//NDS Wifi Header File
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>//Standard C Library
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLWifiSocket.h"//This files own header file
#include "Utils.h"//Datatype helpers

UserdataStubs(WifiSocket, WifiSocket*);

int initTCPSocket(int * sock, char * host, int port, int mode, struct sockaddr_in m_nLocal)
{
	unsigned long ip;

	if(IS_INETADDR(host))
		ip = PA_chartoip(host);
	else
		ip = *(unsigned long *)gethostbyname(host)->h_addr_list[0];

	memset(&m_nLocal, 0, sizeof(struct sockaddr_in));
	m_nLocal.sin_family = AF_INET;
	m_nLocal.sin_port = htons(port);
	m_nLocal.sin_addr.s_addr = ip;

	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if(mode == 1)//PA_NORMAL_TCP)
	{
		if(connect(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in)) == 0)
			return 1;
	}
	else if(mode == 0)//PA_NONBLOCKING_TCP)
	{
		if(connect(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in)) == 0)
		{
			int i = 1;
			ioctl(*sock, FIONBIO, &i);
			return 1;
		}
	}


	return 0;
}

int initTCPServer(int * sock, int port, int mode, int num_connect, struct sockaddr_in m_nLocal)
{
	memset(&m_nLocal, 0, sizeof(struct sockaddr_in));
	m_nLocal.sin_family = AF_INET;
	m_nLocal.sin_port = htons(port);
	m_nLocal.sin_addr.s_addr = 0;
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	if(mode == 1)//PA_NORMAL_TCP)
	{
		bind(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
		listen(*sock, num_connect);
		return 1;
	}
	else if(mode == 0)//PA_NONBLOCKING_TCP)
	{
		bind(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
		int i = 1;
		ioctl(*sock, FIONBIO, &i);
		listen(*sock, num_connect);
		return 1;
	}

	return 0;
}

int wifiSocketConnect(WifiSocket *wifiSocket, const char * host, const int port, const int block)
{
	if(block == 2)
	{
		wifiSocket->m_nHost = (char *)host;
		wifiSocket->m_nPort = port;
		wifiSocket->m_nMode = 0;//NONBLOCKING_TCP;
		if(wifiSocket->m_nHost[0] == 0)
			return initTCPSocket(&wifiSocket->m_nSocket, wifiSocket->m_nPort, wifiSocket->m_nMode, 16, wifiSocket->m_nLocal);
		else
			return initTCPSocket(&wifiSocket->m_nSocket, wifiSocket->m_nHost, wifiSocket->m_nPort, wifiSocket->m_nMode, wifiSocket->m_nLocal);
	}
	else
	{
		wifiSocket->m_nMode = 2;//UDP
		wifiSocket->m_nPort = port;
		bzero(&wifiSocket->m_nLocal, sizeof(struct sockaddr_in));
		wifiSocket->m_nLocal.sin_family = AF_INET;
		wifiSocket->m_nLocal.sin_port = htons(wifiSocket->m_nPort);
		wifiSocket->m_nLocal.sin_addr.s_addr = INADDR_ANY;
		wifiSocket->m_nSocket = socket(AF_INET, SOCK_DGRAM, 0);
		if(bind(wifiSocket->m_nSocket, (struct sockaddr *)&wifiSocket->m_nLocal, sizeof(struct sockaddr_in)) < 0)
		{
			closesocket(wifiSocket->m_nSocket);
			return 0;
		}
		if(block)
		{
			int i = 1;
			ioctl(wifiSocket->m_nSocket, FIONBIO, &i);
		}
	}
	return 1;
}

static int l_GetRemoteIP(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);
	long rIP = (*ppTConnection)->m_nLocal.sin_addr.s_addr;

	sprintf(ip, "%i.%i.%i.%i", (int)((rIP) & 255), (int)((rIP >> 8) & 255), (int)((rIP >> 16) & 255), (int)((rIP >> 24) & 255));

	lua_pushboolean(lState, 1);
	return 1;
}

static int l_GetLocalIP(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);
	int lIP = Wifi_GetIP();

	sprintf(ip, "%i.%i.%i.%i", (int)((lIP) & 255), (int)((lIP >> 8) & 255), (int)((lIP >> 16) & 255), (int)((lIP >> 24) & 255));

	lua_pushboolean(lState, 1);
	return 1;
}

/*static int l_TConnectionAccept(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);

	int returnVal = accept(m_nSocket, (sockaddr *)&m_nLocal, (int *)sizeof(struct sockaddr_in));

	lua_pushvalue(lState, returnVal);
	return 1;
}*/

static int l_TConnectionRecv(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);

	int returnVal;
	if((*ppTConnection)->m_nMode == 2)
		returnVal = recvfrom((*ppTConnection)->m_nSocket, buffer, size, 0, (struct sockaddr *)&(*ppTConnection)->m_nLocal, (int *)sizeof(struct sockaddr_in));
	else
		returnVal = recv((*ppTConnection)->m_nSocket, buffer, size, 0);

	lua_pushvalue(lState, returnVal);
	return 1;
}

static int l_TConnectionRecvFrom(lua_State * lState)
{
	WifiSocket * *     ppTConnection = checkWifiSocket(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);

	int returnVal;
	if((*ppTConnection)->m_nMode == 2)
	{
		if(IS_INETADDR(host))
			(*ppTConnection)->m_nLocal.sin_addr.s_addr = PA_chartoip(host);
		else
			(*ppTConnection)->m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
		returnVal = recvfrom((*ppTConnection)->m_nSocket, buffer, size, 0, (struct sockaddr *)&(*ppTConnection)->m_nLocal, (int *)sizeof(struct sockaddr_in));
	}
	else
		returnVal = recv((*ppTConnection)->m_nSocket, buffer, size, 0);

	lua_pushvalue(lState, returnVal);
	return 1;
}

static int l_TConnectionSend(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);
	int returnVal;

	if((*ppTConnection)->m_nMode == 2)
		return sendto((*ppTConnection)->m_nSocket, buffer, size, 0, (struct sockaddr *)&(*ppTConnection)->m_nLocal, sizeof(struct sockaddr_in));
	else
		return send((*ppTConnection)->m_nSocket, buffer, size, 0);

	lua_pushvalue(lState, returnVal);
	return 1;
}

static int l_TConnectionSendTo(lua_State * lState)
{
	WifiSocket * *     ppTConnection = checkWifiSocket(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);
	int returnVal;

	if((*ppTConnection)->m_nMode == 2)
	{
		if(IS_INETADDR(host))
			(*ppTConnection)->m_nLocal.sin_addr.s_addr = PA_chartoip(host);
		else
			(*ppTConnection)->m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
		return sendto((*ppTConnection)->m_nSocket, buffer, size, 0, (struct sockaddr *)&(*ppTConnection)->m_nLocal, sizeof(struct sockaddr_in));
	}
	else
		return send((*ppTConnection)->m_nSocket, buffer, size, 0);

	lua_pushvalue(lState, returnVal);
	return 1;
}

static int l_TConnectionClose(lua_State * lState)
{
	WifiSocket * *  ppTConnection = checkWifiSocket(lState, 1);
	int returnVal = closesocket((*ppTConnection)->m_nSocket);

	lua_pushvalue(lState, returnVal);
	return 1;
}

static int l_WifiSocketGC(lua_State * lState)
{
	WifiSocket * *   pBGTemp = toWifiSocket(lState, 1);

	if(*pBGTemp)
	{
		delete (*pBGTemp);
		*pBGTemp = NULL;
	}
	return 0;
}

static int l_WifiSocketToString(lua_State * lState)
{
	lua_pushstring(lState, (char*)"__WIFISOCKET__");
	return 1;
}

static const luaL_reg WifiSocket_methods[] = {
	{"GetLocalIP", l_GetLocalIP},
	{"GetRemoteIP", l_GetRemoteIP},
	//{ "Accept", l_TConnectionAccept },
	{"Recv", l_TConnectionRecv},
	{"RecvFrom", l_TConnectionRecvFrom},
	{"Send", l_TConnectionSend},
	{"SendTo", l_TConnectionSendTo},
	{"Close", l_TConnectionClose},
	{0, 0}
};

static const luaL_reg WifiSocket_meta[] = {
	{"__gc", l_WifiSocketGC},
	{"__tostring", l_WifiSocketToString},
	{0, 0}
};

UserdataRegister(WifiSocket, WifiSocket_methods, WifiSocket_meta);

