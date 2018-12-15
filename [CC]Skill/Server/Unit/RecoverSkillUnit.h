#pragma once
#include "skillunit.h"

class cRecoverSkillUnit :
	public cSkillUnit
{
public:
	cRecoverSkillUnit(cActiveSkillObject&);
	virtual ~cRecoverSkillUnit();
	virtual BOOL Excute();
};