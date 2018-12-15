#include "stdafx.h"
#include "Network.h"
#include "Network_guid.h"

#include "ServerTable.h"
#ifdef _AGENTSERVER
#include "TrafficLog.h"
#include "UserTable.h"
#endif

#include "../hseos/Debug/SHDebug.h"

BOOL g_bReady = FALSE;

// 네트워크와 관련 된 처리들은 여기서 다함.
// send, recv, get info
CNetwork g_Network;

CNetwork::CNetwork()
{
	m_pINet = NULL;
}

CNetwork::~CNetwork()
{}

void CNetwork::Release()
{
	SAFE_RELEASE(
		m_pINet);
}


void CNetwork::Init(DESC_NETWORK * desc)
{
	if(!g_pServerTable) return;
	
	HRESULT hr;
	hr = CoCreateInstance( CLSID_4DyuchiNET, NULL, CLSCTX_INPROC_SERVER, IID_4DyuchiNET, (void**)&m_pINet);
	if (FAILED(hr))
		return;
	// 080813 LUJ, 수정된 inetwork.dll은 4개의 인자 값을 받는다
	if(!m_pINet->CreateNetwork(desc,0,0,0))
		return;
}

BOOL CNetwork::StartServerServer(char * szIP, int port)
{
	return m_pINet->StartServerWithServerSide(szIP,(WORD)port);
}
BOOL CNetwork::StartUserServer(char * szIP, int port)
{
	return m_pINet->StartServerWithUserSide(szIP, (WORD)port);
}
BOOL CNetwork::ConnectToServer(char * szIP, int port, void * pParam)
{
	return m_pINet->ConnectToServerWithServerSide(szIP,(WORD)port, OnConnectServerSuccess, OnConnectServerFail, pParam);
}

void CNetwork::Send2Server(DWORD dwConnectionIndex, char * msg, DWORD size)
{
	if(!m_pINet) return ;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->SendToServer(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Send2Server %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Send2Server %u %u %u %u", dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		if(!m_pINet) return ;

		m_pINet->SendToServer(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
}

void CNetwork::Send2AgentServer(char * msg, DWORD size)
{
	if(!m_pINet) return ;

	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
			{
				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
				break;
			}
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Send2AgentServer %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Send2AgentServer %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
		{
			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			break;
		}
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Send2SpecificAgentServer(char * msg, DWORD size)
{
	if(!m_pINet) return ;

	MSGBASE* pmsg = (MSGBASE*)msg;
	ASSERT(pmsg->Category < MP_MAX);
	ASSERT(size < 512000);

	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
			{
				if (pInfo->wServerNum == pmsg->dwObjectID)
				{
					m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
					break;
				}
			}
		}
		catch(...)
		{
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Send2SpecificAgentServer %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Send2SpecificAgentServer %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
		{
			if (pInfo->wServerNum == pmsg->dwObjectID)
			{
				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
				break;
			}
		}
	}
}
// E 주민등록 추가 added by hseos 2007.12.31

void CNetwork::Send2User(DWORD dwConnectionIndex, char * msg, DWORD size)
{
	if(!m_pINet) return ;

#ifdef _AGENTSERVER

	MSGBASE* pMsg = (MSGBASE*)msg;

	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	if( g_pServerSystem->IsEnableCrypt() )
	{
		EnCrypt(pInfo,msg,size);
	}

	// desc_hseos_예외처리01
	// S 예외처리 추가 added by hseos 2007.07.21
	// BOOL rt = m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Send2User %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Send2User %u %u %u %u", dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}

	TRAFFIC->AddSendPacketAll( pMsg->Category, pMsg->Protocol, size );

//---KES Distribute Encrypt 071003
#elif defined _DISTRIBUTESERVER_	// distribute serever일때 

	USERINFO* pInfo = g_pUserTable->FindUser(dwConnectionIndex);
	EnCrypt(pInfo,msg,size);

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Send2User %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Send2User %u %u %u %u", dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
#else
	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			MSGBASE* pMsg = (MSGBASE*)msg;
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Send2User %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Send2User %u %u %u %u", dwConnectionIndex, pMsg->Category, pMsg->Protocol, pMsg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		m_pINet->SendToUser(dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
	// E 예외처리 추가 added by hseos 2007.07.21
#endif
}


#ifdef _AGENTSERVER

void CNetwork::Send2User(MSGBASE * msg, DWORD size)
{
	if(!m_pINet) return ;

	USERINFO * userInfo = g_pUserTableForObjectID->FindUser(msg->dwObjectID);

	if( g_pServerSystem->IsEnableCrypt() )
		EnCrypt(userInfo,(char*)msg,size);

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			if(userInfo)
				m_pINet->SendToUser(userInfo->dwConnectionIndex, (char *)msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Send2User %u %u %u %u %u", g_pServerSystem->GetMapNum(), userInfo->dwConnectionIndex, msg->Category, msg->Protocol, msg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Send2User %u %u %u %u", userInfo->dwConnectionIndex, msg->Category, msg->Protocol, msg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		if(userInfo)
			m_pINet->SendToUser(userInfo->dwConnectionIndex, (char *)msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

#endif

#ifndef _MAPSERVER_	//맵서버만 암호화 안한다.

void CNetwork::EnCrypt(USERINFO * userInfo, char* msg,DWORD size)
{
	if( !userInfo ) return;
	int headerSize = sizeof( MSGROOT );
	if( !userInfo->crypto.Encrypt( ( char *)msg + headerSize, size - headerSize ) )
	{
		ASSERTMSG(0,"Encrypt Error");
		return;
	}
	((MSGBASE*)msg)->Code = userInfo->crypto.GetEnCRCConvertChar();
}
#endif

void CNetwork::Broadcast2Server(char * msg, DWORD size)
{
	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->BroadcastServer(msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2Server %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2Server %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		m_pINet->BroadcastServer(msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2User(MSGBASE * msg, DWORD size)
{
	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			m_pINet->BroadcastUser((char*)msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
		catch(...)
		{
			char szTxt[256];

			#if defined(_MAPSERVER_)
				sprintf(szTxt, "M:%02d  Broadcast2User %u %u %u %u", g_pServerSystem->GetMapNum(), msg->Category, msg->Protocol, msg->dwObjectID,  size);
			#elif defined(_AGENTSERVER)
				sprintf(szTxt, "A:%02d  Broadcast2User %u %u %u", msg->Category, msg->Protocol, msg->dwObjectID,  size);
			#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		m_pINet->BroadcastUser((char*)msg, size, FLAG_SEND_NOT_ENCRYPTION);
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2MapServer(char * msg, DWORD size)
{
	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextMapServer()) != NULL)
			{
				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			}
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2MapServer %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2MapServer %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextMapServer()) != NULL)
		{
			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2MapServerExceptOne(DWORD dwConnectionIndex, char * msg, DWORD size)
{
	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextMapServer()) != NULL)
			{
				if(dwConnectionIndex == pInfo->dwConnectionIndex)
					continue;

				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			}	
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2MapServerExceptOne %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2MapServerExceptOne %u %u %u %u",  dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextMapServer()) != NULL)
		{
			if(dwConnectionIndex == pInfo->dwConnectionIndex)
				continue;

			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}	
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2AgentServer(char* msg, DWORD size)
{
	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
			{
				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			}
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2AgentServer %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2AgentServer %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
		{
			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2AgentServerExceptSelf(char* msg, DWORD size)
{
	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
			{
				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			}
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2AgentServerExceptSelf %u %u %u %u", g_pServerSystem->GetMapNum(), pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2AgentServerExceptSelf %u %u %u", pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
		{
			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}

void CNetwork::Broadcast2AgentServerExceptOne(DWORD dwConnectionIndex, char* msg, DWORD size)
{
	g_pServerTable->SetPositionHead();
	SERVERINFO* pInfo;

	if (CSHDebug::GetExceptionProcGrade())
	{
		try
		{
			while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
			{
				if(dwConnectionIndex == pInfo->dwConnectionIndex)
					continue;

				m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
			}
		}
		catch(...)
		{
			char szTxt[256] = {0};

#if defined(_MAPSERVER_)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "M:%02d  Broadcast2AgentServerExceptOne %u %u %u %u %u", g_pServerSystem->GetMapNum(), dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#elif defined(_AGENTSERVER)
			MSGBASE* pmsg = (MSGBASE*)msg;
			sprintf(szTxt, "A:%02d  Broadcast2AgentServerExceptOne %u %u %u %u", dwConnectionIndex, pmsg->Category, pmsg->Protocol, pmsg->dwObjectID,  size);
#endif
			CSHDebug::LogExceptionError(szTxt);
		}
	}
	else
	{
		while((pInfo = g_pServerTable->GetNextAgentServer()) != NULL)
		{
			if(dwConnectionIndex == pInfo->dwConnectionIndex)
				continue;

			m_pINet->SendToServer(pInfo->dwConnectionIndex, msg, size, FLAG_SEND_NOT_ENCRYPTION);
		}
	}
	// E 예외처리 추가 added by hseos 2007.07.21
}
void CNetwork::GetUserAddress(DWORD dwConnectionIndex, char* ip, WORD * port)
{
	m_pINet->GetUserAddress(dwConnectionIndex, ip, port);
}

void CNetwork::DisconnectUser(DWORD dwConnectionIndex)
{
	m_pINet->CompulsiveDisconnectUser(dwConnectionIndex);
}

// 080407 LYW --- Network : Add function to send message to distribute server.
void CNetwork::Send2DistributeServer(char* pMsg, DWORD dwLength)
{
	if(!m_pINet) return ;

	ASSERT(pMsg) ;

	if(!pMsg)
	{
		MessageBox( NULL, "Invalid a message parameter!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		MessageBox( NULL, "Failed to convert a message to base message!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	ASSERT(pmsg->Category < MP_MAX) ;

	if(pmsg->Category >= MP_MAX)
	{
		MessageBox( NULL, "Protocol category is over limit!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	ASSERT(dwLength < 512000) ;

	if(dwLength >= 512000)
	{
		MessageBox( NULL, "A message size is over limit!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	g_pServerTable->SetPositionHead() ;

	SERVERINFO* pInfo = NULL ;
	
	while((pInfo = g_pServerTable->GetNextServer()) != NULL)
	{
		if(pInfo->wServerKind != DISTRIBUTE_SERVER) continue ;

		m_pINet->SendToServer(pInfo->dwConnectionIndex, pMsg, dwLength, FLAG_SEND_NOT_ENCRYPTION) ;
	}	
}

void CNetwork::Send2AgentExceptThis(char* pMsg, DWORD dwLength)
{
	if(!m_pINet) return ;

	ASSERT(pMsg) ;

	if(!pMsg)
	{
		MessageBox( NULL, "Invalid a message parameter!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		MessageBox( NULL, "Failed to convert a message to base message!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	ASSERT(pmsg->Category < MP_MAX) ;

	if(pmsg->Category >= MP_MAX)
	{
		MessageBox( NULL, "Protocol category is over limit!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	ASSERT(dwLength < 512000) ;

	if(dwLength >= 512000)
	{
		MessageBox( NULL, "A message size is over limit!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	SERVERINFO * pThisServer = NULL ;
	pThisServer = g_pServerTable->GetSelfServer() ;

	ASSERT(pThisServer) ;

	if(!pThisServer)
	{
		MessageBox( NULL, "Failed to receive self server info!!", __FUNCTION__, MB_OK) ;
		return ;
	}

	g_pServerTable->SetPositionHead() ;

	SERVERINFO* pInfo = NULL ;
	
	while((pInfo = g_pServerTable->GetNextServer()) != NULL)
	{
		if(pInfo->wServerKind != AGENT_SERVER) continue ;

		if(pThisServer->wServerNum == pInfo->wServerNum) continue ;

		if(pThisServer->dwConnectionIndex == pInfo->dwConnectionIndex) continue ;

		m_pINet->SendToServer(pInfo->dwConnectionIndex, pMsg, dwLength, FLAG_SEND_NOT_ENCRYPTION) ;
	}	
}