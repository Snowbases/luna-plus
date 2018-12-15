#pragma once

#include "skillunit.h"

class cTameSkillUnit :
	public cSkillUnit
{
public:
	cTameSkillUnit(cActiveSkillObject&);
	virtual ~cTameSkillUnit(void);
	virtual BOOL Excute();
};