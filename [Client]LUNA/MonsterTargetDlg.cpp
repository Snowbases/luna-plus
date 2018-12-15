#include "stdafx.h"
#include "MonsterTargetDlg.h"
#include "ObjectGuagen.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cWindowManager.h"
#include "MonsterGuageDlg.h"
#include "cCheckBox.h"
#include "Monster.h"
#include "GameResourceManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "StatusIconDlg.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[CC]Skill/Client/Info/BuffSkillInfo.h"
#include "BattleGuageDlg.h"
#include "GameIn.h"

cMonsterTargetDlg::cMonsterTargetDlg()
{
	m_pName				=	NULL;
	m_pLevel			=	NULL;
	m_pGuage			=	NULL;
	m_pGuageMain_Left	=	NULL;
	m_pGuageMain_Middle	=	NULL;
	m_pGuageMain_Right	=	NULL;
	m_pBuffCheckBox		=	NULL;
	m_pSightCheckBox	=	NULL;
	m_bVisibleBuff		=	NULL;
	m_bVisibleSight		=	NULL;
	m_dwLifePoint		=	NULL;
	m_dwAlphaStartTime	=	NULL;

	m_pLifePoint_Empty	=	NULL;
	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]	=	NULL;
	}
	m_pSpecies			=	NULL;
	m_pFace				=	NULL;

	m_pVSMain_Left		=	NULL;
	m_pVSMain_Right		=	NULL;
	m_pVS				=	NULL;
	m_pVSGuage			=	NULL;
	m_pVSName			=	NULL;
	mAlphaTick			=	0;
	m_pCurTargetObject	=	NULL;

	m_bVisibleBuff		=	TRUE;
	m_bVisibleSight		=	TRUE;

	m_pMonsterBuffIconDlg	=	NULL;

	m_dwLvColor			=	0;
}

cMonsterTargetDlg::~cMonsterTargetDlg()
{
	SAFE_DELETE( m_pMonsterBuffIconDlg );
}

void cMonsterTargetDlg::Render()
{

	if(mAlphaTick > gCurTime)
		SetMonsterTargetAlpha( BYTE((mAlphaTick - gCurTime) / 20 ) + 5 );

	if( m_bVisibleBuff )
		m_pMonsterBuffIconDlg->Render();

	if( !m_bVisibleSight )
		SetMonsterVSUIActive( false );


	if( m_dwLifePoint == 1 )
	{
		DWORD	dwCurTick		=	GetTickCount();
		DWORD	dwResultTime	=	dwCurTick - m_dwAlphaStartTime;
		if( dwResultTime > 1000 )
		{
			m_dwAlphaStartTime = GetTickCount();
		}
		m_pLifePoint[LP_ONE]->SetAlpha((BYTE)(3.5f * (dwResultTime/10) ) );
	}
	else
	{
		m_pLifePoint[LP_ONE]->SetAlpha( 255 );
	}
	cDialog::Render();
}

void cMonsterTargetDlg::SetAbsXY( LONG x, LONG y )
{
	if( m_pMonsterBuffIconDlg )
	{
		VECTOR2 position = {
			(float)x + m_BuffIconStartPoint.x,
			(float)y + m_BuffIconStartPoint.y };
		m_pMonsterBuffIconDlg->SetPosition( position );
	}

	cDialog::SetAbsXY( x, y );
}

void cMonsterTargetDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case CG_MONSTER_BUFFICON_CHECKBOX:
	case CG_BOSS_BUFFICON_CHECKBOX:
	case CG_PLAYER_BUFFICON_CHECKBOX:
		{
			if( m_pBuffCheckBox->IsChecked() )
				m_bVisibleBuff	=	TRUE;
			else
				m_bVisibleBuff	=	FALSE;
		}
		break;

	case CG_MONSTER_SIGHTICON_CHECKBOX:
	case CG_BOSS_SIGHTICON_CHECKBOX:
	case CG_PLAYER_SIGHTICON_CHECKBOX:
		{
			if( m_pSightCheckBox->IsChecked() )
			{
				m_bVisibleSight	=	TRUE;
			}
			else
			{
				m_bVisibleSight	=	FALSE;
			}
		}
		break;
	}
}

void cMonsterTargetDlg::SetMonsterUIActive( bool val )	
{
	m_pName->SetActive( val );
	m_pLevel->SetActive( val );
	m_pGuage->SetActive( val );
	m_pGuageMain_Left->SetActive( val );
	m_pGuageMain_Middle->SetActive( val );
	m_pGuageMain_Right->SetActive( val );
	m_pBuffCheckBox->SetActive( val );
	m_pSightCheckBox->SetActive( val );

	m_pLifePoint_Empty->SetActive( val );
	m_pSpecies->SetActive( val );
	m_pFace->SetActive( val );
	m_pMonsterBuffIconDlg->RemoveAllSkillIcon();

	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]->SetActive( val );
	}
}

void cMonsterTargetDlg::SetMonsterVSUIActive( bool val )
{
	m_pVSMain_Left->SetActive( val );
	m_pVSMain_Right->SetActive( val );
	m_pVS->SetActive( val );
	m_pVSGuage->SetActive( val );
	m_pVSName->SetActive( val );
}


void cMonsterTargetDlg::SetTargetGuageInfo( CObject* pObject )
{
	CMonster* pMonster	=	(CMonster*)pObject;

	m_dwLifePoint	=	0;

	float	fGuage	=	(float)pMonster->GetLife()/(float)((CMonster*)pMonster)->GetSInfo()->Life;
	SetMonsterLife( fGuage );
	UpdateLifePoint();
}

void cMonsterTargetDlg::SetTargetNameInfo( CObject* pObject )
{
	char targetName[128]	=	{ 0, };
	char targetLevel[128]	=	{ 0, };

	CPlayer*	pPlayer		=	NULL;
	CMonster*	pMonster	=	NULL;

	if( m_eTargetType == CHARACTER )
	{
		m_pCurTargetObject	=	pObject;
		pPlayer	=	(CPlayer*)pObject;
		m_dwLvColor	=	GAMEIN->GetBattleGuageDlg()->SetTargetLevelSignal( pPlayer->GetLevel() );

		sprintf( targetName, "%s",pPlayer->GetObjectName() );
		sprintf( targetLevel, "Lv.%d", pPlayer->GetLevel() );

	}
	else
	{
		m_pCurTargetObject	=	pObject;
		pMonster		=	(CMonster*)pObject;
		m_dwLvColor	=	GAMEIN->GetBattleGuageDlg()->SetTargetLevelSignal( pMonster->GetSInfo()->Level );

		sprintf( targetName, "%s",pMonster->GetSInfo()->Name );
		sprintf( targetLevel, "Lv.%d", pMonster->GetSInfo()->Level );
	}

	SetMonsterUIActive( true );

	m_pName->SetFGColor( RGBA_MAKE( 255, 255, 255, 255 ) );
	m_pName->SetStaticText( targetName );
	m_pName->SetTextAlpha( true );

	m_pLevel->SetFGColor( m_dwLvColor );
	m_pLevel->SetStaticText( targetLevel );
	m_pLevel->SetTextAlpha( true );

	mAlphaTick = gCurTime;
	SetMonsterTargetAlpha( 255 );
}

void cMonsterTargetDlg::SetVersusInfo( DWORD dwTargetVSID )
{
	if( m_pCurTargetObject->GetID() == dwTargetVSID ) 
		return;

	CObject* pVS	=	OBJECTMGR->GetObject( dwTargetVSID );

	if( pVS && !pVS->IsDied() )
	{
		SetMonsterVSUIActive( true );

		m_pVSName->SetStaticText( pVS->GetObjectName() );
		m_pVSName->SetTextAlpha( true );

		m_dwTargetVSID	=	dwTargetVSID;
	}
	else
		SetMonsterVSUIActive( false );
}

void cMonsterTargetDlg::SetLifePointInfo( float fHP )
{
	DWORD	dwCurrentLifePoint	=	0;
	DWORD	dwPreLifePoint	=	m_dwLifePoint;

	if( fHP == 0.0f )
	{
		dwCurrentLifePoint	=	0;
		TargetDie();
	}
	else if( fHP <= LIFEPOINT_ONE )
	{
		dwCurrentLifePoint	=	1;
	}
	else if( fHP <= LIFEPOINT_TWO )
	{
		dwCurrentLifePoint	=	2;
	}
	else if( fHP <= LIFEPOINT_THREE )
	{
		dwCurrentLifePoint	=	3;
	}
	else if( fHP <= LIFEPOINT_FORE )
	{ 
		dwCurrentLifePoint	=	4;
	}
	else if( fHP <= LIFEPOINT_FIVE )
	{
		dwCurrentLifePoint	=	5;
	}

	if( dwCurrentLifePoint != m_dwLifePoint )
	{
		m_dwLifePoint	=	dwCurrentLifePoint;

		if( dwPreLifePoint != 0 )
			ChangeLifePoint();

		UpdateLifePoint();
		if( dwCurrentLifePoint == 1 )
		{
			m_dwAlphaStartTime	=	GetTickCount() - 2000;
			m_pLifePoint[LP_ONE]->SetAlpha( 255 );
		}
	}
}

void cMonsterTargetDlg::UpdateLifePoint()
{
	for( int i = 0; i < (int)m_dwLifePoint; ++i )
	{
		m_pLifePoint[i]->SetActive( true );
		m_pLifePoint[i]->SetAlpha( 255 );
	}

	for( int i = (int)m_dwLifePoint; i < MAXLIFEPOINT; ++i )
	{
		m_pLifePoint[i]->SetActive( false );
	}
}

void cMonsterTargetDlg::SetMonsterLife( float fMonsterLife )
{
	float	fGuage	=	fMonsterLife;

	SetLifePointInfo( fMonsterLife );

	m_pGuage->SetValue( fGuage, 0 );
}

void cMonsterTargetDlg::TargetDlgLink()
{
	m_pName		=	(cStatic*)GetWindowForID(CG_MONSTERGUAGENAME);
	m_pLevel	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGELEVEL);
	m_pGuage	=	(CObjectGuagen*)GetWindowForID(CG_MONSTERGUAGE_HP);
	m_pFace		=	(cStatic*)GetWindowForID(CG_MONSTERRACE);

	m_pGuageMain_Left	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_LEFT);
	m_pGuageMain_Middle	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_MIDDLE);
	m_pGuageMain_Right	=	(cStatic*)GetWindowForID(CG_MONSTERGUAGE_MAIN_RIGHT);

	//몬스터 버프, 시선 체크박스
	m_pBuffCheckBox		=	(cCheckBox*)GetWindowForID(CG_MONSTER_BUFFICON_CHECKBOX);
	m_pSightCheckBox	=	(cCheckBox*)GetWindowForID(CG_MONSTER_SIGHTICON_CHECKBOX);

	//일반몬스터 타겟

	m_pVSMain_Left		=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_MAIN_LEFT);
	m_pVSMain_Right		=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_MAIN_RIGHT);
	m_pVS				=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_IMAGE);
	m_pVSGuage			=	(CObjectGuagen*)GetWindowForID(CG_MONSTERVERSUS_LIFEGUAGE);
	m_pVSName			=	(cStatic*)GetWindowForID(CG_MONSTERVERSUS_NAME);

	//몬스터 LifePoint
	m_pLifePoint_Empty	=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_EMPTY);

	m_pLifePoint[0]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_1);
	m_pLifePoint[1]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_2);
	m_pLifePoint[2]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_3);
	m_pLifePoint[3]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_4);
	m_pLifePoint[4]		=	(cStatic*)GetWindowForID(CG_MONSTER_LIFEPOINT_5);

	m_pSpecies			=	(cStatic*)GetWindowForID(CG_MONSTER_SPECIES);
	m_eTargetType		=	NORMAL;
}

void cMonsterTargetDlg::SetTargetVSGuage( float fVsLife )
{
	m_pVSGuage->SetValue( fVsLife, 0 );
}

void cMonsterTargetDlg::LinkMonsterBuffIcon( float fIconSize, POINT IconPos, DWORD dwSkillIconCount, DWORD dwMaxIconPerLine )
{
	m_pMonsterBuffIconDlg	=	new CStatusIconDlg;

	m_BuffIconStartPoint	=	IconPos;

	VECTOR2 position = {
		(float)IconPos.x,
		(float)IconPos.y };
	m_pMonsterBuffIconDlg->SetPosition( position );
	m_pMonsterBuffIconDlg->SetIconSize( fIconSize, fIconSize );
	m_pMonsterBuffIconDlg->SetMaxIconQuantity( dwSkillIconCount );
	m_pMonsterBuffIconDlg->Init( HERO, &position, dwMaxIconPerLine );
}

void cMonsterTargetDlg::InsertMonsterSkillIcon( DWORD ID, DWORD Index, DWORD time, int count )
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->AddSkillIcon( ID, Index, time, count );
}

void cMonsterTargetDlg::RemoveMonsterSkillIcon( DWORD ID, DWORD Index )
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->RemoveSkillIcon( ID, Index );
}

void cMonsterTargetDlg::RemoveMonsterAllSkillIcon()
{
	if( m_pMonsterBuffIconDlg )
		m_pMonsterBuffIconDlg->RemoveAllSkillIcon();
}
		
void cMonsterTargetDlg::StartFadeOut()
{
	mAlphaTick = gCurTime + 5000;
}

void cMonsterTargetDlg::SetMonsterTargetAlpha( BYTE al )
{
	SetAlpha( al );
	m_pGuageMain_Left->SetAlpha( al );
	m_pGuageMain_Middle->SetAlpha( al );
	m_pGuageMain_Right->SetAlpha( al );
	m_pBuffCheckBox->SetAlpha( al );
	m_pSightCheckBox->SetAlpha( al );
	m_pLifePoint_Empty->SetAlpha( al );
	m_pVSMain_Left->SetAlpha( al );
	m_pVSMain_Right->SetAlpha( al );
	m_pVS->SetAlpha( al );

	m_pName->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
	m_pLevel->SetFGColor( RGBA_MERGE( m_dwLvColor, al ) );
	m_pVSName->SetFGColor( RGBA_MAKE( 255, 255, 255, al ) );
}

void cMonsterTargetDlg::TargetVSDie()
{
	SetMonsterVSUIActive( false );
}

void cMonsterTargetDlg::SetMonsterSpecies( DWORD dwSpecies )
{

}

void cMonsterTargetDlg::SetMonsterAttribute( DWORD dwAttribute )
{

}