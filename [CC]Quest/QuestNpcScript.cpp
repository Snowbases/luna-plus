// QuestNpcScript.cpp: implementation of the CQuestNpcScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"															// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.
#include "QuestDefines.h"
#include "QuestNpcScript.h"
#include "QuestScriptLoader.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestNpcScript::CQuestNpcScript( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx )	// 생성자 함수.
{
	m_dwQuestIdx = dwQuestIdx;														// 퀘스트 인덱스를 세팅한다.
	m_dwSubQuestIdx = dwSubQuestIdx;												// 서브 퀘스트 인덱스를 세팅한다.

	const char* token = pTokens->GetNextTokenUpper();
	if( !token )
	{
		m_dwKind = 0;
	}
	else if( stricmp( token, "@NPC" ) == 0 )
	{
		m_dwKind = eQuest_NpcScript_Normal;
	}
	else if( stricmp( token, "@DENIAL" ) == 0 )
	{
		m_dwKind = eQuest_NpcScript_Lack;
	}

	GetScriptParam( m_dwNpcIdx, pTokens );						// 스크립트로 부터 npc 인덱스를 받는다.
	GetScriptParam( m_dwScriptPage, pTokens );					// 스크립트로 부터 스크립트 페이지를 받는다.
	GetScriptParam( m_dwNpcMarkType, pTokens );					// 스크립트로 부터 npc 마크 타입을 받는다.
	GetScriptParam( m_dwDialogType, pTokens ) ;					// 다이얼로그 내용 출력 여부를 결정한다.
}

CQuestNpcScript::~CQuestNpcScript()													// 소멸자 함수.
{
}

DWORD CQuestNpcScript::IsSameNpc( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx )			// npc 스크립트 페이지를 리턴하는 함수.
{
	*pQuestIdx = m_dwQuestIdx;														// 퀘스트 인덱스를 세팅한다.
	*pSubQuestIdx = m_dwSubQuestIdx;												// 서브 퀘스트 인덱스를 세팅한다.

	return (dwNpcIdx == m_dwNpcIdx ? m_dwScriptPage : 0);
}

DWORD CQuestNpcScript::IsSameNpcMark( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx )	// npc 마크를 리턴하는 함수.
{
	*pQuestIdx = m_dwQuestIdx;														// 퀘스트 인덱스를 세팅한다.
	*pSubQuestIdx = m_dwSubQuestIdx;												// 서브 퀘스트를 세팅한다.

	return (dwNpcIdx == m_dwNpcIdx ? m_dwNpcMarkType : 0);
}
