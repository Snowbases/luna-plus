// ServerListDialog.cpp: implementation of the CServerListDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerListDialog.h"
#include "WindowIDEnum.h"
#include "GameResourceManager.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cButton.h"
#include "./Interface/cStatic.h"
#include "./Interface/cListCtrl.h"
#include "MainTitle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerListDialog::CServerListDialog()
{
	// 061209 LYW --- Delete server list control in CServerListDialog.
	//m_pServerListCtrl = NULL;
	// 061209 LYW --- Added list dialog control.
	m_pServerListDlg	= NULL ;
	m_nMaxServerNum = 0;
	m_nIndex = -1;

	// 061209 LYW --- Add text color.
	m_dwColor = RGB(0, 0, 0) ;

	// 061227 LYW --- Add static control for use background image of scroll bar.
	m_pScrollBack = NULL ;
}

CServerListDialog::~CServerListDialog()
{
}

void CServerListDialog::Linking()
{
	// 061209 LYW --- Delete server list control in CServerListDialog.
	//m_pServerListCtrl = (cListCtrl*)GetWindowForID(SL_LISTCTRL);
	// 061209 LYW --- Added list dialog control.
	m_pServerListDlg	= ( cListDialog* )GetWindowForID( SL_LISTCTRL ) ;
	m_pServerListDlg->SetShowSelect( TRUE ) ;
	m_pServerListDlg->SetStyle( LDS_SERVERLIST ) ;
	m_pServerListDlg->SetAutoScroll( FALSE ) ;
	//m_pServerListDlg->SetAlign( TXT_CENTER ) ;		// text center
	m_pConnectBtn = (cButton*)GetWindowForID(SL_BTN_CONNECT);
	m_pExitBtn = (cButton*)GetWindowForID(SL_BTN_EXIT);

	// 061227 LYW --- Add static control for use background image of scroll bar.
	m_pScrollBack = ( cStatic* )GetWindowForID( SL_ST_SCROLLBGI ) ;
	m_pScrollBack->SetActive( FALSE ) ;

	LoadServerList();
}

void CServerListDialog::LoadServerList()
{
	SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
	m_nMaxServerNum = GAMERESRCMNGR->m_nMaxServerList;

	for( int i = 0; i < m_nMaxServerNum; ++i )
	{
		if( !pServerList[i].bEnter )
		{
			m_dwColor = RGB( 255, 0, 0 ) ;
		}
		else
		{
			m_dwColor = RGB( 255, 255, 255 ) ;
		}

		m_pServerListDlg->AddItem( pServerList[i].ServerName, m_dwColor) ;
	}

	if( m_nMaxServerNum > MAX_COUNT_SERVERLIST )
	{
		m_pServerListDlg->SetShowScroll( TRUE ) ;		
		m_pServerListDlg->SetTopListItemIdx(0) ;
		// 061227 LYW --- Activate static.
		m_pScrollBack->SetActive( TRUE ) ;
	}
	else
	{
		m_pServerListDlg->SetShowScroll( FALSE ) ;
		m_pServerListDlg->SetDisableScroll() ;
		// 061227 LYW --- Deactivate static.
		m_pScrollBack->SetActive( FALSE ) ;
	}

	// 070620 LYW --- ServerListDialog ; Setting Cur Column.
	m_pServerListDlg->SetCurSelectedRowIdx(0) ;
	m_nIndex = 0;
}

DWORD CServerListDialog::ActionEvent( CMouse * mouseInfo )
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);

	if( we & WE_LBTNCLICK )
	{
		SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
		int i = m_pServerListDlg->GetCurSelectedRowIdx() ;
		if( i < m_nMaxServerNum && i != -1 )
		{
			if( m_nIndex != i )
			{
				m_dwColor = RGBA_MAKE(255,234,0,255);
				if( !pServerList[i].bEnter )
				{
					m_dwColor = RGBA_MAKE(255,0,0,255);
				}	
				if( m_nIndex > -1 )
				{
					m_dwColor = 0xffffffff;
					if( !pServerList[m_nIndex].bEnter )
					{
						m_dwColor = RGBA_MAKE(255,0,0,255);
					}
				}					
			}
			m_nIndex = i;
		}
	}
	else if( we & WE_LBTNDBLCLICK )
	{
		int i = m_pServerListDlg->GetCurSelectedRowIdx() ;
		if( i < m_nMaxServerNum )
		{
			TITLE->ConnectToServer( i );
		}		
	}
	return we;
}
