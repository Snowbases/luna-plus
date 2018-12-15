//=================================================================================================
//	FILE		: CCharMakeNewDlg.h
//	PURPOSE		: For new dialog for create character.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 14, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================
#ifndef __CCHARMAKENEWDLG_H__
#define __CCHARMAKENEWDLG_H__


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include ".\interface\cdialog.h"


//=================================================================================================
//	DEFINE PART
//=================================================================================================
// 090423 ONS 신규종족 버튼 추가에 따른 버튼 카운트 변경(7->8)
#define MAX_PUSHUPBTN_COUNT	8
#define MAX_STATIC_COUNT	12
#define MAX_CMBTN_COUNT		4
#define MAX_TEXAREA_COUNT	2
// 070122 LYW --- Modified this part.

#define MAX_MFACE_TYPE		10
#define MAX_MHAIR_TYPE		10
#define MAX_WMFACE_TYPE		10
#define MAX_WMHAIR_TYPE		10


/*
#define MAX_MFACE_TYPE		3
#define MAX_MHAIR_TYPE		1
#define MAX_WMFACE_TYPE		3
#define MAX_WMHAIR_TYPE		1
*/

//=================================================================================================
//	THE OTHERS PART
//=================================================================================================
class cPushupButton ;
class cStatic ;
class cButton ;
class cTextArea ;

// 061215 LYW --- Define for use index for buttons.
enum ENUM_CM_PUSHUPBTN
{
	e_PB_HUMAN = 0,
	e_PB_ELF, 
	// 090423 ONS 신규종족 버튼 추가
	e_PB_DEVIL, 
	e_PB_FIGHTER, 
	e_PB_ROGUE, 
	e_PB_MAGE, 
	e_PB_MAN, 
	e_PB_WOMAN,
} ;

enum ENUM_CM_SPINBTN
{
	e_FACE_LEFT	= 0,
	e_FACE_RIGHT,
	e_HAIR_LEFT, 
	e_HAIR_RIGHT,
} ;

enum ENUM_CM_RACIAL
{
	e_HUMAN = 0, 
	e_ELF,
	// 090423 ONS 신규종족 관련 추가
	e_DEVIL, 
} ;

enum ENUM_CM_CLASS
{
	// 061227 LYW --- Change this code.
	/*
	e_FIGHTER = 0,
	e_MAGE, 
	e_ROGUE,
	*/
	e_FIGHTER = 0,
	e_ROGUE,
	e_MAGE, 
} ;

enum ENUM_CM_GENDER
{
	e_MAN = 0,
	e_WOMAN,
} ;

enum ENUM_CM_APPEARANCE
{
	e_MFACE = 0,
	e_MHAIR,
	e_WMFACE, 
	e_WMHAIR,
	e_EMFACE, 
	e_EMHAIR, 
	e_EWMFACE,
	e_EWMHAIR,
	// 090423 ONS 신규종족 관련 추가
	e_DMFACE, 
	e_DMHAIR, 
	e_DWMFACE,
	e_DWMHAIR,
} ;

enum ENUM_CM_TEXTAREA
{
	e_RACIAL = 0,
	e_CLASS,
} ;

// 070522 LYW --- CharMakeNewDlg : Add enum for replace part.
enum ENUM_CM_REPLACECHARINFO
{
	e_REPLACE_RACE = 0,
	e_REPLACE_GENDER,
	e_REPLACE_HAIR, 
	e_REPLACE_FACE,
} ;

enum ENUM_CM_SELECT_OPTION
{
	e_OPTION_HAIR = 0 ,
	e_OPTION_FACE,
} ;


//=================================================================================================
// NAME			: CCharMakeNewDlg Class.
// PURPOSE		: For new dialog for create character.
// ATTENTION	:
//=================================================================================================
class CCharMakeNewDlg : public cDialog
{
public :
	cPushupButton*	m_pPushupBtn[ MAX_PUSHUPBTN_COUNT ] ;
	cStatic*		m_pStatic[ MAX_STATIC_COUNT ] ;
	cButton*		m_pButton[ MAX_CMBTN_COUNT ] ;
	cTextArea*		m_pTextArea[ MAX_TEXAREA_COUNT ] ;

	cButton*		m_pBtnMake ;
	cButton*		m_pBtnCancel ;

	BYTE			m_byCurRacial ;
	BYTE			m_byCurClass ;
	BYTE			m_byCurGender ;
	BYTE			m_byCurFace ;
	BYTE			m_byCurHair ;

	// 061218 LYW --- Add Alpha static window.
	cStatic*		m_pAlphaWindow ;

	// 061227 LYW --- Add stitc for racial and class.
	cStatic*		m_pST_Racial ;
	cStatic*		m_pST_Class ;

public:
	CCharMakeNewDlg(void);
	virtual ~CCharMakeNewDlg(void);

	void Linking() ;

	// 061215 LYW --- Return pointer that is member variable in this class.
	// num ==> 0 = return mface, 1 = return mhair, 2 = return wmface, 3 = return wmhair
	// 070525 LYW --- CharMakeNewDlg : Modified function GetStatic.
	//cStatic* GetStatic( LONG num ) { return m_pStatic[ num ] ; }
	cStatic* GetStatic( LONG num ) ;

	// 061215 LYW --- Refresh pushup buttons.
	void RefreshPushUpBtn() ;

	// 061215 LYW --- Apply option selected by user.
	void ApplyOption(CHARACTERMAKEINFO* makeInfo) ;

	// 061216 LYW --- Add function to process events.
	void OnActionEvent( LONG lId, void* p, DWORD we ) ;

	// 061218 LYW --- Setting and return m_byCurRacial variable.
	void SetCurRacial( BYTE curRacial ) { m_byCurRacial = curRacial ; }
	BYTE* GetCurRacial() { return &m_byCurRacial ; }

	cStatic* GetAlphaWindow() { return m_pAlphaWindow ; }

	cPtrList* GetCurOptionList(int nItem) ;

	// 070522 LYW --- Add function to change face type of character.
	void ChangeFace( LONG lID ) ;

	// 070522 LYW --- Add function to change hair stype of character.
	void ChangeHair( LONG lID ) ;

	// 070522 LYW --- Add function to change race of character.
	void ChangeRace( LONG lID, BYTE race ) ;

	// 070522 LYW --- Add function to change gender of character.
	void ChangeGender( LONG lID, BYTE gender ) ;

	// 070522 LYW --- Add function to change job of character.
	void ChangeJob( LONG lID, BYTE job ) ;

	// 070522 LYW --- Add function to change text part.
	void ChangeText() ;

	// 070619 LYW --- CharMakeNewDlg : Add function to return pushup button.
	cPushupButton* GetPushUpButton(int nIdx) { if(nIdx >= MAX_PUSHUPBTN_COUNT) return NULL ; return m_pPushupBtn[nIdx] ; }

// 090518 ONS 신규종족 생성관련 함수선언
private:
	void DisableDevilRace( BOOL bDisable );
	void DisableJob( BOOL bDisable );
	void ChangeDevilButton( BOOL bFlag );

	BOOL m_bMakeDevil;
};


#endif // __CCHARMAKENEWDLG_H__