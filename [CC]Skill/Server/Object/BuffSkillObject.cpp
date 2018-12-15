#include "stdafx.h"
#include ".\buffskillobject.h"
#include "../Info/ActiveSkillInfo.h"
#include "../Info/BuffSkillInfo.h"
#include "../Delay/Delay.h"
#include "CharMove.h"
#include "AttackManager.h"
#include "PackedData.h"
#include "Battle.h"
#include "MapDBMsgParser.h"
#include "UserTable.h"
#include "Player.h"
#include "Event.h"
#include "CharacterCalcManager.h"
#include "..\[cc]skill\Server\\Manager\\SkillManager.h"
#include "pet.h"
#include "Monster.h"
#include "../[CC]Header/GameResourceManager.h"
#include "SiegeWarfareMgr.h"
#include "..\4DyuchiGXGFunc/global.h"

cBuffSkillObject::cBuffSkillObject(const cBuffSkillInfo& skillInfo) :
cSkillObject(skillInfo.GetInfo()),
mSkillInfo(&skillInfo)
{
	// 080602 LUJ, 오브젝트 타입을 초기화한다
	mType = cSkillObject::TypeBuff;

	mpDelay = new cDelay;
	mpInterval = new cDelay;
	mpUpdate = new cDelay;
	mpConsumption = new cDelay;

	mpInterval->Init( 1000 );
	mpUpdate->Init( 1800000 );
	mpConsumption->Init( 3000 );

	mDelayCheck = TRUE;
	mDestroy = FALSE;
	mIsUpdate = TRUE;

	mCount = 0;
	mCountType = eBuffSkillCountType_None;
}

cBuffSkillObject::~cBuffSkillObject(void)
{
	delete mpDelay;
	delete mpInterval;
	delete mpUpdate;
	delete mpConsumption;
}

void cBuffSkillObject::Init( sSKILL_CREATE_INFO* pInfo )
{
	/// cSkillObject의 Init함수 호출
	cSkillObject::Init( pInfo );
	
	MSG_SKILLOBJECT_ADD2 msg;
	
	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_SKILLOBJECT_ADD;
	msg.bCreate = TRUE;

	msg.SkillObjectInfo.SkillObjectIdx = pInfo->skillObjectId;
	msg.SkillObjectInfo.SkillIdx = GetSkillIdx();
	msg.SkillObjectInfo.Pos = pInfo->pos;
	msg.SkillObjectInfo.StartTime = 0;
	msg.SkillObjectInfo.Direction = pInfo->skillDir;
	msg.SkillObjectInfo.SkillLevel = ( BYTE )pInfo->level;
	msg.SkillObjectInfo.Operator = pInfo->operatorId;
	msg.SkillObjectInfo.Rate = 1.0f;
	msg.SkillObjectInfo.RemainTime = pInfo->remainTime;
	msg.SkillObjectInfo.Count = pInfo->count;
	msg.SkillObjectInfo.IsNew = pInfo->isNew;

	memcpy( &( msg.SkillObjectInfo.MainTarget ), &( pInfo->mainTarget ), sizeof( MAINTARGET ) );

	msg.SkillObjectInfo.BattleID = GetBattleID();

	mDelayCheck = TRUE;
	mDestroy = FALSE;
	// 090204 LUJ, 스킬을 조건에 따라 적용/해제시키는 플래그
	mIsTurnOn = TRUE;

	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject ||
		FALSE == targetedObject->GetInited())
	{
		mState = SKILL_STATE_DESTROY;
		return;
	}
	else if(targetedObject->GetObjectKind() != eObjectKind_Player &&
		targetedObject->GetObjectKind() != eObjectKind_Pet)
	{
		mIsUpdate = FALSE;
	}

	// AI 스크립트에 따라 버프 사용도 어그로가 적용된다
	AddAggro(
		*targetedObject);
	const DWORD idx = GetSkillIdx() / 100;

	if(cBuffSkillObject* pObject = targetedObject->GetBuffList().GetData( idx ))
	{
		if( pObject->GetSkillLevel() > GetSkillLevel() )
		{
			mState = SKILL_STATE_DESTROY;
			return;
		}
		else if( pObject->GetSkillLevel() == GetSkillLevel() )
		{
			mState = SKILL_STATE_DESTROY;
			pObject->ResetDelay();
			return;
		}
		else
		{
			pObject->SetEndState();
			pObject->EndState();
			ProcessStart();
		}

		msg.SkillObjectInfo.RemainTime	= pObject->GetDelayTime();
	}
	else
	{
		ProcessStart();

		msg.SkillObjectInfo.RemainTime	= GetDelayTime();
	}

	msg.SkillObjectInfo.Count = mCount;

	targetedObject->GetBuffList().Add(this, idx);

	GridPosition* pGridPosition = targetedObject->GetGridPosition();

	if(pGridPosition->x == 65535 || pGridPosition->z == 65535)
	{
		return;
	}

	PACKEDDATA_OBJ->QuickSend( targetedObject, &msg, sizeof( MSG_SKILLOBJECT_ADD2 ) );
}

SKILL_STATE cBuffSkillObject::Update()
{
	if( mState == SKILL_STATE_DESTROY )
	{
		return cSkillObject::Update();
	}

	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject)
	{
		mState = SKILL_STATE_DESTROY;
	}
	else
	{
		if(targetedObject->GetState() == eObjectState_Die )
		{
			if(GetInfo().Die == 0)
			{
				SetEndState();
				EndState();
			}
		}
		else if(CPlayer* pPlayer = (CPlayer*)GetOperator())
		{
			if(eObjectKind_Player == pPlayer->GetObjectKind() &&
				0 != pPlayer->GetPartyIdx() &&
				1 == GetInfo().Party)
			{
				SetEndState();
				EndState();
			}
		}

		switch( mState )
		{
			/// 발동중
		case SKILL_STATE_PROCESS:
			{
				ProcessState();
			}
			break;
		case SKILL_STATE_END:
			{
				EndState();
			}
			break;
		}

		CCharMove::SetPosition(
			this,
			CCharMove::GetPosition(targetedObject));
	}

	return cSkillObject::Update();
}

void cBuffSkillObject::ProcessStart()
{
	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject)
	{
		mState = SKILL_STATE_END;
		return;
	}

	if(GetInfo().DelayTime)
	{
		mDelayCheck = TRUE;
		
		if(GetInfo().DelayTime < 10000)
		{
			mIsUpdate = FALSE;
		}
	}
	else
	{
		mDelayCheck = FALSE;
	}

	if(GetInfo().CountType)
	{
		mCountType = GetInfo().CountType;

		if( mSkillObjectInfo.count )
		{
			mCount = mSkillObjectInfo.count;
		}
		else
		{
			mCount = GetInfo().Count;
		}
	}

	mSkillInfo->AddBuffStatus(
		targetedObject);

	if( mDelayCheck )
	{
		if( mSkillObjectInfo.remainTime )
		{
			mpDelay->Init( mSkillObjectInfo.remainTime );
			mpDelay->Start();
		}
		else
		{
			// 081203 LUJ, 지연 시간을 계산해서 가져온다. 때에 따라 스크립트가 적용되어 시간이 가감된다
			mpDelay->Init(
				GetDelayTime(targetedObject, &GetInfo()));
			mpDelay->Start();

			if( mIsUpdate && ! GetInfo().NoUpdate)
			{
				CharacterBuffAdd(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					mpDelay->GetRemainTime(),
					mCount,
					GetInfo().IsEndTime);
			}
		}
	}
	else
	{
		if( !mSkillObjectInfo.remainTime )
		{
			if( mIsUpdate  && ! GetInfo().NoUpdate)
			{
				CharacterBuffAdd(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					1,
					mCount,
					GetInfo().IsEndTime);
			}
		}
	}

	mpConsumption->Start();
	mpUpdate->Start();

	mActive = TRUE;
	/// 발동중 상태로 변경
	mState = SKILL_STATE_PROCESS;
}

void cBuffSkillObject::ProcessState()
{
	/// 지속 시간 체크
	if( mDelayCheck )
	{
		if( !mpDelay->Check() )
		{
			mState = SKILL_STATE_END;
			return;
		}
	}

	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	BOOL bIsActiveRule = FALSE;
	BOOL bIsStopRule = FALSE;

	if(0 == targetedObject)
	{
		mState = SKILL_STATE_END;
		return;
	}
	else if(targetedObject->GetObjectBattleState())
	{
		switch(GetInfo().mRuleBattle)
		{
		case eBuffSkillActiveRule_Active:
			{
				bIsActiveRule = TRUE;
			}
			break;
		case eBuffSkillActiveRule_Delete:
			{
				SetEndState();
			}
			break;
		case eBuffSkillActiveRule_Stop:
			{
				bIsStopRule = TRUE;
			}
			break;
		}
	}
	else
	{
		switch(GetInfo().mRulePeace)
		{
		case eBuffSkillActiveRule_Active:
			{
				bIsActiveRule = TRUE;
			}
			break;
		case eBuffSkillActiveRule_Delete:
			{
				SetEndState();
			}
			break;
		case eBuffSkillActiveRule_Stop:
			{
				bIsStopRule = TRUE;
			}
			break;
		}
	}

	if(CCharMove::IsMoving(targetedObject))
	{
		switch(GetInfo().mRuleMove)
		{
		case eBuffSkillActiveRule_Active:
			{
				bIsActiveRule = TRUE;
			}
			break;
		case eBuffSkillActiveRule_Delete:
			{
				SetEndState();
			}
			break;
		case eBuffSkillActiveRule_Stop:
			{
				bIsStopRule = TRUE;
			}
			break;
		}
	}
	else
	{
		switch(GetInfo().mRuleStop)
		{
		case eBuffSkillActiveRule_Active:
			{
				bIsActiveRule = TRUE;
			}
			break;
		case eBuffSkillActiveRule_Delete:
			{
				SetEndState();
			}
			break;
		case eBuffSkillActiveRule_Stop:
			{
				bIsStopRule = TRUE;
			}
			break;
		}
	}
	
	if( targetedObject->GetState() == eObjectState_Rest )
	{
		switch( GetInfo().mRuleRest )
		{
		case eBuffSkillActiveRule_Active:
			{
				bIsActiveRule = TRUE;
			}
			break;
		case eBuffSkillActiveRule_Delete:
			{
				SetEndState();
			}
			break;
		case eBuffSkillActiveRule_Stop:
			{
				bIsStopRule = TRUE;
			}
			break;
		}
	}

	// 활성조건을 만족하더라도 제거 조건을 우선적으로 처리한다.
	if( bIsStopRule )
	{
		SetActive( FALSE );
		return;
	}
	else if( bIsActiveRule )
	{
		SetActive( TRUE );
	}

	// 090204 LUJ, 조건을 체크하여 적용/해제시킨다
	if(eObjectKind_Player == targetedObject->GetObjectKind())
	{
		CPlayer* const player = ( CPlayer* )targetedObject;
		const BOOL isEnable = player->IsEnable(GetInfo());
		SetActive( isEnable );
	}

	// 100614 ShinJS --- OFF 된경우 이후처리를 하지 않는다.
	if( !mActive )
		return;

	switch(GetInfo().Status)
	{
	case eStatusKind_Poison:
	case eStatusKind_Bleeding:
	case eStatusKind_Burning:
	case eStatusKind_HolyDamage:
		{
			RESULTINFO	damage = {0};
			damage.mSkillIndex = GetSkillIdx();

			MSG_SKILL_RESULT msg;

			msg.InitMsg( GetID() );
			msg.SkillDamageKind = false;

			CTargetListIterator iter(&msg.TargetList);

			if( mpInterval->IsStart() )
			{
				if( mpInterval->Check() )
				{
				}
				else
				{
					ATTACKMGR->SpecialAttack(
						GetOperator(),
						targetedObject,
						&damage,
						GetInfo().StatusData);

					iter.AddTargetWithResultInfo(
						mSkillObjectInfo.mainTarget.dwMainTargetID,
						1,
						&damage);
					iter.Release();

					PACKEDDATA_OBJ->QuickSend(
						targetedObject,
						&msg,
						msg.GetMsgLength());

					mpInterval->Start();
				}
			}
			else
			{
				ATTACKMGR->SpecialAttack(
					GetOperator(),
					targetedObject,
					&damage,
					GetInfo().StatusData);

				iter.AddTargetWithResultInfo(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					1,
					&damage);
				iter.Release();

				PACKEDDATA_OBJ->QuickSend(
					targetedObject,
					&msg,
					msg.GetMsgLength());

				mpInterval->Start();
			}
		}
		break;
		// 090204 LUJ, 마나 공격
	case eStatusKind_ManaBurn:
		{
			const BOOL isReady		= ( ! mpInterval->IsStart() );
			const BOOL isChecked	= ( ! mpInterval->Check() );
			const BOOL isManaBurn	= ( isReady || isChecked );

			if( isManaBurn )
			{
				ATTACKMGR->BurnMana(
					GetOperator(),
					targetedObject,
					&GetInfo());

				MSG_SKILL_RESULT message;
				ZeroMemory( &message, sizeof( message ) );
				message.InitMsg( GetID() );
				message.SkillDamageKind = false;

				CTargetListIterator iterator( &message.TargetList );
				RESULTINFO resultInfo = { 0 };
				iterator.AddTargetWithResultInfo(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					1,
					&resultInfo );
				iterator.Release();

				PACKEDDATA_OBJ->QuickSend(
					targetedObject,
					&message,
					message.GetMsgLength() );
				mpInterval->Start();
			}
		}
		break;

	// 100612 ShinJS --- 버프 이동 연출 추가
	case eStatusKind_Push_Target:
	case eStatusKind_Push_Operator:
	case eStatusKind_PushPull_Target:
	case eStatusKind_PushPull_Operator:
		{
			mState = SKILL_STATE_END;

			const float distance = GetInfo().StatusData;
			const DWORD duration = (DWORD)GetInfo().StatusDataType;

			CObject* pPushObject = NULL;
			CObject* pBaseObject = NULL;

			switch( GetInfo().Status )
			{
			case eStatusKind_Push_Target:
			case eStatusKind_PushPull_Target:
				{
					pPushObject = targetedObject;
					pBaseObject = GetOperator();
				}
				break;

			case eStatusKind_Push_Operator:
			case eStatusKind_PushPull_Operator:
				{
					pPushObject = GetOperator();
					pBaseObject = targetedObject;
				}
				break;
			}

			if( pBaseObject == NULL ||
				pPushObject == NULL )
				break;

			const float fPushObjectMoveSpeed = ( (pPushObject->GetObjectKind() & eObjectKind_Monster) ? ((CMonster*)pPushObject)->GetSMonsterList().WalkMove : pPushObject->DoGetMoveSpeed() );
			if( pPushObject->GetState() == eObjectState_Die ||
				fPushObjectMoveSpeed == 0.f )
			{
				break;
			}

			VECTOR3 basePosition = {0};
			pBaseObject->GetPosition( &basePosition );

			VECTOR3 targetPosition = {0};
			VECTOR3 fixedPosition = {0};
			pPushObject->GetPosition( &targetPosition );

			switch( GetInfo().Status )
			{
			case eStatusKind_Push_Target:
			case eStatusKind_Push_Operator:
				{
					VECTOR3 directionVector = targetPosition - basePosition;
					if( pBaseObject == pPushObject ||
						targetPosition == basePosition )
					{
						directionVector = m_SkillDir;
					}

					VECTOR3 normalVector = {0};
					Normalize( &normalVector, &directionVector );
					fixedPosition = targetPosition + normalVector * distance;
				}
				break;

			case eStatusKind_PushPull_Target:
			case eStatusKind_PushPull_Operator:
				{
					VECTOR3 directionVector = targetPosition - basePosition;
					if( pBaseObject == pPushObject ||
						targetPosition == basePosition )
					{
						directionVector = m_SkillDir;
					}

					VECTOR3 normalVector = {0};
					Normalize( &normalVector, &directionVector );
					fixedPosition = basePosition + normalVector * (distance + pBaseObject->GetRadius());
				}
				break;
			}

			VECTOR3 collisionPos = {0};
			if( g_pServerSystem->GetMap()->CollisionCheck( &targetPosition, &fixedPosition, &collisionPos, pPushObject ) == TRUE )
			{
				fixedPosition = collisionPos;
			}

			MSG_DWORD5 msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_MOVE;
			msg.Protocol = MP_MOVE_KNOCKBACK_ACK;
			msg.dwData1 = pBaseObject->GetID();
			msg.dwData2 = pPushObject->GetID();
			msg.dwData3 = (DWORD)fixedPosition.x;
			msg.dwData4 = (DWORD)fixedPosition.z;
			msg.dwData5 = duration;

			pPushObject->SetShockTime( duration );

			CCharMove::SetPosition( pPushObject, &fixedPosition );

			PACKEDDATA_OBJ->QuickSend( pPushObject, &msg, sizeof(msg) );
		}
		break;
	}

	if( !mpConsumption->Check() )
	{
		Consumption();
		mpConsumption->Start();
	}

	if( !mpUpdate->Check() )
	{
		if( mDelayCheck )
		{
			if( mIsUpdate && ! GetInfo().NoUpdate)
			{
				CharacterBuffUpdate(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					mpDelay->GetRemainTime(),
					mCount);
			}
		}
		else
		{
			if( mIsUpdate && ! GetInfo().NoUpdate)
			{
				CharacterBuffUpdate(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					1,
					mCount);
			}
		}
		mpUpdate->Start();
	}
}

void cBuffSkillObject::SetEndState()
{
	mState = SKILL_STATE_END;
	mDestroy = TRUE;
	mpDelay->End();
}

void cBuffSkillObject::EndState()
{
	if( mDelayCheck )
	{
		if( mpDelay->Check() )
		{
			if( mIsUpdate )
			{
				CharacterBuffUpdate(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					mpDelay->GetRemainTime(),
					mCount);
			}
		}
		else
		{
			if( mIsUpdate )
			{
				CharacterBuffRemove(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx());

				CPlayer* player	= ( CPlayer* )GetOperator();

				if( player &&
					player->GetObjectKind() == eObjectKind_Player &&
					GetInfo().IsCash)
				{
					SKILL_BASE skill = { 0 };
					skill.wSkillIdx	= GetSkillIdx();

					InsertLogSkill(
						player,
						&skill,
						eLog_SkillCashRemove );
				}
			}
		}
	}
	else
	{
		if( mDestroy )
		{
			if( mIsUpdate )
			{
				CharacterBuffRemove(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx());

				// 080819 LUJ, 유료 스킬 로그
				{
					CPlayer* player	= ( CPlayer* )GetOperator();
					
					if( player &&
						player->GetObjectKind() == eObjectKind_Player &&
						GetInfo().IsCash)
					{
						SKILL_BASE skill = { 0 };;
						skill.wSkillIdx	= GetSkillIdx();

						InsertLogSkill(
							player,
							&skill,
							eLog_SkillCashRemove );
					}
				}
			}
		}
		else
		{
			if( mIsUpdate )
			{
				CharacterBuffUpdate(
					mSkillObjectInfo.mainTarget.dwMainTargetID,
					GetSkillIdx(),
					1,
					mCount);
			}
		}
	}

	mDestroy = FALSE;
	mActive = FALSE;

	if(CObject* const targetedObject = g_pUserTable->FindUser(mSkillObjectInfo.mainTarget.dwMainTargetID))
	{
		// 091211 LUJ, 버프가 적용된 상태에서만 꺼야한다. 장비/상태에 따라 버프가 꺼질 수 있다
		if(mIsTurnOn)
		{
			mSkillInfo->RemoveBuffStatus(
				targetedObject);
		}

		targetedObject->GetBuffList().Remove(
			GetSkillIdx() / 100);
	}

	mDelayCheck = TRUE;
	mState = SKILL_STATE_DESTROY;
}

void cBuffSkillObject::SetActive( BOOL val )
{
	if( mState != SKILL_STATE_PROCESS ||
		mActive == val )
	{
		return;
	}

	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject)
	{
		return;
	}

	mActive = val;

	// 091211 LUJ, 전투/달리기 등의 상태에 따라 버프가 자동으로 켜지거나 꺼진다.
	//			게다가 장비 상태 등에 따라서도 켜고 꺼야한다. 이에 따라 켜고
	//			꺼지는 상황이 반복적으로 계속 발생할 수 있다. 이를 막기 위해
	//			현재 설정 상태를 저장하는 변수로 동작하게 한다
	if(val && mIsTurnOn == FALSE)
	{
		mSkillInfo->AddBuffStatus(
			targetedObject);
		mIsTurnOn = TRUE;
	}
	else if(! val && mIsTurnOn == TRUE)
	{
		mSkillInfo->RemoveBuffStatus(
			targetedObject);
		mIsTurnOn = FALSE;
	}
}


void cBuffSkillObject::Consumption()
{
	if(	GetInfo().mMana.mPlus ||
		GetInfo().mMana.mPercent ||
		GetInfo().mLife.mPlus ||
		GetInfo().mLife.mPercent ||
		GetInfo().Money )
	{
		if( ! GetOperator() )
		{
			SetEndState();
			return;
		}
	}
	// 080728 LUJ, 오퍼레이터가 없으면 소모할 주체가 없으므로 통과한다
	else if( ! GetOperator() )
	{
		return;
	}

	if( GetOperator()->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pOperator = (CPlayer*)GetOperator();

		DWORD mana	= DWORD( float( pOperator->GetMana() ) * GetInfo().mMana.mPercent + GetInfo().mMana.mPlus );
		DWORD life	= DWORD( float( pOperator->GetLife() ) * GetInfo().mLife.mPercent + GetInfo().mLife.mPlus );
		
		if( pOperator->GetMana() < mana )
		{
			SetEndState();
			return;
		}

		if( pOperator->GetLife() < life )
		{
			SetEndState();
			return;
		}

		if( pOperator->GetMoney() < GetInfo().Money )
		{
			SetEndState();
			return;
		}

		// 071203 KTH --- BuffStatus에서 DecreaseManaRate 만큼의 마나 소모량을 줄여준다.
		mana = (DWORD) (mana + ( mana * (pOperator->GetRateBuffStatus()->DecreaseManaRate / 100 ) ) );
		//---
		if( mana )	pOperator->ReduceMana( mana );
		if( life )	pOperator->AddLife( life * -1, NULL );
		if( GetInfo().Money )	pOperator->SetMoney( GetInfo().Money, MONEY_SUBTRACTION, MF_OBTAIN );
	}
	else if( GetOperator()->GetObjectKind() == eObjectKind_Pet )
	{
		CPet* pOperator = (CPet*)GetOperator();

		DWORD mana	= DWORD( float( pOperator->GetMana() ) * GetInfo().mMana.mPercent + GetInfo().mMana.mPlus );
		DWORD life	= DWORD( float( pOperator->GetLife() ) * GetInfo().mLife.mPercent + GetInfo().mLife.mPlus );

		if( pOperator->GetMana() < mana )
		{
			SetEndState();
			return;
		}

		if( pOperator->GetLife() < life )
		{
			SetEndState();
			return;
		}

		// 071203 KTH --- BuffStatus에서 DecreaseManaRate 만큼의 마나 소모량을 줄여준다.
		mana = (DWORD) (mana + ( mana * (pOperator->GetRateBuffStatus()->DecreaseManaRate / 100 ) ) );
		//---
		if( mana )	pOperator->ReduceMana( mana );
		if( life )	pOperator->AddLife( life * -1, NULL );
	}
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD cBuffSkillObject::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
{
	if(isLogin)
	{
		return 0;
	}

	static MSG_SKILLOBJECT_ADD2 message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_SKILL;
	message.Protocol = MP_SKILL_SKILLOBJECT_ADD;
	message.dwObjectID = dwReceiverID;
	message.bCreate = FALSE;
	message.SkillObjectInfo.SkillObjectIdx = mSkillObjectInfo.skillObjectId;
	message.SkillObjectInfo.SkillIdx = GetSkillIdx();
	message.SkillObjectInfo.Pos = mSkillObjectInfo.pos;
	message.SkillObjectInfo.Direction = mSkillObjectInfo.skillDir;
	message.SkillObjectInfo.SkillLevel = ( BYTE )mSkillObjectInfo.level;
	message.SkillObjectInfo.Operator = mSkillObjectInfo.operatorId;
	message.SkillObjectInfo.Rate = 1.0f;
	message.SkillObjectInfo.Count = mCount;
	message.SkillObjectInfo.RemainTime = ( mDelayCheck ? mpDelay->Check() : 0 );
	memcpy( &( message.SkillObjectInfo.MainTarget ), &( mSkillObjectInfo.mainTarget ), sizeof( MAINTARGET ) );
	message.SkillObjectInfo.BattleID = GetBattleID();

	sendMessage = &message;
	return sizeof(message);
}

void cBuffSkillObject::ResetDelay()
{
	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject)
	{
		return;
	}

	mCount = GetInfo().Count;

	mpDelay->Init( GetInfo().DelayTime );

	if( mDelayCheck )
	{
		// 081203 LUJ, 지연 시간을 계산해서 가져온다. 때에 따라 스크립트가 적용되어 시간이 가감된다
		mpDelay->Init(
			GetDelayTime(targetedObject, &GetInfo()));
		mpDelay->Start();
	}

	MSG_DWORD2 msg;

	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_DELAY_RESET_NOTIFY;
	msg.dwData1 = mSkillObjectInfo.skillObjectId;
	msg.dwData2 = mpDelay->GetDelayTime();
	PACKEDDATA_OBJ->QuickSend(
		targetedObject,
		&msg,
		sizeof(msg));

	if( mDelayCheck )
	{
		if( mIsUpdate && ! GetInfo().NoUpdate )
		{
			CharacterBuffUpdate(
				mSkillObjectInfo.mainTarget.dwMainTargetID,
				GetSkillIdx(),
				mpDelay->GetRemainTime(),
				mCount);
		}
	}
	else
	{
		if( mIsUpdate && ! GetInfo().NoUpdate )
		{
			CharacterBuffUpdate(
				mSkillObjectInfo.mainTarget.dwMainTargetID,
				GetSkillIdx(),
				1,
				mCount);
		}
	}
}

// 090204 LUJ, 타입을 명확히 하고, 감소 회수를 지정함
void cBuffSkillObject::RemoveBuffCount( eBuffSkillCountType type, int count )
{
	if( 0 >= count )
	{
		return;
	}
	// 090204 LUJ, 음수도 검사함
	else if( 0 >= mCount )
	{
		return;
	}

	CObject* const targetedObject = g_pUserTable->FindUser(
		mSkillObjectInfo.mainTarget.dwMainTargetID);

	if(0 == targetedObject)
	{
		return;
	}

	switch( mCountType )
	{
	case eBuffSkillCountType_AnyDamageVolumn:
		{
			const BOOL isPhysicalDamageType = ( eBuffSkillCountType_PhysicalDamageVolumn == type );
			const BOOL isMagicalDamageType	= ( eBuffSkillCountType_MagicalDamageVolumn == type );

			mCount -= ( ( isPhysicalDamageType || isMagicalDamageType ) ? count : 0 );
			break;
		}
	case eBuffSkillCountType_AnyDamage:
		{
			const BOOL isPhysicalDamageType = ( eBuffSkillCountType_PhysicalDamageVolumn == type );
			const BOOL isMagicalDamageType	= ( eBuffSkillCountType_MagicalDamageVolumn == type );

			mCount -= ( ( isPhysicalDamageType || isMagicalDamageType ) ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_AnyHitVolumn:
		{
			const BOOL isPhysicalDamageType = ( eBuffSkillCountType_PhysicalHitVolumn == type );
			const BOOL isMagicalDamageType	= ( eBuffSkillCountType_MagicalDamageVolumn == type );

			mCount -= ( ( isPhysicalDamageType || isMagicalDamageType ) ? count : 0 );
			break;
		}
	case eBuffSkillCountType_AnyHit:
		{
			const BOOL isPhysicalDamageType = ( eBuffSkillCountType_PhysicalHitVolumn == type );
			const BOOL isMagicalDamageType	= ( eBuffSkillCountType_MagicalDamageVolumn == type );

			mCount -= ( ( isPhysicalDamageType || isMagicalDamageType ) ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_PhysicalDamage:
		{
			mCount -= ( eBuffSkillCountType_PhysicalDamageVolumn == type ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_PhysicalHit:
		{
			mCount -= ( eBuffSkillCountType_PhysicalHitVolumn == type ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_MagicalDamage:
		{
			mCount -= ( eBuffSkillCountType_MagicalDamageVolumn == type ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_MagicalHit:
		{
			mCount -= ( eBuffSkillCountType_MagicalHitVolumn == type ? 1 : 0 );
			break;
		}
	case eBuffSkillCountType_Heal:
		{
			mCount -= ( eBuffSkillCountType_HealVolumn == type ? 1 : 0 );
			break;
		}
		// 090330 LUJ, 경험치 보호 로그를 한번만 기록하기 위해 분리
	case eBuffSkillCountType_Dead:
		{
			if( mCountType != type )
			{
				break;
			}

			mCount -= count;
			CPlayer* const player = ( CPlayer* )targetedObject;
			const BOOL isNotPlayer = ( ! player || player->GetObjectKind() != eObjectKind_Player );

			if( isNotPlayer ||
				eStatusKind_ProtectExp != GetInfo().Status)
			{
				break;
			}

			InsertLogExp(
				eExpLog_ProtectExp,
				player->GetID(),
				player->GetLevel(),
				0,
				player->GetPlayerExpPoint(),
				0,
				0,
				mCount );
			break;
		}
		// 090330 LUJ, 누락된 처리 타입 추가
	case eBuffSkillCountType_BeHealed:
		{
			mCount -= ( eBuffSkillCountType_BeHealedVolumn == type ? 1 : 0 );
			break;
		}
	default:
		{
			mCount -= ( mCountType == type ? count : 0 );
			break;
		}
	}

	MSG_DWORD2 msg;

	msg.Category = MP_SKILL;
	msg.Protocol = MP_SKILL_COUNT_NOTIFY;
	msg.dwData1 = mSkillObjectInfo.skillObjectId;
	msg.dwData2 = max( 0, mCount );
	targetedObject->SendMsg( &msg, sizeof( msg ) );
	
	// 090204 LUJ, 음수도 검사하도록 함
	if( mCount <= 0 )
	{
		SetEndState();
		EndState();
	}
}

// 080616 LUJ, 버프 중에 이벤트를 처리한다
void cBuffSkillObject::Execute( const CEvent& event )
{
	CObject* object	= GetOperator();

	if( ! object )
	{
		return;
	}

	// 100310 ShinJS --- 특정 eStatusKind 만 처리하는 경우
	if( event.GetTargetStatus() != eStatusKind_None &&
		event.GetTargetStatus() != GetInfo().Status )
	{
		return;
	}

	// 100614 ShinJS --- OFF 된경우 이후처리를 하지 않는다.
	if( !mActive )
		return;

	struct
	{
		float operator() ( const BUFF_SKILL_INFO& info, float value ) const
		{
			switch( info.StatusDataType )
			{
			case BUFF_SKILL_INFO::StatusTypeAdd:
				{
					return max( 0, value + info.StatusData );
				}
			case BUFF_SKILL_INFO::StatusTypePercent:
				{
					// 080708 LUJ, 퍼센트 적용 시 항상 100%를 추가로 더하던 문제 수정
					return max( 0, value * info.StatusData / 100.0f );
				}
			}

			return 0;
		}
	}
	GetValue;

	// 090817 공성전 힐량 가중 가중치 스크립트 적용 
	BOOL bSiegeWar = FALSE ;
	float fSiegeWarWeight = 1.0f;

	if( SIEGEWARFAREMGR->Is_CastleMap() == FALSE &&
  		SIEGEWARFAREMGR->IsSiegeWarfareZone() &&
  		SIEGEWARFAREMGR->GetState() > eSiegeWarfare_State_Before )
	{
		bSiegeWar = TRUE ;
		fSiegeWarWeight = SIEGEWARFAREMGR->GetDecreaseHealRatio() /100 ;
	}

	switch( event.GetType() )
	{
		// 080616 LUJ, 다른 오브젝트에게 피해를 줬을 때 발동되는 이벤트
	case CEvent::TypeGiveDamage:
		{
			CGiveDamageEvent&	giveDamageEvent	= *( ( CGiveDamageEvent* )&event );
			const RESULTINFO&	result			= giveDamageEvent.GetResult();
			
			switch( GetInfo().Status )
			{
				// 080616 LUJ, 준 데미지를 HP로 바꾸자
			case eStatusKind_ConvertHealthWithGivedDamage:
				{
					DWORD dwValue = DWORD( GetValue(GetInfo(), float( result.RealDamage ) ) ) ; 

					if( bSiegeWar )
						dwValue =  DWORD( dwValue * fSiegeWarWeight );

					CCharacterCalcManager::StartUpdateLife(
						object,
						dwValue,
						1,
						0 );

					// 080616 LUJ, 몬스터 HP 변경 정보가 전송되지 않으므로 직접 전송한다
					if(object->GetObjectKind() & eObjectKind_Monster)
					{
						MSG_DWORD3 message;
						ZeroMemory( &message, sizeof( message ) );
						message.Category	= MP_BOSSMONSTER;
						message.Protocol	= MP_FIELD_LIFE_NOTIFY;
						message.dwData1		= object->GetLife();
						message.dwData2		= ( ( CMonster* )object )->GetMonsterKind();
						message.dwData3		= GetID();

						PACKEDDATA_OBJ->QuickSend( this, &message, sizeof( message ) );
					}
					break;
				}
				// 080616 LUJ, 준 데미지를 MP로 바꾸자
			case eStatusKind_ConvertManaWithGivedDamage:
				{
					if( eObjectKind_Player != object->GetObjectKind() )
					{
						break;
					}

					DWORD dwValue = DWORD( GetValue(GetInfo(), float( result.RealDamage ) ) ) ; 

					if( bSiegeWar )
						dwValue = DWORD( dwValue * fSiegeWarWeight );

					// 080616 LUJ, 몬스터 HP 변경 정보가 전송되지 않으므로 직접 전송한다
					CCharacterCalcManager::StartUpdateMana(
						( CPlayer* )object,
						dwValue ,
						1,
						0 );
					break;
				}
			}
			
			break;
		}
		// 080616 LUJ, 다른 오브젝트가 피해를 받았을 때 발동되는 이벤트
	case CEvent::TypeTakeDamage:
		{
			CTakeDamageEvent&	takeDamageEvent	= *( ( CTakeDamageEvent* )&event );
			RESULTINFO&	result					= takeDamageEvent.GetResult();

			switch( GetInfo().Status )
			{
				// 080616 LUJ, 입은 데미지를 반사하자
			case eStatusKind_Reflect:
				{
					CObject* attacker = takeDamageEvent.GetAttacker();
					cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo( result.mSkillIndex );

					if( !	attacker ||
						!	result.RealDamage ||
						!	pSkillInfo )
					{
						break;
					}

					// 100622 ShinJS 액티브스킬 & 물리공격만 반사가 가능하도록 수정.
					if( pSkillInfo->GetType() != cSkillInfo::TypeActive ||
						((cActiveSkillInfo*)pSkillInfo)->GetInfo().Unit != UNITKIND_PHYSIC_ATTCK )
					{
						break;
					}

					// 일정거리 이내에서만 반사 가능
					const float fValidDistance = (float)GetInfo().StatusDataType;
					VECTOR3 objectPos	= *CCharMove::GetPosition( object );
					VECTOR3 attackerPos	= *CCharMove::GetPosition( attacker );
					const float fDist = CalcDistanceXZ( &objectPos, &attackerPos );
					if( fValidDistance < fDist )
					{
						break;
					}

					RESULTINFO	damageResult( result );
					damageResult.RealDamage	= DWORD( max( 0, result.RealDamage * GetInfo().StatusData / 100.0f ) );
					
					if( bSiegeWar )
						damageResult.RealDamage = TARGETDATA( damageResult.RealDamage * fSiegeWarWeight );

					// 100310 ShinJS --- 데미지를 입는 대상이 마나실드를 사용중인 경우
					if( attacker->HasEventSkill( eStatusKind_DamageToManaDamage ) )
					{
						// eStatusKind_DamageToManaDamage 에 대한 처리만 할수 있도록 한다.
						attacker->Execute( CTakeDamageEvent( object, damageResult, eStatusKind_DamageToManaDamage ) );

						// Mana Damage
						attacker->ManaDamage( object, &damageResult );
					}

					attacker->Damage( object, &damageResult );

					switch( attacker->GetObjectKind() )
					{
					case eObjectKind_Monster:
					case eObjectKind_BossMonster:
					case eObjectKind_FieldBossMonster:
					case eObjectKind_FieldSubMonster:
						{
							if( attacker->GetLife() )
							{
								// 080616 LUJ, 몬스터 HP 변경 정보가 전송되지 않으므로 직접 전송한다
								MSG_DWORD3 message;
								ZeroMemory( &message, sizeof( message ) );
								message.Category	= MP_BOSSMONSTER;
								message.Protocol	= MP_FIELD_LIFE_NOTIFY;
								message.dwData1		= attacker->GetLife();
								message.dwData2		= ( ( CMonster* )attacker )->GetMonsterKind();
								message.dwData3		= attacker->GetID();

								PACKEDDATA_OBJ->QuickSend( this, &message, sizeof( message ) );
							}
							else
							{
								// 080616 LUJ, 반사 데미지로 사망한 경우, 자연스럽게 처리하기 위해서 아래 메시지를 전송해야 한다
								//				그렇지 않으면 갑자기 푹 사라진다
								ATTACKMGR->sendDieMsg( object, attacker );

								attacker->Die( object );
							}

							break;
						}
					}

					break;
				}
				// 080616 LUJ, 받은 데이지를 바꾸자
			case eStatusKind_Absorb:
				{
					// 080616 LUJ, 기획 미비. 미구현
					break;
				}
				// 080616 LUJ, 받은 데미지를 HP로 바꾸자
			case eStatusKind_DamageToLife:
				{
					CObject* attacker = takeDamageEvent.GetAttacker();

					if( ! attacker )
					{
						break;
					}

					DWORD dwValue = DWORD( GetValue( GetInfo(), float( result.RealDamage ) ) ) ; 

					if( bSiegeWar )
						dwValue =  DWORD( dwValue * fSiegeWarWeight);

					CCharacterCalcManager::StartUpdateLife(
						( CPlayer* )object,
						dwValue,
						1,
						0 );
					
					// 080616 LUJ, 몬스터 HP 변경 정보가 전송되지 않으므로 직접 전송한다
					if(object->GetObjectKind() & eObjectKind_Monster)
					{
						MSG_DWORD3 message;
						ZeroMemory( &message, sizeof( message ) );
						message.Category	= MP_BOSSMONSTER;
						message.Protocol	= MP_FIELD_LIFE_NOTIFY;
						message.dwData1		= object->GetLife();
						message.dwData2		= ( ( CMonster* )object )->GetMonsterKind();
						message.dwData3		= GetID();

						PACKEDDATA_OBJ->QuickSend( this, &message, sizeof( message ) );
					}

					break;
				}
				// 080616 LUJ, 받은 데미지를 MP로 바꾸자
			case eStatusKind_DamageToMana:
				{
					if( eObjectKind_Player != object->GetObjectKind() )
					{
						break;
					}

					DWORD dwValue = DWORD( GetValue( GetInfo(), float( result.RealDamage ) ) ) ; 

					if( bSiegeWar )
						dwValue = DWORD( dwValue * fSiegeWarWeight);

					CCharacterCalcManager::StartUpdateMana(
						( CPlayer* )object,
						dwValue ,
						1,
						0 );
					break;
				}

				// 100219 ShinJS --- 받은 데미지를 MP 데미지로 전환한다.
			case eStatusKind_DamageToManaDamage:
				{
					if( eObjectKind_Player != object->GetObjectKind() )
					{
						break;
					}

					const DWORD dwRealDamage = result.RealDamage;
					const DWORD dwManaDamage = DWORD( GetValue( GetInfo(), float( result.RealDamage ) ) );
					
					result.ManaDamage = dwManaDamage;
					result.RealDamage = 0;

					// 마나가 부족한 경우
					const DWORD dwCurMana = object->GetMana();
					if( dwCurMana < dwManaDamage )
					{
						const float fHPDamagePercent = float(dwManaDamage - dwCurMana) / (float)dwManaDamage;
						result.RealDamage = (TARGETDATA)(dwRealDamage * fHPDamagePercent);
						result.ManaDamage = dwCurMana;

						// 버프를 제거한다.
						SetEndState();
					}
				}
				break;
			}
			break;
		}
		// 080616 LUJ, 다른 오브젝트에게 피살될 때 발생하는 이벤트
	case CEvent::TypeDie:
		{
			CDieEvent& dieEvent	= *( ( CDieEvent* )&event );

			switch( GetInfo().Status )
			{
			case eStatusKind_EventSkillToVictim:
				{
					// 080708 LUJ, 피해자 오브젝트를 이벤트 클래스에서 획득하도록 함
					CObject* victim = dieEvent.GetVictim();

					if( ! victim )
					{
						break;
					}

					SKILLMGR->AddActiveSkill(
						*victim,
						GetInfo().mEventSkillIndex );
					
					break;
				}
			case eStatusKind_EventSkillToKiller:
				{
					CObject* killer = dieEvent.GetKiller();

					if( ! killer )
					{
						break;
					}

					SKILLMGR->AddActiveSkill(
						*killer,
						GetInfo().mEventSkillIndex );
					break;
				}
			}

			break;
		}
		// 080616 LUJ, 다른 오브젝트를 살해할 때 발생하는 이벤트
	case CEvent::TypeKill:
		{
			CKillEvent& killEvent = *( ( CKillEvent* )&event );

			switch( GetInfo().Status )
			{
			case eStatusKind_EventSkillToVictim:
				{
					CObject* victim = killEvent.GetVictim();

					if( ! victim )
					{
						break;
					}

					SKILLMGR->AddActiveSkill(
						*victim,
						GetInfo().mEventSkillIndex );

					break;
				}
			case eStatusKind_EventSkillToKiller:
				{
					// 080708 LUJ, 살인자 오브젝트를 이벤트 클래스에서 획득하도록 함
					CObject* killer = killEvent.GetKiller();

					if( ! killer )
					{
						break;
					}

					SKILLMGR->AddActiveSkill(
						*killer,
						GetInfo().mEventSkillIndex );

					break;
				}
			}

			break;
		}
	}
}

// 081203 LUJ, 유지 시간을 계산한다
DWORD cBuffSkillObject::GetDelayTime( CObject* object, const BUFF_SKILL_INFO* skillInfo ) const
{
	// 090106 LUJ, 값이 유효한지 검사한다
	if(	! skillInfo ||
		! object )
	{
		return 0;
	}
	// 090106 LUJ, 대상이 플레이어가 아닌 경우 기본 값을 반환한다
	else if( eObjectKind_Player != object->GetObjectKind() )
	{
		return skillInfo->DelayTime;
	}

	CPlayer* const		player				= ( CPlayer* )object;
	const Status* const	buffStatus			= player->GetBuffStatus();
	const Status* const	buffRateStatus		= player->GetRateBuffStatus();
	const Status* const	passiveStatus		= player->GetPassiveStatus();
	const Status* const	passiveRateStatus	= player->GetRatePassiveStatus();
	
	if( !	buffStatus		||
		!	buffRateStatus	||
		!	passiveStatus	||
		!	passiveRateStatus )
	{
		return 0;
	}

	const SkillScript& script = GAMERESRCMNGR->GetSkillScript( skillInfo->Index );
	
	// 081203 LUJ, 보너스 사항이 없으면 스킬에 설정된 값을 반환한다
	if( script.mDurationBonusList.empty() )
	{
		return skillInfo->DelayTime;
	}

	const PlayerStat&	playerStatus		= player->GetCharStats();
	const PlayerStat&	itemBaseStatus		= player->GetItemBaseStats();
	const PlayerStat&	itemOptionStatus	= player->GetItemOptionStats();
	float				bonus			= 0;

	for(	SkillScript::ValueList::const_iterator it = script.mDurationBonusList.begin();
			script.mDurationBonusList.end() != it;
			++it )
	{
		const SkillScript::Value& value = *it;

		switch( value.mType )
		{
		case SkillScript::TypeStrength:
			{
				const float	plusValue		= playerStatus.mStrength.mPlus		+ itemBaseStatus.mStrength.mPlus	+ itemOptionStatus.mStrength.mPlus	+ buffStatus->Str		+ passiveStatus->Str;
				const float percentValue	= playerStatus.mStrength.mPercent	+ itemBaseStatus.mStrength.mPercent + itemOptionStatus.mStrength.mPercent + buffRateStatus->Str	+ passiveRateStatus->Str;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeDexterity:
			{
				const float	plusValue		= playerStatus.mDexterity.mPlus		+ itemBaseStatus.mDexterity.mPlus		+ itemOptionStatus.mDexterity.mPlus		+ buffStatus->Dex		+ passiveStatus->Dex;
				const float percentValue	= playerStatus.mDexterity.mPercent	+ itemBaseStatus.mDexterity.mPercent	+ itemOptionStatus.mDexterity.mPercent	+ buffRateStatus->Dex	+ passiveRateStatus->Dex;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeVitality:
			{
				const float	plusValue		= playerStatus.mVitality.mPlus		+ itemBaseStatus.mVitality.mPlus	+ itemOptionStatus.mVitality.mPlus	+ buffStatus->Vit		+ passiveStatus->Vit;
				const float percentValue	= playerStatus.mVitality.mPercent	+ itemBaseStatus.mVitality.mPercent + itemOptionStatus.mVitality.mPercent + buffRateStatus->Vit	+ passiveRateStatus->Vit;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeIntelligence:
			{
				const float	plusValue		= playerStatus.mIntelligence.mPlus		+ itemBaseStatus.mIntelligence.mPlus	+ itemOptionStatus.mIntelligence.mPlus	+ buffStatus->Int		+ passiveStatus->Int;
				const float percentValue	= playerStatus.mIntelligence.mPercent	+ itemBaseStatus.mIntelligence.mPercent	+ itemOptionStatus.mIntelligence.mPercent + buffRateStatus->Int	+ passiveRateStatus->Int;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeWisdom:
			{
				const float	plusValue		= playerStatus.mWisdom.mPlus	+ itemBaseStatus.mWisdom.mPlus		+ itemOptionStatus.mWisdom.mPlus		+ buffStatus->Wis		+ passiveStatus->Wis;
				const float percentValue	= playerStatus.mWisdom.mPercent	+ itemBaseStatus.mWisdom.mPercent	+ itemOptionStatus.mWisdom.mPercent	+ buffRateStatus->Wis	+ passiveRateStatus->Wis;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeLife:
			{
				const float	plusValue		= playerStatus.mLife.mPlus		+ itemBaseStatus.mLife.mPlus	+ itemOptionStatus.mLife.mPlus	+ buffStatus->MaxLife		+ passiveStatus->MaxLife;
				const float percentValue	= playerStatus.mLife.mPercent	+ itemBaseStatus.mLife.mPercent + itemOptionStatus.mLife.mPercent + buffRateStatus->MaxLife	+ passiveRateStatus->MaxLife;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeMana:
			{
				const float	plusValue		= playerStatus.mMana.mPlus		+ itemBaseStatus.mMana.mPlus	+ itemOptionStatus.mMana.mPlus	+ buffStatus->MaxMana		+ passiveStatus->MaxMana;
				const float percentValue	= playerStatus.mMana.mPercent	+ itemBaseStatus.mMana.mPercent + itemOptionStatus.mMana.mPercent + buffRateStatus->MaxMana	+ passiveRateStatus->MaxMana;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeLifeRecovery:
			{
				const float	plusValue		= playerStatus.mRecoveryLife.mPlus		+ itemBaseStatus.mRecoveryLife.mPlus	+ itemOptionStatus.mRecoveryLife.mPlus	+ buffStatus->LifeRecover		+ passiveStatus->LifeRecover;
				const float percentValue	= playerStatus.mRecoveryLife.mPercent	+ itemBaseStatus.mRecoveryLife.mPercent + itemOptionStatus.mRecoveryLife.mPercent + buffRateStatus->LifeRecover	+ passiveRateStatus->LifeRecover;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeManaRecovery:
			{
				const float	plusValue		= playerStatus.mRecoveryMana.mPlus		+ itemBaseStatus.mRecoveryMana.mPlus	+ itemOptionStatus.mRecoveryMana.mPlus	+ buffStatus->ManaRecover		+ passiveStatus->ManaRecover;
				const float percentValue	= playerStatus.mRecoveryMana.mPercent	+ itemBaseStatus.mRecoveryMana.mPercent + itemOptionStatus.mRecoveryMana.mPercent + buffRateStatus->ManaRecover	+ passiveRateStatus->ManaRecover;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypePhysicAttack:
			{
				const float	plusValue		= playerStatus.mPhysicAttack.mPlus		+ itemBaseStatus.mPhysicAttack.mPlus	+ itemOptionStatus.mPhysicAttack.mPlus	+ buffStatus->PhysicAttack		+ passiveStatus->PhysicAttack;
				const float percentValue	= playerStatus.mPhysicAttack.mPercent	+ itemBaseStatus.mPhysicAttack.mPercent + itemOptionStatus.mPhysicAttack.mPercent + buffRateStatus->PhysicAttack	+ passiveRateStatus->PhysicAttack;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypePhysicDefence:
			{
				const float	plusValue		= playerStatus.mPhysicDefense.mPlus		+ itemBaseStatus.mPhysicDefense.mPlus		+ itemOptionStatus.mPhysicDefense.mPlus		+ buffStatus->PhysicDefense		+ passiveStatus->PhysicDefense;
				const float percentValue	= playerStatus.mPhysicDefense.mPercent	+ itemBaseStatus.mPhysicDefense.mPercent	+ itemOptionStatus.mPhysicDefense.mPercent	+ buffRateStatus->PhysicDefense	+ passiveRateStatus->PhysicDefense;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeMagicAttack:
			{
				const float	plusValue		= playerStatus.mMagicAttack.mPlus		+ itemBaseStatus.mMagicAttack.mPlus		+ itemOptionStatus.mMagicAttack.mPlus		+ buffStatus->MagicAttack		+ passiveStatus->MagicAttack;
				const float percentValue	= playerStatus.mMagicAttack.mPercent	+ itemBaseStatus.mMagicAttack.mPercent	+ itemOptionStatus.mMagicAttack.mPercent	+ buffRateStatus->MagicAttack	+ passiveRateStatus->MagicAttack;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeMagicDefence:
			{
				const float	plusValue		= playerStatus.mMagicDefense.mPlus		+ itemBaseStatus.mMagicDefense.mPlus	+ itemOptionStatus.mMagicDefense.mPlus	+ buffStatus->MagicDefense		+ passiveStatus->MagicDefense;
				const float percentValue	= playerStatus.mMagicDefense.mPercent	+ itemBaseStatus.mMagicDefense.mPercent + itemOptionStatus.mMagicDefense.mPercent + buffRateStatus->MagicDefense	+ passiveRateStatus->MagicDefense;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeMoveSpeed:
			{
				const float	plusValue		= playerStatus.mMoveSpeed.mPlus		+ itemBaseStatus.mMoveSpeed.mPlus		+ itemOptionStatus.mMoveSpeed.mPlus		+ buffStatus->MoveSpeed		+ passiveStatus->MoveSpeed;
				const float percentValue	= playerStatus.mMoveSpeed.mPercent	+ itemBaseStatus.mMoveSpeed.mPercent	+ itemOptionStatus.mMoveSpeed.mPercent	+ buffRateStatus->MoveSpeed	+ passiveRateStatus->MoveSpeed;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeEvade:
			{
				const float	plusValue		= playerStatus.mEvade.mPlus		+ itemBaseStatus.mEvade.mPlus		+ itemOptionStatus.mEvade.mPlus		+ buffStatus->Avoid		+ passiveStatus->Avoid;
				const float percentValue	= playerStatus.mEvade.mPercent	+ itemBaseStatus.mEvade.mPercent	+ itemOptionStatus.mEvade.mPercent	+ buffRateStatus->Avoid	+ passiveRateStatus->Avoid;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeAccuracy:
			{
				const float	plusValue		= playerStatus.mAccuracy.mPlus		+ itemBaseStatus.mAccuracy.mPlus	+ itemOptionStatus.mAccuracy.mPlus	+ buffStatus->Accuracy		+ passiveStatus->Accuracy;
				const float percentValue	= playerStatus.mAccuracy.mPercent	+ itemBaseStatus.mAccuracy.mPercent + itemOptionStatus.mAccuracy.mPercent + buffRateStatus->Accuracy	+ passiveRateStatus->Accuracy;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeCriticalRate:
			{
				const float	plusValue		= playerStatus.mCriticalRate.mPlus		+ itemBaseStatus.mCriticalRate.mPlus	+ itemOptionStatus.mCriticalRate.mPlus	+ buffStatus->CriticalRate		+ passiveStatus->CriticalRate;
				const float percentValue	= playerStatus.mCriticalRate.mPercent	+ itemBaseStatus.mCriticalRate.mPercent + itemOptionStatus.mCriticalRate.mPercent + buffRateStatus->CriticalRate	+ passiveRateStatus->CriticalRate;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		case SkillScript::TypeCriticalDamage:
			{
				const float	plusValue		= playerStatus.mCriticalDamage.mPlus	+ itemBaseStatus.mCriticalDamage.mPlus		+ itemOptionStatus.mCriticalDamage.mPlus		+ buffStatus->CriticalDamage		+ passiveStatus->CriticalDamage;
				const float percentValue	= playerStatus.mCriticalDamage.mPercent	+ itemBaseStatus.mCriticalDamage.mPercent	+ itemOptionStatus.mCriticalDamage.mPercent	+ buffRateStatus->CriticalDamage	+ passiveRateStatus->CriticalDamage;

				bonus	+= plusValue * ( 1.0f + percentValue / 100.0f ) * value.mPlus;
				break;
			}
		}
	}

	return DWORD( max( 0, float( skillInfo->DelayTime ) + bonus ) );
}

// 081203 LUJ, 설정된 유지 시간을 반환한다
DWORD cBuffSkillObject::GetDelayTime() const
{
	return mpDelay ? mpDelay->GetDelayTime() : 0;
}

const BUFF_SKILL_INFO& cBuffSkillObject::GetInfo() const
{
	return mSkillInfo->GetInfo();
}

void cBuffSkillObject::AddAggro(CObject& targetObject)
{
	CObject* const operatorObject = g_pUserTable->FindUser(
		mSkillObjectInfo.operatorId);

	if(0 == operatorObject ||
		eObjectKind_Player != operatorObject->GetObjectKind())
	{
		return;
	}

	const int aggroPoint = int(GetSkillLevel());

	switch(targetObject.GetObjectKind())
	{
	case eObjectKind_Pet:
		{
			CPet& pet = (CPet&)targetObject;
			pet.AddAggroToMyMonsters(
				aggroPoint,
				mSkillObjectInfo.operatorId,
				GetSkillIdx());
			break;
		}
	case eObjectKind_Player:
		{
			CPlayer& player = (CPlayer&)targetObject;
			player.AddAggroToMyMonsters(
				aggroPoint,
				mSkillObjectInfo.operatorId,
				GetSkillIdx());
			break;
		}
	case eObjectKind_Monster:
		{
			CMonster& monster = (CMonster&)targetObject;
			monster.AddAggro(
				mSkillObjectInfo.operatorId,
				aggroPoint,
				GetSkillIdx());
			break;
		}
	}
}