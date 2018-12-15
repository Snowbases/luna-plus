#pragma once

#define REGENMGR	CRegenManager::GetInstance()

class CVehicle;

class CRegenManager  
{
	void RangePosAtOrig(const VECTOR3& OrigPos, int Range, VECTOR3& OutPos) const;
public:
	GETINSTANCE(CRegenManager)
	CRegenManager();
	virtual ~CRegenManager();
	void RegenGroup(DWORD groupID, DWORD dwGridID);
	CMonster* RegenObject(DWORD dwID, DWORD dwSubID, DWORD dwGridID, EObjectKind, WORD wMonsterKind, const VECTOR3*, DWORD dwGroupID, DWORD DropItemID = 0, DWORD dwDropRatio = 100 ,BOOL bRandomPos = TRUE, BOOL bEventRegen = FALSE, DWORD ownedObjectIndex = 0 );
	CVehicle* RegenVehicle(CPlayer*, DWORD monsterKind, VECTOR3& position);
	CVehicle* RegenVehicle(DWORD gridIndex, DWORD monsterKind, VECTOR3& position);
};