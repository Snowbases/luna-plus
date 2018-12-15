#include "stdafx.h"
#include ".\autonoteroom.h"
#include "UserTable.h"
#include "Player.h"
#include "AutoNoteManager.h"


CAutoNoteRoom::CAutoNoteRoom(void)
{
}

CAutoNoteRoom::~CAutoNoteRoom(void)
{
}

void CAutoNoteRoom::CreateRoom( CPlayer* pAskPlayer, CPlayer* pAutoPlayer, DWORD dwQuestion )
{
	m_dwAskCharacter		= pAskPlayer->GetID();
	m_dwAutoCharacter		= pAutoPlayer->GetID();
	m_dwAskUserIdx			= pAskPlayer->GetUserID();		//나중에 에이젼트로 메세지 보낼때 필요하다.
	m_dwAutoUserIdx			= pAutoPlayer->GetUserID();		//나중에 에이젼트로 메세지 보낼때 필요하다.

	SafeStrCpy( m_AutoCharacterName, pAutoPlayer->GetObjectName(), MAX_NAME_LENGTH+1 );

	pAutoPlayer->SetAutoNoteIdx( pAskPlayer->GetID() );

	m_nANRState				= eANRS_WAITANSWER;
	m_dwCreateTime			= gCurTime;

	m_nChance				= 3;

/*
	m_dwKey = 0;
	DWORD dwUnitKey[8];
	int i;
	for( i = 0 ; i < 8 ; ++i )					//0~3까지 숫자로 이루어진 8자리수를 만든다.
	{
		dwUnitKey[7-i] = rand()%4;
		m_dwKey *= 10;
		m_dwKey += dwUnitKey[7-i];
	}

	int n = 0;
	for( i = 0 ; i < 4 ; ++i )
	{
		n += dwUnitKey[i]+1;
		m_dwAskColor[i] = dwUnitKey[ n % 8 ];	//--key값을 통해 문제를 구한다.
	}

	for( i = 0 ; i < 4 ; ++i )					//--key값을 통해 칼라테이블을 섞는다.
		m_dwColorTable[i] = i;

	int swap = m_dwColorTable[dwUnitKey[3]];	m_dwColorTable[dwUnitKey[3]] = m_dwColorTable[dwUnitKey[2]];	m_dwColorTable[dwUnitKey[2]] = swap;
	swap = m_dwColorTable[dwUnitKey[2]];		m_dwColorTable[dwUnitKey[2]] = m_dwColorTable[dwUnitKey[1]];	m_dwColorTable[dwUnitKey[1]] = swap;
	swap = m_dwColorTable[dwUnitKey[1]];		m_dwColorTable[dwUnitKey[1]] = m_dwColorTable[dwUnitKey[0]];	m_dwColorTable[dwUnitKey[0]] = swap;
	
	m_dwAnswerKey = dwUnitKey[7]*100 + dwUnitKey[6]*10 + dwUnitKey[5] * 2;	//---기본키 세팅
	m_dwAnswerKey += dwUnitKey[6]*100 + dwUnitKey[5]*10 + dwUnitKey[4] * 3;	
*/

	for( int i = 0 ; i < 4 ; ++i )
	{
		m_dwAskColor[i] = rand()%4;	//--key값을 통해 문제를 구한다.
	}
}

void CAutoNoteRoom::AskToAuto()
{
	CPlayer* pAskPlayer = (CPlayer*)g_pUserTable->FindUser(m_dwAskCharacter);
	CPlayer* pAutoPlayer = (CPlayer*)g_pUserTable->FindUser(m_dwAutoCharacter);
	
	if( pAskPlayer == NULL || pAutoPlayer == NULL ) return;

//----------
	AUTONOTEMGR->MakeSendRaster( m_dwAskColor );

	MSG_AUTONOTE_IMAGE msg3;
	msg3.Category	= MP_AUTONOTE;
	msg3.Protocol	= MP_AUTONOTE_ASKTOAUTO_IMAGE;
	memcpy( msg3.Image, AUTONOTEMGR->GetSendRaster(), 128*32*3 );
	pAutoPlayer->SendMsg( &msg3, sizeof(msg3) );
//----------
/*
	MSG_DWORD3	msg;
	msg.Category	= MP_AUTONOTE;
	msg.Protocol	= MP_AUTONOTE_ASKTOAUTO;
	msg.dwData1		= m_dwKey - m_dwAutoCharacter + m_dwAskCharacter;		//패킷을 감추기위해 한번 꼬아서 보낸다.
	msg.dwData2		= ( m_dwKey * 2 ) + m_dwAutoCharacter - m_dwAskCharacter;
	msg.dwData3		= m_dwKey + m_dwAutoCharacter;
	pAutoPlayer->SendMsg( &msg, sizeof(msg) );
*/
	MSGBASE		msg2;
	msg2.Category	= MP_AUTONOTE;
	msg2.Protocol	= MP_AUTONOTE_ASKTOAUTO_ACK;
	pAskPlayer->SendMsg( &msg2, sizeof(msg2) );
}

BOOL CAutoNoteRoom::CheckAnswerFromAuto( DWORD dwData1, DWORD dwData2, DWORD dwData3, DWORD dwData4 )
{
/*
	DWORD data[4];
	data[0] = dwData1 - m_dwAnswerKey;
	data[1] = data[0] + dwData2 - m_dwAnswerKey;
	data[2] = data[1] + dwData3 - m_dwAnswerKey;
	data[3] = data[2] + dwData4 - m_dwAnswerKey;

	for( int i = 0 ; i < 4 ; ++i )
	{
		if( data[i] > 3 || m_dwAskColor[i] != m_dwColorTable[data[i]] )
		{
			--m_nChance;	//기회 줄임

			return FALSE;
		}
	}
*/

	DWORD data[4];
	data[0] = dwData1;
	data[1] = dwData2;
	data[2] = dwData3;
	data[3] = dwData4;

	for( int i = 0 ; i < 4 ; ++i )
	{
		if( data[i] > 3 || m_dwAskColor[i] != data[i] )
		{
			--m_nChance;	//기회 줄임

			return FALSE;
		}
	}

	return TRUE;
}

BOOL CAutoNoteRoom::IsTimeOver()
{
	if( gCurTime > m_dwCreateTime + 120*1000 + 5*1000/*여유5초*/ )	//---120초
		return TRUE;

	return FALSE;
}

void CAutoNoteRoom::FastAnswer()
{
	m_nANRState		= eANRS_FASTANSWER;
	m_dwCreateTime += ( rand() % 60 - 50 );	//시간을 랜덤하게 함으로써 더 헛갈리게 함

	m_nChance				= 0;
}