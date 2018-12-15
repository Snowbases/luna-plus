#include "stdafx.h"
#include ".\pet.h"
#include "Player.h"
#include "petmanager.h"
#include "PackedData.h"
#include "itemmanager.h"
#include "Monster.h"
#include "StateMachinen.h"
#include "CharMove.h"
#include "MHError.h"
#include "ObjectStateManager.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "CharacterCalcManager.h"
#include "..\[cc]header\GameResourceManager.h"
#include "Trigger\Manager.h"

CPet::CPet(void)
{
	m_FollowMonsterList.Initialize(5);
	m_byGod = FALSE ;
}

CPet::~CPet(void)
{}

BOOL CPet::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	memset( mItemSlot, 0, sizeof( ITEMBASE ) * ePetEquipSlot_Max );

	memset( &mPassiveStatus, 0, sizeof( Status ) );
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRatePassiveStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	mCheckTime = gCurTime;
	mbBuffSkillLoad = FALSE;

	return CObject::Init( kind, AgentNum, pBaseObjectInfo );
}

void CPet::Release()
{
//---KES Aggro 070918
//---어그로 리스트 릴리스
	RemoveAllAggroed();
//-------------------
	CMonster * pObject = NULL;
	m_FollowMonsterList.SetPositionHead();
	while((pObject = (CMonster *)m_FollowMonsterList.GetData())!= NULL)
	{
		pObject->SetTObject(NULL);
	}
	m_FollowMonsterList.RemoveAll();

	CObject::Release();
}

void CPet::UpdateGravity()
{
	mGravity = 0;
	m_FollowMonsterList.SetPositionHead();

	while(CObject* const pObject = m_FollowMonsterList.GetData())
	{
		mGravity += pObject->GetGravity();
	}
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CPet::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
{
	static MSG_PET_ADD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_PET_ADD;
	// 090629 LUJ, 오브젝트 ID가 없으면 에이전트 서버는 메시지를 전송하지 않는다. 필요없어도 설정해준다
	message.dwObjectID = dwReceiverID;
	message.MasterIdx = GetOwnerIndex();
	message.PetObjectInfo = mpPetObjectInfo;
	message.BaseObjectInfo = m_BaseObjectInfo;
	memcpy(
		message.ItemInfo,
		mItemSlot,
		sizeof(mItemSlot));
	GetSendMoveInfo(&message.MoveInfo,&message.AddableInfo);
	message.IsLogin = isLogin;
	// 091214 ONS 펫 맵이동시 소환 메세지관련 추가.
	message.IsSummoned = this->GetSummonFlag();
 
	sendMessage = &message;
	return message.GetMsgLength();
}

float CPet::DoGetMoveSpeed()	
{
	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	return 0 == ownerObject ? 0 : ownerObject->DoGetMoveSpeed();
}

void CPet::AddExp()
{
	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(0 == ownerObject)
	{
		return;
	}

	if( mpPetObjectInfo.Level > ownerObject->GetLevel() )
		return;

	if( mpPetObjectInfo.Level >= PETMGR->GetMaxLevel() )
		return;

	if( PETMGR->GetGradeUpLevel( mpPetObjectInfo.Grade ) && mpPetObjectInfo.Level >= PETMGR->GetGradeUpLevel( mpPetObjectInfo.Grade ) )
		return;

	WORD exp = mpPetObjectInfo.Exp + 1;

	if( exp >= PETMGR->GetMaxEXP( mpPetObjectInfo.Grade ) )
	{
		LevelUp();
	}
	else
	{
		mpPetObjectInfo.Exp = exp;

		MSG_WORD msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_EXP_NOTIFY;
		msg.wData = exp;

		ownerObject->SendMsg( &msg, sizeof( msg ) );
	}
}

void CPet::LevelUp()
{
	mpPetObjectInfo.Exp = 0;
	++mpPetObjectInfo.Level;

	CalcStats();
	DBInfoUpdate();
	LogPet(
		mpPetObjectInfo,
		ePetLogChangeLevel);

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORD msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_LEVELUP_NOTIFY;
		msg.dwObjectID = GetOwnerIndex();
		msg.dwData = GetID();

		PACKEDDATA_OBJ->QuickSend(
			ownerObject,
			&msg,
			sizeof(msg));
	}
}

void CPet::SetLevel( BYTE level )
{
	mpPetObjectInfo.Exp = 0;
	mpPetObjectInfo.Level = level;

	CalcStats();
	DBInfoUpdate();

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORDBYTE msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_LEVEL_NOTIFY;
		msg.dwObjectID = GetOwnerIndex();
		msg.dwData = GetID();
		msg.bData = level;

		PACKEDDATA_OBJ->QuickSend(
			ownerObject,
			&msg,
			sizeof(msg));
	}
}

void CPet::GradeUp()
{
	if(PETMGR->GetMaxGrade() == mpPetObjectInfo.Grade)
		return;

	const PET_INFO* const pPetInfo = PETMGR->GetInfo(mpPetObjectInfo.Kind);

	// 100719 ONS 업그레이드시 펫스킬슬롯 추가 처리
	const DWORD MaxPetSkillSlotCount = 6;
	const DWORD dwPetSkillSlotCount = (pPetInfo->StaticItemIndex ? MaxPetSkillSlotCount - 1 : MaxPetSkillSlotCount);
	if( mpPetObjectInfo.SkillSlot != dwPetSkillSlotCount )
	{
		const BYTE slot = PETMGR->GetExtendedSkillSlot(
			mpPetObjectInfo.Grade);

		mpPetObjectInfo.SkillSlot = mpPetObjectInfo.SkillSlot + slot;
	}

	++mpPetObjectInfo.Grade;

	CalcStats();
	DBInfoUpdate();
	LogPet(
		mpPetObjectInfo,
		ePetLogChangeGrade);

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORDBYTE2 msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_GRADEUP_NOTIFY;
		msg.dwObjectID = GetOwnerIndex();
		msg.dwData = GetID();
		msg.bData1 = BYTE(mpPetObjectInfo.Type);
		msg.bData2 = mpPetObjectInfo.SkillSlot;

		PACKEDDATA_OBJ->QuickSend(
			ownerObject,
			&msg,
			sizeof(msg));
	}
}

void CPet::FriendlyUp( BYTE val ) 
{ 
	mpPetObjectInfo.Friendly = BYTE( mpPetObjectInfo.Friendly + val); 
	
	if( mpPetObjectInfo.Friendly > 100 ) 
		mpPetObjectInfo.Friendly = 100; 

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_BYTE msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_FRIENDLY_SYNC;
		msg.bData = mpPetObjectInfo.Friendly;

		ownerObject->SendMsg(
			&msg,
			sizeof(msg));
	}
}

ITEMBASE* CPet::GetWearedItem( POSTYPE pos )
{
	if( pos >= ePetEquipSlot_Max )
		return NULL;

	if( mItemSlot[ pos ].dwDBIdx == 0 )
		return NULL;

	return &mItemSlot[ pos ];
}

void CPet::SetWearedItem( POSTYPE pos, ITEMBASE* pitem )
{
	if( pitem )
	{
		mItemSlot[ pos ] = *pitem;
	}
	else
	{
		ZeroMemory(
			&mItemSlot[pos],
			sizeof(mItemSlot[pos]));
	}

	CalcStats();

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_PET_ITEM_INFO msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_ITEM_INFO;
		msg.ID = GetID();
		memcpy(
			msg.Item,
			mItemSlot,
			sizeof(mItemSlot));
		PACKEDDATA_OBJ->QuickSendExceptObjectSelf(
			ownerObject,
			&msg,
			sizeof(msg));
	}
}

void CPet::SetAllWearedItem( ITEMBASE* info )
{
	memcpy( mItemSlot, info, sizeof( ITEMBASE ) * ePetEquipSlot_Max );

	CalcStats();
}

void CPet::CalcStats()
{
	memset( &mPetStat, 0, sizeof( PlayerStat ) );
	memset( &mItemStat, 0, sizeof( PlayerStat ) );	
	memset( &mItemOptionStat, 0, sizeof( PlayerStat ) );

	PET_STATUS_INFO* pInfo = PETMGR->GetPetStatusInfo( mpPetObjectInfo.Type, mpPetObjectInfo.Level );

	for( int i = 0; i < ePetEquipSlot_Max; i++ )
	{
		if( !mItemSlot[ i ].dwDBIdx )
			continue;

		ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( mItemSlot[ i ].wIconIdx );
		if( !pInfo )
			continue;

		// 090625 pdy 펫 기간제 아이템 착용처리 추가
		// 090713 pdy 조건 채크를 eITEM_TYPE_SEAL로 수정 
		if( eITEM_TYPE_SEAL == mItemSlot[ i ].nSealed )	
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

		PETMGR->AddPetItemOptionStat( mItemSlot[ i ], this );
	}
	
	//펫의 스탯 변화량	***	최종 펫의 스탯 = (스탯 * ( 1 + [%]증가옵션)) + [+]증가옵션				

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

	BYTE idx = BYTE(mpPetObjectInfo.Type) * 10 + mpPetObjectInfo.Grade;
	PET_HP_MP_INFO*	pHPMPInfo = PETMGR->GetHPMPInfo( idx );
	
	PET_STATUS_INFO*	pFirstInfo	=	PETMGR->GetPetStatusInfo( ePetType_Basic, 1 );
	DWORD	dwPetLevel			=	GetLevel();
	float	fAccuracyVal	=	11;
	float fPetLevel			=	(float)GetLevel();	

	//HP	= ( ( 레벨 - 1 ) * 10 ) + ( ( 건강 - 최초 건강 + 4 ) * 10 ) + ( 직업상수 * 레벨 * 7 )						
	//MP	= ( ( 레벨 - 1 ) * 10 ) + ( ( 정신 - 최초 정신 + 4 ) * 10 ) + ( 직업상수 * 레벨 * 7 )
	
	//최종 HP = (HP * ( 1 + [%]증가옵션)) + [+]증가옵션				

	// HP % 옵션
	float fHPPercent	=	( ( mRatePassiveStatus.MaxLife + 
							mRateBuffStatus.MaxLife	) * 0.01f )  + 
							mItemOptionStat.mLife.mPercent; 
	
	// HP + 옵션
	float fHPPlus		=	mPassiveStatus.MaxLife +
							mBuffStatus.MaxLife	   + 
							mItemOptionStat.mLife.mPlus;
							

	// MP % 옵션
	float fMPPercent	=	( ( mRatePassiveStatus.MaxMana +
							mRateBuffStatus.MaxMana	)  * 0.01f )  + 
							mItemOptionStat.mMana.mPercent;

	// MP + 옵션
	float fMPPlus		=	mPassiveStatus.MaxMana +
							mBuffStatus.MaxMana	   +
							mItemOptionStat.mMana.mPlus;

	float fHP			=	( ( dwPetLevel - 1 ) * 10 ) + ( ( mPetStat.mVitality.mPlus - pFirstInfo->VIT + 4 ) * 10 ) + ( pHPMPInfo->HP * dwPetLevel * 7 );
	float fMP			=	( ( dwPetLevel - 1 ) * 10 ) + ( ( mPetStat.mWisdom.mPlus - pFirstInfo->WIS + 4 ) * 10 ) + ( pHPMPInfo->MP * dwPetLevel * 7 );
	
	fHP					=	( fHP * ( 1 + fHPPercent ) ) + fHPPlus;
	fMP					=	( fMP * ( 1 + fMPPercent ) ) + fMPPlus;

	SetMaxLife( (DWORD)fHP );
	SetMaxMana( (DWORD)fMP );

	if( mpPetObjectInfo.HP > mpPetObjectInfo.MAXHP  ) SetLife( mpPetObjectInfo.MAXHP );
	if( mpPetObjectInfo.MP > mpPetObjectInfo.MAXMP  ) SetMana( mpPetObjectInfo.MAXMP );


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
			float	fPercent	=	( ( mRatePassiveStatus.PhysicDefense + 
									mRateBuffStatus.PhysicDefense ) * 0.01f )  +
									mItemOptionStat.mPhysicDefense.mPercent;

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
			float	fPercent	= 	( ( mRatePassiveStatus.MagicDefense +
									mRateBuffStatus.MagicDefense )  * 0.01f )   +
									mItemOptionStat.mMagicDefense.mPercent;

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
}

void CPet::RemoveAllAggroed()
{
	while(false == mAggroObjectContainer.empty())
	{
		const ObjectIndex objectIndex = *(mAggroObjectContainer.begin());
		mAggroObjectContainer.erase(
			objectIndex);

		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->RemoveFromAggro(
			GetID());
		monster->GetAbnormalStatus()->Attract = 0;
	}
}

void CPet::AddAggroToMyMonsters(int nAggroAdd, DWORD targetObjectIndex, DWORD skillIndex)
{
	for(ObjectIndexContainer::const_iterator iterator = mAggroObjectContainer.begin();
		mAggroObjectContainer.end() != iterator;
		++iterator)
	{
		const ObjectIndex objectIndex = *iterator;
		CMonster* const monster = (CMonster*)g_pUserTable->FindUser(
			objectIndex);

		if(0 == monster ||
			FALSE == (eObjectKind_Monster & monster->GetObjectKind()))
		{
			continue;
		}

		monster->AddAggro(
			targetObjectIndex,
			nAggroAdd,
			skillIndex);
	}
}

//------------------


BOOL CPet::AddFollowList(CMonster * pMob)
{	
	if( m_FollowMonsterList.GetDataNum() < 50 )		//max 50마리
	{
		m_FollowMonsterList.Add(pMob, pMob->GetID());
		UpdateGravity();
		return TRUE;
	}

    return FALSE;
}

BOOL CPet::RemoveFollowAsFarAs(DWORD GAmount, CObject* pMe )
{	
	VECTOR3 * ObjectPos	= CCharMove::GetPosition(this);
	BOOL bMe = FALSE;

	while(GAmount > 100)
	{	
		CMonster * pObject = NULL;
		CMonster * MaxObject = NULL;
		float	MaxDistance	= -1;
		float	Distance	= 0;

		m_FollowMonsterList.SetPositionHead();
		while((pObject = m_FollowMonsterList.GetData())!= NULL)
		{
			VECTOR3 * TObjectPos = CCharMove::GetPosition(pObject);
			if((Distance = CalcDistanceXZ( ObjectPos, TObjectPos )) > MaxDistance)
			{
				MaxDistance = Distance;
				MaxObject = pObject;
			}
		}
		if(MaxObject)
		{
			if(GAmount > MaxObject->GetGravity())
				GAmount -= MaxObject->GetGravity();
			else
				GAmount = 0;

			MaxObject->SetTObject(NULL);

			GSTATEMACHINE.SetState(MaxObject, eMA_WALKAROUND);

			if( pMe == MaxObject )
				bMe = TRUE;
		}
		else
		{
			MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("amount != 0"));
			GAmount = 0;
		}
	}

	return bMe;	
}

void CPet::RemoveFollowList(DWORD ID)
{
	m_FollowMonsterList.Remove(ID);
	UpdateGravity();
}

void CPet::DoDie(CObject* pAttacker)
{
	if( pAttacker )
		OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,pAttacker->GetID());
	else
		OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,0);

	// 091110 ONS 펫 애니메이션 추가 : 펫 죽음 연출 처리
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die,MONSTERREMOVE_TIME);
//---KES Aggro 070918
//---어그로 리스트 릴리스
	RemoveAllAggroed();	//*주의: 아래 FollowMonsterList해제하기 이전에 해주어야, 다음 어그로를 타겟으로 잡을 수 있다.
//-------------------

	CMonster * pObject = NULL;
	m_FollowMonsterList.SetPositionHead();
	while((pObject = (CMonster *)m_FollowMonsterList.GetData())!= NULL)
	{
		pObject->SetTObject(NULL);
	}
	m_FollowMonsterList.RemoveAll();


	//---KES 죽으면 이동을 멈춰주어야 한다.
	if( CCharMove::IsMoving(this) )
	{
		VECTOR3 pos;
		GetPosition( &pos );
		CCharMove::EndMove( this, gCurTime, &pos );
	}

	mpPetObjectInfo.HP = 0;
	mpPetObjectInfo.MP = 0;
	mpPetObjectInfo.Friendly = 0;
	mpPetObjectInfo.State = ePetState_Die;

	DBInfoUpdate();
	LogPet(
		mpPetObjectInfo,
		ePetLogDie);

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORD msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_DIE_NOTIFY;
		msg.dwData = mpPetObjectInfo.ItemDBIdx;

		ownerObject->SendMsg(
			&msg,
			sizeof(msg));
	}
}

DWORD CPet::Damage(CObject* pAttacker,RESULTINFO* pDamageInfo)
{
	DWORD life = GetLife();
	DWORD beforelife = life;

	if(life > pDamageInfo->RealDamage)
	{
		life -= pDamageInfo->RealDamage;
	}
	else
	{
		life = (m_byGod ? 1 : 0);
	}
	
	SetLife(life,FALSE);

	DoDamage(pAttacker,pDamageInfo,beforelife);

	return life;
}

void CPet::SetLife(DWORD val,BOOL bSendMsg) 
{
	if(GetState() == eObjectState_Die)
		return;

	DWORD maxlife = mpPetObjectInfo.MAXHP;
	if(val > maxlife)
		val = maxlife;
	
	if(mpPetObjectInfo.HP != val)	
	{
		CObject* const ownerObject = g_pUserTable->FindUser(
			GetOwnerIndex());

		if(ownerObject &&
			bSendMsg)
		{
			MSG_INT msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_LIFE_BROAD;
			msg.dwObjectID = GetID();
			msg.nData = val - GetLife();
			PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );
		}
	}
		
	mpPetObjectInfo.HP = val;
}

void CPet::SetMana(DWORD val,BOOL bSendMsg)
{ 
	if(GetState() == eObjectState_Die)
		return;

	DWORD MaxMana = mpPetObjectInfo.MAXMP;
	if(val > MaxMana)
		val = MaxMana;

	if( mpPetObjectInfo.MP != val)
	{
		CObject* const ownerObject = g_pUserTable->FindUser(
			GetOwnerIndex());

		if(ownerObject &&
			bSendMsg)
		{
			MSG_DWORD msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_MANA_NOTIFY;
			msg.dwObjectID = GetID();
			msg.dwData = val;
			ownerObject->SendMsg(
				&msg,
				sizeof(msg));
		}
	}
	
	mpPetObjectInfo.MP = val; 
}

void CPet::FriendlyProcess()
{
	mpPetObjectInfo.FriendlyCheckTime += gCurTime - mCheckTime;
	mCheckTime = gCurTime;

	if( mpPetObjectInfo.FriendlyCheckTime > PETMGR->GetFriendlyReduceTime() )
	{
		mpPetObjectInfo.FriendlyCheckTime = 0;
		mpPetObjectInfo.Friendly = BYTE( mpPetObjectInfo.Friendly - PETMGR->GetFriendlyReducePoint() );

		if( mpPetObjectInfo.Friendly == 0 )
		{
			DoDie( NULL );
		}

		CObject* const ownerObject = g_pUserTable->FindUser(
			GetOwnerIndex());

		if(ownerObject)
		{
			MSG_BYTE msg;
			msg.Category = MP_PET;
			msg.Protocol = MP_PET_FRIENDLY_SYNC;
			msg.bData = mpPetObjectInfo.Friendly;
			ownerObject->SendMsg(
				&msg,
				sizeof(msg));
		}
	}
}

void CPet::SetMaxLife(DWORD val)
{
	mpPetObjectInfo.MAXHP = val;

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORD msg;
		msg.dwObjectID	=	GetID();
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_MAXLIFE_NOTIFY;
		msg.dwData = val;
		PACKEDDATA_OBJ->QuickSend( this, &msg, sizeof( msg ) );
	}
}

void CPet::SetMaxMana(DWORD val)
{
	mpPetObjectInfo.MAXMP = val;

	CObject* const ownerObject = g_pUserTable->FindUser(
		GetOwnerIndex());

	if(ownerObject)
	{
		MSG_DWORD msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_MAXMANA_NOTIFY;
		msg.dwData = val;

		ownerObject->SendMsg(
			&msg,
			sizeof(msg));
	}
}

void CPet::DBInfoUpdate()
{
	PetInfoUpdate(
		GetOwnerIndex(), 
		mpPetObjectInfo.ItemDBIdx, 
		mpPetObjectInfo.Level, 
		mpPetObjectInfo.Grade, 
		mpPetObjectInfo.Type, 
		mpPetObjectInfo.SkillSlot, 
		mpPetObjectInfo.Exp, 
		mpPetObjectInfo.Friendly, 
		mpPetObjectInfo.MAXHP, 
		mpPetObjectInfo.HP, 
		mpPetObjectInfo.MAXMP, 
		mpPetObjectInfo.MP, 
		mpPetObjectInfo.AI, 
		mpPetObjectInfo.State,
		mpPetObjectInfo.FriendlyCheckTime);
}

void CPet::StateProcess()
{
    if( !mbBuffSkillLoad )
	{
		CharacterBuffLoad( GetID() );
		mbBuffSkillLoad = TRUE;
	}
}

// 090226 LUJ, 보유한 펫 스킬인지 검사한다
BOOL CPet::IsHaveSkill( DWORD skillIndex ) const
{
	//090319 pdy 팻 보유스킬 체크 수정 
	//스킬인덱스에서 100을 나누어 종류가 같은지만 채크후 
	//보유 스킬레벨을 구해 (skillIndex % 100)와 같은지 채크한다.

	BOOL  bHaveSkill = FALSE;
	DWORD dwSkillLevel = 0;

	for( DWORD i = 0; i < ePetEquipSlot_Max; ++i )
	{
		const ITEMBASE&		itemBase = mItemSlot[ i ];
		const ITEM_INFO*	itemInfo = ITEMMGR->GetItemInfo( itemBase.wIconIdx );

		if( ! itemInfo )
		{
			continue;
		}
		//스킬종류가 같으면 
		else if( (itemInfo->SupplyValue / 100) ==  (skillIndex / 100 ) )  
		{
			//스킬레벨을 구해준다.
			dwSkillLevel+= (itemInfo->SupplyValue % 100);
			bHaveSkill = TRUE;
		}
	}

	//스킬을 보유하고있고 보유스킬레벨이 같으면 OK
	if( bHaveSkill && dwSkillLevel ==  (skillIndex % 100) ) 
	{
		return TRUE;
	}

	return FALSE;
}

// 090625 pdy 펫 기간제 아이템 착용처리 추가
void CPet::ProcessTimeCheckItem( BOOL bDBUpdate , DWORD dwElapsedMili )
{
	CPlayer* const ownerPlayer = (CPlayer*)g_pUserTable->FindUser(
		GetOwnerIndex());

	if(0 == ownerPlayer ||
		eObjectKind_Player != ownerPlayer->GetObjectKind())
	{
		return;
	}

	DWORD dwElapsedSecond = dwElapsedMili / 1000;

	BOOL bItemDiscard = FALSE;

	for( int i = 0 ; i < ePetEquipSlot_Max ; i ++ )
	{
		ITEMBASE* pItemBase = GetWearedItem( (POSTYPE)i );

		if( ! pItemBase || !pItemBase->dwDBIdx )
			continue;

		ITEM_INFO* info = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );
		if( !info )
			continue;

		if(	info &&
		info->SupplyType == ITEM_KIND_COOLTIME )
		{
			pItemBase->nRemainSecond	= max( 0, int( pItemBase->nRemainSecond - dwElapsedSecond ) );
			// 080820 LUJ, 체크한 시간을 저장하여 주기적 갱신이 없어도 처리할 수 있도록 한다
			pItemBase->ItemParam		= gCurTime;

			UpdateRemainTime(
				GetOwnerIndex(),
				pItemBase->dwDBIdx,
				pItemBase->nSealed,
				pItemBase->nRemainSecond);
		}
		else if( pItemBase->nSealed == eITEM_TYPE_UNSEAL )
		{
			if( pItemBase->nRemainSecond > (int)dwElapsedSecond )
			{
				pItemBase->nRemainSecond -= dwElapsedSecond;

				// 071125 KTH --- Player "RemainSecond가 1분 미만일 경우 클라이언트에게 정보를 보내준다."
				if( pItemBase->nRemainSecond <= 60 )
				{
					MSG_DWORD2 msg;
					msg.Category = MP_PET;
					msg.Protocol = MP_PET_ITEM_TIMELIMT_ONEMINUTE_FROM_PETINVEN;
					msg.dwData1 = pItemBase->wIconIdx;
					msg.dwData2 = pItemBase->Position;
	
					ownerPlayer->SendMsg(&msg, sizeof(msg));
				}

				if( bDBUpdate )
				{
					//DB Update
					ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pItemBase->wIconIdx );
		
					if( pItemInfo )	
					if( pItemInfo->nTimeKind == eKIND_PLAYTIME )
					{
						UpdateRemainTime(
							ownerPlayer->GetID(),
							pItemBase->dwDBIdx,
							pItemBase->nSealed,
							pItemBase->nRemainSecond);
					}
				}
			}
			else
			{
				bItemDiscard = TRUE; 

				ItemDeleteToDB(pItemBase->dwDBIdx);

				MSG_BYTE2 msg;
				msg.Category = MP_PET;
				msg.Protocol = MP_PET_ITEM_DISCARD_ACK;
				msg.bData1 = (BYTE)pItemBase->Position ;
				msg.bData2 = TRUE ;	
				ownerPlayer->SendMsg(&msg, sizeof(msg));	

				LogItemMoney(
					ownerPlayer->GetID(),
					ownerPlayer->GetObjectName(),
					0,
					"",
					eLog_ShopItemUseEndFromPetInven,
					ownerPlayer->GetMoney(),
					0,
					0,
					pItemBase->wIconIdx,
					pItemBase->dwDBIdx,
					pItemBase->Position,
					0,
					1,
					ownerPlayer->GetPlayerExpPoint() );

				memset( &mItemSlot[ pItemBase->Position ], 0, sizeof( ITEMBASE ) );
			}
		}
	}

	if( bItemDiscard ) 
	{
		CalcStats();

		MSG_PET_ITEM_INFO msg;
		msg.Category = MP_PET;
		msg.Protocol = MP_PET_ITEM_INFO;
		msg.ID = GetID();
		memcpy( msg.Item, mItemSlot, sizeof(mItemSlot));
		PACKEDDATA_OBJ->QuickSendExceptObjectSelf(
			ownerPlayer,
			&msg,
			sizeof(msg));
	}
}

void CPet::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Die:
		g_pServerSystem->RemovePet(
			GetID());

		CPlayer* const player = (CPlayer*)g_pUserTable->FindUser(
			GetOwnerIndex());

		if(player &&
			eObjectKind_Player == player->GetObjectKind())
		{
			player->SetPetItemDbIndex(
				0);
		}

		break;
	}
}

float CPet::CalcPhysicAttack( float fAttackPoint )
{
	//최종 물리 공격력 = (물리 공격력 * ( 1 + [%]증가옵션)) + [+]증가옵션					

	float	fPhysicAttack	=	0.0f;

	// % 옵션
	float	fPhysicPercent		=	( ( mRatePassiveStatus.PhysicAttack + 
									mRateBuffStatus.PhysicAttack ) * 0.01f )  +
									mItemOptionStat.mPhysicAttack.mPercent;

	// + 옵션
	float fPhysicPlus			=	mPassiveStatus.PhysicAttack +
									mBuffStatus.PhysicAttack    +
									mItemOptionStat.mPhysicAttack.mPlus;

	fPhysicAttack	=	( fAttackPoint * ( 1 + fPhysicPercent ) ) + fPhysicPlus;

	return	fPhysicAttack;
}

float CPet::CalcMagicAttack( float fAttackPoint )
{
	//최종 마법 공격력 = (마법 공격력 * ( 1 + [%]증가옵션)) + [+]증가옵션					

	float fMagicAttack	=	0.0f;

	// % 옵션
	float fMagicPercent	=	( ( mRatePassiveStatus.MagicAttack +
							mRateBuffStatus.MagicAttack ) * 0.01f )   + 
							mItemOptionStat.mMagicAttack.mPercent;

	// + 옵션
	float fMagicPlus	=	mPassiveStatus.MagicAttack +
							mBuffStatus.MagicAttack	   +
							mItemOptionStat.mMagicAttack.mPlus;

	fMagicAttack		=	 ( fAttackPoint * ( 1 + fMagicPercent ) ) + fMagicPlus;

	return	fMagicAttack;
}

void CPet::ProceedToTrigger()
{
	if(gCurTime < mNextCheckedTick)
	{
		return;
	}

	// 091116 LUJ, 주기적으로 발송하는 메시지 간격을 늘림(0.5 -> 1.0초)
	const DWORD stepTick = 1000;
	mNextCheckedTick = gCurTime + stepTick;
	// 091116 LUJ, 채널에 해당하는 메시지를 할당받도록 한다
	Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(GetGridID());
	message->AddValue(Trigger::eProperty_ObjectIndex, GetID());
	message->AddValue(Trigger::eProperty_ObjectKind, GetObjectKind());
	message->AddValue(Trigger::eProperty_Event, Trigger::eEvent_CheckSelf);
}