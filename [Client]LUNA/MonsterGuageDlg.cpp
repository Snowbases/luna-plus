#include "stdafx.h"
#include "MonsterGuageDlg.h"
#include "ObjectGuagen.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "ChatManager.h"
#include "./Interface/cWindowManager.h"
#include "ObjectManager.h"
#include "PartyManager.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "MHMap.h"
#include "GuildTournamentMgr.h"
#include "cCheckBox.h"
#include "MonsterTargetDlg.h"
#include "CharacterTargetDlg.h"
#include "MHFile.h"
#include "../[cc]skill/client/manager/skillmanager.h"

#include "BattleSystem_client.h"

CMonsterGuageDlg::CMonsterGuageDlg() :
mAlphaTick(0)
{
	m_pHPMark = 0;
	m_pGuildName = 0;
	m_pUnionName = 0;
	m_pPlayerName = 0;
	m_playerLife = 0;
	m_pPlayerGuage = 0;
	m_pNPCFace = 0;
	m_pNPCName = 0;
	m_pNPCOccupation = 0;

	m_pCharacterMain	=	NULL;

	m_TargetDlgType	=	NONE_TYPE;

	m_pNormalMonster	=	NULL;
	m_pBossMonster		=	NULL;
	m_pCurTarget		=	NULL;

	m_dwWantedTarget	=	NULL;
	m_dwTargetVersusID	=	NULL;
}

CMonsterGuageDlg::~CMonsterGuageDlg()
{

}

void CMonsterGuageDlg::Linking()
{
	m_pHPMark = (cStatic*)GetWindowForID(CG_LIFEIMAGE);
	m_pHPMark->SetActive( FALSE );
	m_pMonsterGuage = (CObjectGuagen*)GetWindowForID(CG_MONSTERGUAGELIFE);
	m_pMonsterGuage->SetActive( FALSE );
	m_pGuildName = (cStatic*)GetWindowForID(CG_GUILDNAME);
	m_pUnionName = (cStatic*)GetWindowForID(CG_GUILDUNION_NAME);
	m_pPlayerName = (cStatic*)GetWindowForID(CG_CRTGUAGENAME);
	m_playerLife = (cStatic*)GetWindowForID(CG_CRTLIFE);
	m_pPlayerGuage = (CObjectGuagen*)GetWindowForID(CG_CRTGUAGELIFE);
	m_pPlayerGuage->SetActive( FALSE );
	m_pNPCFace = (cStatic*)GetWindowForID(CG_NPCRACE);
	m_pNPCFace->SetActive(FALSE);
	m_pNPCName = (cStatic*)GetWindowForID(CG_NPCNAME);
	m_pNPCOccupation = (cStatic*)GetWindowForID(CG_NPCOCCUPATION);
	m_pMasterName = (cStatic*)GetWindowForID(CG_PETMASTERNAME);
	m_pPetName = (cStatic*)GetWindowForID(CG_PETNAME);

	m_pCharacterMain	=	(cStatic*)GetWindowForID(CG_PLAYERGUAGEDLG_MAIN);

	m_pBossMonster->TargetDlgLink();
	m_pNormalMonster->TargetDlgLink();
	m_pCharacterTarget->TargetDlgLink();
	
	LoadMonsterBuffIconPosition();
}

void CMonsterGuageDlg::SetActive(BOOL val)
{
	if(MAP->GetMapNum() == GTMAPNUM)
	{
		val = FALSE;
	}

	cDialog::SetActive(val);

	if( val == FALSE )
	{
		m_dwWantedTarget	=	NULL;
		m_dwTargetVersusID	=	NULL;
		m_TargetDlgType	=	NONE_TYPE;
		m_pBossMonster->SetActive( val );
		m_pNormalMonster->SetActive( val );
		m_pCharacterTarget->SetActive( val );
		
		m_pCurTarget =	NULL;
	}
}

void CMonsterGuageDlg::SetMonsterLife( DWORD curVal, DWORD maxVal)
{
	float	fMonsterLife	=	float(curVal) / float(max(1, maxVal) );
	if( m_TargetDlgType != MONSTER_TYPE && m_TargetDlgType != BOSSMONSTER_TYPE && m_TargetDlgType !=  PC_TYPE )
	{
		m_pMonsterGuage->SetValue(
		float(curVal) / float(max(1, maxVal)),
		0);
	}
	else
	{
		m_pCurTarget->SetMonsterLife( fMonsterLife );
	}
}

void CMonsterGuageDlg::Render()
{
#ifdef _GMTOOL_
	if( WINDOWMGR->IsInterfaceHided() ) return;
#endif

	if(mAlphaTick > gCurTime)
	{
		SetAlpha(
			BYTE((mAlphaTick - gCurTime) / 20 ) + 5);
	}
	
	if( m_pCurTarget )
		m_pCurTarget->Render();

	cDialog::Render();
}

DWORD CMonsterGuageDlg::ActionEvent( CMouse* mouseInfo )
{
	const DWORD we = cDialog::ActionEvent( mouseInfo );

	if( we & WE_RBTNCLICK )
	{
		SetActive(FALSE ) ;
		OBJECTMGR->SetSelectedObjectID(0) ;
	}

	return we ;
}

void CMonsterGuageDlg::SetName( CObject* pObject, char* name )
{
	// 080827 LYW --- MonsterGuageDlg : 함수 인자 확인.
   	if( !pObject ) return ;

	m_dwWantedTarget	=	pObject->GetID();
	m_dwTargetVersusID	=	NULL;
   
	mAlphaTick = gCurTime;
	SetAlpha(
		255);

	m_pPlayerName->SetActive( FALSE ) ;
	m_pNPCName->SetActive( FALSE ) ;
	m_pNPCOccupation->SetActive( FALSE ) ;
	m_pMasterName->SetActive( FALSE ) ;	
	m_pPetName->SetActive( FALSE ) ;		

	m_pNormalMonster->SetMonsterUIActive( false );
	m_pNormalMonster->SetMonsterVSUIActive( false );
	m_pBossMonster->SetMonsterUIActive( false );
	m_pBossMonster->SetMonsterVSUIActive( false );
	m_pCharacterTarget->SetMonsterUIActive( false );
	m_pCharacterTarget->SetMonsterVSUIActive( false );

	m_TargetDlgType	=	NPC_TYPE;
	m_pCharacterMain->SetActive( TRUE );
	m_pCurTarget	=	NULL;

	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Monster :
	case eObjectKind_SpecialMonster :
	case eObjectKind_FieldSubMonster :
	case eObjectKind_ToghterPlayMonster :
	case eObjectKind_BossMonster :
	case eObjectKind_FieldBossMonster :
		{
			CMonster* pMonster = (CMonster*)pObject;
			switch( pMonster->GetMonsterTargetType() )
			{
			case MONSTER_TYPE:
				{
					m_Type	=	eObjectKind_Monster;
					SetMonsterTargetType( MONSTER_TYPE );
				}
				break;
			case BOSSMONSTER_TYPE:
				{
					m_Type	=	eObjectKind_BossMonster;
					SetMonsterTargetType( BOSSMONSTER_TYPE );
				}
				break;
			}
			SetTargetNameInfo( pObject );
		}
		break;

	case eObjectKind_Player :
		{
			m_Type	=	eObjectKind_Player;

			SetMonsterTargetType( PC_TYPE );
			SetTargetNameInfo( pObject );

			m_pCharacterTarget->SelectTargetGender( (CPlayer*)pObject );
		}
		break ;

	case eObjectKind_Npc :
		{ 
			m_pNPCName->SetActive( TRUE ) ;
			m_pNPCName->SetStaticText( name ) ;
			// 070612 LYW --- MonsterGuageDlg : Check showjob.
			if( ((CNpc*)pObject)->GetSInfo()->ShowJob )
			{
				m_pNPCOccupation->SetActive( TRUE ) ;
				m_pNPCOccupation->SetStaticText(((CNpc*)pObject)->GetSInfo()->Name) ;
			}
		}
		break ;

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.05.08
	case eObjectKind_FarmObj:
		{
			m_pNPCName->SetActive( TRUE ) ;
			m_pNPCName->SetStaticText( name ) ;
			if( g_csFarmManager.CLI_GetFarmRenderObjList(((CSHFarmRenderObj*)pObject)->GetInfo()->nKind)->bShowGroupName )
			{
				m_pNPCOccupation->SetActive( TRUE ) ;
				m_pNPCOccupation->SetStaticText(g_csFarmManager.CLI_GetFarmRenderObjList(((CSHFarmRenderObj*)pObject)->GetInfo()->nKind)->szGroupName);
			}
		}
		break ;
	// E 농장시스템 추가 added by hseos 2007.05.08
	case eObjectKind_Pet:
		{
			m_pMasterName->SetActive( TRUE ) ;	
			m_pPetName->SetActive( TRUE ) ;

			CPet* pPet = (CPet*)pObject;
			m_pMasterName->SetStaticText( pPet->GetMasterName() );
			m_pPetName->SetStaticText( name ) ;
		}	
		break;

	// 090422 ShinJS --- 탈것추가
	case eObjectKind_Vehicle:
		{
			m_pNPCName->SetActive( TRUE ) ;
			m_pNPCName->SetStaticText( name );
		}
		break;
	//090525 pdy 하우징
	case eObjectKind_Furniture:
		{
			m_pNPCName->SetActive( TRUE ) ;
			m_pNPCName->SetStaticText( name ) ;
		}	
		break;
	}

}


//=================================================================================================
//	NAME		: SetGuildName
//	PURPOSE		: Setting guild name and union name of guild.
//	DATE		: January 23 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CMonsterGuageDlg::SetGuildName( char* guildName, char* unionName )
{
	if( m_TargetDlgType != PC_TYPE )
		return;

	char tempGuildName[128]	=	{ 0, };
	sprintf( tempGuildName, "%s%s", CHATMGR->GetChatMsg(0), guildName );

	m_pCharacterTarget->SetTargetGuildInfo( guildName );
}

void CMonsterGuageDlg::SetLife( CPlayer* pPlayer )
{
	char szValue[50] = {0};

	CHARACTER_TOTALINFO* pInfo = pPlayer->GetCharacterTotalInfo() ;

	if( pInfo->Life > pInfo->MaxLife )
	{
		pInfo->Life = pInfo->MaxLife ;
	}
	
	sprintf( szValue, "%d/%d", pInfo->Life, pInfo->MaxLife );

	if( m_playerLife )
	{
		m_playerLife->SetStaticText(szValue);
		m_playerLife->SetActive( TRUE ) ;
	}
}


//=================================================================================================
//	NAME		: SetGuage
//	PURPOSE		: Setting guage of target.
//	DATE		: January 23 2007 LYW
//	ATTENTION	:
//=================================================================================================
void CMonsterGuageDlg::SetGuage( CObject* pObject )
{
	m_pHPMark->SetActive( FALSE ) ;
	m_pPlayerGuage->SetActive( FALSE ) ;
	m_pMonsterGuage->SetActive( FALSE ) ;

	switch(pObject->GetObjectKind())
	{
	case eObjectKind_Monster :	
	case eObjectKind_SpecialMonster :
	case eObjectKind_FieldSubMonster :
	case eObjectKind_ToghterPlayMonster :
	case eObjectKind_BossMonster :
	case eObjectKind_FieldBossMonster :
		{
			CMonster*	pMonster	=	(CMonster*)pObject;
			switch( pMonster->GetMonsterTargetType() )
			{
			case	MONSTER_TYPE:
				{
					m_Type	=	eObjectKind_Monster;
				}
				break;
			case	BOSSMONSTER_TYPE:
				{
					m_Type	=	eObjectKind_BossMonster;
				}
				break;
			}
			SetTargetGuageInfo( pObject );
		}	
		break;
	case eObjectKind_Player :
		{
			m_Type	=	eObjectKind_Player;
			SetTargetGuageInfo( pObject );
		}
		break ;

	case eObjectKind_Npc :
		{			
			m_pHPMark->SetActive( FALSE ) ;
		}
		break ;

	}
}

void CMonsterGuageDlg::ResetDlg()
{
	mAlphaTick = 0;
	SetAlpha(255);	

	m_pGuildName->SetActive(
		TRUE);
}

void CMonsterGuageDlg::StartFadeOut()
{
	mAlphaTick = gCurTime + 5000;
	if( m_pCurTarget )
		m_pCurTarget->StartFadeOut();
}

void CMonsterGuageDlg::InsertSkillIcon( DWORD ID, DWORD Index, DWORD time, int count )
{
	if( m_pCurTarget )
		m_pCurTarget->InsertMonsterSkillIcon( ID, Index, time, count );
}

void CMonsterGuageDlg::RemoveSkillIcon( DWORD ID, DWORD Index )
{	
	if( m_pCurTarget )
		m_pCurTarget->RemoveMonsterSkillIcon( ID, Index );	
}

void CMonsterGuageDlg::RemoveSkill( DWORD Index )
{
	if( m_pCurTarget )
		m_pCurTarget->RemoveSkill( Index );
}

void CMonsterGuageDlg::RemoveAllSkillIcon()
{
	if( m_pCurTarget )
		m_pCurTarget->RemoveMonsterAllSkillIcon();
}

void CMonsterGuageDlg::SetTargetVersusInfo( DWORD dwTargetVSID )
{
	if( m_pCurTarget )
	{
		m_pCurTarget->SetVersusInfo( dwTargetVSID );	
		m_dwTargetVersusID	=	dwTargetVSID;
	}
}

void CMonsterGuageDlg::SetTargetNameInfo( CObject* pObject )
{
	m_pCurTarget->SetTargetNameInfo( pObject );
}

void CMonsterGuageDlg::SetTargetGuageInfo( CObject* pObject )
{
	if( m_pCurTarget )
	{
		pObject->UpdateTargetBuffSkill();
		m_pCurTarget->SetTargetGuageInfo( pObject );
	}
}

void CMonsterGuageDlg::SetMonsterTargetType( TargetDlgType eTarget )
{
 	switch( eTarget )
	{
	case MONSTER_TYPE:
		{
			m_TargetDlgType	=	MONSTER_TYPE;
			m_pCurTarget	=	m_pNormalMonster;
			m_pCurTarget->SetActive( true );
			m_pCharacterMain->SetActive( FALSE );
		}
		break;
	case BOSSMONSTER_TYPE:
		{
			m_TargetDlgType	=	BOSSMONSTER_TYPE;
			m_pCurTarget	=	m_pBossMonster;
			m_pCurTarget->SetActive( true );
			m_pCharacterMain->SetActive( FALSE );
		}
		break;
	case PC_TYPE:
		{
			m_TargetDlgType =	PC_TYPE;
			m_pCurTarget	=	m_pCharacterTarget;
			m_pCurTarget->SetActive( true );
			m_pCharacterMain->SetActive( FALSE );
		}
		break;
	case NPC_TYPE:
		{
			m_TargetDlgType =	NPC_TYPE;
		}

	}

}

void CMonsterGuageDlg::LoadMonsterBuffIconPosition()
{
	char fileName[128]	=	{ 0, };
	char tempString[MAX_PATH]	=	{ 0, };

	char	strId[128]	=	{ 0, };
	LONG	Id	=	0;
	DWORD	dwIconSize	=	0;
	POINT	position;
	position.x	=	0;
	position.y	=	0;
	DWORD	dwSkillIconCount	= 0;
	DWORD	dwMaxIconPerLine	=	0;

	CMHFile file;
	sprintf(fileName,"Data/Interface/Windows/TargetBuffIconPosition.bin");

	BOOL bReault = file.Init( fileName, "rb" );

	if( !bReault )
	{
		MessageBox( NULL, "Loading failed!! Data/Interface/Windows/TargetBuffIconPosition.bin", 0, 0);
	}

	if( file.IsEOF() )
		return;

	while( !file.IsEOF() )
	{
		strcpy( tempString, file.GetString() );
		if( strcmp( tempString, "ID" ) == 0 )
		{
			file.GetString( strId );
			Id	=	IDSEARCH( strId );
		}
		else if( strcmp( tempString, "SKILLICON_SIZE" ) == 0 )
		{
			dwIconSize	=	file.GetDword();
		}
		else if( strcmp( tempString, "POSITION_X" ) == 0 )
		{	
			position.x	=	file.GetLong();
		}
		else if( strcmp( tempString, "POSITION_Y" ) == 0 )
		{
			position.y	=	file.GetLong();
		}
		else if( strcmp( tempString, "SKILLICON_COUNT" ) == 0 )
		{
			dwSkillIconCount	=	file.GetDword();
		}
		else if( strcmp( tempString, "MAXICONPERLINE" ) == 0 )
		{
			dwMaxIconPerLine	=	file.GetDword();

			if( strcmp( strId, "CG_CHARACTERGUAGE" ) == 0 )
			{
				m_pCharacterTarget->LinkMonsterBuffIcon( (float)dwIconSize, position, dwSkillIconCount, dwMaxIconPerLine );
			}
			else if( strcmp( strId, "CG_BOSSGUAGEDLG" ) == 0 )
			{	
				m_pBossMonster->LinkMonsterBuffIcon( (float)dwIconSize, position, dwSkillIconCount, dwMaxIconPerLine );
			}
			else if( strcmp( strId, "CG_MONSTERGUAGEDLG" ) == 0 )
			{
				m_pNormalMonster->LinkMonsterBuffIcon( (float)dwIconSize, position, dwSkillIconCount, dwMaxIconPerLine );
			}
		}
	}
	file.Release();
}

void CMonsterGuageDlg::TargetVersusDie()
{
	if( m_pCurTarget )
		m_pCurTarget->TargetVSDie();
}
