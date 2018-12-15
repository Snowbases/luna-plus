#include "StdAfx.h"
#include "BattleSystem_Client.h"

#include "BattleFactory.h"
#include "Showdown/BattleFactory_Showdown.h"
#include "GTournament/BattleFactory_GTournament.h"
#include "ObjectManager.h"
#include "ShowdownManager.h"

#include "ChatManager.h"
#include "MHMap.h"
#include "MainGame.h"

#include "MHCamera.h"

CBattleSystem::CBattleSystem()
{
	m_pCurBattle = &m_DefaultBattle;
	
	m_BattleFactory[eBATTLE_KIND_NONE] = NULL;
	m_BattleFactory[eBATTLE_KIND_SHOWDOWN] = new CBattleFactory_Showdown;
	m_BattleFactory[eBATTLE_KIND_GTOURNAMENT] = new CBattleFactory_GTournament;
}

CBattleSystem::~CBattleSystem()
{
	Release();
}

void CBattleSystem::Release()
{
	if(m_pCurBattle != &m_DefaultBattle)
	{
		DeleteBattle(m_pCurBattle);
	}
	
	for(int n=0;n<eBATTLE_KIND_MAX;++n)
	{
		if(m_BattleFactory[n])
		{
			delete m_BattleFactory[n];
			m_BattleFactory[n] = 0;
		}
	}
}

BOOL CBattleSystem::CreateBattle(BATTLE_INFO_BASE* pInfo)
{
	ASSERT(pInfo->BattleKind < eBATTLE_KIND_MAX);
	
	m_pCurBattle = m_BattleFactory[pInfo->BattleKind]->CreateBattle(pInfo, MAP->GetMapNum());
	if(m_pCurBattle == NULL)
		return FALSE;

	return TRUE;
}

void CBattleSystem::DeleteBattle(CBattle* pBattle)
{
	m_BattleFactory[pBattle->GetBattleKind()]->DeleteBattle(pBattle);
}

CBattle* CBattleSystem::GetBattle()
{
	return m_pCurBattle;
}

void CBattleSystem::Render()
{
	m_pCurBattle->Render();
	if(m_pCurBattle->IsDestroyed() == TRUE)
	{
		ASSERT(m_pCurBattle != &m_DefaultBattle);
		m_pCurBattle->OnDestroy();
		DeleteBattle(m_pCurBattle);
		m_pCurBattle = &m_DefaultBattle;
	}
}

void CBattleSystem::ReleaseBattle()
{
	if(m_pCurBattle != &m_DefaultBattle)
	{
		m_pCurBattle->OnDestroy();
		DeleteBattle(m_pCurBattle);
		m_pCurBattle = &m_DefaultBattle;
	}
}

void CBattleSystem::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	SHOWDOWNMGR->NetworkMsgParse( Protocol, pMsg );

	switch(Protocol)
	{
	case MP_BATTLE_INFO:								Battle_Info( pMsg ) ;									break;
	case MP_BATTLE_START_NOTIFY:						m_pCurBattle->StartBattle();							break;		
	case MP_BATTLE_TEAMMEMBER_ADD_NOTIFY:				Battle_Teammember_Add_notify( pMsg ) ;					break;
	case MP_BATTLE_TEAMMEMBER_DELETE_NOTIFY:			Battle_Teammember_Delete_notify( pMsg ) ;				break;		
	case MP_BATTLE_TEAMMEMBER_DIE_NOTIFY:				Battle_Teammember_Die_notify( pMsg ) ;					break;
	case MP_BATTLE_BATTLEOBJECT_DESTROY_NOTIFY:																	break;
	case MP_BATTLE_BATTLEOBJECT_CREATE_NOTIFY:																	break;
	case MP_BATTLE_VICTORY_NOTIFY:						Battle_Victory_Notify( pMsg ) ;							break;
	case MP_BATTLE_DRAW_NOTIFY:							Battle_Draw_Notify( pMsg )  ;							break;
	case MP_BATTLE_DESTROY_NOTIFY:						Battle_Destroy_Notify( pMsg ) ;							break;
	case MP_BATTLE_CHANGE_OBJECTBATTLE:					Battle_Change_Objectbattle( pMsg ) ;					break;
	case MP_BATTLE_SHOWDOWN_CREATESTAGE:				Battle_Showdown_Createstage( pMsg ) ;					break;
	}
}


void CBattleSystem::Battle_Info( void* pMsg )
{
	MSG_BATTLE_INFO* pmsg = (MSG_BATTLE_INFO*)pMsg;
	CreateBattle((BATTLE_INFO_BASE*)pmsg->info);
}


void CBattleSystem::Battle_Teammember_Add_notify( void* pMsg )
{
	MSG_BATTLE_TEAMMEMBER_ADDDELETE* pmsg = (MSG_BATTLE_TEAMMEMBER_ADDDELETE*)pMsg;
	m_pCurBattle->OnTeamMemberAdd(pmsg->Team,pmsg->Member.MemberID,pmsg->Member.MemberName);

	// observer의 경우 처리 ( 안보이게 : 맞나? )
			if( pmsg->Team == /*eBattleTeam_Observer*/eBattleTeam_Max )
	{
		CObject* pObjcet = OBJECTMGR->GetObject( pmsg->Member.MemberID );
		if( pObjcet->GetObjectKind() != eObjectKind_Player )
			return;
		CPlayer* pPlayer = (CPlayer*)pObjcet;
		pPlayer->GetEngineObject()->HideWithScheduling();
		pPlayer->GetCharacterTotalInfo()->bVisible = false;
	}		

	//-----------------------------------
}


void CBattleSystem::Battle_Teammember_Delete_notify( void* pMsg ) 
{
	MSG_BATTLE_TEAMMEMBER_ADDDELETE* pmsg = (MSG_BATTLE_TEAMMEMBER_ADDDELETE*)pMsg;
	m_pCurBattle->OnTeamMemberDelete(pmsg->Team,pmsg->Member.MemberID,pmsg->Member.MemberName);
}


void CBattleSystem::Battle_Teammember_Die_notify( void* pMsg ) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	m_pCurBattle->OnTeamMemberDie(pmsg->dwData1,pmsg->dwData2,pmsg->dwData3);
}


void CBattleSystem::Battle_Victory_Notify( void* pMsg ) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	m_pCurBattle->Victory(pmsg->dwData1,pmsg->dwData2);
}


void CBattleSystem::Battle_Draw_Notify( void* pMsg ) 
{
	m_pCurBattle->Draw();
}


void CBattleSystem::Battle_Destroy_Notify( void* pMsg ) 
{
	m_pCurBattle->SetDestroyFlag();

	// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 적용을 위해 실행
	OBJECTMGR->ApplyOverInfoOptionToAll();
}


void CBattleSystem::Battle_Change_Objectbattle( void* pMsg ) 
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( ! pObject )
	{
		return;
	}
	pObject->RemoveObjectEffect( BATTLE_TEAMEFFECT_ID );
	if( pObject == HERO )
		pObject->RemoveObjectEffect( BATTLE_STAGEEFFECT_ID );	//비무장지우기

	pObject->SetBattle(pmsg->dwData1,(BYTE)pmsg->dwData2);
}

void CBattleSystem::Battle_Showdown_Createstage( void* pMsg ) 
{
	MSG_BATTLE_SHOWDOWN_CREATESTAGE* pmsg = (MSG_BATTLE_SHOWDOWN_CREATESTAGE*)pMsg;

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);
	if(pObject == NULL)
		return;
	
	VECTOR3 vPos;
	pObject->GetPosition( &vPos );
	vPos.x = pmsg->vPosStage.x;
	vPos.z = pmsg->vPosStage.z;

	OBJECTEFFECTDESC desc( 361, EFFECT_FLAG_ABSOLUTEPOS, &vPos );
	pObject->AddObjectEffect( BATTLE_STAGEEFFECT_ID, &desc, 1 );
}