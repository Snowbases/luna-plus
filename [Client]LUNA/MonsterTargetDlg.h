#pragma once

#include "./Interface/cDialog.h"

class cStatic;
class cPushupButton;
class CObjectGuagen;
class CMonster;
class cCheckBox;
class CStatusIconDlg;

#define	LIFEPOINT_FIVE	1.0f
#define	LIFEPOINT_FORE	0.8f 
#define	LIFEPOINT_THREE	0.6f
#define	LIFEPOINT_TWO	0.4f
#define	LIFEPOINT_ONE	0.2f
#define MAXLIFEPOINT	5
enum MonsterLifePoint
{
	LP_ONE	=	0,
	LP_TWO, LP_THREE, LP_FOUR, LP_FIVE,
};

enum TargetType
{
	BOSS = 0,
	NORMAL,
	CHARACTER, 
};

class cMonsterTargetDlg	:	public cDialog
{
public:
	cMonsterTargetDlg();
	virtual	~cMonsterTargetDlg();
public:
	virtual void Render();
	virtual void SetAbsXY(LONG x, LONG y);

	virtual	void OnActionEvent( LONG lId, void* p, DWORD we );
	void SetCurTargetObject( CObject* pObject ) { m_pCurTargetObject = pObject; }
	CObject* GetCurObejct() const { return m_pCurTargetObject; }
	DWORD	GetTargetVSID() { return m_dwTargetVSID; }
	
	virtual void SetMonsterUIActive( bool val );
	virtual void SetMonsterVSUIActive( bool val );
		
	virtual void SetTargetGuageInfo( CObject* pObject );
	void SetTargetNameInfo( CObject* pObject );
	virtual void SetLifePointInfo( float fHP );
	virtual void UpdateLifePoint( );

	virtual void SetVersusInfo( DWORD dwTargetVSID );

	virtual void SetMonsterLife( float fMonsterLife );

	virtual void TargetDlgLink();

	void SetTargetVSGuage( float fVsLife );

	void StartFadeOut();
	virtual void SetMonsterTargetAlpha( BYTE al );
	void LinkMonsterBuffIcon( float fIconSize, POINT IconPos, DWORD dwSkillIconCount, DWORD dwMaxIconPerLine );
	virtual void InsertMonsterSkillIcon( DWORD ID, DWORD Index, DWORD time, int count );
	virtual void RemoveMonsterSkillIcon( DWORD ID, DWORD Index );
	virtual void RemoveSkill( DWORD Index ) {}
	void RemoveMonsterAllSkillIcon();
	virtual void TargetDie() {};
	virtual void ChangeLifePoint() {};

	void SetMonsterSpecies( DWORD dwSpecies );
	void SetMonsterAttribute( DWORD dwAttribute );

	void TargetVSDie();

protected:
	cStatic*		m_pName;
	cStatic*		m_pLevel;
	CObjectGuagen*	m_pGuage;
	cStatic*		m_pGuageMain_Left;
	cStatic*		m_pGuageMain_Middle;
	cStatic*		m_pGuageMain_Right;
	cCheckBox*		m_pBuffCheckBox;
	cCheckBox*		m_pSightCheckBox;
	BOOL			m_bVisibleBuff;
	BOOL			m_bVisibleSight;
	DWORD			m_dwLifePoint;
	DWORD			m_dwAlphaStartTime;

	cStatic*		m_pLifePoint_Empty;
	cStatic*		m_pLifePoint[MAXLIFEPOINT];
	cStatic*		m_pSpecies;
	cStatic*		m_pFace;

	cStatic*		m_pVSMain_Left;
	cStatic*		m_pVSMain_Right;
	cStatic*		m_pVS;
	CObjectGuagen*	m_pVSGuage;
	cStatic*		m_pVSName;
	CObject*		m_pCurTargetObject;
	POINT			m_BuffIconStartPoint;
	TargetType		m_eTargetType;
	DWORD			m_dwLvColor;

	DWORD			m_dwTargetVSID;

	// 몬스터버프 상태
	CStatusIconDlg* m_pMonsterBuffIconDlg;

	DWORD mAlphaTick;
};