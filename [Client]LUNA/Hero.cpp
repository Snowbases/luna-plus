#include "stdafx.h"
#include "Hero.h"
#include "GameIn.h"
#include "MHCamera.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "ItemManager.h"
#include "CharacterCalcManager.h"
#include "PartyManager.h"
#include "cSkillTreeManager.h"
#include "ObjectGuagen.h"
#include "./Audio/MHAudioManager.h"
#include "StreetStallManager.h"
#include "CheatMsgParser.h"
#include "interface/cWindowManager.h"
#include "ChatManager.h"
#include "WindowIdEnum.h"
#include "ReviveDialog.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "DealDialog.h"
#include "ProgressDialog.h"
#include "mhMap.h"
#include "BattleSystem_Client.h"
#include "NpcScriptDialog.h"
#include "CommonCalcFunc.h"
#include "AppearanceManager.h"
#include "ShoutDialog.h"
#include "GuageDialog.h"
#include "InventoryExDialog.h"
#include "StorageDialog.h"
#include "QuickDlg.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "../[cc]skill/client/info/buffskillinfo.h"
#include "../[cc]skill/Client/Object/SkillObject.h"
#include "NpcImageDlg.h"
#include "cskilltrainingdlg.h"
#include "cSkillTreeDlg.h"
#include "ChatManager.h"
#include "ObjectBalloon.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "PKManager.h"
#include "MHTimeManager.h"
#include "CWayPointManager.h"
#include "BigMapDlg.h"
#include "MiniMapDlg.h"
#include "TileManager.h"
#include "MovePoint.h"
#include "WorldMapDlg.h"
#include "MonsterGuageDlg.h"
#include "BattleGuageDlg.h"
#include "..\hseos\Family\SHFamily.h"

VECTOR3 gHeroPos;

extern volatile LONG g_PathThreadActivated;

CHero::CHero() :
m_pcsFamily(new CSHFamily),
m_WayPointManager(new CWayPointManager)
{
	//m_NextAction.Clear();
	m_CurComboNum = 0;
	m_maxExpPoint = 0;
	m_StorageNum = 0;	
	m_matchedQuantity = 0;
	m_byExtendedInvenCount = 0;
}

CHero::~CHero()
{
	ITEMMGR->RemoveCoolTime( GetID() );

	Release();
	SAFE_DELETE(m_pcsFamily);
	SAFE_DELETE(m_WayPointManager);
}

void CHero::InitHero(HERO_TOTALINFO * pHeroTotalInfo)
{
	mMoveSound = 0;
	m_MovingAction.Clear();
	m_NextAction.Clear();
	m_SkillStartAction.Clear();
	m_bIsAutoAttackMode = TRUE;
	m_bIsAutoAttacking = FALSE;
	memcpy(&m_HeroTotalInfo, pHeroTotalInfo, sizeof(HERO_TOTALINFO));

	m_maxExpPoint = GAMERESRCMNGR->GetMaxExpPoint(GetLevel());
	
	ZeroMemory( &mItemBaseStat, sizeof( PlayerStat ) );
	ZeroMemory( &mItemOptionStat, sizeof( PlayerStat ) );

	// 080313 LUJ, 세트 아이템 능력 초기화
	ZeroMemory( &mSetItemStat, sizeof( PlayerStat ) );

	m_dwLastSocietyAction = gCurTime;

	GAMEIN->GetQuickDlg()->SetActiveSheet( 0, 0 );

	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.23 2008.01.11:CPlayer 에서 옮김
	ZeroMemory(&m_stMonstermeterInfo, sizeof(m_stMonstermeterInfo));
	mCurrentSkillObjectIndex = 0;


//---KES 장비 방어력 적용 여부 071128
	m_bNoDeffenceFromArmor = FALSE;
	mSkillDelay = 0;
	mSkillCancelDelay = 0;
	m_dwFollowPlayerIdx = 0;
//---------------

	// 090526 ShinJS --- 이름으로 파티초대시 작업시각 초기화
	m_dwInvitePartyByNameLastTime = 0;
	m_dwInvitePartyByNameLimitTime = 0;


	// 100226 ShinJS --- 인챈트 레벨 정보 추가
	mWeaponEnchantLevel = 0;
	mPhysicDefenseEnchantLevel = 0;
	mMagicDefenseEnchantLevel = 0;
}

void CHero::Process()
{
	CPlayer::Process();
	CAMERA->Process();

	if(m_MovingAction.HasAction())
	{
		if(m_MovingAction.CheckTargetDistance(&GetCurPosition()) == TRUE)
		{
			// 091026 ShinJS --- 액션 실행시 길찾기정보 초기화
			ClearWayPoint();

			MOVEMGR->HeroMoveStop();

			m_MovingAction.ExcuteAction(this);
			m_MovingAction.Clear();
		}
		else
		{
			if( m_MovingAction.GetActionKind() == eActionKind_Skill )
			{
				static DWORD dwSkillPersuitTime	= 0;

				if(gCurTime > dwSkillPersuitTime)
				{
					m_MovingAction.ExcuteAction(this);

					const int SKILL_PERSUIT_TICK = 1000;
					dwSkillPersuitTime = gCurTime + SKILL_PERSUIT_TICK;
				}
			}
			else if( MOVEMGR->IsMoving(this) == FALSE )
			{
				m_MovingAction.Clear();
			}
		}
	}
	else if(m_bIsAutoAttacking)
	{
		if(m_NextAction.HasAction() == FALSE)
		{
			if(GetCurComboNum() < MAX_COMBO_NUM || GetCurComboNum() == SKILL_COMBO_NUM)	// 자동공격은 콤보 2까지만	12/3일 회의 결과 3에서 2로 바뀜
			{
				if(SKILLMGR->OnSkillCommand(this,&m_AutoAttackTarget,FALSE) == FALSE)
					DisableAutoAttack();
			}
		}
		else
		{
			m_NextAction.ExcuteAction(this);
		}
	}

	// 090204 LUJ, 특수 스킬을 하나씩 검사하여 활성/비활성시킨다
	ProcessSpecialSkill();
	GetPosition(&gHeroPos);
	MAP->SetShadowPivotPos(&gHeroPos);
	AUDIOMGR->SetListenerPosition(
		&gHeroPos,
		GetAngle());

	if(IsDied())
	{
		if(eObjectState_Die != GetState())
		{
			OBJECTSTATEMGR->EndObjectState(
				this,
				GetState());
			OBJECTSTATEMGR->StartObjectState(
				this,
				eObjectState_Die);

			ChangeMotion(
				CHARACTER_MOTION[eCharacterMotion_Die1][GetWeaponAniType()],
				0);
			ChangeBaseMotion(
				CHARACTER_MOTION[eCharacterMotion_Died1][GetWeaponAniType()]);
		}

		PKMGR->SetPKStartTimeReset();
	}

	if(GetState() == eObjectState_SkillUsing)
	{
		if(gCurTime - GetStateStartTime() > 10000)	//숫자를 줄이기엔 진법부분이 걸린다.
		{
			OBJECTSTATEMGR->EndObjectState(this,eObjectState_SkillUsing);
		}
	}

	ProcMonstermeterPlayTime();
	OBJECTMGR->ApplyShadowOption(
		this);

	// 090909 ShinJS --- 길찾기 결과에 따른 처리
	switch( m_WayPointManager->GetWMStatus() )
	{
	// 성공했을 때
	case eWMStatus_Success:
		{
			// 현 웨이포인트를 무효화하고, 새로운 데이터를 넣어야 한다
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);

			// 다음 웨이포인트로 이동
			NextMove();

			// 091204 --- 목표 좌표 설정(연속하여 길찾기를 한경우 Stop메세지를 받으면 목표좌표가 지워진다. 다시 설정할수 있도록 한다.)
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
			CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
			CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			if( pBigMapDlg )		pBigMapDlg->SetMovePoint( &vecDest );
			if( pWorldToBigMapDlg )	pWorldToBigMapDlg->SetMovePoint( &vecDest );
			if( pMiniMapDlg )		pMiniMapDlg->SetMovePoint( &vecDest );

			MOVEPOINT->SetPoint( &vecDest );
		}
		break;
	// 실패한 경우
	case eWMStatus_Fail:
		{
			m_WayPointManager->SetWMStatus(eWMStatus_Inactive);
			ClearWayPoint();

			// 091208 ShinJS --- 길찾기 실패시 직선상 이동 최대지점으로 이동한다.
			VECTOR3 vecDest = m_WayPointManager->GetFlagDestination();
			VECTOR3 startPos;
			GetPosition( &startPos );

			// 직선상 이동가능 최대지점을 구한다.
			if( MAP->CollisionLine( &startPos, &vecDest, &vecDest, MAP->GetMapNum(), this ) == FALSE )
				break;

			if( MOVEMGR->IsMoving( this ) )
				MOVEMGR->CalcPositionEx( this, gCurTime );

			if ( MAP->CollisionCheck_OneLine_New( &startPos, &vecDest ) == TRUE)
			{
				Move_Simple( &vecDest );
				MOVEPOINT->SetPoint( &vecDest );
			}
		}
		break;
	}
}

// desc_hseos_몬스터미터01
// S 몬스터미터 추가 added by hseos 2007.05.23 2008.01.11:CPlayer 에서 옮김
void CHero::ProcMonstermeterPlayTime()
{
	if (gCurTime > m_stMonstermeterInfo.nPlayTimeTick)
	{
		m_stMonstermeterInfo.nPlayTimeTick = gCurTime + 1000;
		++m_stMonstermeterInfo.nPlayTime;
		++m_stMonstermeterInfo.nPlayTimeTotal;
	}
}
// E 몬스터미터 추가 added by hseos 2007.05.23 2008.01.11:CPlayer 에서 옮김

void CHero::GetHeroTotalInfo(HERO_TOTALINFO * info)
{
	ASSERT(info);
	memcpy(info, &m_HeroTotalInfo, sizeof(HERO_TOTALINFO));
}


void CHero::ApplyInterface()
{
#ifndef _TESTCLIENT_
	SetMoney(m_HeroTotalInfo.Money);
	SetLevel(m_CharacterInfo.Level);
	SetMaxLife(m_CharacterInfo.MaxLife);
	SetLife(m_CharacterInfo.Life, 0);
	SetMaxMana(m_HeroTotalInfo.MaxMana);
	SetMana(m_HeroTotalInfo.Mana, 0);
	SetMaxExpPoint(m_maxExpPoint);

	SetExpPoint(m_HeroTotalInfo.ExpPoint);

	SetStrength(m_HeroTotalInfo.Str);
	SetDexterity(m_HeroTotalInfo.Dex);
	SetVitality(m_HeroTotalInfo.Vit);
	SetWisdom(m_HeroTotalInfo.Wis);
	SetIntelligence(m_HeroTotalInfo.Int);

	SetPartyIdx(m_HeroTotalInfo.PartyID);
	SetGuildMemberRank(m_CharacterInfo.PositionInMunpa);
	SetGuildIdx(m_CharacterInfo.MunpaID);
	SetGuildName(m_CharacterInfo.GuildName);
	SetFamilyName(m_CharacterInfo.FamilyName);
	GAMEIN->GetCharacterDialog()->RefreshGuildInfo();
	GAMEIN->GetCharacterDialog()->RefreshFamilyInfo();
	SetBadFame(m_CharacterInfo.BadFame);
	// 070117 LYW --- Add function to setting character face.
	GAMEIN->GetGuageDlg()->MakeHeroFace() ;

	if(cStatic* const nameStatic = (cStatic*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGENAME))
	{
		nameStatic->SetStaticText(
			m_BaseObjectInfo.ObjectName);
	}
#endif
}

void CHero::SetMaxLife(DWORD maxlife)
{
	CPlayer::SetMaxLife(maxlife);

	DWORD newMaxLife = GetMaxLife();
	if(newMaxLife == 0)
	{
		newMaxLife = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)m_CharacterInfo.Life/(float)newMaxLife, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetLife(), newMaxLife );

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );	
}

void CHero::Heal(CObject* pHealer,BYTE HealKind,DWORD HealVal)
{
	CHATMGR->AddMsg(
		CTC_GETMONEY,
		CHATMGR->GetChatMsg(778),
		pHealer ? pHealer->GetObjectName() : GetObjectName(),
		HealVal);
	EFFECTMGR->AddHealNumber(
		HealVal,
		pHealer,
		this,
		HealKind);
	ChangeLife(
		HealVal);
}
void CHero::ChangeLife(int changeval)
{
	// 070419 LYW --- Hero : modified function ChangeLife.
	// 080616 LUJ, 최대 HP를 넘지 않게 표시되도록 한다
	DWORD newlife = min( GetMaxLife(), GetLife() + changeval );

	SetLife(newlife);
}

void CHero::SetLife(DWORD life, BYTE type)
{
	CPlayer::SetLife(life);

	if( m_BaseObjectInfo.ObjectBattleState == eObjectBattleState_Battle )
	{
		GAMEIN->GetBattleGuageDlg()->HeroBattleGuageUpdate();
	}

	if(GetState() == eObjectState_Die)
	{
		life = 0;
		GAMEIN->GetBattleGuageDlg()->HeroDie();
	}

//---KES divide 0 방지
	DWORD MaxLife = GetMaxLife();
	if(MaxLife == 0)
	{
		MaxLife = 1;
	}

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFE))->SetValue(
		(float)life/(float)MaxLife, 
		1000);

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", life, GetMaxLife() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELIFETEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetMoney(MONEYTYPE Money)
{
	m_HeroTotalInfo.Money = Money;
	if(GAMEIN->GetInventoryDialog())
		GAMEIN->GetInventoryDialog()->SetMoney(Money);

	GAMEIN->GetSkillTrainingDlg()->SetGold();
}

void CHero::SetMaxExpPoint(EXPTYPE dwPoint)
{
	m_maxExpPoint = dwPoint;

	float value = (float)GetExpPoint()/(float)m_maxExpPoint;

	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, 0 );
	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent( value*100 );

	// 인터페이스 : 수치 표시
	char szValue[50];
	// 070124 LYW --- Modified this line.
	//sprintf( szValue, "%d / %d", GetExpPoint(), dwPoint );
	sprintf( szValue, "%d/%d", GetExpPoint(), dwPoint );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps )	ps->SetStaticText( szValue );
}

void CHero::SetExpPoint(EXPTYPE dwPoint, BYTE type)
{
	m_HeroTotalInfo.ExpPoint = dwPoint;

	// 인터페이스 게이지 변경
	float value = (float)dwPoint/(float)m_maxExpPoint;
	// 080530 LYW --- Hero : Modified a data type DWORD to EXPTYPE.
	DWORDEX valueTime = (type == 0)? 0 : (2000/m_maxExpPoint)*dwPoint;
	//DWORD valueTime = 0 ;

	// 080604 LYW --- Hero : 경험치 게이지 처리 수정.
	// 경험치 게이지는 time을 쓰지 않는게 맞는다고 판단.
	// type을 0으로 세팅한다.
	type = 1 ;

	// 080626 LYW --- Hero : 최대 레벨 제한 수치가 숫자 였던 것을 Define 문으로 수정함.
	//if(GetLevel() == 99 || type == 0 )
	if(GetLevel() == MAX_CHARACTER_LEVEL_NUM || type == 0 )
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( 1, 0 );
	else
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINT))->SetValue( value, (DWORD)valueTime );

	// 인터페이스 : 캐릭터 창
	CObjectGuagen* ps = (CObjectGuagen *)GAMEIN->GetCharacterDialog()->GetWindowForID(CI_EXPPOINTGUAGE);
	if( ps )
	{
		if(type == 0)
			ps->SetValue( value, 0 );
		else
			ps->SetValue( value, (DWORD)valueTime );
	}	

	// 070113 LYW --- Delete this line.
	//GAMEIN->GetCharacterDialog()->SetExpPointPercent(value*100);
	// 인터페이스 : 수치 표시

	// 080603 LYW --- Hero : 경험치 처리 (__int32)에서 (__int64)로 변경 처리.
	//char szValue[50];
	//if(GetLevel() >= 99)
	//	sprintf( szValue, "[ %s ]", CHATMGR->GetChatMsg(179) );	//"[ 完 ]"
	//else
	//	sprintf( szValue, "%.2f%%", value*100 );

//	#ifdef _CHEATENABLE_	
//	if(CHEATMGR->IsCheatEnable())
//	{
//		char buf1[66];
//		wsprintf(buf1, " %d/%d", dwPoint, GetMaxExpPoint());
//		strcat(szValue, buf1);
//	}
//	#endif

	char szValue[50];
	sprintf( szValue, "%.2f%%", value*100 );

#ifdef _CHEATENABLE_	
	if(CHEATMGR->IsCheatEnable())
	{
		char buf1[66];
		EXPTYPE exp = GetMaxExpPoint() ;
 		sprintf(buf1, " %0.f/%0.f", (float)dwPoint, (float)exp);
		strcat(szValue, buf1);
	}
#endif

	cStatic* ps2 = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEEXPPOINTTEXT);
	if( ps2 )	ps2->SetStaticText( szValue );
}

void CHero::SetLevel(LEVELTYPE level)
{
	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetLevel(level);

	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGELEVEL);
	char tempStr[ 128 ] = {0, } ;
	sprintf( tempStr, "%d", level ) ;
	if( ps ) ps->SetStaticText( tempStr ) ;

	CPlayer::SetLevel(level);
	ITEMMGR->RefreshAllItem();
}

void CHero::SetSkillPoint(DWORD point)
{
	m_HeroTotalInfo.SkillPoint += point;
	GAMEIN->GetSkillTrainingDlg()->SetSkillPoint();
	GAMEIN->GetSkillTreeDlg()->SetSkillPoint();
}

void CHero::SetMaxMana(DWORD val)
{
	m_HeroTotalInfo.MaxMana = val;

	// 인터페이스 게이지 변경
	DWORD neMaxMana = GetMaxMana();
	if(neMaxMana == 0)
	{
		neMaxMana = 1;
	}
	((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
		(float)m_HeroTotalInfo.Mana/(float)neMaxMana, 0 );

	char szValue[50] = {0};
	sprintf( szValue, "%d/%d", GetMana(), neMaxMana );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );
}

// 100223 ShinJS --- 마나 데미지로 인하여 마나정보를 변화량으로 받아 갱신
void CHero::ChangeMana(int changeval)
{
	DWORD newMana = min( GetMaxMana(), GetMana() + changeval );
	SetMana( newMana );
}

void CHero::SetMana(DWORD val, BYTE type)
{
	CPlayer::SetMana(val);

	if(m_HeroTotalInfo.MaxMana != 0)
	{
		((CObjectGuagen*)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANA))->SetValue(
			(float)val/(float)GetMaxMana(), 
			1000);
	}

	// 인터페이스 : 수치 표시
	char szValue[50];
	sprintf( szValue, "%d/%d", val, GetMaxMana() );
	cStatic* ps = (cStatic *)GAMEIN->GetGuageDlg()->GetWindowForID(CG_GUAGEMANATEXT);
	if( ps )	ps->SetStaticText( szValue );

	// 수치변경
	m_HeroTotalInfo.Mana = val;
}
void CHero::SetStrength(DWORD val)
{
	m_HeroTotalInfo.Str = val;

	//SW060906 펫작업으로 변경
	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetStrength();
	// 070411 LYW --- Hero : Modified function SetStrength.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetDexterity(DWORD val)
{
	m_HeroTotalInfo.Dex = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);


	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetDexterity();
	// 070411 LYW --- Hero : Modified function SetDexterity.
	//GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->PrintAccuracyRate();
	GAMEIN->GetCharacterDialog()->PrintEvasionRate();
	GAMEIN->GetCharacterDialog()->PrintCriticalRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetVitality(DWORD val)
{
	m_HeroTotalInfo.Vit = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetVitality();
	// 070411 LYW --- Hero : Modified function SetVitality.
	//GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->PrintDefenseRate();

	GAMEIN->GetCharacterDialog()->UpdateData();
}
void CHero::SetWisdom(DWORD val)
{
	m_HeroTotalInfo.Wis = val;

	CHARCALCMGR->CalcCharStats(this);
	//CHARCALCMGR->CalcCharStats(this);

	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetWisdom();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagDefenseRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

// 070411 LYW --- Hero : Add function to setting intelligence to hero.
void CHero::SetIntelligence( DWORD val )
{
	m_HeroTotalInfo.Int = val;

	CHARCALCMGR->CalcCharStats(this);

	// 인터페이스 창 표시 수치 변경
	GAMEIN->GetCharacterDialog()->SetIntelligence();
	GAMEIN->GetCharacterDialog()->UpdateData();
	GAMEIN->GetCharacterDialog()->PrintMagAttackRate() ;
	GAMEIN->GetCharacterDialog()->PrintMagicCriticalRate() ;
	GAMEIN->GetCharacterDialog()->UpdateData();
}

void CHero::SetBadFame( FAMETYPE val )
{
	m_CharacterInfo.BadFame = val;
	GAMEIN->GetCharacterDialog()->SetBadFame( val );	
}

BOOL CHero::StartSocietyAct( int nStartMotion, int nIngMotion, int nEndMotion, BOOL bHideWeapon )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Immortal )
	{
		if( GetState() == eObjectState_Society )
		{
			if( m_ObjectState.State_End_Motion != -1 )	//앉기 동작의 경우
			{
				if( m_ObjectState.bEndState == TRUE )
					return FALSE;
				if( gCurTime - GetStateStartTime() < GetEngineObject()->GetAnimationTime(m_ObjectState.State_Start_Motion ) )
					return FALSE;

				OBJECTSTATEMGR->EndObjectState( this, GetState(), GetEngineObject()->GetAnimationTime( m_ObjectState.State_End_Motion ) );
				return TRUE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	m_ObjectState.State_Start_Motion	= (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion		= (short int)nIngMotion;
	m_ObjectState.State_End_Motion		= (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;


	if(m_ObjectState.State_Start_Motion != -1)
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );

		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}


	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );

	m_BaseObjectInfo.ObjectState	= eObjectState_Society;
	m_ObjectState.State_Start_Time	= gCurTime;
	m_ObjectState.bEndState			= FALSE;

	if( m_ObjectState.State_Ing_Motion == -1 )
		OBJECTSTATEMGR->EndObjectState( this, eObjectState_Society, GetEngineObject()->GetAnimationTime( m_ObjectState.State_Start_Motion ) );

	return TRUE;
}

BOOL CHero::EndSocietyAct()
{
	if( m_ObjectState.State_Ing_Motion != -1 )
	{
		return FALSE;
	}
	return TRUE;
}

void CHero::StartSocietyActForQuest( int nStartMotion, int nIngMotion /* = -1 */, int nEndMotion /* = -1 */, BOOL bHideWeapon /* = FALSE  */ )
{
	m_ObjectState.State_Start_Motion = (short int)nStartMotion;
	m_ObjectState.State_Ing_Motion = (short int)nIngMotion;
	m_ObjectState.State_End_Motion = (short int)nEndMotion;
	m_ObjectState.MotionSpeedRate = 1.0f;

	if( nEndMotion != -1 )
		m_ObjectState.State_End_MotionTime = GetEngineObject()->GetAnimationTime(nEndMotion);
	else
		m_ObjectState.State_End_MotionTime = 0;

	if( m_ObjectState.State_Start_Motion != -1 )
	{
		ChangeMotion( m_ObjectState.State_Start_Motion, FALSE );
		if( m_ObjectState.State_Ing_Motion != -1 )
			ChangeBaseMotion( m_ObjectState.State_Ing_Motion );
	}

	if( bHideWeapon )
		APPEARANCEMGR->HideWeapon( this );
}

#ifdef _GMTOOL_
const char* GetStateName( BYTE state )
{
	static char StateName[eObjectState_MAX][64] = {
	"eObjectState_None",						
	"eObjectState_Enter",						
	"eObjectState_Move",						
	"eObjectState_Ungijosik",					
	"eObjectState_Rest",						
	"eObjectState_Deal",						
	"eObjectState_Exchange",					
	"eObjectState_StreetStall_Owner",			
	"eObjectState_StreetStall_Guest",			
	"eObjectState_PrivateWarehouse",			
	"eObjectState_Munpa",						
	"eObjectState_SkillStart",				
	"eObjectState_SkillSyn",					
	"eObjectState_SkillBinding",				
	"eObjectState_SkillUsing",				
	"eObjectState_SkillDelay",				
	"eObjectState_TiedUp_CanMove",			
	"eObjectState_TiedUp_CanSkill",
	"eObjectState_TiedUp",
	"eObjectState_Die",						
	"eObjectState_BattleReady",
	"eObjectState_Exit",						
	"eObjectState_Immortal",					
	"eObjectState_Society",					
	"eObjectState_ItemUse",
	"eObjectState_TournamentReady",
	"eObjectState_TournamentProcess",
	"eObjectState_TournamentEnd",
	"eObjectState_TournamentDead",
	"eObjectState_Engrave",
	"eObjectState_RestDamage",
	"eObjectState_Connect"
	};

	return StateName[state];
}
#endif

void CHero::OnStartObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Move:
		{
			if(IsGetOnVehicle())
			{
				break;
			}

			SNDIDX walkSound = 1254;
			SNDIDX runSound = 1255;

			if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(GetWearedItemIdx(eWearedItem_Wing)))
			{
				if(ePartType_Wing == itemInfo->Part3DType)
				{
					walkSound = 1342;
					runSound = 1343;
				}
			}

			mMoveSound = AUDIOMGR->Play(
				eMoveMode_Run == m_MoveInfo.MoveMode ? runSound : walkSound,
				GetID());
			break;
		}
	}

	CPlayer::OnStartObjectState(State);
}

void CHero::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Enter:
		{
		}
		break;
	case eObjectState_Deal:
		{
			HERO->HeroStateNotify(eObjectState_Deal);
			GAMEIN->GetNpcImageDlg()->SetDisable( FALSE );
			//GAMEIN->GetNpcImageDlg()->SetActive( FALSE );
		}
		break;

	case eObjectState_SkillUsing:
	case eObjectState_SkillBinding:
		SKILLMGR->OnComboTurningPoint(this);
		break;

	case eObjectState_SkillSyn:
		{
#ifndef _TESTCLIENT_
//			if(SKILLMGR->GetSkillObject(TEMP_SKILLOBJECT_ID) != NULL)	// 서버에서 응답이 안온경우
//				OBJECTSTATEMGR->StartObjectState(this,eObjectState_SkillUsing);
//			else				
				SKILLMGR->OnComboTurningPoint(this);
#else
			SKILLMGR->OnComboTurningPoint(this);
#endif
		}
		break;

	case eObjectState_Move:
		{
			AUDIOMGR->Stop(
				mMoveSound);
		}
		break;
	case eObjectState_SkillStart:
		{
			if(m_SkillStartAction.GetActionKind() != eActionKind_Skill_RealExecute)
				break;

			m_SkillStartAction.ExcuteAction(this);
			m_SkillStartAction.Clear();
		}
		break;
	}
	CPlayer::OnEndObjectState(State);
}

//////////////////////////////////////////////////////////////////////////
// 행동 함수들..
void CHero::Die(CObject* pAttacker,BOOL bFatalDamage,BOOL bCritical, BOOL bDecisive)
{
	//	ASSERT(GetLife() == 0);

	WINDOWMGR->CloseAllMsgBox();

	GAMEIN->GetDealDialog()->SetDisable(FALSE);
	GAMEIN->GetDealDialog()->SetActive(FALSE);	// 상점창 닫기

	GAMEIN->GetNpcScriptDialog()->SetDisable(FALSE);	//NPC창도 닫기
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

	// 프로그레스 창 닫기
	{
		CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );

		if( dialog->IsActive() )
		{
			dialog->Cancel();
		}
	}

	SetMovingAction(NULL);
	DisableAutoAttack();					// 오토공격 끄기
	SetLife(0);

	const eWeaponType weapon = GetWeaponEquipType();

	DWORD time = 0;
	if( bFatalDamage )
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died2 ][ weapon ] );
	}
	else
	{
		time = HERO->GetEngineObject()->GetAnimationTime( CHARACTER_MOTION[ eCharacterMotion_Died1 ][ weapon ] );
	}
	CAMERA->SetCharState( eCS_Die, time );

	CPlayer::Die(pAttacker,bFatalDamage,bCritical,bDecisive);
	GAMEIN->GetCharacterDialog()->SetAttackRate();
	GAMEIN->GetCharacterDialog()->SetDefenseRate();
	GAMEIN->GetCharacterDialog()->UpdateData();
	// 090907 ShinJS --- 죽은 경우 WayPoint를 제거한다
	ClearWayPoint();
}

// 080910 LUJ, 데미지 상태 처리를 RESULTINFO 구조체를 받아 처리하도록 함
void CHero::Damage(CObject* pAttacker,BYTE DamageKind,DWORD Damage,const RESULTINFO& result)
{
	//////////////////////////////////////////////////////////////////////////
	// 카메라 뒤로 밀림
	VECTOR3 push;
	MATRIX4 my;
	SetVector3(&push,0,0,30);
	SetRotationYMatrix(&my,-m_RotateInfo.Angle.ToRad());
	TransformVector3_VPTR2(&push,&push,&my,1);
	CAMERA->SetPushedVibration(&push,200);
	
	if( pAttacker )
	{
		if( pAttacker->GetObjectKind() == eObjectKind_Player )
		{
			PKMGR->SetPKStartTimeReset();
		}

		// 이동 중에 공격받은 방향을 쳐다보지 말아야한다. 그렇지 않으면 문워커가 발생한다
		if(0 == OBJECTMGR->GetSelectedObject() &&
			FALSE == GetBaseMoveInfo()->bMoving)
		{
			VECTOR3 position = pAttacker->GetCurPosition() - GetCurPosition();
			GetEngineObject()->SetDirection(
				&position);
			OBJECTMGR->SetSelectedObject(
				pAttacker);
		}
	}
	
	CPlayer::Damage(pAttacker,DamageKind,Damage,result);
}

void CHero::OnHitTarget(CObject* pMainTarget)
{
	//////////////////////////////////////////////////////////////////////////
	// 주인공과 데미지 입는 메인타겟과 가까우면 조금씩 떨어트림	
	VECTOR3 targetpos = pMainTarget->GetCurPosition();
	VECTOR3 Curpos = HERO->GetCurPosition();

	float WeaponMinDistance = 100;
	switch(GetWeaponEquipType())
	{
	case eWeaponType_Sword	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Mace	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Axe	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Dagger	:	WeaponMinDistance = 100;	break;
	case eWeaponType_Staff	:	WeaponMinDistance = 150;	break;
	case eWeaponType_Bow	:	WeaponMinDistance = 150;	break;	
	case eWeaponType_Gun	:	WeaponMinDistance = 100;	break;	
		// desc_hseos_ASSERT수정_01
		// S ASSERT수정 추가 added by hseos 2007.06.02
		// ..맨손으로 공격일 때도 오류는 아니므로 추가
	case eWeaponType_None	:								break;
		// E ASSERT수정 추가 added by hseos 2007.06.02
	default: ASSERT(0);
	}

	if(CalcDistanceXZ(&targetpos,&Curpos) < WeaponMinDistance)
	{
		VECTOR3 pos;	pos.x = 0;	pos.y = 0;	pos.z = 10.f;
		float angle = HERO->GetAngle();
		TransToRelatedCoordinate(&pos,&pos,angle);
		pos = pos + Curpos;
		MOVEMGR->HeroEffectMove(HERO,&pos,0,0);
	}
}


void CHero::ClearTarget(CObject* pObject)
{
	if(m_AutoAttackTarget.GetTargetID() == pObject->GetID())
		DisableAutoAttack();
	if(m_NextAction.GetTargetID() == pObject->GetID())
	{
		m_NextAction.Clear();
	}
}


void CHero::Revive(VECTOR3* pRevivePos)
{
	DisableAutoAttack();

	CAMERA->SetCharState( eCS_Normal, 0 );	//일어나는 동작이 없다.

	CPlayer::Revive(pRevivePos);
}

DWORD CHero::DoGetAttackRate() 
{
	// 무기 공격력
	float	fWeaponAttack	= 0;
	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicAttack;
		fWeaponAttack += value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// 힘
	float	fStrengthAll	= float( GetStrength() );

	// 순수 힘
	float	fRealStrengthAll	= float( m_HeroTotalInfo.Str );
	float	fPercent		= GetRatePassiveStatus()->PhysicAttack
							+ GetRateBuffStatus()->PhysicAttack
							+ (GetItemOptionStats().mPhysicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicAttack.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= GetPassiveStatus()->PhysicAttack
							+ GetBuffStatus()->PhysicAttack
							+ GetItemOptionStats().mPhysicAttack.mPlus
							+ GetSetItemStats().mPhysicAttack.mPlus;

	// 물리 공격력			= ( ( 무기 공격력 + 레벨 ) * ( 1 + ( 힘 * 0.001 ) ) + ( 순수힘 - 30 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fWeaponAttack + wLevel ) * ( 1 + ( fStrengthAll * 0.001f ) ) + ( fRealStrengthAll - 30 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fMaxAttack, 1 ) ) );
}

DWORD CHero::DoGetDefenseRate() 
{
	// 방어구 방어력
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mPhysicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	DWORD Armor = GetWearedItemIdx( eWearedItem_Dress );
	DWORD Boots = GetWearedItemIdx( eWearedItem_Shoes );
	DWORD Glove = GetWearedItemIdx( eWearedItem_Glove );

	ITEM_INFO* pArmor = ITEMMGR->GetItemInfo( Armor );
	ITEM_INFO* pBoots = ITEMMGR->GetItemInfo( Boots );
	ITEM_INFO* pGlove = ITEMMGR->GetItemInfo( Glove );

	//---KES 장비 방어력 적용 여부 071128
	m_bNoDeffenceFromArmor = FALSE;
	//-----------------------------------

	if( m_CharacterInfo.Job[0] == 2 )	//로그
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES 장비 방어력 적용 여부 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}
	else if( m_CharacterInfo.Job[0] == 3 )	//메이지
	{
		if( ( pArmor && pArmor->ArmorType == eArmorType_Metal ) ||
			( pBoots && pBoots->ArmorType == eArmorType_Metal ) ||
			( pGlove && pGlove->ArmorType == eArmorType_Metal ) )
		{
			if( GetPassiveStatus()->HeavyArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES 장비 방어력 적용 여부 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
		else if( ( pArmor && pArmor->ArmorType == eArmorType_Leather ) ||
				 ( pBoots && pBoots->ArmorType == eArmorType_Leather ) ||
				 ( pGlove && pGlove->ArmorType == eArmorType_Leather ) )
		{
			if( GetPassiveStatus()->LightArmor < 0.1f )
			{
				fDefenseAll = 0;
				//---KES 장비 방어력 적용 여부 071128
				m_bNoDeffenceFromArmor = TRUE;
				//-----------------------------------
			}
		}
	}

	// 캐릭터 레벨
	WORD	wLevel				= GetLevel() ;
	float	DEnchant			= float( GetPhysicDefenseEnchantLevel() );
	float	fRealVitalityAll	= float( m_HeroTotalInfo.Vit );
	// 총건강
	float	fVitality			=	(float)GetVitality();

	// %옵션
	float	fPercent		= GetRatePassiveStatus()->PhysicDefense
							+ GetRateBuffStatus()->PhysicDefense
							+ (GetItemOptionStats().mPhysicDefense.mPercent * 100.f)
							+ (GetSetItemStats().mPhysicDefense.mPercent * 100.f)
							+ GetRatePassiveStatus()->Shield
							+ GetRateBuffStatus()->Shield;
	// +옵션
	float	fPlus			= GetPassiveStatus()->PhysicDefense
							+ GetBuffStatus()->PhysicDefense
							+ GetItemOptionStats().mPhysicDefense.mPlus
							+ GetSetItemStats().mPhysicDefense.mPlus
							+ GetPassiveStatus()->Shield
							+ GetBuffStatus()->Shield;

	// 물리 방어력			= ( ( 장비 방어력 + 레벨 )   * ( 1 + ( 건강 / 3000 ) )         + ( (순수건강 - 40) / 5 ) ) * ( 1 + ( 방어구 인챈트 레벨 총합 / 200 ) )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1.0f + ( fVitality / 3000 ) ) + ( ( fRealVitalityAll - 40 ) / 5 ) ) * ( 1 + DEnchant / 200 ) ) 
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );
}

DWORD CHero::DoGetMagAttackRate() 
{
	// 무기 공격력
	float	fMagicAttack		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicAttack;

		fMagicAttack = value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= GetLevel() ;
	float	AEnchant		= float( GetWeaponEnchantLevel() );

	// 지능
	float	fIntelligenceAll	= float( GetIntelligence() );

	// 순수 지능
	float	fRealIntelligenceAll	= float( m_HeroTotalInfo.Int );
	float	fPercent		= GetRatePassiveStatus()->MagicAttack
							+ GetRateBuffStatus()->MagicAttack
							+ (GetItemOptionStats().mMagicAttack.mPercent * 100.f)
							+ (GetSetItemStats().mMagicAttack.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= GetPassiveStatus()->MagicAttack
							+ GetBuffStatus()->MagicAttack
							+ GetItemOptionStats().mMagicAttack.mPlus
							+ GetSetItemStats().mMagicAttack.mPlus;

	// 마법 공격력			= ( ( 무기 공격력 + 레벨 ) * ( 1 + (지능 * 0.001) ) + ( 순수지능 - 40 ) ) * ( 1 + ( 무기 인챈트 레벨 ^ 2 / 400 ) )
	float	fMaxAttack		= ( ( ( fMagicAttack + wLevel ) * ( 1 + ( fIntelligenceAll * 0.001f ) ) + ( fRealIntelligenceAll - 40 ) ) * ( 1 + ( AEnchant * AEnchant / 400.f ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( max( 0, Round( fMaxAttack, 1 ) ) );
}

DWORD CHero::DoGetMagDefenseRate() 
{
	// 방어구 방어력
	float fDefenseAll		= 0;

	{
		const PlayerStat::Value& value = GetItemBaseStats().mMagicDefense;

		fDefenseAll = value.mPlus * ( 1.0f + value.mPercent );
	}

	// 캐릭터 레벨
	WORD	wLevel			= GetLevel() ;
	float	DEnchant		= float( GetMagicDefenseEnchantLevel() );

	// 정신
	float	fWisdomAll		= float( GetWisdom() );
	float	fRealWisdomAll	= float( m_HeroTotalInfo.Wis);

	// %옵션
	float	fPercent		= GetRatePassiveStatus()->MagicDefense
							+ GetRateBuffStatus()->MagicDefense
							+ (GetItemOptionStats().mMagicDefense.mPercent * 100.f) 
							+ (GetSetItemStats().mMagicDefense.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= GetPassiveStatus()->MagicDefense
							+ GetBuffStatus()->MagicDefense
							+ GetItemOptionStats().mMagicDefense.mPlus
							+ GetSetItemStats().mMagicDefense.mPlus;

	// 마법 방어력			= ( ( 장비 방어력 + 레벨 ) * ( 1 + ( 정신 / 3000 ) ) + ( (순수정신 - 30) / 5 ) ) * ( 1 + ( 방어구 인챈트 레벨 총합 / 200 )
	float	fTotalDefense	= ( ( ( fDefenseAll + wLevel ) * ( 1 + ( fWisdomAll / 3000 ) ) + ( fRealWisdomAll / 5 ) ) * ( 1 + ( DEnchant / 200 ) ) )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;
	
	return DWORD( max( 0, Round( fTotalDefense, 1 ) ) );
}

DWORD CHero::DoGetAccuracyRate() 
{
	// 레벨
	DWORD wLevel		= GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 민첩
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 직업상수
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %옵션
	float	fPercent		= GetRatePassiveStatus()->Accuracy
							+ GetRateBuffStatus()->Accuracy
							+ (GetItemOptionStats().mAccuracy.mPercent * 100.f)
							+ (GetSetItemStats().mAccuracy.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= GetPassiveStatus()->Accuracy
							+ GetBuffStatus()->Accuracy
							+ GetItemOptionStats().mAccuracy.mPlus
							+ GetSetItemStats().mAccuracy.mPlus;

	// 명중					= ( ( ( 민첩 - 최초 민첩 - 5 ) / ( 레벨* 5 ) ) * 0.6f + (  ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 ) * 100 )
	float	fTotalAccuracy	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( floor( 0.5f + fTotalAccuracy ) );
}

DWORD CHero::DoGetEvasionRate() 
{
	// 레벨
	DWORD wLevel		= GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );
	
	// 민첩
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 직업상수
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 14.f ; break ;
	case 2 : fClassOrderVal = 18.f ; break ;
	case 3 : fClassOrderVal = 11.f ; break ;
	case 4 : fClassOrderVal = 15.f ; break ;
	}

	// %옵션
	float	fPercent		= GetRatePassiveStatus()->Avoid
							+ GetRateBuffStatus()->Avoid
							+ (GetItemOptionStats().mEvade.mPercent * 100.f)
							+ (GetSetItemStats().mEvade.mPercent * 100.f);
	
	// +옵션
	float	fPlus			= GetPassiveStatus()->Avoid
							+ GetBuffStatus()->Avoid
							+ GetItemOptionStats().mEvade.mPlus
							+ GetSetItemStats().mEvade.mPlus;

	// 회피					= ( ( ( 민첩 - 최초 민첩 ) / (  ( 레벨 - 1 ) * 5 ) ) * 0.6f + (  ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 ) * 100 )
	float	fTotalEvasion	= ( ( fDexterityRate * 0.6f + ( ( fBaseDexterity - 30.f ) / 1000.f ) + ( wLevel * fClassOrderVal / 15000.f ) ) * 100.f )
							* ( 1 +  ( fPercent / 100 ) ) + fPlus;

	return DWORD( floor( 0.5f + fTotalEvasion ) );
}

DWORD CHero::DoGetCriticalRate() 
{
	// 레벨
	DWORD wLevel		= GetLevel() ;

	// 기본 스탯
	RaceType race			= (RaceType)GetRace();
	BYTE byClass			=  m_CharacterInfo.Job[0];
	const PlayerStat stat	= GAMERESRCMNGR->GetBaseStatus( race, byClass );

	// 민첩
	float fDexterityAll	= float( GetDexterity() );
	float fBaseDexterity	= stat.mDexterity.mPlus;
	float fDexterityRate	= ( wLevel > 1 ? max(fDexterityAll - fBaseDexterity, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 직업상수
	float fClassOrderVal = 0 ;

	switch( m_CharacterInfo.Job[0] )
	{
	case 1 : fClassOrderVal = 0.14f ; break ;
	case 2 : fClassOrderVal = 0.18f ; break ;
	case 3 : fClassOrderVal = 0.11f ; break ;
	case 4 : fClassOrderVal = 0.15f ; break ;
	}

	// %옵션
	float	fPlus		= GetPassiveStatus()->CriticalRate
						+ GetBuffStatus()->CriticalRate
						+ GetItemOptionStats().mCriticalRate.mPlus
						+ GetSetItemStats().mCriticalRate.mPlus;

	float	fPercent	= GetRatePassiveStatus()->CriticalRate
						+ GetRateBuffStatus()->CriticalRate
						+ (GetItemOptionStats().mCriticalRate.mPercent * 100.f)
						+ (GetSetItemStats().mCriticalRate.mPercent * 100.f);
	
	DWORD dwCritical	= DWORD( ( 45.f * fDexterityRate ) + ( ( fBaseDexterity - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f ) );
	
	return dwCritical;
}

DWORD CHero::DoGetMagicCriticalRate()	
{
	// 레벨
	DWORD wLevel		= GetLevel() ;

	// 기본 스탯
	RaceType race				= (RaceType)GetRace();
	BYTE byClass				=  m_CharacterInfo.Job[0];
	const PlayerStat stat		= GAMERESRCMNGR->GetBaseStatus( race, byClass );

	// 지능
	float	fIntelligenceAll	= float( GetIntelligence() );
	float fBaseIntelligence		= stat.mIntelligence.mPlus;
	float fIntelligenceRate		= ( wLevel > 1 ? max(fIntelligenceAll - fBaseIntelligence, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	// 정신
	float	fWisdomAll			= float( GetWisdom() );
	float fBaseWisdomAll		= stat.mWisdom.mPlus;
	float fWisdomAllRate		= ( wLevel > 1 ? max(fWisdomAll - fBaseWisdomAll, 0) / ( ( wLevel - 1 ) * 5.f ) : 0 );

	float	fPlus				= GetPassiveStatus()->MagicCriticalRate
								+ GetBuffStatus()->MagicCriticalRate
								+ GetItemOptionStats().mMagicCriticalRate.mPlus
								+ GetSetItemStats().mMagicCriticalRate.mPlus;

	// %옵션
	float	fPercent			= GetRatePassiveStatus()->MagicCriticalRate
								+ GetRateBuffStatus()->MagicCriticalRate
								+ (GetItemOptionStats().mMagicCriticalRate.mPercent * 100.f)
								+ (GetSetItemStats().mMagicCriticalRate.mPercent * 100.f);
	
	DWORD	dwCritical			= DWORD( ( 10.f * fIntelligenceRate ) + ( 20.f * fWisdomAllRate ) + ( ( fBaseIntelligence - 25 ) / 5.f ) + fPercent + ( fPlus / 45.f ) );
	
	return dwCritical;
}

DWORD CHero::DoGetMoveSpeedRate()	
{
	// 091229 ONS 캐릭터 정보창에 출력되는 이동속도 표시방법 변경
	float	fMoveSpeed = DoGetMoveSpeed() / 4.5f - 100.0f;
	
	return max( 0, DWORD( floor( 0.5f + fMoveSpeed ) ) );
}

DWORD CHero::DoGetCastingSpeedRate()
{
	float	fPercent			= GetRatePassiveStatus()->MagicSkillSpeedRate
								+ GetRateBuffStatus()->MagicSkillSpeedRate;

	// 100209 ONS 캐스팅 속도 표시방법 변경.
	return DWORD( floor( 0.5f + fPercent ) );
}

DWORD CHero::DoGetAttackSpeedRate()		
{
	float	fPercent			= GetRatePassiveStatus()->PhysicSkillSpeedRate
								+ GetRateBuffStatus()->PhysicSkillSpeedRate;
	
	return DWORD( floor( 0.5f + fPercent ) );
}

/* 종료해야할 상태값을 서버에도 알린다. */
void CHero::HeroStateNotify(BYTE EndState)
{
	MSG_BYTE msg;
	msg.Category = MP_CHAR;
	msg.Protocol = MP_CHAR_STATE_NOTIFY;
	msg.dwObjectID = GetID();
	msg.bData = EndState;

	NETWORK->Send(&msg, sizeof(msg));
}

BYTE CHero::GetManaPercent()
{
	return (BYTE)((GetMana()*100) / GetMaxMana());
}

DWORD CHero::DoGetMaxMana()
{
	return m_HeroTotalInfo.MaxMana;
}

void CHero::ClearGuildInfo()
{
	CPlayer::ClearGuildInfo();
}

char* CHero::GetGuildEntryDate()
{
	return m_HeroTotalInfo.MunpaCanEntryDate;
}

void CHero::SetGuildEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.MunpaCanEntryDate, day, 11);
}

char* CHero::GetFamilyEntryDate()
{
	return m_HeroTotalInfo.FamilyCanEntryDate;
}

void CHero::SetFamilyEntryDate(char* day)
{
	SafeStrCpy(m_HeroTotalInfo.FamilyCanEntryDate, day, 11);
}

void CHero::SetGuildName(char* Name)	
{ 
	SafeStrCpy(m_CharacterInfo.GuildName, Name, MAX_GUILD_NAME+1);	
}

char* CHero::GetGuildName()			
{ 
	return m_CharacterInfo.GuildName;
}

void CHero::SetFamilyName(char* Name)	
{ 
	SafeStrCpy(
		m_CharacterInfo.FamilyName,
		Name,
		_countof(m_CharacterInfo.FamilyName));
}

char* CHero::GetFamilyName()			
{ 
	return m_CharacterInfo.FamilyName;
}

BOOL CHero::CanSocietyAction( DWORD curTime )
{
	if( GetState() != eObjectState_None && GetState() != eObjectState_Society &&
		GetState() != eObjectState_Immortal )
		return FALSE;

	DWORD dwElapsedTime = gCurTime - m_dwLastSocietyAction;
	if( dwElapsedTime > 2000 ) //2초
	{
		m_dwLastSocietyAction = gCurTime;
		return TRUE;
	}

	return FALSE;
}
void CHero::ProcessSpecialSkill()
{
	if( mSpecialSkillList.empty() )
	{
		return;
	}

	// 090204 LUJ, 효율성을 위해 컨테이너 맨 앞의 스킬을 검사한 후 맨 뒤로 돌린다
	SpecialSkillData specialSkillData = mSpecialSkillList.front();
	mSpecialSkillList.pop_front();

	cBuffSkillInfo* const buffSkillInfo = specialSkillData.mBuffSkillInfo;
	const BOOL validCondition = IsEnable(*(buffSkillInfo->GetSkillInfo()));
	const BOOL isTurnOn	= (! specialSkillData.mIsTurnOn && validCondition);
	const BOOL isTurnOff = (specialSkillData.mIsTurnOn && ! validCondition);

	if( isTurnOn )
	{
		buffSkillInfo->AddBuffStatus();
		specialSkillData.mIsTurnOn = TRUE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}
	else if( isTurnOff )
	{
		buffSkillInfo->RemoveBuffStatus();
		specialSkillData.mIsTurnOn = FALSE;
		GAMEIN->GetCharacterDialog()->RefreshInfo();
	}

	// 090204 LUJ, 다음 검사를 위해 추가한다
	mSpecialSkillList.push_back( specialSkillData );
}

DWORD CHero::GetAccuracy()
{
	return mAccuracy;
}

DWORD CHero::GetAvoid()
{
	return mAvoid;
}

DWORD CHero::GetBlock()
{
	return mBlock;
}

DWORD CHero::GetPhysicAttackMax()
{
	return mPhysicAttackMax;
}

DWORD CHero::GetPhysicAttackMin()
{
	return mPhysicAttackMin;
}

DWORD CHero::GetMagicAttack()
{
	return mMagicAttack;
}

DWORD CHero::GetPhysicDefense()
{
	return mPhysicDefense;
}

DWORD CHero::GetMagicDefense()
{
	return mMagicDefense;
}

DWORD CHero::GetCriticalRate()
{
	return mCriticalRate;
}

DWORD CHero::GetCriticalDamage()
{
	return mCriticalDamage;
}

DWORD CHero::GetLifeRecover()
{
	return mLifeRecover;
}

DWORD CHero::GetManaRecover()
{
	return mManaRecover;
}

DWORD	CHero::GetMaxLifeWithItem()
{
	return mMaxLife;
}

DWORD	CHero::GetMaxManaWithItem()
{
	return mMaxMana;
}


void CHero::AddMatch()
{
	++m_matchedQuantity;

	SetHeart( TRUE );
}


void CHero::RemoveMatch()
{
	if( 1 > --m_matchedQuantity )
	{
		SetHeart( FALSE );

		m_matchedQuantity = 0;
	}
}


void CHero::CheckAllHeart()
{
	m_matchedQuantity = 0;
	
	// 전체 재계산
	{
		CYHHashTable<CObject>& objectTable = OBJECTMGR->GetObjectTable();
		YHTPOSITION position = objectTable.GetPositionHead();

		for(CObject* object = objectTable.GetDataPos( &position );
            0 < object;
			object = objectTable.GetDataPos( &position ))
		{
			if( eObjectKind_Player != object->GetObjectKind() )
			{
				continue;
			}

			CPlayer* player = ( CPlayer* )object;

			if( player->CheckHeart( this ) )
			{
				player->SetHeart( TRUE );

				AddMatch();
			}
			else
			{
				player->SetHeart( FALSE );

				RemoveMatch();
			}
		}
	}
}

DWORD CHero::GetVitality() const 
{ 
	const float rate =
		mRatePassiveStatus.Vit +
		mRateBuffStatus.Vit +
		mItemBaseStat.mVitality.mPercent +
		mItemOptionStat.mVitality.mPercent +
		mSetItemStat.mVitality.mPercent * 100.f;
	const float plus =
		mPassiveStatus.Vit +
		mBuffStatus.Vit +
		mItemBaseStat.mVitality.mPlus +
		mItemOptionStat.mVitality.mPlus +
		mSetItemStat.mVitality.mPlus;
	float Result = mCharStat.mVitality.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, 반올림시킨다
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetWisdom() const
{ 
	const float rate =
		mRatePassiveStatus.Wis +
		mRateBuffStatus.Wis +
		mItemBaseStat.mWisdom.mPercent +
		mItemOptionStat.mWisdom.mPercent +
		mSetItemStat.mWisdom.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Wis +
		mBuffStatus.Wis + 
		mItemBaseStat.mWisdom.mPlus + 
		mItemOptionStat.mWisdom.mPlus +
		mSetItemStat.mWisdom.mPlus;
	float Result = mCharStat.mWisdom.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, 반올림시킨다
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetStrength() const
{
	const float	rate =
		mRatePassiveStatus.Str +
		mRateBuffStatus.Str +
		mItemBaseStat.mStrength.mPercent +
		mItemOptionStat.mStrength.mPercent +
		mSetItemStat.mStrength.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Str +
		mBuffStatus.Str +
		mItemBaseStat.mStrength.mPlus +
		mItemOptionStat.mStrength.mPlus +
		mSetItemStat.mStrength.mPlus;
	float Result = mCharStat.mStrength.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, 반올림시킨다
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetDexterity() const 
{ 
	const float rate =
		mRatePassiveStatus.Dex +
		mRateBuffStatus.Dex +
		mItemBaseStat.mDexterity.mPercent +
		mItemOptionStat.mDexterity.mPercent +
		mSetItemStat.mDexterity.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Dex +
		mBuffStatus.Dex +
		mItemBaseStat.mDexterity.mPlus +
		mItemOptionStat.mDexterity.mPlus +
		mSetItemStat.mDexterity.mPlus;
	float Result = mCharStat.mDexterity.mPlus * (1.f + rate / 100.f) + plus;
	
	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, 반올림시킨다
	return DWORD( Round( Result, 1 ) );
}

DWORD CHero::GetIntelligence() const
{
	const float	rate =
		mRatePassiveStatus.Int +
		mRateBuffStatus.Int +
		mItemBaseStat.mIntelligence.mPercent +
		mItemOptionStat.mIntelligence.mPercent +
		mSetItemStat.mIntelligence.mPercent * 100.f;
	const float	plus =
		mPassiveStatus.Int +
		mBuffStatus.Int +
		mItemBaseStat.mIntelligence.mPlus +
		mItemOptionStat.mIntelligence.mPlus +
		mSetItemStat.mIntelligence.mPlus;
	float Result = mCharStat.mIntelligence.mPlus * (1.f + rate / 100.f) + plus;

	if( Result < 0 )
	{
		Result = 0;
	}

	// 081203 LUJ, 반올림시킨다
	return DWORD( Round( Result, 1 ) );
}

float CHero::DoGetMoveSpeed()
{
	float Speed = CPlayer::DoGetMoveSpeed();
	
	if( GetAbnormalStatus()->IsMoveStop )
	{
		Speed = 0;
	}

	return Speed;
}

void CHero::StartSkillProgressBar( DWORD time )
{
	GetObjectBalloon()->GetProgressBar()->CLI_Start(CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_TYPE, CSHFarmManager::FARM_CHAR_MOTION_PROGRESSBAR_WIDTH, time);
}
// 071210 LYW --- Hero : HERO의 확장 인벤토리 수를 설정 및 반환하는 함수 추가.
void CHero::Set_HeroExtendedInvenCount(BYTE count)
{
	// 현재는 확장 인벤 수가 두 개로 제한되어 있고, 따로 쓸일이 없을 것 같아 정의 하지 않고 사용한다.
	if( count > 2 ) return ;	

	m_byExtendedInvenCount = count ;
}

void CHero::PassiveSkillCheckForWeareItem()
{
	mPassiveStatus = Status();
	mRatePassiveStatus = Status();

	SKILLTREEMGR->SetPositionHead();

	for(SKILL_BASE* pSkillBase = SKILLTREEMGR->GetSkillBase();
		0 < pSkillBase;
		pSkillBase = SKILLTREEMGR->GetSkillBase())
	{
		const LEVELTYPE skillLevel = min(
			LEVELTYPE(SKILLMGR->GetSkillSize(pSkillBase->wSkillIdx)),
			pSkillBase->Level);
		cActiveSkillInfo* const pSkill = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo(
			pSkillBase->wSkillIdx - 1 + skillLevel);

		if( pSkill->GetKind() != SKILLKIND_PASSIVE )
			continue;

		for( DWORD cnt = 0; cnt < MAX_BUFF_COUNT; cnt++ )			
		{
			const DWORD index = pSkill->GetSkillInfo()->Buff[ cnt ];
			cBuffSkillInfo* pSkillInfo = ( cBuffSkillInfo* )SKILLMGR->GetSkillInfo( index );

			if(0 == pSkillInfo)
			{
				break;
			}
			else if(FALSE == IsEnable(*(pSkillInfo->GetSkillInfo())))
			{
				continue;
			}

			pSkillInfo->AddPassiveStatus();
		}	
	}
}

// 090204 LUJ, 특수 스킬을 추가한다.
void CHero::AddSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	// 090204 LUJ, 처음에는 항상 적용한다. 프로세스 때 검사하면서 조건에 맞지 않을 경우 해제한다
	buffSkillInfo->AddBuffStatus();

	SpecialSkillData specialSkillData = { 0 };
	specialSkillData.mBuffSkillInfo	= buffSkillInfo;
	specialSkillData.mIsTurnOn		= TRUE;
	mSpecialSkillList.push_back( specialSkillData );
}

// 090204 LUJ, 특수 스킬을 제거한다
void CHero::RemoveSpecialSkill( cBuffSkillInfo* buffSkillInfo )
{
	SpecialSkillList::iterator it = mSpecialSkillList.end();

	// 090204 LUJ, 컨테이너 중에 동일한 스킬을 찾는다
	for(	;
			mSpecialSkillList.end() != it;
			++it )
	{
		const SpecialSkillData& specialSkillData = *it;

		if( specialSkillData.mBuffSkillInfo == buffSkillInfo )
		{
			break;
		}
	}

	if( mSpecialSkillList.end() == it )
	{
		return;
	}

	const SpecialSkillData& specialSkillData = *it;

	// 090204 LUJ, 조건에 따라 미적용 상태일 수 있으므로 검사 후 취소해야 한다
	if( specialSkillData.mIsTurnOn )
	{
		buffSkillInfo->RemoveBuffStatus();
	}

	mSpecialSkillList.erase( it );
}

void CHero::ClearWayPoint()
{
	m_WayPointManager->ClearAllWayPoint();

	// Big/Mini Map의 MovePoint 제거
	CBigMapDlg* pBigMapDlg = GAMEIN->GetBigMapDialog();
	CWorldMapDlg* pWorldMapDlg = GAMEIN->GetWorldMapDialog();
	CBigMapDlg* pWorldToBigMapDlg = (pWorldMapDlg != NULL ? pWorldMapDlg->GetBigMapDlg() : NULL);
	CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
	if( pBigMapDlg )		pBigMapDlg->ClearMovePoint();
	if( pWorldToBigMapDlg )	pWorldToBigMapDlg->ClearMovePoint();
	if( pMiniMapDlg )		pMiniMapDlg->ClearMovePoint();
}

BOOL CHero::Move_UsePath( VECTOR3* pDestPos, BOOL bSimpleMode, BOOL bMousePointDisplay, BOOL bIsCollisionPosMove )
{
	// 50cm 타일의 중앙에 서게 한다.
	pDestPos->x = (int(pDestPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	pDestPos->y = 0;
	pDestPos->z = (int(pDestPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;

	// 스레드가 활성화된 동안은 진입하지 않는다
	if( InterlockedCompareExchange(&g_PathThreadActivated, 0, 0) == 1 )
	{
		return FALSE;
	}

	VECTOR3 t_StartPos;
	GetPosition( &t_StartPos );

	/// 0. 타겟이 이동불가지역일 경우
	if (MAP->CollisionCheck_OneTile( pDestPos ) == FALSE)
	{
		// 이동불가지역에서의 이동 여부 판단
		if( !bIsCollisionPosMove )
			return FALSE;

		// 직선상 이동가능 최대지점을 구한다.
		if( MAP->CollisionLine( &t_StartPos, pDestPos, pDestPos, MAP->GetMapNum(), this ) == FALSE )
			return FALSE;
	}

	/// 사전 처리
	ClearWayPoint();

	if( MOVEMGR->IsMoving( this ) )
		MOVEMGR->CalcPositionEx( this, gCurTime );

	/// 1. 한번에 직선 거리로 갈 수 있다면 직선으로 이동
	HTR_S(eRT_OneLineCheck);
	BOOL t_CollisionCheck_Oneline = MAP->CollisionCheck_OneLine_New( &t_StartPos, pDestPos );
	HTR_E(eRT_OneLineCheck);
	if ( t_CollisionCheck_Oneline == TRUE)
	{
		Move_Simple( pDestPos );

		if( bMousePointDisplay )
			MOVEPOINT->SetPoint( pDestPos );
	}
	else
	{
		// 정지
		if( MOVEMGR->IsMoving( this ) )
			MOVEMGR->HeroMoveStop();

		/// 2. 길찾기로 이동
		// 길찾기 쓰레드를 생성한다
		// 쓰레드가 리턴될 때 웨이포인트가 추가된다
		// 091204 ShinJS --- 3배속 사용
		if(FALSE == MAP->PathFind(
			&t_StartPos,
			pDestPos,
			m_WayPointManager,
			FALSE))
		{
			return FALSE;
		}
	}

	m_WayPointManager->SetFlagDestination( *pDestPos );

	return TRUE;
}

BOOL CHero::Move_Simple( VECTOR3* pTargetPos )
{
	m_MoveInfo.InitTargetPosition();
	m_MoveInfo.SetTargetPosition( 0, *pTargetPos );
	m_MoveInfo.SetMaxTargetPosIdx( 1 );

	MOVE_ONETARGETPOS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_ONETARGET;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = HEROID;
	msg.SetStartPos( &m_MoveInfo.CurPosition );
	msg.SetTargetPos( m_MoveInfo.GetTargetPosition(0) );

	NETWORK->Send(&msg,msg.GetSize());

	MOVEMGR->StartMoveEx( this, NULL, gCurTime );

	MACROMGR->CheckToggleState( this );


	// 091208 ShinJS --- 소유주이면서 탑승 중인 경우 탈것도 이동시킨다.
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( GetVehicleID() );
	if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
	{
		if( pVehicle->GetOwnerIndex() == GetID() &&
			IsGetOnVehicle() )
		{
			pVehicle->StartMove( pTargetPos );
		}
	}
	return TRUE;
}

BOOL CHero::NextMove()
{
	VECTOR3 t_WayPoint;
	if ( m_WayPointManager->PopWayPoint(&t_WayPoint) == TRUE )
	{
		if( !Move_Simple( &t_WayPoint ) )
		{
			ClearWayPoint();
			return FALSE;
		}

		return TRUE;
	}

	ClearWayPoint();
	return FALSE;
}

float CHero::GetBonusRange() const
{
	const float staticValue = mBuffStatus.Range + mPassiveStatus.Range;
	const float dynamicValue = mRateBuffStatus.Range + mRatePassiveStatus.Range;

	return staticValue * (1.0f + dynamicValue);
}

BOOL CHero::IsNoEquip(eArmorType armorType, eWeaponType weaponType, eWeaponAnimationType weaponAnimationType)
{
	switch(armorType)
	{
	case eArmorType_Robe:
	case eArmorType_Leather:
	case eArmorType_Metal:
		{
			if(GetArmorType(eWearedItem_Dress) != armorType)
			{
				return TRUE;
			}

			break;
		}
	case eArmorType_Shield:
		{
			if(GetArmorType(eWearedItem_Shield) != armorType)
			{
				return TRUE;
			}

			break;
		}
	}

	if(eWeaponType_None != weaponType)
	{
		if(GetWeaponEquipType() != weaponType)
		{
			return TRUE;
		}
	}

	switch( weaponAnimationType )
	{
	case eWeaponAnimationType_None:
		break;
	case eWeaponAnimationType_OneHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_OneHand:
			case eWeaponAnimationType_Dagger:
			case eWeaponAnimationType_TwoBlade:
				break;
			default:
				return TRUE;
			}
		}
		break;

	case eWeaponAnimationType_TwoHand:
		{
			switch( GetWeaponAniType() )
			{
			case eWeaponAnimationType_TwoHand:
			case eWeaponAnimationType_Staff:
			case eWeaponAnimationType_Bow:
			case eWeaponAnimationType_Gun:
			case eWeaponAnimationType_Spear:
				break;
			default:
				return TRUE;
			}
		}
		break;

	default:
		{
			if( GetWeaponAniType() != weaponAnimationType )
				return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CHero::IsEnable(const BUFF_SKILL_INFO& info)
{
	if(IsNoEquip(
		info.mArmorType,
		info.mWeaponType,
		info.mWeaponAnimationType))
	{
		return FALSE;
	}

	float checkValue = 0;
	const BUFF_SKILL_INFO::Condition& condition = info.mCondition;

	switch( condition.mType )
	{
	case BUFF_SKILL_INFO::Condition::TypeLifePercent:
		{
			checkValue = float(GetLifePercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLife:
		{
			checkValue = float(GetLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeMax:
		{
			checkValue = float(GetMaxLife());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeLifeRecovery:
		{
			checkValue = float(GetLifeRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaPercent:
		{
			checkValue = float(GetManaPercent());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMana:
		{
			checkValue = float(GetMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaMax:
		{
			checkValue = float(GetMaxMana());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeManaRecovery:
		{
			checkValue = float(GetManaRecover());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeStrength:
		{
			checkValue = float(GetStrength());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeIntelligence:
		{
			checkValue = float(GetIntelligence());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeDexterity:
		{
			checkValue = float(GetDexterity());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeWisdom:
		{
			checkValue = float(GetWisdom());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeVitality:
		{
			checkValue = float(GetVitality());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalAttack:
		{
			checkValue = float(GetPhysicAttackMax());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalAttack:
		{
			checkValue = float(GetMagicAttack());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypePhysicalDefense:
		{
			checkValue = float(GetPhysicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMagicalDefense:
		{
			checkValue = float(GetMagicDefense());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalRate:
		{
			checkValue = float(GetCriticalRate());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeCriticalDamage:
		{
			checkValue = float(GetCriticalDamage());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeAccuracy:
		{
			checkValue = float(GetAccuracy());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeEvade:
		{
			checkValue = float(GetAvoid());
			break;
		}
	case BUFF_SKILL_INFO::Condition::TypeMoveSpeed:
		{
			checkValue = float(GetMoveSpeed());
			break;
		}
	}

	BOOL isEnable = TRUE;

	switch(condition.mOperation)
	{
	case BUFF_SKILL_INFO::Condition::OperationMore:
		{
			isEnable = (checkValue > condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationMoreEqual:
		{
			isEnable = (checkValue >= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLess:
		{
			isEnable = (checkValue < condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationLessEqual:
		{
			isEnable = (checkValue <= condition.mValue);
			break;
		}
	case BUFF_SKILL_INFO::Condition::OperationEqual:
		{
			isEnable = (checkValue == condition.mValue);
			break;
		}
	}

	return isEnable;
}

void CHero::SetObjectBattleState(eObjectBattleState eBattleState)
{
	CPlayer::SetObjectBattleState( eBattleState );

	cBattleGuageDlg* pBattleGuageDlg	=	GAMEIN->GetBattleGuageDlg();

	if( !pBattleGuageDlg )
		return;
	
	if( eBattleState == eObjectBattleState_Battle )
	{
		pBattleGuageDlg->StartBattleSignal();
	}
	else if( eBattleState == eObjectBattleState_Peace )
	{
		pBattleGuageDlg->EndBattleSignal();
	}
}

void CHero::SetFamily(CSHFamily* family)
{
	m_pcsFamily->Set(
		family);
}
