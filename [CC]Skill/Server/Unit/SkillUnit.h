#pragma once

class cActiveSkillObject;

class cSkillUnit
{
protected:
	cActiveSkillObject* const mpParentSkill;
	UNITKIND mUnitType;
	float mAccuracy;
	float mAddDamage;
	WORD mAddType;

public:
	cSkillUnit(cActiveSkillObject&);
	virtual ~cSkillUnit() {}
	virtual BOOL Excute() = 0;
	UNITKIND GetKind() const { return mUnitType; }
};
