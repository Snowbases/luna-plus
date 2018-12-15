#include "StdAfx.h"
#include "FlagManager.h"

namespace Trigger
{
	CFlagManager::CFlagManager()
	{}

	CFlagManager::~CFlagManager()
	{}

	void CFlagManager::SetFlag(eOwner ownerType, DWORD ownerIndex, DWORD alias, int value)
	{
		const FlagKey key = {ownerType, ownerIndex, alias};
        mFlagMap[key] = value;
	}

	int CFlagManager::GetFlag(eOwner ownerType, DWORD ownerIndex, DWORD alias) const
	{
		const FlagKey key = {ownerType, ownerIndex, alias};
		const FlagMap::const_iterator iter = mFlagMap.find(key);

		return mFlagMap.end() == iter ? 0 : iter->second;
	}

	bool CFlagManager::IsExist(eOwner ownerType, DWORD ownerIndex, DWORD alias) const
	{
		const FlagKey key = {ownerType, ownerIndex, alias};
		const FlagMap::const_iterator iter = mFlagMap.find(key);

		return mFlagMap.end() != iter;
	}

	void CFlagManager::RemoveFlag(eOwner ownerType, DWORD ownerIndex)
	{
		std::queue< FlagKey > flagQueue;

		for(FlagMap::const_iterator iter = mFlagMap.begin();
			mFlagMap.end() != iter;
			++iter)
		{
			const FlagKey& key = iter->first;

			if(key.mOwnerType != ownerType)
			{
				continue;
			}
			else if(key.mOwnerIndex != ownerIndex)
			{
				continue;
			}

			flagQueue.push(key);
		}

		for(; false == flagQueue.empty();
			flagQueue.pop())
		{
			const FlagKey& key = flagQueue.front();
			mFlagMap.erase(key);
		}
	}
}