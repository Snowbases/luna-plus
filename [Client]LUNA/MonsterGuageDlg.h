// MonsterGuageDlg.h: interface for the CMonsterGuageDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTERGUAGEDLG_H__3B4654AD_257E_42F7_9996_C2C8E89E535E__INCLUDED_)
#define AFX_MONSTERGUAGEDLG_H__3B4654AD_257E_42F7_9996_C2C8E89E535E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cDialog.h"

class cStatic;
class cPushupButton;
class CObjectGuagen;
class CMonster;
class CPlayer;
class CStatusIconDlg;
class cCheckBox;
class cMonsterTargetDlg;
class cCharacterTargetDlg;
enum EObjectKind;

// 070109 LYW --- Add enum for popup menu type.
enum PopupMenu_Style
{
	e_STANDARD = 0x00, 
	e_INVITE, 
	e_EXPULSION,
} ;

enum PlayerState
{
	e_NONE = 0x00, 
	e_CAPTAIN, 
	e_MEMBER,
} ;

enum TargetDlgType
{
	NPC_TYPE	=	0,
	MONSTER_TYPE,
	BOSSMONSTER_TYPE,
	PC_TYPE,
	NONE_TYPE,
};

class CMonsterGuageDlg : public cDialog  
{
public:
	CMonsterGuageDlg();
	virtual ~CMonsterGuageDlg();
	void SetMonsterLife(DWORD curVal, DWORD maxVal);
	void Linking();
	virtual void SetActive(BOOL);
	virtual void Render();
	cMonsterTargetDlg* GetCurTarget() const { return m_pCurTarget; }
	
	EObjectKind GetObjectType() const { return m_Type; }
	virtual DWORD ActionEvent(CMouse*);

protected:
	EObjectKind	m_Type;
	cStatic* m_pHPMark;
	CObjectGuagen*	m_pMonsterGuage ;
	TargetDlgType	m_TargetDlgType;

	/// PLAYER PART.
	cStatic*		m_pGuildName ;
	cStatic*		m_pUnionName ;
	cStatic*		m_pPlayerName ;
	cStatic*		m_playerLife ;
	cStatic*		m_pMasterName ;
	cStatic*		m_pPetName ;
	CObjectGuagen*	m_pPlayerGuage ;
	cStatic*		m_pNPCFace ;
	cStatic*		m_pNPCName ;
	cStatic*		m_pNPCOccupation ;
	DWORD mAlphaTick;

	cStatic*		m_pCharacterMain;

	// 지목된 대상
	DWORD			m_dwWantedTarget;
	DWORD			m_dwTargetVersusID;

	cMonsterTargetDlg* m_pNormalMonster;
	cMonsterTargetDlg* m_pBossMonster;
	cMonsterTargetDlg* m_pCurTarget;
	cCharacterTargetDlg* m_pCharacterTarget;

	POINT			m_BuffIconPos;
public:
	void SetName( CObject* pObject, char* name ) ;

	void SetGuildName( char* guildName, char* unionName ) ;
	cStatic* GetGuildStatic() { return m_pGuildName ; }
	cStatic* GetUnionStatic() { return m_pUnionName ; }
	cStatic* GetMasterStatic() { return m_pMasterName ; }
	void SetLife( CPlayer* pPlayer ) ;
	void SetGuage( CObject* pObject ) ;

	void InsertSkillIcon( DWORD ID, DWORD Index, DWORD time, int count );
	void RemoveSkillIcon( DWORD ID, DWORD Index );
	void RemoveSkill( DWORD Index );
	void RemoveAllSkillIcon();
	DWORD	GetWantedTargetID() { return	m_dwWantedTarget; }
	DWORD	GetTargetVersusID() { return	m_dwTargetVersusID; }

	cStatic* GetHPStatic() { return m_pHPMark ; }
	cStatic* GetLifeStatic() { return m_playerLife ; }

	CObjectGuagen* GetMonsterGuage() { return m_pMonsterGuage ; }
	CObjectGuagen* GetPlayerGuage() { return m_pPlayerGuage ; }
    void ResetDlg();
	void StartFadeOut();

	void SetTargetVersusInfo( DWORD dwTargetVSID );

	void SetTargetNameInfo( CObject* pObject );

	void SetTargetGuageInfo( CObject* pObject );

	void SetMonsterTargetType( TargetDlgType eTarget );

	void SetBossTargetDlg( cDialog* pBossTargetDlg ) { m_pBossMonster = (cMonsterTargetDlg*)pBossTargetDlg; }

	void SetMonsterTargetDlg( cDialog* pMonsterTargetDlg ) { m_pNormalMonster = (cMonsterTargetDlg*)pMonsterTargetDlg; }

	void SetCharacterTargetDlg( cDialog* pCharacterTargetDlg ) { m_pCharacterTarget = (cCharacterTargetDlg*)pCharacterTargetDlg; }

	void LoadMonsterBuffIconPosition();

	void TargetVersusDie();
};

#endif // !defined(AFX_MONSTERGUAGEDLG_H__3B4654AD_257E_42F7_9996_C2C8E89E535E__INCLUDED_)
