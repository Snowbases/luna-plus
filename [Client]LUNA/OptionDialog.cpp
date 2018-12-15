#include "stdafx.h"
#include "OptionDialog.h"
#include "WindowIDEnum.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cGuageBar.h"
#include "./Interface/cCheckBox.h"
#include "./interface/cStatic.h"
#include "MacroManager.h"
#include "GameIn.h"
#include "cMsgBox.h"
#include "ObjectManager.h"
#include "./interface/cScriptManager.h"
#include "ChatManager.h"

#include "TutorialManager.h"
// 100112 ONS 배경, 효과음 게이지바 마우스 이벤트 처리를 위해 추가
#include "./Input/Mouse.h"

COptionDialog::COptionDialog()
{
}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::Add( cWindow* window )
{
	WORD wWindowType = window->GetType() ;																		// 윈도우 타입을 받아온다.

	if( wWindowType == WT_PUSHUPBUTTON )																		// 윈도우 타입이 푸쉬업 버튼이면,
	{
		AddTabBtn( curIdx1++, (cPushupButton*)window ) ;														// 현재 인덱스1로 탭 버튼을 추가한다.
	}
	else if( wWindowType == WT_DIALOG )																			// 윈도우 타입이 다이얼로그이면,
	{
		AddTabSheet( curIdx2++, window ) ;																		// 현재 인덱스2로 쉬트를 추가한다.
	}
	else																										// 이도 저도 아니면,
	{
		cTabDialog::Add( window ) ;																				// 윈도우를 추가한다.
	}
}

void COptionDialog::SetActive( BOOL val )
{
	if( !m_bDisable )																							// 비활성화 상태가 아니면,
	{
		if( val )																								// val 값이 TRUE 이면,
		{
			m_GameOption = *( OPTIONMGR->GetGameOption() ) ;													// 옵션 매니져의 게임 옵션을 받아온다.

			UpdateData( FALSE ) ;																				// FALSE 모드로 업데이트를 한다.					
		}

		cTabDialog::SetActive( val ) ;																			// 탭다이얼 로그를 val 세팅한다.
	}
}

void COptionDialog::Linking()
{
}

void COptionDialog::UpdateData( BOOL bSave )
{
	cDialog* pDlg ;

	if( bSave )
	{
		pDlg = (cDialog*)GetTabSheet( 0 );	

		m_GameOption.bNoDeal		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NODEAL ))->IsChecked();
		m_GameOption.bNoParty		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOPARTY ))->IsChecked();
		m_GameOption.bNoFriend		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFRIEND ))->IsChecked();
		m_GameOption.bNoShowdown	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFIGHT ))->IsChecked();
		m_GameOption.bNoDateMatching= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NODATEMATCHING ))->IsChecked();	
		m_GameOption.bIsOnBattleGuage	=	((cCheckBox*)pDlg->GetWindowForID( 	OTI_CB_ISONBATTLEGUAGE ))->IsChecked();
		m_GameOption.bNameMunpa		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MUNPANAME ))->IsChecked();
		m_GameOption.bNameParty		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_PARTYNAME ))->IsChecked();
		m_GameOption.bNameOthers	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_OTHERSNAME ))->IsChecked();
		m_GameOption.bNoMemberDamage	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MEMBERDAMAGE ))->IsChecked();
		m_GameOption.bNoGameTip		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_GAMETIP ))->IsChecked();
		// ---------------------------------------------
		// 090116 ShinJS --- 안보이기 옵션의 컨트롤 설정
		m_GameOption.bHideOtherPlayer 	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HIDEOTHERPLAYER ))->IsChecked();
		m_GameOption.bHideOtherPet 		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HIDEOTHERPET ))->IsChecked();
		m_GameOption.bHideStreetStallTitle	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HIDESTREETSTALLTITLE ))->IsChecked();
		// ---------------------------------------------
		m_GameOption.nMacroMode		= ((cPushupButton*)pDlg->GetWindowForID( OTI_PB_MACROMODE ))->IsPushed();
		// 100322 ONS 퀘스트알리미 자동등록 여부 체크 추가
		m_GameOption.bAutoQuestNotice		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_AUTOQUESTNOTICE ))->IsChecked();
		
		pDlg = (cDialog*)GetTabSheet( 1 );

		m_GameOption.bNoWhisper		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOWHISPER ))->IsChecked();
		m_GameOption.bNoChatting	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOCHATTING ))->IsChecked();
		m_GameOption.bNoBalloon		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOBALLOON ))->IsChecked();
		m_GameOption.bNoPartyChat	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOPARTYCHAT ))->IsChecked() ;
		m_GameOption.bNoFamilyChat	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFAMILYCHAT ))->IsChecked() ;
		m_GameOption.bNoShoutChat	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_WORLDHIDE ))->IsChecked();
		m_GameOption.bNoGuildChat	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MUNPAHIDE ))->IsChecked();
		m_GameOption.bNoAllianceChat= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_ALLMUNPAHIDE ))->IsChecked();
		m_GameOption.bNoSystemMsg	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOSYSTEMMSG ))->IsChecked();
		m_GameOption.bNoExpMsg		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOACQUIREMSG ))->IsChecked();
		m_GameOption.bNoItemMsg		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOITEMMSG ))->IsChecked();
		// 080916 KTH -- 전체 외치기 창 옵션 체크
		m_GameOption.bShowGeneralShoutDlg	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_GENERAL_SHOUTDLG ))->IsChecked();

		pDlg = (cDialog*)GetTabSheet( 2 ); // 옵션에서 그래픽설정부분이다.

		m_GameOption.nSightDistance = ((cGuageBar*)pDlg->GetWindowForID( OTI_GB_SIGHT ))->GetCurValue(); // 시야거리
		m_GameOption.bShadowHero	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HEROSHADOW ))->IsChecked(  ); // 자신
		m_GameOption.bShadowMonster = ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MONSTERSHADOW ))->IsChecked(  ); // 몬스터/NPC
		m_GameOption.bShadowOthers	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_OTHERSSHADOW ))->IsChecked( ); // 다른케릭터

		m_GameOption.dwHideParts = eOPT_HIDEPARTS_NONE;
		if(!((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
		{
			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_BAND;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_GLASSES;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_WING;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_HEAD;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_DRESS;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_GLOVE;

			if(((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->IsChecked())
				m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_SHOES;
		}
		else
		{
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_BAND;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_GLASSES;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_EQUIP_WING;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_HEAD;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_DRESS;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_GLOVE;
			m_GameOption.dwHideParts |= eOPT_HIDEPARTS_COSTUME_SHOES;
		}

#ifndef _JAPAN_LOCAL_
		m_GameOption.bAutoCtrl		= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_AUTOCONTROL ))->IsChecked() ;

		m_GameOption.nLODMode = ((cPushupButton*)pDlg->GetWindowForID( OTI_PB_DOWNGRAPHIC ))->IsPushed(); 
		m_GameOption.nEffectMode = ((cPushupButton*)pDlg->GetWindowForID( OTI_PB_ONEEFFECT ))->IsPushed();
#endif //_JAPAN_LOCAL_

		pDlg = (cDialog*)GetTabSheet( 3 );

		m_GameOption.nVolumnBGM			= ((cGuageBar*)pDlg->GetWindowForID( OTI_GB_BGMSOUND ))->GetCurValue();
		m_GameOption.nVolumnEnvironment = ((cGuageBar*)pDlg->GetWindowForID( OTI_GB_ENVSOUND ))->GetCurValue();

		m_GameOption.bSoundBGM			= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_BGMSOUND ))->IsChecked();
		m_GameOption.bSoundEnvironment	= ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_ENVSOUND ))->IsChecked();

		// 071025 LYW --- OptionDialog : Setting tutorial option.
		m_GameOption.bShowTutorial		= TUTORIALMGR->Is_ActiveTutorial() ;				// 튜토리얼 옵션을 세팅한다.

		OPTIONMGR->SetGameOption( &m_GameOption );
	}
	else	//정보 가지고 와서 창 세팅 (기본설정이 창이 뜸)
	{
		pDlg = (cDialog*)GetTabSheet( 0 );	

		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NODEAL ))->SetChecked( m_GameOption.bNoDeal );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOPARTY ))->SetChecked( m_GameOption.bNoParty );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFRIEND ))->SetChecked( m_GameOption.bNoFriend );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFIGHT ))->SetChecked( m_GameOption.bNoShowdown );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NODATEMATCHING ))->SetChecked( m_GameOption.bNoDateMatching );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_ISONBATTLEGUAGE ))->SetChecked( m_GameOption.bIsOnBattleGuage );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MUNPANAME ))->SetChecked( m_GameOption.bNameMunpa );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_PARTYNAME ))->SetChecked( m_GameOption.bNameParty );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_OTHERSNAME ))->SetChecked( m_GameOption.bNameOthers );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MEMBERDAMAGE ))->SetChecked( m_GameOption.bNoMemberDamage );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_GAMETIP ))->SetChecked( m_GameOption.bNoGameTip );
		// ---------------------------------------------
		// 090116 ShinJS --- 안보이기 옵션의 컨트롤 설정
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HIDEOTHERPLAYER ))->SetChecked( m_GameOption.bHideOtherPlayer );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HIDEOTHERPET ))->SetChecked( m_GameOption.bHideOtherPet );
		((cCheckBox*)pDlg->GetWindowForID(OTI_CB_HIDESTREETSTALLTITLE ))->SetChecked(m_GameOption.bHideStreetStallTitle );
		// ---------------------------------------------
		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_CHATMODE ))->SetPush( !m_GameOption.nMacroMode );
		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_MACROMODE ))->SetPush( m_GameOption.nMacroMode );
		// 100322 ONS 퀘스트알리미 자동등록 여부 체크 추가
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_AUTOQUESTNOTICE ))->SetChecked( m_GameOption.bAutoQuestNotice );

		// 090116 ShinJS --- 펫 안보이기 옵션 활성화/비활성화
		DisableOtherPetRender( m_GameOption.bHideOtherPlayer );

		
		pDlg = (cDialog*)GetTabSheet( 1 );	

		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOWHISPER ))->SetChecked( m_GameOption.bNoWhisper );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOCHATTING ))->SetChecked( m_GameOption.bNoChatting );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOBALLOON ))->SetChecked( m_GameOption.bNoBalloon );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOPARTYCHAT ))->SetChecked( m_GameOption.bNoPartyChat ) ;
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFAMILYCHAT ))->SetChecked( m_GameOption.bNoFamilyChat ) ;
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_WORLDHIDE ))->SetChecked( m_GameOption.bNoShoutChat );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MUNPAHIDE ))->SetChecked( m_GameOption.bNoGuildChat );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_ALLMUNPAHIDE ))->SetChecked( m_GameOption.bNoAllianceChat );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOSYSTEMMSG ))->SetChecked( m_GameOption.bNoSystemMsg );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOACQUIREMSG ))->SetChecked( m_GameOption.bNoExpMsg );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOITEMMSG ))->SetChecked( m_GameOption.bNoItemMsg );
		// 080916 KTH -- 전체 외치기 창 옵션 체크
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_GENERAL_SHOUTDLG ))->SetChecked( m_GameOption.bShowGeneralShoutDlg );

		pDlg = (cDialog*)GetTabSheet( 2 );

		if( m_GameOption.nSightDistance <= 55 )																// 게이지 값이 최소값인 55보다 작으면,
		{
			m_GameOption.nSightDistance = 55 ;																// 게이지 값을 55로 세팅해, 0이되거나, 
		}																									// 완전 시야를 가리는 일을 막는다.

		//m_GameOption.nSightDistance = 155 ;
		((cGuageBar*)pDlg->GetWindowForID( OTI_GB_SIGHT ))->SetCurValue( m_GameOption.nSightDistance );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_HEROSHADOW ))->SetChecked( m_GameOption.bShadowHero );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MONSTERSHADOW ))->SetChecked( m_GameOption.bShadowMonster );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_OTHERSSHADOW ))->SetChecked( m_GameOption.bShadowOthers );

		if(m_GameOption.dwHideParts == eOPT_HIDEPARTS_NONE)
		{
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked( FALSE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked( FALSE );
		}
		else if(m_GameOption.dwHideParts ==  eOPT_HIDEPARTS_ALL)
		{
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked( TRUE );
			((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked( TRUE );

			DisableGraphicHideParts(TRUE);
		}
		else
		{
			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_EQUIP_BAND)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_EQUIP_GLASSES)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_EQUIP_WING)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_COSTUME_HEAD)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_COSTUME_DRESS)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_COSTUME_GLOVE)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked( TRUE );

			if(m_GameOption.dwHideParts & eOPT_HIDEPARTS_COSTUME_SHOES)
				((cCheckBox*)pDlg->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked( TRUE );
		}

#ifndef _JAPAN_LOCAL_
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_AUTOCONTROL ))->SetChecked( m_GameOption.bAutoCtrl );

		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_BASICGRAPHIC ))->SetPush( !m_GameOption.nLODMode ) ;
		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_DOWNGRAPHIC ))->SetPush( m_GameOption.nLODMode ) ;
		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_BASICEFFECT ))->SetPush( !m_GameOption.nEffectMode ) ;
		((cPushupButton*)pDlg->GetWindowForID( OTI_PB_ONEEFFECT ))->SetPush( m_GameOption.nEffectMode ) ;

		DisableGraphicTab( m_GameOption.bAutoCtrl ) ;
#endif	//_JAPAN_LOCAL_

		pDlg = (cDialog*)GetTabSheet( 3 );

		((cGuageBar*)pDlg->GetWindowForID( OTI_GB_BGMSOUND ))->SetCurValue( m_GameOption.nVolumnBGM );
		((cGuageBar*)pDlg->GetWindowForID( OTI_GB_ENVSOUND ))->SetCurValue( m_GameOption.nVolumnEnvironment );

		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_BGMSOUND ))->SetChecked( m_GameOption.bSoundBGM );
		((cCheckBox*)pDlg->GetWindowForID( OTI_CB_ENVSOUND ))->SetChecked( m_GameOption.bSoundEnvironment );

		// 071025 LYW --- OptionDialog : Setting tutorial option.
		TUTORIALMGR->Set_ActiveTutorial(m_GameOption.bShowTutorial) ;				// 튜토리얼 옵션을 세팅한다.
	}

	DWORD FullColor = 0xffffffff;
	DWORD HalfColor = RGBA_MAKE(200,200,200,255);

	BOOL bDisable = TRUE ;

	if( bDisable )
	{
		FullColor = HalfColor;
	}

	pDlg = (cDialog*)GetTabSheet( 0 );
	cCheckBox* pCheckBox = NULL ;

	pCheckBox = ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_NOFRIEND )) ;
	if( pCheckBox )
	{
		pCheckBox->SetChecked(FALSE) ;

		pCheckBox->SetDisable(bDisable) ;

		pCheckBox->SetImageRGB( FullColor ) ;
	}

/*	pCheckBox = ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_GAMETIP )) ;
	if( pCheckBox )
	{
		pCheckBox->SetDisable(bDisable) ;

		pCheckBox->SetImageRGB( FullColor ) ;
	}
*/
	// ----------------------------------------------------------------
	// 090121 ShinJS --- 필요없는 옵션 제거 (옵션 - 직업별 이펙트 제거)
	//pCheckBox = ((cCheckBox*)pDlg->GetWindowForID( OTI_CB_MUNPAPR )) ;
	//if( pCheckBox )
	//{
	//	pCheckBox->SetDisable(bDisable) ;

	//	pCheckBox->SetImageRGB( FullColor ) ;
	//}
	// ----------------------------------------------------------------

	//pDlg = (cDialog*)GetTabSheet(2) ;
	//cGuageBar* pGuageBar = (cGuageBar*)pDlg->GetWindowForID( OTI_GB_SIGHT ) ;

	//if( pGuageBar )
	//{
	//	//pGuageBar->SetCurValue(155) ;

	//	pGuageBar->SetDisable(TRUE) ;

	//	pGuageBar->SetImageRGB( FullColor ) ;

	//	pGuageBar->SetGuageLock(TRUE, FullColor);

	//	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_SIGHT ))->SetFGColor(FullColor);
	//}
}

// 100112 ONS 배경, 효과음 게이지바 마우스 이벤트 처리를 위해 추가
DWORD COptionDialog::ActionEvent( CMouse* mouseInfo ) 
{
	DWORD we = WE_NULL;
	BOOL bIsChanged = FALSE;

	// 다이얼로그 이벤트를 받는다.
	we = cTabDialog::ActionEvent( mouseInfo ) ;

	cDialog* pDlg = (cDialog*)GetTabSheet( 3 );
	if( !pDlg )
		return WE_NULL;

	// 효과음 조절
	cGuageBar* pBgmGuageBar = (cGuageBar*)pDlg->GetWindowForID( OTI_GB_BGMSOUND );
	if( pBgmGuageBar &&	pBgmGuageBar->IsDrag() )
	{
		m_GameOption.nVolumnBGM = pBgmGuageBar->GetCurValue();
		bIsChanged = TRUE;
	}

	// 배경음악 조절
	cGuageBar* pEnvironmentGuageBar = (cGuageBar*)pDlg->GetWindowForID( OTI_GB_ENVSOUND );
	if( pEnvironmentGuageBar &&	pEnvironmentGuageBar->IsDrag() )
	{
		m_GameOption.nVolumnEnvironment = pEnvironmentGuageBar->GetCurValue();
		bIsChanged = TRUE;
	}
	
	if( bIsChanged )
	{
		OPTIONMGR->SetGameOption( &m_GameOption, FALSE );
		OPTIONMGR->ApplySound();
	}

	return we ;
}

void COptionDialog::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( we & WE_BTNCLICK )
	{
		if( lId == OTI_BTN_OK )
		{
			UpdateData( TRUE );
			OPTIONMGR->ApplySettings();
			OPTIONMGR->SaveGameOption();
			OPTIONMGR->SendOptionMsg();	
			SetActive( FALSE );
		}
		else if( lId == OTI_BTN_CANCEL )
		{
			OPTIONMGR->CancelSettings();
			SetActive( FALSE );
		}
		else if( lId == OTI_BTN_RESET )
		{
			OPTIONMGR->SetDefaultOption();
			m_GameOption = *( OPTIONMGR->GetGameOption() );
			UpdateData( FALSE );
		}
		else if( lId == OTI_BTN_PREVIEW )
		{
			UpdateData( TRUE );
			OPTIONMGR->ApplySettings();
			OPTIONMGR->SaveGameOption();
			OPTIONMGR->SendOptionMsg();		
		}
	}
	
	if( we & WE_PUSHUP )		//라디오 버튼 처리
	{
		if( lId == OTI_PB_CHATMODE || lId == OTI_PB_MACROMODE )
			((cPushupButton*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( lId ))->SetPush( TRUE );

#ifndef _JAPAN_LOCAL_
		if( lId == OTI_PB_BASICGRAPHIC || lId == OTI_PB_DOWNGRAPHIC )
		{
			((cPushupButton*)((cDialog*)GetTabSheet(2))->GetWindowForID(lId))->SetPush(TRUE) ;
		}
		if( lId == OTI_PB_BASICEFFECT || lId == OTI_PB_ONEEFFECT )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( lId ))->SetPush( TRUE );
		}
#endif //_JAPAN_LOCAL_
	}

	if( we & WE_PUSHDOWN )		//
	{
		if( lId == OTI_PB_CHATMODE )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_PB_MACROMODE ))->SetPush( FALSE );
		}
		else if( lId == OTI_PB_MACROMODE )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_PB_CHATMODE ))->SetPush( FALSE );
		}

#ifndef _JAPNA_LOCAL_
		if( lId == OTI_PB_BASICGRAPHIC )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_DOWNGRAPHIC ))->SetPush( FALSE );
		}
		else if( lId == OTI_PB_DOWNGRAPHIC )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICGRAPHIC ))->SetPush( FALSE );
		}
		if( lId == OTI_PB_BASICEFFECT )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_ONEEFFECT ))->SetPush( FALSE );
		}
		else if( lId == OTI_PB_ONEEFFECT )
		{
			((cPushupButton*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICEFFECT ))->SetPush( FALSE );
		}
#endif //_JAPNA_LOCAL_
	}

#ifndef _JAPAN_LOCAL_
	if( we & WE_CHECKED )
	{
		if( lId == OTI_CB_AUTOCONTROL )
		{
			DisableGraphicTab(TRUE) ;
		}
		// 090116 ShinJS --- 캐릭터 안보이기 옵션 변경에 대한 펫 안보이기 옵션 Disable 실행
		else if( lId == OTI_CB_HIDEOTHERPLAYER )
		{
			// 캐릭터가 안보이는 경우 펫도 안보여야 한다
			((cCheckBox*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_CB_HIDEOTHERPLAYER ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_CB_HIDEOTHERPET ))->SetChecked(TRUE);
			DisableOtherPetRender( TRUE );
		}

		else if(lId == OTI_HIDEPARTS_ALL)
		{
			// 모두숨기기 설정
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked(TRUE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked(TRUE);

			DisableGraphicHideParts(TRUE);
		}
		else if(lId == OTI_HIDEPARTS_EQUIP_BAND)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_EQUIP_GLASSES)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_EQUIP_WING)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_COSTUME_HEAD)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_COSTUME_DRESS)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_COSTUME_GLOVE)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked(TRUE);

			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_COSTUME_SHOES)
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked(TRUE);
			
			if(((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->IsChecked())
				((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_ALL ))->SetChecked(FALSE);
		}
	}
	else if( we & WE_NOTCHECKED )
	{
		if( lId == OTI_CB_AUTOCONTROL )
		{
			DisableGraphicTab(FALSE) ;
		}
		// 090116 ShinJS --- 캐릭터 안보이기 옵션 변경에 대한 펫 안보이기 옵션 Disable 실행
		else if( lId == OTI_CB_HIDEOTHERPLAYER )
		{
			((cCheckBox*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_CB_HIDEOTHERPLAYER ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_CB_HIDEOTHERPET ))->SetChecked(FALSE);
			DisableOtherPetRender(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_ALL)
		{
			// 모두숨기기 해제
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked(FALSE);
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked(FALSE);

			DisableGraphicHideParts(FALSE);
		}
		else if(lId == OTI_HIDEPARTS_EQUIP_BAND)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_EQUIP_GLASSES)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_EQUIP_WING)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_COSTUME_HEAD)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_COSTUME_DRESS)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_COSTUME_GLOVE)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ))->SetChecked(FALSE);
		else if(lId == OTI_HIDEPARTS_COSTUME_SHOES)
			((cCheckBox*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ))->SetChecked(FALSE);
	}
#endif //_JAPAN_LOCAL_
}

#ifndef _JAPAN_LOCAL_
void COptionDialog::DisableGraphicTab(BOOL bDisable)
{
			DWORD FullColor = 0xffffffff;
			DWORD HalfColor = RGBA_MAKE(200,200,200,255);

			DWORD StaticColor = RGBA_MAKE(10, 10, 10,255);

			if( bDisable )
			{
				FullColor = HalfColor;
				StaticColor = HalfColor ;
			}

			cWindow* pWnd = NULL;
			cGuageBar* pGuag = NULL;

			pGuag = ((cGuageBar*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_GB_SIGHT ));
			pGuag->SetDisable(bDisable);
			pGuag->SetImageRGB(FullColor);
			pGuag->SetGuageLock(bDisable, FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_SIGHT ))->SetFGColor(StaticColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_SIGHTFAR ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_HEROSHADOW ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_HEROSHADOW ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HEROSHADOW ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_MONSTERSHADOW ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_MONSTERSHADOW ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_MONSTERSHADOW ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_OTHERSSHADOW ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_CB_OTHERSSHADOW ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_OTHERSSHADOW ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICGRAPHIC ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICGRAPHIC ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_BASICGRAPHIC ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_DOWNGRAPHIC ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_DOWNGRAPHIC ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_DOWNGRAPHIC ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICEFFECT ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_BASICEFFECT ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_BASICEFFECT ))->SetFGColor(StaticColor);
//			((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_ONEEFFECT ))->SetDisable(FALSE);
			pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_PB_ONEEFFECT ));
			pWnd->SetDisable(bDisable);
			pWnd->SetImageRGB(FullColor);
			((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_ONEEFFECT ))->SetFGColor(StaticColor);	
}

void COptionDialog::DisableGraphicHideParts(BOOL bDisable)
{
	DWORD FullColor = 0xffffffff;
	DWORD HalfColor = RGBA_MAKE(200,200,200,255);

	DWORD StaticColor = RGBA_MAKE(10, 10, 10,255);

	cWindow* pWnd = NULL;

	if( bDisable )
	{
		FullColor = HalfColor;
		StaticColor = HalfColor ;
	}
	else
	{
		((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_EQUIP_BAND ))->SetFGColor(StaticColor);
	}

	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_EQUIP_BAND ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_BAND ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_EQUIP_GLASSES ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_GLASSES ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_EQUIP_WING ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_EQUIP_WING ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_COSTUME_HEAD ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_HEAD ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_COSTUME_DRESS ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_DRESS ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_COSTUME_GLOVE ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_GLOVE ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
	((cStatic*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_TEXT_HIDEPARTS_COSTUME_SHOES ))->SetFGColor(StaticColor);
	pWnd = ((cWindow*)((cDialog*)GetTabSheet( 2 ))->GetWindowForID( OTI_HIDEPARTS_COSTUME_SHOES ));
	pWnd->SetDisable(bDisable);
	pWnd->SetImageRGB(FullColor);
}

#endif //_JAPAN_LOCAL_

// 090116 ShinJS --- 펫 보이기 옵션 활성화/비활성화
void COptionDialog::DisableOtherPetRender(BOOL bDisable)
{
		DWORD FullColor = 0xffffffff;
		DWORD HalfColor = RGBA_MAKE(200,200,200,255);

		DWORD StaticColor = RGBA_MAKE(255, 255, 255,255);

		cWindow* pWnd = NULL;

		if( bDisable )
		{
			FullColor = HalfColor;
			StaticColor = HalfColor ;
		}

		((cStatic*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_TEXT_HIDEOTHERPET))->SetFGColor(StaticColor);
		pWnd = ((cWindow*)((cDialog*)GetTabSheet( 0 ))->GetWindowForID( OTI_CB_HIDEOTHERPET ));
		pWnd->SetDisable(bDisable);
		pWnd->SetImageRGB(FullColor);
}