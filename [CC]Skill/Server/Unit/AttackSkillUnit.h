#pragma once
#include "skillunit.h"

class cAttackSkillUnit :
	public cSkillUnit
{
public:
	cAttackSkillUnit(cActiveSkillObject&);
	virtual ~cAttackSkillUnit();
	virtual BOOL Excute();
	// 100219 ShinJS --- Damage (Life/Mana) 를 적용한다.
	virtual BOOL ApplyDamageResult( CObject* pAttacker, CObject* pTarget, RESULTINFO& damageInfo );
};
