// QuestMapMgr.cpp: implementation of the CQuestMapMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestMapMgr.h"
#include "Player.h"
#include "RecallManager.h"
#include "ChannelSystem.h"
#include "QuestManager.h"
// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.25	2007.11.28
#include "../hseos/Date/SHDateManager.h"
#include "UserTable.h"
// E 데이트 존 추가 added by hseos 2007.11.25	2007.11.28

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CQuestMapMgr);

CQuestMapMgr::CQuestMapMgr()
{
	m_bQuestMap = FALSE;
}

CQuestMapMgr::~CQuestMapMgr()
{
}

BOOL CQuestMapMgr::Init( WORD wMapNum )
{
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.25
	// ..챌린지 존의 채널처리를 퀘스트 맵의 채널처리로 사용
	// if( QUESTMAPNUM1 == wMapNum || QUESTMAPNUM2 == wMapNum )
	if( g_csDateManager.IsChallengeZone(wMapNum))
	// E 데이트 존 추가 added by hseos 2007.11.25
	{
		m_bQuestMap = TRUE;
		CHANNELSYSTEM->InitQuestMapChannel();
	}
	else
		m_bQuestMap = FALSE;

	return m_bQuestMap;
}

void CQuestMapMgr::RemovePlayer( CPlayer* pPlayer )
{
	if( !m_bQuestMap )	return;

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.28
	// ..챌린지 존일 경우에는 2명의 플레이어 중 나중에 나가는 플레이어일 경우에 채널을 지운다.
	// RECALLMGR->DeleteQuestRecallMonster( pPlayer->GetChannelID() );
	// CHANNELSYSTEM->DestroyQuestMapChannel( pPlayer->GetChannelID() );
	if (g_csDateManager.IsChallengeZoneHere())
	{
		// 기본적으로 몬스터는 삭제. 즉 챌린지 진행중에 1명이라도 맵과 접속이 끊기면 몬스터는 사라진다.
		RECALLMGR->DeleteChallengeZoneRecallMonster( pPlayer->GetChannelID() );

		// 파트너가 들어왔다.
		if (pPlayer->GetChallengeZonePartnerID())
		{
			// 현재 접속 중인지 다시 확인
			CPlayer* pTargetPlayer = (CPlayer*)g_pUserTable->FindUser(pPlayer->GetChallengeZonePartnerID());
			if(pTargetPlayer)
			{
				// 파트너가 접속 중이기에 채널을 지우지는 않고 파트너ID 만 초기화한다.
				// 파트너가 접속을 끊을 떄 이 함수가 수행되면서 채널 삭제 처리를 할 것이다..
				pTargetPlayer->SetChallengeZonePartnerID(0);
			}
			else
			{
				// 파트너가 들어왔음이 분명한데 정보는 없다?? 정상적이라면 여기는 수행이 안 된다..
				// 어쨌든 파트너가 없는 상태고 자신도 나가는 상황이니 채널 삭제
				CHANNELSYSTEM->DestroyQuestMapChannel( pPlayer->GetChannelID() );
			}
		}
		// 파트너가 안 들어왔거나, 나갔다.
		else
		{
			// 파트너가 나갔다면 채널을 지운다.
			// 파트너가 안 들어왔을 경우는, 파트너가 영영 안 들어올 수도 있기 때문에 채널을 지운다.
			// 만약 랙 떄문에 늦게 들어올 경우에는 채널을 새로 할당받도록 처리되어 있다.
			// 물론 늦게 들어온 파트너는 챌린지가 시작되지 않는다. 그냥 맵 구경만..ㅋ
			CHANNELSYSTEM->DestroyQuestMapChannel( pPlayer->GetChannelID() );
		}
	}
	else
	{
		RECALLMGR->DeleteQuestRecallMonster( pPlayer->GetChannelID() );
		CHANNELSYSTEM->DestroyQuestMapChannel( pPlayer->GetChannelID() );
	}
	// E 데이트 존 추가 added by hseos 2007.11.28
}

void CQuestMapMgr::DiePlayer( CPlayer* pPlayer )
{
	if( !m_bQuestMap )	return;

	pPlayer->SetReadyToRevive( FALSE );
//	QUESTMGR->AddQuestEvent( pPlayer, &CQuestEvent( eQuestEvent_Die, g_pServerSystem->GetMapNum(), 0 ) );
}