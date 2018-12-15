#include "stdafx.h"
#include "CharacterDialog.h"
#include "ObjectManager.h"
#include "Interface/CWindowManager.h"
#include "Interface/cStatic.h"
#include "Interface/cButton.h"
#include "WindowIDEnum.h"
#include "GuildManager.h"
#include "GuildMarkDialog.h"
#include "CheatMsgParser.h"
#include "CharacterCalcManager.h"

#include "GameIn.h"
#include "..\hseos\Family\SHFamily.h"
#include "ChatManager.h"

// 070111 LYW --- Include header file to need.
#include "cResourceManager.h"

// 071022 LYW --- CharacterDialog : Include header file of tutorial manager.
#include "TutorialManager.h"

#include "GameIn.h"
#include "MiniMapDlg.h"


CCharacterDialog::CCharacterDialog()
{
	// 070113 LYW --- Add variable for count.
	int count = 0 ;

	m_type = WT_CHARINFODIALOG;
	m_nocoriPoint = 0;
	// 070111 LYW --- Add button control for call class tree dialog.
	m_pClassTreeBtn = NULL ;

	m_bShowTreeDlg		= FALSE ;
	m_bCheckedTreeInfo  = FALSE ;

	// 070112 LYW --- Add variable to store up character image number.
	m_byCharImgNum = 0x01 ;

	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_CHANGE_TEXT ; ++count )
	{
		m_pChangeText[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_STATUS_BTN ; ++count )
	{
		m_pStatusBtn[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_SUB_ITEMS ; ++count )
	{
		m_pSubItem[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
	{
		m_pClassStatic[ count ] = NULL ;
	}

	m_pCloseBtn			= NULL ;
	m_pClassTreeBtn		= NULL ;

	m_pTreeBgImg		= NULL ;
	m_pTreeMark			= NULL ;
	m_pTreeTitle		= NULL ;

	// 070416 LYW --- CharacterDialog : Add static to print racial.
	m_pRacial = NULL ;

	// 070503 LYW --- CharacterDialog : Add static to setting family name.
	m_pFamilyName = NULL ;
}

CCharacterDialog::~CCharacterDialog()
{

}

void CCharacterDialog::Linking()
{
	// 070111 LYW --- Add variable for count.
	int count = 0 ;

	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ] = ( cStatic* )GetWindowForID( CI_CHAR_IMAGE0 + count ) ;
	}

	for( count = 0 ; count < MAX_CHANGE_TEXT ; ++count )
	{
		m_pChangeText[ count ] = ( cStatic* )GetWindowForID( CI_CHANGED_TEXT0 + count ) ;
		m_pChangeText[ count ]->SetActive( TRUE ) ;
	}

	for( count = 0 ; count < MAX_STATUS_BTN ; ++count )
	{
		m_pStatusBtn[ count ] = ( cButton* )GetWindowForID( CI_STATUS_BTN_0 + count ) ;
		// 070122 LYW --- Setting to deactive.
		m_pStatusBtn[ count ]->SetActive( FALSE ) ;
	}

	for( count = 0 ; count < MAX_SUB_ITEMS ; ++count )
	{
		m_pSubItem[ count ] = ( cStatic* )GetWindowForID( CI_SUB_ITEMS1 + count ) ;
	}

	m_pCloseBtn			= ( cButton* )GetWindowForID( CI_CLOSEBTN ) ;
	m_pClassTreeBtn		= ( cButton* )GetWindowForID( CI_CLASSTREEBTN ) ;

	m_pTreeBgImg		= ( cStatic* )GetWindowForID( CI_TREEBGIMG ) ;
	m_pTreeMark			= ( cStatic* )GetWindowForID( CI_TREEMARK ) ;
	m_pTreeTitle		= ( cStatic* )GetWindowForID( CI_TREETITLE ) ;

	for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
	{
		m_pClassStatic[ count ] = ( cStatic* )GetWindowForID( CI_CLASS1_STATIC + count ) ;
	}

	m_pRacial = (cStatic*)GetWindowForID(CI_RACIAL) ;

	// 070503 LYW --- CharacterDialog : Add family name.
	m_pFamilyName = (cStatic*)GetWindowForID(CI_FAMILYNAME) ;
}
void CCharacterDialog::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	//m_type = WT_CHARINFODIALOG;
}

void CCharacterDialog::UpdateData()
{
	BASEOBJECT_INFO ObjInfo;
	CHARACTER_TOTALINFO ChaInfo;
	HERO_TOTALINFO HeroInfo;
	OBJECTMGR->GetHero()->GetBaseObjectInfo(&ObjInfo);
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);
	OBJECTMGR->GetHero()->GetHeroTotalInfo(&HeroInfo);

	
	m_pChangeText[1]->SetStaticValue(ChaInfo.BadFame);
	m_pChangeText[2]->SetStaticText(ObjInfo.ObjectName);

	// 070416 LYW --- CharacterDialog : Setting Class name.
	WORD idx = 1;
	if( ChaInfo.JobGrade > 1 )
	{
		idx = ChaInfo.Job[ChaInfo.JobGrade - 1];
	}
	WORD job = ( ChaInfo.Job[0] * 1000 ) + ( ( ChaInfo.Race + 1 ) * 100 ) + ( ChaInfo.JobGrade * 10 ) + idx;

	m_pChangeText[5]->SetStaticText( RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job)) ) ; 

	// 070416 LYW --- CharacterDialog : Setting racial.
	// 090504 ONS 캐릭터다이얼로그 신규종족명 추가 처리
	if(ChaInfo.Race == RaceType_Human)
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(247)) ;
	}
	else if(ChaInfo.Race == RaceType_Elf)
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(248)) ;
	}
	else
	{
		m_pRacial->SetStaticText(RESRCMGR->GetMsg(1129)) ;
	}

	// 080214 LYW --- CharacterDialog : 패밀리 정보 새로고침 추가.
	RefreshFamilyInfo() ;
}

void CCharacterDialog::SetExpPointPercent(float perc)
{
	static char buf[66];
	sprintf(buf, "[%.2f%%]", perc);

#ifdef _CHEATENABLE_	
	if(CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		wsprintf(buf1, " : %d/%d", HERO->GetExpPoint(), HERO->GetMaxExpPoint());
		strcat(buf, buf1);
	}
#endif

	m_ppStatic.expPercent->SetStaticText(buf);
}
void CCharacterDialog::SetLevel(WORD level)
{
	// 070113 LYW --- Modified this line.
	//m_ppStatic.level->SetStaticValue(level);
	m_pChangeText[0]->SetStaticValue(level);
}

void CCharacterDialog::SetStrength()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetStrength();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Str;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[6]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[6]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[6]->SetStaticText( buf );
}
void CCharacterDialog::SetDexterity()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetDexterity();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Dex;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[7]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[7]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[7]->SetStaticText( buf );
}
void CCharacterDialog::SetVitality()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetVitality();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Vit;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[8]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[8]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[8]->SetStaticText( buf );
}
void CCharacterDialog::SetWisdom()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetWisdom();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Wis;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[9]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[9]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[9]->SetStaticText( buf );
}

void CCharacterDialog::SetIntelligence()
{
	char buf[256] = {0,};
	DWORD resVal = HERO->GetIntelligence();		//KES 070530 : WORD --> DWORD
	DWORD baseVal = HERO->GetHeroTotalInfo()->Int;	//KES 070530 : WORD --> DWORD
	DWORD gapVal = 0;

	// 070412 LYW --- CharacterDialog : Apply extend color to strength
	if( m_pChangeText[10]->IsHasExtendColor() )
	{
		if( baseVal > resVal )
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(1) ) ;

			gapVal = baseVal - resVal;

			sprintf( buf, "%d - %d", baseVal, gapVal );
		}
		else if( baseVal < resVal )
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(2) ) ;

			gapVal = resVal - baseVal;
			
			sprintf( buf, "%d+%d", baseVal, gapVal );
		}
		else
		{
			m_pChangeText[10]->SetFGColor( m_pChangeText[6]->GetExtendColor(0) ) ;

			sprintf( buf, "%d", baseVal );
		}
	}

	m_pChangeText[10]->SetStaticText( buf );
}

void CCharacterDialog::SetBadFame(FAMETYPE badfame)
{
	m_pChangeText[1]->SetStaticValue(badfame); 
}

void CCharacterDialog::SetAttackRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetAttackRate() ) ;
	m_pChangeText[12]->SetStaticText(buf1) ;
}
void CCharacterDialog::SetDefenseRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetDefenseRate() ) ;
	m_pChangeText[17]->SetStaticText(buf1) ;
}
void CCharacterDialog::SetCritical()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetCritical() ) ;
	m_pChangeText[14]->SetStaticText(buf1) ;
}

// 070411 LYW --- CharacterDialog : Add function to print values of physical part.
void CCharacterDialog::PrintAttackRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetAttackRate() ) ;
	m_pChangeText[12]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintDefenseRate()
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetDefenseRate() ) ;
	m_pChangeText[17]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMagAttackRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetMagAttackRate() ) ;
	m_pChangeText[18]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMagDefenseRate() 
{
	char buf1[66] = {0, } ;
	wsprintf( buf1, "%d", HERO->GetMagDefenseRate() ) ;
	m_pChangeText[21]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintAccuracyRate()
{
	char buf1[66] = {0, } ;
	const int nAccuracyRate = (int)HERO->GetAccuracyRate();
	if( nAccuracyRate > 0 )			wsprintf( buf1, "+%d%%", nAccuracyRate );
	else							wsprintf( buf1, "%d%%", nAccuracyRate );
	m_pChangeText[16]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintEvasionRate() 
{
	char buf1[66] = {0, } ;
	const int nEvasionRate = (int)HERO->GetEvasionRate();
	if( nEvasionRate > 0 )			wsprintf( buf1, "+%d%%", nEvasionRate );
	else							wsprintf( buf1, "%d%%", nEvasionRate );
	m_pChangeText[15]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintCriticalRate() 
{
	char buf1[66] = {0, } ;
	const int nCriticalRate = (int)HERO->DoGetCriticalRate();
	if( nCriticalRate > 0 )			wsprintf( buf1, "+%d%%", nCriticalRate );
	else							wsprintf( buf1, "%d%%", nCriticalRate );
	m_pChangeText[14]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMagicCriticalRate()
{
	char buf1[66] = {0, } ;
	const int nCriticalRate = (int)HERO->DoGetMagicCriticalRate();
	if( nCriticalRate > 0 )			wsprintf( buf1, "+%d%%", nCriticalRate );
	else							wsprintf( buf1, "%d%%", nCriticalRate );
	m_pChangeText[24]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintMoveSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nMoveSpeedRate = (int)HERO->DoGetMoveSpeedRate();
	if( nMoveSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nMoveSpeedRate );
	else							wsprintf( buf1, "%d%%", nMoveSpeedRate );
	m_pChangeText[20]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintAttackSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nAttackSpeedRate = (int)HERO->DoGetAttackSpeedRate();
	if( nAttackSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nAttackSpeedRate );
	else							wsprintf( buf1, "%d%%", nAttackSpeedRate );
	m_pChangeText[19]->SetStaticText(buf1) ;
}

void CCharacterDialog::PrintCastingSpeedRate()
{
	char buf1[66] = {0, } ;
	const int nCastingSpeedRate = -(int)HERO->DoGetCastingSpeedRate();
	if( nCastingSpeedRate > 0 )		wsprintf( buf1, "+%d%%", nCastingSpeedRate );
	else							wsprintf( buf1, "%d%%", nCastingSpeedRate );
	m_pChangeText[23]->SetStaticText(buf1) ;
}

void CCharacterDialog::SetAttackRange()
{
	// 070114 LYW --- Modified this line.
	//m_ppStatic.attackdistance->SetStaticText("-");
	m_pChangeText[13]->SetStaticText( "-" ) ;
}
void CCharacterDialog::SetPointLeveling(BOOL val, WORD point)
{
	m_nocoriPoint = point;	
	m_pChangeText[11]->SetStaticValue( m_nocoriPoint );
	// 070113 LYW --- Delete this line.
	/*
	for(int i = 0 ; i < MAX_BTN_POINT ; i++)
		m_pPointBtn[i]->SetActive(val);
		*/
	
	HERO->SetHeroLevelupPoint( point );

	// 070124 LYW --- Setting status button.
	if( m_nocoriPoint > 0 )
	{
		for(int i = 0 ; i < MAX_BTN_POINT ; i++)
			m_pStatusBtn[i]->SetActive(TRUE);
	}
	else
	{
		for(int i = 0 ; i < MAX_BTN_POINT ; i++)
			m_pStatusBtn[i]->SetActive(FALSE);
	}
}
void CCharacterDialog::SetPointLevelingHide()
{
	WORD lvpoint = 0;
	m_pChangeText[11]->SetStaticValue( lvpoint );
	// 070113 LYW --- Delete this line.
	/*
	for(int i = 0 ; i < MAX_BTN_POINT ; i++)
		m_pPointBtn[i]->SetActive(FALSE);
		*/
	
	HERO->SetHeroLevelupPoint( 0 );
}

void CCharacterDialog::OnAddPoint(BYTE whatsPoint)
{
	MSG_WORD msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_POINTADD_SYN;
	msg.dwObjectID = HEROID;
	msg.wData = whatsPoint;

	
	if(--m_nocoriPoint <= 0)
	{
		SetPointLeveling(FALSE);
		//return;
	}
	// 070114 LYW --- Modified this line.
	//m_ppStatic.point->SetStaticValue(m_nocoriPoint);
	m_pChangeText[11]->SetStaticValue(m_nocoriPoint);
	
	NETWORK->Send(&msg,sizeof(MSG_WORD));
}

void CCharacterDialog::RefreshGuildInfo()
{
	if( strlen(HERO->GetGuildName()) <= 0 ) 
	{
		m_pChangeText[3]->SetStaticText("") ;
	}
	else
	{
		m_pChangeText[3]->SetStaticText(HERO->GetGuildName());
	}
}

// 070503 LYW --- CharacterDialog : Add function to setting family name
void CCharacterDialog::RefreshFamilyInfo()
{
	CSHFamily* pFamily = HERO->GetFamily() ;

	if( pFamily )																	// 패밀리 정보가 유효한지 체크한다.
	{
		m_pFamilyName->SetStaticText(pFamily->Get()->szName) ;						// 패밀리명을 세팅한다.
	}
	else
	{
		m_pFamilyName->SetStaticText("") ;
	}
}

void CCharacterDialog::SetActive( BOOL val )
{
	cDialog::SetActive(
		val);

	GAMEIN->GetMiniMapDialog()->ChangePushLevelUpBTN();

	RefreshInfo();
	TUTORIALMGR->Check_OpenDialog(
		GetID(),
		val);
}

void CCharacterDialog::RefreshInfo()
{
	if(FALSE == IsActive())
	{
		return;
	}

	HERO->PassiveSkillCheckForWeareItem();
	CheckHeroImage();

	SetStrength();
	SetDexterity();
	SetVitality();
	SetIntelligence() ;
	SetWisdom();
	SetAttackRate();
	SetDefenseRate();
	SetCritical();
	UpdateData();

	PrintAttackRate() ;
	PrintDefenseRate() ;
	PrintMagAttackRate() ;
	PrintMagDefenseRate() ;
	PrintAccuracyRate() ;
	PrintEvasionRate() ;
	PrintCriticalRate() ;
	PrintMagicCriticalRate() ;
	PrintMoveSpeedRate() ;
	PrintAttackSpeedRate() ;
	PrintCastingSpeedRate() ;
}


// 070111 LYW --- Add function to process events.
//=================================================================================================
// NAME			: ActionEvent
// PURPOSE		: Processing envet function.
// ATTENTION	:
//=================================================================================================
DWORD CCharacterDialog::ActionEvent( CMouse* mouseInfo )
{
	DWORD we = WE_NULL ;

	we |= cDialog::ActionEvent( mouseInfo ) ;

	return we ;
}


//=================================================================================================
// NAME			: OnActionEvent
// PURPOSE		: Processing envet function.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::OnActionEvent( LONG lId, void* p, DWORD we )
{
	if( lId == CI_CLASSTREEBTN )
	{
		if( !m_bCheckedTreeInfo )
		{
			m_bCheckedTreeInfo = TRUE ;
			// 070416 LYW --- CharacterDialog : Delete call check tree.
			//CheckTreeInfo() ;
		}

		if( m_bShowTreeDlg )
		{
			m_bShowTreeDlg = FALSE ;
			ShowTreeDlg() ;
		}
		else
		{
			m_bShowTreeDlg = TRUE ;
			ShowTreeDlg() ;
		}
	}
}


// 070111 LYW --- Add function to check and show tree information.
//=================================================================================================
// NAME			: CheckTreeInfo
// PURPOSE		: Check information of class tree.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::CheckTreeInfo()
{
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( gHeroID );
    
	if( !pPlayer )
		return;

	BYTE jobGrade	= pPlayer->GetCharacterTotalInfo()->JobGrade ;
	BYTE job		= pPlayer->GetCharacterTotalInfo()->Job[0] ;

	switch( job )
	{
	case 1 :
		{
			// 070122 LYW --- Modified this line.
			//m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 41) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 2 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 42) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 3 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 43) ) ;
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 4 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 44) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 5 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 45) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;

	case 6 :
		{
			m_pClassStatic[0]->SetStaticText( RESRCMGR->GetMsg( 46) ) ;
			m_pClassStatic[1]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[2]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[3]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[4]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[5]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[6]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[7]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
			m_pClassStatic[8]->SetStaticText( RESRCMGR->GetMsg( 982) ) ;
		}
		break ;
	}

	for(int i=0; i<9; ++i)
	{
		m_pClassStatic[i]->SetFGColor( RGB(255, 255, 255) ) ;
	}
	switch( jobGrade )
	{
	case 1 :
		{
			m_pClassStatic[0]->SetFGColor( RGB(0, 255, 255) ) ;
			m_pChangeText[5]->SetStaticText( m_pClassStatic[0]->GetStaticText() ) ;
		}
		break ;

	case 2 :
		{
			m_pClassStatic[1]->SetFGColor( RGB(0, 255, 255) ) ;
			m_pChangeText[5]->SetStaticText( m_pClassStatic[1]->GetStaticText() ) ;
		}
		break ;

	case 3 :
		{
			BYTE thirdJob = pPlayer->GetCharacterTotalInfo()->Job[2] ;

			if( (thirdJob%2) != 0 )
			{
				m_pClassStatic[2]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pChangeText[5]->SetStaticText( m_pClassStatic[2]->GetStaticText() ) ;
			}
			else
			{
				m_pClassStatic[3]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pChangeText[5]->SetStaticText( m_pClassStatic[3]->GetStaticText() ) ;
			}
		}
		break ;

	case 4 :
		{
			m_pClassStatic[4]->SetFGColor( RGB(0, 255, 255) ) ;
			m_pChangeText[5]->SetStaticText( m_pClassStatic[4]->GetStaticText() ) ;
		}
		break ;

	case 5 :
		{
			BYTE thirdJob = pPlayer->GetCharacterTotalInfo()->Job[4] ;

			char tempClassName[128] = {0, } ;

			if( (thirdJob%2) != 0 )
			{
				m_pClassStatic[5]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pClassStatic[6]->SetFGColor( RGB(0, 255, 255) ) ;

				sprintf( tempClassName, "%s%s", m_pClassStatic[5]->GetStaticText(), m_pClassStatic[5]->GetStaticText() ) ;
				m_pChangeText[5]->SetStaticText( tempClassName ) ;
			}
			else
			{
				m_pClassStatic[7]->SetFGColor( RGB(0, 255, 255) ) ;
				m_pClassStatic[8]->SetFGColor( RGB(0, 255, 255) ) ;

				sprintf( tempClassName, "%s%s", m_pClassStatic[7]->GetStaticText(), m_pClassStatic[8]->GetStaticText() ) ;
				m_pChangeText[5]->SetStaticText( tempClassName ) ;
			}
		}
		break ;
	}
}


//=================================================================================================
// NAME			: ShowTreeDlg
// PURPOSE		: Setting whether controls for tree dialog to show or not.
// ATTENTION	:
//=================================================================================================
void CCharacterDialog::ShowTreeDlg()
{
	int count = 0 ;

	if( m_bShowTreeDlg )
	{
		m_pTreeBgImg->SetActive( TRUE ) ;
		m_pTreeMark->SetActive( TRUE ) ;
		m_pTreeTitle->SetActive( TRUE ) ;

		for( count = 0 ; count < MAX_SUBITEM ; ++count )
		{
			m_pSubItem[ count ]->SetActive( TRUE ) ;
		}

		for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
		{
			m_pClassStatic[ count ]->SetActive( TRUE ) ;
		}
	}
	else
	{
		m_pTreeBgImg->SetActive( FALSE ) ;
		m_pTreeMark->SetActive( FALSE ) ;
		m_pTreeTitle->SetActive( FALSE ) ;

		for( count = 0 ; count < MAX_SUBITEM ; ++count )
		{
			m_pSubItem[ count ]->SetActive( FALSE ) ;
		}

		for( count = 0 ; count < MAX_CLASS_STATIC ; ++count )
		{
			m_pClassStatic[ count ]->SetActive( FALSE ) ;
		}
	}
}


//=================================================================================================
// NAME			: ShowTreeDlg
// PURPOSE		: Add function to check character image.
// ATTENTION	: // 070112 LYW --- Add function to check character image.
//=================================================================================================
void CCharacterDialog::CheckHeroImage()
{
	int count = 0 ;

	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject( HERO->GetID() );
	if( NULL == pPlayer )
	{
		return;
	}
	m_byCharImgNum	= pPlayer->GetCharacterTotalInfo()->Race ;

	// 090423 ONS 신규종족 캐릭터 정보 다이얼로그 히어로 이미지 추가
	for( count = 0 ; count < MAX_CHAR_IMAGE ; ++count )
	{
		m_pCharImage[ count ]->SetActive( FALSE ) ;
	}

	switch( m_byCharImgNum )
	{
		// 070122 LYW --- Modified this part.

	case RaceType_Human :
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[2]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[3]->SetActive( TRUE ) ;
			}
		}
		break ;

	case RaceType_Elf :
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[0]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[1]->SetActive( TRUE ) ;
			}
		}
		break ;
	// 090423 ONS 신규종족 캐릭터 정보 다이얼로그 히어로 이미지 추가
	case RaceType_Devil:
		{
			if( pPlayer->GetCharacterTotalInfo()->Gender == 0 )
			{
				m_pCharImage[4]->SetActive( TRUE ) ;
			}
			else
			{
				m_pCharImage[5]->SetActive( TRUE ) ;
			}
		}
		break;
	}
}
