#include <typeinfo>
#include <assert.h>
#include <PA9.h>       // Include for PA_Lib

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include "Utils.h"
#include "DSLConnection.h"

//------------------------------------------------------------
//----- Connection base class
//------------------------------------------------------------
UserdataStubs(Connection, Connection *);

Connection::Connection(const ConnectionType connectionType)
	:  m_connectionType(connectionType),
    m_nSocket(-1)
{
}
#define isdigit(c) (c >= '0' && c <= '9')
#define IS_DIGIT_OR_DOT(c) (isdigit(c) || (c == '.'))
#define IS_INETADDR(s) (IS_DIGIT_OR_DOT(s[0]) && IS_DIGIT_OR_DOT(s[1]) && IS_DIGIT_OR_DOT(s[2]) && IS_DIGIT_OR_DOT(s[3]) && IS_DIGIT_OR_DOT(s[4]) && IS_DIGIT_OR_DOT(s[5]) && IS_DIGIT_OR_DOT(s[6]))

int PA_InitTCPSocket(int * sock, char * host, int port, int mode, struct sockaddr_in m_nLocal)
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
	if(mode == PA_NORMAL_TCP)
	{
		if(connect(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in)) == 0)
			return 1;
	}
	else if(mode == PA_NONBLOCKING_TCP)
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

int PA_InitTCPServer(int * sock, int port, int mode, int num_connect, struct sockaddr_in m_nLocal)
{
	memset(&m_nLocal, 0, sizeof(struct sockaddr_in));
	m_nLocal.sin_family = AF_INET;
	m_nLocal.sin_port = htons(port);
	m_nLocal.sin_addr.s_addr = 0;
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	if(mode == PA_NORMAL_TCP)
	{
		bind(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
		listen(*sock, num_connect);
		return 1;
	}
	else if(mode == PA_NONBLOCKING_TCP)
	{
		bind(*sock, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
		int i = 1;
		ioctl(*sock, FIONBIO, &i);
		listen(*sock, num_connect);
		return 1;
	}
/*
   		while(!sock)
   		{
   		   sock=accept(*sock,(struct sockaddr *)&servaddr,&i);
   		}
 */

	return 0;
}

/*int PA_InitUDPSocket(int *sock,char *host,int port,int mode, struct sockaddr_in m_nLocal)
   {
   	 unsigned long ip;
 *sock = socket(AF_INET, SOCK_DGRAM, 0);
   	 if(IS_INETADDR(host))
   	   ip = PA_chartoip(host);
   	 else
   	   ip = *(unsigned long *)gethostbyname(host)->h_addr_list[0];

   		   memset(&m_nLocal, 0, sizeof(struct sockaddr_in));
   	 m_nLocal.sin_family = AF_INET;
   	 m_nLocal.sin_port = htons(port);
   	 m_nLocal.sin_addr.s_addr = ip;

   	 if(mode == PA_NORMAL_TCP)
   	 {
   		bind(*sock,(struct sockaddr *)&m_nLocal,sizeof(struct sockaddr_in));
   		return 1;
   	 }
   	 else if(mode == PA_NONBLOCKING_TCP)
   	 {
   		bind(*sock,(struct sockaddr *)&m_nLocal,sizeof(struct sockaddr_in));
   		int i = 1;
   		ioctl(*sock, FIONBIO, &i);
   		return 1;
   	 }

   return 0;
   }

   int PA_InitUDPServer(int *sock,int port,int mode, struct sockaddr_in m_nLocal)
   {
 *sock = socket(AF_INET, SOCK_DGRAM, 0);

   		   memset(&m_nLocal, 0, sizeof(struct sockaddr_in));
   	 m_nLocal.sin_family = AF_INET;
   	 m_nLocal.sin_port = htons(port);
   	 m_nLocal.sin_addr.s_addr = INADDR_ANY;

   	 if(mode == PA_NORMAL_TCP)
   	 {
   		bind(*sock,(struct sockaddr *)&m_nLocal,sizeof(struct sockaddr_in));
   		return 1;
   	 }
   	 else if(mode == PA_NONBLOCKING_TCP)
   	 {
   		bind(*sock,(struct sockaddr *)&m_nLocal,sizeof(struct sockaddr_in));
   		int i = 1;
   		ioctl(*sock, FIONBIO, &i);
   		return 1;
   	 }

   return 0;
   }


   int UDPConnection::connect(const char *host, const int port)
   {
   		m_nHost = (char*)host;
   		m_nPort = port;
   		m_nMode = PA_NONBLOCKING_TCP;
   		if(m_nHost[0] == 0)
   				return PA_InitUDPServer(&m_nSocket,m_nPort,m_nMode, m_nLocal);
   		else
   				return PA_InitUDPSocket(&m_nSocket,m_nHost,m_nPort,m_nMode, m_nLocal);
   }*/
int UDPConnection::connect(const int port, const int block)
{
	m_nPort = port;
	bzero(&m_nLocal, sizeof(struct sockaddr_in));
	m_nLocal.sin_family = AF_INET;
	m_nLocal.sin_port = htons(m_nPort);
	m_nLocal.sin_addr.s_addr = INADDR_ANY;
	/*if(strcmp((char*)host, "") != 0)
	   {
	   m_nLocal.sin_addr.s_addr = PA_chartoip((char*)host);
	   }*/
	m_nSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(bind(m_nSocket, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in)) < 0)
	{
		closesocket(m_nSocket);
		return 0;
	}
	if(block)
	{
		int i = 1;
		ioctl(m_nSocket, FIONBIO, &i);
	}
	return 1;
}
int SocketConnection::connect(const char * host, const int port)
{
	m_nHost = (char *)host;
	m_nPort = port;
	m_nMode = PA_NONBLOCKING_TCP;
	if(m_nHost[0] == 0)
		return PA_InitTCPServer(&m_nSocket, m_nPort, m_nMode, 16, m_nLocal);
	else
		return PA_InitTCPSocket(&m_nSocket, m_nHost, m_nPort, m_nMode, m_nLocal);
}

Connection::~Connection()
{
}

const char * Connection::toString()
{
	return "__CONNECTION__";
}

static int l_ConnectionGC(lua_State * lState)
{
	Connection * *   pConnectionTemp = toConnection(lState, 1);

	if(*pConnectionTemp)
	{
		delete (*pConnectionTemp);
		*pConnectionTemp = NULL;
	}
	return 0;
}

static int l_ConnectionToString(lua_State * lState)
{
	lua_pushstring(lState, (*toConnection(lState, 1))->toString());
	return 1;
}

static const luaL_reg Connection_methods[] = {
	{0, 0}
};
static const luaL_reg Connection_meta[] = {
	{"__gc", l_ConnectionGC},
	{"__tostring", l_ConnectionToString},
	{0, 0}
};

UserdataRegister(Connection, Connection_methods, Connection_meta);

//------------------------------------------------------------
//----- SocketConnection class
//------------------------------------------------------------
UserdataStubs(SocketConnection, SocketConnection *);

//------------------------------------------------------------
//----- UDPConnection class
//------------------------------------------------------------
UserdataStubs(UDPConnection, UDPConnection *);

SocketConnection::SocketConnection()
	:  Connection(CONNECTION_SOCKET)
{
}

UDPConnection::UDPConnection()
	:  Connection(CONNECTION_UDP)
{
}

int SocketConnection::getRemoteIP(char * ip)
{
	long rIP = m_nLocal.sin_addr.s_addr;

	sprintf(ip, "%i.%i.%i.%i", (int)((m_nLocal.sin_addr.s_addr) & 255), (int)((m_nLocal.sin_addr.s_addr >> 8) & 255), (int)((m_nLocal.sin_addr.s_addr >> 16) & 255), (int)((m_nLocal.sin_addr.s_addr >> 24) & 255));
	return 1;
}

int UDPConnection::getRemoteIP(char * ip)
{
	long rIP = m_nLocal.sin_addr.s_addr;

	sprintf(ip, "%i.%i.%i.%i", (int)((rIP) & 255), (int)((rIP >> 8) & 255), (int)((rIP >> 16) & 255), (int)((rIP >> 24) & 255));
	return 1;
}

int SocketConnection::getLocalIP(char * ip)
{
	int lIP = Wifi_GetIP();

	sprintf(ip, "%i.%i.%i.%i", (int)((lIP) & 255), (int)((lIP >> 8) & 255), (int)((lIP >> 16) & 255), (int)((lIP >> 24) & 255));
	return 1;
}

int UDPConnection::getLocalIP(char * ip)
{
	int lIP = Wifi_GetIP();

	sprintf(ip, "%i.%i.%i.%i", (int)((lIP) & 255), (int)((lIP >> 8) & 255), (int)((lIP >> 16) & 255), (int)((lIP >> 24) & 255));
	return 1;
}

int SocketConnection::accept_( )
{
	return accept(m_nSocket, (sockaddr *)&m_nLocal, (int *)sizeof(struct sockaddr_in));
}

int SocketConnection::recv_(char * buffer, int size)
{
	return recv(m_nSocket, buffer, size, 0);
}

int SocketConnection::recvfrom_(char * host, char * buffer, int size)
{
	/*if(IS_INETADDR(host))
	   m_nLocal.sin_addr.s_addr = PA_chartoip(host);
	   else
	   m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
	   return recvfrom(m_nSocket, buffer, size, 0, (struct sockaddr*)&m_nLocal, (int*)sizeof(struct sockaddr_in));*/
	return recv(m_nSocket, buffer, size, 0);
}

int UDPConnection::recvfrom_(char * host, char * buffer, int size)
{
	if(IS_INETADDR(host))
		m_nLocal.sin_addr.s_addr = PA_chartoip(host);
	else
		m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
	return recvfrom(m_nSocket, buffer, size, 0, (struct sockaddr *)&m_nLocal, (int *)sizeof(struct sockaddr_in));
}

int UDPConnection::recv_(char * buffer, int size)
{
	return recvfrom(m_nSocket, buffer, size, 0, (struct sockaddr *)&m_nLocal, (int *)sizeof(struct sockaddr_in));
}

int SocketConnection::send_(char * buffer, int size)
{
	return send(m_nSocket, buffer, size, 0);
}

int SocketConnection::sendto_(char * host, char * buffer, int size)
{
	/*if(IS_INETADDR(host))
	   m_nLocal.sin_addr.s_addr = PA_chartoip(host);
	   else
	   m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
	   return sendto(m_nSocket,buffer,size,0,(struct sockaddr *)&m_nLocal,sizeof(struct sockaddr_in));*/
	return send(m_nSocket, buffer, size, 0);
}

int UDPConnection::sendto_(char * host, char * buffer, int size)
{
	if(IS_INETADDR(host))
		m_nLocal.sin_addr.s_addr = PA_chartoip(host);
	else
		m_nLocal.sin_addr.s_addr = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
	return sendto(m_nSocket, buffer, size, 0, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
}

int UDPConnection::send_(char * buffer, int size)
{
	return sendto(m_nSocket, buffer, size, 0, (struct sockaddr *)&m_nLocal, sizeof(struct sockaddr_in));
}

int SocketConnection::close_( )
{
	return closesocket(m_nSocket);
}

int UDPConnection::close_( )
{
	return closesocket(m_nSocket);
}

SocketConnection::~SocketConnection()
{
	// delete connection
	if(m_nSocket >= 0)
	{
		closesocket(m_nSocket);
	}
}

UDPConnection::~UDPConnection()
{
	// delete connection
	if(m_nSocket >= 0)
	{
		closesocket(m_nSocket);
	}
}

const char * SocketConnection::toString()
{
	return "__TCPCONNECTION__";
}

const char * UDPConnection::toString()
{
	return "__UDPCONNECTION__";
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UDPGetRemoteIP(lua_State * lState)
{
	UDPConnection * *  ppTConnection = checkUDPConnection(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);

	(*ppTConnection)->getRemoteIP(ip);
	lua_pushboolean(lState, 1);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TCPGetRemoteIP(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);

	(*ppTConnection)->getRemoteIP(ip);
	lua_pushboolean(lState, 1);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UDPGetLocalIP(lua_State * lState)
{
	UDPConnection * *  ppTConnection = checkUDPConnection(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);

	(*ppTConnection)->getLocalIP(ip);
	lua_pushboolean(lState, 1);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TCPGetLocalIP(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);
	char *  ip = (char *)luaL_checkstring(lState, 2);

	(*ppTConnection)->getLocalIP(ip);
	lua_pushboolean(lState, 1);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionAccept(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);

	int returnVal = (*ppTConnection)->accept_();

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionRecv(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);

	int returnVal = (*ppTConnection)->recv_((char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionRecvFrom(lua_State * lState)
{
	SocketConnection * *     ppTConnection = checkSocketConnection(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);

	int returnVal = (*ppTConnection)->recvfrom_((char *)host, (char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_TUDPRecvFrom(lua_State * lState)
{
	UDPConnection * *     ppTConnection = checkUDPConnection(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);

	int returnVal = (*ppTConnection)->recvfrom_((char *)host, (char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}


//------------------------------------------------------------
//------------------------------------------------------------
static int l_TUDPRecv(lua_State * lState)
{
	UDPConnection * *     ppTConnection = checkUDPConnection(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);

	int returnVal = (*ppTConnection)->recv_((char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionSend(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);
	int returnVal = (*ppTConnection)->send_((char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionSendTo(lua_State * lState)
{
	SocketConnection * *     ppTConnection = checkSocketConnection(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);
	int returnVal = (*ppTConnection)->sendto_((char *)host, (char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TUDPSendTo(lua_State * lState)
{
	UDPConnection * *     ppTConnection = checkUDPConnection(lState, 1);
	const char *             host = luaL_checkstring(lState, 2);
	const char *             buffer = luaL_checkstring(lState, 3);
	int size = luaL_checkint(lState, 4);
	int returnVal = (*ppTConnection)->sendto_((char *)host, (char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TUDPSend(lua_State * lState)
{
	UDPConnection * *     ppTConnection = checkUDPConnection(lState, 1);
	const char *             buffer = luaL_checkstring(lState, 2);
	int size = luaL_checkint(lState, 3);

	int returnVal = (*ppTConnection)->send_((char *)buffer, size);

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TConnectionClose(lua_State * lState)
{
	SocketConnection * *  ppTConnection = checkSocketConnection(lState, 1);
	int returnVal = (*ppTConnection)->close_();

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_TUDPClose(lua_State * lState)
{
	UDPConnection * *  ppTConnection = checkUDPConnection(lState, 1);
	int returnVal = (*ppTConnection)->close_();

	lua_pushvalue(lState, returnVal);
	return 1;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_SocketConnectionGC(lua_State * lState)
{
	SocketConnection * *   pBGTemp = toSocketConnection(lState, 1);

	if(*pBGTemp)
	{
		delete (*pBGTemp);
		*pBGTemp = NULL;
	}
	return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
static int l_UDPConnectionGC(lua_State * lState)
{
	UDPConnection * *   pBGTemp = toUDPConnection(lState, 1);

	if(*pBGTemp)
	{
		delete (*pBGTemp);
		*pBGTemp = NULL;
	}
	return 0;
}

static int l_SocketConnectionToString(lua_State * lState)
{
	lua_pushstring(lState, (*toSocketConnection(lState, 1))->toString());
	return 1;
}

static int l_UDPConnectionToString(lua_State * lState)
{
	lua_pushstring(lState, (*toUDPConnection(lState, 1))->toString());
	return 1;
}

static const luaL_reg SocketConnection_methods[] = {
	{"GetLocalIP", l_TCPGetLocalIP},
	{"GetRemoteIP", l_TCPGetRemoteIP},
	//{ "Accept", l_TConnectionAccept },
	{"Recv", l_TConnectionRecv},
	{"RecvFrom", l_TConnectionRecvFrom},
	{"Send", l_TConnectionSend},
	{"SendTo", l_TConnectionSendTo},
	{"Close", l_TConnectionClose},
	{0, 0}
};

static const luaL_reg UDPConnection_methods[] = {
	{"GetLocalIP", l_UDPGetLocalIP},
	{"GetRemoteIP", l_UDPGetRemoteIP},
	{"RecvFrom", l_TUDPRecvFrom},
	{"Recv", l_TUDPRecv},
	{"SendTo", l_TUDPSendTo},
	{"Send", l_TUDPSend},
	{"Close", l_TUDPClose},
	{0, 0}
};

static const luaL_reg SocketConnection_meta[] = {
	{"__gc", l_SocketConnectionGC},
	{"__tostring", l_SocketConnectionToString},
	{0, 0}
};

static const luaL_reg UDPConnection_meta[] = {
	{"__gc", l_UDPConnectionGC},
	{"__tostring", l_UDPConnectionToString},
	{0, 0}
};

UserdataRegister(SocketConnection, SocketConnection_methods, SocketConnection_meta);

UserdataRegister(UDPConnection, UDPConnection_methods, UDPConnection_meta);


