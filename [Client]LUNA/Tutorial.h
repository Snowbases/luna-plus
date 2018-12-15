#pragma once
//=================================================================================================
//	FILE		: Tutorial.h
//	PURPOSE		: The class that stored all info of tutorial.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 16, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "stdafx.h"															// 표준 헤더파일을 포함한다.




//=========================================================================
//		필요한 컨트롤 인터페이스를 선언한다.
//=========================================================================
enum START_CONDITION														// 튜토리얼 발동 조건 enum을 정의한다.
{
	e_FIRST_GAMEIN = 0,														// 처음 게임에 들어섰을 때.
	e_COMPLETE_TUTORIAL,													// 정해진 튜토리얼을 완료 했을 때.
	e_LEVEL,																// 일정 레벨에 도달했을 때.
	e_GET_ITEM,																// 특정 아이템을 획득했을 때.
	e_TALK_NPC,																// 특정 npc와 대화를 시작했을 때.
	e_NOT_RUNNING,															// 상황에 따른 발동이 되지 않는 튜토리얼.

	e_MAX_SCONDITION,														// 최대 발동 조건 수.
} ;

enum COMPLETE_CONDITION														// 튜토리얼 완료 조건 enum을 정의한다.
{
	e_CLICK_OKBTN = 0,														// OK 버튼을 클릭 해야 할 때.
	e_NPC_TALK,																// 특정 npc와 대화를 해야 할 때.
	e_OPEN_DIALOG,															// 특정 다이얼로그를 열어야 할때.
	e_CLICK_SPECIFIED_BTN,													// 지정된 버튼을 클릭해야 할 때,
	e_RECEIVE_QUEST,														// 지정된 퀘스트를 수락해야 할 때,

	e_MAX_CCONDITION,														// 최대 완료 조건 수.
} ;

enum ITEM_CONDITION
{
	e_GET_FIRST_ITEM = 0,
	e_GET_STACK_ITEM,
} ;

struct SUB_TUTORIAL															// 서브 튜토리얼 구조체를 정의 한다.
{
	int nIndex;
	int nSpeech_Idx;
	SNDIDX nSound_Idx;
	int nNpc_Idx;
	int nFace_Idx;
	int nComplete_Condition1;
	int nComplete_Condition2;
	char szCompleteCondition2[24];

	SUB_TUTORIAL()
	{
		ZeroMemory(
			this,
			sizeof(*this));
	};
} ;





//=========================================================================
//		튜토리얼 클래스 정의
//=========================================================================
class cTutorial																// 튜토리얼 정보를 담을 클래스를 정의한다.
{
	int m_nIndex ;															// 튜토리얼 인덱스.

	int m_nStartCondition1 ;												// 발동 조건1.
	int m_nStartCondition2 ;												// 발동 조건2.

	BOOL	m_bCompleted ;													// 현재 튜토리얼의 완료 여부를 체크하는 변수.

	cPtrList			m_SubTutorial_List ;								// 서브 튜토리얼 정보를 담을 리스트.
public:
	cTutorial(void);														// 생성자 함수.
	virtual ~cTutorial(void);												// 소멸자 함수.

	void Init() ;															// 초기화 함수.

	void Release() ;														// 해제 함수.

	void Set_Index(int nIndex) { m_nIndex = nIndex ; }						// 튜토리얼 인덱스를 세팅하는 함수.
	int  Get_Index() { return m_nIndex ; }									// 튜토리얼 인덱스를 반환하는 함수.

	void Set_StartCondition1(int nStartCondition1)							// 제 1 발동 조건을 세팅하는 함수.
	{
		m_nStartCondition1 = nStartCondition1 ; 							// 제 1 발동 조건을 세팅한다.
	}
	int  Get_StartCondition1() { return m_nStartCondition1 ; }				// 제 1 발동 조건을 반환하는 함수.

	void Set_StartCondition2(int nStartCondition2)							// 제 2 발동 조건을 세팅하는 함수.
	{
		m_nStartCondition2 = nStartCondition2 ; 							// 제 2 발동 조건을 세팅한다.
	}
	int  Get_StartCondition2() { return m_nStartCondition2 ; }				// 제 2 발동 조건을 반환하는 함수.

	int	 Get_SubTutorial_Count() { return m_SubTutorial_List.GetCount() ; }	// 서브 튜토리얼 리스트의 카운트를 리턴하는 함수.

	void Add_SubTutorial(SUB_TUTORIAL* pSubTutorial) ;						// 서브 튜토리얼을 추가하는 함수.
	SUB_TUTORIAL* Get_SubTutorial(int nIndex) ;								// 인덱스에 해당하는 서브 튜토리얼 정보를 리턴하는 함수.

	void Set_Complete(BOOL bVal) { m_bCompleted = bVal ; }					// 튜토리얼의 완료 여부를 세팅하는 함수.
	BOOL Is_Completed() { return m_bCompleted ; }							// 튜토리얼의 완료 여부를 리턴하는 함수.
};
