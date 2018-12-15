// QuestLimit.cpp: implementation of the CQuestLimit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																			// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestLimit.h"																		// 퀘스트 제한 헤더파일을 불러온다.

#include "QuestScriptLoader.h"																// 스크립트 로더 헤더를 불러온다.
#include "QuestLimitKind.h"																	// 퀘스트 제한 종류 헤더를 불러온다.
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestLimit::CQuestLimit( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// 생성자 함수.
{
	ReadLimitKind( pTokens,  dwQuestIdx, dwSubQuestIdx );									// 퀘스트 제한 종류를 읽어들인다.
}

CQuestLimit::~CQuestLimit()																	// 소멸자 함수.
{
	Release();																				// 해제 함수를 호출한다.
}

void CQuestLimit::Release()																	// 해제 함수.
{
	PTRLISTPOS pos = m_QuestLimitKindList.GetHeadPosition();								// 퀘스트 리미트 종류 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	CQuestLimitKind* pQuestLimitKind = NULL;												// 퀘스트 리미트 정보를 받을 포인터를 선언하고 null 처리를 한다.

	while( pos )																			// 위치 정보가 유효할 동안 while을 돌린다.
	{
		pQuestLimitKind = (CQuestLimitKind*)m_QuestLimitKindList.GetNext( pos );			// 퀘스트 리미트 종류 정보를 받는다.

		if( pQuestLimitKind )																// 퀘스트 리미트 종류 정보가 유효하면,
		{
			delete pQuestLimitKind;															// 퀘스트 리미트 종류 정보를 삭제한다.
		}
	}
	m_QuestLimitKindList.RemoveAll();														// 퀘스트 리미트 종류 정보를 담는 리스트를 모두 비운다.
}

BOOL CQuestLimit::ReadLimitKind( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// 리미트 종류를 읽어들이는 함수.
{
	char* OneToken;																					// 토큰 포인터를 선언한다.

	while( (OneToken = pTokens->GetNextTokenUpper()) != NULL )										// 토큰 포인터가 NULL이 아닌경우 while문을 돌린다.
	{
		if( OneToken[0] == '&' )																	// 토큰이 &와 같으면,
		{
			// 퀘스트 리미트 종류 정보를 담는 리스트에 추가한다.
			m_QuestLimitKindList.AddTail( CQuestScriptLoader::LoadQuestLimitKind( OneToken, pTokens, dwQuestIdx, dwSubQuestIdx ) );
		}
	}
	return TRUE;																					// TRUE를 리턴한다.
}

BOOL CQuestLimit::CheckLimit( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest )		// 퀘스트 리미트를 체크한다.(서버)
{
	PTRLISTPOS pos = m_QuestLimitKindList.GetHeadPosition();										// 퀘스트 리미트 종류 정보를 담는 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	CQuestLimitKind* pQuestLimitKind = NULL;														// 퀘스트 리미트 정보를 받을 포인터를 선언하고 null 처리를 한다.

	while( pos )																					// 위치 정보가 유효한 동안 while문을 돌린다.
	{
		pQuestLimitKind = (CQuestLimitKind*)m_QuestLimitKindList.GetNext( pos );					// 퀘스트 리미트 종류 정보를 받는다.

		if( pQuestLimitKind )																		// 퀘스트 리미트 종류 정보가 유효하면,
		{
			if( !pQuestLimitKind->CheckLimit( pPlayer, pQuestGroup, pQuest ) )						// 퀘스트 리미트에 걸리는지 체크한다.
			{
				return FALSE;																		// FALSE를 리턴한다.
			}
		}
	}
	return TRUE;																					// TRUE를 리턴한다.
}

BOOL CQuestLimit::CheckLimit( DWORD dwQuestIdx, DWORD dwSubQuestIdx )								// 퀘스트 리미트를 체크한다. (클라이언트)
{
	PTRLISTPOS pos = m_QuestLimitKindList.GetHeadPosition();										// 퀘스트 리미트 종류 정보를 담는 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	CQuestLimitKind* pQuestLimitKind = NULL;														// 퀘스트 리미트 정보를 받을 포인터를 선언한다.

	while( pos )																					// 위치 정보가 유효한동안 while문을 돌린다.
	{
		pQuestLimitKind = (CQuestLimitKind*)m_QuestLimitKindList.GetNext( pos );					// 퀘스트 리미트 종류 정보를 받는다.

		if( pQuestLimitKind )																		// 퀘스트 리미트 종류 정보가 유효하면,
		{
			if( !pQuestLimitKind->CheckLimit( dwQuestIdx, dwSubQuestIdx ) )							// 퀘스트 리미트에 걸리는지 체크한다.
			{
				return FALSE;																		// FALSE를 리턴한다.
			}
		}
	}
	return TRUE;																					// TRUE를 리턴한다.
}
