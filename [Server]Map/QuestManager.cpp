// QuestManager.cpp: implementation of the CQuestManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestManager.h"

#include "MHFile.h"
#include "UserTable.h"
#include "..\[CC]Quest\QuestScriptLoader.h"
#include "..\[CC]Quest\QuestString.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "QuestGroup.h"
#include "QuestUpdater.h"
#include "Player.h"
#include "Quest.h"
#include "MapDBMsgParser.h"
#include "SiegeWarfareMgr.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CQuestManager);

CQuestManager::CQuestManager()
{
	m_QuestInfoTable.Initialize(MAX_QUEST);
	memset( m_QuestEvent, 0, sizeof(MQUESTEVENT)*MAX_QUESTEVENT_MGR );
	m_dwQuestEventCount = 0;

	m_QuestUpdater = new CQuestUpdater;
}

CQuestManager::~CQuestManager()
{
	Release();
}

void CQuestManager::Release()
{
	CQuestInfo* pQuestInfo = NULL;
	m_QuestInfoTable.SetPositionHead();
	while( (pQuestInfo = m_QuestInfoTable.GetData()) != NULL)
	{
		delete pQuestInfo;
	}
	m_QuestInfoTable.RemoveAll();

	if( m_QuestUpdater )
	{
		delete m_QuestUpdater;
		m_QuestUpdater =NULL;
	}
}

BOOL CQuestManager::LoadQuestScript()
{
	CMHFile file;
	if( !file.Init( "./system/Resource/QuestScript.bin", "rb") )
		return FALSE;

	char Token[256];
	DWORD dwQuestNum = 0;
	while( !file.IsEOF() )
	{
		file.GetString( Token );
		if( strcmp(Token,"$QUEST") == 0 )
		{
			dwQuestNum = file.GetDword();			
			m_QuestInfoTable.Add( CQuestScriptLoader::LoadQuestInfo( &file, dwQuestNum ), dwQuestNum );
		}
	}
	file.Release();
	
	return TRUE;
}

void CQuestManager::CreateQuestForPlayer( PLAYERTYPE* pPlayer )
{
	m_QuestInfoTable.SetPositionHead();

	while(CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData())
	{
		pPlayer->GetQuestGroup().CreateQuest(
			pQuestInfo->GetQuestIdx(),
			pQuestInfo);
	}
}

void CQuestManager::SetMainQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime )
{
	pPlayer->GetQuestGroup().SetMainQuestData(
		dwQuestIdx,
		dwSubQuestFlag,
		dwData,
		registTime,
		bCheckType,
		dwCheckTime);
}

void CQuestManager::SetSubQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime )
{
	pPlayer->GetQuestGroup().SetSubQuestData(
		dwQuestIdx,
		dwSubQuestIdx,
		dwData,
		dwTime);
}

void CQuestManager::SetQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, int nItemNum )
{
	// 일단 item갯수가 0보다 작으면...
	if( nItemNum < 0 )
		return;

	pPlayer->GetQuestGroup().SetQuestItem(
		dwQuestIdx,
		dwItemIdx,
		nItemNum);
}

void CQuestManager::DeleteQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, BOOL bTime, BOOL bSendMsg )
{
	if(pPlayer->GetQuestGroup().IsQuestComplete( dwQuestIdx ) )
	{
		return;
	}

	pPlayer->GetQuestGroup().DeleteQuest(
		dwQuestIdx);
	m_QuestUpdater->DeleteQuest(
		pPlayer,
		pPlayer->GetQuestGroup().GetQuest(dwQuestIdx),
		1);

	if( bSendMsg )
	{
		SEND_QUEST_IDX msg;
		msg.Category = MP_QUEST;
		msg.Protocol = MP_QUEST_DELETE_ACK;
		msg.dwObjectID = pPlayer->GetID();
		msg.MainQuestIdx = (WORD)dwQuestIdx;
		msg.dwFlag = (DWORD)bTime;
		
		pPlayer->SendMsg(&msg, sizeof(msg));
	}
}

// 091008 LUJ, DeleteQuest()와의 차이는 완료 여부를 검사하지 않는다는 것뿐이다
void CQuestManager::DeleteQuestForcedly(CPlayer& player, DWORD questIndex, BOOL bSendMsg)
{
	player.GetQuestGroup().DeleteQuest(
		questIndex);
	m_QuestUpdater->DeleteQuest(
		&player,
		player.GetQuestGroup().GetQuest(questIndex),
		1);

	if( bSendMsg )
	{
		SEND_QUEST_IDX message;
		ZeroMemory(&message, sizeof(message));
		message.Category = MP_QUEST;
		message.Protocol = MP_QUEST_DELETE_ACK;
		message.dwObjectID = player.GetID();
		message.MainQuestIdx = WORD(questIndex);
		player.SendMsg(
			&message,
			sizeof(message));
	}
}

void CQuestManager::StartQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = pPlayer->GetQuestGroup().GetQuest( dwQuestIdx );

	if( !pQuest )	return;

	// DB 
	m_QuestUpdater->StartQuest( pPlayer, pQuest );

	// Network Send
	SEND_QUEST_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_START_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.MainQuestIdx = (WORD)pQuest->GetQuestIdx();
	Msg.SubQuestIdx = 0;
	Msg.dwFlag = pQuest->GetSubQuestFlag();
	Msg.dwData = pQuest->IsQuestComplete();
	Msg.registTime = pQuest->GetQuestRegistTime();
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestManager::EndQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = pPlayer->GetQuestGroup().GetQuest( dwQuestIdx );

	if( !pQuest )	return;


	// DB
	if( dwSubQuestIdx )	// 반복
		m_QuestUpdater->DeleteQuest( pPlayer, pQuest, 0 );
	else				// 1번
		m_QuestUpdater->EndQuest( pPlayer, pQuest );

	SEND_QUEST_IDX Msg;

	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_END_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.MainQuestIdx = (WORD)pQuest->GetQuestIdx();
	Msg.SubQuestIdx = (WORD)dwSubQuestIdx;
	Msg.dwFlag = pQuest->GetSubQuestFlag();
	Msg.dwData = 1;
	Msg.registTime = pQuest->GetQuestRegistTime();
	pPlayer->SendMsg(&Msg, sizeof(Msg));

	
}

void CQuestManager::StartSubQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = pPlayer->GetQuestGroup().GetQuest( dwQuestIdx );

	if( !pQuest )	return;

	// DB
	m_QuestUpdater->StartSubQuest( pPlayer, pQuest, dwSubQuestIdx );
//	JouranlInsertQuest(pPlayer->GetID(), eJournal_Quest, dwQuestIdx, dwSubQuestIdx, FALSE);

	// network send
	SEND_QUEST_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_SUBQUEST_START_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.MainQuestIdx = (WORD)pQuest->GetQuestIdx();
	Msg.SubQuestIdx = (WORD)dwSubQuestIdx;
	Msg.dwFlag = pQuest->GetSubQuestFlag();
	Msg.dwData = pQuest->GetSubQuestData( dwSubQuestIdx );
	Msg.registTime = pQuest->GetSubQuestTime( dwSubQuestIdx );
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestManager::EndSubQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	CQuest* pQuest = pPlayer->GetQuestGroup().GetQuest( dwQuestIdx );

	if( !pQuest )	return;

	// DB
	m_QuestUpdater->EndSubQuest( pPlayer, pQuest, dwSubQuestIdx );
//	JouranlInsertQuest(pPlayer->GetID(), eJournal_Quest, dwQuestIdx, dwSubQuestIdx, TRUE);

	// network send
	SEND_QUEST_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_SUBQUEST_END_ACK;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.MainQuestIdx = (WORD)pQuest->GetQuestIdx();
	Msg.SubQuestIdx = (WORD)dwSubQuestIdx;
	Msg.dwFlag = pQuest->GetSubQuestFlag();
	Msg.dwData = pQuest->GetSubQuestData( dwSubQuestIdx );
	Msg.registTime = pQuest->GetSubQuestTime( dwSubQuestIdx );
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestManager::UpdateSubQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount )
{
	CQuest* pQuest = pPlayer->GetQuestGroup().GetQuest( dwQuestIdx );

	if( pQuest == NULL )	return;

	// DB
	m_QuestUpdater->UpdateSubQuest( pPlayer, pQuest, dwSubQuestIdx );

	// network send
	SEND_SUBQUEST_UPDATE Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_SUBQUEST_UPDATE;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.wQuestIdx = (WORD)pQuest->GetQuestIdx();
	Msg.wSubQuestIdx = (WORD)dwSubQuestIdx;
	Msg.dwMaxCount = dwMaxCount;
	Msg.dwData = pQuest->GetSubQuestData( dwSubQuestIdx );
	Msg.dwTime = pQuest->GetSubQuestTime( dwSubQuestIdx );
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

BOOL CQuestManager::GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	if( !pPlayer ) return FALSE ;

	BOOL b_Result = FALSE ;

	// DB
	m_QuestUpdater->GiveQuestItem( pPlayer, dwQuestIdx, dwItemIdx, dwItemNum );

	// network send
	SEND_QUESTITEM_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUESTITEM_GIVE;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dwItemIdx = dwItemIdx;
	Msg.dwItemNum = dwItemNum;
	pPlayer->SendMsg(&Msg, sizeof(Msg));

	b_Result = TRUE ;

	return b_Result ;
}

void CQuestManager::TakeQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	// DB
	m_QuestUpdater->TakeQuestItem( pPlayer, dwQuestIdx, dwItemIdx, dwItemNum );

	// network send
	SEND_QUESTITEM_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUESTITEM_TAKE;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dwItemIdx = dwItemIdx;
	Msg.dwItemNum = dwItemNum;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestManager::UpdateQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum )
{
	// DB
	m_QuestUpdater->UpdateQuestItem( pPlayer, dwQuestIdx, dwItemIdx, dwItemNum );

	// network send
	SEND_QUESTITEM_IDX Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUESTITEM_UPDATE;
	Msg.dwObjectID = pPlayer->GetID();
	Msg.dwItemIdx = dwItemIdx;
	Msg.dwItemNum = dwItemNum;
	pPlayer->SendMsg(&Msg, sizeof(Msg));
}

void CQuestManager::UpdateQuestCheckTime( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwCheckType, DWORD dwCheckTime )
{
	// DB
	m_QuestUpdater->UpdateQuestCheckTime( pPlayer, dwQuestIdx, dwCheckType, dwCheckTime );
}

void CQuestManager::AddQuestEvent( PLAYERTYPE* pPlayer, CQuestEvent* pQuestEvent, DWORD dwQuestIdx )
{
	if( m_dwQuestEventCount < MAX_QUESTEVENT_MGR )
	{
		m_QuestEvent[m_dwQuestEventCount].pPlayer = pPlayer;
		m_QuestEvent[m_dwQuestEventCount].QuestEvent = *pQuestEvent;
		m_QuestEvent[m_dwQuestEventCount].dwQuestIdx = dwQuestIdx;
		++m_dwQuestEventCount;
	}
}

void CQuestManager::Process()
{
	for(DWORD i = 0; i < m_dwQuestEventCount; ++i)
	{
		MQUESTEVENT& event = m_QuestEvent[i];

		if(event.pPlayer)
		{
			event.pPlayer->GetQuestGroup().AddQuestEvent(
				&event.QuestEvent,
				event.dwQuestIdx);
		}
	}

	memset( m_QuestEvent, 0, sizeof(MQUESTEVENT)*MAX_QUESTEVENT_MGR );
	m_dwQuestEventCount = 0;
}

BOOL CQuestManager::CheckFullQuestCount(CPlayer* pPlayer)
{
	if( !pPlayer ) return FALSE ;

	int nQuestCount = pPlayer->GetQuestGroup().GetQuestCount();

	if( nQuestCount >= 20 ) return TRUE ;
	else return FALSE ;
}

void CQuestManager::SendFullQuestMsg(CPlayer* pPlayer)
{
	if( !pPlayer ) return ;

	MSGBASE Msg ;																			// 에러 처리를 위한 메시지 구조체를 선언한다.

	Msg.Category = MP_QUEST;																// 카테고리를 퀘스트로 세팅한다.
	Msg.Protocol = MP_QUEST_ERROR_FULLQUEST;												// 프로토콜을 아이템 반납 실패로 세팅한다.

	pPlayer->SendMsg( &Msg, sizeof(Msg) );													// 플레이어에게 메시지를 전송한다.
}

void CQuestManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_QUESTEVENT_NPCTALK:
		{			
			SEND_QUEST_REQUITAL_IDX* pmsg = (SEND_QUEST_REQUITAL_IDX*)pMsg;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )	return;

			//길드관리인 퀘스트는 성주인만 가능하도록 설정
			if( pmsg->MainQuestIdx == 135 || pmsg->MainQuestIdx == 136)
			{
				//현재 공성중이라면 실패
				if( SIEGEWARFAREMGR->GetState( ) != eSiegeWarfare_State_Before )
					return;

				// 현재 성의 마스터 길드가 아니면 실패.
				DWORD dwCastleGuild = SIEGEWARFAREMGR->GetCastleGuildIdx(SIEGEWARFAREMGR->GetMapInfo()) ;
				if( dwCastleGuild != pPlayer->GetGuildIdx() || dwCastleGuild == 0 )			
					return;
			}

			CQuestEvent qe;
			qe.SetValue( eQuestEvent_NpcTalk, pmsg->MainQuestIdx, pmsg->SubQuestIdx );
			AddQuestEvent( pPlayer, &qe );

			// 100414 ONS 선택보상이 있을경우 해당 퀘스트인덱스와 보상 정보를 저장한다.
			if( pmsg->dwRequitalIdx > 0 && pmsg->dwRequitalCount > 0 )
			{
				pPlayer->GetQuestGroup().SetQuestRequital(
					pmsg->SubQuestIdx,
					pmsg->dwRequitalIdx,
					pmsg->dwRequitalCount);
			}
		}
		break;
	case MP_QUESTEVENT_NPCTALK_SYN:
		{
			SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )	return;

			stTIME time;
			time.value = GetCurTime();
			char temp[256] = {0,};
			sprintf( temp, "%d-%d-%d %d:%d", time.GetYear()+2000, time.GetMonth(), time.GetDay(), 0, 0 );//time.GetHour(), time.GetMinute() );
			QuestEventCheck( pPlayer->GetUserID(), pmsg->dwObjectID, pmsg->MainQuestIdx, pmsg->SubQuestIdx, temp );			
		}
		break;
	case MP_QUEST_CHANGESTATE:
		{
			SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )	return;

			m_QuestUpdater->UpdateQuest( pPlayer, pmsg->dwFlag, pmsg->dwData, pmsg->registTime );
		}
		break;

	// 070416 LYW --- QuestManager : Add change stage part.
	case MP_QUEST_CHANGESTAGE_SYN :
		{
			MSG_BYTE2* pmsg = (MSG_BYTE2*)pMsg ;

			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
			if( !pPlayer )	return;

			pPlayer->GetQuestGroup().ChangeStage(
				pmsg->bData1,
				pmsg->bData2);
		}
		break ;

	case MP_QUEST_DELETE_SYN:
		{
			SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;
			CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwObjectID);
			if( pPlayer == NULL )	return;

			DeleteQuest( pPlayer, pmsg->MainQuestIdx );			
		}
		break;
	}	
}





//-------------------------------------------------------------------------------------------------
//	NAME : WriteLog
//	DESC : 퀘스트 관련 로그를 남기는 함수 추가.
//  DATE : MAY 15, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CQuestManager::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	char tempFile[128] = {0,} ;
	sprintf(tempFile, "Log/Map(%d)-QuestErr.log", g_pServerSystem->GetMapNum()) ;

	FILE *fp = fopen(tempFile, "a+");
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime);
		fclose(fp);
	}
}
