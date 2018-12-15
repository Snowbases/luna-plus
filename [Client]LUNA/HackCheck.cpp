// HackCheck.cpp: implementation of the CHackCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HackCheck.h"
#include "ExitManager.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "MHTimeManager.h"
// 090109 LUJ, 클라이언트를 일정 시간 후에 종료시키기 위해 참
#include "GlobalEventFunc.h"

#ifdef _NPROTECT_
#include "NProtectManager.h"
#endif

GLOBALTON(CHackCheck)

CHackCheck::CHackCheck()
{
	m_bIsHackUser	= FALSE;
	m_dwLastTime	= gCurTime;
	// 090109 LUJ, 스크립트 체크 변수 초기화
	ZeroMemory( &mScriptHackCheck, sizeof( mScriptHackCheck ) );
}

CHackCheck::~CHackCheck()
{
	PTRLISTPOS pos = m_ListSpeedHackCheck.GetHeadPosition();
	while( pos )
	{
		sCHECKTIME* pTime = (sCHECKTIME*)m_ListSpeedHackCheck.GetNext( pos );
		delete pTime;
	}
	m_ListSpeedHackCheck.RemoveAll();
}

void CHackCheck::Release()
{
	PTRLISTPOS pos = m_ListSpeedHackCheck.GetHeadPosition();
	while( pos )
	{
		sCHECKTIME* pTime = (sCHECKTIME*)m_ListSpeedHackCheck.GetNext( pos );
		delete pTime;
	}
	m_ListSpeedHackCheck.RemoveAll();
	
	m_bIsHackUser	= FALSE;
	m_dwLastTime	= gCurTime;
	// 090109 LUJ, 해제될 때 초기화시켜야 계속 전송하지 않는다
	ZeroMemory( &mScriptHackCheck, sizeof( mScriptHackCheck ) );
}

void CHackCheck::Process()
{
	// 090109 LUJ, 일정 주기마다 스크립트에서 추출한 고유값을 서버에 전송한다
	if( mScriptHackCheck.mCheckStepTick )
	{
		const BOOL isCheckTime = ( mScriptHackCheck.mNextCheckTick < gCurTime );

		if( isCheckTime )
		{
			MSG_SCRIPT_CHECK message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_HACKCHECK;
			message.Protocol	= MP_HACKCHECK_SCRIPTHACK_SYN;
			message.dwObjectID	= HEROID;
			message.mCheckValue	= mScriptHackCheck.mCheckValue;

			NETWORK->Send(
				&message,
				sizeof( message ) );

			mScriptHackCheck.mNextCheckTick = gCurTime + mScriptHackCheck.mCheckStepTick;
		}
	}

	if( m_ListSpeedHackCheck.IsEmpty() )
		return;

	if( gCurTime - m_dwLastTime >= SPEEDHACK_CHECKTIME )	//gCurTime이 옛날시간(작은시간)이라도 상관없다.
	{
#ifdef _NPROTECT_
		NPROTECTMGR->Check();
#endif

		sCHECKTIME* pTime = (sCHECKTIME*)m_ListSpeedHackCheck.GetHead();
		if( pTime )
		{
			MSG_DWORD msg;
			msg.Category	= MP_HACKCHECK;
			msg.Protocol	= MP_HACKCHECK_SPEEDHACK;
			msg.dwObjectID	= HEROID;
			msg.dwData		= pTime->dwServerSendTime;
			NETWORK->Send( &msg, sizeof( msg ) );

			m_ListSpeedHackCheck.RemoveHead();
			delete pTime;

			if( !m_ListSpeedHackCheck.IsEmpty() )
			{
				m_dwLastTime = ((sCHECKTIME*)m_ListSpeedHackCheck.GetHead())->dwArrivedTime;
			}
		}
	}
}

void CHackCheck::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_HACKCHECK_SPEEDHACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			sCHECKTIME* pTime = new sCHECKTIME;
			//pTime->dwArrivedTime = gCurTime;	//여기에 옛날 시간이?
			pTime->dwArrivedTime	= MHTIMEMGR->GetNewCalcCurTime();
			pTime->dwServerSendTime = pmsg->dwData;
			m_ListSpeedHackCheck.AddTail( pTime );
			m_dwLastTime = ((sCHECKTIME*)m_ListSpeedHackCheck.GetHead())->dwArrivedTime;
		}
		break;
	case MP_HACKCHECK_BAN_USER:
		{
			m_bIsHackUser = TRUE;
			//타이머 10초 이후에 종료해야 하는건가?
		}
		break;
		// 090109 LUJ, 스크립트 체크 시작
	case MP_HACKCHECK_SCRIPTHACK_ACK:
		{
			const MSG_DWORD* const message = ( MSG_DWORD* )pMsg;

			mScriptHackCheck.mCheckStepTick = message->dwData;
			SetScriptCheckValue( mScriptHackCheck.mCheckValue );
		}
		break;
		// 090109 LUJ, 스크립트가 잘못되어 접속 종료해야하는 경우
	case MP_HACKCHECK_SCRIPTHACK_NACK:
		{
			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1275 ) );
			// 090115 LUJ, GM툴은 클라이언트측에서 자동 종료하지 않도록 매크로 처리한다
#ifndef _GMTOOL_
			// 090120 LUJ, 패치 버전을 최초로 돌린다
			{
				const char* const	versionFile = "LunaVerInfo.ver";
				FILE* const			file		= fopen( versionFile, "r+" );
	
				if( file )
				{
					// 090109 LUJ, 4글자로 구성된 버전 헤더를 파일에서 읽어와 문자열 변수에 담는다
					const size_t headerSize = 4;
					char header[ headerSize + 1 ] = { 0 };
					fread(
						header,
						headerSize,
						sizeof( *header ),
						file );

					// 090109 LUJ, 전체 패치를 수행할 버전 번호를 만든다
					char versionText[ MAX_PATH ] = { 0 };
					sprintf(
						versionText,
						"%s00000000",
						header );
					// 090109 LUJ, 패치 버전 문자열을 파일에 쓴다
					fseek(
						file,
						0,
						SEEK_SET );
					fwrite(
						versionText,
						sizeof( *versionText ),
						strlen( versionText ),
						file );
					fclose( file );
				}
			}

			// 090120 LUJ, 카운팅 후 클라이언트가 종료되도록 한다
			MessageBox_Func(
				MBI_EXIT,
				0,
				MBI_YES );
#endif
		}
		break;
	}
}
