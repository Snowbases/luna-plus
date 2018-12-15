#pragma once


#include "Object.h"

struct BASE_MONSTER_LIST;

class CMonster : public CObject  
{
	MONSTER_TOTALINFO m_MonsterInfo;

	BASE_MONSTER_LIST * m_pSInfo;
	DWORD mLastCheckedTick;

	void InitMonster(MONSTER_TOTALINFO* pTotalInfo);
public:
	CMonster();
	virtual ~CMonster();
	
	BASE_MONSTER_LIST * GetSInfo() { /*CheckTmpFunc();*/ return m_pSInfo; }
	virtual void SetLife(DWORD val, BYTE type = 1);
	virtual DWORD GetLife(){ /*CheckTmpFunc();*/ return m_MonsterInfo.Life;	}
	virtual DWORD DoGetMaxLife(); //몬스터 MAXHP
	virtual float DoGetMoveSpeed();
	virtual LPTSTR GetObjectName();
	virtual void Process();

	friend class CObjectManager;

	WORD GetMonsterKind() const { return m_MonsterInfo.MonsterKind;	}
	virtual DWORD GetOwnerIndex() const { return m_MonsterInfo.OwnedObjectIndex; }
	void SetOwnerIndex(DWORD index) { m_MonsterInfo.OwnedObjectIndex = index; }
	virtual void SetMotionInState(EObjectState);
	virtual void Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive);
	// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
	virtual void Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO&);
	virtual void Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal);

	float GetWeight();
	float GetRadius();
	const MONSTER_TOTALINFO& GetMonsterTotalInfo() const { return m_MonsterInfo; }

	DWORD	GetMonsterTargetType();
};