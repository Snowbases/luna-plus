#ifndef _CHARACTERMAKE_MANAGER_
#define _CHARACTERMAKE_MANAGER_

#include "MHfile.h"

class cStatic;

enum CHAR_M
{
//	CE_JOB,
	CE_SEX, 
	CE_MHAIR, 
	CE_WMHAIR, 
	CE_MFACE, 
	CE_WMFACE, 
	CE_WEAR, 
	CE_BOOT, 
	CE_WEAPON,
	CE_AREA,
	CE_MAX
};

// 061215 LYW --- Add enum for new make list.
enum CHARMAKE_LIST
{
	CM_MFACE, 
	CM_MHAIR, 
	CM_WMFACE, 
	CM_WMHAIR,
	CM_EMFACE,
	CM_EMHAIR,
	CM_EWFACE,
	CM_EWHAIR,
	// 090604 ONS Ω≈±‘¡æ¡∑ ƒ≥∏Ø≈Õº”º∫ √ﬂ∞°
	CM_DMFACE,
	CM_DMHAIR,
	CM_DWFACE,
	CM_DWHAIR,
	CM_MAX
} ;

enum{CM_PREV, CM_NEXT};

struct CM_OPTION
{
	char		strName[20];
	DWORD		dwData;
	DWORD		dwHelperData;
//	void*		pDescCombo;
	cStatic*	pDescStatic;
	VECTOR2		vTextPos;
};


#define CHARMAKEMGR 	USINGTON(cCharMakeManager)

enum ENUM_CM_CLASS;
class CPlayer;
class cStatic;

// 061215 LYW --- Include interface.
class CCharMakeNewDlg ;

class cCharMakeManager
{
protected:
	CPlayer*				m_pNewPlayer;					// ¬ª√µ¬∑√é¬ø√Æ √Ñ¬≥¬∏¬Ø√Ö√ç
	CHARACTERMAKEINFO		m_CharMakeInfo;
	// 090529 LUJ, ¡˜æ˜ø° µ˚∏• ±‚∫ª æ∆¿Ã≈€
	struct WearedItem
	{
		DWORD mWeaponIndex;
		DWORD mDressIndex;
	}
	mWearedItem;
	cPtrList				m_UserSelectOption[CM_MAX];		// ¬ª√ß¬ø√´?√ö¬∞¬° ¬º¬±√Ö√É¬∞¬°¬¥√â ¬ø√â¬º√á ¬∏√±¬∑√è
	BOOL					m_bInit;
	CCharMakeNewDlg*		m_pCharMakeNewDlg ;
public:

	cCharMakeManager();
	virtual ~cCharMakeManager();

	void Init();	// √É√ä¬±√¢√à¬≠
	void Release();

	void CreateNewCharacter(VECTOR3* pv3Pos);
	
	// 070523 LYW --- CharMakeManager : Modified function ReplaceCharMakeInfo.
	//BOOL ReplaceCharMakeInfo( int idx, WORD wValue );
	void ReplaceCharMakeInfo();
	void ComboBoxResetAll();
	void ListBoxResetAll();

	//SW050812
	// 070523 LYW --- CharMakeManager : Delete function that is not use.
	//void RotateSelection( int idx, int Dir );

	CPlayer* GetNewPlayer() { return m_pNewPlayer; }

	CHARACTERMAKEINFO* GetCharacterMakeInfo() { return &m_CharMakeInfo; }
	DWORD GetWeaponIndex( ENUM_CM_CLASS ) const;
	DWORD GetDressIndex( ENUM_CM_CLASS ) const;
	CCharMakeNewDlg* GetCharMakeNewDlg() { return m_pCharMakeNewDlg ; }

	// 061214 LYW ---- Add function to load option list.
	void LoadOptionList() ;
	// 061214 LYW ---- End add.

	// 061215 LYW --- Add fucntion to setting selection value.
	void SetOptionList( int curStatic, int curIdx ) ;

	// desc_hseos_º∫∫∞º±≈√01
	// S º∫∫∞º±≈√ √ﬂ∞° added by hseos 2007.06.16
	void SetCharSexKindFromDB(BYTE nSexKind)	{ m_CharMakeInfo.SexType = nSexKind; }
	// E º∫∫∞º±≈√ √ﬂ∞° added by hseos 2007.06.16

	cPtrList* GetOptionList(int idx) ;

};
EXTERNGLOBALTON(cCharMakeManager)
#endif // _CHARACTERMAKE_MANAGER_
