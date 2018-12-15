#include "StdAfx.h"
#include "ActionSummonMonsterGroup.h"
#include "common.h"
#include "./Dungeon/DungeonMgr.h"

namespace Trigger
{
	void CActionsummonMonsterGroup::DoAction()
	{
		const DWORD dwGroupAlias	= GetValue(eProperty_GroupAlias);
		const DWORD dwAlias			= GetValue(eProperty_Alias);
		DWORD dwAliasValue			= 0;
		eAliasKind	Aliaskind		= eAliasKind_None;	

		if(dwAlias)
		{
			dwAliasValue = dwAlias;
			Aliaskind = eAliasKind_Alias;
		}
		if(dwGroupAlias)
		{		
			dwAliasValue = dwGroupAlias;
			Aliaskind = eAliasKind_GroupAlias;
		}

		if(0 == dwAliasValue)
			return ;

		DungeonMGR->SummonMonsterGroup( dwAliasValue, mParameter.mChannelID, Aliaskind );
	}
}