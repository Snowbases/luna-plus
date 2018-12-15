//=================================================================================================
//	FILE		: HelperDlg.cpp
//	PURPOSE		: Implemention part of helper dialog.
//	PROGRAMMER	: Yongs Lee
//	DATE		: October 16, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		INCLUDE HEADER FILES
//=========================================================================
#include "stdafx.h"															// 표준 헤더파일을 포함한다.
#include ".\helperdlg.h"													// 도움말 다이얼로그 헤더를 포함한다.

#include "../WindowIDEnum.h"												// 윈도우 아이디 정의 헤더를 포함한다.
#include "GameIn.h"															// gamein 헤더를 포함한다.

#include "./interface/cListDialog.h"										// 리스트 다이얼로그 헤더를 포함한다.
#include "../Input/Mouse.h"													// 마우스 클래스 헤더를 포함한다.
#include "./Interface/cCheckBox.h"											// 체크박스 클래스 헤더를 포함한다.

#include "TutorialDlg.h"													// 튜토리얼 다이얼로그 헤더를 포함한다.

#include "OptionManager.h"													// 옵션 매니져를 포함한다.





//=========================================================================
//		도움말 다이얼로그의 생성자 함수.
//=========================================================================
cHelperDlg::cHelperDlg(void)
{
	m_pHelpList		= NULL ;												// 도움말 리스트 포인터를 null처리를 한다.

	m_pShowTutorial = NULL ;												// 튜토리얼의 보이기 여부를 체크하는 체크박스 포인터를 null처리를 한다.

	m_pStartBtn		= NULL ;												// 시작 버튼 포인터를 null처리를 한다.
	m_pCloseBtn		= NULL ;												// 닫기 버튼 포인터를 null처리를 한다.

	m_pLinkList		= NULL ;												// 도움말 링크 리스트 포인터를 null 처리를 한다.

	m_nSelectedLinkNum = 0 ;												// 선택 된 링크 번호를 0로 세팅한다.
}





//=========================================================================
//		도움말 다이얼로그의 소멸자 함수.
//=========================================================================
cHelperDlg::~cHelperDlg(void)												// 소멸자 함수.
{
}





//=========================================================================
//		도움말 다이얼로그의 하위 컨트롤 링크 함수.
//=========================================================================
void cHelperDlg::Linking()													// 다이얼로그 하위 컨트롤들을 링크하는 함수.
{
	m_pHelpList	= (cListDialog*)GetWindowForID(HD_LINKLIST);
	m_pShowTutorial	= (cCheckBox*)GetWindowForID(HD_SHOWTUTORIAL);
	m_pStartBtn = (cButton*)GetWindowForID(HD_START);
	m_pCloseBtn = (cButton*)GetWindowForID(HD_END);
	m_pLinkList = &TUTORIALMGR->GetHelperList();

	HELPLINK* pLink ;														// 도움말 링크 포인터를 선언한다.

	PTRLISTPOS pos = NULL ;													// 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_pLinkList->GetHeadPosition() ;									// 링크 리스트를 헤드로 세팅하고 위치 정보를 받는다.

	while(pos)																// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		pLink = NULL ;														// 링크 포인터를 null 처리를 한다.
		pLink = (HELPLINK*)m_pLinkList->GetNext(pos) ;						// 링크 정보를 받고, 위치를 다음으로 세팅한다.

		if( !pLink ) continue ;												// 링크 정보가 없으면 continue 처리를 한다.

		if( pLink->nIndex == 0 )											// 링크 인덱스가 0과 같으면,
		{
			m_pHelpList->AddItem(pLink->Title, SELECT_LINKCOLOR, pLink->nIndex) ;	// 선택 된 색상으로 아이템을 추가한다.
		}
		else																// 링크 인덱스가 0이 아니면,
		{
			m_pHelpList->AddItem(pLink->Title, DEFULT_LINKCOLOR, pLink->nIndex) ;	// 기본 색상으로 아이템을 추가한다.
		}
	}

	if( OPTIONMGR->GetGameOption()->bShowTutorial )
	{
		m_pShowTutorial->SetChecked(TRUE) ;
	}
	else
	{
		m_pShowTutorial->SetChecked(FALSE) ;
	}
}





//=========================================================================
//		지정 된 컨트롤 이벤트를 처리 할 함수.
//=========================================================================
void cHelperDlg::OnActionEvent(LONG lId, void* p, DWORD we) 				// 지정 된 컨트롤 이벤트를 처리 할 함수.
{
	switch(lId)
	{
	case HD_START :															// 시작 버튼이 눌려졌다면,
		{
			if(m_nSelectedLinkNum < 0)										// 선택 된 줄 번호가 0보다 작으면,
			{
				#ifdef _GMTOOL_												// GM툴 모드라면,
				char tempBuf[128] = {0, } ;									// 에러 출력용 임시 버퍼를 선언한다.
				sprintf(tempBuf, HD_EMSG2, m_nSelectedLinkNum) ;			// 에러 메시지를 완성한다.
				MessageBox(NULL, tempBuf, OAE_CAPTION, MB_OK);				// 에러 메시지를 메시지 박스로 띄운다.
				#endif

				return ;													// return 처리한다.
			}

			cTutorial* pTutorial = NULL ;									// 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pTutorial = Get_Linked_Tutorial() ;								// 링크 된 튜토리얼을 받는다.

			if( !pTutorial )												// 튜토리얼 정보가 유효하지 않으면,
			{
				#ifdef _GMTOOL_												// GM툴 모드라면,
				char tempBuf[128] = {0, } ;									// 에러 출력용 임시 버퍼를 선언한다.
				sprintf(tempBuf, HD_EMSG7, m_nSelectedLinkNum) ;			// 에러 메시지를 완성한다.
				MessageBox(NULL, tempBuf, OAE_CAPTION, MB_OK);				// 에러 메시지를 메시지 박스로 띄운다.
				#endif

				return ;													// return 처리한다.
			}

			TUTORIALMGR->Start_HelpTutorial(pTutorial) ;					// 튜토리얼을 시작한다.
		}
		break ;

	case HD_END :															// 정지 버튼이 눌려졌다면,
		{
			cTutorialDlg* pDlg = NULL ;										// 튜토리얼 다이얼로그 정보를 받을 포인터를 선언하고 null 처리를 한다.
			pDlg = GAMEIN->GetTutorialDlg() ;								// 튜토리얼 다이얼로그 정보를 받는다.

			if( !pDlg )														// 튜토리얼 다이얼로그 정보가 유효하지 않으면,
			{
				#ifdef _GMTOOL_												// GM툴 모드라면,
				MessageBox(NULL, HD_EMSG8, OAE_CAPTION, MB_OK);				// 에러 메시지를 메시지 박스로 띄운다.
				#endif

				return ;													// return 처리한다.
			}

			pDlg->SetActive(FALSE) ;										// 튜토리얼 다이얼로그를 비활성화 한다.
		}
		break ;
	}
}





//=========================================================================
//		마우스 이벤트 처리를 하는 함수.
//=========================================================================
DWORD cHelperDlg::ActionEvent(CMouse* mouseInfo) 							// 마우스 이벤트 처리를 하는 함수.
{
	DWORD we = WE_NULL ;													// 윈도우 이벤트를 받을 변수를 처리하고 null 이벤트로 세팅한다.

	we = cDialog::ActionEvent(mouseInfo) ;									// 기본 다이얼로그 이벤트를 받는다.

	if( !IsActive() ) return we ;											// 활성화 상태가 아니면, we를 return 처리한다.

	int nMouseX = mouseInfo->GetMouseX() ;									// 이벤트가 발생하는 마우스 X좌표를 받는다.
	int nMouseY = mouseInfo->GetMouseY() ;									// 이벤트가 발생하는 마우스 Y좌표를 받는다.

	if( !m_pHelpList->PtInWindow(nMouseX, nMouseY) ) return we ;			// 리스트 상의 이벤트가 아니면, we를 return 처리를 한다.

	if( we & WE_LBTNCLICK )													// 마우스 LBUTTON을 클릭했다면,
	{
		m_nSelectedLinkNum = m_pHelpList->GetCurSelectedRowIdx() ;			// 현재 선택 된 줄 번호를 받는다.

		if(m_nSelectedLinkNum < 0)											// 선택 된 줄 번호가 0보다 작으면,
		{
			#ifdef _GMTOOL_													// GM툴 모드라면,
			char tempBuf[128] = {0, } ;										// 에러 출력용 임시 버퍼를 선언한다.
			sprintf(tempBuf, HD_EMSG2, m_nSelectedLinkNum) ;				// 에러 메시지를 완성한다.
			MessageBox(NULL, tempBuf, AE_CAPTION, MB_OK);					// 에러 메시지를 메시지 박스로 띄운다.
			#endif

			return we ;														// we를 return 처리한다.
		}

		ITEM* pItem = NULL ;												// 아이템 정보를 받을 포인터를 선언하고 null 처리를 한다.
		pItem = m_pHelpList->GetItem(m_nSelectedLinkNum) ;					// 선택 된 줄의 아이템을 받는다.

		if( !pItem )														// 아이템 정보가 유효하지 않으면,
		{
			#ifdef _GMTOOL_													// GM툴 모드라면,
			char tempBuf[128] = {0, } ;										// 에러 출력용 임시 버퍼를 선언한다.
			sprintf(tempBuf, HD_EMSG3, m_nSelectedLinkNum) ;				// 에러 메시지를 완성한다.
			MessageBox(NULL, tempBuf, AE_CAPTION, MB_OK);					// 에러 메시지를 메시지 박스로 띄운다.
			#endif

			return we ;														// we를 return 처리한다.
		}

		ITEM* pDefaultItem ;												// 선택 되지 않은, 다른 아이템 정보를 받을 포인터를 선언한다.

		int listCount = m_pHelpList->GetItemCount() ;						// 현재 도움말 리스트의 아이템 개수를 받는다.

		for( int count = 0 ; count < listCount ; ++count )					// 아이템 수 만큼 for문을 돌린다.
		{
			pDefaultItem = NULL ;											// 기본 아이템 포인터를 null 처리를 한다.
			pDefaultItem = m_pHelpList->GetItem(count) ;					// 카운트에 해당하는 아이템 정보를 받는다.

			if( !pDefaultItem )												// 아이템 정보가 유효하지 않으면,
			{
				#ifdef _GMTOOL_												// GM툴 모드라면,
				char tempBuf[128] = {0, } ;									// 에러 출력용 임시 버퍼를 선언한다.
				sprintf(tempBuf, HD_EMSG4, m_nSelectedLinkNum) ;			// 에러 메시지를 완성한다.
				MessageBox(NULL, tempBuf, AE_CAPTION, MB_OK);				// 에러 메시지를 메시지 박스로 띄운다.
				#endif

				continue ;													// continue 처리를 한다.
			}

			pDefaultItem->rgb = DEFULT_LINKCOLOR ;							// 아이템 색상을 기본 색상으로 세팅한다.
		}

		pItem->rgb = SELECT_LINKCOLOR ;										// 선택 된 아이템의 색상을 선택 된 색상으로 세팅한다.
	}
	else if( we & WE_LBTNDBLCLICK )											// 더블 클릭을 했다면,
	{
		cTutorial* pTutorial = NULL ;										// 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
		pTutorial = Get_Linked_Tutorial() ;									// 링크 된 튜토리얼을 받는다.

		if( !pTutorial )													// 튜토리얼 정보가 유효하지 않으면,
		{
			#ifdef _GMTOOL_													// GM툴 모드라면,
			char tempBuf[128] = {0, } ;										// 에러 출력용 임시 버퍼를 선언한다.
			sprintf(tempBuf, HD_EMSG7, m_nSelectedLinkNum) ;				// 에러 메시지를 완성한다.
			MessageBox(NULL, tempBuf, AE_CAPTION, MB_OK);					// 에러 메시지를 메시지 박스로 띄운다.
			#endif

			return we ;														// we를 return 처리한다.
		}

		TUTORIALMGR->Start_HelpTutorial(pTutorial) ;						// 튜토리얼을 시작한다.
	}

	return we ;																// we를 return 처리한다.
}





//=========================================================================
//		도움말 다이얼로그 렌더 함수.
//=========================================================================
void cHelperDlg::Render()													// 렌더 함수.
{
	if( !IsActive() ) return ;												// 활성화 상태가 아니면, return 처리를 한다.

	cDialog::RenderWindow();												// 윈도우를 렌더한다.
	cDialog::RenderComponent();												// 하위 객체들을 렌더한다.
}





//=========================================================================
//		링크 된 튜토리얼 정보를 반환하는 함수.
//=========================================================================
cTutorial* cHelperDlg::Get_Linked_Tutorial()			 					// 링크 된 튜토리얼 정보를 반환하는 함수.
{
	ITEM* pItem = NULL ;													// 아이템 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pItem = m_pHelpList->GetItem(m_nSelectedLinkNum) ;						// 선택 된 줄의 아이템을 받는다.

	if( !pItem )															// 아이템 정보가 유효하지 않으면,
	{
		#ifdef _GMTOOL_														// GM툴 모드라면,
		char tempBuf[128] = {0, } ;											// 에러 출력용 임시 버퍼를 선언한다.
		sprintf(tempBuf, HD_EMSG3, m_nSelectedLinkNum) ;					// 에러 메시지를 완성한다.
		MessageBox(NULL, tempBuf, GLT_CAPTION, MB_OK);						// 에러 메시지를 메시지 박스로 띄운다.
		#endif

		return NULL ;														// null return 처리한다.
	}

	PTRLISTPOS pos = NULL ;													// 위치 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pos = m_pLinkList->FindIndex(pItem->line) ;								// 아이템의 인덱스에 해당하는 링크 위치를 받는다.

	if( !pos )																// 위치 정보를 받는데 실패 했다면,
	{
		#ifdef _GMTOOL_														// GM툴 모드라면,
		char tempBuf[128] = {0, } ;											// 에러 출력용 임시 버퍼를 선언한다.
		sprintf(tempBuf, HD_EMSG5, pItem->line) ;							// 에러 메시지를 완성한다.
		MessageBox(NULL, tempBuf, GLT_CAPTION, MB_OK);						// 에러 메시지를 메시지 박스로 띄운다.
		#endif

		return NULL ;														// null return 처리한다.
	}				

	HELPLINK* pLinkItem = NULL ;											// 도움말 링크 포인터를 선언하고 null 처리를 한다.
	pLinkItem = (HELPLINK*)m_pLinkList->GetAt(pos) ;						// 위치에 해당하는 링크 아이템을 받는다.

	if( !pLinkItem )														// 링크 아이템 정보가 유효하지 않으면,
	{
		#ifdef _GMTOOL_														// GM툴 모드라면,
		char tempBuf[128] = {0, } ;											// 에러 출력용 임시 버퍼를 선언한다.
		sprintf(tempBuf, HD_EMSG6, pItem->line) ;							// 에러 메시지를 완성한다.
		MessageBox(NULL, tempBuf, GLT_CAPTION, MB_OK);						// 에러 메시지를 메시지 박스로 띄운다.
		#endif

		return NULL ;														// null return 처리한다.
	}

	cTutorial* pTutorial = NULL ;											// 튜토리얼 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pTutorial = TUTORIALMGR->Get_Tutorial(pLinkItem->nTutorial_Idx) ;		// 링크 된 튜토리얼 정보를 받는다.

	return pTutorial ;														// 튜토리얼 정보를 return 처리한다.
}