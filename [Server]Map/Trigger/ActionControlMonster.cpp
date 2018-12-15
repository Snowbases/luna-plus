#include "StdAfx.h"
#include "ActionControlMonster.h"
#include "common.h"
#include "..\UserTable.h"
#include "..\StateMachinen.h"
#include "..\Monster.h"
#include "..\..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\GridSystem.h"
#include "..\CharMove.h"

namespace Trigger
{
	void CActionControlMonster::DoAction()
	{
		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CMonster* const monster = (CMonster*)g_pUserTable->FindUser(objectIndex);

			if(0 == monster)
			{
				continue;
			}
			else if(FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
			{
				continue;
			}

			switch(GetValue(eProperty_ControlType))
			{
			case eControlTypePause:
				{
					GSTATEMACHINE.SetState(
						monster,
						eMA_PAUSE);
					break;
				}
			case eControlTypeResume:
				{
					GSTATEMACHINE.SetState(
						monster,
						eMA_STAND);
					break;
				}
			case eControlTypeSkill:
				{
                    UseSkill(*monster);
					break;
				}
			}
		}
	}

	void CActionControlMonster::UseSkill(CMonster& monster)
	{
		const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
		DWORD skillIndex = GetValue(eProperty_SkillIndex);

		if(0 == skillIndex)
		{
			const DWORD index = baseMonsterList.AttackNum % max(1, baseMonsterList.AttackNum);
			skillIndex = baseMonsterList.AttackIndex[index];
		}

		const DWORD range = GetValue(eProperty_Range);
		const DWORD searchRange = (0 == range ? baseMonsterList.SearchRange : range);
		CObject* targetObject = &monster;
		
		switch(GetValue(eProperty_ObjectKind))
		{
		case eObjectKind_Player:
			{
				targetObject = g_pServerSystem->GetGridSystem()->FindPlayerInRange(
					CCharMove::GetPosition(&monster),
					float(searchRange),
					&monster,
					FIND_CLOSE);
				break;
			}
		case eObjectKind_Monster:
			{
				targetObject = monster.DoFriendSearch(
					searchRange);
				break;
			}
		}

		SKILLMGR->MonsterAttack(
			skillIndex,
			&monster,
			targetObject);
		monster.SetTObject(
			&monster == targetObject ? 0 : targetObject);
		GSTATEMACHINE.SetState(
			&monster,
			eMA_PERSUIT);
	}
}