// QuestNpcData.cpp: implementation of the CQuestNpcData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"														// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestNpcData.h"												// 퀘스트 NPC 데이터 헤더파일을 불러온다.

#include "QuestScriptLoader.h"											// 퀘스트 스크립트 로더 헤더파일을 불러온다.
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestNpcData::CQuestNpcData( DWORD dwKind, CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// 생성자 함수.
{
	GetScriptParam( m_dwNpcIdx, pTokens );	// 스크립트로 파라메터를 받는다.
}

CQuestNpcData::~CQuestNpcData()				// 소멸자 함수.
{
}
