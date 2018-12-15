#pragma once

#include "MonsterTargetDlg.h"


class	cBossTargetDlg	:	public	cMonsterTargetDlg
{
public:
	cBossTargetDlg();
	virtual	~cBossTargetDlg();
public:
	virtual void Render();

public:
	virtual void SetMonsterUIActive( bool val );

	virtual void SetTargetGuageInfo( CObject* pObject );

	virtual void SetLifePointInfo( float fHP );
	virtual void UpdateLifePoint();
	virtual void SetMonsterLife( float fMonsterLife );

	virtual void TargetDlgLink();

	void SetBossMonsterLifeGuage();
	void GodBuffCheck( DWORD dwSkillIdx, BOOL IsRemoveSkill = 0 );
	void GodGuageSwitch( BOOL bSwitch );

	virtual void InsertMonsterSkillIcon( DWORD ID, DWORD Index, DWORD time, int count );
	virtual void RemoveMonsterSkillIcon( DWORD ID, DWORD Index );
	virtual void RemoveSkill( DWORD Index );
	
	void MonsterEmergency();
	virtual void SetMonsterTargetAlpha( BYTE al );
	virtual void TargetDie();
	virtual void ChangeLifePoint();

	void RemainLifePointStart();
	void RenderRemainLifePoint();

public:
	CObjectGuagen*	m_pBossGuage[MAXLIFEPOINT];
	CObjectGuagen*	m_pUnbeatableGuage;
	//보스 무적 버프 여부
	BOOL			m_bIsUnbeatable;
	cStatic*		m_pBossEmergency_Left;
	cStatic*		m_pBossEmergency_Middle;
	cStatic*		m_pBossEmergency_Right;
	cStatic*		m_pBossEmptyGuage_Left;
	cStatic*		m_pBossEmptyGuage_Middle;
	cStatic*		m_pBossEmptyGuage_Right;
	DWORD			m_dwEmergencyTick;
	BOOL			m_bIsEmergency;
	
	BOOL			m_bIsRemainLifePoint;
	DWORD			m_dwRemainTimeStart;
	DWORD			m_dwRemainLifePoint;
};