#pragma once

#define RECALLMGR	CRecallManager::GetInstance()

class CObject;
class CMonster;

class CRecallManager  
{
public:
	GETINSTANCE(CRecallManager)
	CRecallManager();
	virtual ~CRecallManager();
	void EventRecall(CObject * pRecaller, WORD MonsterKind, BYTE cbMonsterCount, BYTE cbChannel, VECTOR3* pPos, WORD wRadius, DWORD DropItemID, DWORD dwDropRatio, DWORD dwGroupID=EVENTRECALL_GROUP_ID, DWORD dwDieTime=0);
	void DeleteEventRecallMonster();
	void RecallRange(CObject* pRecaller, DWORD dwGridID, VECTOR3 * vPos,  WORD wRecallMonsterKind, DWORD wRecallNum, DWORD dwRange, int GroupID, WORD ObjectKind = 32 /*eObjectKind_Monster */);
	void Recall(WORD monsterKind, BYTE monsterCount, DWORD channel, VECTOR3&, WORD radius, DWORD groupIndex, DWORD dieTime = 0, DWORD itemIndex = 0, DWORD ItemRatio = 100);
	void QuestRecall( WORD wMonsterKind, BYTE bMonsterCount, DWORD dwChannel, VECTOR3* pMonsterPos, WORD wRadius );
	void DeleteQuestRecallMonster( DWORD dwChannel );
	void ChallengeZoneRecall( UINT wMonsterKind, BYTE bMonsterCount, DWORD dwChannel, VECTOR3* pMonsterPos, WORD wRadius );
	void DeleteChallengeZoneRecallMonster( DWORD dwChannel );
};