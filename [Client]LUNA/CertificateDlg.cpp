//=================================================================================================
//	FILE		: CertificateDlg.cpp
//	PURPOSE		: Implementation part for certificate some condition.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 18, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "stdafx.h"
#include ".\certificatedlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "ChatManager.h"
#include "CharSelect.h"
#include "cMsgBox.h"

// 061227 LYW --- Include profile's header file.
#include "ProfileDlg.h"

// 070122 LYW --- Include header file.
#include "cResourceManager.h"

#include "./interface/cWindowManager.h"
#include "./interface/cButton.h"
#include "./interface/cStatic.h"
#include "./interface/cEditBox.h"
#include "./interface/cTextArea.h"


//=================================================================================================
// NAME			: CCertificateDlg()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CCertificateDlg::CCertificateDlg(void)
{
	m_pBtnOk		= NULL ;
	m_pBtnCancel	= NULL ;
	m_pRanNum		= NULL ;
	m_pEditBox		= NULL ;
	m_pText			= NULL ;

	m_nRanNum = 0 ;
}


//=================================================================================================
// NAME			: ~CCertificateDlg()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CCertificateDlg::~CCertificateDlg(void)
{
}


//=================================================================================================
// NAME			: ~Linking()
// PURPOSE		: The function for linking components to window manager.
// ATTENTION	:
//=================================================================================================
void CCertificateDlg::Linking()
{
	m_pBtnOk		= ( cButton* )GetWindowForID(CTF_BTN_OK) ;
	m_pBtnCancel	= ( cButton* )WINDOWMGR->GetWindowForIDEx(CTF_BTN_CANCEL) ;

	m_pRanNum		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(CTF_ST_RANNUM) ;
	m_pRanNum->SetStaticText("") ;

	m_pEditBox		= ( cEditBox* )WINDOWMGR->GetWindowForIDEx(CTF_EB_EDITBOX) ;
	m_pEditBox->SetValidCheck( VCM_CHARNAME ) ;
	m_pEditBox->SetEditText("") ;
	m_pEditBox->SetTextOffset(5,5,3) ;

	m_pText			= ( cTextArea* )WINDOWMGR->GetWindowForIDEx(CTF_TA_TEXT) ;
	// 070118 LYW --- Delete this line.
	//m_pText->SetScriptText("정말 캐릭터를 삭제 하시려면 화면에 보이는 숫자를 입력하십시오.") ;

	
	// 070117 LYW --- Add static control for title.
	// 070118 LYW --- Delete this line.
	//m_pTitle		= ( cStatic* )WINDOWMGR->GetWindowForIDEx(CTF_ST_TEXT) ;
	//m_pTitle->SetStaticText("◈알림") ;
}


//=================================================================================================
// NAME			: OnActionEvent()
// PURPOSE		: The function for process event.
// ATTENTION	:
//=================================================================================================
void CCertificateDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
		case CTF_BTN_OK :
			{
				if( strcmp(m_pRanNum->GetStaticText(), m_pEditBox->GetEditText() ) == 0 )
				{
					CHARSELECT->DeleteCharacter();
					m_pEditBox->SetEditText("") ;
					m_pRanNum->SetStaticText("") ;
					SetActive( FALSE ) ;
				}
				else
				{
					m_pEditBox->SetEditText("") ;
					m_pRanNum->SetStaticText("") ;
					SetActive( FALSE ) ;

					WINDOWMGR->MsgBox( MBI_CANCEL, MBT_OK, RESRCMGR->GetMsg(289) );
				}
			}
			break ;

		case CTF_BTN_CANCEL :
			{
				m_pEditBox->SetEditText("") ;
				m_pRanNum->SetStaticText("") ;
				SetActive( FALSE ) ;
				CHARSELECT->SetDisablePick( FALSE );
			}
			break ;
		}
	}
}


//=================================================================================================
// NAME			: OnActionEvent()
// PURPOSE		: Make random number for certificate some condition.
// ATTENTION	:
//=================================================================================================
void CCertificateDlg::MakeRandomNumber()
{
	char szStr[8] ;
	m_nRanNum = random(0, 99) ;

	if( m_pRanNum )
	{
		m_pRanNum->SetStaticText("") ;
		sprintf(szStr, "%d", m_nRanNum ) ;
		m_pRanNum->SetStaticText(szStr) ;
	}
}
