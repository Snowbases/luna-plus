// GuageDialog.h: interface for the CGuageDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUAGEDIALOG_H__78CD1521_0A6D_4ABB_A485_8E21C8B23184__INCLUDED_)
#define AFX_GUAGEDIALOG_H__78CD1521_0A6D_4ABB_A485_8E21C8B23184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

#define FLICKER_TIME	100

// 070110 LYW --- Define count for controls of character dialog.
#define MAX_POPUP1_STATIC	3
#define MAX_POPUP2_STATIC	4
#define MAX_POPUP1_BUTTON	3
#define MAX_POPUP2_BUTTON	4

// 070117 LYW --- Define count for character face.
// 090504 ONS 신규종족 이미지 추가
#define MAX_HERO_FACE		6

class cButton;
class cStatic;

class CGuageDialog : public cDialog
{
	cStatic* m_pFlicker01;
	cStatic* m_pFlicker02;

	BOOL	m_bFlicker;
	BOOL	m_bFlActive;
	DWORD	m_dwFlickerSwapTime;
	// 070110 LYW --- Add controls for popup menu.
	cButton*	m_pMenuButton ;
	cStatic*	m_pBGImage1[ MAX_POPUP1_STATIC ] ;
	cStatic*	m_pBGImage2[ MAX_POPUP2_STATIC ] ;

	cButton*	m_pPopupBtn1[ MAX_POPUP1_BUTTON ] ;
	cButton*	m_pPopupBtn2[ MAX_POPUP2_BUTTON ] ;

	// 070117 LYW --- Add static controls for charater image.
	cStatic*	m_pHeroFace[ MAX_HERO_FACE ] ;

	BOOL		m_bShowPopupMenu1 ; 
	BOOL		m_bShowPopupMenu2 ; 

	BYTE		m_byHeroState ;
	// 091127 LUJ, 파티 마스터 로고
	cImage		m_MasterMark;

public:
	CGuageDialog();
	virtual ~CGuageDialog();

	void Linking();
	// 070110 LYW --- Delete this function.
	//void OnActionEvent(LONG lId, void * p, DWORD we);
	virtual void Render();
	virtual DWORD ActionEvent(CMouse*);
	void SetPopupMenu() ;
	void SetSecondPopup() ;
	void SecessionPartyCaptain() ;

	BYTE GetHeroState() { return m_byHeroState ; }

	// 070117 LYW --- Add function for character face.
	void MakeHeroFace() ;

	virtual void SetActive(BOOL val);
};

#endif // !defined(AFX_GUAGEDIALOG_H__78CD1521_0A6D_4ABB_A485_8E21C8B23184__INCLUDED_)
