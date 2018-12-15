#include "stdafx.h"
#include "CharMake.h"

#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cResourceManager.h"
#include "./Interface/cPushupButton.h"

#include "ObjectManager.h"
#include "GameResourceManager.h"
#include "MHCamera.h"
#include "MAINGAME.h"

#include "cWindowSystemFunc.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "CharMakeNewDlg.h"
#include "CharMakeManager.h"

#include "mhMap.h"
#include "./Input/UserInput.h"

// 061218 LYW --- Add for test.
#include "./interface/cFont.h"

//#include "Gamein.h"
#include "GMNotifyManager.h"
#include "cImageSelf.h"
#include "CameraConfiguration.h"

GLOBALTON(CCharMake)
CCharMake::CCharMake() :
mCameraConfiguration( new CCameraConfiguration( CCameraConfiguration::TypeMakePlayer ) ),
mImageBottomRightBar( new cImageSelf ),
mImageBottomLeftBar( new cImageSelf )
{
	// 062117 LYW --- Delete this code.
	//m_pCharMakeDlg = NULL;
	m_pCharMakeNewDlg = NULL ;
	m_bEnterGame = FALSE;
}

CCharMake::~CCharMake()
{
	SAFE_DELETE( mCameraConfiguration );
	SAFE_DELETE( mImageBottomRightBar );
	SAFE_DELETE( mImageBottomLeftBar );
}

BOOL CCharMake::Init(void* pInitParam)
{	
	// 080507 LUJ, 리소스 관리자를 통해 읽어오도록 한다
	{
		IMAGE_NODE* bottomBar = RESRCMGR->GetInfo( 25 );

		if( bottomBar )
		{
			mImageBottomLeftBar->LoadSprite( bottomBar->szFileName );
			mImageBottomRightBar->LoadSprite( bottomBar->szFileName );
		}
	}

	if(MAP->IsInited() == FALSE)
	{
		MAP->InitMap(200) ;
	}
	
	NETWORK->SetCurState(this);
	CreateCharMakeNewDlg() ;
	

	CHARMAKEMGR->Init();
	WINDOWMGR->AfterInit();
	CAMERA->SetCurCamera( 0 );
	CAMERA->SetCameraMode(eCM_LOGIN);
	AdjustBar();

	m_bEnterGame = FALSE;

	return TRUE;
}

void CCharMake::AdjustBar()
{
	const SIZE standardResolution = {1024, 768};
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	const float ratioWidth = float(dispInfo.dwWidth) / standardResolution.cx;
	const float ratioHeight = float(dispInfo.dwHeight) / standardResolution.cy;
	const float fixedScale = min(ratioWidth, ratioHeight);

	// 091221 LUJ, 하단 막대 오른쪽 막대 위치를 설정한다.
	//			로고 비율을 1:1로 표시하기 위해 추가되었다
	{
		VECTOR2 imageSize = {0};
		mImageBottomRightBar->GetImageOriginalSize(
			&imageSize);
		mScaleBottomRightBar.x = fixedScale;
		mScaleBottomRightBar.y = fixedScale;
		mPositionBottomRightBar.x = dispInfo.dwWidth - (imageSize.x * mScaleBottomRightBar.x);
		mPositionBottomRightBar.y = dispInfo.dwHeight - (imageSize.y * mScaleBottomRightBar.y);
	}

	// 091221 LUJ, 하단 왼쪽 막대를 설정한다. 오른쪽 막대가 1:1로 축소/확대된다.
	//			그래서 이것은 빈 왼쪽 공간을 채우기 위한 목적으로 사용된다.
	{
		// 091221 LUJ, 로고가 표시된 부분을 자른다
		cImageRect* const imageRect = mImageBottomLeftBar->GetImageRect();
		imageRect->right -= 200;
		mImageBottomLeftBar->SetImageSrcRect(
			imageRect);

		VECTOR2 imageSize = {0};
		mImageBottomLeftBar->GetImageOriginalSize(
			&imageSize);

		// 091221 LUJ, 확대하고 위치를 조정한다
		mScaleBottomLeftBar.x = (dispInfo.dwWidth - imageSize.x * fixedScale) / imageSize.x;
		mScaleBottomLeftBar.y = fixedScale;
		mPositionBottomLeftBar.x = 0;
		mPositionBottomLeftBar.y = mPositionBottomRightBar.y;
	}
}

void CCharMake::Release(CGameState* pNextGameState)
{
	mImageBottomLeftBar->Release();
	mImageBottomRightBar->Release();
	OBJECTMGR->RemoveAllObject();
	
	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(12);
	CHARMAKEMGR->Release();
//	HELPERMGR->Release();
		
	if(pNextGameState == NULL || m_bEnterGame )
		MAP->Release();
			
}

void CCharMake::Process()
{	
	if(IsBlock()) return;

#ifdef _GMTOOL_
	if( mCameraConfiguration->Process() )
	{
		CPlayer* const player = CHARMAKEMGR->GetNewPlayer();

		if( player )
		{
			VECTOR3 playerPosition = mCameraConfiguration->GetRacePosition( player->GetRace() );
			VECTOR3 scaleVector = mCameraConfiguration->GetScale( player->GetRace() );
			player->GetEngineObject()->SetScale( &scaleVector );
			player->SetPosition( &playerPosition );
		}
	}
#endif

	if( 0 == CHARMAKEMGR->GetNewPlayer() )
	{
		mCameraConfiguration->UpdateCamera();

		VECTOR3 playerPosition = { 0 };
		CHARMAKEMGR->CreateNewCharacter( &playerPosition );
		
		CPlayer* const player = CHARMAKEMGR->GetNewPlayer();
		VECTOR3 playerScale = mCameraConfiguration->GetScale( player->GetRace() );
		playerPosition = mCameraConfiguration->GetRacePosition( player->GetRace() );
		player->GetEngineObject()->SetScale( &playerScale );
		player->SetPosition( &playerPosition );

		mCameraConfiguration->SetRace( player->GetRace() );
		VECTOR3 cameraPosition = mCameraConfiguration->GetCameraPositionByRace( player->GetRace() );
		VECTOR3 cameraRotation = mCameraConfiguration->GetCameraRotationByRace( player->GetRace() );
		g_pExecutive->GetGeometry()->SetCameraPos( &cameraPosition, 0 );
		g_pExecutive->GetGeometry()->SetCameraAngleRad( &cameraRotation, 0 );
	}

	ProcessDragRotation();
}

void CCharMake::BeforeRender()
{
	g_pExecutive->GetRenderer()->BeginRender(0,0,0);
	g_pExecutive->GetRenderer()->EndRender();
	MAP->Process( gCurTime );
}

void CCharMake::AfterRender()
{	
	WINDOWMGR->Render();
	mImageBottomRightBar->RenderSprite(
		&mScaleBottomRightBar,
		0,
		0,
		&mPositionBottomRightBar,
		0xffffffff);
	mImageBottomLeftBar->RenderSprite(
		&mScaleBottomLeftBar,
		0,
		0,
		&mPositionBottomLeftBar,
		0xffffffff);
}

void CCharMake::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	switch(Category) 
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
			case MP_USERCONN_CHARACTER_NAMECHECK_NACK:
				{
					UserConn_Character_NameCheck_Nack( pMsg ) ;
				}
				return;
			case MP_USERCONN_CHARACTER_NAMECHECK_ACK:
				{
					UserConn_Character_NameCheck_Ack() ;
				}
				return;
			case MP_USERCONN_CHARACTER_MAKE_ACK:
				{
					UserConn_Character_Make_Ack( pMsg ) ;
				}
				return;
			case MP_USERCONN_CHARACTER_MAKE_NACK:
				{
					UserConn_Character_Make_Nack() ;
				}
				return;
			case MP_USERCONN_CHARACTERLIST_ACK:
				{
					UserConn_CharacterList_Ack( pMsg ) ;
				}
				return;
			}
		}
		return;
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
		break;
	}
//	LOG(EC_UNKNOWN_PROTOCOL);
}


void CCharMake::DisplayNotice( int MsgNum )
{
	WINDOWMGR->MsgBox( MBI_CHARMAKEERROR, MBT_OK, CHATMGR->GetChatMsg(MsgNum) );
	SetDisableDlg( TRUE );
}

void CCharMake::SetDisableDlg( BOOL val )
{
	if( m_pCharMakeNewDlg )
		m_pCharMakeNewDlg->SetDisable( val ) ;
}

void CCharMake::UserConn_Character_NameCheck_Nack( void* pMsg )
{
	CHARMAKEMGR->GetCharacterMakeInfo()->bDuplCheck = FALSE;
	cEditBox * editboxName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(CM_IDEDITBOX);
	editboxName->SetEditText("");
	DisplayNotice( 14 );
	SetDisableDlg( TRUE );

	// 070619 LYW --- GlobalEventFunc : Setting To Disable Gender button.
	// 성별을 선택할 수 없도록 하기 위해 Disable로 세팅해 준다.
	GetCharMakeNewDlg()->GetPushUpButton(e_PB_MAN)->SetDisable(TRUE) ;
	GetCharMakeNewDlg()->GetPushUpButton(e_PB_WOMAN)->SetDisable(TRUE) ;
}


//=================================================================================================
//	NAME		: UserConn_Character_NameCheck_Ack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharMake::UserConn_Character_NameCheck_Ack()
{
	CHARMAKEMGR->GetCharacterMakeInfo()->bDuplCheck =TRUE;
	
	DisplayNotice( 13 );
	SetDisableDlg( FALSE );

	// 070619 LYW --- GlobalEventFunc : Setting To Disable Gender button.
	// 성별을 선택할 수 없도록 하기 위해 Disable로 세팅해 준다.
	GetCharMakeNewDlg()->GetPushUpButton(e_PB_MAN)->SetDisable(TRUE) ;
	GetCharMakeNewDlg()->GetPushUpButton(e_PB_WOMAN)->SetDisable(TRUE) ;
}


//=================================================================================================
//	NAME		: UserConn_Character_Make_Nack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharMake::UserConn_Character_Make_Nack()
{
	DisplayNotice( 16 );

	SetDisableDlg( FALSE );
}


//=================================================================================================
//	NAME		: UserConn_CharacterList_Ack()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharMake::UserConn_CharacterList_Ack( void* pMsg )
{
	SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
	SetDisableDlg( FALSE );
	MAINGAME->SetGameState(eGAMESTATE_CHARSELECT, (void *)pmsg, sizeof(SEND_CHARSELECT_INFO));									
	
	CAMERA->MoveStart(eCP_CHARSEL);		
}


//=================================================================================================
//	NAME		: Cheat_EventNotify_On()
//	PURPOSE		: Process error message.
//	DATE		: February 5 2007 LYW
//	ATTENTION	: 
//=================================================================================================
void CCharMake::Cheat_EventNotify_On( void* pMsg  )
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
void CCharMake::Cheat_EventNotify_Off( void* pMsg )
{
	NOTIFYMGR->SetEventNotify( FALSE );
	NOTIFYMGR->SetEventNotifyChanged( FALSE );
}


void CCharMake::ProcessDragRotation()
{
	CMouse*		mouse	= g_UserInput.GetMouse();
	CPlayer*	player	= CHARMAKEMGR->GetNewPlayer();

	if( ! mouse		||
		! player )
	{
		return;
	}

	const int	x	= mouse->GetMouseX();
	const int	y	= mouse->GetMouseY();

	if( mouse->LButtonDown() )
	{
		mPosition.mIsDrag	= TRUE;
		mPosition.mX		= mouse->GetMouseX();
		mPosition.mY		= mouse->GetMouseY();
	}
	else if( mouse->LButtonUp() )
	{
		mPosition.mIsDrag	= FALSE;
	}
	
	if( ! mPosition.mIsDrag ||
		( mPosition.mX == x && mPosition.mY == y ) )
	{
		if( mRotation.mRadian )
		{
			player->SetAngle( player->GetAngle() + mRotation.mSpeed );

			if( 0 > ( mRotation.mRadian -= abs( mRotation.mSpeed ) ) )
			{
				mRotation.mRadian = 0;
			}
		}

		return;
	}
	
	// 마우스를 클릭한채로 이동했으면 라디안 값을 구해 캐릭터를 회전시킨다.
	{
		const float bias = 300.0f;
		const float	vectorSize = sqrtf( pow( float( mPosition.mX - x ), 2 ) + pow( float( mPosition.mY - y ), 2 ) );
		const float	ratio = vectorSize / bias;
		const float	direction = ( x > mPosition.mX ? 1.0f : -1.0f );

		mRotation.mRadian = 2.0f * PI * ratio;
		mRotation.mSpeed = direction * ratio;
		mPosition.mX = x;
		mPosition.mY = y;
	}
}

void CCharMake::UserConn_Character_Make_Ack( void* pMsg )
{
	// 090625 ONS 시작맵을 신규종족과 기존종족을 구분하여 설정하도록 메세지에 추가.
	MSG_DWORD2* pmsg = ( MSG_DWORD2* )pMsg;
	const DWORD objectIndex = pmsg->dwData1;
	const MAPTYPE mapType = MAPTYPE(pmsg->dwData2);

	MSG_WORD message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_CHARACTERSELECT_SYN;
	message.dwObjectID = objectIndex;
	message.wData = WORD(gChannelNum);
	NETWORK->Send(&message, sizeof(message));

	MAINGAME->SetGameState(
		eGAMESTATE_MAPCHANGE,
		LPVOID(&mapType),
		sizeof(mapType));

	CAMERA->ResetInterPolation();

	gChannelNum = 0;

	m_bEnterGame = TRUE;
}
