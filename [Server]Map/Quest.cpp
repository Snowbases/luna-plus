// Quest.cpp: implementation of the CQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Quest.h"

#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"
#include "..\[CC]Quest\QuestEvent.h"
#include "Quest.h"
#include "QuestManager.h"
#include "Player.h"
#include "MapDBMsgParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CQuest::CQuest( CPlayer* pPlayer, CQuestInfo* pQuestInfo )
{
	m_pPlayer = pPlayer;
	m_pQuestInfo = pQuestInfo;

	memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );
	m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );

	m_dwSubQuestCount = m_pQuestInfo->GetSubQuestCount();
	memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );
	m_dwSubQuestFlag = 0;
	m_dwData = 0;
	m_RegistTime = 0;
	
	m_bCheckTime = FALSE;
	m_CheckType = 0;
	m_CheckTime.value = 0;


	// 유효시간 설정
	InitDateTime();
}

CQuest::~CQuest()
{
}

void CQuest::SetMainQuestData( DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime )
{
	m_CurSubQuestInfoArray[0] = NULL;
	m_dwSubQuestFlag = dwSubQuestFlag;
	m_dwData = dwData;
	m_RegistTime = registTime;

	m_CheckType = bCheckType;
	m_CheckTime.value = dwCheckTime;
	if( m_CheckType != 0 )
		m_bCheckTime = TRUE;

	// 유효시간 설정
	InitDateTime();
}

void CQuest::SetSubQuestData( DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime )
{
	m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );
	m_SubQuestTable[dwSubQuestIdx].dwData = dwData;
	m_SubQuestTable[dwSubQuestIdx].dwTime = dwTime;
}

void CQuest::SetSubQuestFlag( DWORD dwSubQuestIdx )
{
	DWORD value = 1;
	m_dwSubQuestFlag = m_dwSubQuestFlag | ( value << (31-dwSubQuestIdx) );
}

BOOL CQuest::EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx)
{	
	if( dwSubQuestIdx )	// 반복
	{
		memset( m_CurSubQuestInfoArray, 0, sizeof(m_CurSubQuestInfoArray));
		m_CurSubQuestInfoArray[0] = m_pQuestInfo->GetSubQuestInfo( 0 );

		memset( m_SubQuestTable, 0, sizeof(SUBQUEST)*MAX_SUBQUEST );
		m_dwSubQuestFlag = 0;
		m_dwData = 0;
		_time64( &m_RegistTime );

		m_bCheckTime = FALSE;
		m_CheckType = 0;
		m_CheckTime.value = 0;
	}
	else				// 1번
	{
		memset( m_CurSubQuestInfoArray, 0, sizeof(CSubQuestInfo*)*MAX_SUBQUEST );
		m_dwData = 1;
		_time64( &m_RegistTime );

		m_bCheckTime = FALSE;
		m_CheckType = 0;
		m_CheckTime.value = 0;
	}
	return TRUE;
}

BOOL CQuest::StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	if( m_CurSubQuestInfoArray[dwSubQuestIdx] == NULL )
	{
		m_CurSubQuestInfoArray[dwSubQuestIdx] = m_pQuestInfo->GetSubQuestInfo( dwSubQuestIdx );
		m_SubQuestTable[dwSubQuestIdx].dwData = 0;
		m_SubQuestTable[dwSubQuestIdx].dwTime = GetCurTime();
		return TRUE;
	}
	return FALSE;
}

BOOL CQuest::EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx )
{
	if( m_CurSubQuestInfoArray[dwSubQuestIdx] )
	{
		m_CurSubQuestInfoArray[dwSubQuestIdx] = NULL;
		m_SubQuestTable[dwSubQuestIdx].dwData = 0;		
		m_SubQuestTable[dwSubQuestIdx].dwTime = GetCurTime();
		SetSubQuestFlag( dwSubQuestIdx );	
		if( dwSubQuestIdx == 0 )	// Quest Start
		{
			m_dwData = 0;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CQuest::ChangeSubQuestValue( DWORD dwSubQuestIdx, DWORD dwKind )
{
	SUBQUEST* pSubQuest = &m_SubQuestTable[dwSubQuestIdx];
	switch( dwKind )
	{
	case eQuestValue_Add:
		{
			++pSubQuest->dwData;
		}
		break;
	case eQuestValue_Minus:
		{
			if( pSubQuest->dwData > 0 )
				--pSubQuest->dwData;		
			else
				pSubQuest->dwData = 0;
		}
		break;
	case eQuestValue_Reset:
		{
			pSubQuest->dwData = 0;
		}
		break;
	}
	return TRUE;
}

void CQuest::OnQuestEvent( CPlayer* pPlayer, CQuestGroup* pQuestGroup, CQuestEvent* pQuestEvent, DWORD dwQuestIdx )
{
	if( dwQuestIdx != 0 )
	if( m_pQuestInfo->GetQuestIdx() != dwQuestIdx )
		return;

	if( !CheckTime( pQuestEvent->m_dwQuestEventKind ) )	return;

	if( m_CurSubQuestInfoArray[0] &&
		m_CurSubQuestInfoArray[0]->OnQuestEvent( pPlayer, pQuestGroup, this, pQuestEvent ) == TRUE )
		return;

	for( int i = m_dwSubQuestCount-1; i > 0; --i )
	{
		if( m_CurSubQuestInfoArray[i] &&
			m_CurSubQuestInfoArray[i]->OnQuestEvent( pPlayer, pQuestGroup, this, pQuestEvent ) == TRUE )
			return;
	}
}

// getfn
BOOL CQuest::IsSubQuestComplete( DWORD dwSubQuestIdx )
{
	DWORD value = 1;
	return ( m_dwSubQuestFlag & ( value << (31-dwSubQuestIdx) ) ) ? TRUE : FALSE;
}

DWORD CQuest::GetQuestIdx()
{
	return m_pQuestInfo->GetQuestIdx();
}

BOOL CQuest::CheckTime( DWORD dwEventKind )
{
	if( !m_bCheckTime )						return TRUE;

	if( m_CheckType == 1 || m_CheckType == 2 )
	{
		if( dwEventKind != eQuestEvent_Time && dwEventKind != eQuestEvent_NpcTalk )	return FALSE;
		
		stTIME curtime;
		curtime.value = GetCurTime();

		if( curtime > m_CheckTime )
		{
			m_bCheckTime = FALSE;
			m_CheckType = 0;
			m_CheckTime.value = 0;
			// dbupdate
			QUESTMGR->UpdateQuestCheckTime( m_pPlayer, GetQuestIdx(), 0, 0 );

			MSG_DWORD Msg;
			Msg.Category = MP_QUEST;
			Msg.Protocol = MP_QUEST_UNREGIST_CHECKTIME;
			Msg.dwData = GetQuestIdx();
			m_pPlayer->SendMsg( (MSGBASE*)&Msg, sizeof(Msg) );

			return TRUE;
		}
		else 
		{
			if( dwEventKind == eQuestEvent_NpcTalk )
			{
				MSG_DWORD2 Msg;
				Msg.Category = MP_QUEST;
				Msg.Protocol = MP_QUEST_TIME_LIMIT;
				Msg.dwData1 = GetQuestIdx();
				Msg.dwData2 = m_CheckTime.value;
				m_pPlayer->SendMsg( (MSGBASE*)&Msg, sizeof(Msg) );
				return FALSE;
			}

			return FALSE;
		}
	}
	else if( m_CheckType == 3 || m_CheckType == 4 )
	{
		stTIME curtime;
		curtime.value = GetCurTime();
		if( curtime > m_CheckTime )
		{
			m_bCheckTime = FALSE;
			m_CheckType = 0;
			m_CheckTime.value = 0;
			QUESTMGR->DeleteQuest( m_pPlayer, GetQuestIdx(), TRUE );
			return FALSE;
		}
		else
			return TRUE;
	}

	return TRUE;
}

void CQuest::RegistCheckTime( BOOL bStart, BYTE bCheckType, stTIME sTime )
{
	m_bCheckTime = bStart;
	m_CheckType = bCheckType;
	m_CheckTime = sTime;

	// DB
	QUESTMGR->UpdateQuestCheckTime( m_pPlayer, GetQuestIdx(), m_CheckType, m_CheckTime.value );

	MSG_DWORD3 Msg;
	Msg.Category = MP_QUEST;
	Msg.Protocol = MP_QUEST_REGIST_CHECKTIME;
	Msg.dwData1 = GetQuestIdx();
	Msg.dwData2 = m_CheckType;
	Msg.dwData3 = m_CheckTime.value;
	m_pPlayer->SendMsg( (MSGBASE*)&Msg, sizeof(Msg) );
}

BOOL CQuest::IsRepeat() 
{ 
	return m_pQuestInfo->IsRepeat(); 
}

void CQuest::InitDateTime()
{
	// 유효시간정보 설정
	m_IsValidDateTime = m_pQuestInfo->IsValidDateTime();
	m_NextDateTime = ( m_IsValidDateTime == TRUE ? m_pQuestInfo->GetNextEndDateTime() : m_pQuestInfo->GetNextStartDateTime() );
}

BOOL CQuest::IsValidDateTime()
{
	if( !m_pQuestInfo->HasDateLimit() )
		return TRUE;

	__time64_t curTime=0;
	_time64( &curTime );

	// 다음 시작/종료 시간 검사
	if( m_NextDateTime < curTime )
	{
		InitDateTime();
		
	}

	return m_IsValidDateTime;
}

BOOL CQuest::HasDateLimit() const
{
	return m_pQuestInfo->HasDateLimit();
}