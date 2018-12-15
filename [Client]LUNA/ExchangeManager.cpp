#include "stdafx.h"
#include "ExchangeManager.h"
#include "Item.h"
#include "GameIn.h"
#include "ExchangeItem.h"
#include "./interface/cWindowManager.h"
#include "WindowIdEnum.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "MoveManager.h"
#include "ShowdownManager.h"

#include "ExchangeDialog.h"
#include "InventoryExDialog.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"
#include "ObjectStateManager.h"
#include "QuickManager.h"

#include "Battle.h"
#include "BattleSystem_Client.h"

GLOBALTON(CExchangeManager)

// 070223 LYW --- ExchangeManager : Modified message number.

CExchangeManager::CExchangeManager()
{
	m_IconIndexCreator.Init(MAX_ITEMICON_NUM, IG_EXCHANGEITEM_STARTINDEX);
	
	m_nLinkedItemNum	= 0;
	m_bLockSyncing		= FALSE;
	m_bExchangeSyncing	= FALSE;
	m_bInventoryActive	= FALSE;
//	m_nExchangeState	= eEXSTT_NONE;
}

CExchangeManager::~CExchangeManager()
{
	m_IconIndexCreator.Release();
}

void CExchangeManager::Init()	//exchangedialog에서 active false 일때 호출한다. CONFIRM
{
	m_nLinkedItemNum	= 0;
	m_bLockSyncing		= FALSE;
	m_bExchangeSyncing	= FALSE;
	m_IsExchanging		= FALSE;
}

void CExchangeManager::ExchangeStart()
{
//---교환신청상태로 만들기
	HERO->DisableAutoAttack();					//자동 공격 취소
	HERO->SetNextAction(NULL);					//스킬 취소
	MOVEMGR->HeroMoveStop();					//제자리 멈춤 메세지
	OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Exchange);

	m_IsExchanging = TRUE;
//------------------------
}

void CExchangeManager::ExchangeEnd()
{
	if( HERO->GetState() == eObjectState_Exchange )
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Exchange);

	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);

	m_IsExchanging = FALSE;
}

BOOL CExchangeManager::ApplyExchange()
{
#ifdef _TESTCLIENT_
	return FALSE;
#endif
//---교환신청 가능상태인지 체크
	if( m_IsExchanging  )
	{
		//"이미 교환 신청중 입니다."
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(455) );
		return FALSE;
	}

	if( HERO->GetState() != eObjectState_None && HERO->GetState() != eObjectState_Move &&
		HERO->GetState() != eObjectState_Immortal )
	{
		//"교환 할 수 없는 상태입니다."
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(456) );
		return FALSE;
	}

	if( SHOWDOWNMGR->IsShowdown() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(456) );
		return FALSE;
	}

	CObject* pOpPlayer = OBJECTMGR->GetSelectedObject();
	if( pOpPlayer == NULL ) 
	{
		//"교환을 하시려면 먼저 원하는 상대를 선택해 주세요."
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(458) );
		return FALSE;
	}

	if( pOpPlayer->GetObjectKind() != eObjectKind_Player || pOpPlayer == HERO ) 
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(458) );
		return FALSE;
	}

	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	if( pBattle && pBattle->GetBattleKind() == eBATTLE_KIND_GTOURNAMENT )
	if( HERO->GetBattleTeam() == 2 )
		return FALSE;

//------------------------------	
	
	ExchangeStart();

//---상대에게 교환신청 메세지 보내기
	MSG_DWORD msgApply;
	msgApply.Category	= MP_EXCHANGE;
	msgApply.Protocol	= MP_EXCHANGE_APPLY_SYN;
	msgApply.dwObjectID	= gHeroID;
	msgApply.dwData		= pOpPlayer->GetID();
	NETWORK->Send( &msgApply, sizeof( msgApply ) );
//-----------------------------------

	return TRUE;
}

BOOL CExchangeManager::CanAcceptExchange( CPlayer* pAccepter )
{
	if( pAccepter == NULL )					return FALSE;
	if( SHOWDOWNMGR->IsShowdown() )				return FALSE;

	if( pAccepter->GetState() != eObjectState_None && pAccepter->GetState() != eObjectState_Move
		&& pAccepter->GetState() != eObjectState_Immortal )
		return FALSE;
	
	return TRUE;	
}

void CExchangeManager::AcceptExchange( BOOL bAccept )
{
	if( bAccept )
	{
		if( CanAcceptExchange( HERO ) )
		{
			ExchangeStart();	//nack에서 해제확인
			
			MSGBASE msgAccept;
			msgAccept.Category		= MP_EXCHANGE;
			msgAccept.dwObjectID	= gHeroID;
			msgAccept.Protocol		= MP_EXCHANGE_ACCEPT_SYN;
			NETWORK->Send( &msgAccept, sizeof( msgAccept ) );

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(454) );//교환을 수락하셨습니다.

		}
		else
		{
			MSGBASE msgError;		//차후 에러체크를 추가하자.
			msgError.Category		= MP_EXCHANGE;
			msgError.dwObjectID		= gHeroID;
			msgError.Protocol		= MP_EXCHANGE_CANTAPPLY;
			NETWORK->Send( &msgError, sizeof(MSGBASE) );

			m_IsExchanging			 = FALSE;

			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(456) );//교환할 수 없는상태입니다.
		}
	}
	else
	{
		MSGBASE msgAccept;
		msgAccept.Category		= MP_EXCHANGE;
		msgAccept.dwObjectID	= gHeroID;
		msgAccept.Protocol		= MP_EXCHANGE_REJECT_SYN;
		NETWORK->Send( &msgAccept, sizeof( msgAccept ) );

		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(447) );

		ExchangeEnd();
	}	
}

void CExchangeManager::CancelApply()
{
	MSGBASE msg;

	msg.Category	= MP_EXCHANGE;
	msg.Protocol	= MP_EXCHANGE_WAITING_CANCEL_SYN;
	msg.dwObjectID	= gHeroID;

	NETWORK->Send( &msg, sizeof(MSGBASE) );
}

BOOL CExchangeManager::LinkItem( CItem* pItem )
{

	if( pItem->GetType() != WT_ITEM ) return FALSE;		//인벤토리 아이템이 아니면 return
	//장착되어 있는 아이템은....안되는것 추가..		//confirm

	if( pItem->GetLinkItem() != NULL ) return FALSE;		//이미 링크된 아이템이면 return
	//링크가 되었는지 안되었는지만 알면 되지 않을까?

	if( m_nLinkedItemNum >= MAX_EXCHANGEITEM ) return FALSE;

	++m_nLinkedItemNum;
	
	pItem->SetLock( TRUE );


	MSG_LINKITEM msg;

	msg.Category	= MP_EXCHANGE;
	msg.Protocol	= MP_EXCHANGE_ADDITEM_SYN;
	msg.dwObjectID	= gHeroID;
	msg.ItemInfo	= pItem->GetItemBaseInfo();
	msg.wAbsPosition = POSTYPE(m_nLinkedItemNum);

	NETWORK->Send( &msg, sizeof( MSG_LINKITEM ) );

	return TRUE;
}

void CExchangeManager::UnLinkItem( int nTypeIndex, cIcon* pItem )
{
	if( nTypeIndex == 0 )
	{
		((CExchangeItem*)pItem)->GetLinkItem()->SetLinkPosition( 0 );
		((CExchangeItem*)pItem)->GetLinkItem()->SetLinkItem( NULL );
		((CExchangeItem*)pItem)->GetLinkItem()->SetLock( FALSE );		
	}

	m_IconIndexCreator.ReleaseIndex( pItem->GetID() );
	WINDOWMGR->AddListDestroyWindow( pItem );
}

void CExchangeManager::Lock( BOOL bLock )
{
	MSGBASE msg;
	msg.Category	= MP_EXCHANGE;
	msg.dwObjectID	= gHeroID;

	if( bLock )
	{
		msg.Protocol = MP_EXCHANGE_LOCK_SYN;
		NETWORK->Send( &msg, sizeof( MSGBASE ) );

		m_bLockSyncing = TRUE;
	}

//	else
//	{
//		msg.Protocol = MP_EXCHANGE_UNLOCK_SYN;
//	}
}


void CExchangeManager::Exchange()
{
	MSGBASE msg;
	msg.Category	= MP_EXCHANGE;
	msg.dwObjectID	= gHeroID;
	msg.Protocol	= MP_EXCHANGE_EXCHANGE_SYN;

	NETWORK->Send( &msg, sizeof( MSGBASE ) );
	
	//KES 040801 교환중에 인벤토리 사용 막기
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);

	m_bExchangeSyncing = TRUE;
}


void CExchangeManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_EXCHANGE_APPLY:					Exchange_Apply( pMsg ) ;				break;
	case MP_EXCHANGE_APPLY_ACK:				Exchange_Apply_Ack( pMsg ) ;			break;
	case MP_EXCHANGE_APPLY_NACK:			Exchange_Apply_Nack() ;					break;
	case MP_EXCHANGE_ACCEPT_NACK:			Exchange_Accept_Nack( pMsg ) ;			break;
	case MP_EXCHANGE_ACCEPT:				Exchange_Accept( pMsg ) ;				break;
	case MP_EXCHANGE_REJECT:				Exchange_Reject( pMsg ) ;				break;
	case MP_EXCHANGE_CANTAPPLY:				Exchange_Cantapply( pMsg ) ;			break;
	case MP_EXCHANGE_WAITING_CANCEL:		Exchange_Waiting_Cancel( pMsg ) ;		break;
	case MP_EXCHANGE_WAITING_CANCEL_ACK:	Exchange_Waiting_Cancel_Ack() ;			break;
	case MP_EXCHANGE_WAITING_CANCEL_NACK:	Exchange_Waiting_Cancel_Nack() ;		break;	
	case MP_EXCHANGE_START:					Exchange_Start() ;						break;
	case MP_EXCHANGE_ADDITEM_ACK:			Exchange_Additem_Ack( pMsg ) ;			break;
	case MP_EXCHANGE_ADDITEM_NACK:			Exchange_Additem_Nack( pMsg ) ;			break;
	case MP_EXCHANGE_ADDITEM:				Exchange_Additem( pMsg ) ;				break;		
	case MP_EXCHANGE_DELITEM_ACK:			Exchange_Delitem_Ack( pMsg ) ;			break;
	case MP_EXCHANGE_DELITEM_NACK:			Exchange_Delitem_Nack( pMsg ) ;			break;
	case MP_EXCHANGE_DELITEM:				Exchange_Delitem( pMsg ) ;				break;		
	case MP_EXCHANGE_INPUTMONEY_ACK:		Exchange_Inputmoney_Ack( pMsg ) ;		break;
	case MP_EXCHANGE_INPUTMONEY_NACK:		Exchange_Inputmoney_Nack() ;			break;
	case MP_EXCHANGE_INPUTMONEY:			Exchange_Inputmoney( pMsg ) ;			break;
	case MP_EXCHANGE_LOCK_ACK:				Exchange_Lock_Ack() ;					break;
	case MP_EXCHANGE_LOCK_NACK:				Exchange_Lock_Nack()  ;					break;
	case MP_EXCHANGE_LOCK:					Exchange_Lock() ;						break;
	case MP_EXCHANGE_UNLOCK_ACK:			Exchange_Unlock_Ack() ;					break;
	case MP_EXCHANGE_UNLOCK_NACK:			Exchange_Unlock_Nack() ;				break;
	case MP_EXCHANGE_EXCHANGE_ACK:													break;
	case MP_EXCHANGE_EXCHANGE:				Exchange_Exchange() ;					break;
	case MP_EXCHANGE_EXCHANGE_NACK:			Exchange_Exchange_Nack() ;				break;
	case MP_EXCHANGE_CANCEL_ACK:			Exchange_Cancel_Ack() ;					break;	
	case MP_EXCHANGE_CANCEL_NACK:			Exchange_Cancel_Nack() ;				break;
	case MP_EXCHANGE_CANCEL:				Exchange_Cancel( pMsg ) ;				break;
	case MP_EXCHANGE_SETMONEY:				Exchange_Setmoney( pMsg ) ;				break;		
	case MP_EXCHANGE_INSERT:				Exchange_Insert( pMsg ) ;				break;
	case MP_EXCHANGE_REMOVE:				Exchange_Remove( pMsg ) ;				break;
	}
}


void CExchangeManager::Exchange_Apply( void* pMsg ) 
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( msg->dwData );
	
	BOOL isOpen = FALSE;

	// 조합/인챈트/강화/분해 중인 경우 교환 불가
	{
		cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
		cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
		cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
		cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
		cDialog* fishingpointDialog = WINDOWMGR->GetWindowForID( FISHINGPOINTDLG );

		ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

		isOpen =	mixDialog->IsActive()		||
					enchantDialog->IsActive()	||
					reinforceDialog->IsActive()	||
					dissoloveDialog->IsActive() ||
					fishingpointDialog->IsActive();
	}

	if( pPlayer == NULL || m_IsExchanging == TRUE || isOpen )
	{
		MSGBASE msgReject;
		msgReject.Category		= MP_EXCHANGE;
		msgReject.dwObjectID	= gHeroID;
		msgReject.Protocol		= MP_EXCHANGE_CANTAPPLY;
		NETWORK->Send( &msgReject, sizeof(MSGBASE) );

		//에러메세지
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(456) );	//교환할 수 없는상태입니다.
		return ;
	}

	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);

	WINDOWMGR->MsgBox( MBI_EXCHANGEAPPLY, MBT_YESNO,
				CHATMGR->GetChatMsg(441), pPlayer->GetObjectName() );

	if( GAMEIN->GetExchangeDialog() )
		GAMEIN->GetExchangeDialog()->SetPlayerName( HERO->GetObjectName(), pPlayer->GetObjectName() );

	m_IsExchanging = TRUE;
}


void CExchangeManager::Exchange_Apply_Ack( void* pMsg ) 
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( msg->dwData );
	if( pPlayer == NULL )	//CHECK 교환상대를 찾을 수 없다?
	{
		//교환 신청을 수행하지 못하였습니다.
		ExchangeEnd();
		return ;
	}
	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(442), pPlayer->GetObjectName() );

	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_EXCHANGEWAITING, MBT_CANCEL, CHATMGR->GetChatMsg(457) );
	if( pMsgBox )
		pMsgBox->SetDefaultBtn(-1);	//디폴트 버튼 없다.
}


void CExchangeManager::Exchange_Apply_Nack() 
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(443) );
	ExchangeEnd();
}


void CExchangeManager::Exchange_Accept_Nack( void* pMsg ) 
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(446) );			
	ExchangeEnd();
}


void CExchangeManager::Exchange_Accept( void* pMsg ) 
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( msg->dwData );
	if( pPlayer == NULL ) return;

	ASSERT(pPlayer->GetObjectKind() == eObjectKind_Player);

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(444), pPlayer->GetObjectName() );

	if( GAMEIN->GetExchangeDialog() )
		GAMEIN->GetExchangeDialog()->SetPlayerName( HERO->GetObjectName(), pPlayer->GetObjectName() );

	cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEWAITING );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );
}


void CExchangeManager::Exchange_Reject( void* pMsg ) 
{
	ExchangeEnd();
	
	cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEWAITING );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(446) );
}


void CExchangeManager::Exchange_Cantapply( void* pMsg ) 
{
	ExchangeEnd();

	cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEWAITING );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(443) );
	//315 거리가 너무 멀어 교환할 수 없습니다.
}


void CExchangeManager::Exchange_Waiting_Cancel( void* pMsg ) 
{
	ExchangeEnd();
			
	cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEAPPLY );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );
	
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(446) );
}


void CExchangeManager::Exchange_Waiting_Cancel_Ack() 
{
	ExchangeEnd();

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(449) );
}


void CExchangeManager::Exchange_Waiting_Cancel_Nack() 
{
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(436) );
}


void CExchangeManager::Exchange_Start() 
{
	GAMEIN->GetExchangeDialog()->SetActive( TRUE );

	if( !GAMEIN->GetInventoryDialog()->IsActive() )
	{
		GAMEIN->GetInventoryDialog()->SetActive( TRUE );
		m_bInventoryActive = FALSE;
	}
	else
	{
		m_bInventoryActive = TRUE;
	}
}


void CExchangeManager::Exchange_Additem_Ack( void* pMsg ) 
{
	MSG_LINKITEM* msg = (MSG_LINKITEM*)pMsg;

	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );		//FAKE MOVE에서 TRUE
	CItem* pItem = (CItem*)GAMEIN->GetInventoryDialog()->GetItemForPos( msg->ItemInfo.Position );
	ASSERT( pItem );
	if( !pItem ) return ;

	CExchangeItem* pExItem = new CExchangeItem;

	// 071215 LYW --- ExchangeManager : 아이템의 봉인여부를 세팅한다.
	if( msg->ItemInfo.nSealed  == eITEM_TYPE_SEAL )
	{
		pExItem->SetSeal( TRUE ) ;
	}
	else
	{
		pExItem->SetSeal( FALSE ) ;
	}
						
	//Init바꾸는게 낫겠다.
	pExItem->InitItem( pItem, msg->wAbsPosition, m_IconIndexCreator.GenerateIndex() );
	pExItem->SetType( WT_EXCHANGEITEM );
	pExItem->SetDurability( msg->ItemInfo.Durability );
//---KES 071205 사기 방지를 위해 아이템을 교환창에서 지울 수 없도록 수정
	pExItem->SetMovable( FALSE );
//--------------------------------

	WINDOWMGR->AddWindow( pExItem );

	// 071126 LYW --- ExchangeManager : 교환 아이템의 봉인 여부 표시 처리.
	const ITEMBASE& pItemBase = pItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed == eITEM_TYPE_SEAL )
	{
		pExItem->SetSeal(TRUE) ;
	}

	GAMEIN->GetExchangeDialog()->AddItem( 0, msg->wAbsPosition, pExItem );

	pItem->SetLinkPosition(
		POSTYPE(m_nLinkedItemNum));
	pItem->SetLinkItem( pExItem );

	//인벤토리 체크
	int nBlankNum = GAMEIN->GetInventoryDialog()->GetBlankNum();

	if( nBlankNum < (int)msg->wAbsPosition - EXCHANGEMGR->GetLinkedItemNum() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(454) );
//		m_bExchangable = FALSE;
	}
}


void CExchangeManager::Exchange_Additem_Nack( void* pMsg )
{
	MSG_LINKITEM* msg = (MSG_LINKITEM*)pMsg;
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );		//FAKE MOVE에서 TRUE

	--m_nLinkedItemNum;

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( msg->ItemInfo.Position );
	if( !pItem ) return ;

	// 071125 LYW --- ExchangeDialog : 봉인 해제 된 아이템 교환 불가 처리 추가.
	const ITEMBASE& pItemBase = pItem->GetItemBaseInfo() ;

	if( pItemBase.nSealed == eITEM_TYPE_UNSEAL )
	{
		WINDOWMGR->MsgBox(MBI_SEALITEM_EXCHANGE, MBT_OK, CHATMGR->GetChatMsg(1174));

		return ;
	}

	pItem->SetLock( FALSE );			
}


void CExchangeManager::Exchange_Additem( void* pMsg )
{
	//			MSG_LINKITEM* msg = (MSG_LINKITEM*)pMsg;
	MSG_LINKITEM_ADD* msg = (MSG_LINKITEM_ADD*)pMsg;

	CExchangeItem* pItem = new CExchangeItem;
	cImage tmpImage;
	pItem->Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, 
					ITEMMGR->GetIconImage( msg->ItemInfo.wIconIdx, &tmpImage ), m_IconIndexCreator.GenerateIndex() );

	pItem->SetType( WT_EXCHANGEITEM );
	pItem->SetDurability( msg->ItemInfo.Durability );
	pItem->SetMovable( FALSE );
	pItem->SetData( msg->ItemInfo.wIconIdx );
	pItem->SetDBId( msg->ItemInfo.dwDBIdx );
	pItem->SetSeal( msg->ItemInfo.nSealed  == eITEM_TYPE_SEAL );

	ITEMMGR->AddOption(msg->sOptionInfo);
	ITEMMGR->AddToolTip( pItem ) ;
	WINDOWMGR->AddWindow( pItem );

	// 071126 LYW --- ExchangeManager : 교환 아이템의 봉인 여부 표시 처리.
	if( msg->ItemInfo.nSealed == eITEM_TYPE_SEAL )
	{
		pItem->SetSeal(TRUE) ;
	}

	GAMEIN->GetExchangeDialog()->AddItem( 1, msg->wAbsPosition, pItem );
}


void CExchangeManager::Exchange_Delitem_Ack( void* pMsg ) 
{
	MSG_WORD* msg = (MSG_WORD*)pMsg;
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
	GAMEIN->GetExchangeDialog()->DelItem( 0, msg->wData );
	--m_nLinkedItemNum;
}


void CExchangeManager::Exchange_Delitem_Nack( void* pMsg )
{
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
}


void CExchangeManager::Exchange_Delitem( void* pMsg ) 
{
	MSG_WORD* msg = (MSG_WORD*)pMsg;

	//옵션이 있으면 지워준다.
	GAMEIN->GetExchangeDialog()->DelItemOptionInfo( msg->wData );
	//GAMEIN->GetExchangeDialog()->DelItemRareOptionInfo( msg->wData );

	GAMEIN->GetExchangeDialog()->DelItem( 1, msg->wData );			
}


void CExchangeManager::Exchange_Inputmoney_Ack( void* pMsg )
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	GAMEIN->GetExchangeDialog()->InputMoney( 0, msg->dwData );
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
}


void CExchangeManager::Exchange_Inputmoney_Nack()
{
	//"돈입력에 실패하였습니다."
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
	GAMEIN->GetExchangeDialog()->MoneyInputButtonEnable( TRUE );
}


void CExchangeManager::Exchange_Inputmoney( void* pMsg ) 
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	GAMEIN->GetExchangeDialog()->InputMoney( 1, msg->dwData );
}


void CExchangeManager::Exchange_Lock_Ack() 
{
	m_bLockSyncing = FALSE;
	GAMEIN->GetExchangeDialog()->MyLock( TRUE );	//이미 했으나 한번더 확인
	GAMEIN->GetExchangeDialog()->MoneyInputButtonEnable( FALSE );
}


void CExchangeManager::Exchange_Lock_Nack() 
{
	m_bLockSyncing = FALSE;
	GAMEIN->GetExchangeDialog()->MyLock( FALSE );
}


void CExchangeManager::Exchange_Lock()
{
	GAMEIN->GetExchangeDialog()->OpLock( TRUE );

	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_EXCHANGE_MONEY );
	if( pDlg )
	{
		WINDOWMGR->AddListDestroyWindow( pDlg );

		GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
	}

	GAMEIN->GetExchangeDialog()->MoneyInputButtonEnable( FALSE );
}


void CExchangeManager::Exchange_Unlock_Ack() 
{
	m_bLockSyncing = FALSE;			
	GAMEIN->GetExchangeDialog()->MyLock( FALSE );
}


void CExchangeManager::Exchange_Unlock_Nack()
{
	m_bLockSyncing = FALSE;
	GAMEIN->GetExchangeDialog()->MyLock( TRUE );
}

void CExchangeManager::Exchange_Exchange() 
{
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
	GAMEIN->GetExchangeDialog()->SetActive( FALSE );			
	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );

	//돈입력버튼 지우기.	CONFIRM
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_EXCHANGE_MONEY );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );


	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(445) );

	ExchangeEnd();
}


void CExchangeManager::Exchange_Exchange_Nack()
{
	ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
}


void CExchangeManager::Exchange_Cancel_Ack()
{
	////////아이템 옵션 제거 Active False 전에 할것
	GAMEIN->GetExchangeDialog()->DelItemOptionInfoAll();
	//GAMEIN->GetExchangeDialog()->DelItemRareOptionInfoAll();

	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
	GAMEIN->GetExchangeDialog()->SetActive( FALSE );

	if( !m_bInventoryActive )
	{
		ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
		GAMEIN->GetInventoryDialog()->SetActive( FALSE );
	}

	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_EXCHANGE_MONEY );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(449) );

	ExchangeEnd();
}


void CExchangeManager::Exchange_Cancel_Nack()
{
	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );
}


void CExchangeManager::Exchange_Cancel( void* pMsg ) 
{
	MSG_BYTE* msg = (MSG_BYTE*)pMsg;

	GAMEIN->GetExchangeDialog()->SetDisable( FALSE );	//체크..임시.
	GAMEIN->GetInventoryDialog()->SetDisable( FALSE );

	//돈입력버튼 지우기.	CONFIRM
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_EXCHANGE_MONEY );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );
	

	////////아이템 옵션 제거 : Active False 전에 할것.
	GAMEIN->GetExchangeDialog()->DelItemOptionInfoAll();
	//GAMEIN->GetExchangeDialog()->DelItemRareOptionInfoAll();

	//교환이 취소되었때와 교환허락을 하지 않았을시...
	GAMEIN->GetExchangeDialog()->SetActive( FALSE );
	if( !m_bInventoryActive )
		GAMEIN->GetInventoryDialog()->SetActive( FALSE );
	
	//신청메세지들 지우기....
	
	pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEAPPLY);
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );

	pDlg = WINDOWMGR->GetWindowForID( MBI_EXCHANGEWAITING );
	if( pDlg )
		WINDOWMGR->AddListDestroyWindow( pDlg );			

//error msg 출력
	switch( msg->bData )
	{
		case eEE_USERCANCEL:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(446) );
			break;
		case eEE_USERLOGOUT:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(434) );
			break;
		case eEE_USERDIE:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(433) );
			break;
		case eEE_DIE:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(432) );
			break;
		case eEE_NOTENOUGHMONEY:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(431) );
			break;
		case eEE_NOTENOUGHSPACE:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(430) );
			break;
		case eEE_MAXMONEY:
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(429) );
			break;
		default:
			break;
	}

	ExchangeEnd();
}


void CExchangeManager::Exchange_Setmoney( void* pMsg )
{
	MSG_DWORD* msg = (MSG_DWORD*)pMsg;
	HERO->SetMoney( msg->dwData );	//+인벤토리
}


void CExchangeManager::Exchange_Insert( void* pMsg ) 
{
	MSG_ITEM* msg = (MSG_ITEM*)pMsg;
	BOOL rt = GAMEIN->GetInventoryDialog()->AddItem(&msg->ItemInfo);
	ASSERT(rt);
	//아이템을 넣을때 다시 옵션을 넣는다.?

	QUICKMGR->RefreshQickItem();
}


void CExchangeManager::Exchange_Remove( void* pMsg ) 
{
	MSG_EXCHANGE_REMOVEITEM* msg = (MSG_EXCHANGE_REMOVEITEM*)pMsg;
	CItem* pOutItem = NULL;
	ITEMMGR->DeleteItem( msg->wAbsPosition, &pOutItem );

	//옵션을 가진 아이템이었다면 옵션이 지워진다. 자동으로.
}



