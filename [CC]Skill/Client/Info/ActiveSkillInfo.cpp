#include "stdafx.h"
#include ".\activeskillinfo.h"
#include "../manager/Skillmanager.h"
#include "ActionTarget.h"

#include "GameIn.h" 
#include "Hero.h"
#include "../[CC]Skill/Client/Object/SkillObject.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "ChatManager.h"
#include "BattleSystem_client.h"
#include "ObjectManager.h"
#include "ShowdownManager.h"
#include "../[CC]Header/GameResourceStruct.h"
#include "../hseos/Date/SHDateManager.h"
#include "itemmanager.h"
#include "petmanager.h"

cActiveSkillInfo::cActiveSkillInfo(void) :
cSkillInfo(cSkillInfo::TypeActive),
mCoolTime(0)
{
	ZeroMemory(
		&mInfo,
		sizeof(mInfo));
}

cActiveSkillInfo::~cActiveSkillInfo(void)
{}

void cActiveSkillInfo::Parse(LPCTSTR text)
{
	GAMERESRCMNGR->Parse(
		text,
		mInfo);
}

BOOL cActiveSkillInfo::IsExcutableSkillState(CHero* pHero,int SkillLevel,CActionTarget* pTarget)
{	
	if(pHero->IsDied())
	{
		return FALSE;
	}
	else if( pHero->GetAbnormalStatus()->IsStun ||
		pHero->GetAbnormalStatus()->IsFreezing ||
		pHero->GetAbnormalStatus()->IsStone ||
		pHero->GetAbnormalStatus()->IsSlip ||
		pHero->GetAbnormalStatus()->IsParalysis )
	{
		return FALSE;
	}
	if( pHero->GetAbnormalStatus()->IsSilence )
	{
		// 080616 LUJ, 침묵 때는 물리 공격을 제외한 전체 스킬을 사용하지 못하도록 한다
		if( mInfo.Unit != UNITKIND_PHYSIC_ATTCK )
			return FALSE;
	}
	if( pHero->GetAbnormalStatus()->IsBlockAttack )
	{
		if( mInfo.Unit == UNITKIND_PHYSIC_ATTCK )
			return FALSE;
	}

	// 090709 ShinJS ---- 탈것에 탑승중인 경우 스킬 사용을 막는다.
	if( pHero->IsGetOnVehicle() )
	{
		pHero->DisableAutoAttack();
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1839) );
		return FALSE;
	}

	if( !mInfo.mIsBattle && pHero->GetObjectBattleState() == eObjectBattleState_Battle )
		return FALSE;

	if( !mInfo.mIsPeace && pHero->GetObjectBattleState() == eObjectBattleState_Peace )
		return FALSE;

	if( !mInfo.mIsMove && pHero->GetBaseMoveInfo()->bMoving )
		return FALSE;

	if( !mInfo.mIsStop && !pHero->GetBaseMoveInfo()->bMoving )
		return FALSE;

	if(pHero->IsNoEquip(
		mInfo.mArmorType,
		mInfo.mWeaponType,
		mInfo.mWeaponAnimationType))
	{
		pHero->SetNextAction(0);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(770));
		return FALSE;
	}

	if( mInfo.RequiredBuff )
	{
		if(FALSE == STATUSICONDLG->IsHasBuff(mInfo.RequiredBuff))
		{
			pHero->SetNextAction(NULL);
			cSkillInfo* pInfo = SKILLMGR->GetSkillInfo(mInfo.RequiredBuff);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1972), pInfo->GetName() );
			return FALSE;
		}
	}

	if(gCurTime < pHero->GetSkillDelay())
	{
		CAction NextAction;
		NextAction.InitSkillAction(this,pTarget);

		pHero->SetNextAction(&NextAction);
		return FALSE;
	}
	else if(gCurTime < GetCoolTime())
	{
		pHero->SetNextAction(
			0);

		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			CHATMGR->GetChatMsg(1083),
			float(GetCoolTime() - gCurTime) / 1000);
		CHATMGR->AddMsg(
			CTC_SYSMSG,
			text);
		return FALSE;
	}

#ifndef _TESTCLIENT_
	//////////////////////////////////////////////////////////////////////////////////
	// 06. 06. 내력소모 감소효과 체크 - 이영준
	// 클라이언트는 내력소모 감소효과 체크를 안하던 것을 추가 
	// 080616 LUJ, 퍼센트 정보도 처리하도록 한다
	DWORD NeedMana = DWORD( float( HERO->GetMana() ) * mInfo.mMana.mPercent + mInfo.mMana.mPlus );

	Status* pStatus = NULL ;
	pStatus = HERO->GetRateBuffStatus() ;
	
	if(NeedMana)
	{
		if( pStatus->DecreaseManaRate > 0 )
		{
			float fRate = 0.0f ;
			fRate = pStatus->DecreaseManaRate/100 ;

			float fRealMana = NeedMana * fRate ;
			NeedMana -= (DWORD)fRealMana ;
		}

		//if( pHero->GetMana() < GetNeedMana(SkillLevel) )		// 내력검사
		if( pHero->GetMana() < NeedMana )		// 내력검사
		{
			pHero->SetNextAction(NULL);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(656) );
			return FALSE;
		}
	}
	
	// 080616 LUJ, 퍼센트 정보도 처리하도록 한다
	DWORD NeedLife = DWORD( float( HERO->GetLife() ) * mInfo.mLife.mPercent + mInfo.mLife.mPlus );
	
	if( pHero->GetLife() < NeedLife )		// 내력검사
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1117) );
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////
#endif //_TESTCLIENT_

	if( pHero->GetState() != eObjectState_None &&
		pHero->GetState() != eObjectState_Move &&
		pHero->GetState() != eObjectState_TiedUp_CanSkill &&
		pHero->GetState() != eObjectState_Society &&
		pHero->GetState() != eObjectState_ImmortalDefense
		) 
	{

		if( pHero->GetState() != eObjectState_SkillSyn &&
			pHero->GetState() != eObjectState_SkillUsing &&
			pHero->GetState() != eObjectState_SkillBinding &&
			pHero->GetState() != eObjectState_SkillStart &&			
			pHero->GetState() != eObjectState_SkillDelay )
		{
			//			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(141) );
			pHero->DisableAutoAttack();
		}

		return FALSE;
	}

	if( pHero->GetState() == eObjectState_Society )
	{
		if( pHero->EndSocietyAct() == FALSE )
		{
			pHero->DisableAutoAttack();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL cActiveSkillInfo::IsInRange(CObject& object, CActionTarget& target) const
{
	VECTOR3* const targetPosition = target.GetTargetPosition();

	if(0 == targetPosition)
	{
		return FALSE;
	}

	VECTOR3 objectPosition = {0};
	object.GetPosition(&objectPosition);

	const float distance = abs(CalcDistanceXZ(&objectPosition, targetPosition)) - target.GetRadius();
	const float range = mInfo.Range + object.GetBonusRange();

	return range >= distance;
}

BOOL cActiveSkillInfo::IsExcutablePetSkillState(CHeroPet* pHeroPet,int SkillLevel,CActionTarget* pTarget)
{	
	BOOL chk = FALSE;
	DWORD index = 0;
	ITEM_INFO* pInfo = NULL;
	for( WORD pos = ePetEquipSlot_Skill1; pos <= ePetEquipSlot_Skill6; pos ++ )
	{
		index = pHeroPet->GetWearedItem( pos );
		pInfo = ITEMMGR->GetItemInfo( index );
		if( pInfo )
		{
			if( pInfo->SupplyType == ITEM_KIND_PET_SKILL && pInfo->SupplyValue / 100 == GetIndex() / 100 )
				chk = TRUE;
		}
	}

	if( GetIndex() == PETMGR->GetClosedSkillIndex() ||
		GetIndex() == PETMGR->GetRangeSkillIndex() )
		chk = TRUE;
		

	if( !chk )
		return FALSE;

	if( pHeroPet->GetAbnormalStatus()->IsStun ||
		pHeroPet->GetAbnormalStatus()->IsFreezing ||
		pHeroPet->GetAbnormalStatus()->IsStone ||
		pHeroPet->GetAbnormalStatus()->IsSlip ||
		pHeroPet->GetAbnormalStatus()->IsParalysis )
	{
		return FALSE;
	}

	if( pHeroPet->GetAbnormalStatus()->IsSilence )
	{
		// 080616 LUJ, 침묵 때는 물리 공격을 제외한 전체 스킬을 사용하지 못하도록 한다
		if( mInfo.Unit != UNITKIND_PHYSIC_ATTCK )
			return FALSE;
	}
	if( pHeroPet->GetAbnormalStatus()->IsBlockAttack )
	{
		if( mInfo.Unit == UNITKIND_PHYSIC_ATTCK )
			return FALSE;
	}

	if( !mInfo.mIsBattle && pHeroPet->GetObjectBattleState() == eObjectBattleState_Battle )
		return FALSE;

	if( !mInfo.mIsPeace && pHeroPet->GetObjectBattleState() == eObjectBattleState_Peace )
		return FALSE;

	if( !mInfo.mIsMove && pHeroPet->GetBaseMoveInfo()->bMoving )
		return FALSE;

	if( !mInfo.mIsStop && !pHeroPet->GetBaseMoveInfo()->bMoving )
		return FALSE;


	if( pHeroPet->GetSkillDelay()->Check() )
	{
		CAction NextAction;
		NextAction.InitSkillAction(this,pTarget);

		pHeroPet->SetNextAction(&NextAction);

		return FALSE;
	}

	if(gCurTime < mCoolTime)
	{
		float cool = (mCoolTime - gCurTime) / 1000.f;
		pHeroPet->SetNextAction(NULL);
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2006), cool );
		return FALSE;
	}

	DWORD NeedMana = DWORD( float( pHeroPet->GetMana() ) * mInfo.mMana.mPercent + mInfo.mMana.mPlus );

	if(NeedMana)
	{
		if( pHeroPet->GetMana() < NeedMana )		// 내력검사
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2005) );
			return FALSE;
		}
	}
	
	DWORD NeedLife = DWORD( float( pHeroPet->GetLife() ) * mInfo.mLife.mPercent + mInfo.mLife.mPlus );
	
	if( pHeroPet->GetLife() < NeedLife )		// 내력검사
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1117) );
		return FALSE;
	}

	if( pHeroPet->GetState() != eObjectState_None &&
		pHeroPet->GetState() != eObjectState_Move &&
		pHeroPet->GetState() != eObjectState_TiedUp_CanSkill &&
		pHeroPet->GetState() != eObjectState_Society
		) 
	{

		if( pHeroPet->GetState() != eObjectState_SkillSyn &&
			pHeroPet->GetState() != eObjectState_SkillUsing &&
			pHeroPet->GetState() != eObjectState_SkillBinding &&
			pHeroPet->GetState() != eObjectState_SkillStart &&			
			pHeroPet->GetState() != eObjectState_SkillDelay )
		{
			pHeroPet->DisableAutoAttack();
		}

		return FALSE;
	}

	return TRUE;
}

BOOL cActiveSkillInfo::IsValidTarget(CHero* pHero,CActionTarget* pTarget)
{
	CBattle* pBattle = BATTLESYSTEM->GetBattle();
	CObject* pTargetObj = OBJECTMGR->GetObject( pTarget->GetTargetID() );

	if(pTargetObj == NULL)
		return FALSE;

	switch( pTargetObj->GetObjectKind() )
	{
	case eObjectKind_Npc:
	case eObjectKind_Vehicle:
		return FALSE;
	case eObjectKind_Monster:
	case eObjectKind_BossMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_FieldSubMonster:
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_Trap:
		{
			if( ( ( CMonster* )pTargetObj )->GetSInfo()->SpecialType == 2 )
				return FALSE;
		}
		break;
	}

	// 100615 ShinJS --- 부활스킬을 제외한 스킬의 타겟이 죽은 경우
	if( mInfo.Unit != UNITKIND_RESURRECTION &&
		pTargetObj->IsDied() )
		return FALSE;

	if( !mInfo.Area )
	{
		switch( mInfo.Unit )
		{
		case UNITKIND_PHYSIC_ATTCK:
		case UNITKIND_MAGIC_ATTCK:
		case UNITKIND_FAKE_DAMAGE:
		case UNITKIND_PHYSIC_FAKE_DAMAGE:
		case UNITKIND_MAGIC_FAKE_DAMAGE:
		case UNITKIND_DECOY:
			{
				if( pBattle->IsEnemy(pHero,pTargetObj) == FALSE )
					return FALSE;
			}	
			break;
		case UNITKIND_LIFE_RECOVER:
			{
				if( pBattle->IsEnemy(pHero,pTargetObj) == TRUE )
					return FALSE;
			}
			break;
		case UNITKIND_RESURRECTION:
			{
				if( pBattle->IsEnemy(pHero,pTargetObj) == TRUE )
					return FALSE;
				if (g_csDateManager.IsChallengeZoneHere())
				{
					return FALSE;
				}
				// 080317 LUJ, 비무 중일 때 사용할 수 없음
				else if( SHOWDOWNMGR->IsShowdown() )
				{
					return FALSE;
				}
			}
			break;
		// E 데이트 존 추가 added by hseos 2008.01.16
		}
	}
	else
	{
		switch( mInfo.Target )
		{
		case eSkillTargetKind_OnlyOthers:
		case eSkillTargetKind_AllObject:
			{
				switch( mInfo.Unit )
				{
				case UNITKIND_PHYSIC_ATTCK:
				case UNITKIND_MAGIC_ATTCK:
				case UNITKIND_FAKE_DAMAGE:
				case UNITKIND_PHYSIC_FAKE_DAMAGE:
				case UNITKIND_MAGIC_FAKE_DAMAGE:
				case UNITKIND_DECOY:
					{
						if( pBattle->IsEnemy(pHero,pTargetObj) == FALSE )
							return FALSE;
					}	
					break;
				case UNITKIND_LIFE_RECOVER:
				case UNITKIND_RESURRECTION:
					{
						if( pBattle->IsEnemy(pHero,pTargetObj) == TRUE )
							return FALSE;
						// desc_hseos_데이트 존_01
						// S 데이트 존 추가 added by hseos 2007.12.03
						// ..챌린지 존에서는 부활 불가
						if (g_csDateManager.IsChallengeZoneHere())
						{
							return FALSE;
						}
						// E 데이트 존 추가 added by hseos 2007.12.03
						// 080317 LUJ, 비무 중일 때 사용할 수 없음
						else if( SHOWDOWNMGR->IsShowdown() )
						{
							return FALSE;
						}
					}
					break;
				}
			}
			break;
		}
	}

	return TRUE;
}

void cActiveSkillInfo::SetCooling()
{
	mCoolTime = gCurTime + mInfo.CoolTime;
}

DWORD cActiveSkillInfo::GetBuffSkillIdx(int index)
{
	DWORD dwBuffSkillIdx = 0 ;

	if( index < 0 || index >= MAX_BUFF_COUNT )
	{
		return dwBuffSkillIdx ;
	}

	return mInfo.Buff[index] ;
}