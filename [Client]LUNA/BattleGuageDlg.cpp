#include "stdafx.h"
#include "BattleGuageDlg.h"
#include "WindowIDEnum.h"
#include "ObjectGuagen.h"
#include "./Interface/cStatic.h"
#include "./Interface/cWindowManager.h"
#include "Monster.h"
#include "GameResourceManager.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "BattleSystem_client.h"
#include "OptionManager.h"
#include "GameIn.h"
#include "MonsterGuageDlg.h"

cBattleGuageDlg::cBattleGuageDlg()	: cDialog()
{
	m_pTargetBattleHPGuageMain	=	NULL;
	m_pHeroBattleHPGuageMain	=	NULL;

	m_pTargetBattleHPGuage		=	NULL;
	m_pHeroBattleHPGuage		=	NULL;

	m_pTargetLevelSignalRed		=	NULL;	
	m_pTargetLevelSignalYellow	=	NULL;
	m_pTargetLevelSignalWhite	=	NULL;
	m_pTargetLevelSignalGreen	=	NULL;
	m_pTargetLevelSignalGrey	=	NULL;

	m_pCurLevelSignal			=	NULL;

	m_pHeroLevelSignalRed		=	NULL;	
	m_pHeroLevelSignalYellow	=	NULL;
	m_pHeroLevelSignalWhite		=	NULL;
	m_pHeroLevelSignalGreen		=	NULL;
	m_pHeroLevelSignalGrey		=	NULL;

	m_pCurHeroLevelSignal		=	NULL;

	m_pTargetHPSignal			=	NULL;
	m_pHeroHPSignal				=	NULL;
	m_pCurTargetObject			=	NULL;
	m_dwAlphaTick				=	0;
}

cBattleGuageDlg::~cBattleGuageDlg()
{
	
}

void cBattleGuageDlg::Render()
{	
	if( !OPTIONMGR->GetGameOption()->bIsOnBattleGuage )
		return;

	if(m_dwAlphaTick > gCurTime)
		SetAlpha( BYTE((m_dwAlphaTick - gCurTime) / 20 ) + 5 );

	CObject* pTargetObject	=	OBJECTMGR->GetObject( HERO->GetAutoAttackTarget()->GetTargetID() );

	pTargetObject	=	m_pCurTargetObject;

	if( !pTargetObject )
		return;

	POINT pt = { 0, };
	POINT hPt = { 0, };
	
	//Target
	GetObjectScreenCoord( pTargetObject, &pt );

	if( m_pTargetBattleHPGuage )
	{
		m_pTargetBattleHPGuageMain->SetAbsXY( (LONG)pt.x + m_ptTargetHPGuageMainDelta.x, (LONG)pt.y + m_ptTargetHPGuageMainDelta.y );
		m_pTargetBattleHPGuage->SetAbsXY( (LONG)pt.x + m_ptTargetHPGuageDelta.x, (LONG)pt.y + m_ptTargetHPGuageDelta.y );

		if( m_pCurLevelSignal ) 
			m_pCurLevelSignal->SetAbsXY( (LONG)pt.x + m_ptTargetLvSignalDelta.x, (LONG)pt.y + m_ptTargetLvSignalDelta.y );

		m_pTargetHPSignal->SetAbsXY( (LONG)pt.x + m_ptTargetHPSignalDelta.x, (LONG)pt.y + m_ptTargetHPSignalDelta.y );	//X Delta
	}

	GetObjectScreenCoord( HERO, &hPt );

	if( m_pHeroBattleHPGuage )
	{
		m_pHeroBattleHPGuageMain->SetAbsXY( (LONG)hPt.x + m_ptHeroHPGuageMainDelta.x, (LONG)hPt.y + m_ptHeroHPGuageMainDelta.y );
		m_pHeroBattleHPGuage->SetAbsXY( (LONG)hPt.x + m_ptHeroHPGuageDelta.x, (LONG)hPt.y + m_ptHeroHPGuageDelta.y );

		if(m_pCurHeroLevelSignal )
			m_pCurHeroLevelSignal->SetAbsXY( (LONG)hPt.x + m_ptHeroLvSignalDelta.x, (LONG)hPt.y + m_ptHeroLvSignalDelta.y );

		m_pHeroHPSignal->SetAbsXY( (LONG)hPt.x + m_ptHeroHPSignalDelta.x, (LONG)hPt.y + m_ptHeroHPSignalDelta.y );	
	}	
	
	cDialog::Render();
}

void cBattleGuageDlg::SetAlpha( BYTE al )
{
	if( m_pTargetHPSignal && m_pHeroHPSignal )
	{
		m_pHeroHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
		m_pTargetHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
	}

	cDialog::SetAlpha( al );
}

void cBattleGuageDlg::Linking()
{
	m_pTargetBattleHPGuageMain	=	(cStatic*)GetWindowForID(CG_TARGET_BATTLEHP_GUAGE_MAIN);
	m_pHeroBattleHPGuageMain	=	(cStatic*)GetWindowForID(CG_HERO_BATTLEHP_GUAGE_MAIN);

	m_pTargetBattleHPGuage	=	(CObjectGuagen*)GetWindowForID(CG_TARGET_BATTLEHP_GUAGE);
	m_pTargetBattleHPGuage->SetActive( FALSE );

	m_pHeroBattleHPGuage	=	(CObjectGuagen*)GetWindowForID(CG_HERO_BATTLEHP_GUAGE);
	m_pHeroBattleHPGuage->SetActive( FALSE );

	m_pTargetLevelSignalRed		=	(cStatic*)GetWindowForID(CG_TARGET_LEVELSIGNAL_RED);
	m_pTargetLevelSignalYellow	=	(cStatic*)GetWindowForID(CG_TARGET_LEVELSIGNAL_YELLOW);
	m_pTargetLevelSignalWhite	=	(cStatic*)GetWindowForID(CG_TARGET_LEVELSIGNAL_WHITE);
	m_pTargetLevelSignalGreen	=	(cStatic*)GetWindowForID(CG_TARGET_LEVELSIGNAL_GREEN);
	m_pTargetLevelSignalGrey	=	(cStatic*)GetWindowForID(CG_TARGET_LEVELSIGNAL_GREY);

	m_pTargetLevelSignalRed->SetActive( FALSE );
	m_pTargetLevelSignalYellow->SetActive( FALSE );
	m_pTargetLevelSignalWhite->SetActive( FALSE );
	m_pTargetLevelSignalGreen->SetActive( FALSE );
	m_pTargetLevelSignalGrey->SetActive( FALSE );

	m_pHeroLevelSignalRed		=	(cStatic*)GetWindowForID(CG_HERO_LEVELSIGNAL_RED);
	m_pHeroLevelSignalYellow	=	(cStatic*)GetWindowForID(CG_HERO_LEVELSIGNAL_YELLOW);
	m_pHeroLevelSignalWhite		=	(cStatic*)GetWindowForID(CG_HERO_LEVELSIGNAL_WHITE);
	m_pHeroLevelSignalGreen		=	(cStatic*)GetWindowForID(CG_HERO_LEVELSIGNAL_GREEN);
	m_pHeroLevelSignalGrey		=	(cStatic*)GetWindowForID(CG_HERO_LEVELSIGNAL_GREY);

	m_pHeroLevelSignalRed->SetActive( FALSE );
	m_pHeroLevelSignalYellow->SetActive( FALSE );
	m_pHeroLevelSignalWhite->SetActive( FALSE );
	m_pHeroLevelSignalGreen->SetActive( FALSE );
	m_pHeroLevelSignalGrey->SetActive( FALSE );


	m_pTargetHPSignal	=	(cStatic*)GetWindowForID(CG_TARGET_HP_SIGNAL);
	m_pHeroHPSignal		=	(cStatic*)GetWindowForID(CG_HERO_HP_SIGNAL);

	m_pHeroHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, 150 ) );
	m_pTargetHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, 150 ) );
	
	m_pTargetHPSignal->SetTextAlpha( TRUE );
	m_pHeroHPSignal->SetTextAlpha( TRUE );

	m_pTargetHPSignal->SetActive( FALSE );
	m_pHeroHPSignal->SetActive( FALSE );

	m_ptTargetLvSignalDelta.x		=	(LONG)m_pTargetLevelSignalRed->GetRelX();
	m_ptTargetLvSignalDelta.y		=	(LONG)m_pTargetLevelSignalRed->GetRelY();

	m_ptTargetHPGuageDelta.x		=	(LONG)m_pTargetBattleHPGuage->GetRelX();
	m_ptTargetHPGuageDelta.y		=	(LONG)m_pTargetBattleHPGuage->GetRelY();

	m_ptTargetHPGuageMainDelta.x	=	(LONG)m_pTargetBattleHPGuageMain->GetRelX();
	m_ptTargetHPGuageMainDelta.y	=	(LONG)m_pTargetBattleHPGuageMain->GetRelY();

	m_ptTargetHPSignalDelta.x		=	(LONG)m_pTargetHPSignal->GetRelX();
	m_ptTargetHPSignalDelta.y		=	(LONG)m_pTargetHPSignal->GetRelY();

	m_ptHeroHPSignalDelta.x			=	(LONG)m_pHeroHPSignal->GetRelX();
	m_ptHeroHPSignalDelta.y			=	(LONG)m_pHeroHPSignal->GetRelY();

	m_ptHeroHPGuageDelta.x			=	(LONG)m_pHeroBattleHPGuage->GetRelX();
	m_ptHeroHPGuageDelta.y			=	(LONG)m_pHeroBattleHPGuage->GetRelY();

	m_ptHeroHPGuageMainDelta.x		=	(LONG)m_pHeroBattleHPGuageMain->GetRelX();
	m_ptHeroHPGuageMainDelta.y		=	(LONG)m_pHeroBattleHPGuageMain->GetRelY();

	m_ptHeroLvSignalDelta.x			=	(LONG)m_pHeroLevelSignalRed->GetRelX();
	m_ptHeroLvSignalDelta.y			=	(LONG)m_pHeroLevelSignalRed->GetRelY();
}

DWORD cBattleGuageDlg::SetTargetLevelSignal( DWORD dwTargetLv )
{
	DWORD	dwHeroLevel		=	HERO->GetLevel();
	DWORD	dwTargetLevel	=	dwTargetLv;
	DWORD	dwLevelGap		=	0;

	DWORD	dwColor	=	0;

 	if( dwHeroLevel > dwTargetLevel )
	{
		dwLevelGap	=	dwHeroLevel - dwTargetLevel;
		if( dwLevelGap >= 1 && dwLevelGap <= 7 )
		{
			
			m_pNextLevelSignal		=	m_pTargetLevelSignalGreen;
			
			m_pNextHeroLevelSignal	=	m_pHeroLevelSignalYellow;
			dwColor	=	RGBA_MAKE( 245, 112, 19, 0 );
		}
		else if( dwLevelGap >= 8 && dwLevelGap <= 15 )
		{
			//³ì»ö
			m_pNextLevelSignal		=	m_pTargetLevelSignalGreen;
			//³ë¶û
			m_pNextHeroLevelSignal	=	m_pHeroLevelSignalYellow;
			dwColor	=	RGBA_MAKE( 0, 128, 0, 0 );
		}
		else
		{
			//È¸»ö
			m_pNextLevelSignal		=	m_pTargetLevelSignalGrey;
			//ºÓÀº»ö
			m_pNextHeroLevelSignal	=	m_pHeroLevelSignalRed;
			dwColor	=	RGBA_MAKE( 128, 128, 128, 0 );
		}
	}
	else if( dwTargetLevel > dwHeroLevel )
	{
		dwLevelGap	=	dwTargetLevel - dwHeroLevel;
		if( dwLevelGap >= 1&& dwLevelGap <= 7 )
		{
			//³ë¶û
			m_pNextLevelSignal		=	m_pTargetLevelSignalYellow;

			m_pNextHeroLevelSignal	=	m_pHeroLevelSignalGreen;
			dwColor	=	RGBA_MAKE( 245, 112, 19, 0 );
		}
		else
		{
			//ºÓÀº
			m_pNextLevelSignal		=	m_pTargetLevelSignalRed;
			m_pNextHeroLevelSignal	=	m_pHeroLevelSignalGrey;
			dwColor	=	RGBA_MAKE( 255, 0, 0, 0 );
		}
	}
	else
	{
		//Èò»ö
		m_pNextLevelSignal		=	m_pTargetLevelSignalWhite;
		m_pNextHeroLevelSignal	=	m_pHeroLevelSignalWhite;
		dwColor	=	RGBA_MAKE( 0, 0, 0, 0 );
	}
	return	dwColor;
}

void cBattleGuageDlg::StartBattleSignal()
{
	CBattle* pBattle = BATTLESYSTEM->GetBattle();

	CObject* pTargetObject	=	OBJECTMGR->GetObject( HERO->GetAutoAttackTarget()->GetTargetID() );

	if( pTargetObject )
	{
		if( pBattle->IsEnemy( HERO, pTargetObject ) )
		{
			m_pCurTargetObject		=	pTargetObject;
			m_bIsOnBattleGuage	=	TRUE;
		}
		else 
			return;
	}
	else
	{	
		if( m_pCurTargetObject && m_pCurTargetObject->IsDied() )
		{	
			TargetDie();
		}
		return;
	}

	if( pTargetObject->GetID() == GAMEIN->GetMonsterGuageDlg()->GetWantedTargetID() )
	{
		m_pCurLevelSignal		=	m_pNextLevelSignal;
		m_pCurHeroLevelSignal	=	m_pNextHeroLevelSignal;
	}

	m_dwAlphaTick	=	0;

	m_pTargetBattleHPGuage->SetActive( TRUE);
	m_pHeroBattleHPGuage->SetActive( TRUE );
	m_pTargetHPSignal->SetActive( TRUE );
	m_pHeroHPSignal->SetActive( TRUE );

	AllLevelSignalActice( FALSE );

	if( m_pCurLevelSignal )
		m_pCurLevelSignal->SetActive( TRUE );

	if( m_pCurHeroLevelSignal )
		m_pCurHeroLevelSignal->SetActive( TRUE );

	SetBattleTargetInfo( pTargetObject );
	//HERO
	float	fHLife		=	(float)HERO->GetLife();
	float	fHMaxLife	=	(float)HERO->GetMaxLife();

	char szHeroHPSignal[20] = { 0, };
	float	fHeroHPSignal	=	fHLife / fHMaxLife;
	int		iHeroHPSignal	=	(int)( fHeroHPSignal * 100 );
	itoa( iHeroHPSignal, szHeroHPSignal, 10  );
	strcat( szHeroHPSignal, "%" );
	m_pHeroHPSignal->SetStaticText( szHeroHPSignal );

	m_pHeroBattleHPGuage->SetValue( fHLife / fHMaxLife, 0 );

	SetAlpha( 255 );

	
	m_pHeroHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, 150 ) );
	m_pTargetHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, 150 ) );

}

void cBattleGuageDlg::EndBattleSignal()
{
	m_bIsOnBattleGuage	=	FALSE;

	m_pCurTargetObject	=	NULL;

	m_pTargetBattleHPGuage->SetActive( FALSE );
	m_pHeroBattleHPGuage->SetActive( FALSE );

	AllLevelSignalActice( FALSE );

	m_pTargetHPSignal->SetActive( FALSE );
	m_pHeroHPSignal->SetActive( FALSE );
}

void cBattleGuageDlg::HeroBattleGuageUpdate()
{
	if( !m_bIsOnBattleGuage )
		return;

	float	fHLife		=	(float)HERO->GetLife();
	float	fHMaxLife	=	(float)HERO->GetMaxLife();

	char szHeroHPSignal[20] = { 0, };
	float	fHeroHPSignal	=	fHLife / fHMaxLife;
	int		iHeroHPSignal	=	(int)( fHeroHPSignal * 100 );
	itoa( iHeroHPSignal, szHeroHPSignal, 10  );
	strcat( szHeroHPSignal, "%" );
	m_pHeroHPSignal->SetStaticText( szHeroHPSignal );

	m_pHeroBattleHPGuage->SetValue( fHLife / fHMaxLife, 0 );

	m_pHeroHPSignal->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) );

	SetAlpha( 255 );
}


void cBattleGuageDlg::SetBattleTargetInfo( CObject* pTargetObject )
{
	float	fLife		=	(float)pTargetObject->GetLife();
	float	fMaxLife	=	(float)pTargetObject->GetMaxLife();
	
	char szTargetHPSignal[20] = { 0, };
	float	fTargetHPSignal	=	fLife / fMaxLife;
	int		iTargetHPSignal	=	(int)( fTargetHPSignal * 100 );
	itoa( iTargetHPSignal, szTargetHPSignal, 10 );
	strcat( szTargetHPSignal, "%" );
	m_pTargetHPSignal->SetStaticText( szTargetHPSignal );

	m_pTargetBattleHPGuage->SetValue( fTargetHPSignal, 0 );
}

void cBattleGuageDlg::GetObjectScreenCoord( CObject* pObject, POINT* pPosition )
{
	VECTOR3 OutPos3, InPos3;

	if( !pObject || !pPosition )
		return;

	pObject->GetPosition( &InPos3 );
	GetScreenXYFromXYZ( g_pExecutive->GetGeometry(), 0, &InPos3, &OutPos3 );
	if( OutPos3.x < 0 || OutPos3.x > 1 || OutPos3.y < 0 || OutPos3.y > 1 ) return;

	pPosition->x	=	(LONG)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwWidth*OutPos3.x);
	pPosition->y	=	(LONG)(GAMERESRCMNGR->m_GameDesc.dispInfo.dwHeight*OutPos3.y);
}

void cBattleGuageDlg::TargetDie()
{
	m_dwAlphaTick	=	gCurTime + 5000;

	m_pTargetHPSignal->SetStaticText( "0%" );

	m_pTargetBattleHPGuage->SetValue( 0.0f, 0 );
}

void cBattleGuageDlg::HeroDie()
{
	m_dwAlphaTick	=	gCurTime + 5000;
	
	m_pHeroHPSignal->SetStaticText( "0%" );

	m_pHeroBattleHPGuage->SetValue( 0.0f, 0 );
}


void cBattleGuageDlg::AllLevelSignalActice( BOOL val )
{
	m_pTargetLevelSignalRed->SetActive( FALSE );
	m_pTargetLevelSignalYellow->SetActive( FALSE );
	m_pTargetLevelSignalWhite->SetActive( FALSE );
	m_pTargetLevelSignalGreen->SetActive( FALSE );
	m_pTargetLevelSignalGrey->SetActive( FALSE );

	m_pHeroLevelSignalRed->SetActive( FALSE );
	m_pHeroLevelSignalYellow->SetActive( FALSE );
	m_pHeroLevelSignalWhite->SetActive( FALSE );
	m_pHeroLevelSignalGreen->SetActive( FALSE );
	m_pHeroLevelSignalGrey->SetActive( FALSE );
}
