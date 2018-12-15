// GMNotifyManager.cpp: implementation of the CGMNotifyManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"																		// 표준 시스템과, 프로젝트가 지정한, 자주쓰는 해더들을 모은 해더파일을 불러온다.

#include "GMNotifyManager.h"															// GM공지 매니져 클래스 헤더를 불러온다.

#include "Interface/cFont.h"															// 폰트 클래스 헤더를 불러온다.

#include "MainGame.h"																	// 메인 게임 클래스 헤더를 불러온다.

#include "GameResourceStruct.h"															// 게임 리소스 구조체 헤더를 불러온다.

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLOBALTON(CGMNotifyManager)																// GM매니져 클래스를 글로벌 톤으로 설정한다.

CGMNotifyManager::CGMNotifyManager()													// GM매니져 클래스 생성자 함수.
{
	Init( MAX_NOTIFYMSG_LINE );															// 최대 공지 메시지 라인으로 초기화를 한다.
	SetPosition( NOTIFYMSG_DEFAULT_X, NOTIFYMSG_DEFAULT_Y, NOTIFYMSG_DEFAULT_WIDTH );	// 기본 출력 위치로 위치 세팅을 한다.
	SetLineSpace( NOTIFYMSG_DEFAULT_LINESPACE );										// 기본 라인 간격으로 라인 간격을 세팅한다.
	SetFont( NOTIFYMSG_DEFAULT_FONT );													// 기본 폰트로 공지 폰트를 세팅한다.

	memset( m_ApplyEventList, 0, sizeof(BOOL)*eEvent_Max );								// 이벤트 종류만큼 이벤트 리스트를 메모리 셋 한다.

	// 070622 LYW --- GMNotifyManager : Add function to load display part.
	LoadGameDesc() ;																	// 게임 설정을 로딩한다.

}

void CGMNotifyManager::LoadGameDesc()													// 게임 설정을 로딩하는 함수.
{
	GAMEDESC_INI GameDesc ;																// 게임 설정을 담는 구조체를 선언한다.

	ZeroMemory(&GameDesc, sizeof(GAMEDESC_INI)) ;										// 게임 설정 구조체를 메모리 셋 한다.

	FILE* fp = fopen("system\\launcher.sav","r");										// 파일을 연다.

	if( fp )																			// 파일 열기가 성공 했다면,
	{
		char buffer[ 100 ];																// 임시 버퍼를 선언한다.
		const char* separator = " =\n";													// 분리 기호 변수를 선언하고 세팅한다.

		while( fgets( buffer, sizeof( buffer ), fp ) )									// 스트링을 임시 버퍼로 읽어들인다.
		{
			const char* token = strtok( buffer, separator );							// 문자열에서 분리 기호와 같은 토큰을 찾는다.

			if( 0 == token || ';' == token[ 0 ] )										// 토큰이 빈 라인이거나, 코멘트라면,
			{
				continue;																// 계속 한다.
			}
			else if( ! strcmpi( "resolution",  token ) )								// 토큰이 리솔루션과 같지 않으면,
			{
				token = strtok( 0, separator );											// 토큰을 받는다.

				switch( atoi( token ) )													// 토큰을 숫자로 변환하여 확인한다.
				{
				case 0:																	// 0과 같으면,
					{
						GameDesc.dispInfo.dwWidth = 800;								// 게임 디스플레이 정보의 가로를 800으로 세팅한다.
						GameDesc.dispInfo.dwHeight = 600;								// 게임 디스플레이 정보의 세로를 600으로 세팅한다.
						break;
					}
				case 1:																	// 1과 같으면,
					{
						GameDesc.dispInfo.dwWidth = 1024;								// 게임 디스플레이 정보의 가로를 1024으로 세팅한다.
						GameDesc.dispInfo.dwHeight = 768;								// 게임 디스플레이 정보의 세로를 768으로 세팅한다.
						break;
					}
				case 2:																	// 2와 같으면,
					{
						GameDesc.dispInfo.dwWidth = 1280;								// 게임 디스플레이 정보의 가로를 1280으로 세팅한다.
						GameDesc.dispInfo.dwHeight = 1024;								// 게임 디스플레이 정보의 세로를 1024으로 세팅한다.
						break;
					}
				default:																// 그 외의 경우가 있어서는 안된다~!!
					{
						GameDesc.dispInfo.dwWidth = _ttoi(token);
						LPCTSTR textHeight = _tcstok(0, separator);
						GameDesc.dispInfo.dwHeight = _ttoi(textHeight ? textHeight : "");
						break;
					}
				}
			}
			else if( ! strcmpi( "windowMode", token ) )									// 토큰이 윈도우 모드와 같으면,
			{
				token = strtok( NULL, separator );										// 토큰을 받는다.

				GameDesc.dispInfo.dispType = ( ! strcmpi( "true", token ) ? WINDOW_WITH_BLT : FULLSCREEN_WITH_BLT );	// 디스플레이 모드를 세팅한다.
			}
		}

		fclose( fp );																	// 파일을 닫는다.
	}

	m_rcPos.right = GameDesc.dispInfo.dwWidth ;											// 공지 출력 영역의 가로 사이즈를 디스플레이 가로로 세팅한다.
	m_rcPos.left = 0 ;																	// 공지 출력 영역의 시작을 0으로 세팅한다.
}

CGMNotifyManager::~CGMNotifyManager()													// GM공지 매니져 클래스의 소멸자 함수.
{
	Release();																			// 해제 함수를 호출한다.
}

void CGMNotifyManager::Init( int nMaxLine )												// 초기화 함수.
{
	m_nMaxLine			= nMaxLine;														// 최대 라인 수를 인자로 넘어온 수로 세팅한다.

	m_bUseEventNotify	= FALSE;														// 이벤트 공지 사용 여부를 false로 세팅한다.
	ZeroMemory( m_strEventNotifyTitle, 32 );											// 이벤트 공지 제목 문자열을 제로 메모리 한다.
	ZeroMemory( m_strEventNotifyContext, 128 );											// 이벤트 공지 내용 문자열을 제로 메모리 한다.

	m_bNotifyChanged	= FALSE;														// 공지 변경 여부를 false로 세팅한다.
}

void CGMNotifyManager::Release()														// 해제 함수.
{
	PTRLISTPOS pos = m_ListNotify.GetHeadPosition();									// 공지 리스트를 헤드로 세팅하고 위치 정보를 받을 포인터를 선언하고 헤드 위치를 받는다.

	while(pos)																			// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		sGMNOTIFY* pNotify = (sGMNOTIFY *)m_ListNotify.GetNext(pos);					// 공지 구조체 포인터를 선언하고 위치에 따른 공지 구조체 정보를 받는다.
		SAFE_DELETE(pNotify);															// 안전하게 공지 정보를 삭제한다.
	}
	
	m_ListNotify.RemoveAll();															// 공지 리스트를 모두 비운다.
}

void CGMNotifyManager::PushMsg( char* pMsg, DWORD dwColor )								// 메시지 추가 함수( 메시지, 색상 )
{
	sGMNOTIFY* pNotify = new sGMNOTIFY;													// 공지 구조체 포인터를 생성한다.
	
	pNotify->nStrLen = strlen(pMsg);													// 문자열 길이를 세팅한다.

	if( pNotify->nStrLen > MAX_NOTIFYMSG_LENGTH )										// 문자열 길이가 최대 공지 길이 보다 길면,
	{
		strncpy( pNotify->strMsg, pMsg, MAX_NOTIFYMSG_LENGTH );							// 길이 만큼만 공지 메시지를 복사한다.
		pNotify->strMsg[MAX_NOTIFYMSG_LENGTH] = NULL;									// 공지 메시지 문자열의 마지막 값을 null로 세팅한다.
		pNotify->nStrLen = MAX_NOTIFYMSG_LENGTH;										// 공지 메시지 길이를 공지 최고 메시지 길이로 세팅한다.
	}
	else																				// 문자열 길이가 최대 공지 길이 보다 작으면,
	{
		strcpy( pNotify->strMsg, pMsg );												// 공지 메시지를 복사한다.
	}

	pNotify->dwReceiveTime	= gCurTime;													// 공지 메시지 수락 시간을 세팅한다.
	pNotify->dwColor		= dwColor;													// 공지 메시지 색상을 세팅한다.

	m_ListNotify.AddTail( pNotify );													// 공지 메시지 리스트에 공지 구조체 정보를 추가한다.

	if( m_ListNotify.GetCount() > m_nMaxLine )											// 공지 메시지가 최대 개수를 넘으면, 
	{
		PopMsg();																		// 공지 메시지를 뺀다.
	}
}

void CGMNotifyManager::PopMsg()															// 공지 메시지를 빼내는 함수.
{
	if( m_ListNotify.GetCount() )														// 공지 리스트 카운트가 있으면,
	{
		sGMNOTIFY* pNotify = (sGMNOTIFY*)m_ListNotify.GetHead();						// 공지 구조체 정보를 받을 포인터를 선언하고 정보를 받는다.
		SAFE_DELETE( pNotify );															// 안전하게 공지 구조체 정보를 삭제한다.
		m_ListNotify.RemoveHead();														// 공지 리스트 헤드를 지운다.
	}
}

void CGMNotifyManager::AddMsg( char* pMsg, int nClass )									// 메시지 추가함수( 메시지, 공지 타입 )
{
	int nLen = strlen( pMsg );															// 메시지 길이를 구한다.

	char buf[128];																		// 임시 버퍼를 선언한다.
	int nCpyNum = 0;																	// 복사 길이를 담을 변수를 선언하고 0으로 세팅한다.

	while( nLen > MAX_NOTIFYMSG_PER_LINE )												// 메시지 길이가 최대 메시지 길이를 넘으면,
	{
		if( ( pMsg + MAX_NOTIFYMSG_PER_LINE ) !=										// 메시지 더하기 라인 별 메시지 최대 길이가
			CharNext( CharPrev( pMsg, pMsg + MAX_NOTIFYMSG_PER_LINE ) ) )				// 현재 글자의 다음 글자를 체크해 2바이트 글자이면,
		{
			nCpyNum = MAX_NOTIFYMSG_PER_LINE - 1;										// 복사길이를 라인 별 최대 메시지 길이 -1로 세팅한다.
		}
		else																			// 그게 아니면,
		{
			nCpyNum = MAX_NOTIFYMSG_PER_LINE;											// 복사길이를 라인 별 최대 메시지 길이로 세팅한다.
		}

		strncpy( buf, pMsg, nCpyNum );													// 임시 버퍼에 복사 길이 만큼 메시지를 복사한다.
		buf[nCpyNum] = 0;																// 마지막을 0으로 세팅한다.
		PushMsg( buf, dwNotifyColor[nClass] );											// 공지 리스트에 복사한 메시지를 추가한다.
		nLen -= nCpyNum;																// 복사 길이 만큼 메시지 길이를 줄인다.
		pMsg += nCpyNum;																// 메시지 포인터를 이동한다.
		if( *pMsg == ' ' ) ++pMsg;														//라인 첫글자가 스페이스가 아니도록...				
	}

	if( nLen > 0 )																		// 길이가 0보다 크면,
		PushMsg( pMsg, dwNotifyColor[nClass] );											// 메시지를 추가한다.
}

void CGMNotifyManager::Render()															// 렌더 함수.
{
	if( m_ListNotify.GetCount() == 0 ) return;											// 공지 리스트 카운트가 0과 같으면 리턴 처리를 한다.

	if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN )								// 메인 게임 상태가 게임인 상태가 아니면,
	{
		return;																			// 리턴 처리를 한다.
	}

	sGMNOTIFY* pNotify = (sGMNOTIFY*)m_ListNotify.GetHead();							// 공지 구조체 정보를 받을 포인터를 선언하고 헤드의 정보를 받는다.

	if( gCurTime - pNotify->dwReceiveTime > NOTIFYMSG_LAST_TIME )						// 현재 시간에서 수락시간을 뺀것이 10초가 지나면,
	{
		PopMsg();																		// 리스트에서 메시지를 빼낸다.
	}

	LONG	lPosY = m_rcPos.top;														// 출력위치 높이를 담을 변수를 선언하고, 출력 영역 렉트의 탑으로 세팅한다.
	LONG	lPosX;																		// 출력 위치의 X좌표를 담을 변수를 선언한다.
	LONG	lTextWidth;																	// 텍스트의 길이를 담을 변수를 선언한다.
	RECT	rect;																		// 출력 영역을 위한 렉트를 선언한다.

	PTRLISTPOS pos = m_ListNotify.GetHeadPosition();									// 위치 정보를 받을 포인터 변수를 선언하고, 공지 리스트의 헤드를 받는다.

	while(pos)																			// 위치 정보가 유효할 동안 while문을 돌린다.
	{
		sGMNOTIFY* pNotify = (sGMNOTIFY *)m_ListNotify.GetNext(pos);					// 공지 구조체 정보를 받을 포인터를 선언하고 위치에 따른 정보를 받는다.

		lTextWidth = CFONT_OBJ->GetTextExtentEx( 5, pNotify->strMsg, pNotify->nStrLen );// 폰트와, 메시지, 길이에 따른 텍스트 가로 길이를 구한다.

		lPosX = m_rcPos.left + (m_rcPos.right - lTextWidth)/2 ;							// 출력 위치 X좌표를 받는다.

		SetRect( &rect, lPosX, lPosY, lPosX + lTextWidth, lPosY + 1 );					// 출력 위치를 설정한다.

		CFONT_OBJ->RenderFont( 5, pNotify->strMsg, pNotify->nStrLen, &rect, RGB_HALF( 30, 30, 30 ) );	// 글자를 출력한다.(그림자)
		OffsetRect( &rect, -1, -1 );													// 출력위치 옵셋을 한다.
		CFONT_OBJ->RenderFont( 5, pNotify->strMsg, pNotify->nStrLen, &rect, RGB_HALF( 255, 255, 255 ) );// 다시 한번 글자를 출력한다.(기본 텍스트)
		lPosY += m_nLineSpace + 10;														// 높이 값을 10증가 시킨다.
	}
}

void CGMNotifyManager::SetPosition( LONG lx, LONG ly, LONG lWidth )						// 위치 설정 함수.
{
	m_rcPos.left	= lx;																// 출력위치 left를 인자 lx로 세팅한다.
	m_rcPos.right	= lx + lWidth;														// 출력위치 right를 인자 lx + lWidth로 세팅한다.
	m_rcPos.top		= ly;																// 출력위치 top을 인자 ly로 세팅한다.
	m_rcPos.bottom	= m_rcPos.top + 1;													// 출력위치 bottom을 탑 +1로 세팅한다.
}

void CGMNotifyManager::SetEventNotifyStr( char* pStrTitle, char* pStrContext )			// 이벤트 공지 문자열을 세팅하는 함수.
{	
	SafeStrCpy( m_strEventNotifyTitle, pStrTitle, 32 );									// 이벤트 공지 제목을 세팅한다.
	SafeStrCpy( m_strEventNotifyContext, pStrContext, 128 );							// 이벤트 공지 내용을 세팅한다.
}

void CGMNotifyManager::SetEventNotify( BOOL bUse )										// 이벤트 공지의 사용 여부를 세팅하는 함수.
{	
	m_bUseEventNotify = bUse;															// 인자로 넘어온 값으로 이벤트 공지의 사용 여부를 세팅한다.
}
