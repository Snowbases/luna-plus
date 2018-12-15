// TSToolManager.cpp - iros
//  : interface for the CGMToolManager class. 
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_TSTOOLMANAGER_H__24066759_94EB_4F2E_AF66_D435A58252D0__INCLUDED_
#define AFX_TSTOOLMANAGER_H__24066759_94EB_4F2E_AF66_D435A58252D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _TESTCLIENT_

#define TSTOOLMGR USINGTON(CTSToolManager)

enum eTS_MENU
{
	eTSMenu_Char,
	eTSMenu_Item,
	eTSMenu_Action,
	eTSMenu_Effect,
	eTSMenu_Mob,
	eTSMenu_Move,
	eTSMenu_Quest,
    eTSMenu_Weather,
	eTSMenu_Count,
};

class CTSToolManager
{
protected:
	HWND	m_hWndDlg;
	HWND	m_hWndOutDlg;

	HWND	m_hWndSub[eTSMenu_Count];

	int		m_nSelectMenu;
	BOOL	m_bShow;

	BOOL	m_bSubShow;
	BYTE	m_cbChannelCount;

	//for BossMob
	LONG	m_lBossStartIndex;

	std::string m_outString;
public:
	CTSToolManager();
	virtual ~CTSToolManager();

	BOOL CreateTSDialog();
	BOOL DestroyTSDialog();
	BOOL CreateTSSubDialog();
	BOOL DestroyTSSubDialog();

	void ShowTSDialog( BOOL bShow, BOOL bUpdate = FALSE );
	BOOL IsShowing()	{ return m_bShow; }
	BOOL IsSubShow()	{ return m_bSubShow; }

	BOOL IsTSDialogMessage( LPMSG pMessage );

	void SetPositionByMainWindow();
	void SetPositionByOutputWindow();
	void SetPositionSubDlg();

	void OnClickMenu( int nMenu );

	// Command Func.
	BOOL OnCharCommand();
	BOOL OnItemCommand();
	BOOL OnRegenCommand();
	BOOL OnMoveCommand( int nMethod );
	BOOL OnQuestCommand();

	void ItemDlgCtrlDisableAll();

	void SetOutString(const char* str);
	void UpdateOutString();
	void AddWeatherType(LPCTSTR);
};
inline void CTSToolManager::SetOutString(const char* str)
{
	m_outString = str;
	UpdateOutString();
}
inline void CTSToolManager::UpdateOutString()
{
	SetWindowText( GetDlgItem( m_hWndOutDlg, IDC_TSOUTPUT_EDIT), m_outString.c_str());
}

INT_PTR CALLBACK TSDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TSOutDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

INT_PTR CALLBACK TSSubCharDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK TSSubItemDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK TSSubRegenDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK TSSubMoveDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK TSSubQuestDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK TSSubWeatherDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

EXTERNGLOBALTON(CTSToolManager)

#endif //_TESTCLIENT_

#endif // !defined(AFX_TSTOOLMANAGER_H__24066759_94EB_4F2E_AF66_D435A58252D0__INCLUDED_)