#include "stdafx.h"
#include ".\heropet.h"
#include "ObjectManager.h"
#include "PetManager.h"
#include "MoveManager.h"
#include "GameIn.h"
#include "PetWearedDialog.h"
#include "PetInfoDialog.h"
#include "ItemManager.h"
#include "Item.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "ObjectGuagen.h"
#include "WindowIdEnum.h"
#include "cStatic.h"
#include "chatmanager.h"
#include "conductmanager.h"
#include "petstatedialog.h"
#include "petstatusicondialog.h"
#include "CommonCalcFunc.h"

CHeroPet::CHeroPet(void)
{
	mSkillIndex = 0;
}

CHeroPet::~CHeroPet(void)
{
	GAMEIN->GetPetInfoDlg()->SetActive( FALSE );

	for( WORD i = 0; i < ePetEquipSlot_Max; ++i )
	{
		cIcon* pIcon = NULL;
		
		GAMEIN->GetPetWearedDlg()->DeleteItem( i, &pIcon );

		if( !pIcon )
		{
			continue;
		}

		ITEMMGR->ItemDelete( ( CItem* )pIcon );
	}

	Release();
}

void CHeroPet::InitPet( PET_OBJECT_INFO* pPetObjectInfo ) 
{
	CPet::InitPet( pPetObjectInfo );

	mItemDBIdx = pPetObjectInfo->ItemDBIdx;
	mSkillSlot = pPetObjectInfo->SkillSlot;
	mExp = pPetObjectInfo->Exp;
	mFriendly = pPetObjectInfo->Friendly;
	mAI = pPetObjectInfo->AI;
	mType = pPetObjectInfo->Type;
	mMP = pPetObjectInfo->MP;
	mMaxMP = pPetObjectInfo->MAXMP;

	memset( &mPetStat, 0, sizeof( PlayerStat ) );
	memset( &mItemStat, 0, sizeof( PlayerStat ) );
	
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );
	mSkillIndex = 0;
	
	m_MovingAction.Clear();
	m_NextAction.Clear();

	mDistanceCheckTime = 0;

	PETMGR->SetLastSummonTime();
	GAMEIN->GetPetUIDlg()->SetActive( TRUE );

	CItem* pItem = ITEMMGR->GetItem( mItemDBIdx );
	pItem->SetLock( TRUE );

	cStatic* name = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_NAME);
	if( name )	name->SetStaticText( GetObjectName() );	
	cStatic* level = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LEVEL);
	if( level )	level->SetStaticValue( mLevel );	

	SetMaxLife( mMaxHP );
	SetMaxMana( mMaxMP );
	SetExp( mExp );
	SetFriendly( mFriendly );

	VECTOR2 sidpos;
	sidpos.x = 0;
	sidpos.y = 0;
	PETSTATUSICONDLG->Init(HERO,&sidpos,8);
}

void CHeroPet::Release()
{
	CItem* pItem = ITEMMGR->GetItem( mItemDBIdx );
	if( pItem ) pItem->SetLock( FALSE );

	CPet::Release();
	
	memset( &mPetStat, 0, sizeof( PlayerStat ) );
	memset( &mItemStat, 0, sizeof( PlayerStat ) );
	
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	mDistanceCheckTime = 0;
	
	mHP = 0;
	mMP = 0;

	GAMEIN->GetPetUIDlg()->SetActive( FALSE );
	
	PETMGR->SetLastSummonTime();
	PETSTATUSICONDLG->Release();
}

void CHeroPet::CalcStats()
{
	memset( &mPetStat, 0, sizeof( PlayerStat ) );
	memset( &mItemStat, 0, sizeof( PlayerStat ) );	
	memset( &mItemOptionStat, 0, sizeof( PlayerStat ) );

	PET_STATUS_INFO* pInfo = PETMGR->GetPetStatusInfo( mType, mLevel );

	if( !pInfo )
		return;

	for( WORD i = 0; i < ePetEquipSlot_Max; ++i )
	{
		if( mWearedItem[ i ] == 0 )
			continue;

		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( mWearedItem[ i ] );

		CPetWearedDialog* pDlg = GAMEIN->GetPetWearedDlg();

		CItem* pItem= (CItem*)pDlg->GetIconForIdx(i) ;
		if(! pItem )
			continue;

		const ITEMBASE* pItemBase = &pItem->GetItemBaseInfo();
		if(! pItemBase )
			continue;

		// 090625 pdy 펫 기간제 아이템 착용처리 추가
		// 090713 pdy 조건 채크를 eITEM_TYPE_SEAL로 수정 
		if( eITEM_TYPE_SEAL == pItemBase->nSealed )
			continue;

		mItemStat.mPhysicAttack.mPlus += pInfo->PhysicAttack;
		mItemStat.mPhysicDefense.mPlus += pInfo->PhysicDefense;
		mItemStat.mMagicAttack.mPlus += pInfo->MagicAttack;
		mItemStat.mMagicDefense.mPlus += pInfo->MagicDefense;
		mItemStat.mStrength.mPlus += pInfo->ImprovementStr;
		mItemStat.mDexterity.mPlus += pInfo->ImprovementDex;
		mItemStat.mVitality.mPlus += pInfo->ImprovementVit;
		mItemStat.mIntelligence.mPlus += pInfo->ImprovementInt;
		mItemStat.mWisdom.mPlus += pInfo->ImprovementWis;
		mItemStat.mLife.mPlus += pInfo->ImprovementLife;
		mItemStat.mMana.mPlus += pInfo->ImprovementMana;

		PETMGR->AddPetItemOptionStat( pItem->GetItemBaseInfo() );
	}
	
	mPetStat.mStrength.mPlus		= ( pInfo->STR * ( 1 + mRatePassiveStatus.Str + mRateBuffStatus.Str + mItemStat.mStrength.mPercent ) ) +
									  ( mPassiveStatus.Str + mBuffStatus.Str + mItemStat.mStrength.mPlus );	

	mPetStat.mDexterity.mPlus		= ( pInfo->DEX * ( 1 + mRatePassiveStatus.Dex + mRateBuffStatus.Dex + mItemStat.mDexterity.mPercent ) ) +  
									  ( mPassiveStatus.Dex + mBuffStatus.Dex + mItemStat.mDexterity.mPlus );

	mPetStat.mVitality.mPlus		= ( pInfo->VIT * ( 1 + mRatePassiveStatus.Vit + mRateBuffStatus.Vit + mItemStat.mVitality.mPercent ) ) + 
								      ( mPassiveStatus.Vit + mBuffStatus.Vit + mItemStat.mVitality.mPlus );

	mPetStat.mIntelligence.mPlus	= ( pInfo->INT * ( 1 + mRatePassiveStatus.Int + mRateBuffStatus.Int + mItemStat.mIntelligence.mPercent ) ) + 
									  ( mPassiveStatus.Int + mBuffStatus.Int + mItemStat.mIntelligence.mPlus );

	mPetStat.mWisdom.mPlus			= ( pInfo->WIS * ( 1 + mRatePassiveStatus.Wis + mRateBuffStatus.Wis + mItemStat.mWisdom.mPercent ) ) +
	                                  ( mPassiveStatus.Wis + mBuffStatus.Wis + mItemStat.mWisdom.mPlus );


	PET_STATUS_INFO*	pFirstInfo	=	PETMGR->GetPetStatusInfo( ePetType_Basic, 1 );
	float				fAccuracyVal	=	11;
	float fPetLevel			=	(float)GetLevel();	

	{// 공격력
					
	//	최대 물리 공격력	= (  ( 무기 공격력 + 레벨 ) * ( 1 + ( 힘 * 0.001 ) ) + ( 순수힘 - 30 ) )						

		float fPhysicAttackMax	=	( mItemStat.mPhysicAttack.mPlus + fPetLevel ) * ( 1.0f + ( mPetStat.mStrength.mPlus * 0.001f ) ) + 
									( pInfo->STR - 30.0f );
	
	//	최소 물리 공격력	= 스탯 → ( 힘 / 2.6 + 민첩 / 1.6 )		
		float fPhysicAttackMin	=	( mPetStat.mStrength.mPlus / 2.6f + mPetStat.mDexterity.mPlus / 1.6f );

		if( fPhysicAttackMax < fPhysicAttackMin )
		{
			m_fPhysicAttackMax	=	fPhysicAttackMax;
			m_fPhysicAttackMin	=	fPhysicAttackMax;
		}
		else
		{
			m_fPhysicAttackMax	=	fPhysicAttackMax;
			m_fPhysicAttackMin	=	fPhysicAttackMin;
		}

		mPetStat.mPhysicAttack.mPlus	=	CalcPhysicAttack( m_fPhysicAttackMax );
	}

	{// 방어력
	//	물리 방어력	= ( ( 장비 방어력 + 레벨 ) * ( 1 + ( 건강 / 3000 ) ) + ( (순수건강 - 40) / 5 ) )							
	
		float	fResult	=	( mItemStat.mPhysicDefense.mPlus + fPetLevel ) * ( 1 + ( mPetStat.mVitality.mPlus / 3000.0f ) ) + 
							( ( pInfo->VIT - 40.0f ) / 5.0f );

		if( fResult <= 0.0f )
		{
			mPetStat.mPhysicDefense.mPlus	=	0.0f;
		}
		else
		{
			//최종 물리 방어력 = (물리 방어력 * ( 1 + [%]증가옵션)) + [+]증가옵션			
			
			// % 옵션
			float	fPercent	=	( mRatePassiveStatus.PhysicDefense + 
									mRateBuffStatus.PhysicDefense    +
									mItemOptionStat.mPhysicDefense.mPercent ) * 0.01f; 

			float	fPlus		=	mPassiveStatus.PhysicDefense +
									mBuffStatus.PhysicDefense    + 
									mItemOptionStat.mPhysicDefense.mPlus;

			fResult		=	( fResult * ( 1.0f + fPercent) ) + fPlus;


			mPetStat.mPhysicDefense.mPlus	=	fResult;
		}
	}

	{// 마법 공격력
	//	최대 마법 공격력	= (  ( 무기 공격력 + 레벨 ) * ( 1 + (지능 * 0.001) ) + ( 순수지능 - 40 ) )						

		float fMagicAttackMax	=	( mItemStat.mMagicAttack.mPlus + fPetLevel ) * ( 1.0f + ( mPetStat.mIntelligence.mPlus * 0.001f ) ) + 
									( pInfo->INT - 40.0f );

	//	최소 마법 공격력	= 스탯 → ( 지능 / 2.6 + 정신 / 1.6 )		

		float fMagicAttackMin	=	mPetStat.mIntelligence.mPlus / 2.6f + mPetStat.mWisdom.mPlus / 1.6f;

		if( fMagicAttackMax < fMagicAttackMin )
		{
			m_fMagicAttackMax	=	fMagicAttackMax;
			m_fMagicAttackMin	=	fMagicAttackMax;
		}
		else
		{
			m_fMagicAttackMax	=	fMagicAttackMax;
			m_fMagicAttackMin	=	fMagicAttackMin;
		}

		mPetStat.mMagicAttack.mPlus	=	CalcMagicAttack( m_fMagicAttackMax );
	}

	{// 마법 방어력
	//	마법 방어력	= ( ( 장비 방어력 + 레벨 ) * ( 1 + ( 정신 / 3000 ) ) + ( 순수정신 / 5 ) )						

		float fMagDefenseAll	=	mItemStat.mMagicDefense.mPlus;

		float	fResult	=	( fMagDefenseAll + fPetLevel ) * ( 1.0f + ( mPetStat.mWisdom.mPlus / 3000.0f ) ) + ( pInfo->WIS / 5.0f );

		if( fResult	<= 0 )
		{
			mPetStat.mMagicDefense.mPlus	=	0.0f;
		}
		else
		{
			//최종 마법 방어력 = (마법 방어력 * ( 1 + [%]증가옵션)) + [+]증가옵션					

			// % 옵션
			float	fPercent	=	( mRatePassiveStatus.MagicDefense +
									mRateBuffStatus.MagicDefense    +
									mItemOptionStat.mMagicDefense.mPercent ) * 0.01f;

			float	fPlus		=	mPassiveStatus.MagicDefense + 
									mBuffStatus.MagicDefense    +
									mItemOptionStat.mMagicDefense.mPlus;

			fResult		=	( fResult	* ( 1.0f + fPercent ) ) + fPlus;

			mPetStat.mMagicDefense.mPlus	=	fResult;
		}
	}

	{// 명중
		//	명중 수치	= (  ( ( 민첩 - 최초 민첩 ) / (  ( 레벨 - 1 ) * 5 ) ) * 0.6 ) + (  ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 )											

		float fRightVal	=	mPetStat.mDexterity.mPlus - pFirstInfo->DEX;
		float fLeftVal	=	fPetLevel - 1.0f;

		float fRootVal	=	0.0f;
		float fPetDex	=	(float)pFirstInfo->DEX;

		if( fLeftVal )
		{
			fLeftVal	=	fLeftVal * 5.0f;
			fRootVal	=	fRightVal / fLeftVal;
		}
		else
		{
			fRootVal	=	0.0f;
		}

		float fAccuracyResult	=	( fRootVal * 0.6f ) + 
									( ( fPetDex - 30.0f ) / 1000.0f ) + 
									( fPetLevel * fAccuracyVal / 15000.0f );

		// + 옵션
		float	fPlus	=	mPassiveStatus.Accuracy +
							mBuffStatus.Accuracy	+
							mItemOptionStat.mAccuracy.mPlus;

		fAccuracyResult	=	fAccuracyResult * 100.0f;

		fAccuracyResult	+=	fPlus;

		mPetStat.mAccuracy.mPlus	=	fAccuracyResult;
	}

	{// 회피
		//회피 수치	= (  ( 민첩 - 최초 민첩 ) / (  ( 레벨 - 1 ) * 5 ) * 0.6 ) + (  ( 최초 민첩 - 30 ) / 1000 ) + ( 레벨 * 명중상수 / 15000 )											

		float fRightVal	=	mPetStat.mDexterity.mPlus - pFirstInfo->DEX;
		float fLeftVal	=	fPetLevel - 1.0f;

		float fRootVal		=	0.0f;
		float fPetDex	=	(float)pFirstInfo->DEX;

		if( fLeftVal )
		{
			fLeftVal	=	fLeftVal * 5.0f;
			fRootVal	=	fRightVal / fLeftVal;
		}
		else
		{
			fRootVal	=	0.0f;
		}


		float fEvadeResult	=	( fRootVal * 0.6f ) +
								( ( fPetDex - 30.0f ) / 1000.0f ) + 
								( fPetLevel *  fAccuracyVal / 15000.0f );

		float	fPlus	=	mPassiveStatus.Avoid +
							mBuffStatus.Avoid	 +
							mItemOptionStat.mEvade.mPlus;

		fEvadeResult	=	fEvadeResult * 100.0f;

		fEvadeResult	+=	fPlus;

		mPetStat.mEvade.mPlus	=	fEvadeResult;
	}

	SetMaxLife( mMaxHP );
	SetMaxMana( mMaxMP );

	GAMEIN->GetPetStateDlg()->SetInfo();
}

#define SKILL_PERSUIT_TICK	1000

void CHeroPet::Process()
{
	CObject::Process();

	if( IsShocked() )
		return;
	
	static DWORD	dwSkillPersuitTime	= 0;

	float distance = 0.f;

	if( gCurTime - mDistanceCheckTime > PETMGR->GetDistanceCheckTime() )
	{
		VECTOR3 HeroPos;
		VECTOR3 PetPos;

		HERO->GetPosition(&HeroPos);
		GetPosition(&PetPos);
		distance = CalcDistanceXZ(&HeroPos, &PetPos); 

		mDistanceCheckTime = gCurTime;
	}

	if( distance > PETMGR->GetMaxDistance() )
	{
		SetMovingAction(NULL);
		SetNextAction(NULL);
		
		Recall();
		return;
	}

	if( mAI == ePetAI_Stand )
	{
		SetMovingAction(NULL);
		SetNextAction(NULL);
		// 091214 ONS 펫 대기상태일경우 공격을 멈추도록 처리.
		DisableAutoAttack();
		return;
	}

	if(m_MovingAction.HasAction())
	{
		if(m_MovingAction.CheckTargetDistance(&GetCurPosition()) == TRUE)
		{
			MOVEMGR->HeroPetMoveStop();

			m_MovingAction.ExcutePetAction(this);
			m_MovingAction.Clear();

			return;
		}
		else
		{
			if( m_MovingAction.GetActionKind() == eActionKind_Skill )
			{
				if( gCurTime - dwSkillPersuitTime > SKILL_PERSUIT_TICK )
				{
					m_MovingAction.ExcutePetAction(this);
					dwSkillPersuitTime = gCurTime;
				
					return;
				}
			}
			else if( MOVEMGR->IsMoving(this) == FALSE )
			{
				m_MovingAction.Clear();
			}
		}
	}
	else
	{
		if(m_bIsAutoAttacking)
		{
			if(m_NextAction.HasAction() == FALSE)
			{
				if(SKILLMGR->OnPetSkillCommand(this,&m_AutoAttackTarget,FALSE) == FALSE)
				{
					DisableAutoAttack();
				}			
			}
			else
			{
				if(CObject* const targetObject = OBJECTMGR->GetObject(m_NextAction.GetTargetID()))
				{
					CObject* const ownerObject = OBJECTMGR->GetObject(
						targetObject->GetOwnerIndex());

					if(ownerObject &&
						eObjectKind_Player == ownerObject->GetObjectKind())
					{
						SetNextAction(
							0);
						return;
					}
				}

				m_NextAction.ExcutePetAction(this);
				return;
			}
		}
	}


	switch( mAI )
	{
	case ePetAI_Aggressive:
		{
			if( !SetTarget() )
			{
				if( distance > PETMGR->GetAreaDistance() )
				{
					if( MOVEMGR->IsMoving( this ) )
					{
						MOVEMGR->HeroPetMoveStop();
					}

					MOVEMGR->SetHeroPetTarget();
				}		
			}
		}
		break;
	case ePetAI_Support:
		{
			// 100630 ONS 공격상태의 펫이동중 이동타겟을 변경하지 않도록 수정.
			if( !HERO->IsAutoAttacking() )
			{
				if( distance > PETMGR->GetAreaDistance() )
				{
					if( MOVEMGR->IsMoving( this ) )
					{
						MOVEMGR->HeroPetMoveStop();
					}
					MOVEMGR->SetHeroPetTarget();
				}

				if( IsAutoAttacking() )
				{
					// 091216 ONS 펫AI가 서포트일때, HERO가 공격상태가 아닐경우 공격을 중지시킨다.
					SetMovingAction(NULL);
					SetNextAction(NULL);
					DisableAutoAttack();	
				}
			}
		}
		break;
	case ePetAI_Stand:
		{
		}
		break;
	}
}

void CHeroPet::Recall()
{
	MSGBASE msg;

	msg.Category = MP_PET;
	msg.Protocol = MP_PET_RECALL_SYN;
	msg.dwObjectID = HEROID;

	NETWORK->Send( &msg, sizeof( msg ) );
}

DWORD CHeroPet::SetTarget()
{
	VECTOR3 HeroPos;
	VECTOR3 PetPos;

	HERO->GetPosition(&HeroPos);
	GetPosition(&PetPos);

	DWORD id = OBJECTMGR->GetSingleTargetInRange(
		&PetPos,
		float(mpPetInfo->Sight),
		0,
		1);

	if( id )
	{	
		CObject* pTargetObj = OBJECTMGR->GetObject( id );
		CActionTarget Target;
		Target.InitActionTarget(pTargetObj,NULL);

		EnableAutoAttack(&Target);		
		SKILLMGR->OnPetSkillCommand(this, &Target, FALSE);
	}
	return id;
}


void CHeroPet::SetTarget( DWORD id )
{
	CObject* pTargetObj = OBJECTMGR->GetObject(id);

	if(0 == pTargetObj)
	{
		return;
	}
	else if(CObject* const ownerObject = OBJECTMGR->GetObject(pTargetObj->GetOwnerIndex()))
	{
		if(eObjectKind_Player == ownerObject->GetObjectKind())
		{
			return;
		}
	}

	CActionTarget Target;
	Target.InitActionTarget(pTargetObj,NULL);

	EnableAutoAttack(&Target);		

	if( mAI != ePetAI_Stand )
		SKILLMGR->OnPetSkillCommand(this, &Target, FALSE);
}

void CHeroPet::Attack()
{
	MSGBASE msg;

	msg.Category = MP_PET;
	msg.Protocol = MP_PET_ATTACK_SYN;
	msg.dwObjectID = HEROID;

	NETWORK->Send( &msg, sizeof( msg ) );
}

void CHeroPet::UseSkill()
{
	MSG_DWORD msg;

	msg.Category = MP_PET;
	msg.Protocol = MP_PET_SKILL_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData = mSkillIndex;

	NETWORK->Send( &msg, sizeof( msg ) );

	mSkillIndex = 0;
}


void CHeroPet::AddExp()
{
	SetExp( mExp + 1 ); 
}
	
void CHeroPet::SetExp( WORD exp )
{
	mExp = exp;

	// 091209 ShinJS --- 변경된 정보 갱신
	GAMEIN->GetPetStateDlg()->SetInfo();
}

void CHeroPet::SetWearedItem( ITEMBASE* pItemInfo )
{
	CPet::SetWearedItem( pItemInfo );

	for( WORD i = 0; i < ePetEquipSlot_Max; ++i )
	{
		if( pItemInfo[ i ].dwDBIdx == 0 )
		{
			continue;
		}

		CItem * newItem = ITEMMGR->MakeNewItem( &pItemInfo[ i ], "AddItem" );
		if(newItem == NULL)
		{
			continue;
		}

		GAMEIN->GetPetWearedDlg()->AddItem( i, ( cIcon* )newItem );
	}

	CalcStats();
}

void CHeroPet::SetMaxLife(DWORD maxlife)
{
	CPet::SetMaxLife( maxlife );

	((CObjectGuagen*)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LIFE_GUAGE))->SetValue(
		(float)mHP/(float)mMaxHP, 0 );

	char szValue[50];
	sprintf( szValue, "%d/%d", mHP, mMaxHP );

	cStatic* ps = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LIFE_STATIC);
	if( ps )	ps->SetStaticText( szValue );	
}

void CHeroPet::SetLife(DWORD life, BYTE type)
{
	if(GetState() == eObjectState_Die)
		life = 0;
	
	CPet::SetLife( life );

	((CObjectGuagen*)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LIFE_GUAGE))->SetValue(
		(float)mHP/(float)mMaxHP, 
		(type == 0 ? 0 : (2000/mMaxHP)*mHP) );

	char szValue[50];
	sprintf( szValue, "%d/%d", mHP, mMaxHP );
	cStatic* ps = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LIFE_STATIC);
	if( ps )	ps->SetStaticText( szValue );
}


void CHeroPet::SetMaxMana(DWORD val)
{
	mMaxMP = val;

	((CObjectGuagen*)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_MANA_GUAGE))->SetValue(
		(float)mMP/(float)mMaxMP, 0 );

	char szValue[50];
	sprintf( szValue, "%d/%d", mMP, mMaxMP );

	cStatic* ps = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_MANA_STATIC);
	if( ps )	ps->SetStaticText( szValue );
}

void CHeroPet::SetMana(DWORD val, BYTE type)
{
	mMP = val;

	((CObjectGuagen*)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_MANA_GUAGE))->SetValue(
		(float)mMP/(float)mMaxMP, 
		( type == 0 ? 0 : (1000/mMaxMP)*mMP) );
	char szValue[50];
	sprintf( szValue, "%d/%d", mMP, mMaxMP );
	cStatic* ps = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_MANA_STATIC);
	if( ps )	ps->SetStaticText( szValue );
}

void CHeroPet::SetLevel( BYTE level )	
{ 
	mLevel = level; 
	cStatic* plevel = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_LEVEL);
	if( plevel )	plevel->SetStaticValue( mLevel );	
	SetExp( 0 ); 
}

void CHeroPet::LevelUp()	
{ 
	SetLevel( ++mLevel ); 
}

void CHeroPet::SetFriendly( BYTE byte ) 
{ 
	PET_FRIENDLY_STATE* pFriendlyState = PETMGR->GetFriendlyStateTable( byte );

	if( pFriendlyState )
	{
		BYTE rate = ( BYTE )random( 1, 100 );

		if( rate > BYTE(pFriendlyState->SpeechRate) )
			rate = rate - BYTE(pFriendlyState->SpeechRate);
		else
		{
			cPtrList* pList = PETMGR->GetPetSpeechInfoTable( pFriendlyState->SpeechIndex );

			if( pList )
			{
				char* speech = NULL;

				PTRLISTPOS pos = pList->GetHeadPosition();
				while(pos)
				{
					speech = (char *)pList->GetNext(pos);
				}

				OBJECTMGR->ShowChatBalloon( this, speech );
			}
		}

		if( rate > pFriendlyState->EmoticonRate )
			rate = rate - pFriendlyState->EmoticonRate;
		else
		{
			cPtrList* pList = PETMGR->GetPetEmoticonInfoTable( pFriendlyState->EmoticonIndex );

			if( pList )
			{
 				DWORD* emoticon = NULL;

				PTRLISTPOS pos = pList->GetHeadPosition();
				while(pos)
				{
					emoticon = (DWORD *)pList->GetNext(pos);
				}

				if( emoticon )
					CONDUCTMGR->PetEmoticonCommand( *emoticon );
			}

		}
	}

	mFriendly = byte; 

	// 091209 ShinJS --- 펫 UI 수정(경험치↔친밀도)
	((CObjectGuagen*)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_FRIENDLY_GUAGE))->SetValue( ((float)mFriendly / 100.0f), 0 );
	char szValue[50];
	sprintf( szValue, "%d%%", mFriendly );
	cStatic* pFriendlyStatic = (cStatic *)GAMEIN->GetPetUIDlg()->GetWindowForID(PET_UI_FRIENDLY_STATIC);
	if( pFriendlyStatic )	pFriendlyStatic->SetStaticText( szValue );
}

DWORD CHeroPet::GetRealSkillIndex( DWORD index )
{
	DWORD level = 0;

	for( BYTE slot = ePetEquipSlot_Skill1; slot <= ePetEquipSlot_Skill6; slot++ )
	{
		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( mWearedItem[ slot ] );

		if( pInfo )
		{
			DWORD skillindex = pInfo->SupplyValue;

			if( skillindex / 100 == index / 100 )
			{
				level += ( skillindex % 100 );
			}
		}
	}

	return ( index / 100 ) * 100 + level;
}

void CHeroPet::ClearTarget(CObject* pObject)
{
	if(m_AutoAttackTarget.GetTargetID() == pObject->GetID())
		DisableAutoAttack();
	if(m_NextAction.GetTargetID() == pObject->GetID())
	{
		m_NextAction.Clear();
	}
}

void CHeroPet::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Enter:
		{
		}
		break;
	case eObjectState_Deal:
		{
		}
		break;

	case eObjectState_SkillUsing:
	case eObjectState_SkillBinding:
		SKILLMGR->OnPetComboTurningPoint(this);
		break;

	case eObjectState_SkillSyn:
		{
			SKILLMGR->OnPetComboTurningPoint(this);
		}
		break;

	case eObjectState_Move:
		{
		}
		break;
	case eObjectState_SkillStart:
		{
		}
		break;
	}
}

float	CHeroPet::CalcPhysicAttack( float fAttackPoint )
{
	//최종 물리 공격력 = (물리 공격력 * ( 1 + [%]증가옵션)) + [+]증가옵션					

	float	fPhysicAttack	=	0.0f;

	// % 옵션
	float	fPhysicPercent		=	( mRatePassiveStatus.PhysicAttack + 
									mRateBuffStatus.PhysicAttack    +
									mItemOptionStat.mPhysicAttack.mPercent ) * 0.01f;

	// + 옵션
	float fPhysicPlus			=	mPassiveStatus.PhysicAttack +
									mBuffStatus.PhysicAttack    +
									mItemOptionStat.mPhysicAttack.mPlus;

	fPhysicAttack	=	( fAttackPoint * ( 1 + fPhysicPercent ) ) + fPhysicPlus;

	return	fPhysicAttack;
}

float	CHeroPet::CalcMagicAttack( float fAttackPoint )
{
	//최종 마법 공격력 = (마법 공격력 * ( 1 + [%]증가옵션)) + [+]증가옵션					

	float fMagicAttack	=	0.0f;

	// % 옵션
	float fMagicPercent	=	( mRatePassiveStatus.MagicAttack +
							mRateBuffStatus.MagicAttack    + 
							mItemOptionStat.mMagicAttack.mPercent ) * 0.01f;

	// + 옵션
	float fMagicPlus	=	mPassiveStatus.MagicAttack +
							mBuffStatus.MagicAttack	   +
							mItemOptionStat.mMagicAttack.mPlus;

	fMagicAttack		=	 ( fAttackPoint * ( 1 + fMagicPercent ) ) + fMagicPlus;

	return	fMagicAttack;
}
