#include "stdafx.h"
#include "CharSelect.h"
#include "MainGame.h"
#include "MainTitle.h"
#include "cWindowSystemFunc.h"
#include "WindowIDEnum.h"
#include "./interface/cWindowManager.h"
#include "./Interface/cResourceManager.h"
#include "ObjectManager.h"
#include "ChannelDialog.h"
#include "ChatManager.h"
#include "CameraConfiguration.h"
#include "MHMap.h"
#include "MHCamera.h"
#include "Interface/cScriptManager.h"
#include "UserInfoManager.h"
#include "ProfileDlg.h"
#include "CertificateDlg.h"
#include "cImageSelf.h"
#include "CharMakeManager.h"
#include "cMsgBox.h"
#include "Input/UserInput.h"
#include "GMNotifyManager.h"
#include "ConductManager.h"
#include "Effect\EffectManager.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "cMonsterSpeechManager.h"
#include "NpcScriptManager.h"
#include "PetManager.h"
#include "UiSettingManager.h"
#include "MotionManager.h"

#define CHARSELECT_STAND_POSX 27930
#define CHARSELECT_STAND_POSZ 7834
#define CHARSELECT_STAND_HEIGHT	100

GLOBALTON(CCharSelect)
CCharSelect::CCharSelect() :
mCameraConfiguration( new CCameraConfiguration( CCameraConfiguration::TypeSelectPlayer ) ),
mImageBar( new cImageSelf ),
mImageLogo( new cImageSelf ),
mIsNoResourceLoaded(TRUE)
{
	m_CurSelectedPlayer = UINT_MAX;
	m_pDoublcClickedPlayer = NULL;

	const VECTOR2 emptyVector = {0};
	mPositionBar = emptyVector;
	mScaleBar = emptyVector;
	mPositionLogo = emptyVector;
	mScaleLogo = emptyVector;
	ZeroMemory(
		m_pPlayer, 
		sizeof(m_pPlayer));
	m_pProfile = NULL ;
	
	m_pChannelDlg = NULL;
	m_bEnterGame = FALSE;
	
	for(DWORD i = 0; i < _countof(m_MaxChannel); ++i)
	{
		m_MaxChannel[i] = MAX_CHANNEL_NUM;
	}
}

CCharSelect::~CCharSelect()
{
	SAFE_DELETE( mCameraConfiguration );
	SAFE_DELETE( mImageLogo );
	SAFE_DELETE( mImageBar );
}

BOOL CCharSelect::Init(void* pInitParam)
{	
	if(IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(25))
	{
		mImageBar->LoadSprite(
			imageNode->szFileName,
			imageNode->size.x,
			imageNode->size.y);
	}

	if(IMAGE_NODE* const imageNode = RESRCMGR->GetInfo(26))
	{
		mImageLogo->LoadSprite(
			imageNode->szFileName,
			imageNode->size.x,
			imageNode->size.y);
	}

	if(MAP->IsInited() == FALSE)
	{
		MAP->InitMap(200) ;
	}

	ZeroMemory(
		m_pPlayer,
		sizeof(m_pPlayer));
	NETWORK->SetCurState(this);

	NewCreateCharSelect_m();
	CreateChannelDlg();
	CreateProfileDlg();
	m_pProfile = ( CProfileDlg* )WINDOWMGR->GetWindowForID( CP_PROFILEDLG ) ;

	// 061218 LYW --- Add dialog for certificate some condition.
	CreateCertificateDlg() ;
	m_pCertificateDlg->SetActive( FALSE ) ;


	WINDOWMGR->AfterInit();
	SEND_CHARSELECT_INFO * pmsg = (SEND_CHARSELECT_INFO *)pInitParam;
	NetworkMsgParse(pmsg->Category, pmsg->Protocol, pInitParam, sizeof(SEND_CHARSELECT_INFO));

	//////////////////////////////////////////////////////////////////////////
	// 그림자 처리를 위해
	VECTOR3 pos = {0,0,0};
	float xpos = CHARSELECT_STAND_POSX;
	float zpos = CHARSELECT_STAND_POSZ;

	for(int n=0;n<MAX_CHARACTER_NUM;++n)
	{
		pos.x = xpos + (n *120);
		pos.y = CHARSELECT_STAND_HEIGHT;
		pos.z = zpos + 30 * n;
	}

	MAP->SetShadowPivotPos(&pos);

	m_CurSelectedPlayer = UINT_MAX;
	m_pDoublcClickedPlayer = NULL;
	m_bBackToMainTitle = FALSE;
	m_bEnterGame = FALSE;

	AdjustBar();
	CHATMGR->InitializeWhisperName() ;

	if(mIsNoResourceLoaded)
	{
		GAMERESRCMNGR->LoadMonsterList();
		GAMERESRCMNGR->LoadMapChangeArea();
		GAMERESRCMNGR->LoadNpcChxList();
		GAMERESRCMNGR->LoadNpcList();
		GAMERESRCMNGR->LoadStaticNpc();
		GAMERESRCMNGR->LoadExpPoint();
		GAMERESRCMNGR->LoadFishingExpPoint();
		GAMERESRCMNGR->LoadPreDataTable();
		GAMERESRCMNGR->LoadHousing_AllList();
		GAMERESRCMNGR->LoadDungeonKeyList();

		mIsNoResourceLoaded = FALSE;
	}

	return TRUE;
}

void CCharSelect::AdjustBar()
{
	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	if(cImageRect* const imageRect = mImageBar->GetImageRect())
	{
		mScaleBar.x = float(screenRect.right) / imageRect->right;
		mScaleBar.y = 1;
		mPositionBar.x = 0;
		mPositionBar.y = float(screenRect.bottom) - imageRect->bottom;
	}

	if(cImageRect* const imageRect = mImageLogo->GetImageRect())
	{
		mScaleLogo.x = 1;
		mScaleLogo.y = 1;
		mPositionLogo.x = float(screenRect.right) - imageRect->right;
		mPositionLogo.y = float(screenRect.bottom) - imageRect->bottom;
	}
}

void CCharSelect::Release(CGameState* pNextGameState)
{
	mImageBar->Release();
	mImageLogo->Release();
	OBJECTMGR->RemoveAllObject();
	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(11);
		
	if(pNextGameState == NULL || m_bEnterGame )
		MAP->Release();
}

void CCharSelect::Process()
{
	if( m_pDoublcClickedPlayer )
	{
		if(Motion_SelAfterstop == m_pDoublcClickedPlayer->GetEngineObject()->GetCurMotion())
		{
			SendMsgGetChannelInfo();

			m_pDoublcClickedPlayer = NULL;
		}
	}	

	if( WINDOWMGR->IsMouseInputProcessed() ) return;
	if( m_bDiablePick ) return;

	if( MOUSE->LButtonDoubleClick() )
	{
		CPlayer* pPlayer = (CPlayer*)GetSelectedObjectBoneCheck( MOUSE->GetMouseEventX(), MOUSE->GetMouseEventY());

		if( pPlayer && m_pDoublcClickedPlayer == NULL )
		if( pPlayer == GetCurSelectedPlayer() )
		{
			m_pDoublcClickedPlayer = pPlayer;
			SetDisablePick( TRUE );
		}
	}
	else if( MOUSE->LButtonDown()  )
	{		
		CPlayer* pPlayer = (CPlayer*)GetSelectedObjectBoneCheck( MOUSE->GetMouseEventX(), MOUSE->GetMouseEventY());

		if( pPlayer && m_pDoublcClickedPlayer == NULL )
		{
			for( int i = 0 ; i < MAX_CHARACTER_NUM ; ++i )
			{
				if( m_pPlayer[i] == pPlayer )
				{
					SelectPlayer( i );
					break;
				}
			}
		}
	}

	for( int count = 0 ; count < _countof(m_pPlayer); ++count )
	{
		if( m_pPlayer[count] )
		{
			if( m_pPlayer[count]->GetEyeTime()->Check() == 0 )
			{
				m_pPlayer[count]->InitializeEye() ;
			}
		}
	}
	
#ifdef _GMTOOL_
	if( mCameraConfiguration->Process() )
	{
		for( DWORD i = 0; i < _countof(m_pPlayer); ++i )
		{
			CPlayer* const player = m_pPlayer[ i ];

			if( 0 == player )
			{
				continue;
			}

			VECTOR3 position = mCameraConfiguration->GetPlayerPosition( i );
			player->SetPosition( &position );
		}
	}
#endif
}

void CCharSelect::BeforeRender()
{
	g_pExecutive->GetRenderer()->BeginRender(0,0,0);
	g_pExecutive->GetRenderer()->EndRender();
	MAP->Process( gCurTime );
}

void CCharSelect::AfterRender()
{
	mImageBar->RenderSprite(
		&mScaleBar,
		0,
		0,
		&mPositionBar,
		0xffffffff);
	mImageLogo->RenderSprite(
		&mScaleLogo,
		0,
		0,
		&mPositionLogo,
		0xffffffff);
	WINDOWMGR->Render();
}

CPlayer* CCharSelect::GetCurSelectedPlayer()
{
	if(m_CurSelectedPlayer == -1)
		return NULL;
	return m_pPlayer[m_CurSelectedPlayer];
}

void CCharSelect::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	switch(Category) 
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
			case MP_USERCONN_CHARACTERLIST_ACK:
				{	
					UserConn_CharacterList_Ack( pMsg ) ;
				}				
				return;
			case MP_USERCONN_CHARACTERLIST_NACK:
				{
					LOG(EC_CHARACTERLIST_FAILED);
				}				
				return;
			case MP_USERCONN_CHARACTER_REMOVE_ACK:
				{
					UserConn_Character_Remove_Ack() ;
				}
				return;
			case MP_USERCONN_CHARACTER_REMOVE_NACK:
				{
					UserConn_Character_Remove_Nack( pMsg ) ;
				}
				return;

			case MP_USERCONN_DISCONNECT_ACK:
				{
					UserConn_DisConnect_Ack() ;
				}
				return;
			case MP_USERCONN_CHANNELINFO_ACK:
				{
					UserConn_ChannelInfo_Ack( pMsg ) ;
				}
				return;
			case MP_USERCONN_CHANNELINFO_NACK:
				{
					UserConn_ChannelInfo_Nack( pMsg ) ;
				}
				return;
			// desc_hseos_성별선택01
			// S 성별선택 추가 added by hseos 2007.06.17
			case MP_USERCONN_USER_SEXKIND:
				{
					UserConn_Character_SexKind(pMsg);
				}
				return;
			// E 성별선택 추가 added by hseos 2007.06.17
			}
		}
		break; 
	case MP_CHEAT:
		{
			switch(Protocol) 
			{
				case MP_CHEAT_EVENTNOTIFY_ON:
				{
					Cheat_EventNotify_On( pMsg ) ;
				}
				break;
		
				case MP_CHEAT_EVENTNOTIFY_OFF:
				{
					Cheat_EventNotify_Off( pMsg ) ;
				}	
				break;

			}
		}		
	}
}

void CCharSelect::SetCurSelctedPlayer(DWORD num)
{	
	cPushupButton * charBtn[MAX_CHARACTER_NUM];

	if(num == -1)
	{
		for(int n=0;n<MAX_CHARACTER_NUM;++n)
		{
			charBtn[n] = (cPushupButton *)WINDOWMGR->GetWindowForIDEx(MT_FIRSTCHOSEBTN+n);
			charBtn[n]->SetPush(FALSE);
		}
		m_CurSelectedPlayer = UINT_MAX;
		return;
	}

	for(DWORD n=0;n<MAX_CHARACTER_NUM;++n)
	{
		charBtn[n] = (cPushupButton *)WINDOWMGR->GetWindowForIDEx(MT_FIRSTCHOSEBTN+n);
		charBtn[n]->SetPush(n == num);
	}
	m_CurSelectedPlayer = num;
}

void CCharSelect::SelectPlayer(int num)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(MBI_DELETECHAR))
	{
		if(dialog->IsActive())
		{
			return;
		}
	}

	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CHA_CHANNELDLG))
	{
		if(dialog->IsActive())
		{
			return;
		}
	}

	if( m_pCertificateDlg->IsActive() )
	{
		return ;
	}
	else if( num == int(m_CurSelectedPlayer))
	{
		return;
	}
	else if(num != -1 && m_pPlayer[num] == NULL)
	{
		cPushupButton* const button = (cPushupButton *)WINDOWMGR->GetWindowForIDEx(MT_FIRSTCHOSEBTN+num);
		button->SetPush(
			FALSE);
		return;
	}

	PlayDeselectedMotion(m_CurSelectedPlayer);
	PlaySelectedMotion(num);

	SetCurSelctedPlayer(num);
	
	// 061215 LYW --- Update profile.
	if( m_pPlayer[num] != NULL )
	{
		m_pProfile->UpdateProfile( m_pPlayer[num]->GetObjectName(), m_pPlayer[num]->GetCharacterTotalInfo() ) ; 
	}
}

BOOL CCharSelect::IsFull()
{
	for(size_t n = 0; n < _countof(m_pPlayer); ++n)
	{
		if(m_pPlayer[n] == NULL)
			return FALSE;
	}

	return TRUE;
}

BOOL CCharSelect::EnterGame()
{
	if(0 == GetCurSelectedPlayer())
		return FALSE;
	
	MSG_WORD message;
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_CHARACTERSELECT_SYN;
	message.dwObjectID = GetCurSelectedPlayer()->GetID();
	message.wData = WORD(gChannelNum);
	NETWORK->Send(&message, sizeof(message));

	const MAPTYPE mapType = GetCurSelectedPlayer()->GetCharacterTotalInfo()->LoginMapNum;

	MAINGAME->SetGameState(
		eGAMESTATE_MAPCHANGE,
		LPVOID(&mapType),
		sizeof(mapType));

	m_bEnterGame = TRUE;

	// 카메라 이동보간 리셋, 하이트필드 적용
	CAMERA->ResetInterPolation();

	return TRUE;
}

void CCharSelect::DeleteCharacter()
{
	CPlayer* pPlayer = GetCurSelectedPlayer();
	if(pPlayer == NULL)
		return;
		
	MSG_DWORD  msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTER_REMOVE_SYN;
	msg.dwData = pPlayer->GetID();
	NETWORK->Send(&msg,sizeof(msg));
}

void CCharSelect::PlaySelectedMotion(DWORD num)
{
	if(MAX_CHARACTER_NUM <= num)
	{
		return;
	}
	else if(m_pPlayer[num] == 0)
	{
		return;
	}

	m_pPlayer[num]->ChangeMotion(
		Motion_SelAfter,
		0);
	m_pPlayer[num]->ChangeBaseMotion(
		Motion_SelAfterstop);
}
void CCharSelect::PlayDeselectedMotion(DWORD num)
{
	if(MAX_CHARACTER_NUM <= num)
	{
		return;
	}
	else if(m_pPlayer[num] == 0)
	{
		return;
	}

	m_pPlayer[num]->ChangeMotion(
		Motion_SelBack,
		0);
	m_pPlayer[num]->ChangeBaseMotion(
		Motion_SelBefore);
}

void CCharSelect::DisplayNotice(int MsgNum)
{
	WINDOWMGR->MsgBox( MBI_CHARSELERROR, MBT_OK, CHATMGR->GetChatMsg(MsgNum) );
	SetDisablePick( TRUE );
}


void CCharSelect::SetDisablePick( BOOL val )
{
	if( m_pProfile ) m_pProfile->SetDisable( val ) ;

	// 061219 LYW --- Disable all dialog when called this function.
	cDialog* pDlg = ( cDialog* )WINDOWMGR->GetWindowForID(CS_CHARSELECTDLG) ;
	if( pDlg ) pDlg->SetDisable( val ) ;
	
}

void CCharSelect::BackToMainTitle()
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_DISCONNECT_SYN;
	NETWORK->Send(&msg,sizeof(msg));

	SetBlock(TRUE);
	
	gUserID = 0;
	
	SetDisablePick(TRUE);
	m_bBackToMainTitle = TRUE;
}

void CCharSelect::OnDisconnect()
{
	if( m_bBackToMainTitle )
	{
		SetBlock(FALSE);
		OBJECTMGR->RemoveAllObject();	//여기서 안지우고 release에서 지우면 maintitle화면에 잠시 보인다.
		MAINGAME->SetGameState(eGAMESTATE_TITLE);	//에이젼트가 끊나면..
		m_bBackToMainTitle = FALSE;
		NETWORK->ReleaseKey();
	}
	else
	{
		CGameState::OnDisconnect();
	}
}

void CCharSelect::SendMsgGetChannelInfo()
{
	CPlayer* pPlayer = GetCurSelectedPlayer();
	if(pPlayer == NULL)
	{
		DisplayNotice(18);
		return;
	}

	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANNELINFO_SYN;
	msg.dwObjectID = pPlayer->GetID();
	
	NETWORK->Send( &msg, sizeof(msg) );
}

void CCharSelect::UserConn_CharacterList_Ack( void* pMsg )
{
	// 090529 LUJ, 카메라 설정 정보를 갱신한다
	mCameraConfiguration->UpdateCamera();

	SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
	ZeroMemory(
		m_pPlayer,
		sizeof(m_pPlayer));
	const DWORD basiccolor = RGBA_MAKE(255, 255, 255,255);
	const DWORD overcolor = RGBA_MAKE(255,255,0,255);
	const DWORD presscolor = RGBA_MAKE(255,255,0,255);

	for(int i = 0; i < pmsg->CharNum ; i++)
	{
		WORD standpoint = pmsg->StandingArrayNum[i];
		
		// YH 일단 선택창에서 무기 빼놓음.
		pmsg->ChrTotalInfo[i].WearedItemIdx[eWearedItem_Weapon] = 0;
		pmsg->ChrTotalInfo[i].MunpaID = 0;
		pmsg->ChrTotalInfo[i].MarkName = 0;
		
		m_pPlayer[standpoint] = OBJECTMGR->AddPlayer(&pmsg->BaseObjectInfo[i], 0, &pmsg->ChrTotalInfo[i]);
		CPlayer* pPlayer = m_pPlayer[standpoint];						

		pPlayer->GetEngineObject()->ApplyHeightField(FALSE);
		pPlayer->SetOverInfoOption(0);
							
		char LevelName[255];

		cPushupButton * btn = (cPushupButton *)WINDOWMGR->GetWindowForIDEx(MT_FIRSTCHOSEBTN+standpoint);
		wsprintf(LevelName,"[Lv%2d] %-16s",pmsg->ChrTotalInfo[i].Level,pmsg->BaseObjectInfo[i].ObjectName );
		btn->SetText(LevelName, basiccolor, overcolor, presscolor);
		cImage ToolTipImg;
		SCRIPTMGR->GetImage( 0, &ToolTipImg, PFT_HARDPATH );
		{
			btn->SetToolTip( GetMapName(pmsg->ChrTotalInfo[i].LoginMapNum), RGB_HALF(255,255,255), &ToolTipImg, TTCLR_ITEM_CANEQUIP );
		}

		pPlayer->ChangeBaseMotion(Motion_SelBefore);
		pPlayer->ChangeMotion(Motion_SelBefore, 0);
		
		pPlayer->ShowObjectName( FALSE );
		pPlayer->GetEngineObject()->EnableShadow(TRUE);

		VECTOR3 scale = { 0.4f, 0.4f, 0.4f };
		pPlayer->GetEngineObject()->SetScale( &scale );
		// 090529 LUJ, 순서에 따른 플레이어 위치 값을 가져온다.
		VECTOR3 position( mCameraConfiguration->GetPlayerPosition( standpoint ) );
		pPlayer->SetPosition( &position );
		pPlayer->SetAngle( 0 );
		pPlayer->GetEyeTime()->Init(random(0, 8)*500);
		pPlayer->GetEyeTime()->Start() ;
	}
}


//=================================================================================================
//	NAME		: UserConn_Character_Remove_Ack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::UserConn_Character_Remove_Ack()
{
	CPlayer* pPlayer = GetCurSelectedPlayer();
	if(!pPlayer)
	{
		ASSERTMSG(0, CHATMGR->GetChatMsg(20));
		return;
	}	

	USERINFOMGR->DeleteUserQuestInfo(pPlayer->GetID());
	cPushupButton * editboxName = (cPushupButton *)WINDOWMGR->GetWindowForIDEx(MT_FIRSTCHOSEBTN + m_CurSelectedPlayer);
	editboxName->SetText("",RGBA_MAKE(255,255,255,255),NULL,NULL);
	editboxName->SetPush(FALSE);
	
	m_pPlayer[m_CurSelectedPlayer] = NULL;
	m_CurSelectedPlayer = UINT_MAX;
	OBJECTMGR->AddGarbageObject(pPlayer);

	SelectPlayer(-1);
	SetDisablePick( FALSE );

	// 080226 LYW --- Refresh profile dialog.
	CHARSELECT->GetProfileDlg()->RefreshProfile() ;
}


//=================================================================================================
//	NAME		: UserConn_Character_Remove_Nack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::UserConn_Character_Remove_Nack( void* pMsg )
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

	if( pmsg->dwData == 4 )
		WINDOWMGR->MsgBox( MBI_DELETECHARFAIL, MBT_OK, CHATMGR->GetChatMsg(316) );
	else
		WINDOWMGR->MsgBox( MBI_DELETECHARFAIL, MBT_OK, CHATMGR->GetChatMsg(317) );

	SetDisablePick( FALSE );
}


//=================================================================================================
//	NAME		: UserConn_DisConnect_Ack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::UserConn_DisConnect_Ack()
{
	SetDisablePick( TRUE );
	m_bBackToMainTitle = TRUE;
}


//=================================================================================================
//	NAME		: UserConn_ChannelInfo_Ack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::UserConn_ChannelInfo_Ack( void* pMsg )
{
	MSG_CHANNEL_INFO * pmsg = (MSG_CHANNEL_INFO*)pMsg;
	GetChannelDialog()->SetChannelList(pmsg);
	SetDisablePick(TRUE);
	
	// 체널이 하나이면 바로 들어가게 함
	if(pmsg->Count == 1)
	{
		GetChannelDialog()->OnConnect();	
		GetChannelDialog()->SetActive(FALSE);
	}
}


//=================================================================================================
//	NAME		: UserConn_ChannelInfo_Nack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::UserConn_ChannelInfo_Nack( void* pMsg )
{
	MSG_BYTE* msg = (MSG_BYTE*)pMsg;
	if( msg->bData == 0 )
	{
		WINDOWMGR->MsgBox( MBI_MAPSERVER_CLOSE, MBT_OK, CHATMGR->GetChatMsg(213) );
	}
	else
	{
		WINDOWMGR->MsgBox( MBI_MAPSERVER_CLOSE, MBT_OK, CHATMGR->GetChatMsg(318) );
	}
}


//=================================================================================================
//	NAME		: Cheat_EventNotify_On()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::Cheat_EventNotify_On( void* pMsg )
{
	MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

	NOTIFYMGR->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
	NOTIFYMGR->SetEventNotify( TRUE );
	NOTIFYMGR->SetEventNotifyChanged( TRUE );		
	NOTIFYMGR->ResetEventApply();

	for(int i=0; i<eEvent_Max; ++i)
	{
		if( pmsg->EventList[i] )
			NOTIFYMGR->SetEventApply( i );
	}						
}


//=================================================================================================
//	NAME		: Cheat_EventNotify_Off()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharSelect::Cheat_EventNotify_Off( void* pMsg )
{
	NOTIFYMGR->SetEventNotify( FALSE );
	NOTIFYMGR->SetEventNotifyChanged( FALSE );
}

// desc_hseos_성별선택01
// S 성별선택 추가 added by hseos 2007.06.17
void CCharSelect::UserConn_Character_SexKind(void *pMsg)
{
	MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;

	CHARMAKEMGR->SetCharSexKindFromDB(pPacket->dwData);
}
// E 성별선택 추가 added by hseos 2007.06.17

// 090424 ONS 신규종족 생성 조건(레벨:50이상)을 체크하는 함수
BOOL CCharSelect::HasLevelOfChar( const LEVELTYPE level ) const
{
	for( int i=0 ; i<MAX_CHARACTER_NUM ; ++i )
	{
		if( m_pPlayer[i] )
		{
			const CHARACTER_TOTALINFO* info = m_pPlayer[i]->GetCharacterTotalInfo();
			if( info )
				if( info->Level >= level )
					return TRUE;
		}
	}
	return FALSE;
}
