#pragma once

#include "skillunit.h"

class cSummonSkillUnit :
	public cSkillUnit
{
public:
	cSummonSkillUnit(cActiveSkillObject&);
	virtual ~cSummonSkillUnit(void);
	virtual BOOL Excute();
};