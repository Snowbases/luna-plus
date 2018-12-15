#include "stdafx.h"
#include ".\activeskillobject.h"
#include "..\Delay\Delay.h"
#include "..\Info\ActiveSkillInfo.h"
#include "..\Info\BuffSkillInfo.h"
#include "../Unit/AttackSkillUnit.h"
#include "../Unit/RecoverSkillUnit.h"
#include "../Unit/cDeBuffSkillUnit.h"
#include "../Unit/cSpecialSkillUnit.h"
#include "../Unit/SummonSkillUnit.h"
#include "../Unit/TameSkillUnit.h"
#include "..\Object\BuffSkillObject.h"
#include "..\Manager\SkillManager.h"
#include "..\Target\Target.h"
#include "Player.h"
#include "../[CC]BattleSystem/Battle.h"
#include "PackedData.h"
#include "UserTable.h"
#include "MapDBMsgParser.h"
#include "pet.h"
#include "ItemManager.h"
#include "Network.h"

cActiveSkillObject::cActiveSkillObject(const cActiveSkillInfo& skillInfo) :
cSkillObject(skillInfo.GetInfo()),
mSkillInfo(&skillInfo),
mTarget(new cTarget),
mpSkillUnit(0),
mCastingTick(0),
mProcessTick(0),
mCoolingTick(0)
{
	// 080602 LUJ, 오브젝트 타입을 초기화한다
	mType = cSkillObject::TypeActive;
	m_BuffSkillTable.Initialize( 3 );

	mpSkillUnit = CreateUnit(
		skillInfo.GetInfo());
}

cActiveSkillObject::~cActiveSkillObject(void)
{
	SAFE_DELETE(
		mpSkillUnit);
}

void cActiveSkillObject::Init( sSKILL_CREATE_INFO* pInfo )
{
	cSkillObject::Init(
		pInfo);

	GetTarget().RemoveAll();

	// 광역 스킬은 시전자가 포함될지 여부를 클라이언트에서 조회해온다
	if(TARGET_KIND_SINGLE == GetInfo().Area )
	{
		GetTarget().AddData(
			pInfo->mainTarget.dwMainTargetID);
	}

	m_BuffSkillTable.RemoveAll();

	for(DWORD i = 0; i < MAX_BUFF_COUNT; ++i)
	{
		const cBuffSkillInfo* const buffSkillInfo = SKILLMGR->GetBuffInfo(
			GetInfo().Buff[i]);

		if(0 == buffSkillInfo)
		{
			break;
		}
		else if(buffSkillInfo->GetInfo().IsEndTime &&
			buffSkillInfo->GetInfo().NoUpdate)
		{
			CharacterBuffAdd(
				GetOperator()->GetID(),
				buffSkillInfo->GetIndex(),
				buffSkillInfo->GetInfo().DelayTime,
				buffSkillInfo->GetInfo().Count,
				TRUE);
		}
	}

	mAttackSpeedRate = 1.0f;

	if( GetOperator()->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = ( CPlayer* )GetOperator();

		if( GetInfo().Kind == SKILLKIND_PHYSIC ||
			GetInfo().Kind == SKILLKIND_MAGIC )
		{
			if( (GetSkillIdx() / 100000) % 10 )
			{
				switch( GetInfo().Unit )
				{
				case UNITKIND_PHYSIC_ATTCK:
				case UNITKIND_PHYSIC_FAKE_DAMAGE:
					{
						mAttackSpeedRate += ( ( pPlayer->GetRateBuffStatus()->PhysicSkillSpeedRate + pPlayer->GetRatePassiveStatus()->PhysicSkillSpeedRate ) * 0.01f );
					}
					break;

				case UNITKIND_NONE:
				case UNITKIND_MAGIC_ATTCK:
				case UNITKIND_LIFE_RECOVER:
				case UNITKIND_RESURRECTION:
				case UNITKIND_DEBUFF:
				case UNITKIND_MANA_RECOVER:
				case UNITKIND_MAGIC_FAKE_DAMAGE:
					{
						float SpeedRate = min( pPlayer->GetRateBuffStatus()->MagicSkillSpeedRate + pPlayer->GetRatePassiveStatus()->MagicSkillSpeedRate, 99.f );
						mAttackSpeedRate = ( 1 / ( 1 - ( SpeedRate*0.01f ) ) );
						if( (GetInfo().CastingTime / mAttackSpeedRate) < MIN_MAGIC_CASTING_TIME )
							mAttackSpeedRate = GetInfo().CastingTime / MIN_MAGIC_CASTING_TIME;
					}
					break;
				}
			}
			else
			{
				mAttackSpeedRate += ( ( pPlayer->GetRateBuffStatus()->NormalSpeedRate + pPlayer->GetRatePassiveStatus()->NormalSpeedRate ) * 0.01f );
			}
		}
	}
	/// 플레이어의 경우
	if( GetOperator()->GetObjectKind() == eObjectKind_Player )
	{
		mCoolingTick = gCurTime + GetInfo().CoolTime;
	}

	if( mAttackSpeedRate <= 0 )
	{
		mAttackSpeedRate = 1.0f;
	}

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
	msg.SkillObjectInfo.Rate = mAttackSpeedRate;
	msg.SkillObjectInfo.RemainTime = 0;
	msg.SkillObjectInfo.Count = 0;

	memcpy( &( msg.SkillObjectInfo.MainTarget ), &( pInfo->mainTarget ), sizeof( MAINTARGET ) );

	msg.SkillObjectInfo.BattleID = GetBattleID();

	PACKEDDATA_OBJ->QuickSend( GetOperator(), &msg, sizeof( MSG_SKILLOBJECT_ADD2 ) );

	GetOperator()->CurCastingSkillID = GetID();

	if(0 < GetInfo().CastingTime)
	{
		CastingStart();
	}
	else
	{
		ProcessStart();
	}
}

SKILL_STATE cActiveSkillObject::Update()
{
	if( !GetOperator() )
	{
		mState = SKILL_STATE_DESTROY;
	}

	if( mState == SKILL_STATE_DESTROY )
	{
		return cSkillObject::Update();
	}

	switch( mState )
	{
	/// 시전중
	case SKILL_STATE_CASTING:
		{
			CastingState();
		}
		break;
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
	case SKILL_STATE_DESTROY:
		break;
	}

	return cSkillObject::Update();
}

void cActiveSkillObject::Excute()
{
	// 081203 LUJ, 공격 스킬이 실패했을 경우에만 참으로 설정된다
	BOOL attackUnitIsFailed = FALSE;

	// 081203 LUJ, 유닛 실행 결과에 따라 플래그 값을 설정한다
	if( mpSkillUnit )
	{
		const BOOL		unitIsFailed	= ( ! mpSkillUnit->Excute() );
		const UNITKIND	kind			= mpSkillUnit->GetKind();
		const BOOL		isAttackUnit	= ( UNITKIND_MAGIC_ATTCK == kind || UNITKIND_PHYSIC_ATTCK == kind );

		attackUnitIsFailed = ( unitIsFailed && isAttackUnit );
	}

	CObject* const pOperator = GetOperator();

	if(0 == pOperator)
	{
		mState = SKILL_STATE_DESTROY;
		return;
	}

	pOperator->CurCastingSkillID = 0;

	if(FALSE == Consume(*pOperator))
	{
		mState = SKILL_STATE_DESTROY;
		return;
	}

	if(GetInfo().IsMove)
	{
		GetTarget().SetPositionHead();

		while(CObject* const pTarget = GetTarget().GetData())
		{
			if( pTarget->GetState()  == eObjectState_Die )
			{
				continue;
			}

			pTarget->SetEffectMove();
		}
	}

	// 081203 LUJ, 공격 스킬이 실패한 경우에는 버프를 실행하지 않는다
	for(	int i = ( attackUnitIsFailed ? MAX_BUFF_COUNT : 0 );
			i < MAX_BUFF_COUNT;
			++i )
	{
		if(GetInfo().BuffRate[i] <= rand() % 100)
		{
			continue;
		}

		cBuffSkillInfo* const buffSkillInfo = (cBuffSkillInfo*)SKILLMGR->GetBuffInfo(
			GetInfo().Buff[i]);

		if(0 == buffSkillInfo)
		{
			continue;
		}

		GetTarget().SetPositionHead();

		while(CObject* const pTarget = GetTarget().GetData())
		{
			if( pTarget->GetState()  == eObjectState_Die )
			{
				continue;
			}
			else if(const AbnormalStatus* const status = pTarget->GetAbnormalStatus())
			{
				if(status->IsUnableBuff)
				{
					continue;
				}
			}

			sSKILL_CREATE_INFO skillCreateInfo = mSkillObjectInfo;
			skillCreateInfo.mainTarget.SetMainTarget(
				pTarget->GetID());

			if(cBuffSkillObject* pBuff = (cBuffSkillObject*)SKILLMGR->OnBuffSkillStart(buffSkillInfo, &skillCreateInfo))
			{
				m_BuffSkillTable.Add(
					pBuff,
					pBuff->GetID());
				pTarget->AddEventSkill(
					*pBuff);
			}
		}
	}
}

void cActiveSkillObject::CastingStart()
{
	mCastingTick = gCurTime + DWORD(GetInfo().CastingTime / mAttackSpeedRate);
	mState = SKILL_STATE_CASTING;
}

void cActiveSkillObject::CastingState()
{
	if(gCurTime > mCastingTick)
	{
		Excute();

		mState = SKILL_STATE_END;
	}
}

void cActiveSkillObject::ProcessStart()
{
	switch(GetInfo().DelayType)
	{
		/// 발동 딜레이 없음
	case DELAY_NONE:
		{
			/// 스킬 종료 상태로 변경
			mState = SKILL_STATE_END;

			/// 스킬 실행
			Excute();
		}
		break;
		/// 시간형
	case DELAY_TIME:
		{
			mProcessTick = gCurTime + DWORD(GetInfo().DelayTime / mAttackSpeedRate);
			mState = SKILL_STATE_PROCESS;
		}
		break;
		/// 이동형
	case DELAY_SCOPE:
		{
			VECTOR3 oper;
			VECTOR3 tar;

			/// 시전자 위치
			GetOperator()->GetPosition( &oper );

			/// 타겟 위치
			tar.x = mSkillObjectInfo.mainTarget.cpTargetPos.wx;
			tar.z = mSkillObjectInfo.mainTarget.cpTargetPos.wz;

			const DWORD distance = ( DWORD )CalcDistanceXZ( &oper, &tar );
			mProcessTick = ( distance / GetInfo().DelayTime ) * 1000 + gCurTime;
			mState = SKILL_STATE_PROCESS;
		}
		break;
	}
}

void cActiveSkillObject::ProcessState()
{
	if(gCurTime > mProcessTick)
	{
		/// 실행
		Excute();

		mState = SKILL_STATE_END;
	}
}

void cActiveSkillObject::RemoveAllBuffSkillObject()
{
	m_BuffSkillTable.SetPositionHead();

	while(cBuffSkillObject* object = m_BuffSkillTable.GetData())
	{
		object->SetEndState();
		object->EndState();
	}
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD cActiveSkillObject::SetAddMsg(DWORD dwReceiverID, BOOL isLogin, MSGBASE*& sendMessage)
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
	message.SkillObjectInfo.Rate = mAttackSpeedRate;
	memcpy( &( message.SkillObjectInfo.MainTarget ), &( mSkillObjectInfo.mainTarget ), sizeof( MAINTARGET ) );
	message.SkillObjectInfo.BattleID = GetBattleID();

	sendMessage = &message;
	return sizeof(message);
}

// 090109 LUJ, 캐스팅 중인지 반환한다
BOOL cActiveSkillObject::IsCasting() const
{
	return gCurTime < mCastingTick;
}

void cActiveSkillObject::SetMultiTarget(CTargetList* pTList)
{
	if(TARGET_KIND_MULTI > GetInfo().Area)
	{
		return;
	}

	// 090108 LUJ, 스크립트에 정의된 허용 범위값을 가져온다
	const float validArea = GetInfo().AreaData + cSkillObject::GetAllowRangeForWideSkill();

	// 090108 LUJ, 주 목표의 위치를 알아낸다
	VECTOR3 mainTargetPosition = { 0 };
	{
		CObject* mainTargetObject = 0;
		GetMainTargetPos(
			&mSkillObjectInfo.mainTarget,
			&mainTargetPosition,
			&mainTargetObject );

		if( mainTargetObject )
		{
			mainTargetObject->GetPosition( &mainTargetPosition );
		}
	}

	CTargetListIterator iter(pTList);

	while(iter.GetNextTarget())
	{
		CObject* const pObject = g_pUserTable->FindUser(iter.GetTargetID());

		if(pObject == NULL)
		{
			continue;
		}

		// 090108 LUJ, 타겟의 위치를 알아낸다
		VECTOR3 targetPosition = { 0 };
		pObject->GetPosition( &targetPosition );

		// 090108 LUJ, 주 타겟을 중심으로 범위가 유효한 경우에만 타겟으로 등록시킨다
		if(validArea < CalcDistanceXZ(&mainTargetPosition, &targetPosition))
		{
			continue;
		}

		GetTarget().AddData(
			pObject->GetID());
	}

	iter.Release();
}

const ACTIVE_SKILL_INFO& cActiveSkillObject::GetInfo() const
{
	return mSkillInfo->GetInfo();
}

cSkillUnit* cActiveSkillObject::CreateUnit(const ACTIVE_SKILL_INFO& skillInfo)
{
	switch(skillInfo.Unit)
	{
	case UNITKIND_PHYSIC_ATTCK:
	case UNITKIND_MAGIC_ATTCK:
		{
			return new cAttackSkillUnit(
				*this);
		}
	case UNITKIND_LIFE_RECOVER:
	case UNITKIND_MANA_RECOVER:
		{
			return new cRecoverSkillUnit(
				*this);
		}
	case UNITKIND_DEBUFF:
		{
			return new cDeBuffSkillUnit(
				*this);
		}
	case UNITKIND_RESURRECTION:
	case UNITKIND_FAKE_DAMAGE:
	case UNITKIND_PHYSIC_FAKE_DAMAGE:
	case UNITKIND_MAGIC_FAKE_DAMAGE:
	case UNITKIND_DECOY:
	case UNITKIND_RETURN:
	case UNITKIND_SPELLBREAK:
		{
			return new cSpecialSkillUnit(
				*this);
		}
	case UNITKIND_SUMMON:
		{
			return new cSummonSkillUnit(
				*this);
		}
	case UNITKIND_TAME:
		{
			return new cTameSkillUnit(
				*this);
		}
	}

	return 0;
}

BOOL cActiveSkillObject::Consume(CObject& object) const
{
	if(FALSE == mSkillInfo->IsExcutableSkillState(
		object,
		mSkillObjectInfo.usedItem))
	{
		return FALSE;
	}
	else if(eObjectKind_Player == object.GetObjectKind())
	{
		CPlayer& player = (CPlayer&)object;

		if(0 < GetInfo().Money)
		{
			player.SetMoney(
				GetInfo().Money,
				MONEY_SUBTRACTION,
				MF_LOST);
			LogItemMoney(
				player.GetID(),
				player.GetObjectName(),
				GetInfo().Index,
				"*skill",
				eLog_SkillMoneyUse,
				player.GetMoney() + GetInfo().Money,
				player.GetMoney(),
				GetInfo().Money,
				0,
				0,
				0,
				0,
				0,
				0);
		}

		const eItemUse_Err itemUseError = ITEMMGR->UseItem(
			&player,
			mSkillObjectInfo.usedItem.Position,
			mSkillObjectInfo.usedItem.wIconIdx);

		if(eItemUseSuccess == itemUseError)
		{
			MSG_ITEM_USE_ACK message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_ITEM;
			message.Protocol = MP_ITEM_USE_ACK;
			message.TargetPos = mSkillObjectInfo.usedItem.Position;
			message.dwItemIdx = mSkillObjectInfo.usedItem.wIconIdx;
			message.eResult = itemUseError;

			player.SendMsg(
				&message,
				sizeof(message));
		}

		switch(GetInfo().mConsumePoint.mType)
		{
		case ACTIVE_SKILL_INFO::ConsumePoint::TypeExperience:
			{
				player.ReduceExpPoint(
					GetInfo().mConsumePoint.mValue,
					eExpLog_LoseBySkillUse);
				break;
			}
		case ACTIVE_SKILL_INFO::ConsumePoint::TypePlayerKill:
			{
				const DWORD badFame = player.GetBadFame() - GetInfo().mConsumePoint.mValue;
				player.SetBadFame(
					badFame);
				BadFameCharacterUpdate(
					player.GetID(),
					badFame);

				MSG_FAME message;
				message.Category = MP_CHAR;
				message.Protocol = MP_CHAR_BADFAME_NOTIFY;
				message.dwObjectID = player.GetID();
				message.Fame = badFame;

				PACKEDDATA_OBJ->QuickSend(
					&player,
					&message,
					sizeof(message));
				break;
			}
		}

		// LUJ, 전역 스킬이 실행될 경우 패킷을 전송한다
		if(TARGET_KIND_WORLD == GetInfo().Area)
		{
			MSG_ITEM_SKILL_START_TO_MAP message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_SKILL;
			message.Protocol = MP_SKILL_START_TO_MAP;
			message.mGuildIndex = player.GetGuildIdx();
			message.mFamilyIndex = player.GetFamilyIdx();

			switch(GetInfo().AreaTarget)
			{
			case eSkillAreaTargetGuild:
				{
					message.dwObjectID = player.GetGuildIdx();
					break;
				}
			case eSkillAreaTargetFamily:
				{
					message.dwObjectID = player.GetFamilyIdx();
					break;
				}
			case eSkillAreaTargetParty:
				{
					message.dwObjectID = player.GetPartyIdx();
					break;
				}
			}

			message.mSkill.wSkillIdx = GetSkillIdx();
			message.mSkill.Level = GetSkillLevel();

			SKILLMGR->NetworkMsgParse(
				message.Protocol,
				&message);
			g_Network.Broadcast2AgentServer(
				(char*)&message,
				sizeof(message));
		}
	}

	const float manaBonus = 1.0f + object.GetRateBuffStatus()->DecreaseManaRate / 100.0f;
	const float usedMana = float(object.GetMana()) * GetInfo().mMana.mPercent + GetInfo().mMana.mPlus;
	const float usedLife = float(object.GetLife()) * GetInfo().mLife.mPercent + GetInfo().mLife.mPlus;

	object.ReduceMana(
		DWORD(manaBonus * usedMana));
	object.ReduceLife(
		DWORD(usedLife));

	return TRUE;
}