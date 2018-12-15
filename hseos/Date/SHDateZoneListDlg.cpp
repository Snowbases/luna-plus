#include "stdafx.h"
#include "SHDateZoneListDlg.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "cScriptManager.h"
#include "cResourceManager.h"
#include "cRITEMEx.h"
#include "../hseos/Date/SHDateManager.h"

CSHDateZoneListDlg::CSHDateZoneListDlg()
{
	m_nSelDateZone = 0;
}

CSHDateZoneListDlg::~CSHDateZoneListDlg()
{
	m_pcsDateZoneList->DeleteAllItems() ;
}

void CSHDateZoneListDlg::SetActive( BOOL val )
{
	cDialog::SetActive( val );
}

void CSHDateZoneListDlg::Linking()
{
	m_pcsDateZoneList = ( cListCtrl* )GetWindowForID( DMD_DATEZONE_MOVELIST_LISTCTRL ) ;

	for(WORD i=0; i<m_pcsDateZoneList->GetRowsNum(); ++i)
	{
		cRITEMEx *ritem = new cRITEMEx(1);

		strcpy(ritem->pString[0], RESRCMGR->GetMsg(m_pcsDateZoneList->GetRowsMsgID(i)));
		if (i == 0) ritem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
		else		ritem->rgb[0] = RGBA_MAKE(10, 10, 10, 255);

		m_pcsDateZoneList->InsertItem(i, ritem);
	}
} 

void CSHDateZoneListDlg::Render()
{
 	if( !IsActive() ) return;

	cDialog::Render();
}

void CSHDateZoneListDlg::OnActionEvent( LONG lId, void* p, DWORD we )																	
{
 	if( we & WE_BTNCLICK )																											
	{
 		switch( lId )																												
		{
		case DMD_DATEZONE_MOVELIST_OK:
			g_csDateManager.CLI_RequestDateMatchingEnterDateZone(m_nSelDateZone);
			SetActive(FALSE);
			break;
		case DMD_DATEZONE_MOVELIST_CANCEL:
			SetActive(FALSE);
			break;
		}
	}
}

DWORD CSHDateZoneListDlg::ActionEvent(CMouse* mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	int nRowIndex = m_pcsDateZoneList->GetSelectedRowIdx();
	if(we & WE_ROWCLICK)
	{
		SelectDateZone(nRowIndex);
	}
	else if( we & WE_ROWDBLCLICK )
	{
		g_csDateManager.CLI_RequestDateMatchingEnterDateZone(nRowIndex);
		SetActive(FALSE);
	}

	return we;
}

void CSHDateZoneListDlg::SelectDateZone(int nRowIndex)
{
	for(WORD i=0; i<m_pcsDateZoneList->GetItemCount(); ++i)
	{
		cRITEMEx* pRItem = (cRITEMEx*)m_pcsDateZoneList->GetRItem(i);
		if (i == nRowIndex)
		{
			pRItem->rgb[0] = RGBA_MAKE(255, 255, 255, 255);
			m_nSelDateZone = nRowIndex;
		}
		else
		{
			pRItem->rgb[0] = RGBA_MAKE(10, 10, 10, 255);
		}
	}
}
