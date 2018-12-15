// Network.h: interface for the CNetwork class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__19C7FECD_93EA_4809_9B83_4E0330849642__INCLUDED_)
#define AFX_NETWORK_H__19C7FECD_93EA_4809_9B83_4E0330849642__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ¼­¹öÆ÷Æ®¼±¾ð.

extern BOOL g_bReady;

// 080813 LUJ, 수정된 inetwork.dll 참조위해 수정
#include "..\4DYUCHINET_COMMON\inetwork.h"
//#include "ServerSystem.h"
#ifdef _AGENTSERVER
#include "UserTable.h"
#endif

class CNetwork  
{
	I4DyuchiNET* m_pINet;
	friend class CDataBase;
public:
	CNetwork();
	virtual ~CNetwork();
	
	I4DyuchiNET* GetINet()	{	return m_pINet;	}

	void Init(DESC_NETWORK * desc);
	void Release();

	//void Start();
	//void BatchConnectToOtherServer();

	BOOL StartServerServer(char * szIP, int port);
	BOOL StartUserServer(char * szIP, int port);
	BOOL ConnectToServer(char * szIP, int port, void * pParam);

	void GetUserAddress(DWORD dwConnectionIndex, char* ip, WORD * port);
	
	//--------- send, recv process --------------------------------
	// map port·Î Ã£´Â °ÍÀº Á÷Á¢ ÄÚµùÇÒ°Í
	void Send2Server(DWORD dwConnectionIndex, char * msg, DWORD size);
	void Send2AgentServer(char * msg, DWORD size);
	// desc_hseos_주민등록_01
	// S 주민등록 추가 added by hseos 2007.12.31
	// ..특정 에이전트 서버에 보내기
	void Send2SpecificAgentServer(char * msg, DWORD size);
	// E 주민등록 추가 added by hseos 2007.12.31

	// 080407 LYW --- Network : Add function to send message to distribute server.
	void Send2DistributeServer(char* pMsg, DWORD dwLength) ;
	void Send2AgentExceptThis(char* pMsg, DWORD dwLength) ;
	
	void Send2User(DWORD dwConnectionIndex, char * msg, DWORD size);
#ifdef _AGENTSERVER
	void Send2User(MSGBASE * msg, DWORD size);
#endif
#ifndef _MAPSERVER_
	void EnCrypt(USERINFO * userInfo, char* msg,DWORD size);
#endif
	void Broadcast2Server(char * msg, DWORD size);
	void Broadcast2User(MSGBASE * msg, DWORD size);
	void Broadcast2MapServer(char * msg, DWORD size);
	void Broadcast2MapServerExceptOne(DWORD dwConnectionIndex, char * msg, DWORD size);
	void Broadcast2AgentServer(char* msg, DWORD size);
	void Broadcast2AgentServerExceptOne(DWORD dwConnectionIndex, char* msg, DWORD size);
	void Broadcast2AgentServerExceptSelf(char* msg, DWORD size);
	//-------------------------------------------------------------
	
	void DisconnectUser(DWORD dwConnectionIndex);

};

extern CNetwork g_Network;
#endif // !defined(AFX_NETWORK_H__19C7FECD_93EA_4809_9B83_4E0330849642__INCLUDED_)
