#pragma once
//=================================================================================================
//	FILE		: HelperDlg.h
//	PURPOSE		: Window interface to render Help list.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 16, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		GM툴 모드로 실행 할 때 에러 메시지를 출력하기 위한 에러 메시저 정의
//=========================================================================
#ifdef _GMTOOL_

// CATION PART.
#define LK_CAPTION	"cHelperDlg::Linking"									// 링크 함수 캡션.
#define AE_CAPTION	"cHelperDlg::ActionEvent"								// 액션 이벤트 함수 캡션.
#define OAE_CAPTION	"cHelperDlg::OnActionEvent"								// 온 액션 이벤트 함수 캡션.
#define GLT_CAPTION	"cHelperDlg::Get_Linked_Tutorial"						// 링크 된 튜토리얼 반환 함수 캡션.

// ERROR MESSAGE
#define HD_EMSG1	"Failed to receive helper link list."					// 도움말 링크 받기 실패.
#define HD_EMSG2	"Invalid Selected row number. %d"						// 선택 된  줄 번호 에러.
#define HD_EMSG3	"Failed to receive selected item pointer. %d"			// 선택 된 아이템 포인터 받기 실패.
#define HD_EMSG4	"Failed to receive default item pointer. %d"			// 기본 아이템 포인터 받기 실패.
#define HD_EMSG5	"Failed to receive link item position. %d"				// 링크 아이템 포지션 받기 실패.
#define HD_EMSG6	"Failed to receive link item pointer. %d"				// 링크 아이템 포인터 받기 실패.
#define HD_EMSG7	"Failed to receive linked tutorial pointer. %d"			// 링크 된 튜토리얼 포인터 받기 실패.
#define HD_EMSG8	"Failed to receive tutorial dialog's pointer."			// 튜토리얼 다이얼로그 포인터 받기 실패.

#endif //_GMTOOL_





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "./Interface/cDialog.h"											// 다이얼로그 인터페이스를 포함한다.

#include "TutorialManager.h"												// 튜토리얼 매니져 헤더를 포함한다.





//=========================================================================
//		필수 정의 파트.
//=========================================================================
#define DEFULT_LINKCOLOR	RGBA_MAKE(255, 255, 255, 255)					// 기본 링크 색상을 정의 한다.
#define SELECT_LINKCOLOR	RGBA_MAKE(255, 255, 0, 255)						// 선택 된 색상을 정의 한다.





//=========================================================================
//		필요한 컨트롤 인터페이스를 선언한다.
//=========================================================================
class cListDialog ;															// 리스트 다이얼로그 클래스를 선언한다.
class cCheckBox ;															// 체크 박스 클래스를 선언한다.
class cButton ;																// 버튼 클래스를 선언한다.





//=========================================================================
//		도움말 링크 다이얼로그 클래스 정의
//=========================================================================
class cHelperDlg : public cDialog											// 도움말 파트를 진행할 다이얼로그 클래스를 정의한다.
{
	cListDialog*			m_pHelpList ;									// 도움말 리스트 포인터.

	cCheckBox*				m_pShowTutorial ;								// 튜토리얼의 보이기 여부를 체크하는 체크박스 포인터.

	cButton*				m_pStartBtn ;									// 시작 버튼 포인터.
	cButton*				m_pCloseBtn ;									// 닫기 버튼 포인터.

	cPtrList*				m_pLinkList ;									// 도움말 링크 리스트 포인터.

	int						m_nSelectedLinkNum ;							// 선택 된 링크 번호를 담는 멤버 변수.

public:
	cHelperDlg(void);														// 생성자 함수.
	virtual ~cHelperDlg(void);												// 소멸자 함수.

	void Linking() ;														// 다이얼로그 하위 컨트롤들을 링크하는 함수.

	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;				// 지정 된 컨트롤 이벤트를 처리 할 함수.
	virtual DWORD ActionEvent(CMouse* mouseInfo) ;							// 마우스 이벤트 처리를 하는 함수.

	virtual void Render() ;													// 렌더 함수.

	cTutorial* Get_Linked_Tutorial() ;										// 링크 된 튜토리얼 정보를 반환하는 함수.
};
