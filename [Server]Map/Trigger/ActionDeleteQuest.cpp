#include "StdAfx.h"
#include "common.h"
#include "ActionDeleteQuest.h"
#include "../Player.h"
#include "../UserTable.h"
#include "../QuestManager.h"

namespace Trigger
{
	void CActionDeleteQuest::DoAction()
	{
		const DWORD mainQuestIndex = GetValue(eProperty_Quest);
		const BOOL isForcedly = GetValue(eProperty_IsForcedly);

		for(DWORD objectIndex = GetHeadTarget();
			0 < objectIndex;
			objectIndex = GetNextTarget())
		{
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

			if(0 == player)
			{
				continue;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				PutLog("Target is not player");
				continue;
			}

			DeleteQuest(
				*player,
				mainQuestIndex,
				isForcedly);
		}
	}

	void CActionDeleteQuest::DeleteQuest(CPlayer& player, DWORD mainQuestIndex, BOOL isForcedly)
	{
		// 091019 LUJ, 퀘스트 수행 여부에 관계없이 삭제한다
		if(TRUE == isForcedly)
		{
			QUESTMGR->DeleteQuestForcedly(
				player,
				mainQuestIndex);
		}
		// 0901019 LUJ, 수행한 퀘스트는 삭제되지 않는다
		else
		{
			QUESTMGR->DeleteQuest(
				&player,
				mainQuestIndex);
		}
	}
}