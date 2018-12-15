// QuestNpcScript.h: interface for the CQuestNpcScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_)
#define AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStrTokenizer;																			// 스트링 토크나이져 클래스를 선언한다.

class CQuestNpcScript																			// NPC 스크립트 클래스.
{
protected:
	DWORD		m_dwQuestIdx;																	// 퀘스트 인덱스를 담는 변수.
	DWORD		m_dwSubQuestIdx;																// 서브 퀘스트 인덱스를 담는 변수.

	DWORD		m_dwKind;																		// 퀘스트 상태에 대한 종류
	DWORD		m_dwNpcIdx;																		// npc 인덱스를 담는 변수.
	DWORD		m_dwScriptPage;																	// 스크립트 페이지를 담는 변수.
	DWORD		m_dwNpcMarkType;																// npc 마크 타입을 담는 변수.
	DWORD		m_dwDialogType ;																// 다이얼로그 출력 타입.

public:
	CQuestNpcScript( CStrTokenizer* pTokens, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	virtual ~CQuestNpcScript();																	// 소멸자 함수.

	DWORD	IsSameNpc( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx );					// npc 스크립트 페이지를 리턴하는 함수.
	DWORD	IsSameNpcMark( DWORD dwNpcIdx, DWORD* pQuestIdx, DWORD* pSubQuestIdx );				// npc 마크를 리턴하는 함수.

	DWORD	GetKind() const				{ return m_dwKind; }
	DWORD	GetNpcIdx() const			{ return m_dwNpcIdx ; }
	DWORD	GetScriptPage() const		{ return m_dwScriptPage ; }
	DWORD	GetNpcMarkType() const		{ return m_dwNpcMarkType ; }
	DWORD	GetDialogType() const		{ return m_dwDialogType ; }
	DWORD	GetNpcScriptPage() const		{ return m_dwScriptPage; }							// npc 스크립트 페이지를 리턴하는 함수.
};

#endif // !defined(AFX_QUESTNPCSCRIPT_H__D5E07727_465F_40AF_93D5_889FF980954F__INCLUDED_)
