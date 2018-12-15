#include "StdAfx.h"
#include "common.h"
#include "ActionBeginQuest.h"
#include "../Player.h"
#include "../UserTable.h"
#include "../QuestManager.h"
#include "../../[CC]Quest/QuestInfo.h"

namespace Trigger
{
	void CActionBeginQuest::DoAction()
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
			else if(player->GetQuestGroup().IsQuestComplete(mainQuestIndex))
			{
				PutLog("Player finished quest already");
				continue;
			}

			BeginQuest(
				*player,
				mainQuestIndex,
				subQuestIndex);
		}
	}

	void CActionBeginQuest::BeginQuest(CPlayer& player, DWORD mainQuestIndex, DWORD subQuestIndex)
	{
		// 091019 LUJ, 서브 퀘스트 번호가 0인 경우, 항상 퀘스트가 시작되어야 한다
		if(0 == subQuestIndex)
		{
			QUESTMGR->StartQuest(
				&player,
				mainQuestIndex,
				0);
		}

		QUESTMGR->StartSubQuest(
			&player,
			mainQuestIndex,
			subQuestIndex);
	}
}