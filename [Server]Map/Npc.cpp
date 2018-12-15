// Npc.cpp: implementation of the CNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Npc.h"
#include "ObjectStateManager.h"
#include "AISystem.h"
#include "MHError.h"
#include "UserTable.h"
#include "Player.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "CharMove.h"
#include "MapDBMsgParser.h"
#include "PackedData.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNpc::CNpc()
{

}

CNpc::~CNpc()
{

}


void CNpc::InitNpc(NPC_TOTALINFO* pTotalInfo)
{
	memcpy(&m_NpcInfo,pTotalInfo,sizeof(NPC_TOTALINFO));
	m_FirstTalkCharacterID = 0;
	m_DieTime = gCurTime + 60000*5;
	m_Angle = 0.0f;
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CNpc::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
{
	static SEND_NPC_TOTALINFO message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_NPC_ADD;
	message.dwObjectID = dwReceiverID;
	message.Angle = m_Angle;
	GetSendMoveInfo( &message.MoveInfo,&message.AddableInfo );
	GetBaseObjectInfo( &message.BaseObjectInfo );
	message.TotalInfo = m_NpcInfo;
	message.bLogin = BYTE(isLogin);

	sendMessage = &message;
	return message.GetMsgLength();
}

void CNpc::DoDie(CObject* pAttacker)
{
	//send msg
	

	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,m_FirstTalkCharacterID);
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die,MONSTERREMOVE_TIME);
}

void CNpc::SetFirstTalkCharacterID(DWORD CharacterIdx)
{
	m_FirstTalkCharacterID = CharacterIdx;
}

DWORD CNpc::GetFirstTalkCharacterID()
{
	return m_FirstTalkCharacterID;
}

void CNpc::SetDieTime(DWORD time)
{
	m_DieTime = time;
}

void CNpc::Process()
{
	if(m_DieTime)
	{
		if(m_DieTime < gCurTime)
		{			
			MSG_DWORD2 msg;
			msg.Category	= MP_USERCONN;
			msg.Protocol	= MP_USERCONN_NPC_DIE;
			msg.dwObjectID	= 0;
			msg.dwData1		= 0;
			msg.dwData2		= GetID();
			
			PACKEDDATA_OBJ->QuickSend((CObject*)this,&msg,sizeof(msg));

			Die(NULL);
			m_DieTime = 0;
		}
	}
}

void CNpc::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Die:
		{
			MSG_DWORD msg;
			msg.Category = MP_NPC;
			msg.Protocol = MP_NPC_DIE_ACK;
			msg.dwData = GetID();
			PACKEDDATA_OBJ->QuickSendExceptObjectSelf(this, &msg, sizeof(msg));
			
			g_pServerSystem->RemoveNpc(GetID());
		}
		break;
	}
}

void CNpc::Release()
{
	m_DieTime = 0;
	m_FirstTalkCharacterID = 0;
	g_pAISystem.ReleaseMonsterID(GetID());
	CObject::Release();
}

WORD CNpc::GetNpcJob()
{
	return m_NpcInfo.NpcJob;
}

void CNpc::DoJob(CPlayer* pPlayer, DWORD ConnectIdx)
{
	WORD job = GetNpcJob();
	switch(job)
	{
	case MAPCHANGE_ROLE:
		MapChangeRole(pPlayer, ConnectIdx);
		break;
	case BOMUL_ROLE:
		BomulRole(pPlayer); 
		break;
	default:
		ASSERT(0); break;
	}
}

void CNpc::MapChangeRole(CPlayer* pPlayer, DWORD dwConnectionIndex)
{
	if(GetState() == eObjectState_Die)
	{
		SendNackMsg(pPlayer, 2); 
		return;
	}
	MAPCHANGE_INFO*	pChangeInfo = GAMERESRCMNGR->GetMapChangeInfoForDestMapNum(12); //12 : Jang Ahn
	if( !pChangeInfo ) 
	{
		SendNackMsg(pPlayer, 3); return;
	}

	VECTOR3 ObjectPos	= *CCharMove::GetPosition(pPlayer);
	VECTOR3 TObjectPos	= *CCharMove::GetPosition(this);
	DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );
	if(Distance > 3000.0f)
	{
		SendNackMsg(pPlayer, 1); return;
	}

	pPlayer->UpdateLogoutToDB();
	
	// 081218 LUJ, 해제하기 전에 값을 복사하자
	const DWORD		playerIndex		= pPlayer->GetID();
	const LEVELTYPE	level			= pPlayer->GetLevel();
	const EXPTYPE	experience		= pPlayer->GetPlayerExpPoint();
	const MONEYTYPE	inventoryMoney	= pPlayer->GetMoney( eItemTable_Inventory );
	const MONEYTYPE	storageMoney	= pPlayer->GetMoney( eItemTable_Storage );

	g_pServerSystem->RemovePlayer(pPlayer->GetID());
	
	// 리턴 받으면 맵 아웃 메시지 보냄.
	// 081218 LUJ, 플레이어 소지금 처리 추가
	SaveMapChangePointReturn(
		playerIndex,
		pChangeInfo->Kind,
		dwConnectionIndex,
		level,
		experience,
		inventoryMoney,
		storageMoney );
}

void CNpc::SendNackMsg(CPlayer* pPlayer, BYTE err)
{
	MSG_DWORDBYTE msg;
	msg.Category = MP_NPC;
	msg.Protocol = MP_NPC_DOJOB_NACK;
	msg.bData = err;
	msg.dwData = GetUniqueIdx();
	pPlayer->SendMsg(&msg, sizeof(msg));
}

WORD CNpc::GetUniqueIdx()
{
	return m_NpcInfo.NpcUniqueIdx;
}

void CNpc::BomulRole(CPlayer* pPlayer)
{
	if(GetFirstTalkCharacterID() != pPlayer->GetID())
	{
		ASSERT(0);
		return;
	}
	DoDie(pPlayer);
}

void CNpc::AddChat(char * str)
{
	TESTMSG pmsg;
	pmsg.Category = MP_CHAT;
	pmsg.Protocol = MP_CHAT_FROMMONSTER_ALL;
	pmsg.dwObjectID = GetID();
	//strcpy(pmsg.Msg, str);
	SafeStrCpy( pmsg.Msg, str, MAX_CHAT_LENGTH+1 );
	//g_Console.LOG(4, pmsg->Msg);

	//CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
	//if(!pPlayer) return;

	//PACKEDDATA_OBJ->QuickSend(this,&pmsg,sizeof(pmsg));
	PACKEDDATA_OBJ->QuickSend(this,&pmsg,pmsg.GetMsgLength());	//CHATMSG 040324
}
