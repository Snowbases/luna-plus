//=================================================================================================
//	FILE		: MainSystemDlg.h
//	PURPOSE		: Header file for main system dialog.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 19, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================
#pragma once
#ifndef __MAINSYSTEMDLG_h__
#define __MAINSYSTEMDLG_h__


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "./interface/cTabDialog.h"


//=================================================================================================
//	DEFINE PART
//=================================================================================================
// 070115 LYW --- Define max count of button controls for systemset part.
#define MAX_SYSTEMSET_BTN	5

// 070115 LYW --- Define max count of button controls for gamesystem part.
#define MAX_GAMESYSTEM_BTN	10

// 090420 ONS 캐릭터정보 메뉴에 "전직가이드" 추가
// 091211 ONS 캐릭터정보 메뉴에 "미니홈피" 추가
#define MAX_CHARINFO_BTN	3

// 100106 ONS 웹정보/인벤토리 메뉴 추가
#define MAX_WEBINFO_BTN		4
#define MAX_INVENTORY_BTN	2
enum eMainMenuKind
{
	eMainMenu_None = 0,
	eMainMenu_WebInfo,
	eMainMenu_CharInfo,
	eMainMenu_Inventory,
	eMainMenu_GameSystem,
	eMainMenu_GameOption
};

// 100106 ONS 녹화버튼 상태 속성
enum eRecordBtnState
{
	eRecordBtnState_Start = 1,
	eRecordBtnState_Stop	
};

//=================================================================================================
//	THE OTHERS PART
//=================================================================================================
class cWindow ;

// 070115 LYW --- Call interface to need.
class cButton ;
class cStatic ;

//=================================================================================================
// NAME			: CMainSystemDlg Class.
// PURPOSE		: The class for main system dialog.
// ATTENTION	:
//=================================================================================================
// 070115 LYW --- Modified this line.
//class CMainSystemDlg : public cTabDialog
class CMainSystemDlg : public cDialog
{
protected :
	// 070115 LYW --- Add static controls for background image of sub dialog.
	cStatic*	m_pSystemSet ;
	cStatic*	m_pSystemSet_Bottom ;
	cStatic*	m_pGameSystem ;

	// 070115 LYW --- Add controls for systemset part.
	cButton*	m_pMenuSystemSet[ MAX_SYSTEMSET_BTN ] ;
	cStatic*	m_pTitleSystemSet ;
	cStatic*	m_pMarkSystemSet ;
	
	// 070115 LYW --- Add controls for gamesystem part.
	cButton*	m_pMenuGameSystem[ MAX_GAMESYSTEM_BTN ] ;
	cStatic*	m_pTitleGameSystem ;
	cStatic*	m_pMarkGameSystem ;

	// 070115 LYW --- Add variables whether show sub dialog or not.
	BOOL		m_bShowSystemSet ;
	BOOL		m_bShowGameSystem ;

	// 090420 ONS 캐릭터정보 메뉴에 "전직가이드" 추가
	cStatic*	m_pCharInfo ;
	cButton*	m_pMenuCharInfo[ MAX_CHARINFO_BTN ] ;
	cStatic*	m_pTitleCharInfo ;
	cStatic*	m_pMarkCharInfo ;
	BOOL		m_bShowCharInfo ;

	// 100106 ONS 웹정보/인벤토리 메뉴 추가
	cButton*	m_pMenuWebInfo[ MAX_WEBINFO_BTN ];
	cStatic*	m_pTitleWebInfo;
	cStatic*	m_pMarkWebInfo;
	cStatic*	m_pBGWebInfoTop;
	cStatic*	m_pBGWebInfoBottom;
	BOOL		m_bShowWebInfo;

	cButton*	m_pMenuInventory[ MAX_INVENTORY_BTN ];
	cStatic*	m_pTitleInventory;
	cStatic*	m_pMarkInventory;
	cStatic*	m_pBGInventoryTop;
	cStatic*	m_pBGInventoryBottom;
	BOOL		m_bShowInventory;
	
	eMainMenuKind	m_eMainMenuKind;

public:
	CMainSystemDlg(void);
	virtual ~CMainSystemDlg(void);

	// 070115 LYW --- Delete this functions.
	/*
	void Render() ;
	void Add( cWindow* window ) ;
	*/

	// 070115 LYW --- Add function ActionEvent.
	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;

	/// The function for processing event from controls of this dialog.
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;

	// 070115 LYW --- Add function to linking controls to window manager.
	void Linking() ;

	// 070115 LYW --- Add function to setting systemset part.
	void SetSystemSet() ;
	
	// 070115 LYW --- Add function to setting gamesystem part.
	void SetGameSystem() ;

	// 090420 ONS 캐릭터정보 메뉴에 "전직가이드" 추가
	void SetCharInfo();

	// 100106 ONS 웹정보/ 인벤토리 메뉴 추가
	void SetWebInfo();
	void SetInventory();
	void CloseSubMenu();
};
#endif //__MAINSYSTEMDLG_h__



















