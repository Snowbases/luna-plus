#pragma once

#include "./Interface/cDialog.h"

class cStatic;
class CObjectGuagen;

class cBattleGuageDlg	:	public cDialog
{
public:
	cBattleGuageDlg();
	virtual ~cBattleGuageDlg();

public:
	virtual void Render();
	virtual void SetAlpha( BYTE al );
	void	Linking();

	DWORD SetTargetLevelSignal( DWORD dwTargetLv );

	void StartBattleSignal();

	void EndBattleSignal();

	void SetBattleTargetInfo( CObject* pTargetObject );

	void GetObjectScreenCoord( CObject* pObject, POINT* pPosition );

	void TargetDie();

	void HeroDie();

	void AllLevelSignalActice( BOOL val );

	void HeroBattleGuageUpdate();

	CObject* GetCurTargetObject() const { return m_pCurTargetObject; }

public:
	cStatic*		m_pTargetBattleHPGuageMain;
	cStatic*		m_pHeroBattleHPGuageMain;

	CObjectGuagen*	m_pTargetBattleHPGuage;
	CObjectGuagen*	m_pHeroBattleHPGuage;

	cStatic*		m_pTargetLevelSignalRed;
	cStatic*		m_pTargetLevelSignalYellow;
	cStatic*		m_pTargetLevelSignalWhite;
	cStatic*		m_pTargetLevelSignalGreen;
	cStatic*		m_pTargetLevelSignalGrey;

	cStatic*		m_pCurLevelSignal;
	cStatic*		m_pNextLevelSignal;

	cStatic*		m_pHeroLevelSignalRed;
	cStatic*		m_pHeroLevelSignalYellow;
	cStatic*		m_pHeroLevelSignalWhite;
	cStatic*		m_pHeroLevelSignalGreen;
	cStatic*		m_pHeroLevelSignalGrey;

	cStatic*		m_pCurHeroLevelSignal;
	cStatic*		m_pNextHeroLevelSignal;

	cStatic*		m_pTargetHPSignal;
	cStatic*		m_pHeroHPSignal;

	POINT			m_ptTargetLvSignalDelta;
	POINT			m_ptTargetHPGuageDelta;
	POINT			m_ptTargetHPGuageMainDelta;
	POINT			m_ptTargetHPSignalDelta;
	POINT			m_ptHeroLvSignalDelta;
	POINT			m_ptHeroHPGuageDelta;
	POINT			m_ptHeroHPGuageMainDelta;
	POINT			m_ptHeroHPSignalDelta;		
	CObject*		m_pCurTargetObject;
	DWORD			m_dwAlphaTick;
	BOOL			m_bIsOnBattleGuage;
};