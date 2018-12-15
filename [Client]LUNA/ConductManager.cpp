#include "StdAfx.h"
#include "conductmanager.h"
#include "MHFile.h"
#include "ConductInfo.h"
#include "conductbase.h"
#include "Interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "WindowIDEnum.h"
#include "GameIn.h"
#include "cSkillTreeDlg.h"
#include "cJobSkillDlg.h"
#include "interface/cIconGridDialog.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "ExchangeManager.h"
#include "StreetStallManager.h"
#include "ShowdownManager.h"
#include "PartyManager.h"
#include "MoveManager.h"
#include "AppearanceManager.h"
#include "input/UserInput.h"
#include "ObjectStateManager.h"
#include "Object.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "AutoNoteManager.h"
#include "StoreSearchDlg.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "FishingManager.h"
#include "MHMap.h"
#include "CookDlg.h"
#include "cHousingMgr.h"

cConductManager::cConductManager(void)
{
	m_pConductInfo = NULL;

	InitInfoTable();
}

cConductManager::~cConductManager(void)
{
	ReleaseInfoTable() ;
}

void cConductManager::InitInfoTable()
{
	m_ConductInfoTable.Initialize(MAX_CONDUCT_COUNT) ;

	LoadConductInfoList() ;

	// 070528 LYW --- ConductManager : Load emoticon list.
	LoadEmoticonList() ;
}

void cConductManager::ReleaseInfoTable()
{
	m_ConductInfoTable.SetPositionHead() ;

	for(cConductInfo* info = m_ConductInfoTable.GetData();
		0 < info;
		info = m_ConductInfoTable.GetData())
	{
		SAFE_DELETE(
			info);
	}

	m_ConductInfoTable.RemoveAll() ;

	// 070528 LYW --- ConductManager : Add emoticon part.
	EMOTICON* pEmoticon = NULL ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition() ;

	while(pos)
	{
		pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos) ;

		if( pEmoticon )
		{
			delete pEmoticon ;
			pEmoticon = NULL ;
		}
	}

	m_EmoticonList.RemoveAll() ;
}

void cConductManager::LoadConductInfoList()
{
	CMHFile	file ;

	file.Init( CONDUCT_FILE_NAME, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // 추후 ASSERT 처리 할 것.

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		cConductInfo* pCInfo = new cConductInfo ;
		pCInfo->InitConductInfo(&file) ;

		ASSERT( m_ConductInfoTable.GetData( pCInfo->GetConductIdx() ) == NULL ) ;
		m_ConductInfoTable.Add( pCInfo, pCInfo->GetConductIdx() ) ;
	}

	file.Release() ;
}

// 070528 LYW --- ConductManager : Add function to load emoticon list.
void cConductManager::LoadEmoticonList()
{
	CMHFile	file ;

	file.Init( EMOTICON_FILE_NAME, "rb" ) ;

	if( file.IsInited() == FALSE )	return ; // 추후 ASSERT 처리 할 것.

	while(1)
	{
		if( file.IsEOF() != FALSE ) break ;

		EMOTICON* pEmoticon = new EMOTICON ;
		
		pEmoticon->index = file.GetWord() ;
		strcpy(pEmoticon->command, file.GetString()) ;
		pEmoticon->effectNum = file.GetWord() ;

		m_EmoticonList.AddTail(pEmoticon) ;		
	}

	file.Release() ;
}

cConductInfo* cConductManager::GetConductInfo(DWORD conductIdx)
{
	if( conductIdx == 0 )
	{
		YHTPOSITION pos = m_ConductInfoTable.GetPositionHead() ;

		return m_ConductInfoTable.GetDataPos( &pos ) ;
	}
	return (cConductInfo*)m_ConductInfoTable.GetData(conductIdx);
}

void cConductManager::RegistConduct()
{
	cSkillTreeDlg* const pSkillTreeDlg = GAMEIN->GetSkillTreeDlg();
	
	m_ConductInfoTable.SetPositionHead() ;

	for(cConductInfo* info = m_ConductInfoTable.GetData();
		0 < info;
		info = m_ConductInfoTable.GetData())
	{
		cJobSkillDlg* const pJobSkillDlg = (cJobSkillDlg*)pSkillTreeDlg->GetTabSheet(
			BYTE(info->GetConductKind()));
		cIconGridDialog* const pSkillDlg = (cIconGridDialog*)pJobSkillDlg->GetTabSheet(
			0);
		pSkillDlg->AddIcon(
			info->GetConductPos(),
			NewConductIcon(BYTE(info->GetConductIdx())));
	}
}

// 080203 LYW --- ConductManager : 행동관련 아이콘 추가 처리를 수정함.
cConductBase* cConductManager::NewConductIcon( BYTE bIndex )
{
	cSkillTreeDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetSkillTreeDlg() ;															// 스킬 트리 다이얼로그를 받는다.

	if( !pDlg )																					// 다이얼로그 체크.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive skill tree dialog.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;															
	}

	CIndexGenerator* pIdxGenerator = NULL ;
	pIdxGenerator = pDlg->GetIndexGenerator() ;													// 인덱스 생성기를 받는다.

	if( !pIdxGenerator )																		// 인덱스 생성기 체크.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive index generator.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;																
	}

	WORD	wIconSize	= DEFAULT_ICONSIZE ;													// 아이콘 사이즈를 받는다.
	DWORD	dwIndex		= pIdxGenerator->GenerateIndex() ;										// 인덱스를 할당받는다.
	DWORD	dwMaxIndex	= IG_CONDUCT_STARTINDEX + MAX_CONDUCTICON_NUM ;							// 최대 인덱스를 받는다.

	if( dwIndex < IG_CONDUCT_STARTINDEX || dwIndex >= dwMaxIndex )								// 인덱스를 체크.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Invalid icon index", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;
	}

	cConductInfo* pConductInfo = NULL ;
	pConductInfo = CONDUCTMGR->GetConductInfo((WORD)bIndex) ;									// 아이콘 정보를 받는다.

	if( !pConductInfo )																			// 아이콘 정보 체크.
	{
		#ifdef _DEBUG
		MessageBox( NULL, "Failed to receive conduct info.", "NewConductIcon", MB_OK ) ;
		#endif
		return NULL ;
	}

	cImage LowImage ;
	cImage HighImage ;																			// 아이콘 이미지 선언.

	GetIconImage(bIndex, &LowImage, &HighImage) ;												// 이미지를 받는다.

	cConductBase* icon	= new cConductBase ;													// 기본 정보를 생성한다.

	icon->Init(0,0, wIconSize, wIconSize, &LowImage, &HighImage, dwIndex) ;						// 아이콘을 처리한다.
	icon->SetData((DWORD)bIndex) ;																// 인덱스 정보를 세팅한다.

	icon->m_pConductInfo = pConductInfo ;														// 행동 관련 정보를 세팅한다.

	SetToolTipIcon( icon ) ;																	// 툴팁을 설정한다.

	WINDOWMGR->AddWindow(icon) ;																// 윈도우 매니져에 아이콘을 추가한다.

	return icon ;																				// 아이콘을 리턴한다.
}

void cConductManager::GetIconImage(DWORD Conductldx, cImage * pLowImage , cImage * pHighImage)
{
	cConductInfo* pConductInfo = CONDUCTMGR->GetConductInfo(Conductldx);
	if(pConductInfo == NULL)
	{
		return;
	}

	/*if( pConductInfo->GetLowImage() != -1 )
	SCRIPTMGR->GetImage( pConductInfo->GetLowImage(), pLowImage, PFT_CONDUCTPATH );*/
	if( pConductInfo->GetHighImage() != -1 )
	SCRIPTMGR->GetImage( pConductInfo->GetHighImage(), pHighImage, PFT_CONDUCTPATH );
}

void cConductManager::SetToolTipIcon(cConductBase *pConduct)
{
	cConductInfo* pCommConduct = pConduct->m_pConductInfo;
	if(!pCommConduct)	return;

	cImage imgToolTip;
	SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
	pConduct->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &imgToolTip, TTCLR_ITEM_CANEQUIP );

	char line[64];

	wsprintf(line, "[%s]", pCommConduct->GetConductName() );

	pConduct->AddToolTipLine( line, TTTC_SKILLNAME );
}

// 070220 LYW --- Add function to use conduct icon.
BOOL cConductManager::OnConductCommand( DWORD idx )
{
	// 070620 LYW --- ConductManager : Check die.
	if( HERO->IsDied() ) return FALSE ;

	// 070509 LYW --- ConductManager : Check running emotion.
	if(HERO->IsRunningEmotion()) return FALSE ;

	cConductInfo* pConductInfo = NULL ;
	if( idx == 0 )
	{
		YHTPOSITION pos = m_ConductInfoTable.GetPositionHead() ;

		pConductInfo = m_ConductInfoTable.GetDataPos( &pos ) ;
	}
	else
	{
		pConductInfo = m_ConductInfoTable.GetData( idx );
	}

	if( !pConductInfo )
	{
		return FALSE;
	}

	// 091022 ShinJS --- 탈것 탑승시 사용할수 없는 Action 인 경우
	if( pConductInfo->IsInvalidOnVehicle() && 
		HERO->IsGetOnVehicle() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
		return FALSE;
	}

	// 090911 ShinJS --- WayPoint Clear
	HERO->ClearWayPoint();

	WORD conductKind = pConductInfo->GetConductKind();

	switch( conductKind )
	{
	case eSkillDlgTab_Action :
		{
			LifeSkillCommand( pConductInfo->GetConductIdx() ) ;
		}
		break ;

	case eSkillDlgTab_Motion :
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.09.18
			// ..농장 모션 중일 땐 리턴
			if (g_csFarmManager.CLI_GetIngCharMotion())	return FALSE;

			if ( eObjectState_HouseRiding == HERO->GetState() )
			{
				//090527 pdy 하우징 시스템메세지 탑승상태시 제한행동 [모션]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1876 ) );	//1876	"탑승 상태를 해제한 후 사용하셔야 합니다."
				return FALSE;
			}
			// E 농장시스템 추가 added by hseos 2007.09.18
			ExpressEmotion( pConductInfo->GetConductIdx() ) ;

			// 080205 LYW --- ConductManager : 모션을 실행한다.
			HERO->SetRunningEmotion(TRUE) ;
		}
		break ;

	// 070425 LYW --- ConductManager : Add emoticon part.
	case eSkillDlgTab_Emotiocon :
		{
			EmoticonCommand( pConductInfo->GetConductIdx() ) ;

			// 080205 LYW --- ConductManager : 모션을 실행한다.
			HERO->SetRunningEmotion(TRUE) ;
		}
		break ;
	}

	m_pConductInfo = pConductInfo ;

	// 080205 LYW --- ConductManager : 실제 실행 될 곳에서만 사용하도록, 삭제 처리.
	//HERO->SetRunningEmotion(TRUE) ;
	return TRUE ;
}

// 070220 LYW --- ConductManager : Add function to use life skill.
void cConductManager::LifeSkillCommand( DWORD idx )
{
	if ( eObjectState_HouseRiding == HERO->GetState() )
	{
		//090527 pdy 하우징 시스템메세지 탑승상태시 제한사항 [일반액션]
		//가구에 탑승시엔 일반액션은 다제한된다
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1876 ) );	//1876	"탑승 상태를 해제한 후 사용하셔야 합니다."
		return;
	}

 	switch( idx )
	{
	case 0 :	// 공격
		{
			g_UserInput.GetHeroMove()->AttackSelectedObject() ;
		}
		break ;

	case 1 :	//앉기
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.09.20
			if (g_csFarmManager.CLI_GetIngCharMotion()) return;
			// E 농장시스템 추가 added by hseos 2007.09.20

			// 070502 LYW --- ConductManager : Send rest infor to server.

			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			EObjectState IsRestMode = OBJECTSTATEMGR->GetObjectState(hero);
			BOOL bReady = FALSE;

			if( IsRestMode == eObjectState_None || IsRestMode == eObjectState_Move || IsRestMode == eObjectState_Rest )
			{
				bReady = TRUE ;
			}

			if( !bReady )
			{
				return ;
			}

			if( IsRestMode == eObjectState_Rest )
			{
//				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(hero, eObjectState_None);

				IsRestMode = eObjectState_None;
			}
			else
			{
				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(hero, eObjectState_Rest);

				IsRestMode = eObjectState_Rest;
			}

			MSG_BYTE msg;
			msg.Category = MP_EMOTION;
			msg.Protocol = MP_EMOTION_SITSTAND_SYN;
			msg.dwObjectID = gHeroID;
			msg.bData = BYTE(IsRestMode);
			NETWORK->Send(&msg, sizeof(MSG_BYTE));
		}
		break ;

	case 2 :	//090204 LYW --- ConductManager : 2번 기능은 [ 파티 생성 ] 기능이다.
		{
			//STREETSTALLMGR->ChangeDialogState();
			GAMEIN->GetPartyDialog()->SetActive(TRUE);
		}
		break ;

	case 3 :	//거래
		{
			
			//090527 pdy 하우징 시스템메세지 꾸미기 모드일시 제한사항 [거래]
			if( HERO->GetState() == eObjectState_Housing  )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1874 ) );	//1874	"꾸미기 모드에선 사용하실 수 없습니다."
				return;
			}

			//SHOWDOWNMGR->ApplyShowdown() ;
			EXCHANGEMGR->ApplyExchange() ;
		}
		break ;

	case 4 :	//상점
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.09.20
			if (g_csFarmManager.GetFarmZoneID(MAP->GetMapNum(), NULL)) 
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1792 ) );
				return;
			}

			if (g_csFarmManager.CLI_GetIngCharMotion()) return;
			// E 농장시스템 추가 added by hseos 2007.09.20
			//GAMEIN->GetPartyDialog()->SetActive(TRUE);

			// 080429 NYJ --- 낚시중 사용불가
			if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}
			// 080429 NYJ --- 낚시포인트교환중 사용불가
			else if(GAMEIN->GetFishingPointDlg()->IsActive() )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) );
				return;
			}

			if( HOUSINGMGR->IsHousingMap() )
			{
				//090527 pdy 하우징 시스템메세지 하우징 맵일때 제한행동 [상점]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"이 지역에서는 사용하실 수 없습니다."
				return;
			}

			STREETSTALLMGR->ChangeDialogState();
		}
		break ;

	case 5 : // 결투
		{
			if ( HOUSINGMGR->IsHousingMap() )
			{
				//090527 pdy 하우징 시스템메세지 하우징 맵일때 제한행동 [결투]
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1867 ) );	//1867	"이 지역에서는 사용하실 수 없습니다."
				return;
			}

			SHOWDOWNMGR->ApplyShowdown() ;
		}
		break ;

	case 52 :	// 조합
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

	case 53 :	// 분해
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

	case 54:		// 강화
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;
	case 55:		// 인챈트
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	 );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );
		}
		break;

//---KES AUTONOTE
	case 59:
		{
			AUTONOTEMGR->ToggleAutoNote();
		}
		break;
//---------------

	case 64:
		{
			PKMGR->ToggleHeroPKMode();
		}
		break;

//---KES 상점검색
	case 65:
		{
			if( GAMEIN->GetStoreSearchDlg()->IsActive() )
				GAMEIN->GetStoreSearchDlg()->SetActive( FALSE );
			else
				GAMEIN->GetStoreSearchDlg()->SetActive( TRUE );
		}
		break;
//---------------

//---KES 따라가기
	case 66:
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();
			if( pObject )
			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				if( HERO )
					HERO->SetFollowPlayer( pObject->GetID() );
			}
		}
		break;
//---------------

// 080410 NYJ --- 낚시
	case 67:
		{	
			if(!HERO)
				break;

			CFishingDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetFishingDlg();

			if( !pDlg) return ;

			if(!pDlg->IsActive())						// 낚시창이 닫혀있으면 창열기
				pDlg->SetActive(TRUE) ;
		}
		break;

	case 68:
		{
			if(!HERO)
				break;

			if( GAMEIN->GetCookDlg()->IsActive() )
				GAMEIN->GetCookDlg()->SetActive( FALSE );
			else
				GAMEIN->GetCookDlg()->SetActive( TRUE );
		}
		break;
	}
}

// 070220 LYW --- ConductManager : Add function to use express emotion.
void cConductManager::ExpressEmotion( DWORD idx )
{
	switch( HERO->GetState() )
	{
	case eObjectState_Rest:
	case eObjectState_Exchange:			
	case eObjectState_StreetStall_Owner:		
	case eObjectState_StreetStall_Guest:		
	case eObjectState_PrivateWarehouse:		
	case eObjectState_Munpa:				

	case eObjectState_SkillStart:			
	case eObjectState_SkillSyn:			
	case eObjectState_SkillBinding:			
	case eObjectState_SkillUsing:			
	case eObjectState_SkillDelay:		
	
	case eObjectState_TiedUp_CanMove:		
	case eObjectState_TiedUp_CanSkill:
	case eObjectState_TiedUp:
	
	case eObjectState_Die:	
	case eObjectState_Fishing:
	case eObjectState_FishingResult:
		return;
	}

	// 090422 ShinJS --- 탈것에 탑승중인경우 실행하지 않는다
	if( HERO->IsGetOnVehicle() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
		return;
	}

	cConductInfo* pCInfo = m_ConductInfoTable.GetData(idx) ;

	if(0 == pCInfo)
	{
		return;
	}

	APPEARANCEMGR->HideWeapon(HERO);
	MOVEMGR->HeroMoveStop();
	//HERO->ChangeEmotion( (int)motionNum ) ;

#ifdef _TESTCLIENT_
	HERO->GetEngineObject()->ChangeMotion(
		pCInfo->GetMotionIdx(),
		FALSE);
#else
	SendToServer(
		pCInfo->GetMotionIdx());
#endif
}	

// 070425 LYW --- ConductManager : Add function to use emoticon.
void cConductManager::EmoticonCommand( DWORD idx )
{
	int emoticonNum = -1 ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition();
	while(pos)
	{
		EMOTICON* pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos);
		if (idx == pEmoticon->index)
		{
			emoticonNum = pEmoticon->effectNum;
			break;
		}
	}
	if( emoticonNum > -1 )
	{
		HERO->PlayEmoticon_Syn(
			BYTE(emoticonNum));
	}
}

void cConductManager::PetEmoticonCommand( DWORD idx )
{
	int emoticonNum = -1 ;

	PTRLISTPOS pos = m_EmoticonList.GetHeadPosition();
	while(pos)
	{
		EMOTICON* pEmoticon = (EMOTICON*)m_EmoticonList.GetNext(pos);
		if (idx == pEmoticon->index)
		{
			emoticonNum = pEmoticon->effectNum;
			break;
		}
	}
	if( emoticonNum > -1 )
	{
		TARGETSET set;
		set.pTarget = HEROPET;
			
		EFFECTMGR->StartEffectProcess(emoticonNum,HEROPET,&set,0,HEROPET->GetID());
	}
}

// 070228 LYW --- ConductManager : Add function to send message to server.
void cConductManager::SendToServer( int motionNum )
{
	MSG_INT data ;

	data.Category	= MP_EMOTION ;
	data.Protocol	= MP_EMOTION_NORMAL ;
	data.dwObjectID	= HEROID ;
	data.nData		= motionNum ;
	
	NETWORK->Send(&data, sizeof(MSG_INT));
}

// 070228 LYW --- ConductManager : Add function to parsing message.
void cConductManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch( Protocol )
	{
	case MP_EMOTION_NORMAL :
		{
			MSG_INT* data = ( MSG_INT* )pMsg ;

			CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( data->dwObjectID ) ;

			if( !pPlayer ) return ;

			// 070619 LYW --- ConductManager : Stop Player.
			MOVEMGR->MoveStop((CObject*)pPlayer, &(pPlayer->GetCurPosition())) ;

			// 070503 LYW --- ConductManager : Check emotion kind.
			APPEARANCEMGR->HideWeapon( pPlayer ) ;
			pPlayer->SetRunningEmotion(TRUE) ;

			pPlayer->ChangeEmotion( data->nData ) ;
		}
		break ;

	// 070502 LYW --- ConductManager : Add message to setting rest mode part.
	case MP_EMOTION_SITSTAND_ACK :
		{
			const MSG_BYTE* pmsg = ( MSG_BYTE* )pMsg ;

			if( gHeroID != pmsg->dwObjectID )
			{
				OBJECTSTATEMGR->StartObjectState(
					OBJECTMGR->GetObject(pmsg->dwObjectID),
					EObjectState(pmsg->bData));
			}
		}
		break ;

	// 070502 LYW --- ConductManager : Add message to setting ing motion.
	case MP_EMOTION_ING_MOTION_ACK :
		{
			MSG_INT* pmsg = ( MSG_INT* )pMsg ;

			if( gHeroID != pmsg->dwObjectID )
			{
				CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( pmsg->dwObjectID ) ;

				if( !pPlayer ) return ;
			}
		}
		break ;

	case MP_PLAY_EMOTICON_ACK :
		{
			MSG_INT* data = ( MSG_INT* )pMsg ;

			CPlayer* pPlayer = ( CPlayer* )OBJECTMGR->GetObject( data->dwObjectID ) ;

			if( !pPlayer ) return ;

			pPlayer->PlayEmoticon(data->nData) ;
		}
		break ;
	}
}
