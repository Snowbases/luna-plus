// UserTable.cpp: implementation of the CUserTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserTable.h"

#include "Network.h"
#include "TrafficLog.h"
#include "GMPowerList.h"
#include "..\[CC]Header\Pool.h"
#include "..\hseos\Family\SHFamilyManager.h"
#include "..\hseos\ResidentRegist\SHResidentRegistManager.h"
#include "PunishManager.h"
//-------------

// 080418 LYW --- UserTable : Include chatroom manager.
#include "./ChatRoomMgr.h"

#ifdef _NPROTECT_
#include "ggsrv25.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUserTable * g_pUserTable = NULL;
CUserTable * g_pUserTableForUserID = NULL;
#include "AgentDBMsgParser.h"
CUserTable * g_pUserTableForObjectID = NULL;

extern int g_nServerSetNum;

CUserTable::CUserTable()
{
}

CUserTable::~CUserTable()
{
}

void CUserTable::Init(DWORD dwBucket,USERDisconnectProc pProc)
{
	CYHHashTable<USERINFO>::Initialize(dwBucket);
	m_MaxUserCount = 0;
	m_addCount = 0;
	m_removeCount = 0;

	m_pOnDisconnectProc	=	pProc;
}

BOOL CUserTable::AddUser(USERINFO* pObject,DWORD dwKey)
{
	++m_addCount;

	return CYHHashTable<USERINFO>::Add(pObject,dwKey);	
}

USERINFO * CUserTable::RemoveUser(DWORD dwKey)
{
	USERINFO * info = GetData(dwKey);

	if( !info )
		return NULL;

	CSHFamily* const family = g_csFamilyManager.GetFamily(
		info->mFamilyIndex);

	g_csFamilyManager.ASRV_SendMemberConToOtherAgent(
		family,
		info->dwCharacterID,
		CSHFamilyMember::MEMBER_CONSTATE_LOGOFF);
	g_csFamilyManager.DelFamilyToTbl(
		info->dwCharacterID,
		family);
	g_csFamilyManager.ASRV_SendInfoToClient(
		family,
		2);
	info->mFamilyIndex = 0;

	g_csResidentRegistManager.ASRV_EndDateMatchingChat(
		info);
	CHATROOMMGR->DestroyPlayer_From_Lobby(
		info);

	if( m_pOnDisconnectProc)
			m_pOnDisconnectProc( info);				// 지워질때 콜백을 호출.

	Remove(dwKey);

	++m_removeCount;

	return info;
}

void CUserTable::SetCalcMaxCount(DWORD CurCount)
{
	if(m_MaxUserCount < CurCount)
	{
		m_MaxUserCount = CurCount;
	}
}

BOOL CUserTable::SendToUser(DWORD dwKey,DWORD dwUniqueConnectIdx,MSGBASE* pmsg,DWORD size)
{
	USERINFO* pUserInfo = FindUser(dwKey);
	if(pUserInfo == NULL)
		return FALSE;
	if(pUserInfo->dwUniqueConnectIdx != dwUniqueConnectIdx)
		return FALSE;

	g_Network.Send2User(pUserInfo->dwConnectionIndex,(char*)pmsg,size);
	return TRUE;
}

// 081107 LUJ, 풀 클래스 교체
extern CPool< USERINFO > g_UserInfoPool;

#ifdef _NPROTECT_
extern CPool<CCSAuth2> g_CCSAuth2Pool;
#endif

BOOL CUserTable::OnDisconnectUser(DWORD dwKey)
{
	USERINFO* pInfo = g_pUserTable->FindUser( dwKey );
	if(pInfo == NULL) return FALSE;

	//---KES Network Process Fix 071114
	g_pUserTable->RemoveUser( dwKey );
	//----------------------------

	if( pInfo->UserLevel == eUSERLEVEL_GM )
	{
		 GMINFO->RemoveGMList( dwKey );
	}


//update Billing
	if(pInfo->dwCharacterID)
	{
		g_pUserTableForObjectID->RemoveUser(pInfo->dwCharacterID);

		SERVERINFO* pServerInfo = g_pServerTable->FindServerForConnectionIndex(pInfo->dwMapServerConnectionIndex);
		if( pServerInfo )
		{
			// 로그인 Character_ID정보 초기화
			MSG_DWORD msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_DISCONNECTED;
			msg.dwObjectID	= pInfo->dwCharacterID;
			msg.dwData		= pInfo->dwUserID;
			g_Network.Send2Server(pInfo->dwMapServerConnectionIndex, (char*)&msg, sizeof(msg));
		}
		else
		{
			if(pInfo->dwUserID)
				LoginCheckDelete(pInfo->dwUserID);		// LOGOUT시간을 업데이트 합니다.
		}

		// ChangeMapPoint 초기화
		SaveMapChangePointUpdate(pInfo->dwCharacterID, 0);
	}
	else
	{
		if(pInfo->dwUserID)
			LoginCheckDelete(pInfo->dwUserID);		// LOGOUT시간을 업데이트 합니다.
	}

	if(pInfo->dwUserID)
	{
		g_pUserTableForUserID->RemoveUser(pInfo->dwUserID);
#ifdef _NPROTECT_
		if( pInfo->m_pCSA )
		{
			pInfo->m_pCSA->Close();
			g_CCSAuth2Pool.Free(pInfo->m_pCSA);
		}
#endif
		//---KES PUNISH
		PUNISHMGR->RemovePunishUnitAll( pInfo->dwUserID );
		//---------------
	}

	const DWORD userIndex = pInfo->dwUserID;

	memset( pInfo, 0, sizeof(USERINFO) );
	g_UserInfoPool.Free(pInfo);	
	
	SERVERINFO* myInfo = g_pServerTable->GetSelfServer();
	myInfo->wAgentUserCnt = WORD(GetDataNum());

	MSG_WORD2 msg2;
	msg2.Category = MP_SERVER;
	msg2.Protocol = MP_SERVER_USERCNT;
	msg2.wData1 = myInfo->wPortForServer;
	msg2.wData2 = myInfo->wAgentUserCnt;

	
	g_pServerTable->SetPositionHead();
	SERVERINFO* pOtherAgentInfo = NULL;
	while( (pOtherAgentInfo = g_pServerTable->GetNextDistServer()) != NULL)
	{
		if(myInfo == pOtherAgentInfo)
			continue;

		g_Network.Send2Server(pOtherAgentInfo->dwConnectionIndex, (char *)&msg2, sizeof(msg2));
	}

	{
		MSGBASE message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_USERCONN;
		message.Protocol = MP_USERCONN_BILLING_STOP_SYN;
		message.dwObjectID = userIndex;
		g_Network.Send2DistributeServer(
			(char*)&message,
			sizeof(message));
	}

	return TRUE;
}

BOOL DisconnectUser(DWORD dwConnectionIndex)
{
	g_Network.DisconnectUser(dwConnectionIndex);

	return TRUE;
}