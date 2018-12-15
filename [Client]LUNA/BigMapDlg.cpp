#include "stdafx.h"
#include "BigMapDlg.h"
#include "MHFile.h"
#include "GameIn.h"
#include "Npc.h"
#include "Interface/cScriptManager.h"
#include "Interface/cWindowManager.h"
#include "Input/Mouse.h"
#include "MiniMapDlg.h"
#include "TutorialManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "./SiegeWarfareMgr.h"
#include "./MHMap.h"
#include "cHousingMgr.h"
#include "cButton.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "DungeonMgr.h"
#include "../[cc]header/GameResourceManager.h"
#include "QuestManager.h"
#include "WindowIDEnum.h"
#include "WorldMapDlg.h"
#include "Input/UserInput.h"

#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

const DWORD IconDescription[] = 
{
	// index, width, height
	0,	14,	18,			//eBMICON_SELF,
	1,	16,	16,			//eBMICON_PARTYMEMBER,
	2,	16,	16,			//eBMICON_FAMILYMEMBER,
	3,	16,	16,			//eBMICON_STORE,
	4,	16,	16,			//eBMICON_TRAINER,
	5,	16,	16,			//eBMICON_BANK,
	6,	16,	16,			//eBMICON_NOMALNPC,
	7,	16,	16,			//eBMICON_LOADINGPOINT,
	8,	16,	16,			//eBMICON_LOGIN,
	9,	20, 48,			//eBMICON_CAMERA,
	10,	18, 18,			//eBMICON_QUEST1,
	11,	18, 18,			//eBMICON_QUEST2,
	12,	18, 18,			//eBMICON_QUEST3,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.31
	6,	20, 48,			//eBMICON_FARM,
	// E 농장시스템 추가 added by hseos 2007.10.31

	// 090909 ShinJS --- 길찾기시 Move Point 출력용 이미지
	21, 27, 32,			//eBMICON_MOVEPOINT1,
	22, 27, 32,			//eBMICON_MOVEPOINT2,
	23,	27, 32,			//eBMICON_MOVEPOINT3,
	24,	27, 32,			//eBMICON_MOVEPOINT4,
	23,	27, 32,			//eBMICON_MOVEPOINT5,
	22,	27, 32,			//eBMICON_MOVEPOINT6,
};

#define MAP_WIDTH	512
#define MAP_HEIGHT	512

CBigMapDlg::CBigMapDlg()
{
	m_dwImageRGB = RGBA_MAKE( 255, 255, 255, 190 );
	m_IconTable.Initialize(64);
	m_IconPool.Init(100, 10, "MinimapIconPool");
	m_bUseMovePath = TRUE;
}

CBigMapDlg::~CBigMapDlg()
{
	RemoveALLIcon();
}

void CBigMapDlg::Linking()
{
	//memset(m_pIconImage,0,sizeof(cImage*)*eBMICON_MAX);
	ZeroMemory( m_pIconImage, sizeof( m_pIconImage ) );
	InitIconImage();
}

void CBigMapDlg::LoadMinimapImageInfo( MAPTYPE MapNum )
{
	DIRECTORYMGR->SetLoadMode(eLM_Minimap);

	char	name[ 80 ];
	CMHFile file;

	sprintf( name, "Minimap%d.bin", MapNum );
	file.Init( name, "rb", MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT );

	if( ! file.IsInited())
	{
		DIRECTORYMGR->SetLoadMode(eLM_Root);
		return;
	}
	
	for( int i = 0 ; i <eMINIMAPMODE_MAX ; ++i )
	{
		file.GetString( name );

		if( file.IsEOF() )
			break;
		
		file.GetDword();
		file.GetDword();

		if( eMINIMAPMODE_SMALL == i )
		{
			if( !m_MapImage.IsNull() )
				m_MapImage.Release();

			m_MapImage.LoadSprite( name );
			
			const cImageRect r = { 0, 0, MAP_HEIGHT, MAP_HEIGHT };

			m_MapImage.SetImageSrcRect( &r );
		}
	}
	
	DIRECTORYMGR->SetLoadMode(eLM_Root);
}

void CBigMapDlg::InitBigMap( MAPTYPE MapNum )
{
	LoadMinimapImageInfo( m_MapNum = MapNum );
}

void CBigMapDlg::AddStaticNpcIcon(CObject* pObject)
{
	CNpc* pNpc = (CNpc*)pObject;
	NPC_TOTALINFO* pInfo = pNpc->GetNpcTotalInfo();
	if( !pInfo )		return;
	AddStaticNpcIcon( pInfo->NpcJob, pObject->GetID(), NULL, pObject );
}

void CBigMapDlg::AddStaticNpcIcon( const WORD wNpcJob, const DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject )
{
	switch( wNpcJob )
	{
	case DEALER_ROLE:
		{
			AddIcon( eBMICON_STORE, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;

	case CHANGGO_ROLE:
		{
			AddIcon( eBMICON_BANK, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;

	case 7:
		{
			AddIcon( eBMICON_LOGIN, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;

	case MAP_MOVE_ROLE:
		{
			AddIcon( eBMICON_LOADINGPOINT, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;

	case 10:
		{
			AddIcon( eBMICON_TRAINER, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;

	case NOTE_ROLE:
	case SIEGEFLAGROLE_RUSHEN:
	case SIEGEFLAGROLE_ZEVYN:
	case CANDLESTAND_ROLE:
		// 080929 LYW --- BigMapDlg : 공성전으로 인한, 길드 깃발 촛대의 아이콘은 미니맵에 출력하지 않는다. ( 송가람 )
		// 우체통은 미니맵 아이콘 그리지 않는다.
		return;

	default:
		{
			AddIcon( eBMICON_NOMALNPC, dwObjectID, pObjectPos, pObject, NULL, NULL );
		}
		break;
	}
}

void CBigMapDlg::AddPartyMemberIcon(PARTY_MEMBER* pMember)
{
	AddIcon( eBMICON_PARTYMEMBER, pMember->dwMemberID, NULL, NULL, pMember, NULL );
}

void CBigMapDlg::AddHeroIcon(CObject* pObject)
{
	m_HeroIcon.Init( &m_pIconImage[eBMICON_SELF], eBMICON_SELF );
	m_HeroIcon.SetData( pObject->GetID(), NULL, NULL, NULL );
	m_HeroIcon.SetRGBColor( RGBA_MAKE(255,255,255,220) );
	m_HeroIcon.SetAlwaysTooltip( TRUE );
	m_HeroIcon.SetCameraImage( &m_pIconImage[eBMICON_CAMERA] );
}

void CBigMapDlg::AddFarmIcon(CSHFarm* pFarm)
{
	AddIcon( eBMICON_FARM, pFarm->GetID() + FARM_RENDER_OBJ_ID_START, NULL, NULL, NULL, pFarm );
}

void CBigMapDlg::ReleaseMinniMapIcon(CMiniMapIcon* pIcon)
{
	m_IconPool.Free(pIcon);
}

void CBigMapDlg::AddIcon( WORD Kind, DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject, PARTY_MEMBER* pMember, CSHFarm* pFarm )
{
	RemoveIcon( dwObjectID );

	CMiniMapIcon* pIcon = m_IconPool.Alloc();
	pIcon->Init( &m_pIconImage[Kind], Kind );
	pIcon->SetData( dwObjectID, pObjectPos, pMember, pFarm );

	// NPC인 경우 퀘스트마크 설정.
	STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)dwObjectID );
	if( pObject != NULL || pStaticNpcInfo != NULL )
		pIcon->SetMarkImage( &m_pIconImage[ eBMICON_QUEST1 ], 0, 3 );

	m_IconTable.Add( pIcon, dwObjectID );
	pIcon->SetRGBColor( RGBA_MAKE(255,255,255,220) );
	pIcon->SetAlwaysTooltip( TRUE );
}

void CBigMapDlg::RemoveIcon(DWORD ObjectID)
{
	CMiniMapIcon* pIcon = (CMiniMapIcon*)m_IconTable.GetData(ObjectID);

	if( pIcon )
	{
		ReleaseMinniMapIcon(pIcon);
		m_IconTable.Remove(ObjectID);
	}
}

void CBigMapDlg::RemoveALLIcon()
{
	CMiniMapIcon* pIcon = NULL;
	m_IconTable.SetPositionHead();
	while( (pIcon = m_IconTable.GetData()) != NULL )
	{
		ReleaseMinniMapIcon( pIcon );
	}
	m_IconTable.RemoveAll();
}

void CBigMapDlg::Render()
{
	if( !	m_bActive	||
			m_MapImage.IsNull() )
	{
		return;
	}

	cDialog::RenderWindow();

	if( this->IsMovedWnd() )
	{
		m_vScale.x =  MAP_WIDTH / 512.f ;
		m_vScale.y =  MAP_HEIGHT / 512.f ;

		SetMovedWnd( FALSE ) ;
	}

	m_MapImage.RenderSprite( &m_vScale, NULL, 0, &m_absPos, m_dwImageRGB );
	
	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		pIcon->Render( &m_absPos, NULL );	
		pIcon->RenderQuestMark(&m_absPos,NULL);
	}

	if( m_MapNum == MAP->GetMapNum() )
	{
		m_HeroIcon.Render(&m_absPos);

		// 090909 ShinJS --- Move Point 출력, 기존의 퀘스트마크 출력을 이용
		m_MovePoint.RenderQuestMark( &m_absPos, NULL );
	}

//가장위에 한번더
	if( m_pIconForToolTip )
	{
		m_pIconForToolTip->Render( &m_absPos, NULL );
		m_pIconForToolTip->RenderQuestMark( &m_absPos,NULL );
	}

	cDialog::RenderComponent();
}

void CBigMapDlg::SetPartyIconObjectPos(DWORD PlayerID, int posX, int posZ)
{
	CMiniMapIcon* pIcon = m_IconTable.GetData(PlayerID);

	if(pIcon)
	{
		pIcon->SetObjectPos(posX, posZ);
		pIcon->SetTargetPos(posX, posZ);
	}
}

void CBigMapDlg::SetPartyIconTargetPos(DWORD PlayerID, int posX, int posZ)
{
	CMiniMapIcon* pIcon = m_IconTable.GetData(PlayerID);

	if(pIcon)
	{
		pIcon->SetTargetPos(posX, posZ);
	}
}

BOOL CBigMapDlg::CanActive()
{
	if (g_csDateManager.IsChallengeZone(m_MapNum))
	{
		return FALSE;
	}
	else if ( HOUSINGMGR->IsHousingMap() )
	{
		return FALSE;
	}

	return TRUE;
}

void CBigMapDlg::InitIconImage()
{
	const SIZE_T size = sizeof( IconDescription ) / ( sizeof( DWORD ) * 3 );

	for( SIZE_T i = 0; i < size; ++i )
	{
		cImage*	image = new cImage;

		const DWORD index	= IconDescription[ 3 * i ];
		const DWORD width	= IconDescription[ 3 * i + 1 ];
		const DWORD height	= IconDescription[ 3 * i + 2 ];
		
		SCRIPTMGR->GetImage( index, image, PFT_MINIMAPPATH );

		m_pIconImage[ i ].InitIconImage( image, width, height );
	}

	SCRIPTMGR->GetImage( 0, &m_TooltipImage, PFT_HARDPATH );

	m_pIconForToolTip = NULL;

	// 090909 ShinJS --- Move Point 초기화
	m_MovePoint.Init( NULL, eMNMICON_MOVEPOINT1 );
	m_MovePoint.SetRGBColor( RGBA_MAKE(255,255,255,255) );
	m_MovePoint.SetMarkImage( &m_pIconImage[eMNMICON_MOVEPOINT1], 0, 6 );
	m_MovePoint.ShowQuestMark( eQM_NONE );
	m_MovePoint.SetImagePosRatio( 0.4f, 0.85f );
	m_MovePoint.SetFrameTick( 200 );
	m_MovePoint.SetUsePublicFrame( FALSE );
}

void CBigMapDlg::Process()
{
	if( ! IsActive() )
		return;

	m_HeroIcon.Update( MAP_WIDTH, MAP_HEIGHT );

	// 090909 ShinJS --- Move Point Update
	m_MovePoint.Update( MAP_WIDTH, MAP_HEIGHT );

	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		pIcon->Update( MAP_WIDTH, MAP_HEIGHT );
	}
}

DWORD CBigMapDlg::ActionEvent( CMouse* mouseInfo )
{
	const DWORD we = cDialog::ActionEvent( mouseInfo );

	// 활성화 상태가 아니거나 파괴된 상태면 처리 이유 없음
	if( !	m_bActive	||
			m_bDestroy )
	{
		return we;
	}
	
	m_dwViewCurTime = GetTickCount();

	if( !	WINDOWMGR->IsMouseOverUsed()									&&	// 마우스 커서가 윈도우에 위치한 상태이고
			PtInWindow( mouseInfo->GetMouseX(), mouseInfo->GetMouseY() )	&&	// 맵 창 영역안에 존재하며
			WINDOWMGR->GetDragDlg() != this									&& 	// 드래그 중인 창이 이 윈도우가 아니면
			TOOLTIP_DELAY_TIME < m_dwViewCurTime - m_dwViewLastTime	)			// 표시 가능한 시간이면
	{
		m_ttX = mouseInfo->GetMouseX();
		m_ttY = mouseInfo->GetMouseY();
	}
	else
	{
		m_dwViewLastTime = m_dwViewCurTime = 0;
	}

	// 툴팁을 표시한다
	{
		char text[ 128 ] = { 0, };
		VECTOR2 trans;
		trans.x = GetAbsX();
		trans.y = GetAbsY();
		
		float olddist = 10000.0f;	//충분히 큰수
		
		m_IconTable.SetPositionHead();

		for(CMiniMapIcon* pIcon = m_IconTable.GetData();
			0 < pIcon;
			pIcon = m_IconTable.GetData())
		{
			float dist;
					
			switch( pIcon->GetToolTip(	mouseInfo->GetMouseX(),
										mouseInfo->GetMouseY(),
										&trans,
										text,
										&dist ) )
			{
			case 1:	// NPC
				{
					if( dist < olddist )
					{
						m_pIconForToolTip	= pIcon;
						olddist				= dist;

						SetToolTip( text, RGB_HALF( 255, 255, 0 ), &m_TooltipImage, RGB_HALF( 0, 0, 0 ) );
					}
					
					break;
				}
			case 2:	// Party
				{
					if( dist < olddist )
					{
						m_pIconForToolTip	= pIcon;
						olddist				= dist;

						SetToolTip( text, RGB_HALF(185, 241, 69), &m_TooltipImage, RGB_HALF( 0, 0, 0 ) );
					}

					break;
				}
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.10.31
			case 3:	// Farm
				{
					if( dist < olddist )
					{
						m_pIconForToolTip	= pIcon;
						olddist				= dist;

						SetToolTip( text, RGB_HALF( 255, 255, 0 ), &m_TooltipImage, RGB_HALF( 0, 0, 0 ) );
					}

					break;
				}
			// E 농장시스템 추가 added by hseos 2007.10.31
			}
		}

		if( ! strlen( text ) )
		{
			SetToolTip( "" );

			m_pIconForToolTip = NULL;
		}
	}

	if( m_bDisable )
	{
		return we | ActionEventComponent( mouseInfo );
	}

	return we | ActionEventWindow( mouseInfo) | ActionEventComponent( mouseInfo );
}

void CBigMapDlg::OnActionEvent( LONG id, void* p, DWORD we )
{
	switch( id )
	{
	case BM_PATHFIND_BTN:
		{
			const BOOL IsInValidMove = ( m_bUseMovePath == FALSE ||
										(we & WE_BTNCLICK) == 0 || 
										WINDOWMGR->IsMouseDownUsed() == TRUE ||
										WINDOWMGR->IsDragWindow() );
			if( IsInValidMove )
			{
				break;
			}

			Move_UsePath();
		}
		break;
	}
}

void CBigMapDlg::ShowQuestMarkIcon( CObject* pTarget, int nKind )
{
	CMiniMapIcon* pIcon = m_IconTable.GetData( pTarget->GetID() );

	if( pIcon )
	{
		pIcon->ShowQuestMark( nKind );
	}
}

void CBigMapDlg::UpdateQuestMark()
{
	m_IconTable.SetPositionHead();
	for( CMiniMapIcon* pIcon = m_IconTable.GetData() ; pIcon > 0 ; pIcon = m_IconTable.GetData() )
	{
		eQUEST_MARK mark = eQM_NONE;
		const DWORD dwObjectID = pIcon->GetObjectID();
		CObject* pObject = OBJECTMGR->GetObject( dwObjectID );
		STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)dwObjectID );
		if( pObject != NULL && pObject->GetObjectKind() == eObjectKind_Npc )
		{
			CNpc* pNpc = (CNpc*)pObject;
			mark = QUESTMGR->GetQuestMarkState( pNpc->GetNpcUniqueIdx(), pNpc->GetID() );
		}
		else if( pStaticNpcInfo != NULL )
		{
			mark = QUESTMGR->GetQuestMarkState( pStaticNpcInfo->wNpcUniqueIdx, 0 );
		}

		pIcon->ShowQuestMark( mark );
	}
}

void CBigMapDlg::SetActive( BOOL isActive )
{
	// 080817 LYW --- BigMapDlg : 공성관련 맵이고 스케줄 관리맵이 아닌경우 BigMap을 비활성화 한다.
	// 091120 ShinJS --- 인던맵인 경우 비활성화한다.
	if( SIEGEWARFAREMGR->IsSiegeWarfareKindMap( m_MapNum ) && ! SIEGEWARFAREMGR->Is_CastleMap() ||
	 	DungeonMGR->IsDungeonMap( m_MapNum ) )
  	{
  		cDialog::SetActive( FALSE ) ;
  		return;
  	}

	cDialog::SetActive( CanActive() && isActive );

	TUTORIALMGR->Check_OpenDialog(this->GetID(), isActive);
}

void CBigMapDlg::SetMovePoint( VECTOR3* pDestPos )
{
	if( !pDestPos )
		return;

	m_MovePoint.ShowQuestMark( eQM_EXCLAMATION );
	m_MovePoint.SetObjectPos( (DWORD)pDestPos->x, (DWORD)pDestPos->z );
}

void CBigMapDlg::Move_UsePath()
{
	// 100610 ONS GM툴 미니맵이동 기능 추가.
#ifdef _GMTOOL_
	if( GMTOOLMGR->MoveOnMiniMap( (MOUSE->GetMouseX() - m_absPos.x), 
		(MAP_HEIGHT - (MOUSE->GetMouseY() - m_absPos.y)) ) )
	{
		return;
	}
#endif

    static DWORD dwNextValidMoveTime;
	if( dwNextValidMoveTime < gCurTime )
	{
		dwNextValidMoveTime = gCurTime + MOUSE_MOVE_TICK;

		// 선택된 아이콘 좌표로 이동
		VECTOR3 destPos;
		destPos.x = (MOUSE->GetMouseX() - m_absPos.x) * 100.0f;
		destPos.y = 0.0f;
		destPos.z = (MAP_HEIGHT - (MOUSE->GetMouseY() - m_absPos.y)) * 100.0f;

		// 100304 ShinJS --- 이동처리는 MoveManager에서 검사후 진행하도록 수정
		if( MOVEMGR->SetHeroTarget( &destPos, TRUE ) )
		{
			SetMovePoint( &destPos );

			// 목적지를 표시한다
			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
			CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
			if( pMiniMapDlg )		pMiniMapDlg->SetMovePoint( &destPos );
			if( pWorldToBigMapDlg )	pWorldToBigMapDlg->SetMovePoint( &destPos );
		}
	}
}