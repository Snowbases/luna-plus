#include "stdafx.h"
#include "WindowIDEnum.h"

#include "KeySettingTipDlg.h"

// 070202 LYW --- Include header file.
#include "../[CC]Header/GameResourceManager.h"
#include "../Interface/cScriptManager.h"

// 071022 LYW --- KeySettingTipDlg : Include header file of tutorial manager.
#include "TutorialManager.h"

CKeySettingTipDlg::CKeySettingTipDlg(void)
{
	m_wMode = 2;

	m_scale.x = 1.0f ;
	m_scale.y = 1.0f ;
}

CKeySettingTipDlg::~CKeySettingTipDlg(void)
{
}

void CKeySettingTipDlg::Linking()
{
	SCRIPTMGR->GetImage( 96, &m_KeyImage );
	SCRIPTMGR->GetImage( 97, &m_KeyImage2 );

	m_pCloseButton = (cButton*)GetWindowForID(KS_CLOSEBTN) ;
	m_pCloseButton->SetActive(TRUE) ;
	m_pCloseButton->SetDepend(TRUE) ;
}

void CKeySettingTipDlg::Render()
{
	if( !m_bActive ) return;
	if(m_wMode > 1) return;

	cDialog::RenderWindow();

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();

	switch( dispInfo.dwWidth )
	{
	case 800 :	m_pRenderImage = &m_KeyImage2 ; break ;
	default :	m_pRenderImage = &m_KeyImage ;	break ;
	}

	if( m_pRenderImage )
	{
		m_pRenderImage->RenderSprite(&m_scale, NULL, 0, &m_absPos, RGBA_MAKE(255,255,255,255) );
	}

	cDialog::RenderComponent();
}

void CKeySettingTipDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		if(lId == KS_CLOSEBTN)
		{
			SetActive(FALSE) ;
		}
	}
}

// 071022 LYW --- KeySettingTipDlg : Add function to setting active or deactive dialog.
void CKeySettingTipDlg::SetActive(BOOL val)
{
	cDialog::SetActive( val );

	// 071022 LYW --- KeySettingTipDlg : Check open dialog.
	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;
}