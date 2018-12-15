// SubQuestInfo.h: interface for the CSubQuestInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_)
#define AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuestDefines.h"															// 퀘스트 정의 선언 헤더파일을 불러온다.

class CQuestLimit;																	// 퀘스트 제한 클래스를 선언한다.
class CQuestTrigger;																// 퀘스트 트리거 클래스를 선언한다.
class CQuestNpcScript;																// 퀘스트 npc 스크립트 클래스를 선언한다.
class CQuestNpc;																	// 퀘스트 npc 클래스를 선언한다.

class CQuestGroup;																	// 퀘스트 그룹 클래스를 선언한다.
class CQuest;																		// 퀘스트 클래스를 선언한다.
class CQuestEvent;																	// 퀘스트 이벤트 클래스를 선언한다.
class CQuestExecute;

class CSubQuestInfo																	// 서브 퀘스트 정보 클래스.
{
protected:
	DWORD		m_dwQuestIdx;
	DWORD		m_dwSubQuestIdx;
	cPtrList	m_QuestLimitList;													// 퀘스트 제한 정보를 담는 리스트.
	cPtrList	m_QuestTriggerList;													// 퀘스트 트리거 정보를 담는 리스트.
	std::map< DWORD, CQuestNpcScript* > m_QuestNpcScript;							// 퀘스트 npc 스크립트 리스트.
	cPtrList	m_QuestNpcList;														// 퀘스트 npc 리스트.

	DWORD		m_dwMaxCount;														// 최대 카운트.

	std::vector< CQuestExecute* > m_FoundQuestExecute;
	std::vector< CQuestExecute* >::const_iterator m_FoundQuestExecuteIter;

	// 100405 ShinJS --- QuestEvent 실행 가능 여부
    BOOL	CanDoOnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest );
	BOOL	CanDoOnQuestEvent( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
public:
	CSubQuestInfo( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	virtual ~CSubQuestInfo();														// 소멸자 함수.
	
	void	Release();																// 해제 함수.
	// script
	void	AddQuestLimit( CQuestLimit* pQuestLimit );								// 퀘스트 제한을 추가하는 함수.
	void	AddQuestTrigger( CQuestTrigger* pQuestTrigger );						// 퀘스트 트리거를 추가하는 함수.

	void	AddQuestNpcScipt( CQuestNpcScript* pQuestNpcScript );					// 퀘스트 npc 스크립트를 추가하는 함수.
	CQuestNpcScript* GetQuestNpcScript();

	void	AddQuestNpc( CQuestNpc* pQuestNpc );									// 퀘스트 npc를 추가하는 함수.

	// event
	BOOL	OnQuestEvent( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, CQuest* pQuest, CQuestEvent* pQuestEvent);	// 퀘스트 이벤트를 적용하는 함수.
	// npc
	BOOL	IsNpcRelationQuest( DWORD dwNpcIdx );									// npc와 관련된 퀘스트 인지 여부를 반환하는 함수.
	DWORD	GetNpcScriptPage( DWORD dwNpcIdx );										// npc  스크립트 페이지를 반환하는 함수.
	DWORD	GetNpcMarkType( DWORD dwNpcIdx );										// npc 마크 타입을 반환하는 함수.
	eQuestState IsQuestState( DWORD dwNpcIdx );										// 퀘스트 상태를 확인하는 함수.
	cPtrList& GetQuestNpcList() { return m_QuestNpcList; }
	BOOL	CheckLimitCondition( DWORD dwQuestIdx );
	DWORD	GetMaxCount() const { return m_dwMaxCount; }
	void	SetMaxCount( DWORD dwMaxCount )	{ m_dwMaxCount = dwMaxCount; }
	cPtrList& GetLimitList() { return m_QuestLimitList ; }
	cPtrList& GetTriggerList() { return m_QuestTriggerList ; }
	cPtrList& GetNpcList() { return m_QuestNpcList ; }
	// 100422 ShinJS --- eQuestExecute 종류가 같은 CQuestExecute를 찾는다.
	void	StartLoopByQuestExecuteKind( DWORD dwExecuteKind );
	// 100422 ShinJS --- StartLoopByQuestExecuteKind()로 찾은 eQuestExecute 종류가 같은 CQuestExecute를 순회한다.
	CQuestExecute* GetNextQuestExecute();
	const std::map< DWORD, CQuestNpcScript* >& GetQuestNpcScriptList() const { return m_QuestNpcScript; }
};

#endif // !defined(AFX_SUBQUESTINFO_H__AC05E747_C207_4E04_8D35_09F423BA4A7E__INCLUDED_)
