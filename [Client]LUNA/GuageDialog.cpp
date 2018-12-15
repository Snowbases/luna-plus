// GuageDialog.cpp: implementation of the CGuageDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuageDialog.h"
#include "ObjectGuagen.h"
#include "WindowIDEnum.h"
#include "./Interface/cButton.h"
#include "./interface/cStatic.h"
#include "MHTimeManager.h"
#include "ObjectManager.h"

// 070110 LYW --- Include header files to need.
#include "MonsterGuageDlg.h"
#include "Party.h"
#include "PartyManager.h"
#include "ChatManager.h"
#include "GameIn.h"

// 070118 LYW --- Include header file to need.
#include "./Interface/cResourceManager.h"

#include "cMsgBox.h"

#include "./Input/Mouse.h"
#include "./interface/cWindowManager.h"

// 071004 LYW --- GuageDialog : Include script manager.
#include "./Interface/cScriptManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuageDialog::CGuageDialog()
{
/*	m_pMussangBtn	= NULL;
	m_pFlicker01	= NULL;
	m_pFlicker02	= NULL;

	m_bFlicker		= FALSE;
	m_bFlActive		= FALSE;
	m_dwFlickerSwapTime	= 0;	*/

	// 070110 LYW --- Add controls for popup menu.
	int count = 0 ;

	m_pMenuButton = NULL ;

	for( count = 0 ; count < MAX_POPUP1_STATIC ; ++count )
	{
		m_pBGImage1[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_POPUP2_STATIC ; ++count )
	{
		m_pBGImage2[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_POPUP1_BUTTON ; ++count )
	{
		m_pPopupBtn1[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_POPUP2_BUTTON ; ++count )
	{
		m_pPopupBtn2[ count ] = NULL ;
	}

	// 070117 LYW --- Add static controls for charater image.
	for( count = 0 ; count < MAX_HERO_FACE ; ++count )
	{
		m_pHeroFace[ count ] = NULL ;
	}

	m_bShowPopupMenu1	= FALSE ; 
	m_bShowPopupMenu2	= FALSE ; 

	m_byHeroState		= e_NONE ;
}

CGuageDialog::~CGuageDialog()
{

}

void CGuageDialog::Linking()
{
	for( int count = 0 ; count < MAX_HERO_FACE ; ++count )
	{
		m_pHeroFace[ count ] = ( cStatic* )GetWindowForID( CG_ST_HERO_ELF_MAN + count ) ;
		m_pHeroFace[ count ]->SetActive( FALSE ) ;
	}

	const DWORD masterMarkImageIndex = 118;
	SCRIPTMGR->GetImage(
		masterMarkImageIndex,
		&m_MasterMark,
		PFT_HARDPATH ) ;
}

void CGuageDialog::Render()
{
//	FlickerMussangGuage();
	//cDialog::Render();

	if( !IsActive() ) return ;
	cDialog::RenderWindow();
	cDialog::RenderComponent();

	if(PARTYMGR->GetMasterID() == gHeroID)
	{
		const float extraWidth = 11.0f;
		const float markWidth = 14.0f;
		VECTOR2 position = {
			GetAbsX() + float(GetWidth()) - extraWidth - markWidth,
			GetAbsY() + 8.0f};
		VECTOR2 scale = {1.0f, 1.0f};

		m_MasterMark.RenderSprite(
			&scale,
			0,
			0,
			&position,
			0xffffffff) ;
	}
}


// 070110 LYW --- Add function to processing event.
//=================================================================================================
// NAME			: ActionEvent
// PURPOSE		: Processing envet function.
// ATTENTION	:
//=================================================================================================
DWORD CGuageDialog::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = WE_NULL ;

	we |= cDialog::ActionEvent( mouseInfo ) ;

	if( we & WE_LBTNCLICK )
	{
		OBJECTMGR->SetSelectedObject( HERO );
	}

	return we ;
}

void CGuageDialog::SetPopupMenu()
{
	int count = 0 ;

	DWORD basicColor = m_pPopupBtn1[0]->GetBasicColor() ;
	DWORD overColor  = m_pPopupBtn1[0]->GetOverColor() ;
	DWORD pressColor = m_pPopupBtn1[0]->GetPressColor() ;

	if( m_bShowPopupMenu1 )
	{
		switch( m_byHeroState )
		{
		case e_CAPTAIN :
			{
				for( count = 0 ; count < MAX_POPUP1_STATIC ; ++count )
				{
					m_pBGImage1[ count ]->SetActive( TRUE ) ;
				}

				for( count = 0 ; count < MAX_POPUP1_BUTTON ; ++count )
				{
					m_pPopupBtn1[ count ]->SetActive( TRUE ) ;
				}

				// 070118 LYW --- Modified this part.
				/*
				m_pPopupBtn1[0]->SetText("아이템 분배" , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[1]->SetText("파티 탈퇴" , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[2]->SetText("취소" , basicColor, overColor, pressColor ) ;
				*/

				m_pPopupBtn1[0]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[1]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[2]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
			}
			break ;

		case e_MEMBER :
			{
				//m_byCurPopupMode = e_STANDARD ;

				m_pBGImage1[0]->SetActive( TRUE ) ;
				m_pBGImage1[1]->SetActive( TRUE ) ;

				for( count = 0 ; count < MAX_POPUP1_BUTTON-1 ; ++count )
				{
					m_pPopupBtn1[ count ]->SetActive( TRUE ) ;
				}

				// 070118 LYW --- Modified this part.
				/*
				m_pPopupBtn1[0]->SetText("파티 탈퇴" , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[1]->SetText("취소" , basicColor, overColor, pressColor ) ;
				*/

				m_pPopupBtn1[0]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
				m_pPopupBtn1[1]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
			}
			break ;

		case e_NONE :
			{
				m_pBGImage1[0]->SetActive( TRUE ) ;
				m_pPopupBtn1[0]->SetActive( TRUE ) ;

				// 070118 LYW --- Modified this line.
                //m_pPopupBtn1[0]->SetText("취소" , basicColor, overColor, pressColor ) ;				
				m_pPopupBtn1[0]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;				
			}
			break ;
		}
	}
	else
	{
		// 070110 LYW --- Add controls for popup menu.
		for( count = 0 ; count < MAX_POPUP1_STATIC ; ++count )
		{
			m_pBGImage1[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_POPUP2_STATIC ; ++count )
		{
			m_pBGImage2[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_POPUP1_BUTTON ; ++count )
		{
			m_pPopupBtn1[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_POPUP2_BUTTON ; ++count )
		{
			m_pPopupBtn2[ count ]->SetActive( FALSE ) ;
		}
	}
}


//=================================================================================================
// NAME			: SetSecondPopup
// PURPOSE		: Setting  second popup mode.
// ATTENTION	:
//=================================================================================================
void CGuageDialog::SetSecondPopup()
{
	int count = 0 ;

	DWORD basicColor = m_pPopupBtn2[0]->GetBasicColor() ;
	DWORD overColor  = m_pPopupBtn2[0]->GetOverColor() ;
	DWORD pressColor = m_pPopupBtn2[0]->GetPressColor() ;

	if( m_bShowPopupMenu2 )
	{
		for( count = 0 ; count < MAX_POPUP2_STATIC ; ++count )
		{
			m_pBGImage2[ count ]->SetActive( TRUE ) ;
		}

		for( count = 0 ; count < MAX_POPUP2_BUTTON ; ++count )
		{
			m_pPopupBtn2[ count ]->SetActive( TRUE ) ;
		}

		// 070118 LYW --- Modified this part.
		/*
		m_pPopupBtn2[0]->SetText("파티장 분배"		, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[1]->SetText("순서대로 분배"	, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[2]->SetText("랜덤 분배"		, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[3]->SetText("데미지 우선 분배" , basicColor, overColor, pressColor ) ;
		*/

		m_pPopupBtn2[0]->SetText(RESRCMGR->GetMsg( 982)	, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[1]->SetText(RESRCMGR->GetMsg( 982)	, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[2]->SetText(RESRCMGR->GetMsg( 982)	, basicColor, overColor, pressColor ) ;
		m_pPopupBtn2[3]->SetText(RESRCMGR->GetMsg( 982) , basicColor, overColor, pressColor ) ;
	}
	else
	{
		for( count = 0 ; count < MAX_POPUP2_STATIC ; ++count )
		{
			m_pBGImage2[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_POPUP2_BUTTON ; ++count )
		{
			m_pPopupBtn2[ count ]->SetActive( FALSE ) ;
		}
	}
}


//=================================================================================================
// NAME			: SecessionPartyCaptain
// PURPOSE		: Processing withdrawal captain from party.
// ATTENTION	:
//=================================================================================================
void CGuageDialog::SecessionPartyCaptain()
{
	int count = 0 ; 
	DWORD dwNextCaptain = 0 ;

	for( count = 1 ; count <  MAX_PARTY_LISTNUM ; ++count ) 
	{
		dwNextCaptain = PARTYMGR->GetParty().GetMemberID(count) ;

		if( dwNextCaptain != 0 ) break ;
	}

	if( count < MAX_PARTY_LISTNUM )
	{
		dwNextCaptain = PARTYMGR->GetParty().GetMemberID(count) ;

		MSG_DWORD2 msg;
		msg.Category = MP_PARTY;
		msg.Protocol = MP_PARTY_CHANGEMASTER_SYN;
		msg.dwObjectID = HEROID;
		msg.dwData1 = dwNextCaptain;
		msg.dwData2 = HERO->GetPartyIdx();
		NETWORK->Send(&msg, sizeof(msg));
		
		//PARTYMGR->SetChangingMaster(TRUE);
	}
}


//=================================================================================================
//	NAME		: MakeHeroFace()
//	PURPOSE		: Add function for character face.
//	DATE		: January 17, 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CGuageDialog::MakeHeroFace()
{
	int count = 0 ;

	for( count = 0 ; count < MAX_HERO_FACE ; ++count )
	{
		m_pHeroFace[ count ]->SetActive( FALSE ) ;
	}

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( HERO->GetID() );

	BYTE racial		= pPlayer->GetCharacterTotalInfo()->Race ;
	BYTE gender		= pPlayer->GetCharacterTotalInfo()->Gender ;

	switch( racial )
	{
	case RaceType_Human :
		{
			if( gender == 0 )
			{
				m_pHeroFace[2]->SetActive( TRUE ) ;
			}
			else
			{
				m_pHeroFace[3]->SetActive( TRUE ) ;
			}
		}
		break ;

	case RaceType_Elf :
		{
			if( gender == 0 )
			{
				m_pHeroFace[0]->SetActive( TRUE ) ;
			}
			else
			{
				m_pHeroFace[1]->SetActive( TRUE ) ;
			}
		}
		break ;
	// 090504 ONS 신규종족 이미지 추가
	case RaceType_Devil :
		{
			if( gender == 0 )
			{
				m_pHeroFace[4]->SetActive( TRUE ) ;
			}
			else
			{
				m_pHeroFace[5]->SetActive( TRUE ) ;
			}
		}
		break ;
	}
}

void CGuageDialog::SetActive(BOOL val)
{
	if( val )
	{
		MakeHeroFace();
	}

	cDialog::SetActive( val );
}