#pragma once
#include "skillunit.h"

class cSpecialSkillUnit :
	public cSkillUnit
{
public:
	cSpecialSkillUnit(cActiveSkillObject&);
	virtual ~cSpecialSkillUnit();
	virtual BOOL Excute();
};
