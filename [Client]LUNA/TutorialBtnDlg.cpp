#include "stdafx.h"
#include "tutorialbtndlg.h"
#include "../WindowIDEnum.h"
#include "Interface/cButton.h"
#include "TutorialDlg.h"
#include "GameIn.h"
#include "Audio/MHAudioManager.h"
#include "TutorialManager.h"
#include "OptionManager.h"

cTutorialBtnDlg::cTutorialBtnDlg(void)
{
	m_pButton = NULL ;														// 버튼 포인터를 null 처리를 한다.

	m_nAlpha = 255 ;														// 버튼 알파 값을 255로 세팅한다.
}

cTutorialBtnDlg::~cTutorialBtnDlg(void)
{}

void cTutorialBtnDlg::Linking()
{
	m_pButton = (cButton*)GetWindowForID(TTR_NOTICEBTN);
}

void cTutorialBtnDlg::OnActionEvent(LONG lId, void* p, DWORD we )
{
	if( lId != TTR_NOTICEBTN ) return ;										// 공지 버튼이 아니면 return 처리를 한다.

	cTutorialDlg* pDlg = GAMEIN->GetTutorialDlg() ;							// 튜토리얼 다이얼로그를 얻는다.

	if( !pDlg ) return ;													// 튜토리얼 다이얼로그가 유효하지 않으면, 

	pDlg->Start_Tutorial() ;												// 튜토리얼을 시작한다.

	SetActive(FALSE) ;														// 공지 버튼 다이얼로그를 비활성화 한다.
	m_pButton->SetActive(FALSE) ;											// 버튼을 비활성화 한다.
}

void cTutorialBtnDlg::Render()
{
	if( !IsActive() ) return ;												// 활성화 상태가 아니면, return 처리를 한다.

	DWORD dwCurTick = GetTickCount() ;

	DWORD dwResultTime = dwCurTick - m_dwStartTime ;

	if( dwResultTime >= 1000 )
	{
		m_dwStartTime = GetTickCount() ;
	}

	m_nAlpha = (int)(2.5f * (dwResultTime/10)) ;

	m_pButton->SetAlpha((BYTE)m_nAlpha) ;									// 버튼에 알파 값을 적용한다.

	cDialog::RenderWindow();												// 윈도우를 렌더한다.
	cDialog::RenderComponent();												// 하위 객체들을 렌더한다.

	//m_pButton->Render() ;													// 버튼을 렌더한다.
}

void cTutorialBtnDlg::Active_NoticeBtn()
{
	SetActive(TRUE) ;														// 다이얼로그를 활성화 시킨다.

	m_nAlpha = 255 ;														// 버튼 알파 값을 255로 세팅한다.

	m_dwStartTime = GetTickCount() ;										// 시작 시간을 받는다.

	m_pButton->SetActive(
		TRUE);
	AUDIOMGR->Play(
		TUTORIALMGR->Get_ShowBtnSndIdx(),
		gHeroID);
}