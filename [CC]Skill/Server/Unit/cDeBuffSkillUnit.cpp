#include "stdafx.h"
#include ".\cdebuffskillunit.h"
#include "..\Object\ActiveSkillObject.h"
#include "../Object/BuffSkillObject.h"
#include "../Target/Target.h"

cDeBuffSkillUnit::cDeBuffSkillUnit(cActiveSkillObject& skillObject) :
cSkillUnit(skillObject)
{}

cDeBuffSkillUnit::~cDeBuffSkillUnit(void)
{
}

BOOL cDeBuffSkillUnit::Excute()
{
	mpParentSkill->GetTarget().SetPositionHead();

	while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
	{
		if( pTarget->GetState()  == eObjectState_Die )
		{
			continue;
		}		

		pTarget->GetBuffList().SetPositionHead();

		while(cBuffSkillObject* const pBuffSkill = pTarget->GetBuffList().GetData())
		{
			// 100601 ShinJS --- 디버프의 레벨을 스크립트의 UnitDataType으로 이용
			const int skillUnitData			= (int)mAddDamage;
			const WORD skillUnitDataType	= mAddType;

			if(pBuffSkill->GetInfo().Status == skillUnitData )
			{
				if( skillUnitDataType >= pBuffSkill->GetSkillLevel() )
				{
					pBuffSkill->SetEndState();
					pBuffSkill->EndState();
				}
			}
		}
	}

	return TRUE;
}