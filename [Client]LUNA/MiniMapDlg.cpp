#include "stdafx.h"
#include "MiniMapDlg.h"
#include "BigMapDlg.h"
#include "Interface/cWindowManager.h"
#include "Interface/cStatic.h"
#include "Interface/cPushupButton.h"
#include "Interface/cScriptManager.h"
#include "Npc.h"
#include "WindowIDEnum.h"
#include "Input/UserInput.h"
#include "mhFile.h"
#include "GameIn.h"
#include "ChatManager.h"
#include "PartyIconManager.h"
#include "mhMap.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Date/SHDateManager.h"
#include "../Interface/cResourceManager.h"
#include "./SiegeWarfareMgr.h"
#include "cHousingMgr.h"
#include "ObjectManager.h"
#include "DungeonMgr.h"
#include "VideoCaptureManager.h"
#include "MoveManager.h"
#include "CharacterDialog.h"
#include "cSkilltreedlg.h"
#include "ChannelComboBox.h"
#include "cListCtrl.h"
#include "CharSelect.h"
#include "ChannelDialog.h"
#include "../[cc]header/GameResourceManager.h"
#include "QuestManager.h"
#include "WorldMapDlg.h"

extern int g_nServerSetNum;

const DWORD IconDescription[] = 
{
	// index, relative x, relative y
	// 이미지의 
	 //0,	9,	9,			//eMNMICON_SELF,
	0, 14, 18,
	13,	8,	8,			//eMNMICON_PARTYMEMBER,
	14,	8,	8,			//eMNMICON_FAMILYMEMBER,
	15,	8,	8,			//eMNMICON_NORMALNPC,
	16,	8,	8,			//eMNMICON_LOADINGPOINT,
	//17, 16, 44,			//eMNMICON_CAMERA,
	9, 20, 48,
	18, 11, 11,			//eMNMICON_QUEST1,
	19, 11, 11,			//eMNMICON_QUEST2,
	20,	11, 11,			//eMNMICON_QUEST3,
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.31
	0,	0, 0,			//eMNMICON_NULL01,
	0,	0, 0,			//eMNMICON_NULL02,
	0,	0, 0,			//eMNMICON_NULL03,
	0,	0, 0,			//eMNMICON_NULL04,
	6,	8, 8,			//eMNMICON_FARM,
	// E 농장시스템 추가 added by hseos 2007.10.31

	// 090909 ShinJS --- 길찾기시 Move Point 출력용 이미지
	21, 27, 32,			//eMNMICON_MOVEPOINT1,
	22, 27, 32,			//eMNMICON_MOVEPOINT2,
	23,	27, 32,			//eMNMICON_MOVEPOINT3,
	24,	27, 32,			//eMNMICON_MOVEPOINT4,
	23,	27, 32,			//eMNMICON_MOVEPOINT5,
	22,	27, 32,			//eMNMICON_MOVEPOINT6,
};


extern int g_MapChannelNum[MAX_MAP_NUM];


CMiniMapDlg::CMiniMapDlg()
{
	m_pMapName		= NULL;
	// 080121 LYW --- MiniMapDlg : 채널용 스태틱 추가.
	m_pPosXY		= NULL;
	m_CurMode		= eMINIMAPMODE_SMALL;
	m_dwImageRGB	= RGBA_MAKE( 255, 255, 255, 190 );
	
	m_IconTable.Initialize(64);
	
	m_IconPool.Init(100, 10, "MinimapIconPool");

	// 미니맵 상수
	{
		/// 표시할 크기
		m_MiniMap.mViewport.x = 155;
		m_MiniMap.mViewport.y = 166;

		/// 윈도우 틀만큼 이동한 후 미니맵을 표시해야한다.
		m_MiniMap.mTranslation.x = 6;
		m_MiniMap.mTranslation.y = 28;
	}

	m_pVideoCaptureStartBtn = NULL;
	m_pVideoCaptureStopBtn = NULL;

	m_pLevelUpBTN	=	NULL;

	m_pChannelComboBox	=	NULL;
}


CMiniMapDlg::~CMiniMapDlg()
{	
	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		ReleaseMinniMapIcon(pIcon);
	}

	m_IconTable.RemoveAll();
}

void CMiniMapDlg::InitMiniMap(MAPTYPE MapNum)
{
	LoadMinimapImageInfo( m_MapNum = MapNum );

	if( m_pMapName )
	{
		m_pMapName->SetStaticText(GetMapName(MapNum)) ;
	}

	// 090703 ShinJS --- 초기화시 SetActive()를 실행하여 필요하지 않은 맵에서 보이지 않도록 한다
	SetActive( TRUE );
}

void CMiniMapDlg::InitIconImage()
{
	const SIZE_T size = sizeof( IconDescription ) / ( sizeof( DWORD ) * 3 );

	for( SIZE_T i = 0; i < size; ++i )
	{
		cImage* image = new cImage;

		const DWORD index	= IconDescription[ 3 * i ];
		const DWORD x		= IconDescription[ 3 * i + 1 ];
		const DWORD y		= IconDescription[ 3 * i + 2 ];
		
		SCRIPTMGR->GetImage( index, image, PFT_MINIMAPPATH );

		m_pIconImage[ i ].InitIconImage( image, x, y );
	}

	// 090909 ShinJS --- Move Point 초기화
	m_MovePoint.Init( NULL, eMNMICON_MOVEPOINT1 );
	m_MovePoint.SetRGBColor( RGBA_MAKE(255,255,255,255) );
	m_MovePoint.SetMarkImage( &m_pIconImage[eMNMICON_MOVEPOINT1], 0, 6 );
	m_MovePoint.ShowQuestMark( eQM_NONE );
	m_MovePoint.SetImagePosRatio( 0.5f, 0.85f );
	m_MovePoint.SetFrameTick( 200 );
	m_MovePoint.SetUsePublicFrame( FALSE );

	SCRIPTMGR->GetImage( 0, &m_TooltipImage, PFT_HARDPATH );
}

void CMiniMapDlg::Linking()
{
	memset(m_pIconImage,0,sizeof(cImage*)*eMNMICON_MAX);
	InitIconImage();
	m_pMapName	= (cStatic*)GetWindowForID(MNM_MAPNAME);
	m_pPosXY	= (cStatic*)GetWindowForID(MNM_POSXY);

	m_pVideoCaptureStartBtn = (cButton*)GetWindowForID( MNM_VIDEO_REC_START_BTN );
	m_pVideoCaptureStopBtn  = (cButton*)GetWindowForID( MNM_VIDEO_REC_STOP_BTN );


	// 091214 ShinJS --- 녹화 상태에 맞게 버튼을 설정한다.
	ShowVideoCaptureStartBtn( ! VIDEOCAPTUREMGR->IsCapturing() );


	// 100104 ShinJS --- PC방 Mall 버튼
	cWindow* pMallBtn = GetWindowForID( MNM_PCROOM_MALL_BTN );
	if( pMallBtn )
		pMallBtn->SetActive( TRUE );

	// 100105 ShinJS --- PC방 아이템 지급 버튼
	cWindow* pProvideItemBtn = GetWindowForID( MNM_PCROOM_PROVIDE_ITEM_BTN );
	if( pProvideItemBtn )
		pProvideItemBtn->SetActive( FALSE );

	cPushupButton* pLevelUpBTN	=	(cPushupButton*)GetWindowForID( MNM_LEVELUP_BTN );
	if( pLevelUpBTN )
	{
		pLevelUpBTN->SetActive( FALSE );
		m_pLevelUpBTN	=	pLevelUpBTN;
	}

	cWindow* pLevelUpOffBTN =	GetWindowForID( MNM_LEVELUP_OFF_BTN );
	if( pLevelUpOffBTN )
	{
		pLevelUpOffBTN->SetActive( TRUE );
		pLevelUpOffBTN->SetDisable( TRUE );
	}

	// 채널 변경...
	cChannelComboBox* pComboBox	=	(cChannelComboBox*)GetWindowForID( MNM_CHANNEL_COMBOBOX );
	if( pComboBox )
	{
		m_pChannelComboBox	=	pComboBox;
		m_pChannelComboBox->LoadChannelInfo();
	}
}

void CMiniMapDlg::AddMonsterIcon(CObject* pObject)
{
}
void CMiniMapDlg::AddBossMonsterIcon(CObject* pObject)
{
}
void CMiniMapDlg::AddStaticNpcIcon(CObject* pObject)
{
	CNpc* pNpc = (CNpc*)pObject;
	switch( pNpc->GetNpcJob() )
	{
	case NOTE_ROLE:
	case SIEGEFLAGROLE_RUSHEN:
	case SIEGEFLAGROLE_ZEVYN:
	case CANDLESTAND_ROLE:
		// 우체통은 미니맵 아이콘 그리지 않는다.
		// 080929 LYW --- MiniMapDlg : 공성전으로 인한, 길드 깃발 촛대의 아이콘은 미니맵에 출력하지 않는다. ( 송가람 )
		return;

	case 30:
	case 31:
		{
			AddIcon( eMNMICON_LOADINGPOINT, pObject->GetID(), NULL, pObject, NULL, NULL );
		}
		break;

	default:
		{
			AddIcon( eMNMICON_NORMALNPC, pObject->GetID(), NULL, pObject, NULL, NULL );
		}
		break;

	}

	CBigMapDlg* dialog = GAMEIN->GetBigMapDialog();
	if( dialog )
	{
		dialog->AddStaticNpcIcon( pObject );
	}
}

void CMiniMapDlg::AddPartyMemberIcon(PARTY_MEMBER* pMember)
{
	AddIcon( eMNMICON_PARTYMEMBER, pMember->dwMemberID, NULL, NULL, pMember, NULL );

	CBigMapDlg* dialog = GAMEIN->GetBigMapDialog();
	if( dialog )
	{
		dialog->AddPartyMemberIcon( pMember );
	}
}

void CMiniMapDlg::AddHeroIcon(CObject* pObject)
{
	m_HeroIcon.Init( &m_pIconImage[eMNMICON_SELF], eMNMICON_SELF );
	m_HeroIcon.SetData( pObject->GetID(), NULL, NULL, NULL );
	m_HeroIcon.SetCameraImage( &m_pIconImage[eMNMICON_CAMERA] );

	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	if( pBigMapDlg )
	{
		pBigMapDlg->AddHeroIcon( pObject );
	}
	pBigMapDlg = GAMEIN->GetWorldMapDialog()->GetBigMapDlg();
	if( pBigMapDlg )
	{
		pBigMapDlg->AddHeroIcon( pObject );
	}
}

void CMiniMapDlg::AddFarmIcon(CSHFarm* pFarm)
{
	AddIcon( eMNMICON_FARM, pFarm->GetID() + FARM_RENDER_OBJ_ID_START, NULL, NULL, NULL, pFarm );

	CBigMapDlg* dialog = GAMEIN->GetBigMapDialog();
	if( dialog )
	{
		dialog->AddFarmIcon(pFarm);
	}
}

void CMiniMapDlg::ReleaseMinniMapIcon(CMiniMapIcon* pIcon)
{
	m_IconPool.Free(pIcon);
}

void CMiniMapDlg::AddIcon( WORD Kind, DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject, PARTY_MEMBER* pMember, CSHFarm* pFarm )
{
	RemoveIcon( dwObjectID );

	CMiniMapIcon* pIcon = m_IconPool.Alloc();
	pIcon->Init( &m_pIconImage[Kind], Kind );
	pIcon->SetData( dwObjectID, pObjectPos, pMember, pFarm );
	if( pObject )
	{
		pIcon->SetMarkImage( &m_pIconImage[ eMNMICON_QUEST1 ], 0, 3 );
	}

	m_IconTable.Add( pIcon, dwObjectID );
}

void CMiniMapDlg::RemoveIcon(DWORD ObjectID)
{
	CMiniMapIcon* pIcon = m_IconTable.GetData( ObjectID );

	if( pIcon )
	{
		ReleaseMinniMapIcon(pIcon);
		m_IconTable.Remove(ObjectID);
	}
}

void CMiniMapDlg::Process()
{
	const MINIMAPIMAGE& pMinimap = m_MinimapImage[m_CurMode];
	DWORD iWidth,iHeight;

	if(m_CurMode == eMINIMAPMODE_FULL)
	{
		iWidth = (DWORD)m_MiniMap.mViewport.x;
		iHeight = (DWORD)m_MiniMap.mViewport.y;
	}
	else
	{
		iWidth = pMinimap.size.cx;
		iHeight = pMinimap.size.cy;
	}

	m_HeroIcon.Update(iWidth,iHeight);
	// 090909 ShinJS --- Move Point Update
	m_MovePoint.Update( iWidth, iHeight );

	PICONMGR->Process();
	
	if(m_pPosXY)
	{
		char text[128];
		VECTOR3 heropos = m_HeroIcon.GetPosition();

		sprintf( text, "%4.0f,%4.0f", heropos.x * 0.01f,heropos.z * 0.01f );
		m_pPosXY->SetStaticText(text);
	}

	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		pIcon->Update(iWidth,iHeight);
	}

	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	if( pBigMapDlg )
	{
		pBigMapDlg->Process();
	}
	pBigMapDlg = GAMEIN->GetWorldMapDialog()->GetBigMapDlg();
	if( pBigMapDlg )
	{
		pBigMapDlg->Process();
	}

	// change map image
	{
		cImageRect& rect = m_MiniMap.mRect;

		if( eMINIMAPMODE_FULL == m_CurMode )
		{
			rect.left = 0;
			rect.top = 0;
		}
		else
		{
			const VECTOR2* heroIconPosition = m_HeroIcon.GetScreenPos();

			rect.left = ( LONG )( heroIconPosition->x - m_MiniMap.mViewport.x / 2.0f );
			rect.top = ( LONG )( heroIconPosition->y - m_MiniMap.mViewport.y / 2.0f );
		}

		rect.right = rect.left + ( LONG )m_MiniMap.mViewport.x;
		rect.bottom = rect.top + ( LONG )m_MiniMap.mViewport.y;

		m_MiniMap.mImage.SetImageSrcRect( &rect );
	}
}

void CMiniMapDlg::LoadMinimapImageInfo(MAPTYPE MapNum)
{
	DIRECTORYMGR->SetLoadMode(eLM_Minimap);

	char buffer[256];
	CMHFile file;

	sprintf( buffer, "Minimap%d.bin", MapNum );
	file.Init( buffer, "rb", MHFILE_FLAG_DONOTCHANGE_DIRECTORY_TOROOT );

	if( ! file.IsInited() )
	{
		DIRECTORYMGR->SetLoadMode(eLM_Root);
		return;
	}
	
	char name[ 256 ];

	{
		file.GetString( buffer );
		sprintf( name, "../image/%s", buffer );

		MINIMAPIMAGE& data = m_MinimapImage[ eMINIMAPMODE_SMALL ];

		data.size.cx = file.GetDword();
		data.size.cy = file.GetDword();
		data.image.LoadSprite( name );
	}
	
	{
		file.GetString( buffer );
		sprintf( name, "../image/%s", buffer );

		MINIMAPIMAGE& data = m_MinimapImage[ eMINIMAPMODE_FULL ];

		data.size.cx = file.GetDword();
		data.size.cy = file.GetDword();
		data.image.LoadSprite( name );
	}
	
	//file.Release();	
	DIRECTORYMGR->SetLoadMode(eLM_Root);

	CBigMapDlg* dialog = GAMEIN->GetBigMapDialog();
	if( dialog )
	{
		dialog->LoadMinimapImageInfo( MapNum );
	}

	/// 미니맵 초기화
	{
		if( eMINIMAPMODE_SMALL == m_CurMode )
		{
			const SIZE& size = m_MinimapImage[ eMINIMAPMODE_SMALL ].size;

			m_MiniMap.mScale.x = m_MiniMap.mViewport.x / size.cx;
			m_MiniMap.mScale.y = m_MiniMap.mViewport.y / size.cy;
		}
		else
		{
			m_MiniMap.mScale.x = 1.0f;
			m_MiniMap.mScale.y = 1.0f;
		}

		m_MiniMap.mImage.SetSpriteObject( m_MinimapImage[ m_CurMode ].image.GetSpriteObject() );
		m_MiniMap.mImage.SetImageSrcSize( &m_MiniMap.mViewport );
	}
}

DWORD CMiniMapDlg::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = cDialog::ActionEvent( mouseInfo );

	if( ! m_bActive )
	{
		return we;
	}

	if( we & WE_MOUSEOVER )
	{
		cImageRect& rect = m_MiniMap.mRect;
		char text[128] = {0,};
		
		VECTOR2 trans;
		trans.x = GetAbsX() + m_MiniMap.mTranslation.x - rect.left;
		trans.y = GetAbsY() + m_MiniMap.mTranslation.y - rect.top;

		m_IconTable.SetPositionHead();

		for(CMiniMapIcon* pIcon = m_IconTable.GetData();
			0 < pIcon;
			pIcon = m_IconTable.GetData())
		{
			int sort = pIcon->GetToolTip(mouseInfo->GetMouseX(),mouseInfo->GetMouseY(),&trans,text);

			if(sort == 1)				// Npc
			{
				SetToolTip( text, RGB_HALF( 255, 255, 0 ), &m_TooltipImage, RGB_HALF( 0, 0, 0 )  );
			}
			else if(sort == 2)			// Party
			{
				SetToolTip( text, RGB_HALF(185, 241, 69), &m_TooltipImage, RGB_HALF( 0, 0, 0 )  );
			}
			else if(sort == 3)			// Farm
			{
				SetToolTip( text, RGB_HALF( 255, 255, 0 ), &m_TooltipImage, RGB_HALF( 0, 0, 0 )  );
			}
		}
 
		if( ! strlen( text ) )
		{
			SetToolTip("");
		}
	}

	return we;
}

void CMiniMapDlg::Render()
{
	if( !	m_bActive ||
			m_MiniMap.mImage.IsNull() )
	{
		return;
	}
	// 100612 레벨업 버튼 활성화시
	if( m_pLevelUpBTN )
	{
		if( m_pLevelUpBTN->IsActive() && !(m_pLevelUpBTN->IsPushed()) )
		{
			DWORD dwCurTick		=	GetTickCount();
			DWORD dwResultTime	=	dwCurTick - m_dwLevelUPBTNStartTime;

			if( dwResultTime > 1000 )
			{
				m_dwLevelUPBTNStartTime = GetTickCount();
			}
			m_pLevelUpBTN->SetAlpha((BYTE)(2.5f * (dwResultTime/10)) );
		}
		else
			m_pLevelUpBTN->SetAlpha( 255 );
	}


	cDialog::RenderWindow();
	
	VECTOR2 trans;
	trans.x = GetAbsX() + m_MiniMap.mTranslation.x;
	trans.y = GetAbsY() + m_MiniMap.mTranslation.y;
	
	m_MiniMap.mImage.RenderSprite( &m_MiniMap.mScale, 0, 0, &trans, m_dwImageRGB );

	cImageRect* rect = &m_MiniMap.mRect;

	trans.x -= rect->left;
	trans.y -= rect->top;
	
	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		pIcon->Render(&trans,rect);
	}

	m_IconTable.SetPositionHead();

	for(CMiniMapIcon* pIcon = m_IconTable.GetData();
		0 < pIcon;
		pIcon = m_IconTable.GetData())
	{
		pIcon->RenderQuestMark(&trans,rect);
	}

	m_HeroIcon.Render(&trans);

	// 090909 ShinJS --- Move Point 출력, 기존의 퀘스트마크 출력을 이용
	m_MovePoint.RenderQuestMark( &trans, rect );

	cDialog::RenderComponent();
}

void CMiniMapDlg::SetActive( BOOL val )
{
	// 080817 LYW --- MiniMapDlg : 공성관련 맵이고 스케줄 관리맵이 아닌경우 BigMap을 비활성화 한다.
	// 091120 ShinJS --- 인던맵인 경우 비활성화한다.
	if( (SIEGEWARFAREMGR->IsSiegeWarfareKindMap(MAP->GetMapNum()) && ! SIEGEWARFAREMGR->Is_CastleMap()) ||
		DungeonMGR->IsDungeonMap( MAP->GetMapNum() ) )
   	{
   		cDialog::SetActive( FALSE ) ;
   		return ;
   	}

	if( CanActive() == FALSE )
		val = FALSE;
	
	cDialog::SetActive( val );
}


void CMiniMapDlg::SetPartyIconTargetPos(DWORD PlayerID, int posX, int posZ)
{
	CMiniMapIcon* pIcon = m_IconTable.GetData(PlayerID);
	if(pIcon)
		pIcon->SetTargetPos(posX, posZ);

	if( GAMEIN->GetBigMapDialog() )
		GAMEIN->GetBigMapDialog()->SetPartyIconTargetPos( PlayerID, posX, posZ );
}

void CMiniMapDlg::SetPartyIconObjectPos(DWORD PlayerID, int posX, int posZ)
{
	CMiniMapIcon* pIcon = m_IconTable.GetData(PlayerID);
	if(pIcon)
	{
		pIcon->SetObjectPos(posX, posZ);
		pIcon->SetTargetPos(posX, posZ);
	}

	if( GAMEIN->GetBigMapDialog() )
		GAMEIN->GetBigMapDialog()->SetPartyIconObjectPos( PlayerID, posX, posZ );
}

BOOL CMiniMapDlg::CanActive()
{
	if (g_csDateManager.IsChallengeZone(MAP->GetMapNum()))
	{
		return FALSE;
	}

	// 090525 pdy  하우징 맵엔 미니맵이 없다.
	if ( HOUSINGMGR->IsHousingMap() )
	{
		return FALSE;
	}

	return TRUE;
}

void CMiniMapDlg::UpdateQuestMark()
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

			if( GAMEIN->GetBigMapDialog() )
				GAMEIN->GetBigMapDialog()->ShowQuestMarkIcon( pNpc, mark );
			
		}
		else if( pStaticNpcInfo != NULL )
		{
			mark = QUESTMGR->GetQuestMarkState( pStaticNpcInfo->wNpcUniqueIdx, 0 );
		}

		pIcon->ShowQuestMark( mark );
		
	}
}

void CMiniMapDlg::SetMovePoint( VECTOR3* pDestPos )
{
	if( !pDestPos )
		return;

	m_MovePoint.ShowQuestMark( eQM_EXCLAMATION );
	m_MovePoint.SetObjectPos( (DWORD)pDestPos->x, (DWORD)pDestPos->z );
}

void CMiniMapDlg::Move_UsePath()
{
	static DWORD dwMouseMoveTick;

	if( gCurTime - dwMouseMoveTick >= MOUSE_MOVE_TICK )
	{
		dwMouseMoveTick = gCurTime;

		const MINIMAPIMAGE& pMinimap = m_MinimapImage[m_CurMode];
		DWORD iWidth,iHeight;
		if(m_CurMode == eMINIMAPMODE_FULL)
		{
			iWidth = (DWORD)m_MiniMap.mViewport.x;
			iHeight = (DWORD)m_MiniMap.mViewport.y;
		}
		else
		{
			iWidth = pMinimap.size.cx;
			iHeight = pMinimap.size.cy;
		}

		int nMouseX = g_UserInput.GetMouse()->GetMouseEventX();
		int nMouseY = g_UserInput.GetMouse()->GetMouseEventY();

		VECTOR3 destPos;
		destPos.x = ((nMouseX - m_absPos.x + m_MiniMap.mRect.left - m_MiniMap.mTranslation.x) / iWidth) * 51200.0f;
		destPos.y = 0.0f;
		destPos.z = (iHeight - (nMouseY - m_absPos.y + m_MiniMap.mRect.top - m_MiniMap.mTranslation.y)) / iHeight * 51200.0f;

		// 100304 ShinJS --- 이동처리는 MoveManager에서 검사후 진행하도록 수정
		if( MOVEMGR->SetHeroTarget( &destPos, TRUE ) )
		{
			SetMovePoint( &destPos );

			// 목적지를 표시한다
			CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
			CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
			CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
			if( pBigMapDlg )		pBigMapDlg->SetMovePoint( &destPos );
			if( pWorldToBigMapDlg )	pWorldToBigMapDlg->SetMovePoint( &destPos );
		}
	}
}

void CMiniMapDlg::ShowVideoCaptureStartBtn( BOOL bShow )
{
	m_pVideoCaptureStartBtn->SetActive( bShow );
	m_pVideoCaptureStopBtn->SetActive( !bShow );
}

//레벨업 버튼의 활성여부에 따라 버튼의 상태를 바꾼다.
void CMiniMapDlg::ChangeLevelUpBTNState()	
{
	const HERO_TOTALINFO*	heroInfo	=	HERO->GetHeroTotalInfo();

	cPushupButton* pLevelUpButton	= (cPushupButton*)GetWindowForID( MNM_LEVELUP_BTN );
	cPushupButton* pLevelUpOffButton =  (cPushupButton*)GetWindowForID( MNM_LEVELUP_OFF_BTN );

	if( !(pLevelUpButton && pLevelUpOffButton) )
		return;
		

	if( heroInfo->SkillPoint < MINIMUM_SKILL_POINT && heroInfo->LevelUpPoint < MINIMUM_LEVELUP_POINT )
	{
		pLevelUpButton->SetDisable( FALSE );
		pLevelUpButton->SetActive( FALSE );

		pLevelUpOffButton->SetDisable( FALSE );
		pLevelUpOffButton->SetActive( TRUE);
	}
	else
	{
		pLevelUpButton->SetDisable( FALSE );
		pLevelUpButton->SetActive( TRUE );

		pLevelUpOffButton->SetDisable( FALSE );
		pLevelUpOffButton->SetActive( FALSE);

		ActiveLevelUpButton();
	}
}

//레벨업 버튼이 눌렸느지 않눌렸는지 검사한다.
void CMiniMapDlg::ChangePushLevelUpBTN()	
{
	cPushupButton* pLevelUpButton		= m_pLevelUpBTN;
	cPushupButton* pLevelUpOffButton	=	(cPushupButton*)GetWindowForID( MNM_LEVELUP_OFF_BTN );

	ChangeLevelUpBTNState();

	if( !(pLevelUpButton && pLevelUpOffButton) )
		return;

	if( GAMEIN->GetSkillTreeDlg()->IsActive() || GAMEIN->GetCharacterDialog()->IsActive() )
	{
		pLevelUpButton->SetPush( TRUE );
		pLevelUpButton->SetDisable( TRUE );	
	}
	else if( !(GAMEIN->GetSkillTreeDlg()->IsActive()) && !(GAMEIN->GetCharacterDialog()->IsActive()) )
	{
		pLevelUpButton->SetPush( FALSE );
		pLevelUpButton->SetDisable( FALSE );
	}

}
//레벨업 버튼의 알파렌더를 시작한다.
void CMiniMapDlg::ActiveLevelUpButton()
{
	m_pLevelUpBTN->SetAlpha( 255 );
	m_dwLevelUPBTNStartTime	=	GetTickCount();
}

void CMiniMapDlg::SetChanelComboBox( MSG_CHANNEL_INFO* pInfo )
{
	m_pChannelComboBox->RemoveAllChannelComboItem();

	int		iMapNum	=	MAP->GetMapNum();
	
	// 100618 클라이언트에서 맵의 채널 개수를 얻어온다.
	int	iChannelCount	=	g_MapChannelNum[iMapNum];
	
	if( iChannelCount > CHARSELECT->GetMaxChannel( g_nServerSetNum ) )
		iChannelCount = CHARSELECT->GetMaxChannel( g_nServerSetNum );

	char buf[MAX_PATH]	=	{ 0, };

	for( int i = 0; i < iChannelCount; ++i )
	{
		sprintf( buf, "%s %d", pInfo->ChannelName, i + 1 );
		m_pChannelComboBox->InsertComboItem( buf );

		if(pInfo->PlayerNum[i] < m_pChannelComboBox->GetFreeCount() )
		{
			m_pChannelComboBox->InsertChannelInfo( RESRCMGR->GetMsg( 286 ), m_pChannelComboBox->GetFreeColor() );
		}
		else if(pInfo->PlayerNum[i] < m_pChannelComboBox->GetBusyCount() )	//MAX USER PER CHANNEL
		{
			m_pChannelComboBox->InsertChannelInfo( RESRCMGR->GetMsg( 287 ), m_pChannelComboBox->GetBusyColor() );
		}
		else 
		{
			m_pChannelComboBox->InsertChannelInfo( RESRCMGR->GetMsg( 288 ), m_pChannelComboBox->GetExceedColor() );
		}

	}
	m_pChannelComboBox->SelectComboText( gChannelNum );
}

void CMiniMapDlg::ChangeChannel()
{
	if( (WORD)m_pChannelComboBox->GetCurSelectedIdx() == gChannelNum )
		return;

	if( HERO->GetState() != eObjectState_None )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 2222 ) );
		m_pChannelComboBox->SelectComboText( gChannelNum );
		return;
	}


	MSG_WORD msg;
	ZeroMemory( &msg, sizeof(msg) );
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANGE_CHANNEL_IN_GAME_SYN;
	msg.dwObjectID = HEROID;
	msg.wData = (WORD)m_pChannelComboBox->GetCurSelectedIdx();
	NETWORK->Send( &msg, sizeof(msg) );
}

void CMiniMapDlg::RequestChannelInfo()
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANNELINFO_SYN;
	msg.dwObjectID = HEROID;
	
	NETWORK->Send( &msg, sizeof(msg) );
}