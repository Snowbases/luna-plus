//=================================================================================================
//	FILE		: Tutorial.cpp
//	PURPOSE		: Implemention part of tutorial class.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 16, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		INCLUDE HEADER FILES
//=========================================================================
#include ".\tutorial.h"





//=========================================================================
//		튜토리얼 클래스의 생성자 함수.
//=========================================================================
cTutorial::cTutorial(void)													// 생성자 함수.
{
	Init() ;																// 클래스를 초기화 한다.
}





//=========================================================================
//		튜토리얼 클래스의 소멸자 함수.
//=========================================================================
cTutorial::~cTutorial(void)													// 소멸자 함수.
{
	Release() ;																// 해제 함수를 호출한다.
}





//=========================================================================
//		튜토리얼 클래스의 초기화 함수.
//=========================================================================
void cTutorial::Init()
{
	m_nIndex = -1 ;															// 튜토리얼 인덱스를 -1로 초기화 한다.

	m_nStartCondition1 = -1 ;												// 발동 조건1을 -1로 초기화 한다.
	m_nStartCondition2 = -1 ;												// 발동 조건2를 -1로 초기화 한다.

	m_bCompleted = FALSE ;													// 튜토리얼 완료 여부를 false로 세팅한다.

	Release() ;																// 서브 튜토리얼 리스트를 초기화 한다.
}





//=========================================================================
//		튜토리얼 클래스의 헤제 함수.
//=========================================================================
void cTutorial::Release() 													// 해제 함수.
{
	SUB_TUTORIAL* pDelete ;													// 서브 튜토리얼 정보를 담을 포인터를 선언한다.

	PTRLISTPOS pDeletePos = NULL ;											// 리스트 포인터의 위치를 받을 포인터를 선언하고 null 처리를 한다.
	pDeletePos = m_SubTutorial_List.GetHeadPosition() ;						// 서브 튜토리얼을 담은 리스트를 헤드로 세팅하고, 위치정보를 받는다.

	while(pDeletePos)														// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pDelete = NULL ;													// 서브 튜토리얼 정보를 담을 포인터를 null 처리를 한다.
		pDelete = (SUB_TUTORIAL*)m_SubTutorial_List.GetNext(pDeletePos) ;	// 위치에 해당하는 서브 튜토리얼 정보를 받고, 다음 위치를 세팅한다.

		if( !pDelete ) continue ;											// 삭제 할 서브 튜토리얼 정보가 없으면, continue 처리를 한다.

		delete pDelete ;													// 서브 튜토리얼 정보를 삭제한다.
	}

	m_SubTutorial_List.RemoveAll() ;										// 서브 튜토리얼 리스트를 모두 비운다.
}





//=========================================================================
//		서브 튜토리얼 추가 함수.
//=========================================================================
void cTutorial::Add_SubTutorial(SUB_TUTORIAL* pSubTutorial)					// 서브 튜토리얼을 추가하는 함수.
{
	if( !pSubTutorial ) return ;											// 인자로 넘어온 서브 튜토리얼 정보가 유효하지 않으면 return 처리를 한다.

	m_SubTutorial_List.AddTail(pSubTutorial) ;								// 서브 튜토리얼 리스트에, 서브 튜토리얼 정보를 추가한다.
}





//=========================================================================
//		지정된 인덱스의 서브 튜토리얼을 반환하는 함수.
//=========================================================================
SUB_TUTORIAL* cTutorial::Get_SubTutorial(int nIndex) 						// 인덱스에 해당하는 서브 튜토리얼 정보를 리턴하는 함수.
{
	int nSubTutorial_Count = 0 ;											// 서브 튜토리얼 수를 받을 변수를 선언하고 0으로 초기화 한다.
	nSubTutorial_Count = m_SubTutorial_List.GetCount() ;					// 서브 튜토리얼 개수를 받는다.

	if( nIndex < 0 || nIndex >= nSubTutorial_Count ) return NULL ;			// 인자로 넘어온 인덱스가, 0보다 작거나, 현재 서브 튜토리얼 수보다 크면, null return 처리를 한다.

	PTRLISTPOS returnPos = NULL ;											// 인덱스에 해당하는 서브 튜토리얼 위치를 담을 포인터를 선언하고 null 처리를 한다.
	returnPos = m_SubTutorial_List.FindIndex(nIndex) ;						// 인덱스에 해당하는 서브 튜토리얼의 위치정보를 받는다.

	if( !returnPos ) return NULL ;											// 위치 정보가 유효하지 않으면, null return 처리를 한다.

	return (SUB_TUTORIAL*)m_SubTutorial_List.GetAt(returnPos) ;				// 위치에 해당하는, 서브 튜토리얼 정보를 return 처리를 한다.
}