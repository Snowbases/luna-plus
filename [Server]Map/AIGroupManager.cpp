#include "StdAfx.h"
#include "AIGroupManager.h"

CAIGroupManager::CAIGroupManager()
{}

CAIGroupManager::~CAIGroupManager()
{}

CAIGroup& CAIGroupManager::AddGroup(DWORD groupIndex, DWORD gridIndex)
{
	mGroupContainer.erase(
		GroupIndex(groupIndex, gridIndex));

	CAIGroup& group = mGroupContainer[GroupIndex(groupIndex, gridIndex)];
	group.SetGroupID(groupIndex);
	group.SetGridID(gridIndex);

	return group;
}

CAIGroup* CAIGroupManager::GetGroup(DWORD dwGroupID, DWORD dwGridID)
{
	const GroupContainer::iterator iterator = mGroupContainer.find(GroupIndex(dwGroupID, dwGridID));

	return mGroupContainer.end() == iterator ? 0 : &(iterator->second);
}

void CAIGroupManager::RegenProcess()
{
	for(GroupContainer::iterator iterator = mGroupContainer.begin();
		mGroupContainer.end() != iterator;
		++iterator)
	{
		CAIGroup& group = iterator->second;
		group.RegenProcess();
	}
}

const VECTOR3& CAIGroupManager::GetDomain(DWORD groupIndex, DWORD gridIndex, DWORD regenIndex)
{
	static const VECTOR3 emptyPosition = {0};
	CAIGroup* const aiGroup = GetGroup(
		groupIndex,
		gridIndex);

	if(0 == aiGroup)
	{
		return emptyPosition;
	}

	CAIGroup::Parameter* const parameter = aiGroup->GetRegenObject(regenIndex);

	if(0 == parameter)
	{
		return emptyPosition;
	}

	return parameter->vPos;
}