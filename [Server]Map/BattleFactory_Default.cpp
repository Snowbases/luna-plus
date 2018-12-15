// BattleFactory_Default.cpp: implementation of the CBattleFactory_Default class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BattleFactory_Default.h"
#include "GridSystem.h"
#include "TileManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBattleFactory_Default::CBattleFactory_Default()
{

}

CBattleFactory_Default::~CBattleFactory_Default()
{

}

CBattle* CBattleFactory_Default::CreateBattle( BATTLE_INFO_BASE* pCreateInfo, MAPTYPE MapNum )
{
#ifdef _MAPSERVER_
	//±×¸®µå »ý¼º
	g_pServerSystem->GetGridSystem()->CreateGridTable(pCreateInfo->BattleID, eGridType_General, 512,512, MapNum);
	
	if(g_pServerSystem->GetMap()->GetTileManager()->CreateTileGroup(pCreateInfo->BattleID, MapNum) == FALSE)
	{
		return NULL;
	}
	CBattle* pBattle = new CBattle;
	pBattle->Initialize( pCreateInfo, NULL, NULL );
	return pBattle;
#else
	return NULL;
#endif
}

void CBattleFactory_Default::DeleteBattle( CBattle* pBattle )
{
#ifdef _MAPSERVER_
	g_pServerSystem->GetGridSystem()->DeleteGridTable(pBattle->GetBattleID());
	g_pServerSystem->GetMap()->GetTileManager()->DeleteTileGroup(pBattle->GetBattleID());
	delete	pBattle;					// 요 코드 빠져있었음. 2007/05/14 - 이진영
#endif
}
