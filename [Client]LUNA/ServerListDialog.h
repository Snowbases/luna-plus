// ServerListDialog.h: interface for the CServerListDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERLISTDIALOG_H__2827868C_735C_4097_85A8_F9A0693D724B__INCLUDED_)
#define AFX_SERVERLISTDIALOG_H__2827868C_735C_4097_85A8_F9A0693D724B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./interface/cDialog.h"
#include ".\interface\cWindowHeader.h"
#include "cRITEMEx.h"

class cLsitCtrl;

// 061209 LYW --- Define limited count for server list.
#define MAX_COUNT_SERVERLIST	8

class CServerListDialog : public cDialog  
{
	// 061209 LYW --- Delete server list control in CServerListDialog.
	//cListCtrl*	m_pServerListCtrl;
	// 061209 LYW --- Added list dialog control.
	cListDialog*	m_pServerListDlg ;
	int			m_nMaxServerNum;
	int			m_nIndex;
	cButton*	m_pConnectBtn;
	cButton*	m_pExitBtn;

	// 061209 LYW --- Add text color.
	DWORD		m_dwColor ;

	// 061227 LYW --- Delete this code.
	// 061227 LYW --- Add static control for use background image of scroll bar.
	cStatic*	m_pScrollBack ;

public:
	CServerListDialog();
	virtual ~CServerListDialog();

	void	Linking();
	void	LoadServerList();
	virtual	DWORD ActionEvent( CMouse* mouseInfo );

	int		GetSelectedIndex()		{ return m_nIndex; }

};

#endif // !defined(AFX_SERVERLISTDIALOG_H__2827868C_735C_4097_85A8_F9A0693D724B__INCLUDED_)
