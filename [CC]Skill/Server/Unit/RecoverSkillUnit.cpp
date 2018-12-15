#include "stdafx.h"
#include ".\recoverskillunit.h"

#include "../Target/Target.h"
#include "../Object/ActiveSkillObject.h"
#include "../Info/ActiveSkillInfo.h"

#include "AttackManager.h"
#include "PackedData.h"

cRecoverSkillUnit::cRecoverSkillUnit(cActiveSkillObject& skillObject) :
cSkillUnit(skillObject)
{}

cRecoverSkillUnit::~cRecoverSkillUnit(void)
{
}

BOOL cRecoverSkillUnit::Excute()
{
	MSG_SKILL_RESULT msg;

	msg.InitMsg( mpParentSkill->GetID() );
	msg.SkillDamageKind = false;

	CTargetListIterator iter(&msg.TargetList);

	mpParentSkill->GetTarget().SetPositionHead();

	while(CObject* const pTarget = mpParentSkill->GetTarget().GetData())
	{
		if( pTarget->GetState()  == eObjectState_Die )
		{
			continue;
		}

		RESULTINFO resultinfo = {0};
		resultinfo.mSkillIndex = mpParentSkill->GetSkillIdx();

		switch(mUnitType)
		{
		case UNITKIND_LIFE_RECOVER:
			{
				ATTACKMGR->RecoverLife(
					mpParentSkill->GetOperator(),
					pTarget,
					mAddDamage,
					&resultinfo);
				break;
			}
		case UNITKIND_MANA_RECOVER:
			{
				ATTACKMGR->RecoverMana(
					mpParentSkill->GetOperator(),
					pTarget,
					mAddDamage,
					&resultinfo);
				break;
			}
		}

		iter.AddTargetWithResultInfo( pTarget->GetID(), 1, &resultinfo);
		iter.Release();

		PACKEDDATA_OBJ->QuickSend( pTarget, &msg, msg.GetMsgLength() );
	}

	return TRUE;
}