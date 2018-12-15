#include "StdAfx.h"
#include "ActionSummonMonster.h"
#include "common.h"
#include "Monster.h"
#include "..\..\[CC]Header\GameResourceManager.h"
#include "..\AISystem.h"
#include "..\RegenManager.h"

namespace Trigger
{
	void CActionSummonMonster::DoAction()
	{
		const WORD monsterKind = (WORD)GetValue(eProperty_MonsterKind);
		const BASE_MONSTER_LIST* const monsterList = GAMERESRCMNGR->GetMonsterListInfo(monsterKind);

		if(0 == monsterList)
		{
			return;
		}

		VECTOR3 position = {0};
		position.x = (float)GetValue(eProperty_PosX);
		position.z = (float)GetValue(eProperty_PosZ);
		const DWORD groupAlias = GetValue(eProperty_GroupAlias);

		CMonster* const monster = REGENMGR->RegenObject(
			g_pAISystem.GeneraterMonsterID(),
			0,
			mParameter.mChannelID,
			monsterList->ObjectKind,
			monsterKind,
			&position,
			groupAlias,
			0,
			0,
			FALSE);;

		if(0 == monster)
		{
			// 몬스터 생성 실패. 오류
			return;
		}

		const DWORD alias = GetValue(eProperty_Alias);
		monster->SetAlias(alias);
		monster->SetGroupAlias(groupAlias);
	}
}