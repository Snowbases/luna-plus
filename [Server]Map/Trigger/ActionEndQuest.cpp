#include "StdAfx.h"
#include "common.h"
#include "ActionEndQuest.h"
#include "../Player.h"
#include "../UserTable.h"
#include "../QuestManager.h"
#include "../../[CC]Quest/QuestInfo.h"

namespace Trigger
{
	void CActionEndQuest::DoAction()
	{
		const DWORD mainQuestIndex = GetValue(eProperty_Quest);
		const DWORD subQuestIndex = GetValue(eProperty_QuestSub);
		CQuestInfo* const questInfo = QUESTMGR->GetQuestInfo(mainQuestIndex);

		if(0 == questInfo)
		{
			PutLog("Quest is not found");
			return;
		}
		else if(questInfo->GetSubQuestCount() < subQuestIndex)
		{
			PutLog("Sub quest is out of range");
			return;
		}

		for(DWORD objectIndex = GetHeadTarget();
			0 < objectIndex;
			objectIndex = GetNextTarget())
		{
			CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(objectIndex);

			if(0 == player)
			{
				PutLog("Player is not found");
				continue;
			}
			else if(eObjectKind_Player != player->GetObjectKind())
			{
				PutLog("Target is not player");
				continue;
			}

			EndQuest(
				*player,
				*questInfo,
				mainQuestIndex,
				subQuestIndex);
		}
	}

	void CActionEndQuest::EndQuest(CPlayer& player, CQuestInfo& questInfo, DWORD mainQuestIndex, DWORD subQuestIndex)
	{
		QUESTMGR->EndSubQuest(
			&player,
			mainQuestIndex,
			subQuestIndex);

		// 091019 LUJ, 마지막 서브 퀘스트인 경우 종료되어야 한다
		if(questInfo.GetSubQuestCount() == subQuestIndex)
		{
			QUESTMGR->EndQuest(
				&player,
				mainQuestIndex,
				0);
		}
	}
}