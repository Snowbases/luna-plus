#include "stdafx.h"
#include "WorldMapDlg.h"
#include "MHFile.h"
#include "WindowIDEnum.h"
#include "cStatic.h"
#include "cButton.h"
#include "cWindowManager.h"
#include "BigMapDlg.h"
#include "GameResourceManager.h"
#include "MHMap.h"
#include "./interface/cFont.h"
#include "GlobalEventFunc.h"

CWorldMapDlg::CWorldMapDlg(void)
{
	m_PointerBtnID = 0;
	m_pPointerBtn = NULL;
	m_ActivatedMapnum = 0;

	m_nCurPosFontSize = 10;

	LoadWorldMapScript();
}

CWorldMapDlg::~CWorldMapDlg(void)
{
}

void CWorldMapDlg::LoadWorldMapScript()
{
	m_mapMapBtnList.clear();
	m_mapMapnumMapbtnid.clear();

	enum Block
	{
		BlockNone,
		BlockCurrentMapInfo,
		BlockPointer,
		BlockMap,
	}
	blockType = BlockNone;

	BOOL bIsComment = FALSE;
	stMapBtnInfo curMapBtnInfo;

	CMHFile file;
	file.Init( "./Data/Interface/Windows/WorldMapDefine.bin", "rb" );

	while( ! file.IsEOF() )
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// 중간 주석 제거
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		const char* delimit = " \n\t=,~";
		const char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "CurrentMapInfo", token ) )
		{
			blockType = BlockCurrentMapInfo;
		}
		else if( ! stricmp( "Pointer", token ) )
		{
			blockType = BlockPointer;
		}
		else if( ! stricmp( "Map", token ) )
		{
			blockType = BlockMap;
			ZeroMemory( &curMapBtnInfo, sizeof(curMapBtnInfo) );
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			switch( blockType )
			{
			case BlockCurrentMapInfo:
			case BlockPointer:
				{
					blockType = BlockNone;
				}
				break;
			case BlockMap:
				{
					blockType = BlockNone;
					m_mapMapBtnList.insert( MAPBTNINFO::value_type( curMapBtnInfo.imageButtonID, curMapBtnInfo ) );
					m_mapMapnumMapbtnid.insert( MAPNUM_MAPBTNID::value_type( curMapBtnInfo.mapnum, curMapBtnInfo.imageButtonID ) );
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockCurrentMapInfo:
			{
				if( ! stricmp( "staticID", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					m_MapNameStaticID = atoi( token );
				}
			}
			break;
		case BlockPointer:
			{
				if( ! stricmp( "buttonID", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					m_PointerBtnID = (LONG)atoi( token );
				}
			}
			break;

		case BlockMap:
			{
				if( ! stricmp( "num", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					curMapBtnInfo.mapnum = (MAPTYPE)atoi( token );
				}
				else if( ! stricmp( "buttonID", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					curMapBtnInfo.buttonID = (LONG)atoi( token );
				}
				else if( ! stricmp( "imageButtonID", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					curMapBtnInfo.imageButtonID = (LONG)atoi( token );
				}
				else if( ! stricmp( "pointerPos", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					curMapBtnInfo.pointerX = (LONG)atoi( token );

					token = strtok( 0, delimit );
					if( !token )	break;
					curMapBtnInfo.pointerY = (LONG)atoi( token );
				}
			}
			break;
		}
	}
}

void CWorldMapDlg::Linking()
{
	for( MAPBTNINFO::iterator mapbtnIter = m_mapMapBtnList.begin() ; mapbtnIter != m_mapMapBtnList.end() ; ++mapbtnIter )
	{
		stMapBtnInfo& mapBtnInfo = mapbtnIter->second;

		mapBtnInfo.pButton			= (cButton*)GetWindowForID( mapBtnInfo.buttonID );
		mapBtnInfo.pImageButton		= (cButton*)GetWindowForID( mapBtnInfo.imageButtonID );
	}

	m_pBigMapDlg = (CBigMapDlg*)WINDOWMGR->GetDlgInfoFromFile("Data/Interface/Windows/WorldToBigMap.bin", "rb");
	if( m_pBigMapDlg )
	{
		WINDOWMGR->AddWindow( m_pBigMapDlg );
		m_pBigMapDlg->Linking();
		m_pBigMapDlg->SetActive( FALSE );
		m_pBigMapDlg->SetcbFunc( BigMapDlgFunc );
	}

	m_pMapNameStatic = (cStatic*)GetWindowForID( m_MapNameStaticID );
	m_pPointerBtn = (cButton*)GetWindowForID( m_PointerBtnID );

	ActiveMap( MAP->GetMapNum(), TRUE );
}

void CWorldMapDlg::SetActive( BOOL bActive )
{
	cDialog::SetActive( bActive );

	if( bActive )
	{
		// 현재 맵 Animation 활성화
		ActiveMap( MAP->GetMapNum(), TRUE );
	}
	else
	{
		m_pBigMapDlg->SetActive( FALSE );
	}
}

DWORD CWorldMapDlg::ActionEvent( CMouse * mouseInfo )
{
	DWORD we = cDialog::ActionEvent( mouseInfo );
	if( !m_bActive )
	{
		return we;
	}

	if( we & ( WE_MOUSEOVER | WE_LBTNCLICK | WE_LBTNDBLCLICK | WE_RBTNCLICK | WE_RBTNDBLCLICK) )
	{
		for( MAPBTNINFO::const_iterator mapbtnIter = m_mapMapBtnList.begin() ; mapbtnIter != m_mapMapBtnList.end() ; ++mapbtnIter )
		{
			const stMapBtnInfo& mapBtnInfo = mapbtnIter->second;
			if( mapBtnInfo.pButton == NULL ||
				mapBtnInfo.pImageButton == NULL )
				continue;

			if( mapBtnInfo.pButton->GetBtnState() != mapBtnInfo.pImageButton->GetBtnState() )
			{
				mapBtnInfo.pButton->SetBtnState( mapBtnInfo.pImageButton->GetBtnState() );
			}
		}
	}

	return we;
}

void CWorldMapDlg::OnActionEvent( LONG id, void* p, DWORD event )
{
	if( !m_pBigMapDlg ||
		(event & WE_BTNCLICK) == 0 ||
		WINDOWMGR->IsDragWindow() )
		return;

	MAPBTNINFO::const_iterator mapbtnIter = m_mapMapBtnList.find( id );
	if( mapbtnIter == m_mapMapBtnList.end() )
		return;

	const stMapBtnInfo& mapBtnInfo = mapbtnIter->second;
	OpenMap( mapBtnInfo.mapnum );
}

void CWorldMapDlg::Render()
{
	if( !IsActive() )
		return;

	cDialog::Render();

	for( MAPBTNINFO::const_iterator mapbtnIter = m_mapMapBtnList.begin() ; mapbtnIter != m_mapMapBtnList.end() ; ++mapbtnIter )
	{
		const stMapBtnInfo& mapBtnInfo = mapbtnIter->second;
		if( mapBtnInfo.pButton == NULL ||
			mapBtnInfo.pImageButton == NULL )
			continue;

		switch( mapBtnInfo.pImageButton->GetBtnState() )
		{
		case cButton::eBtn_State_Over:
		case cButton::eBtn_State_Press:
			{
				// 이미지위에 텍스트이미지 출력
				mapBtnInfo.pImageButton->Render();
				mapBtnInfo.pButton->Render();
			}
			break;
		}
	}

	// 포인터는 항상 위에 출력.
	m_pPointerBtn->Render();
}

void CWorldMapDlg::OpenMap( MAPTYPE mapnum )
{
	if( !m_pBigMapDlg )
		return;

	m_pBigMapDlg->RemoveALLIcon();
	m_pBigMapDlg->InitBigMap( mapnum );

	m_pBigMapDlg->UseMovePath( (mapnum == MAP->GetMapNum()) );

	GAMERESRCMNGR->SetStaticNpcTablePositionHead();
	for( STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetNextStaticNpcInfo() ; pStaticNpcInfo > 0 ; pStaticNpcInfo = GAMERESRCMNGR->GetNextStaticNpcInfo() )
	{
		if( pStaticNpcInfo->MapNum != mapnum )
			continue;

		NPC_LIST* pNpcInfo = GAMERESRCMNGR->GetNpcInfo( pStaticNpcInfo->wNpcJob );
		if( !pNpcInfo )
			continue;

		m_pBigMapDlg->AddStaticNpcIcon( pNpcInfo->JobKind, pStaticNpcInfo->wNpcUniqueIdx, &pStaticNpcInfo->vPos, NULL );
	}

	m_pBigMapDlg->Process();
	m_pBigMapDlg->UpdateQuestMark();
	m_pBigMapDlg->SetActive( TRUE );
}

CWorldMapDlg::stMapBtnInfo& CWorldMapDlg::GetMapBtnInfo( MAPTYPE mapnum )
{
	static stMapBtnInfo emptyMapBtnInfo;

	MAPNUM_MAPBTNID::const_iterator mapnumMapbtnidIter = m_mapMapnumMapbtnid.find( mapnum );
	if( mapnumMapbtnidIter == m_mapMapnumMapbtnid.end() )
		return emptyMapBtnInfo;

	const DWORD dwButtonID = mapnumMapbtnidIter->second;
	MAPBTNINFO::iterator mapbtninfoIter = m_mapMapBtnList.find( dwButtonID );
	if( mapbtninfoIter == m_mapMapBtnList.end() )
		return emptyMapBtnInfo;

	return mapbtninfoIter->second;
}

void CWorldMapDlg::ActiveMap( MAPTYPE mapnum, BOOL bActive )
{
	stMapBtnInfo& mapBtnInfo = GetMapBtnInfo( mapnum );
	if( mapBtnInfo.mapnum != mapnum ||
		mapBtnInfo.pButton == NULL ||
		mapBtnInfo.pImageButton == NULL )
		return;

	// 이전활성화한 맵을 비활성화 시킨다.
	if( m_ActivatedMapnum != mapnum )
	{
		stMapBtnInfo& activatedMapBtnInfo = GetMapBtnInfo( m_ActivatedMapnum );
		if( activatedMapBtnInfo.pButton != NULL &&
			activatedMapBtnInfo.pImageButton != NULL )
		{
			activatedMapBtnInfo.pImageButton->SetActiveAnimationImage( FALSE );
		}
	}

	mapBtnInfo.pImageButton->SetActiveAnimationImage( bActive );

	// 맵 이름 설정
	if( m_pMapNameStatic )
	{
		m_pMapNameStatic->SetStaticText( GetMapName( mapnum ) );
	}

	// 포인터를 설정된 좌표로 이동
	if( m_pPointerBtn )
	{
		m_pPointerBtn->SetRelXY( mapBtnInfo.pointerX, mapBtnInfo.pointerY );
		m_pPointerBtn->SetAbsXY( (LONG)(GetAbsX() + mapBtnInfo.pointerX), (LONG)(GetAbsY() + mapBtnInfo.pointerY) );
	}
}