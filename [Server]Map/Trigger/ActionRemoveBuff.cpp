#include "StdAfx.h"
#include "ActionRemoveBuff.h"
#include "common.h"
#include "Trigger.h"
#include "Manager.h"
#include "..\UserTable.h"
#include "..\..\[cc]skill\Server\Info\BuffSkillInfo.h"
#include "..\..\[cc]skill\server\manager\skillmanager.h"
#include "..\..\[CC]Skill\Server\Object\BuffSkillObject.h"
#include "./Dungeon/DungeonMgr.h"

namespace Trigger
{
	void CActionRemoveBuff::DoAction()
	{
		const DWORD buffSkillIndex = GetValue(eProperty_SkillIndex);
		const cBuffSkillInfo* const skillInfo = SKILLMGR->GetBuffInfo(buffSkillIndex);

		if(0 == skillInfo ||
			skillInfo->GetKind() != SKILLKIND_BUFF)
		{
			return;
		}

		// 100419 ONS 인던 스위치버프를 저장한다.
		DungeonMGR->DelSwitchBuff( GetChannelID(), buffSkillIndex );	

		for(DWORD objectIndex = GetHeadTarget();
			objectIndex > 0;
			objectIndex = GetNextTarget())
		{
			CObject* const object = g_pUserTable->FindUser(objectIndex);

			if(0 == object)
			{
				continue;
			}

			object->GetBuffList().SetPositionHead();

			while(cBuffSkillObject* const skillObject = object->GetBuffList().GetData())
			{
				if(skillInfo->GetIndex() == skillObject->GetSkillIdx())
				{
					skillObject->SetEndState();
					skillObject->EndState();	
				}
			}
		}
	}
}