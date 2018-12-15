#ifndef _BATTLEFACTORY_SHOWDOWN_H
#define _BATTLEFACTORY_SHOWDOWN_H

#include "BattleFactory.h"
#include "Battle_Showdown.h"

/*
struct BATTLE_INFO_BASE
{
	DWORD BattleID;		-->indexgenerate?
	BYTE BattleKind;	-->enum?
	BYTE BattleState;	-->enum?
	DWORD BattleTime;	-->Á¦ÇÑ½Ã°£?
};
*/

class CBattleFactory_Showdown : public CBattleFactory
{
public:
	CBattleFactory_Showdown();
	virtual ~CBattleFactory_Showdown();

	virtual CBattle* CreateBattle(BATTLE_INFO_BASE* pCreateInfo, MAPTYPE MapNum);
	virtual void DeleteBattle(CBattle* pBattle);
	

};




#endif