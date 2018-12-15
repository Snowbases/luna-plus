#pragma once

#include "AIGroupPrototype.h"

#define GROUPMGR	CAIGroupManager::GetInstance()
class CAIGroup;

class CAIGroupManager  
{
	// 100104 LUJ, first: 그룹 번호, second: 그리드 번호
	typedef std::pair< DWORD, DWORD > GroupIndex;
	typedef std::map< GroupIndex, CAIGroup > GroupContainer;
	GroupContainer mGroupContainer;
	CAIGroupManager();
	virtual ~CAIGroupManager();

public:
	GETINSTANCE(CAIGroupManager)
	CAIGroup& AddGroup(DWORD groupIndex, DWORD gridIndex);
	CAIGroup* GetGroup(DWORD dwGroupID, DWORD dwGridID);
	void RegenProcess();
	// 091214 LUJ, 리젠 지점을 얻어낸다
	const VECTOR3& GetDomain(DWORD groupIndex, DWORD gridIndex, DWORD regenIndex);
};