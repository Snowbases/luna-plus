// ExitManager.cpp: implementation of the CExitManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExitManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "MoveManager.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "cMsgBox.h"
#include "MainGame.h"
#include "PKManager.h"
#include "MHTimeManager.h"
extern HWND _g_hWnd;

// 070122 LYW --- Include header file.
#include "cResourceManager.h"
#include "StreetStallManager.h"
#include "ItemManager.h"

GLOBALTON(CExitManager);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExitManager::CExitManager()
{
	m_bExitProcessing	= FALSE;
	m_dwStartTime		= 0;
	m_nExitCount		= EXIT_COUNT;
	m_nCount			= 0;
	m_bExitSync			= FALSE;
}

CExitManager::~CExitManager()
{

}


int CExitManager::CanExit()
{
	if( HERO->IsPKMode() )	//PK모드중엔 종료할 수 없습니다.
		return eEXITCODE_PKMODE;

	if( PKMGR->IsPKLooted() )	//PK루팅을 당하는 중에는 종료할 수 없다.
		return eEXITCODE_LOOTING;

	if( HERO->GetState() == eObjectState_Exchange )	//교환중엔 종료할 수 없다.
		return eEXITCODE_NOT_ALLOW_STATE;

	if( HERO->GetState() == eObjectState_StreetStall_Owner ||
		HERO->GetState() == eObjectState_StreetStall_Guest )	//노점중엔 종료할 수 없다.
		return eEXITCODE_NOT_ALLOW_STATE;

	if( HERO->GetState() == eObjectState_Deal )	//상점 이용중엔 종료할 수 없다.
		return eEXITCODE_NOT_ALLOW_STATE;

	return eEXITCODE_OK;
}


BOOL CExitManager::SendExitMsg( int nExitKind )
{
	if( m_bExitSync || m_bExitProcessing ) return FALSE;

	m_nExitKind		= nExitKind;
	m_bExitSync		= TRUE;

////
	if( HERO->GetState() == eObjectState_StreetStall_Owner )
	{
		STREETSTALLMGR->CloseStreetStall();
	}
	HERO->DisableAutoAttack();					//자동 공격 취소
	HERO->SetNextAction(NULL);					//스킬 취소
	MOVEMGR->HeroMoveStop();					//이동멈춤
	OBJECTSTATEMGR->StartObjectState( HERO, eObjectState_Exit );
////
	
	MSGBASE msg;
	msg.Category	= MP_CHAR;
	msg.Protocol	= MP_CHAR_EXITSTART_SYN;
	msg.dwObjectID	= HEROID;	
	NETWORK->Send( &msg, sizeof(msg) );
	
	return TRUE;
}

void CExitManager::StartExit()	//Start_Ack
{
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(TRUE, eItemTable_Storage);


	//m_bExitSync		= FALSE;	//여기선 false로 만들지 않는다. 카운트끝나고씀
	m_bExitProcessing	= TRUE;
	DWORD dwCurTime = MHTIMEMGR->GetNewCalcCurTime();
	m_dwStartTime		= dwCurTime - 1000;	//시작하자마자 카운트를 하기위해서 -1000
	m_nCount			= m_nExitCount;
}

void CExitManager::RejectExit( int nExitCode )	//Start_Nack
{
	m_bExitSync			= FALSE;
	OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Exit );

	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);

	switch( nExitCode )
	{
	case eEXITCODE_PKMODE:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(552) );	// "살기를 드러낸 상태에서는 종료할 수 없습니다."
		break;
	case eEXITCODE_LOOTING:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(553) );	//"루팅을 당하는 중에는 종료할 수 없습니다."
		break;
	default:
		// 070622 LYW --- GlobalEventFunc : 메시지 번호 수정.
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );	//"종료할 수 없는 상태입니다."
		break;
	}
	
}

void CExitManager::Exit()
{
	if( m_nExitKind == eEK_GAMEEXIT )
	{
//		MAINGAME->SetGameState( eGAMESTATE_END );
		WINDOWMGR->MsgBox( MBI_NOBTNMSGBOX, MBT_NOBTN, CHATMGR->GetChatMsg( 307 ) );
		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
	}
	else if( m_nExitKind == eEK_CHARSEL )
	{
		MSGBASE data;
		data.Category	= MP_USERCONN;
		data.Protocol	= MP_USERCONN_BACKTOCHARSEL_SYN;
		data.dwObjectID	= HEROID;
		NETWORK->Send(&data,sizeof(data));

		WINDOWMGR->MsgBox(
			MBI_NOBTNMSGBOX,
			MBT_NOBTN,
			RESRCMGR->GetMsg(262));
	}
	
	m_bExitSync = FALSE;	//확실하게 하려면....?
}

void CExitManager::CancelExit( int nErrorCode )	//죽거나 기타요인에 의해서 취소됨
{
	m_bExitProcessing	= FALSE;
	m_bExitSync			= FALSE;
	OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Exit );

	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Inventory);
	ITEMMGR->SetDisableDialog(FALSE, eItemTable_Storage);

	switch( nErrorCode )
	{
	case eEXITCODE_PKMODE:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(552) );
		break;
	case eEXITCODE_LOOTING:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(553) );
		break;
	case eEXITCODE_SPEEDHACK:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(556) );
		break;
	case eEXITCODE_DIEONEXIT:
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(555) );
		break;
	default:
		// 070622 LYW --- GlobalEventFunc : 메시지 번호 수정.
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );
		break;
	}	
}

void CExitManager::Process()
{
	if( !m_bExitProcessing || !m_bExitSync ) return;

	if( gCurTime - m_dwStartTime >= 1000 )	//1초마다 체크
	{
		m_dwStartTime += 1000;
		
		if( m_nCount <= 0  )
		{
			m_bExitProcessing = FALSE;
			
			MSGBASE msg;
			msg.Category	= MP_CHAR;
			msg.Protocol	= MP_CHAR_EXIT_SYN;
			msg.dwObjectID	= HEROID;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		else
		{
			// 070125 LYW --- Modified this part.
			/*
			if( m_nExitKind == eEK_GAMEEXIT )
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 540 ), m_nCount );
			else if( m_nExitKind == eEK_CHARSEL )
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 541 ), m_nCount );
				*/
			// 080922 LYW --- ExitManager : 접속종료/캐릭터 선택화면은 단축창과 메인창에 모두 출력되도록 수정(원상미)
			//if( m_nExitKind == eEK_GAMEEXIT )
			//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 300 ), m_nCount );
			//else if( m_nExitKind == eEK_CHARSEL )
			//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 218 ), m_nCount );

			if( m_nExitKind == eEK_GAMEEXIT )
				CHATMGR->AddMsg( CTC_EXITCOUNT, CHATMGR->GetChatMsg( 300 ), m_nCount );
			else if( m_nExitKind == eEK_CHARSEL )
				CHATMGR->AddMsg( CTC_EXITCOUNT, CHATMGR->GetChatMsg( 218 ), m_nCount );

		}

		--m_nCount;		//카운트다운
	}
}
