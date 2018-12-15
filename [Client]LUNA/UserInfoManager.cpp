// UserInfoManager.cpp: implementation of the CUserInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UserInfoManager.h"

#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "QuestManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "QuestQuickViewDialog.h"

GLOBALTON(CUserInfoManager)

#define USERINFO_USERSTATE_VER 0x0006060801

CUserInfoManager::CUserInfoManager()
{
	m_strUserID[0] = 0;
	m_bMapChange = FALSE;
	m_dwSaveFolderName = 0;
}

CUserInfoManager::~CUserInfoManager()
{
	
}

void CUserInfoManager::SetSaveFolderName( DWORD dwUserID )
{
	const DWORD Key = 740705;

	m_dwSaveFolderName = dwUserID + Key;
}

void CUserInfoManager::LoadUserInfo( DWORD dwFlag )
{
	char strFilePath[MAX_PATH];

	if( dwFlag & eUIK_USERSTATE )
	{
		wsprintf( strFilePath, "%s\\data\\interface\\log\\%d\\%d.UIS", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName, m_dwSaveFolderName );
		LoadUserState( strFilePath );
	}

	if(0 < gHeroID)
	{
		if( dwFlag & eUIK_USERQUEST )
		{
			wsprintf(
				strFilePath,
				"%s\\data\\interface\\log\\%d\\%d.UIQ",
				DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
				m_dwSaveFolderName,
				gHeroID);
			LoadUserQuestQuickView( strFilePath ) ;
		}
	}
}

void CUserInfoManager::SaveUserInfo( DWORD dwFlag )
{
	if( dwFlag == eUIK_NONE ) return;

	char strFilePath[MAX_PATH];

	//make directory
	wsprintf( strFilePath, "%s\\data\\interface\\log", DIRECTORYMGR->GetFullDirectoryName(eLM_Root) );
	CreateDirectory( strFilePath, NULL );
	wsprintf( strFilePath, "%s\\data\\interface\\log\\%d", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName );
	CreateDirectory( strFilePath, NULL );

	if(0 < gHeroID)
	{
		if( dwFlag & eUIK_USERSTATE )
		{
			wsprintf( strFilePath, "%s\\data\\interface\\log\\%d\\%d.UIS", DIRECTORYMGR->GetFullDirectoryName(eLM_Root), m_dwSaveFolderName, m_dwSaveFolderName );
			SaveUserState( strFilePath );
		}

		if( dwFlag & eUIK_USERQUEST )
		{
			wsprintf(
				strFilePath,
				"%s\\data\\interface\\log\\%d\\%d.UIQ",
				DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
				m_dwSaveFolderName,
				gHeroID);
			SaveUserQuestQuickView( strFilePath );
		}
	}
}

void CUserInfoManager::LoadUserState( char* strFilePath )
{
	HANDLE hFile = CreateFile( strFilePath, GENERIC_READ, 0, NULL, 
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE ) return;

	DWORD dwSize;
	DWORD dwVersion;
	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) )
	{
		CloseHandle( hFile );
		return;
	}
	if( dwVersion != USERINFO_USERSTATE_VER )
	{
		CloseHandle( hFile );
		return;
	}

	sUSERINFO_USERSTATE UIS;
	if( !ReadFile( hFile, &UIS, sizeof(UIS), &dwSize, NULL ) )
	{
		CloseHandle( hFile );
		return;
	}

	CloseHandle( hFile );

	if(m_bMapChange && gHeroID)
	{
		CAMERA->SetDistance( 0, UIS.fZoomDistance, 0 );
	}
}

void CUserInfoManager::SaveUserState( char* strFilePath )
{
	if(0 == gHeroID)
	{
		return;
	}

	sUSERINFO_USERSTATE UIS;
	UIS.fZoomDistance = CAMERA->GetDistance(0);

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, 
								CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE ) return;

	DWORD dwSize;
	DWORD dwVersion = USERINFO_USERSTATE_VER;
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL );
	WriteFile( hFile, &UIS, sizeof(UIS), &dwSize, NULL );

	CloseHandle( hFile );
}

void CUserInfoManager::SaveUserQuestQuickView( char* strFilePath )
{
	sUSERINFO_QUESTQUICKVIEW UIQ = {0};
	CQuestQuickViewDialog* pQuickView = GAMEIN->GetQuestQuickViewDialog();

	if( !pQuickView ) return ;

	cPtrList* pList = NULL ;
	pList = pQuickView->GetViewList() ;

	if( !pList ) return ;

	DWORD* pIdx ;

	int nCount = 0 ;

	PTRLISTPOS pos = NULL ;
	pos = pList->GetHeadPosition() ;

	while(pos)
	{
		pIdx = NULL ;
		pIdx = (DWORD*)pList->GetNext(pos) ;

		if( !pIdx ) continue ;

		UIQ.dwQuestID[nCount] = *pIdx ;

		++nCount ;
	}

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,		// 파일 패스에 따라 파일을 생성한다.
							   FILE_ATTRIBUTE_NORMAL, NULL ) ;

	if( hFile == INVALID_HANDLE_VALUE )													// 결과가 실패 했으면 리턴 처리를 한다.
	{
		return ;
	}

	DWORD dwSize = 0;
	DWORD dwVersion = gHeroID;
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) ;						// 버전과 사이즈를 기록한다.
	WriteFile( hFile, &UIQ, sizeof(UIQ), &dwSize, NULL ) ;								// 퀘스트 인덱스 정보를 기록한다.

	CloseHandle( hFile ) ;																// 파일을 닫는다.
}

void CUserInfoManager::LoadUserQuestQuickView( char* strFilePath ) 						// 유저의 퀘스트 알림이 정보를 로딩하는 함수.
{
	HANDLE hFile = CreateFile( strFilePath, GENERIC_READ, 0, NULL,						// 지정된 파일 패스로 파일을 읽는다.
							   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL ) ;

	if( hFile == INVALID_HANDLE_VALUE )													// 실패하면, 
	{
		return ;																		// 리턴 처리를 한다.
	}

	DWORD dwSize ;																		// 사이즈를 담을 변수를 선언한다.
	DWORD dwVersion ;																	// 버전을 담을 변수를 선언한다.

	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwSize, NULL ) )					// 사이즈와 버전을 읽어들인다. 실패하면,
	{
		CloseHandle( hFile ) ;															// 파일을 닫는다.
		return ;																		// 리턴 처리를 한다.
	}
	else if(dwVersion != gHeroID)
	{
		CloseHandle( hFile ) ;															// 파일을 닫는다.
		return ;																		// 리턴 처리를 한다.
	}

	sUSERINFO_QUESTQUICKVIEW UIQ ;														// 퀘스트 알림이에 등록 된 퀘스트 아이디를 저장하는 구조체.

	if( !ReadFile( hFile, &UIQ, sizeof(UIQ), &dwSize, NULL ) )							// 퀘스트 아이디를 읽어들인다. 실패하면,
	{
		CloseHandle( hFile ) ;															// 파일을 닫는다.
		return ;																		// 리턴 처리를 한다.
	}

	CloseHandle( hFile ) ;																// 파일을 닫는다.

	QUESTMGR->LoadQuickViewInfoFromFile(UIQ.dwQuestID) ;				// 퀘스트 매니져에 읽어들인 정보를 세팅한다.
}

void CUserInfoManager::DeleteUserQuestInfo(DWORD playerIndex)
{
	char strFilePath[MAX_PATH];
	wsprintf(
		strFilePath,
		"%s\\data\\interface\\log\\%d\\%d.UIQ",
		DIRECTORYMGR->GetFullDirectoryName(eLM_Root),
		m_dwSaveFolderName,
		playerIndex);
	DeleteFile(strFilePath) ;
}


void CUserInfoManager::SetUserID( char* strID )
{
	strcpy( m_strUserID, strID );
}


//for Interface
BOOL CUserInfoManager::IsValidInterfacePos( RECT* prcCaption, LONG lX, LONG lY )
{
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();

	if( lX + prcCaption->left < 0 )		return FALSE;
	if( lX + prcCaption->right > (LONG)(dispInfo.dwWidth) ) return FALSE;
	if( lY + prcCaption->top < 0 )		return FALSE;
	if( lY + prcCaption->bottom > (LONG)(dispInfo.dwHeight) ) return FALSE;

	// 070202 LYW --- End.

	return TRUE;
}