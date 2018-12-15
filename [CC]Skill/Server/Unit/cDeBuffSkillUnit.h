#pragma once
#include "skillunit.h"

class cDeBuffSkillUnit :
	public cSkillUnit
{
public:
	cDeBuffSkillUnit(cActiveSkillObject&);
	virtual ~cDeBuffSkillUnit();
	virtual BOOL Excute();
};
