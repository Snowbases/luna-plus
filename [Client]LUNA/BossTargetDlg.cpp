#include "stdafx.h"
#include "BossTargetDlg.h"
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

cBossTargetDlg::cBossTargetDlg() 
: cMonsterTargetDlg()
{
	m_pBossEmergency_Left		=	NULL;
	m_pBossEmergency_Middle	=	NULL;
	m_pBossEmergency_Right	=	NULL;

	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pBossGuage[i] = NULL;
	}
	m_pUnbeatableGuage	=	NULL;
	m_bIsUnbeatable		=	FALSE;
	m_bIsEmergency		=	FALSE;

	m_bIsRemainLifePoint	=	FALSE;
	m_dwRemainLifePoint		=	0;
	m_dwRemainTimeStart		=	0;
}

cBossTargetDlg::~cBossTargetDlg()
{

}

void cBossTargetDlg::Render()
{
	cMonsterTargetDlg::Render();

	if( m_bIsEmergency )
	{
		DWORD	dwCurTick		=	GetTickCount();

		DWORD	dwResultTime	=	dwCurTick - m_dwEmergencyTick;
		if( dwResultTime > 1000 )
		{
			m_dwEmergencyTick = GetTickCount();
		}
		m_pBossGuage[LP_ONE]->SetAlpha( (BYTE)(3.5f * (dwResultTime/10) ) ); 
		
		m_pBossEmergency_Left->SetAlpha( (BYTE)(3.5f * (dwResultTime/10) ) );
		m_pBossEmergency_Middle->SetAlpha( (BYTE)(3.5f * (dwResultTime/10) ) );
		m_pBossEmergency_Right->SetAlpha( (BYTE)(3.5f * (dwResultTime/10) ) );
	}
	else
	{
		m_pBossGuage[LP_ONE]->SetAlpha( 255 );
	}

	RenderRemainLifePoint();
}

void cBossTargetDlg::SetMonsterUIActive( bool val )
{
	cMonsterTargetDlg::SetMonsterUIActive( val );
	
	if( m_eTargetType == BOSS )
	{
		for( int i = 0; i < MAXLIFEPOINT; ++i )
		{
			m_pBossGuage[i]->SetActive( val );
		}
	}

	m_pBossEmptyGuage_Left->SetActive( val );
	m_pBossEmptyGuage_Middle->SetActive( val );
	m_pBossEmptyGuage_Right->SetActive( val );

	if( val == FALSE )
	{
		m_pBossEmergency_Left->SetActive( val );
		m_pBossEmergency_Middle->SetActive( val );
		m_pBossEmergency_Right->SetActive( val );
		m_pUnbeatableGuage->SetActive( val );
	}

	m_bIsUnbeatable	=	FALSE;
}

void cBossTargetDlg::SetTargetGuageInfo( CObject* pObject )
{
	CMonster* pMonster	=	(CMonster*)pObject;

	if( !pMonster )
		return;

	m_pBossEmergency_Left->SetActive( FALSE );
	m_pBossEmergency_Middle->SetActive( FALSE );
	m_pBossEmergency_Right->SetActive( FALSE );

	m_dwLifePoint	=	0;
	
	float	fGuage	=	(float)pMonster->GetLife()/(float)((CMonster*)pMonster)->GetSInfo()->Life;

	m_bIsRemainLifePoint	=	FALSE;

	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pBossGuage[i]->SetValue( 1.0f, 0 );
	}
	SetMonsterLife( fGuage );
	UpdateLifePoint();
}

void cBossTargetDlg::SetLifePointInfo( float fHP )
{
	cMonsterTargetDlg::SetLifePointInfo( fHP );
}

void cBossTargetDlg::UpdateLifePoint()
{
	cMonsterTargetDlg::UpdateLifePoint();
	if( m_bIsRemainLifePoint )
		m_pLifePoint[m_dwRemainLifePoint]->SetActive( TRUE );
}

void cBossTargetDlg::SetMonsterLife( float fMonsterLife )
{
	SetLifePointInfo( fMonsterLife );

	SetBossMonsterLifeGuage();
}

void cBossTargetDlg::TargetDlgLink()
{
	m_pName		=	(cStatic*)GetWindowForID(CG_BOSSGUAGENAME);
	m_pLevel	=	(cStatic*)GetWindowForID(CG_BOSSGUAGELEVEL);
	m_pGuage	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_5);

	m_pBossGuage[LP_ONE]	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_1);
	m_pBossGuage[LP_TWO]	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_2);
	m_pBossGuage[LP_THREE]	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_3);
	m_pBossGuage[LP_FOUR]	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_4);
	m_pBossGuage[LP_FIVE]	=	(CObjectGuagen*)GetWindowForID(CG_BOSSGUAGELIFE_5);
	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pBossGuage[i]->SetActive(true);
	}

	m_pFace		=	(cStatic*)GetWindowForID(CG_BOSSRACE);

	m_pGuageMain_Left	=	(cStatic*)GetWindowForID(CG_BOSSGUAGE_MAIN_LEFT);
	m_pGuageMain_Middle	=	(cStatic*)GetWindowForID(CG_BOSSGUAGE_MAIN_MIDDLE);
	m_pGuageMain_Right	=	(cStatic*)GetWindowForID(CG_BOSSGUAGE_MAIN_RIGHT);

	// Empty Guage
	m_pBossEmptyGuage_Left		=	(cStatic*)GetWindowForID(CG_BOSS_EMPTY_GUAGE_LEFT);
	m_pBossEmptyGuage_Middle	=	(cStatic*)GetWindowForID(CG_BOSS_EMPTY_GUAGE_MIDDLE);
	m_pBossEmptyGuage_Right		=	(cStatic*)GetWindowForID(CG_BOSS_EMPTY_GUAGE_RIGHT);

	// 보스 이펙트
	m_pBossEmergency_Left		=	(cStatic*)GetWindowForID(CG_BOSS_EFFACT_LEFT);
	m_pBossEmergency_Middle		=	(cStatic*)GetWindowForID(CG_BOSS_EFFACT_MIDDLE);
	m_pBossEmergency_Right		=	(cStatic*)GetWindowForID(CG_BOSS_EFFACT_RIGHT);

	m_pBossEmergency_Left->SetActive( FALSE );
	m_pBossEmergency_Middle->SetActive( FALSE );
	m_pBossEmergency_Right->SetActive( FALSE );



	//보스 버프, 시선 체크박스
	m_pBuffCheckBox		=	(cCheckBox*)GetWindowForID(CG_BOSS_BUFFICON_CHECKBOX);
	m_pSightCheckBox	=	(cCheckBox*)GetWindowForID(CG_BOSS_SIGHTICON_CHECKBOX);

	m_pVSMain_Left		=	(cStatic*)GetWindowForID(CG_BOSSVERSUS_MAIN_LEFT);
	m_pVSMain_Right		=	(cStatic*)GetWindowForID(CG_BOSSVERSUS_MAIN_RIGHT);
	m_pVS				=	(cStatic*)GetWindowForID(CG_BOSSVERSUS_IMAGE);
	m_pVSGuage			=	(CObjectGuagen*)GetWindowForID(CG_BOSSVERSUS_LIFEGUAGE);
	m_pVSName			=	(cStatic*)GetWindowForID(CG_BOSSVERSUS_NAME);

	//몬스터 LifePoint]
	m_pLifePoint_Empty	=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_EMPTY);

	m_pLifePoint[0]		=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_1);
	m_pLifePoint[1]		=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_2);
	m_pLifePoint[2]		=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_3);
	m_pLifePoint[3]		=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_4);
	m_pLifePoint[4]		=	(cStatic*)GetWindowForID(CG_BOSS_LIFEPOINT_5);

	m_pUnbeatableGuage	=	(CObjectGuagen*)GetWindowForID(CG_BOSS_UNBEATABLE_GUAGE);
	m_pUnbeatableGuage->SetActive( FALSE );

	//몬스터 속성
	m_pSpecies			=	(cStatic*)GetWindowForID(CG_BOSS_SPECIES);
	m_eTargetType		=	BOSS;
}

void cBossTargetDlg::SetBossMonsterLifeGuage()
{
	CMonster* pMonster	=	(CMonster*)m_pCurTargetObject;
	m_bIsEmergency	=	FALSE;

	int iCurGuage	=	m_dwLifePoint - 1;
	//보스 게이지 마지막
	if( iCurGuage < 0 )
		iCurGuage = 0;

	for( int i = 0; i < MAXLIFEPOINT; ++i )
	{
		m_pBossGuage[i]->SetActive( false );
		m_pBossGuage[i]->SetValue(1.0f, 0);
	}
	m_pUnbeatableGuage->SetActive( FALSE );
	m_pUnbeatableGuage->SetValue(1.0f, 0);

	m_pBossGuage[0]->SetActive( true );
	
	float	fDivision	=	(float)(pMonster->GetSInfo()->Life) / MAXLIFEPOINT;
	
	float	fDivisionLife	=	(float)pMonster->GetLife() - (m_dwLifePoint-1)*fDivision;

	float	fDivisionGuage	=	fDivisionLife / fDivision;

	if( iCurGuage > 0 )
	{
		m_pBossGuage[iCurGuage-1]->SetActive(true);
	}

	//무적이 아닐경우
	if( !m_bIsUnbeatable )
	{
		m_pBossGuage[iCurGuage]->SetActive(true);
		m_pBossGuage[iCurGuage]->SetValue(fDivisionGuage, 0);
	}
	//무적일경우
	else
	{
		m_pUnbeatableGuage->SetActive( TRUE );
		m_pUnbeatableGuage->SetValue( fDivisionGuage, 0 );
	}

	if( m_dwLifePoint == 1 &&  fDivisionGuage <= 0.5f )
	{
		MonsterEmergency();
	}
}

void cBossTargetDlg::GodBuffCheck( DWORD dwSkillIdx, BOOL IsRemoveSkill )
{	
	cBuffSkillInfo*		skillInfo	= ( cBuffSkillInfo* )( SKILLMGR->GetSkillInfo( dwSkillIdx ) );
	BUFF_SKILL_INFO*	pBuffSkillInfo	=	NULL;

	if( skillInfo )
		pBuffSkillInfo	=	skillInfo->GetSkillInfo();

	if( pBuffSkillInfo )
	{
		// 버프스킬 무적
		if( pBuffSkillInfo->Status == eStatusKind_God  )
		{
			if( !IsRemoveSkill ) 
				m_bIsUnbeatable	=	TRUE;	
			else
				m_bIsUnbeatable	=	FALSE;
		}
	}
	SetBossMonsterLifeGuage();
}

void cBossTargetDlg::GodGuageSwitch( BOOL bSwitch )
{
	m_bIsUnbeatable	=	bSwitch;
}

void cBossTargetDlg::InsertMonsterSkillIcon( DWORD ID, DWORD Index, DWORD time, int count )
{
	cMonsterTargetDlg::InsertMonsterSkillIcon( ID, Index, time, count );
	GodBuffCheck( Index );
}

void cBossTargetDlg::RemoveMonsterSkillIcon( DWORD ID, DWORD Index )
{
	cMonsterTargetDlg::RemoveMonsterSkillIcon( ID, Index );	
}

void cBossTargetDlg::RemoveSkill( DWORD Index )
{
	GodBuffCheck( Index, true );
}

void cBossTargetDlg::MonsterEmergency()
{
	m_bIsEmergency	=	TRUE;

	m_dwEmergencyTick	=	GetTickCount();

	m_pBossGuage[LP_ONE]->SetAlpha( 255 );
	m_dwEmergencyTick	=	gCurTime;

	m_pBossEmergency_Left->SetActive( TRUE );
	m_pBossEmergency_Middle->SetActive( TRUE );
	m_pBossEmergency_Right->SetActive( TRUE );

	m_pBossEmergency_Left->SetAlpha( 255 );
	m_pBossEmergency_Middle->SetAlpha( 255 );
	m_pBossEmergency_Right->SetAlpha( 255 );
}	

void cBossTargetDlg::SetMonsterTargetAlpha( BYTE al )
{
	cMonsterTargetDlg::SetMonsterTargetAlpha( al );
	
	m_pBossEmptyGuage_Left->SetAlpha( al );
	m_pBossEmptyGuage_Middle->SetAlpha( al );
	m_pBossEmptyGuage_Right->SetAlpha( al );

	m_pBossEmergency_Left->SetAlpha( al );
	m_pBossEmergency_Middle->SetAlpha( al );
	m_pBossEmergency_Right->SetAlpha( al );
}

void cBossTargetDlg::TargetDie()
{
	m_pBossEmergency_Left->SetActive( FALSE );
	m_pBossEmergency_Middle->SetActive( FALSE );
	m_pBossEmergency_Right->SetActive( FALSE );
}

void cBossTargetDlg::RemainLifePointStart()
{
	m_dwRemainTimeStart		=	GetTickCount();
	m_dwRemainLifePoint		=	m_dwLifePoint;
	m_bIsRemainLifePoint	=	TRUE;
}

void cBossTargetDlg::RenderRemainLifePoint()
{
	if( m_bIsRemainLifePoint )
	{
		DWORD	dwCurTick		=	GetTickCount();
		DWORD	dwResultTime	=	dwCurTick - m_dwRemainTimeStart;
		if( dwResultTime > 2000 )
		{
			m_bIsRemainLifePoint = FALSE;
			m_pLifePoint[m_dwRemainLifePoint]->SetActive( FALSE );
		}
		m_pLifePoint[m_dwRemainLifePoint]->SetAlpha((BYTE)(3.5f * (dwResultTime/10) ) );
	}
}

void cBossTargetDlg::ChangeLifePoint()
{
	if( m_dwLifePoint < MAXLIFEPOINT )
		RemainLifePointStart();
	else
		m_bIsRemainLifePoint	=	FALSE;
}
