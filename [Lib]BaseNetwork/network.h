#pragma once

#include <WinSock2.h>
#include "define.h"

class CIndexCreator;
class CConnection;

class CNetwork
{
	CIndexCreator*			m_pIndexCreator;
	HWND					m_hProcWnd;
	CConnection**			m_ppConnectionTable;
	WORD					m_wPort;
	DWORD					m_dwConnectionNum;
	DWORD					m_dwMaxConnectionNum;
	SOCKET					m_socketListen;
	sockaddr_in				m_sckadrAdress;
	WORD					m_wSockEventWinMsgID;				// Added by chan78 at 2002/05/28

	void					InitSocket();
	void					ReleaseSocket();
	BOOL					PrepareRead(DWORD	dwIndex);
	DWORD					AddConnection(SOCKET s);

public:
	WORD					GetWinMsgID()						{ return m_wSockEventWinMsgID; };

	void					CloseConnection(DWORD dwIndex);
	void					CloseAllConnection();
    void					(*ReceivedMsg)(DWORD dwInex,char* pMsg,DWORD dwLen);
	void					(*OnDisconnect)(DWORD dwInex);
	void					(*OnConnect)(DWORD dwInex);
	SOCKET					GetListenSocket()					{return m_socketListen;}
	DWORD					GetConnectionIndex( CConnection* );
	WORD					GetPort() {return m_wPort;}
	BOOL					Send(DWORD dwConnectionIndex,char* msg,DWORD length);
	DWORD					ConnectToServer(char* szIP,WORD port);
	BOOL					StartServer(char* ip,WORD port);
	void					CompulsiveDisconnect(DWORD dwIndex);
	CConnection*			GetConnection(DWORD dwConnectionIndex);
	friend	void			SocketEventProc1();

	CNetwork( HWND hWnd,DWORD dwMaxConnectionNum, WORD wSockEventWinMsgID );
	~CNetwork();
};

extern CNetwork* g_pNetwork;