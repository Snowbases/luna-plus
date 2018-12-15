// BattleSystem.h: interface for the CBattleSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLESYSTEM_H__7473B4EA_913D_4B3D_9399_9328FBBEDDF1__INCLUDED_)
#define AFX_BATTLESYSTEM_H__7473B4EA_913D_4B3D_9399_9328FBBEDDF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Battle.h"

class CBattleFactory;

class CHero;

#define BATTLESYSTEM	CBattleSystem::GetInstance()

class CBattleSystem  
{
	CBattle m_DefaultBattle;
	CBattle* m_pCurBattle;
	
	CBattleFactory* m_BattleFactory[eBATTLE_KIND_MAX];	
	
	CBattleSystem();
public:
	MAKESINGLETON(CBattleSystem);
	virtual ~CBattleSystem();

	void Release();
	void ReleaseBattle();
	BOOL CreateBattle(BATTLE_INFO_BASE*);
	void DeleteBattle(CBattle*);
	CBattle* GetBattle();
	void Render();
	void NetworkMsgParse(BYTE Protocol,void* pMsg);
	// 070205 LYW --- Add functions to process network msg.
public :
	void Battle_Info( void* pMsg ) ;
	void Battle_Teammember_Add_notify( void* pMsg ) ;
	void Battle_Teammember_Delete_notify( void* pMsg ) ;
	void Battle_Teammember_Die_notify( void* pMsg ) ;
	void Battle_Victory_Notify( void* pMsg ) ;
	void Battle_Draw_Notify( void* pMsg ) ;
	void Battle_Destroy_Notify( void* pMsg ) ;
	void Battle_Change_Objectbattle( void* pMsg ) ;
	void Battle_Showdown_Createstage( void* pMsg ) ;
};

#endif // !defined(AFX_BATTLESYSTEM_H__7473B4EA_913D_4B3D_9399_9328FBBEDDF1__INCLUDED_)
