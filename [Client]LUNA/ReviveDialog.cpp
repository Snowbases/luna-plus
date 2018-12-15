#include "stdafx.h"
#include "ReviveDialog.h"
#include "WindowIDEnum.h"
#include "MHMap.h"
#include "interface/cButton.h"
#include "Interface/cTextArea.h"


CReviveDialog::CReviveDialog()
{

}

CReviveDialog::~CReviveDialog()
{

}

void CReviveDialog::Linking()
{
	m_pPresentBtn = (cButton*)GetWindowForID(CR_PRESENTSPOT);
	m_pLoginBtn = (cButton*)GetWindowForID(CR_LOGINSPOT);
	// 070112 LYW --- Deactive login button.
	m_pLoginBtn->SetActive( FALSE ) ;
	m_pVillageBtn = (cButton*)GetWindowForID(CR_TOWNSPOT);

	// 070112 LYW --- Add textarea control.
	m_pTextArea = ( cTextArea* )GetWindowForID( CR_TEXTAREA ) ;
}

void CReviveDialog::SetActive( BOOL val )
{
	cDialog::SetActive( val );

	// 070122 LYW --- Modified thsi part.
	/*
	if( SIEGEMGR->GetSiegeWarMapNum() && MAP->GetMapNum() == SIEGEMGR->GetSiegeWarMapNum() )
	{
		m_pPresentBtn->SetActive( FALSE );
		m_pVillageBtn->SetActive( TRUE );
	}
	else
	{
		m_pPresentBtn->SetActive( TRUE );
		m_pVillageBtn->SetActive( FALSE );
	}
	*/

	m_pPresentBtn->SetActive( TRUE );
	m_pVillageBtn->SetActive( TRUE );
}
