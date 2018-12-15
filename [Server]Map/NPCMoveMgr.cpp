// 090227 ShinJS --- 이동NPC관리
// NPCMoveMgr.cpp: implementation of the CNPCMoveMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NPCMoveMgr.h"
#include "MHFile.h"
#include "Network.h"
#include "../[CC]Header/GameResourceManager.h"
#include "Player.h"
#include "UserTable.h"

//////////////////////////////////////////////////////////////////////
// CMoveNPCRes : 이동NPC 정보를 보관할 리소스 클래스
//////////////////////////////////////////////////////////////////////
CMoveNPCRes::CMoveNPCRes()
{
	m_htMoveNpc.Initialize( MAX_MOVENPC_NUM );		// 이동NPC 초기화

	// 이동NPC 리소스에서 읽어오기
	CMHFile file;
	if( !file.Init( "./system/Resource/MoveNpc.bin", "rb") )
		return;

	char Token[256];	
	while( !file.IsEOF() )
	{
		file.GetString( Token );

		if( strcmp(Token, "//") == 0 )
		{
			file.GetLine(Token, 256);
			continue;
		}

		// 이동NPC 추가
		if( strcmp(Token,"$NPC") == 0 )
		{
			// 최대 이동NPC 개수 초과인경우
			if( m_htMoveNpc.GetDataNum() >= MAX_MOVENPC_NUM )
				break;

			WORD wNpcIdx		= file.GetWord();							// NPC Index
			WORD wNpcKindIdx	= file.GetWord();							// NPC Kind
			WORD wStartMapNum	= file.GetWord();							// 처음 생성될 맵 번호
			DWORD dwMoveTime	= file.GetDword();							// 이동시간 간격(초)
			DWORD dwReadyToTime	= file.GetDword();							// 이동전 준비시간(초)
			WORD wMoveInfoIdx	= 0;										// 테이블 인덱스

			CMoveNPC* pMoveNPC = new CMoveNPC( wNpcIdx, wNpcKindIdx, wStartMapNum, dwMoveTime, dwReadyToTime );
			ASSERT( pMoveNPC );
			if( !pMoveNPC )		break;

			while( !file.IsEOF() )
			{
				file.GetString( Token );

				if( strcmp(Token, "//") == 0 )
				{
					file.GetLine(Token, 256);
					continue;
				}

				if( Token[0] == '}' )
					break;

				// 이동NPC의 이동정보 추가
				if( strcmp(Token, "#MOVEINFO") == 0 )
				{
					// 이동정보 개수를 넘어간 경우 추가하지 않는다
					if( wMoveInfoIdx >= MAX_NPC_MOVEINFO )
						continue;

					stNPC_MoveInfo* pMoveInfo = new stNPC_MoveInfo();
					ASSERT( pMoveInfo );
					if( !pMoveInfo )	break;

					pMoveInfo->wMapNum = file.GetWord();					// 이동하는 맵번호
					pMoveInfo->cPos.wx = file.GetWord();					// 이동하는 x좌표
					pMoveInfo->cPos.wz = file.GetWord();					// 이동하는 z좌표
					pMoveInfo->wDir	   = file.GetWord();					// 이동후 캐릭터 방향

					pMoveNPC->AddMoveInfo( pMoveInfo, wMoveInfoIdx++ );		// 테이블에 정보 추가
				}
			}

			m_htMoveNpc.Add( pMoveNPC, wNpcIdx );							// 이동NPC 추가
		}
	}
	file.Release();
}

CMoveNPCRes::~CMoveNPCRes()
{
	// 이동NPC 제거
	CMoveNPC* pData = NULL;
	// 090707 ShinJS --- 메모리 해제 오류 수정
	m_htMoveNpc.SetPositionHead();
	while( (pData = m_htMoveNpc.GetData()) != NULL )
	{
		delete pData;
		pData = NULL;
	}
	m_htMoveNpc.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
// CNPCMoveMgr : NPC 이동을 관리할 클래스
//////////////////////////////////////////////////////////////////////
CNPCMoveMgr::CNPCMoveMgr()
{
	m_htMoveNpc.Initialize( MAX_MOVENPC_NUM );		// 이동NPC 초기화
}

CNPCMoveMgr::~CNPCMoveMgr()
{
	// 이동NPC 제거
	m_htMoveNpc.RemoveAll();
}


void CNPCMoveMgr::Init()
{	
	CYHHashTable<CMoveNPC>* pMoveNpcRes = MOVENPCRES->GetMoveNpcTable();
	if( !pMoveNpcRes )		return;

	CMoveNPC* pMoveNpc = NULL;
	pMoveNpcRes->SetPositionHead();

	while( (pMoveNpc = pMoveNpcRes->GetData()) != NULL )
	{
		// 현재 맵번호에 맞는 이동NPC 추가
		if( pMoveNpc->GetStartMapNum() == GAMERESRCMNGR->GetLoadMapNum() )
		{
			// 관리자에 이동NPC를 등록한다
			m_htMoveNpc.Add( pMoveNpc, pMoveNpc->GetNPCIdx() );

			stNPC_MoveInfo* pMoveInfo = pMoveNpc->GetCurMoveInfo();
			if( !pMoveInfo )	continue;

			// GAMERESRCMNGR에 이동NPC 정보를 추가하여 작업(거래)을 활성화 시킨다
			GAMERESRCMNGR->AddMoveNpcInfo( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx(), pMoveInfo->cPos.wx, pMoveInfo->cPos.wz, pMoveInfo->wDir );
		}
	}

}

void CNPCMoveMgr::Process()
{
	CMoveNPC* pMoveNPC = NULL;

	m_htMoveNpc.SetPositionHead();
	while( (pMoveNPC = m_htMoveNpc.GetData()) != NULL )
	{
		DWORD dwLastMoveTime	= pMoveNPC->GetLastMoveTime();				// 마지막 이동시각
		DWORD dwMoveTime		= pMoveNPC->GetMoveTime();					// 이동시간 간격(초)
		DWORD dwReadyToTime		= pMoveNPC->GetReadyToTime();				// 이동준비시간 간격(초)

		// 이동준비작업 알림 (이동준비시간이고 준비메세지를 전송하지 않은경우)
		if( (gCurTime - dwLastMoveTime) > (dwMoveTime - dwReadyToTime)*1000 &&
			!pMoveNPC->IsSendReadyToMove() )
		{
			stNPC_MoveInfo* pCurMoveInfo = pMoveNPC->GetCurMoveInfo();		// 이동전 정보
			if( !pCurMoveInfo )		continue;

			// StaticNpc Table에 이동NPC 제거 (StaticNpc Table만 제거하여 작업(거래)을 못하도록한다)
			GAMERESRCMNGR->RemoveStaticNpcInfo( pMoveNPC->GetNPCIdx() );
			
			// 유저에게 이동준비 메세지를 보낸다
			MSG_WORD msg;
			memset( &msg, 0, sizeof(MSG_WORD) ) ;

			msg.Category	= MP_NPC ;
			msg.Protocol	= MP_NPC_READYTOMOVE_NOTIFY;
			msg.wData		= pMoveNPC->GetNPCIdx();

			g_pUserTable->SetPositionUserHead();
			CObject* pObject = NULL;
			while( (pObject = g_pUserTable->GetUserData() ) != NULL)
			{	
				if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

				CPlayer* pReceiver = (CPlayer*)pObject;
				pReceiver->SendMsg( &msg, sizeof(MSG_WORD) );
			}

			// 이동준비 플래그 설정
			pMoveNPC->SetSendReadyToMove( TRUE );
		}

		// 이동해야할 시간인 경우
		if( (gCurTime - dwLastMoveTime) > dwMoveTime*1000 )
		{
			stNPC_MoveInfo* pCurMoveInfo = pMoveNPC->GetCurMoveInfo();		// 이동전 정보
			stNPC_MoveInfo* pNextMoveInfo = pMoveNPC->GetNextMoveInfo();	// 이동후 정보
			if( !pCurMoveInfo || !pNextMoveInfo )		continue;

			// 맵이 바뀌는 경우
			if( pCurMoveInfo->wMapNum != pNextMoveInfo->wMapNum )
			{
				// 이동NPC 이동 메세지 전송 (이동할 맵에 추가 성공시 제거)
				MSG_WORD4 msg;
				memset( &msg, 0, sizeof(MSG_WORD4) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_MOVE_MOVENPC_SYN;
				msg.wData1		= pCurMoveInfo->wMapNum;
				msg.wData2		= pNextMoveInfo->wMapNum;
				msg.wData3		= pMoveNPC->GetNPCIdx();
				msg.wData4		= pMoveNPC->GetCurMoveIdx();

				g_Network.Send2AgentServer( (char*)&msg, sizeof(msg) );

				pMoveNPC->SetLastMoveTime( gCurTime );												// 마지막 이동시각 갱신
			}
			// 같은 맵으로 이동하는 경우
			else
			{
				// 바로 이동시킨다
				pMoveNPC->Move();																	// 이동NPC를 이동한다
				pMoveNPC->SetLastMoveTime( gCurTime );												// 마지막 이동시각 갱신
				pMoveNPC->SetSendReadyToMove( FALSE );												// 이동준비 플래그 설정

				// 이동된 정보로 설정해준다(추가시 존재하는 경우 변경)
				GAMERESRCMNGR->AddMoveNpcInfo( pMoveNPC->GetNPCIdx(), pMoveNPC->GetNPCKindIdx(), pNextMoveInfo->cPos.wx, pNextMoveInfo->cPos.wz, pNextMoveInfo->wDir );
				
				// 유저에게 추가메세지 전송(존재하는경우 추가없이 기존NPC를 이동한다)
				MSG_DWORD4 msg;
				memset( &msg, 0, sizeof(MSG_DWORD4) );

				msg.Category	= MP_NPC;
				msg.Protocol	= MP_NPC_ADD_MOVENPC_SYN;
				msg.dwData1		= MAKEDWORD( pNextMoveInfo->wMapNum, pNextMoveInfo->wDir );			// Map 번호, 방향
				msg.dwData2		= MAKEDWORD( pMoveNPC->GetNPCIdx(), pMoveNPC->GetNPCKindIdx() );	// NPC Index, NPC Kind
				msg.dwData3		= MAKEDWORD( pNextMoveInfo->cPos.wx, pNextMoveInfo->cPos.wz );		// NPC 위치

				g_pUserTable->SetPositionUserHead();
				CObject* pObject = NULL ;
				while( (pObject = g_pUserTable->GetUserData()) != NULL )
				{	
					if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

					CPlayer* pReceiver = (CPlayer*)pObject;
					pReceiver->SendMsg( &msg, sizeof(MSG_DWORD4) );
				}
			}
		}
	}
}

BOOL CNPCMoveMgr::AddMoveNpc( WORD wNpcIdx, WORD wMoveIdx )
{
	// 이동NPC 리소스에서 해당Index를 가진 이동NPC를 구한다
	CMoveNPC* pMoveNpc = MOVENPCRES->GetMoveNpc( wNpcIdx );
	if( !pMoveNpc ) return FALSE;

	// 해당 NPC가 존재하지 않는 경우에 추가 (맵 다운이나 재실행으로 인해 MoveNpc가 이중생성된 경우?)
	if( m_htMoveNpc.GetData( pMoveNpc->GetNPCIdx() ) == NULL )
		m_htMoveNpc.Add( pMoveNpc, pMoveNpc->GetNPCIdx() );								// 이동NPC 추가	
	
	pMoveNpc->Move( wMoveIdx );															// 이동NPC 이동
	pMoveNpc->SetLastMoveTime( gCurTime );												// 마지막 이동시각 갱신
	pMoveNpc->SetSendReadyToMove( FALSE );												// 이동준비 플래그 설정

	stNPC_MoveInfo* pCurMoveInfo = pMoveNpc->GetCurMoveInfo();
	if( !pCurMoveInfo )	return FALSE;

	// 이동된 정보로 설정해준다(추가시 존재하는 경우 변경)
	GAMERESRCMNGR->AddMoveNpcInfo( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx(), pCurMoveInfo->cPos.wx, pCurMoveInfo->cPos.wz, pCurMoveInfo->wDir );

	// 유저들에게 추가 메세지 전송
	MSG_DWORD4 msg;
	memset( &msg, 0, sizeof(MSG_DWORD4) );

	msg.Category	= MP_NPC;
	msg.Protocol	= MP_NPC_ADD_MOVENPC_SYN;
	msg.dwData1		= MAKEDWORD( pCurMoveInfo->wMapNum, pCurMoveInfo->wDir );			// Map 번호, 방향
	msg.dwData2		= MAKEDWORD( pMoveNpc->GetNPCIdx(), pMoveNpc->GetNPCKindIdx() );	// NPC Index, NPC Kind
	msg.dwData3		= MAKEDWORD( pCurMoveInfo->cPos.wx, pCurMoveInfo->cPos.wz );		// NPC 위치

	g_pUserTable->SetPositionUserHead();
	CObject* pObject = NULL ;
	while( (pObject = g_pUserTable->GetUserData()) != NULL )
	{	
		if( pObject->GetObjectKind() != eObjectKind_Player ) continue;

		CPlayer* pReceiver = (CPlayer*)pObject;
		pReceiver->SendMsg( &msg, sizeof(MSG_DWORD4) );
	}

	return TRUE;
}

void CNPCMoveMgr::RemoveMoveNpc( WORD wNpcIdx )
{
	m_htMoveNpc.Remove( wNpcIdx );

	// GAMERESRCMNGR에 이동NPC 정보를 제거하여 작업을 비활성화 시킨다
	GAMERESRCMNGR->RemoveMoveNpcInfo( wNpcIdx );
}

BOOL CNPCMoveMgr::IsReadyToMove( WORD wNpcIdx )
{
	CMoveNPC* pMoveNpc = (CMoveNPC*)m_htMoveNpc.GetData( wNpcIdx );
	if( pMoveNpc )
		return pMoveNpc->IsSendReadyToMove();

	return FALSE;
}