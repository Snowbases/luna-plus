#include "stdafx.h"
#include "BattleFactory_Showdown.h"
#include "BattleTeam_Showdown.h"

#ifdef _MAPSERVER_
#include "BattleSystem_server.h"
#endif


CBattleFactory_Showdown::CBattleFactory_Showdown()
{

}

CBattleFactory_Showdown::~CBattleFactory_Showdown()
{

}


CBattle* CBattleFactory_Showdown::CreateBattle(BATTLE_INFO_BASE* pCreateInfo, MAPTYPE MapNum)
{	
#ifdef _MAPSERVER_
	BATTLE_INFO_SHOWDOWN* pInfo = (BATTLE_INFO_SHOWDOWN*)pCreateInfo;
	pInfo->BattleID = BATTLESYSTEM->MakeNewBattleID();
#endif

	// battle setting
	CBattle_Showdown* pBattle = new CBattle_Showdown;
	pBattle->Initialize(pCreateInfo,NULL,NULL);
	
	return pBattle;
}

void CBattleFactory_Showdown::DeleteBattle(CBattle* pBattle)
{
#ifdef _MAPSERVER_
	BATTLESYSTEM->ReleaseBattleID(pBattle->GetBattleID());
#endif

	delete pBattle;	
}
