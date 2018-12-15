#include "StdAfx.h"
#include "SkillUnit.h"
#include "..\Object\ActiveSkillObject.h"

cSkillUnit::cSkillUnit(cActiveSkillObject& skillObject) :
mpParentSkill(&skillObject),
mUnitType(skillObject.GetInfo().Unit),
mAccuracy(skillObject.GetInfo().Success),
mAddDamage(float(skillObject.GetInfo().UnitData)),
mAddType(skillObject.GetInfo().UnitDataType)
{}