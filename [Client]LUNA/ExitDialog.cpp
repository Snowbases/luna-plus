#include "stdafx.h"
#include "ExitDialog.h"
#include "GameIn.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"

CExitDialog::CExitDialog()
{
}
void CExitDialog::SetActive( BOOL val )
{
	cDialog::SetActive( val );

//--- main bar icon
	// 061220 LYW --- Delete this code.
	/*
	CMainBarDialog* pDlg = GAMEIN->GetMainInterfaceDialog();
	if( pDlg )
		pDlg->SetPushBarIcon( OPT_EXITDLGICON, m_bActive );	
		*/
}
// 070102 LYW --- Add destructor function.
CExitDialog::~CExitDialog()
{
}