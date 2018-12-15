#include "stdafx.h"
#include ".\storesearchdlg.h"
#include "WindowIDEnum.h"

#include "./Interface/cEditBox.h"
#include "StreetStallManager.h"

CStoreSearchDlg::CStoreSearchDlg(void)
{
	m_pEBSearch = NULL;
}

CStoreSearchDlg::~CStoreSearchDlg(void)
{
}

void CStoreSearchDlg::Linking()
{
	m_pEBSearch	= (cEditBox*)GetWindowForID( SSD_EDITBOX );
}

void CStoreSearchDlg::SetActive( BOOL val )
{
	if( val == TRUE )
	{
		m_pEBSearch->SetEditText( "" );
		m_pEBSearch->SetFocusEdit( TRUE );
	}
	else
	{
		m_pEBSearch->SetFocusEdit( FALSE );
	}

	cDialog::SetActive( val );
}

void CStoreSearchDlg::OnActionEvent(LONG lId, void * p, DWORD we)
{
	if( we & WE_BTNCLICK )
	{
		if( lId == SSD_BTN_OK )
		{
			STREETSTALLMGR->SetSearchWord( m_pEBSearch->GetEditText() );
			STREETSTALLMGR->SearchWordInArea();

			SetActive( FALSE );
		}
		else if( lId == SSD_BTN_CANCEL )
		{
			SetActive( FALSE );
		}
	}
	else if( we & WE_RETURN )
	{
		if( m_pEBSearch->IsFocus() )
		{
			STREETSTALLMGR->SetSearchWord( m_pEBSearch->GetEditText() );
			STREETSTALLMGR->SearchWordInArea();

			SetActive( FALSE );
		}
	}
}