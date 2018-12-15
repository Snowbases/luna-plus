#include "stdafx.h"
#include "ShowdownManager.h"
#include "./interface/cWindowManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "GameIn.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "objectstatemanager.h"

#include "BattleSystem_Client.h"

GLOBALTON(CShowdownManager);

CShowdownManager::CShowdownManager()
{
	m_bIsShowdown = FALSE;
}

CShowdownManager::~CShowdownManager()
{

}

void CShowdownManager::Init()
{
	m_bIsShowdown = FALSE;
}

void CShowdownManager::ApplyShowdown()
{
	if( BATTLESYSTEM->GetBattle()->GetBattleKind() != eBATTLE_KIND_NONE || m_bIsShowdown )
	{
		//ºñ¹«¸¦ ÇÒ ¼ö ¾ø´Â »óÅÂÀÔ´Ï´Ù. confirm
		return;
	}
	if( HERO->IsPKMode() )
	{
		// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(370) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(417) );
		return;
	}
	if( HERO->GetState() != eObjectState_None && HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
	{
		// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(370) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(417) );
		return;
	}

	CObject* pOpPlayer = OBJECTMGR->GetSelectedObject();

	if( pOpPlayer == NULL || pOpPlayer == HERO )
	{
		// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(380) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(427) );
		return;
	}
	if( pOpPlayer->GetObjectKind() != eObjectKind_Player )
	{
		// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(380) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(427) );
		return;
	}
	if( pOpPlayer->GetState() == eObjectState_Die || pOpPlayer->GetState() == eObjectState_Fishing)
	{
		// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(368) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(415) );
		return;
	}

	CPlayer* pTargetPlayer = (CPlayer*)pOpPlayer;

	// 090424 ShinJS --- 고정형 탈것에 탑승중인경우 실행하지 않는다(결투신청자)
	if( HERO->IsGetOnVehicle() )
	{
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( HERO->GetVehicleID() );

		// 탑승중인 탈것이 고정형인 경우
		if( pVehicle && 
			pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->DoGetMoveSpeed() <= 0.0f )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
			return;
		}
	}

	// 090424 ShinJS --- 고정형 탈것에 탑승중인경우 실행하지 않는다(결투대상자)
	if( pTargetPlayer->IsGetOnVehicle() )
	{
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pTargetPlayer->GetVehicleID() );

		// 탑승중인 탈것이 고정형인 경우
		if( pVehicle && 
			pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->DoGetMoveSpeed() <= 0.0f )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
			return;
		}
	}

	SetShowdown( TRUE );
	HERO->DisableAutoAttack();			//°ø°Ý Ãë¼Ò
	HERO->SetNextAction(NULL);			//½ºÅ³ Ãë¼Ò
	MOVEMGR->HeroMoveStop();
//	HERO->SetState( eObjectState_BattleReady );
	OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_BattleReady);
	

	
//---NETWORK MSG	
	MSG_DWORD msg;
	msg.Category	= MP_BATTLE;
	msg.Protocol	= MP_BATTLE_SHOWDOWN_APPLY_SYN;
	msg.dwObjectID	= gHeroID;
	msg.dwData		= pOpPlayer->GetID();
	NETWORK->Send( &msg, sizeof(MSG_DWORD) );
}

void CShowdownManager::CancelApply()
{
	MSGBASE msg;

	msg.Category	= MP_BATTLE;
	msg.Protocol	= MP_BATTLE_SHOWDOWN_WAITING_CANCEL_SYN;
	msg.dwObjectID	= gHeroID;

	NETWORK->Send( &msg, sizeof(MSGBASE) );	
}

void CShowdownManager::AcceptShowdown( BOOL bAccept )
{
	if( bAccept )
	{
		if( CanAcceptShowdown( HERO ) )
		{
			HERO->DisableAutoAttack();					//ÀÚµ¿ °ø°Ý Ãë¼Ò
			HERO->SetNextAction(NULL);					//½ºÅ³ Ãë¼Ò
			MOVEMGR->HeroMoveStop();
//			HERO->SetState( eObjectState_BattleReady );	//ÇØÁ¦È®ÀÎ
			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_BattleReady);


			MSGBASE msg;
			msg.Category	= MP_BATTLE;
			msg.Protocol	= MP_BATTLE_SHOWDOWN_ACCEPT_SYN;
			msg.dwObjectID	= gHeroID;
			NETWORK->Send( &msg, sizeof(MSGBASE) );

			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(378) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(425) );
		}
		else
		{
			MSG_BYTE msg;
			msg.Category	= MP_BATTLE;
			msg.Protocol	= MP_BATTLE_SHOWDOWN_ERROR;
			msg.dwObjectID	= gHeroID;
			msg.bData		= 1;
			NETWORK->Send( &msg, sizeof(MSG_BYTE) );

			// 070215 LYW --- ShowdownManager : Modified message number.
		//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(370) );
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(417) );
			SetShowdown( FALSE );
		}
	}
	else
	{
		MSGBASE msg;
		msg.Category	= MP_BATTLE;
		msg.Protocol	= MP_BATTLE_SHOWDOWN_REJECT_SYN;
		msg.dwObjectID	= gHeroID;
		NETWORK->Send( &msg, sizeof(MSGBASE) );
	}
}

BOOL CShowdownManager::CanAcceptShowdown( CPlayer* pAccepter )
{
	if( pAccepter == NULL )					return FALSE;

	if( BATTLESYSTEM->GetBattle()->GetBattleKind() != eBATTLE_KIND_NONE )	 return FALSE;
	if( pAccepter->IsPKMode() )				return FALSE;
//	if( m_bIsShowdown == TRUE )				return FALSE; //ÇöÀç ÀÌ ÇÔ¼ö´Â HERO¸¸ ÀÎÀÚ·Î ³Ñ¾î¿Â´Ù!
	if( pAccepter->GetState() != eObjectState_None && pAccepter->GetState() != eObjectState_Move
		&& pAccepter->GetState() != eObjectState_Immortal )
		return FALSE;
	
	return TRUE;
}

void CShowdownManager::SetShowdown( BOOL bShowdown )
{
	m_bIsShowdown = bShowdown;
	
//--- main bar icon
//	CMainBarDialog* pDlg = GAMEIN->GetMainInterfaceDialog();
//	if( pDlg )
//		pDlg->SetPushBarIcon( OPT_SHOWDOWNICON, m_bIsShowdown );
}

void CShowdownManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_BATTLE_SHOWDOWN_APPLY:	//»ó´ë°¡ ºñ¹«¸¦ ½ÅÃ»ÇØ¿È
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;
			CPlayer* pApplyer = (CPlayer*)OBJECTMGR->GetObject( msg->dwData );
			if( pApplyer == NULL || m_bIsShowdown == TRUE )
			{
				MSG_BYTE msg;
				msg.Category	= MP_BATTLE;
				msg.Protocol	= MP_BATTLE_SHOWDOWN_ERROR;
				msg.dwObjectID	= gHeroID;
				msg.bData		= 1;	//AcceptÇÒ¼ö ¾ø´Â »óÅÂ
				NETWORK->Send( &msg, sizeof(MSG_BYTE) );
				return;
			}

			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_SHOWDOWNAPPLY, MBT_YESNO,
									// 070126 LYW --- MsgBox : Modified message number.
									//CHATMGR->GetChatMsg(373), pApplyer->GetObjectName() );
									CHATMGR->GetChatMsg(420), pApplyer->GetObjectName() );
			if( pMsgBox ) pMsgBox->SetDefaultBtn( -1 );
			
			SetShowdown( TRUE );
		}
		break;

	case MP_BATTLE_SHOWDOWN_APPLY_ACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData );

			if( pPlayer )
				// 070215 LYW --- ShowdownManager : Modified message number.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(371), pPlayer->GetObjectName() );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(418), pPlayer->GetObjectName() );
			
			// 070215 LYW --- ShowdownManager : Modified message number.
			//WINDOWMGR->MsgBox( MBI_SHOWDOWNWAITING, MBT_CANCEL, CHATMGR->GetChatMsg(372) );
			WINDOWMGR->MsgBox( MBI_SHOWDOWNWAITING, MBT_CANCEL, CHATMGR->GetChatMsg(419) );
		}
		break;

	case MP_BATTLE_SHOWDOWN_APPLY_NACK:
		{
			SetShowdown( FALSE );
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);
			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(368) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(415) );
		}
		break;

	case MP_BATTLE_SHOWDOWN_ACCEPT:	//»ó´ë°¡ ¹Þ¾ÆµéÀÌ´Ù
		{
			MSG_DWORD* msg = (MSG_DWORD*)pMsg;
			CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( msg->dwData );

			if( pPlayer )
				// 070126 LYW --- Modified this line.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(374), pPlayer->GetObjectName() );
				// 070215 LYW --- ShowdownManager : Modified message number.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(380) );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(421), pPlayer->GetObjectName() );

			cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_SHOWDOWNWAITING );
			if( pDlg )
				WINDOWMGR->AddListDestroyWindow( pDlg );
		}
		break;

	case MP_BATTLE_SHOWDOWN_ACCEPT_ACK:
		{
			
		}
		break;

	case MP_BATTLE_SHOWDOWN_ACCEPT_NACK:
		{
			//MSGBASE* pmsg = (MSGBASE*)pMsg;
			//ºñ¹«½ÇÆÐ¸Þ¼¼Áö
			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(370) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(417) );

			SetShowdown( FALSE );
			if( HERO->GetState() == eObjectState_BattleReady )				
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);
		}
		break;

	case MP_BATTLE_SHOWDOWN_ERROR:
		{
			MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
			if( pmsg->bData == 0 )
			{

			}
			else if( pmsg->bData == 1 )		//»ó´ë°¡ accepter°¡º¸³½ error
			{
				// 070215 LYW --- ShowdownManager : Modified message number.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(381) );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(428) );
			}
			else if( pmsg->bData == 2 )
			{
				// 070215 LYW --- ShowdownManager : Modified message number.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(369) );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(416) );
			}
			else if( pmsg->bData == 3 )
			{
				// 070215 LYW --- ShowdownManager : Modified message number.
				//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(367) );
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(414) );
			}
			// 090423 ShinJS --- 탑승시 결투 불가
			else if( pmsg->bData == 4 )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1839) );
			}

			cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_SHOWDOWNWAITING );
			if( pDlg )
				WINDOWMGR->AddListDestroyWindow( pDlg );

			SetShowdown( FALSE );
			if( HERO->GetState() == eObjectState_BattleReady )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);
		}
		break;

	case MP_BATTLE_SHOWDOWN_REJECT:	//»ó´ë°¡ °ÅÀýÇÏ´Ù
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(422) );
			
			SetShowdown( FALSE );
			if( HERO->GetState() == eObjectState_BattleReady )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);

			cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_SHOWDOWNWAITING );
			if( pDlg )
				WINDOWMGR->AddListDestroyWindow( pDlg );
		}
		break;

	case MP_BATTLE_SHOWDOWN_REJECT_ACK:
		{
			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(379) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(426) );
			SetShowdown( FALSE );
		}
		break;

	case MP_BATTLE_SHOWDOWN_REJECT_NACK:
		{

		}
		break;

	case MP_BATTLE_SHOWDOWN_WAITING_CANCEL:	//»ó´ë°¡ ½ÅÃ» Ãë¼Ò
		{
			SetShowdown( FALSE );
			if( HERO->GetState() == eObjectState_BattleReady )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);

			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(376) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(423) );

			cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_SHOWDOWNAPPLY );
			if( pDlg )
				WINDOWMGR->AddListDestroyWindow( pDlg );
		}
		break;

	case MP_BATTLE_SHOWDOWN_WAITING_CANCEL_ACK:
		{
			SetShowdown( FALSE );
			if( HERO->GetState() == eObjectState_BattleReady )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_BattleReady);

			// 070215 LYW --- ShowdownManager : Modified message number.
			//CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(377) );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(424) );
		}
		break;
	case MP_BATTLE_DESTROY_NOTIFY:
		{
			SetShowdown( FALSE );			
		}
		break;
	}
}