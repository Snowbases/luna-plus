#include "StdAfx.h"
#include "RecallManager.h"
#include "Monster.h"
#include "RegenManager.h"
#include "AISystem.h"
#include "CharMove.h"
#include "ChannelSystem.h"
#include "UserTable.h"
#include "..\[CC]Header\GameResourceManager.h"

CRecallManager::CRecallManager()
{
}

CRecallManager::~CRecallManager()
{

}

void CRecallManager::DeleteEventRecallMonster()
{
	g_pUserTable->SetPositionUserHead();
	CObject* pObject;
	while( (pObject = g_pUserTable->GetUserData() )!= NULL)
	{
		if( pObject->GetObjectKind() & eObjectKind_Monster )
		{
			if( ((CMonster*)pObject)->GetMonsterGroupNum() == (WORD)EVENTRECALL_GROUP_ID )
			{
				g_pServerSystem->RemoveMonster( pObject->GetID() );
			}
		}
	}
}

void CRecallManager::EventRecall(CObject * pRecaller, WORD MonsterKind, BYTE cbMonsterCount, BYTE cbChannel, VECTOR3* pPos, WORD wRadius, DWORD DropItemID, DWORD dwDropRatio, DWORD dwGroupID, DWORD dwDieTime )
{
	DWORD channel = 0;
	DWORD maxChannel = CHANNELSYSTEM->GetChannelCount();

	if(0 < cbChannel)
	{
		channel = cbChannel - 1;
		maxChannel = cbChannel;
	}

	for(; channel < maxChannel; ++channel)
	{
		Recall(
			MonsterKind,
			cbMonsterCount,
			CHANNELSYSTEM->GetChannelID(channel),
			*pPos,
			wRadius,
			DWORD(EVENTRECALL_GROUP_ID),
			dwDieTime ? dwDieTime + gCurTime : 0,
			DropItemID,
			dwDropRatio);
	}
}

void CRecallManager::RecallRange( CObject* pRecaller, DWORD dwGridID, VECTOR3 * vPos, WORD wRecallMonsterKind, DWORD wRecallNum, DWORD dwRange, int GroupID, WORD ObjectKind /* = eObjectKind_Monster */ )
{
	Recall(
		wRecallMonsterKind,
		BYTE(wRecallNum),
		dwGridID,
		*vPos,
		WORD(dwRange),
		DWORD(RECALL_GROUP_ID));
}

void CRecallManager::QuestRecall( WORD wMonsterKind, BYTE bMonsterCount, DWORD dwChannel, VECTOR3* pMonsterPos, WORD wRadius )
{
	Recall(
		wMonsterKind,
		bMonsterCount,
		dwChannel,
		*pMonsterPos,
		wRadius,
		DWORD(QUESTRECALL_GROUP_ID));
}

void CRecallManager::DeleteQuestRecallMonster( DWORD dwChannel )
{
	CObject* pObject = NULL;
	CMonster* pMonster = NULL;

	g_pUserTable->SetPositionUserHead();	
	while( (pObject = g_pUserTable->GetUserData() )!= NULL)
	{
		if( pObject->GetObjectKind() & eObjectKind_Monster )
		{
			pMonster = (CMonster*)pObject;
			if( pMonster->GetMonsterGroupNum() == (WORD)QUESTRECALL_GROUP_ID &&
				pMonster->GetBattleID() == dwChannel )
				g_pServerSystem->RemoveMonster( pObject->GetID() );
		}
	}
}

void CRecallManager::ChallengeZoneRecall( UINT wMonsterKind, BYTE bMonsterCount, DWORD dwChannel, VECTOR3* pMonsterPos, WORD wRadius )
{
	Recall(
		WORD(wMonsterKind),
		bMonsterCount,
		dwChannel,
		*pMonsterPos,
		wRadius,
		DWORD(CHALLENGERECALL_GROUP_ID));
}

void CRecallManager::DeleteChallengeZoneRecallMonster( DWORD dwChannel )
{
	CObject* pObject = NULL;
	CMonster* pMonster = NULL;

	g_pUserTable->SetPositionUserHead();	
	while( (pObject = g_pUserTable->GetUserData() )!= NULL)
	{
		if( pObject->GetObjectKind() & eObjectKind_Monster )
		{
			pMonster = (CMonster*)pObject;
			if( pMonster->GetMonsterGroupNum() == (WORD)CHALLENGERECALL_GROUP_ID &&
				pMonster->GetBattleID() == dwChannel )
				g_pServerSystem->RemoveMonster( pObject->GetID() );
		}
	}
}

void CRecallManager::Recall(WORD monsterKind, BYTE monsterCount, DWORD channel, VECTOR3& position, WORD radius, DWORD groupIndex, DWORD dieTime, DWORD itemIndex, DWORD itemRatio)
{
	const BASE_MONSTER_LIST* const baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo(monsterKind);

	if(0 == baseMonsterList)
	{
		return;
	}

	for(BYTE i = 0; i < monsterCount; ++i)
	{
		if(0 == channel)
		{
			continue;
		}

		VECTOR3 vRecallPos = position;

		if(0 < radius)
		{
			vRecallPos.x = position.x + (rand() % (radius * 2)) - radius;
			vRecallPos.z = position.z + (rand() % (radius * 2)) - radius;
		}

		const float maxMapSize = 51199;
		vRecallPos.x = max(vRecallPos.x, 0);
		vRecallPos.x = min(vRecallPos.x, maxMapSize);
		vRecallPos.z = max(vRecallPos.z, 0);
		vRecallPos.z = min(vRecallPos.z, maxMapSize);

		CMonster* const monster = REGENMGR->RegenObject(
			g_pAISystem.GeneraterMonsterID(),
			0,
			channel,
			baseMonsterList->ObjectKind,
			monsterKind,
			&vRecallPos,
			groupIndex,
			itemIndex,
			itemRatio,
			FALSE,
			TRUE);

		if(0 == monster)
		{
			continue;
		}

		monster->SetDieTime(dieTime);
	}
}