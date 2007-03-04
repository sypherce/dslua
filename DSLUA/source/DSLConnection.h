#ifndef _DSLCONNECTION_H_
#define _DSLCONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Utils.h"

int luaopen_DSLWifiLib( lua_State* );

//------------------------------------------------------------
//----- Connection base class
//------------------------------------------------------------
class Connection
{
public:
   enum           ConnectionType{ CONNECTION_SOCKET, CONNECTION_SOCKET_NON_BLOCKING, CONNECTION_SERVER, CONNECTION_SERVER_NON_BLOCKING, CONNECTION_UDP };

public:
   const ConnectionType         m_connectionType;

   virtual const char   *toString();
   virtual              ~Connection() = 0;

protected:
   int  m_nSocket;
   char *m_nHost;
   int   m_nPort;
   int   m_nMode;

   Connection( const ConnectionType m_connectionType );

private:
   Connection();
};

//------------------------------------------------------------
//----- SocketConnection class
//------------------------------------------------------------
class SocketConnection: public Connection
{
public:
   SocketConnection();
   ~SocketConnection();

   int connect(const char*, const int);
   void disconnect();
   int getLocalIP(char *);
   int getRemoteIP(char *);
   int accept_();
   int send_(char*,int);
   int sendto_(char *, char*,int);
   int recv_(char*,int);
   int recvfrom_(char *, char*,int);
   int close_();
   struct sockaddr_in m_nLocal;

   const char* toString();
};

//------------------------------------------------------------
//----- UDPConnection class
//------------------------------------------------------------
class UDPConnection: public Connection
{
public:
   UDPConnection();
   ~UDPConnection();

   int connect(const int, const int);
   void disconnect();
   int getLocalIP(char *);
   int getRemoteIP(char *);
   int sendto_(char*, char*,int);
   int send_(char*,int);
   int recvfrom_(char*, char*,int);
   int recv_(char*,int);
   int close_();
   sockaddr_in m_nLocal;

   const char* toString();
};


//-------------------------
//-------------------------
ExternDeclare( Connection, Connection* );
ExternDeclare( SocketConnection, SocketConnection* );
ExternDeclare( UDPConnection, UDPConnection* );

#define AVAILABLE_CONNECTIONS   "SocketConnection", "UDPConnection"

#ifdef __cplusplus
}	   // extern "C"
#endif

#endif // _DSLCONNECTION_H_
