#include "stdafx.h"
#include ".\itemshopdlg.h"
#include "WindowIDEnum.h"
#include "cBrowser.h"

CItemShopDlg::CItemShopDlg()
{
	m_pBrowser = NULL;
}

CItemShopDlg::~CItemShopDlg()
{
}

void CItemShopDlg::Linking()
{
	m_pBrowser = (cBrowser*)GetWindowForID( ISD_BROWSER );
	SetAlwaysTop( TRUE );
}

void CItemShopDlg::SetActive( BOOL val )
{
	cDialog::SetActiveRecursive( val );
}

void CItemShopDlg::Navigate( char* pURL )
{
	m_pBrowser->Navigate( pURL );
}