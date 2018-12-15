#pragma once

namespace Trigger
{
	enum eOwner;

	class CFlagManager
	{
	public:
		CFlagManager();
		~CFlagManager();
		void SetFlag(eOwner, DWORD ownerIndex, DWORD alias, int value);
		int GetFlag(eOwner, DWORD ownerIndex, DWORD alias) const;
		void RemoveFlag(eOwner, DWORD ownerIndex);
		bool IsExist(eOwner, DWORD ownerIndex, DWORD alias) const;

	private:
		struct FlagKey
		{
			eOwner mOwnerType;
			DWORD mOwnerIndex;
			DWORD mAlias;
		};
		typedef int FlagValue;
		class CompareKey
		{
		public:
			bool operator()(const FlagKey& lhs, const FlagKey& rhs) const
			{
				if(lhs.mOwnerType < rhs.mOwnerType)
				{
					return true;
				}
				else if(lhs.mOwnerIndex < lhs.mOwnerIndex)
				{
					return true;
				}
				
				return lhs.mAlias < rhs.mAlias;
			}
		};
		typedef std::map< FlagKey, FlagValue, CompareKey > FlagMap;
		FlagMap mFlagMap;
	};
}