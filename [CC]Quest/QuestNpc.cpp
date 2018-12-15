// QuestNpc.cpp: implementation of the CQuestNpc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																			// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestNpc.h"																		// 퀘스트 NPC 헤더를 불러온다.

#include "QuestScriptLoader.h"																// 퀘스트 스크립터 로더 헤더를 불러온다.
#include "QuestNpcData.h"																	// 퀘스트 NPC 데이터 헤더를 불러온다.
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestNpc::CQuestNpc( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )		// 생성자 함수.
{
	m_dwQuestIdx = dwQuestIdx;																// 퀘스트 인덱스를 세팅한다.
	m_dwSubQuestIdx = dwSubQuestIdx;														// 서브 퀘스트 인덱스를 세팅한다.
	ReadNpcData( pTokens, dwQuestIdx, dwSubQuestIdx );										// NPC 데이터를 읽어들인다.
}

CQuestNpc::~CQuestNpc()																		// 소멸자 함수.
{
	Release();																				// 해제 함수를 호출한다.
}

void CQuestNpc::Release()																	// 해제 함수.
{
	CQuestNpcData* pNpcData = NULL;															// 퀘스트 NPC 데이터 정보를 담을 포인터를 선언하고 NULL 처리를 한다.

	PTRLISTPOS pos = m_NpcDataList.GetHeadPosition();										// NPC 데이터 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while( pos )																			// 위치 정보가 유효한동안 while문을 돌린다.
	{
		pNpcData = (CQuestNpcData*)m_NpcDataList.GetNext( pos );							// 위치에 해당하는 npc 데이터를 받는다.

		if( pNpcData )																		// npc 데이터 정보가 유효하면,
		{
			delete pNpcData;																// npc 데이터 정보를 삭제한다.
		}
	}
}

BOOL CQuestNpc::ReadNpcData( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// npc 데이터 정보를 읽는 함수.
{
	char* OneToken;																				// 토큰 포인터를 선언한다.

	while( (OneToken = pTokens->GetNextTokenUpper()) != NULL )									// 토큰 정보가 유효한동안 while문을 돌린다.
	{
		if( OneToken[0] == '@' )																// 토큰이 @와 같다면,
		{
			m_NpcDataList.AddTail( CQuestScriptLoader::LoadQuestNpcData( OneToken, pTokens, dwQuestIdx, dwSubQuestIdx ) );	// npc 데이터 리스트에 npc데이터럴 로드해
		}																													// 추가한다.
	}
	return TRUE;																				// TRUE를 리턴한다.
}