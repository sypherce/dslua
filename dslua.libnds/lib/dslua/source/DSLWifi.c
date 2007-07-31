#include <nds.h>//NDS Header File - Included by default
#include <dswifi9.h>//NDS Wifi Header File
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>//Standard C Library
#include "lua.h"//LUA Header File - Included by default
#include "lauxlib.h"//LUA Auxilary Library Header File - Included by default
#include "DSLWifiSocket.h"//This files own header file
#include "DSLWifi.h"//This files own header file

//relocate


unsigned long makelong(unsigned char data1, unsigned char data2,unsigned char data3,unsigned char data4)
{
    return (data4<<24) | (data3<<16) | (data2<<8) | data1;  
}

int PA_InitSocket(int *sock,char *host,int port,int mode)
{
     unsigned long ip;
     struct     sockaddr_in    servaddr;   
     *sock = socket(AF_INET, SOCK_STREAM, 0);
     if(IS_INETADDR(host))
       ip = PA_chartoip(host);
     else
       ip = *(unsigned long *)gethostbyname(host)->h_addr_list[0];
       
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(port);     
     servaddr.sin_addr.s_addr = ip;
     
     if(mode == 1)//PA_NORMAL_TCP)
     {
     if(connect(*sock, (struct sockaddr *) &servaddr, sizeof(servaddr))==0)
        return 1;
     }
     else if(mode == 0)//PA_NONBLOCKING_TCP)
     {
     if(connect(*sock, (struct sockaddr *) &servaddr, sizeof(servaddr))==0)
        {
        int i = 1;
        ioctl(*sock, FIONBIO, &i);
        return 1;
        }
     }
     
        
return 0;
}

int PA_InitServer(int *sock,int port,int mode, int num_connect)
{
    int i;
     struct     sockaddr_in    servaddr;   
     *sock = socket(AF_INET, SOCK_STREAM, 0);
       
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(port);     
     servaddr.sin_addr.s_addr = 0;
     
     if(mode == 1)//PA_NORMAL_TCP)
     {
        bind(*sock,(struct sockaddr *)&servaddr,sizeof(servaddr));
        listen(*sock,num_connect);
        return 1;
     }
     else if(mode == 0)//PA_NONBLOCKING_TCP)
     {
        bind(*sock,(struct sockaddr *)&servaddr,sizeof(servaddr));
        i = 1;
        ioctl(*sock, FIONBIO, &i);
        listen(*sock,num_connect);
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

int search_word(char *mot1, char *mot2, int depart){
	int i,j,erreur=1;
	for(i=depart;i<strlen(mot1);i++){
	
		if(mot1[i]==mot2[0]){
			erreur=0;
			
			for(j=0;j<strlen(mot2);j++) 
				if(mot2[j]!=mot1[i+j]) 
					erreur=1;
					
			if(erreur==0)
				return i;
		}
	}
	return -1;
}



int get_HTTP_serveur(char *buffer, char *buffer2)
{
    int i,depart=0;
    if(search_word(buffer2,"http://",0)!=-1)
       depart+=7;
    for(i = depart; buffer2[i] != '\0' && buffer2[i] != '/' && buffer2[i] != '\\'; i++)
    {
          buffer[i-depart]=buffer2[i];
    }
    buffer[i-depart]='\0';
return i;
}

int PA_GetHTTP(char *buffer, char *adress)
{
    int sock;
    char serveur[256];
    char buffer3[256];
    int pos = get_HTTP_serveur(serveur,adress);
    PA_InitSocket(&sock,serveur,80,0);//PA_NONBLOCKING_TCP);
    char buffer2[256];
    sprintf(buffer2, "GET %s HTTP/1.1\r\nhost: %s\r\nAccept: */*\r\n\r\n",adress+pos,serveur);
    send(sock,buffer2,256,0);
    strcpy(buffer,"");
    while(search_word(buffer3,"\r\n\r\n",0) == -1)
    {
       memset(buffer3,0,sizeof(buffer3));
       recv(sock,buffer3,256,0);
    }
    int poshtml = search_word(buffer3,"\r\n\r\n",0)+4;
    strcat(buffer,buffer3+poshtml);
    while(search_word(buffer,"</html>",0) == -1 && search_word(buffer,"</HTML>",0) == -1)
    {
    memset(buffer3,0,sizeof(buffer3));
    if(recv(sock,buffer3,256,0)<1)
       break;
    strcat(buffer,buffer3);
    }
    if(sock)
       closesocket(sock);
return 1;
}
//reloacte

static int l_ConnectWFC(lua_State * lState)
{

	int i;
	int returnVal;

	Wifi_AutoConnect(); // request connect

	while(1)
	{
		i=Wifi_AssocStatus(); // check status
		if(i==ASSOCSTATUS_ASSOCIATED)
		{
			returnVal = true;
			break;
		}
		if(i==ASSOCSTATUS_CANNOTCONNECT)
		{
			returnVal = false;
			break;
		}
	}

	lua_pushvalue(lState, returnVal);
	return returnVal;
}

static int l_Disconnect(lua_State * lState)
{
	int returnVal = Wifi_DisconnectAP();

	Wifi_DisableWifi();
	lua_pushvalue(lState, returnVal);
	return returnVal;
}

static int l_GetHTTP(lua_State * lState)
{
	char * address = (char *)luaL_checkstring(lState, 1);
	char * buffer = (char *)luaL_checkstring(lState, 2);

	PA_GetHTTP(buffer, address);
	lua_pushboolean(lState, 1);
	return 1;
}

static int l_TConnectionInitSocket(lua_State * lState)
{
	WifiSocket **ppTConnection = NULL;
	ppTConnection = pushWifiSocket(lState);
	*ppTConnection = (WifiSocket*)malloc(sizeof(WifiSocket));
	const char *             host = luaL_checkstring(lState, 1);
	int port = luaL_checkint(lState, 2);


	// now initialize it
	int returnVal = wifiSocketConnect(ppTConnection, (char *)host, port, 2);

	return returnVal;
}

static int l_TConnectionInitUDP(lua_State * lState)
{
	WifiSocket **ppTConnection = NULL;
	ppTConnection = pushWifiSocket(lState);
	*ppTConnection = (WifiSocket*)malloc(sizeof(WifiSocket));
	int port = luaL_checkint(lState, 1);
	int mode = luaL_checkint(lState, 2);

	// now initialize it
	int returnVal = wifiSocketConnect(ppTConnection, (char*)NULL, port, mode);

	return returnVal;
}

static const struct luaL_reg DSLWifiLib [] =
{
	{"ConnectWFC", l_ConnectWFC},
	{"GetHTTP", l_GetHTTP},
	{"InitTCP", l_TConnectionInitSocket},
	{"InitUDP", l_TConnectionInitUDP},
	{"Disconnect", l_Disconnect},
	{NULL, NULL}
};

int luaopen_DSLWifiLib(lua_State * lState)
{
	// register the wifi library
//	SocketConnection_register(lState);
//	UDPConnection_register(lState);
	luaL_openlib(lState, "Wifi", DSLWifiLib, 0);
	return 1;
}

