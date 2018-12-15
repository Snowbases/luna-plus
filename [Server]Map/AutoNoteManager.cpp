#include "stdafx.h"
#include ".\autonotemanager.h"
#include "UserTable.h"
#include "Player.h"
#include "AutoNoteRoom.h"
#include "Network.h"
#include "MapDBMsgParser.h"

CAutoNoteManager::CAutoNoteManager(void)
{
	m_bInited = FALSE;
}

CAutoNoteManager::~CAutoNoteManager(void)
{
}

void CAutoNoteManager::Init()
{
	m_htAutoNoteRoom.Initialize( 100 );
	m_pmpAutoNoteRoom = new CPool< CAutoNoteRoom >;
	m_pmpAutoNoteRoom->Init( 100, 100, "CAutoNoteRoom" );

//---이미지로드
	m_pOriRaster[0] = new BYTE[16*16*3];
	m_pOriRaster[1] = new BYTE[16*16*3];
	m_pOriRaster[2] = new BYTE[16*16*3];
	m_pOriRaster[3] = new BYTE[16*16*3];
	m_pBGRaster		= new BYTE[128*32*3];
	m_pNoiseRaster	= new BYTE[16*16*3];
	m_pSendRaster	= new BYTE[128*32*3];

	if( BMP_RasterLoad( "./AutoNoteImage/01Red.bmp", m_pOriRaster[0] ) == FALSE ) return;
	if( BMP_RasterLoad( "./AutoNoteImage/02Yellow.bmp", m_pOriRaster[1] ) == FALSE ) return;
	if( BMP_RasterLoad( "./AutoNoteImage/03Blue.bmp", m_pOriRaster[2] ) == FALSE ) return;
	if( BMP_RasterLoad( "./AutoNoteImage/04Black.bmp", m_pOriRaster[3] ) == FALSE ) return;
	if( BMP_RasterLoad( "./AutoNoteImage/00BG.bmp", m_pBGRaster ) == FALSE ) return;
	if( BMP_RasterLoad( "./AutoNoteImage/00Noise.bmp", m_pNoiseRaster ) == FALSE ) return;

	// --------------------------------------------------
	// 090106 ShinJS --- 재실행/부재중판단 시간 설정 추가
	m_dwCanRetryTime		= 300;
	m_dwTargetAbsentTime	= 10;
	// --------------------------------------------------

	//---TEST
//	MakeSendRaster();

	m_bInited = TRUE;

	return;
}

void CAutoNoteManager::Release()
{
	m_pmpAutoNoteRoom->Release();
	delete m_pmpAutoNoteRoom;
	m_htAutoNoteRoom.RemoveAll();

//---이미지해제
	delete[] m_pOriRaster[0];
	delete[] m_pOriRaster[1];
	delete[] m_pOriRaster[2];
	delete[] m_pOriRaster[3];
	delete[] m_pBGRaster;
	delete[] m_pNoiseRaster;
	delete[] m_pSendRaster;
}

BOOL CAutoNoteManager::BMP_RasterLoad( char* filename, BYTE* pRaster )
{
	HANDLE hFile = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD dwFileSize = GetFileSize( hFile, NULL );
	BYTE* data = new BYTE[dwFileSize];

	DWORD dwRead;
	ReadFile( hFile, data, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)data;
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)( data + sizeof(BITMAPFILEHEADER) );

	int RasterSize = ( ih->biWidth * ih->biHeight * ih->biBitCount ) / 8;
	if( RasterSize < 0 ) RasterSize = -RasterSize;

	memcpy( pRaster, data + ((BITMAPFILEHEADER*)fh)->bfOffBits, RasterSize );

	delete[] data;

	return TRUE;
}

void CAutoNoteManager::MakeSendRaster( DWORD* pData )
{
	int x, y, cx, cy, rot;
	float fszx, fszy;

//---Clear
	memset( m_pSendRaster, 0xee, 128*32*3 ); 

//---Background
	fszx = (rand()%5 + 11) / 10.f;	fszy = (rand()%5 + 11) / 10.f;
	BlitImage( m_pSendRaster, m_pBGRaster, 128, 32, 128, 32, 0, 0, fszx, fszy );

//---noise
	for( int i = 0 ; i < 4 ; ++i )
	{
		x = rand()%(128-16);				y = rand()%(32-16);
		fszx = (rand()%10 + 10) / 10.f;		fszy = (rand()%10 + 10) / 10.f;
		cx = rand()%4 + 6;					cy = rand()%4 + 6;
		rot = rand()%41 - 20;
		BlitImage( m_pSendRaster, m_pNoiseRaster, 128, 32, 16, 16, x, y, fszx, fszy, float( rot ), cx, cy );
	}

//---txt
	int firstpos = rand()%32;
	int xrand = ( ( 128 - firstpos ) - 90 ) / 4;
	for( int i = 0 ; i < 4 ; ++i )
	{
		x = rand()%xrand + firstpos + i*24;	//i*32;
		y = rand()%8 + 3;
		fszx = (rand()%5 + 11) / 10.f;		fszy = (rand()%5 + 11) / 10.f;
		cx = rand()%4 + 6;					cy = rand()%4 + 6;
		rot = rand()%61 - 30;
		BlitImage( m_pSendRaster, m_pOriRaster[pData[i]], 128, 32, 16, 16, x, y, fszx, fszy, float( rot ), cx, cy, BIF_RANDOMCOLOR );
	}
}

void CAutoNoteManager::BlitImage( BYTE* pDest, BYTE* pSrc, int dw, int dh, int sw, int sh, int x, int y, float fszw, float fszh, float fRot, int cx, int cy, int Flag )
{
	int lsw = int( sw * fszw );
	int lsh = int( sh * fszh );

	for( int row = 0 ; row < lsh ; ++row )
	{
		for( int col = 0 ; col < lsw ; ++col )
		{
			int osx = int( col / fszw + 0.5f );
			int osy = int( row / fszh + 0.5f );

			if( osx >= sw || osy >= sh )
				continue;

			float fRad = 3.14f * fRot / 180.f;
			int ldx = int( ( col - cx ) * cos( fRad ) - ( row - cy ) * sin( fRad ) + cx );
			int ldy = int( ( col - cx ) * sin( fRad ) + ( row - cy ) * cos( fRad ) + cy );

			if( ldx + x >= dw || ldy + y >= dh )
				continue;

			BYTE Color[3]; 
			Color[0] = *(pSrc + osx * 3 + osy * 3 * sw);
			Color[1] = *(pSrc + osx * 3 + osy * 3 * sw + 1);
			Color[2] = *(pSrc + osx * 3 + osy * 3 * sw + 2);

			if( Color[0] != 255 || Color[1] != 255 || Color[2] != 255 )	//흰색이 아니면
			{
				if( Flag == BIF_RANDOMCOLOR )
				{
					//생상 랜덤 조정
					Color[0] = BYTE( rand() % 100 );	//255 뒤집혀도 상관없자나?
					Color[1] = BYTE( rand() % 100 );
					Color[2] = BYTE( rand() % 100 );
				}

				memcpy( pDest + ( ldx + x ) * 3 + ( ldy + y ) * 3 * dw, Color, 3 );
			}
		}
	}
}

void CAutoNoteManager::Process()
{
	m_htAutoNoteRoom.SetPositionHead();
	CAutoNoteRoom* pANRoom = NULL ;
	while( (pANRoom = m_htAutoNoteRoom.GetData() )!= NULL)
	{
		if( pANRoom->IsTimeOver() )
		{
			CPlayer* pAutoPlayer = (CPlayer*)g_pUserTable->FindUser( pANRoom->GetAutoCharacterIdx() );

			if( pANRoom->GetAutoNoteRoomState() == eANRS_WAITANSWER )	//---답변 입력을 기다리는 중이다. 답변이 늦었다.
			{
				if( pAutoPlayer )
				{
					MSG_DWORD msg1;
					msg1.Category	= MP_AUTONOTE;
					msg1.Protocol	= MP_AUTONOTE_ANSWER_TIMEOUT;
					msg1.dwData		= pANRoom->GetAutoUserIdx();
					pAutoPlayer->SendMsg( &msg1, sizeof(msg1) );
				}

				//---신고자에게 신고대상이 제재되었음을 알림 (어디에 있을지 모르므로 모든 에이전트로 보냄)
				MSG_DWORD msg2;
				msg2.Category	= MP_AUTONOTE;
				msg2.Protocol	= MP_AUTONOTE_KILLAUTO;
				msg2.dwData		= pANRoom->GetAskUserIdx();
				g_Network.Broadcast2AgentServer( (char*)&msg2, sizeof(msg2) );

				//---DB 신고자 오토리스트에 등록
				AutoNoteListAdd( pANRoom->GetAskCharacterIdx(), pANRoom->GetAutoCharacterName(), pANRoom->GetAutoCharacterIdx(), pANRoom->GetAutoUserIdx() );

				//---대상 접속 끊기
				MSG_DWORD msg3;
				msg3.Category	= MP_AUTONOTE;
				msg3.Protocol	= MP_AUTONOTE_DISCONNECT;
				msg3.dwData		= pANRoom->GetAutoUserIdx();
				g_Network.Broadcast2AgentServer( (char*)&msg3, sizeof(msg3) );
			}
			else if( pANRoom->GetAutoNoteRoomState() == eANRS_FASTANSWER )
			{
				//---신고자에게 신고대상이 제재되었음을 알림 (어디에 있을지 모르므로 모든 에이전트로 보냄)
				MSG_DWORD msg2;
				msg2.Category	= MP_AUTONOTE;
				msg2.Protocol	= MP_AUTONOTE_KILLAUTO;
				msg2.dwData		= pANRoom->GetAskUserIdx();
				g_Network.Broadcast2AgentServer( (char*)&msg2, sizeof(msg2) );

				//---DB 신고자 오토리스트에 등록
				AutoNoteListAdd( pANRoom->GetAskCharacterIdx(), pANRoom->GetAutoCharacterName(), pANRoom->GetAutoCharacterIdx(), pANRoom->GetAutoUserIdx() );

				//---대상 접속 끊기
				MSG_DWORD msg3;
				msg3.Category	= MP_AUTONOTE;
				msg3.Protocol	= MP_AUTONOTE_DISCONNECT;
				msg3.dwData		= pANRoom->GetAutoUserIdx();
				g_Network.Broadcast2AgentServer( (char*)&msg3, sizeof(msg3) );
			}

			if( pAutoPlayer )	pAutoPlayer->SetAutoNoteIdx( 0 );
			m_htAutoNoteRoom.Remove( pANRoom->GetAskCharacterIdx() );
			m_pmpAutoNoteRoom->Free( pANRoom );
			break;	//한번에 하나씩만 지우자 (안전하게)
		}
	}
}

void CAutoNoteManager::AutoPlayerLogOut( CPlayer* pAutoPlayer )
{
	CAutoNoteRoom* pANRoom = m_htAutoNoteRoom.GetData( pAutoPlayer->GetAutoNoteIdx() );
	if( pANRoom == NULL ) return;

	//---신고한 유저가 나가버렸다.
	MSG_DWORD msg;
	msg.Category	= MP_AUTONOTE;
	msg.Protocol	= MP_AUTONOTE_ANSWER_LOGOUT;
	msg.dwData		= pANRoom->GetAutoUserIdx();
	pAutoPlayer->SendMsg( &msg, sizeof(msg) );

	//---신고자에게 신고대상이 제재되었음을 알림 (어디에 있을지 모르므로 모든 에이전트로 보냄)
	MSG_DWORD msg2;
	msg2.Category	= MP_AUTONOTE;
	msg2.Protocol	= MP_AUTONOTE_KILLAUTO;
	msg2.dwData		= pANRoom->GetAskUserIdx();
	g_Network.Broadcast2AgentServer( (char*)&msg2, sizeof(msg2) );

	//---DB 신고자 오토리스트에 등록
	AutoNoteListAdd( pANRoom->GetAskCharacterIdx(), pANRoom->GetAutoCharacterName(), pANRoom->GetAutoCharacterIdx(), pANRoom->GetAutoUserIdx() );

	//---오토 질답방 해제
	m_htAutoNoteRoom.Remove( pANRoom->GetAskCharacterIdx() );
	m_pmpAutoNoteRoom->Free( pANRoom );
}

void CAutoNoteManager::NetworkMsgParse( BYTE Protocol, void* pMsg, DWORD dwLength )
{
	switch(Protocol)
	{
	case MP_AUTONOTE_ASKTOAUTO_SYN:
		{
			MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;

            CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
			if( pPlayer == NULL )	return;

			int error = eAutoNoteError_None;

			if( AUTONOTEMGR->CanUseAutoNote() == FALSE )	//---이미지 읽는 초기화를 실패하였으므로 오토노트 기능을 사용할 수 없다.
			{
				goto AutoNoteAskError;
			}

			if( g_pServerSystem->GetMap()->IsAutoNoteAllow() == FALSE )
			{
				error = eAutoNoteError_CantUseMap;				//---사용할 수 없는 맵
				goto AutoNoteAskError;				
			}

			// ------------------------------------------------------------------------
			// 090106 ShinJS --- 오토노트 마지막 적발 실패 후 마지막 사용시간 체크 추가
			if( pPlayer->GetAutoNoteLastExecuteTime() != 0 &&
				gCurTime < pPlayer->GetAutoNoteLastExecuteTime() + m_dwCanRetryTime*1000 )
			{				
				MSG_DWORD msg;
				msg.Category	= MP_AUTONOTE;
				msg.Protocol	= MP_AUTONOTE_PUNISH;
				msg.dwData		= m_dwCanRetryTime*1000 - (gCurTime - pPlayer->GetAutoNoteLastExecuteTime());
				pPlayer->SendMsg( &msg, sizeof(msg) );
				return;
			}
			// ------------------------------------------------------------------------

			if( CAutoNoteRoom* pRoom = m_htAutoNoteRoom.GetData( pPlayer->GetID() ) )
			{
				error = eAutoNoteError_AlreadyAsking;			//---신고중(한번에 한캐릭터만 신고할 수 있다)
				goto AutoNoteAskError;
			}

			CPlayer* pAutoPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwData1 );
			if( pAutoPlayer == NULL )
			{
				error = eAutoNoteError_CantFind;				//---상대가 없다
				goto AutoNoteAskError;
			}

			if( pAutoPlayer->GetAutoNoteIdx() )		
			{
				error = eAutoNoteError_AlreadyAsked;			//---누가 벌써 신고한 캐릭터다
				goto AutoNoteAskError;
			}

			// 090106 ShinJS --- 부재중 판단 변수 적용(m_dwTargetAbsentTime)
			if( gCurTime > pAutoPlayer->GetLastActionTime() + m_dwTargetAbsentTime*1000 )
			{
				error = eAutoNoteError_NotProperState;			//---상대가 마지막 스킬 사용 후 10초가 지났다. 신고할 수 없는 상태.
				goto AutoNoteAskError;
			}
            
			CAutoNoteRoom* pANRoom = m_pmpAutoNoteRoom->Alloc();
			if( pANRoom == NULL )								//---방생성 실패
			{
				goto AutoNoteAskError;
			}

			m_htAutoNoteRoom.Add( pANRoom, pPlayer->GetID() );

			pANRoom->CreateRoom( pPlayer, pAutoPlayer, pmsg->dwData2 );
			pANRoom->AskToAuto();

			// 090609 ShinJS --- Add Log
			LogAutoNote( eAutoNoteLogKind_Report, pPlayer, pAutoPlayer );
			return;

AutoNoteAskError:
			MSG_DWORD msg;
			msg.Category	= MP_AUTONOTE;
			msg.Protocol	= MP_AUTONOTE_ASKTOAUTO_NACK;
			msg.dwData		= error;
			pPlayer->SendMsg( &msg, sizeof(msg) );
		}
		break;

	case MP_AUTONOTE_ANSWER_SYN:
		{
			MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

			CPlayer* pAutoPlayer = (CPlayer*)g_pUserTable->FindUser( pmsg->dwObjectID );
			if( pAutoPlayer == NULL ) return;

			CAutoNoteRoom* pANRoom = m_htAutoNoteRoom.GetData( pAutoPlayer->GetAutoNoteIdx() );
			if( pANRoom == NULL ) return;	//ACK MSG?

			//---정답 체크
			BOOL bCorrect = pANRoom->CheckAnswerFromAuto( pmsg->dwData1, pmsg->dwData2, pmsg->dwData3, pmsg->dwData4 );

			if( bCorrect )	//---정답
			{
				//---답변시간 체크
				DWORD dwAnswerTime = pANRoom->GetAnswerTime();
				if( dwAnswerTime <= 2000 ) //2초내로 답변을 했다. 보통의 경우엔 불가능
				{
					//---오토에겐 맞추었다고 알리고 안심시킨다.
					//---신고대상에게 답변을 맞추었다고 알림
					MSG_DWORD msg;
					msg.Category	= MP_AUTONOTE;
					msg.Protocol	= MP_AUTONOTE_ANSWER_ACK;
					msg.dwData		= pANRoom->GetAskUserIdx();
					pAutoPlayer->SendMsg( &msg, sizeof(msg) );

					pANRoom->FastAnswer();	//적당한 시간이 후 오토를 끊어버린다.

					// -------------------------------------------------------
					// 090106 ShinJS --- 적발 실패시 마지막 실행시간 저장 추가
					CPlayer* pAskPlayer = (CPlayer*)g_pUserTable->FindUser( pANRoom->GetAskCharacterIdx() );
					if( pAskPlayer )
						pAskPlayer->SetAutoNoteLastExecuteTime();
					// -------------------------------------------------------
					return;
				}

				//---신고한 사람에게 대상이 오토가 아님을 알림 (다른 맵으로 갔을 수도 있으니, 모든 에이젼트로 날려준다.)
				MSG_DWORD msg1;
				msg1.Category	= MP_AUTONOTE;
				msg1.Protocol	= MP_AUTONOTE_NOTAUTO;
				msg1.dwData		= pANRoom->GetAskUserIdx();
				g_Network.Broadcast2AgentServer( (char*)&msg1, sizeof(msg1) );

				//---신고대상에게 답변을 맞추었다고 알림
				MSG_DWORD msg2;
				msg2.Category	= MP_AUTONOTE;
				msg2.Protocol	= MP_AUTONOTE_ANSWER_ACK;
				msg2.dwData		= pANRoom->GetAskUserIdx();
				pAutoPlayer->SendMsg( &msg2, sizeof(msg2) );

				//---오토 질답방 해제
				pAutoPlayer->SetAutoNoteIdx( 0 );
				m_htAutoNoteRoom.Remove( pANRoom->GetAskCharacterIdx() );
				m_pmpAutoNoteRoom->Free( pANRoom );

				// -------------------------------------------------------
				// 090106 ShinJS --- 적발 실패시 마지막 실행시간 저장 추가
				CPlayer* pAskPlayer = (CPlayer*)g_pUserTable->FindUser( pANRoom->GetAskCharacterIdx() );
				if( pAskPlayer )
					pAskPlayer->SetAutoNoteLastExecuteTime();
				// -------------------------------------------------------
			}
			else	//---오답
			{
				if( pANRoom->GetChance() <= 0 )							//---3번 모두 틀렸다.
				{
					//---대상에게 답변 실패했음을 알림
					MSG_DWORD msg1;
					msg1.Category	= MP_AUTONOTE;
					msg1.Protocol	= MP_AUTONOTE_ANSWER_FAIL;
					msg1.dwData		= pANRoom->GetAutoUserIdx();
					pAutoPlayer->SendMsg( &msg1, sizeof(msg1) );

					//---대상 접속 끊기
					MSG_DWORD msg2;
					msg2.Category	= MP_AUTONOTE;
					msg2.Protocol	= MP_AUTONOTE_DISCONNECT;
					msg2.dwData		= pANRoom->GetAutoUserIdx();
					pAutoPlayer->SendMsg( &msg2, sizeof(msg2) );	//---에이젼트로 보내진다

					//---신고자에게 신고대상이 제재되었음을 알림 (어디에 있을지 모르므로 모든 에이전트로 보냄)
					MSG_DWORD msg3;
					msg3.Category	= MP_AUTONOTE;
					msg3.Protocol	= MP_AUTONOTE_KILLAUTO;
					msg3.dwData		= pANRoom->GetAskUserIdx();
					g_Network.Broadcast2AgentServer( (char*)&msg3, sizeof(msg3) );

					//---DB 신고자 오토리스트에 등록
					AutoNoteListAdd( pANRoom->GetAskCharacterIdx(), pANRoom->GetAutoCharacterName(), pANRoom->GetAutoCharacterIdx(), pANRoom->GetAutoUserIdx() );

					//---오토 질답방 해제
					pAutoPlayer->SetAutoNoteIdx( 0 );
					m_htAutoNoteRoom.Remove( pANRoom->GetAskCharacterIdx() );
					m_pmpAutoNoteRoom->Free( pANRoom );
				}
				else
				{
					MSG_DWORD msg;
					msg.Category	= MP_AUTONOTE;
					msg.Protocol	= MP_AUTONOTE_ANSWER_NACK;
					msg.dwData		= pANRoom->GetChance();
					pAutoPlayer->SendMsg( &msg, sizeof(msg) );
				}
			}
		}
		break;
	}
}