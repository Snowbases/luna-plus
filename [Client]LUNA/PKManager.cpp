// PKManager.cpp: implementation of the CPKManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PKManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "ChatManager.h"
#include "./Interface/cWindowManager.h"
#include "WindowIdEnum.h"
#include "cMsgBox.h"
#include "ShowdownManager.h"
#include "PKLootingDialog.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "QuickManager.h"
#include "cSkillTreeManager.h"
#include "ObjectStateManager.h"
#include "MainGame.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBALTON(CPKManager)

CPKManager::CPKManager()
{
	m_dwPKContinueTime	= 20*60*1000;	//---KES 30분에서 20분으로 기획 수정 : 2008.1.4
	m_dwPKModeStartTime	= gCurTime;
	m_htLootingDlg.Initialize( 3 );
}

CPKManager::~CPKManager()
{
}

void CPKManager::Init()
{
	m_IndexGenerator.Init(1000, IG_PKLOOTINGDLG_START);	
	m_bSyncing			= FALSE;
	m_bLooted			= FALSE;
	m_bLooting			= FALSE;
	m_bPKAllow			= FALSE;
}

void CPKManager::Release()
{
	m_htLootingDlg.RemoveAll();	//window new한건 windowmanager가 지워준다
	m_IndexGenerator.Release();
}

BOOL CPKManager::HEROPKModeOn()
{
	if( !IsPKAllow() && MAINGAME->GetUserLevel() > eUSERLEVEL_GM )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1199 ) );
		return FALSE;
	}		

	if( m_bSyncing )			return FALSE;
	if( !HERO )					return FALSE;
	if( HERO->IsPKMode() )		return FALSE;	//이미 on상태이다.
	if( WINDOWMGR->GetWindowForID( MBI_PKMODE ) ) return FALSE;		//이미 창을 띄웠다.

	cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_PKMODE, MBT_YESNO, CHATMGR->GetChatMsg( 1197 ) );		
	if( !pMsgBox ) return FALSE;

	return TRUE;
}

void CPKManager::SendMsgPKOn()
{
/*	
	if( HERO->GetState() != eObjectState_None && HERO->GetState() != eObjectState_Move )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 557 ) );
		return;			//보통상태에서만 가능
	}
*/

	if( HERO->GetState() == eObjectState_Die )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1228 ) );
		return;			//죽은상태만 아니면
	}

	if( SHOWDOWNMGR->IsShowdown() )	//비무중엔 안된다.
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1228 ) );
		return;
	}

	MSGBASE msg;
	msg.Category	= MP_PK;
	msg.Protocol	= MP_PK_PKON_SYN;
	msg.dwObjectID	= gHeroID;
	NETWORK->Send( &msg, sizeof(msg) );

	m_bSyncing		= TRUE;
}

BOOL CPKManager::HEROPKModeOff()
{
	if( m_bSyncing )		return FALSE;
	if( !HERO )				return FALSE;
	if( !HERO->IsPKMode() )	return FALSE;	//이미 off상태이다.

	if( gCurTime - m_dwPKModeStartTime >= m_dwPKContinueTime )
	{
		MSGBASE msg;
		msg.Category	= MP_PK;
		msg.Protocol	= MP_PK_PKOFF_SYN;
		msg.dwObjectID	= gHeroID;
		NETWORK->Send( &msg, sizeof(msg) );

		m_bSyncing		= TRUE;
		
		return TRUE;
	}
	else
	{
		int nRemain = (int)( m_dwPKContinueTime - (gCurTime - m_dwPKModeStartTime) ) / 1000 + 1	;//+1은 버퍼
		if( nRemain < 0 ) nRemain = 0;

		int nMinutes = nRemain / 60;
		nRemain -= nMinutes * 60;
        
		if( nMinutes == 0 && nRemain == 0 ) nRemain = 1;
//		else if( nRemain > 60 ) nRemain = 60;//나중에 상태에따라 초가 늘어나야한다.

//		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1206 ), nRemain + 1);

		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1206 ), nMinutes, nRemain);

		if( MAINGAME->GetUserLevel() <= eUSERLEVEL_GM )
		{
			MSGBASE msg;
			msg.Category	= MP_PK;
			msg.Protocol	= MP_PK_PKOFF_SYN;
			msg.dwObjectID	= gHeroID;
			NETWORK->Send( &msg, sizeof(msg) );

			m_bSyncing		= TRUE;

			return TRUE ;
		}
	}

	return FALSE;
}

void CPKManager::SetPlayerPKMode( CPlayer* pPlayer, BOOL bPKMode )
{
	pPlayer->SetPKMode( bPKMode );
}

BOOL CPKManager::ToggleHeroPKMode()
{
	if( HERO )
	if( HERO->IsPKMode() )
	{
		return HEROPKModeOff();
	}
	else
	{
		return HEROPKModeOn();
	}

	return FALSE;

}

void CPKManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_PK_PKON_ACK:				Pk_PkOn_Ack( pMsg ) ;				break;
	case MP_PK_PKON_NACK:				Pk_PkOn_Nack( pMsg ) ;				break;
	case MP_PK_PKOFF_ACK:				Pk_PkOff_Ack( pMsg ) ;				break;
	case MP_PK_DISAPPEAR:				Pk_Disappear( pMsg ) ;				break;
	case MP_PK_PKOFF_NACK:				Pk_PkOff_Nack() ;					break;
	case MP_PK_LOOTING_START:			Pk_Looting_Start( pMsg ) ;			break;
	case MP_PK_LOOTING_BEINGLOOTED:		Pk_Looting_BeingLooted( pMsg ) ;	break;
	case MP_PK_LOOTING_ITEMLOOTING:		Pk_Looting_ItemLooting( pMsg ) ;	break;
	case MP_PK_LOOTING_ITEMLOOTED:		Pk_Looting_ItemLooted( pMsg ) ;		break;
	case MP_PK_LOOTING_MOENYLOOTING:	Pk_Looting_MoenyLooting( pMsg ) ;	break;
	case MP_PK_LOOTING_MOENYLOOTED:		Pk_Looting_MoenyLooted( pMsg ) ;	break;
	case MP_PK_LOOTING_EXPLOOTING:		Pk_Looting_ExpLooting( pMsg ) ;		break;
	case MP_PK_LOOTING_EXPLOOTED:		Pk_Looting_ExpLooted( pMsg ) ;		break;
	case MP_PK_LOOTING_NOLOOTING:		Pk_Looting_NoLooting( pMsg ) ;		break;		
	case MP_PK_LOOTING_NOINVENSPACE:	Pk_Looting_NoInvenspace( pMsg ) ;	break;
	case MP_PK_LOOTING_ENDLOOTING:		Pk_Looting_EndLooting( pMsg ) ;		break;
	case MP_PK_DESTROY_ITEM:			Pk_Destroy_Item( pMsg ) ;			break;
	case MP_PK_LOOTING_ERROR:			Pk_Looting_Error( pMsg ) ;			break;
	case MP_PK_PKALLOW_CHANGED:			Pk_Pkallow_Changed( pMsg ) ;		break;
	case MP_PK_EXPPENALTY:
		{
			// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
			//MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
			MSG_DWORDEX2* pmsg = (MSG_DWORDEX2*)pMsg;

			if( pmsg->dweData2 == 0 )
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1237 ), (float)pmsg->dweData1 );	//---KES PK
			else
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1238 ), (float)pmsg->dweData1 );	//---KES PK
		}
		break;
	}
}


//--------------
//인터페이스 관련
CPKLootingDialog* CPKManager::CreateLootingDlg( DWORD dwDiePlayerIdx )
{
	//매번 파일에서 읽어온다...?
	CPKLootingDialog* pLootDlg = (CPKLootingDialog*)WINDOWMGR->GetDlgInfoFromFile( "./Data/Interface/Windows/PKLootDlg.bin", "rb" );
	pLootDlg->Linking();

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	LONG x = ( dispInfo.dwWidth - 251 ) / 2;
	LONG y = ( dispInfo.dwHeight - 275 ) / 2;
	// 070202 LYW --- End.

	//표지션이 겹치는 dlg가 있으면 위치조정
	PositioningDlg( x, y );

	pLootDlg->InitPKLootDlg( AllocWindowId(), x, y, dwDiePlayerIdx );
	WINDOWMGR->AddWindow( pLootDlg );
	
	pLootDlg->SetActive(TRUE);


//이미 존재하는가 한번 보자	//반드시 이위치. 위에서 먼저 검사하면 안된다. 윈도우 아이디문제.
	CPKLootingDialog* pLootTempDlg = m_htLootingDlg.GetData( dwDiePlayerIdx );
	if( pLootTempDlg )
		CloseLootingDialog( pLootTempDlg, FALSE );

	m_htLootingDlg.Add( pLootDlg, dwDiePlayerIdx );

	return pLootDlg;
}

void CPKManager::PositioningDlg( LONG& rX, LONG& rY )
{
	int nCount = 0;
	m_htLootingDlg.SetPositionHead();

	for(CPKLootingDialog* pLootDlg = m_htLootingDlg.GetData();
		0 < pLootDlg;
		pLootDlg = m_htLootingDlg.GetData())
	{
		if( (LONG)pLootDlg->GetAbsX() == rX )
		if( (LONG)pLootDlg->GetAbsY() == rY )
		{
			rX += 20;
			rY += 20;
			if( ++nCount >= 10 )//한없이 내려가는것 방지
			{
				rX -= 200;
				rY -= 200;
				nCount = 0;
			}
		}
	}
}


void CPKManager::CloseLootingDialog( CPKLootingDialog* pLootingDlg, BOOL bMsgToServer )
{
	pLootingDlg->SetDisable( FALSE );		//^^ disable인건 acitve가 안먹어.
	pLootingDlg->SetActiveRecursive( FALSE );
	pLootingDlg->ReleaseAllIcon();
	pLootingDlg->SetDisable( TRUE );
	m_htLootingDlg.Remove( pLootingDlg->GetDiePlayerIdx() );
	FreeWindowId( pLootingDlg->GetID() );
	WINDOWMGR->AddListDestroyWindow( pLootingDlg );

	if( m_htLootingDlg.GetDataNum() == 0 )
	{
		SetPKLooting( FALSE );	//서버와 맞추어야 할텐데
	}

	if( bMsgToServer )
	{
		MSG_DWORD	msg;
		msg.Category	= MP_PK;
		msg.Protocol	= MP_PK_LOOTING_ENDLOOTING;
		msg.dwObjectID	= HEROID;
		msg.dwData		= pLootingDlg->GetDiePlayerIdx();

		NETWORK->Send( &msg, sizeof(msg) );
	}
}

int CPKManager::GetLootingChance( DWORD dwBadFame )
{
	if( dwBadFame < 100000 )
		return 3;
	else if( dwBadFame < 500000 )
		return 4;
	else if( dwBadFame < 1000000 )
		return 5;
	else if( dwBadFame < 5000000 )
		return 6;
	else if( dwBadFame < 10000000 )
		return 7;
	else if( dwBadFame < 50000000 )
		return 8;
	else if( dwBadFame < 100000000 )
		return 9;
	else
		return 10;
}

int CPKManager::GetLootingItemNum( DWORD dwBadFame )
{
	if( dwBadFame < 100000000 )
		return 1;
	else if( dwBadFame < 400000000 )
		return 2;
	else if( dwBadFame < 700000000 )
		return 3;
	else if( dwBadFame < 1000000000 )
		return 4;
	else
		return 5;
}

/*
void CPKManager::RemoveAllLootingDlg()
{
	m_htLootingDlg.SetPositionHead();

	CPKLootingDialog* pLootDlg = NULL;
	while( pLootDlg = m_htLootingDlg.GetData() )
	{
		WINDOWMGR->AddListDestroyWindow( pLootDlg );	//delete는 매니져가 해준다
		
	}
}
*/


void CPKManager::Pk_PkOn_Ack( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData );
	if( !pPlayer ) return ;

	// 090116 ShinJS --- PK모드를 적용 후 옵션을 적용해야 적을 판단하기 때문에 먼저 실행되도록 함
	pPlayer->SetPKMode( TRUE );

	if( HERO )
	if( pmsg->dwData == HEROID )
	{
		m_bSyncing			= FALSE;
		m_dwPKModeStartTime	= gCurTime;
		m_dwPKContinueTime	= 20*60*1000 + ( HERO->GetBadFame() / 75 ) * 5*60*1000; //---KES PK 071124 수치조정

		HERO->DisableAutoAttack();	//자동 공격 취소
		HERO->SetNextAction(NULL);	//스킬 취소
		if( HERO->GetState() == eObjectState_Immortal )
			OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Immortal );

		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1202 ) );
		
		// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 적용을 위해 실행
		OBJECTMGR->ApplyOverInfoOptionToAll();
	}
	
	pPlayer->SetPKMode( TRUE );

	//캐릭터 오버인포 바꾸기
	OBJECTMGR->ApplyOverInfoOption( pPlayer );
	//OBJECTEFFECTDESC desc(FindEffectNum("maintain_PK_S.beff"));
	OBJECTEFFECTDESC desc( 39 );	//---KES PK
	//SW050915 살기는 이펙트 옵션에 상관없이 항상 보이기
	//pPlayer->AddObjectEffect( PK_EFFECT_ID, &desc, 1 );
	pPlayer->AddObjectEffect( PK_EFFECT_ID, &desc, 1, HERO);	//HERO 는 MakeEffectUnit( 에서 보여주기위한 하드코딩
}


void CPKManager::Pk_PkOn_Nack( void* pMsg ) 
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
	m_bSyncing		= FALSE;

	switch( pmsg->bData )
	{
	case ePKCODE_SHOWDOWN:
	case ePKCODE_STATECONFLICT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1228 ) );
		break;
	case ePKCODE_NOTALLAW:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1199 ) );
		break;
	}
}


void CPKManager::Pk_PkOff_Ack( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData );
	if( !pPlayer ) return ;

	pPlayer->SetPKMode( FALSE );
	
	if( HERO )
	if( pmsg->dwData == gHeroID )
	{
		m_bSyncing		= FALSE;
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1203 ) );

		// 090116 ShinJS --- 캐릭터 안보이기 옵션에 대한 적용을 위해 실행
		OBJECTMGR->ApplyOverInfoOptionToAll();
	}
	else
	{
		//현재 공격타겟이 pk를 킨 유저라면 타겟을 없애자.
		HERO->ClearTarget( pPlayer );				
	}

	//캐릭터 오버인포 바꾸기
	OBJECTMGR->ApplyOverInfoOption( pPlayer );
	pPlayer->RemoveObjectEffect( PK_EFFECT_ID );
}


void CPKManager::Pk_Disappear( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData );
	if( !pPlayer ) return ;

	pPlayer->SetPKMode( FALSE );
	
	if( HERO )
	if( pmsg->dwData == gHeroID )
	{
		//m_bSyncing		= FALSE;
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1203 ) );
	}
	else
	{
		//현재 공격타겟이 pk를 킨 유저라면 타겟을 없애자.
		HERO->ClearTarget( pPlayer );
	}

	//캐릭터 오버인포 바꾸기
	OBJECTMGR->ApplyOverInfoOption( pPlayer );
	pPlayer->RemoveObjectEffect( PK_EFFECT_ID );
}


void CPKManager::Pk_PkOff_Nack() 
{
	//시간이안되었다.
	m_bSyncing		= FALSE;
}


void CPKManager::Pk_Looting_Start( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CreateLootingDlg( pmsg->dwData );
	SetPKLooting( TRUE );
}


void CPKManager::Pk_Looting_BeingLooted( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CPlayer* pAttacker = (CPlayer*)OBJECTMGR->GetObject( pmsg->dwData );
	if( pAttacker )
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1229 ), pAttacker->GetObjectName() );

	SetPKLooted( TRUE );
}


void CPKManager::Pk_Looting_ItemLooting( void* pMsg ) 
{
	MSG_LOOTINGIEM* pmsg = (MSG_LOOTINGIEM*)pMsg;
	
	ITEMMGR->AddOption( pmsg->sOptionInfo );
	
	CItem* pItem = ITEMMGR->MakeNewItem(&pmsg->ItemInfo,"MP_PK_LOOTING_ITEMLOOTING");
	GAMEIN->GetInventoryDialog()->AddItem(pItem);
	QUICKMGR->RefreshQickItem();

	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwDiePlayerIdx );
	if( pDlg )
	{
		pDlg->ChangeIconImage( pmsg->wAbsPosition, eLIK_ITEM, pmsg->ItemInfo.wIconIdx );
		pDlg->AddLootingItemNum();
		pDlg->SetMsgSync( FALSE );
	}

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1230 ), 
		ITEMMGR->GetItemInfo(pmsg->ItemInfo.wIconIdx)->ItemName );

	ITEMMGR->ItemDropEffect( pmsg->ItemInfo.wIconIdx );
}


void CPKManager::Pk_Looting_ItemLooted( void* pMsg ) 
{
	MSG_EXCHANGE_REMOVEITEM* pmsg = (MSG_EXCHANGE_REMOVEITEM*)pMsg;
	const eITEMTABLE wTableIdx = ITEMMGR->GetTableIdxForAbsPos( pmsg->wAbsPosition );
	const ITEMBASE* pItemInfo = ITEMMGR->GetItemInfoAbsIn( HERO, pmsg->wAbsPosition );
	ITEMMGR->DeleteItemofTable( wTableIdx, pmsg->wAbsPosition, FALSE );
	QUICKMGR->RefreshQickItem();
	
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1231 ), 
		ITEMMGR->GetItemInfo(pItemInfo->wIconIdx)->ItemName );
}


void CPKManager::Pk_Looting_MoenyLooting( void* pMsg ) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwData1 );
	if( pDlg )
	{
		pDlg->ChangeIconImage( (WORD)pmsg->dwData2, eLIK_MONEY );
		pDlg->AddLootingItemNum();
		pDlg->SetMsgSync( FALSE );
	}

	CHATMGR->AddMsg(
		CTC_SYSMSG,
		CHATMGR->GetChatMsg(1232),
		AddComma(pmsg->dwData3));
	EFFECTMGR->StartHeroEffectProcess(
		eEffect_GetMoney);
}


void CPKManager::Pk_Looting_MoenyLooted( void* pMsg ) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;

	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1233 ), AddComma( pmsg->dwData3 ) );
}


void CPKManager::Pk_Looting_ExpLooting( void* pMsg ) 
{
	// 080602 LYW --- Player : 경험치 수치 (__int32) 에서 (__int64) 사이즈로 변경 처리.
	//MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	MSG_DWORDEX3* pmsg = (MSG_DWORDEX3*)pMsg;

	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( (DWORD)pmsg->dweData1 );
	if( pDlg )
	{
		pDlg->ChangeIconImage( (WORD)pmsg->dweData2, eLIK_EXP );
		pDlg->AddLootingItemNum();
		pDlg->SetMsgSync( FALSE );
	}
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1234 ), (float)pmsg->dweData3 );

	//EFFECTMGR->StartHeroEffectProcess(FindEffectNum("m_ba_080.beff")); --> 머지? 경험치 획득?
}


void CPKManager::Pk_Looting_ExpLooted( void* pMsg ) 
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1235 ), pmsg->dwData3 );
}


void CPKManager::Pk_Looting_NoLooting( void* pMsg )
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwData1 );
	if( pDlg )
	{
		pDlg->ChangeIconImage( (WORD)pmsg->dwData2, eLIK_NONE );	//빈것으로
		pDlg->SetMsgSync( FALSE );
	}
}


void CPKManager::Pk_Looting_NoInvenspace( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1236 ) );
	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwData );
	if( pDlg )
		pDlg->SetMsgSync( FALSE );
}


void CPKManager::Pk_Looting_EndLooting( void* pMsg ) 
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
	if( pmsg->dwData == HEROID )
	{
		SetPKLooted( FALSE );
	}
	else
	{
		CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwData );
		if( pDlg )
		{
			pDlg->SetLootingEnd( TRUE );
			CloseLootingDialog( pDlg, FALSE );
		}
	}
}


void CPKManager::Pk_Destroy_Item( void* pMsg ) 
{
	MSG_ITEM_DESTROY* pmsg = (MSG_ITEM_DESTROY*)pMsg;
	const eITEMTABLE wTableIdx = ITEMMGR->GetTableIdxForAbsPos( pmsg->wAbsPosition );
	const ITEMBASE* pItemInfo = ITEMMGR->GetItemInfoAbsIn( HERO, pmsg->wAbsPosition );

	if( pItemInfo == NULL ) return;
	ITEMMGR->DeleteItemofTable( wTableIdx, pmsg->wAbsPosition, FALSE );
	QUICKMGR->RefreshQickItem();
	
	ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItemInfo->wIconIdx );
	if( pInfo )
	{
		if( pmsg->cbReason == 0 )
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1236 ), pInfo->ItemName );
//		else
//			CHATMGR->AddMsg( CTC_SYSMSG, "악행으로 현상범에 등록되어 있어 아이템[%s]이(가) 파괴 되었습니다.", pInfo->ItemName );
	}
}


void CPKManager::Pk_Looting_Error( void* pMsg ) 
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
	CPKLootingDialog* pDlg = m_htLootingDlg.GetData( pmsg->dwData1 );
	if( pDlg )
		pDlg->SetMsgSync( FALSE );

	//에러
	switch( pmsg->dwData2 )
	{
	case eLOOTINGERROR_OVER_DISTANCE:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1241 ) ); //거리가 너무 멀어.
		break;
	default:
		break;
	}
}


void CPKManager::Pk_Pkallow_Changed( void* pMsg ) 
{
	MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
	PKMGR->SetPKAllow( (BOOL)pmsg->bData );

	//모든 플레이어의 PK모드 끄기.
	if( PKMGR->IsPKAllow() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1200 ) );
	}
	else
	{
		OBJECTMGR->AllPlayerPKOff();
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1201 ) );
	}
}

