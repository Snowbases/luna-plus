#include "StdAfx.h"
#include "BossMonster.h"
#include "BossMonsterManager.h"
#include "BossMonsterInfo.h"
#include "CharMove.h"
#include "PackedData.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "CharacterCalcManager.h"
#include "MHError.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "ObjectStateManager.h"
#include "Player.h"
#include "Pet.h"
#include "UserTable.h"
#include "GridSystem.h"
#include "StateMachinen.h"
#include "../[cc]skill/server/info/ActiveSkillInfo.h"

CBossMonster::CBossMonster()
{
}

CBossMonster::~CBossMonster()
{
}

BOOL CBossMonster::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	CMonster::Init(kind, AgentNum, pBaseObjectInfo);
	
	/// 06. 08. 2차 보스 - 이영준
	/*m_CurAttackIdx = 1601;*/
	m_AttackStartTime = 0 ;
	m_EventActionValue = 0;
	m_BossEventState = NULL;
	m_IsEventStating = FALSE;
	DistributeDamageInit();
	m_bOpenning = TRUE;
	
	BOSSMONMGR->AddBossMonster(this);

	return TRUE;
}

void CBossMonster::DoDie(CObject* pAttacker)
{
	BOSSMONMGR->RegenGroup(this, GetSummonFileNum(), m_pBossMonsterInfo->GetDieGroupID());
	
	// 리젠 정보 등록
	BOSSMONMGR->SetBossRandRegenChannel(GetMonsterKind(), GetGridID(), GetMonsterGroupNum());
	BOSSMONMGR->DeleteBossMonster(this);

	const DWORD AttackerID = (pAttacker ? pAttacker->GetID() : 0);
	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,AttackerID);
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die,MONSTERREMOVE_TIME);
	SetTObject(0);
	RemoveAllAggro();
//-------------------
}

void CBossMonster::Release()
{
	if(m_BossEventState)
	{
		delete [] m_BossEventState;
		m_BossEventState = NULL;
	}
	CMonster::Release();
}

void CBossMonster::SetBossInfo(CBossMonsterInfo* pInfo)
{
	m_pBossMonsterInfo = pInfo;
	m_CurAttackIdx = pInfo->GetFirstAttackIdx();
	m_AttackStartTime = 0 ;
	m_EventActionValue = 0;
	
	BOSSEVENTSTATE* pEventState = pInfo->GetEventStateInfo();
	if(pInfo->GetMaxEventStateNum() == 0)
	{
		ASSERTMSG(0, "BossEventState Num is Zero");
	}
	else
	{		
		m_BossEventState = new BOSSEVENTSTATE[pInfo->GetMaxEventStateNum()];
		for(int i=0; i<pInfo->GetMaxEventStateNum(); ++i)
		{
			m_BossEventState[i].Action = pEventState[i].Action;
			m_BossEventState[i].ActionValue = pEventState[i].ActionValue;
			m_BossEventState[i].Condition = pEventState[i].Condition;
			m_BossEventState[i].ConditionValue = pEventState[i].ConditionValue;
			m_BossEventState[i].Count = pEventState[i].Count;
		}
	}
	m_BossState.Init();
	m_IsEventStating = FALSE;

	m_bDelete = FALSE;
}

void CBossMonster::SetLife(DWORD Life, BOOL bSendMsg)
{
	CMonster::SetLife(Life, bSendMsg);	
	
	if(m_BossEventState == 0)
	{
		ASSERTMSG(0, "Boss SetLife EventState is NULL");
	}
	else
	{
		OnLifeEvent(Life);		
	}
	
	MSG_DWORD2 msg;
	msg.Category = MP_BOSSMONSTER;
	msg.Protocol = MP_BOSS_LIFE_NOTIFY;
	msg.dwData1 = GetLife();
	msg.dwData2 = GetID();
	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
}

void CBossMonster::DoDamage(CObject* pAttacker,RESULTINFO* pDamageInfo,DWORD beforeLife)
{
	//CMonster::DoDamage(pAttacker, pDamageInfo, beforeLife);
	
	if( pAttacker->GetGridID() != GetGridID() )
		return;
	
	SetObjectBattleState( eObjectBattleState_Battle );
	if ( pAttacker )
	{
		pAttacker->SetObjectBattleState( eObjectBattleState_Battle );
	}

	if( GetAbnormalStatus()->IsSlip )
	{
		EndBuffSkillByStatus( eStatusKind_Slip );
	}
	
	if( pDamageInfo->RealDamage > beforeLife )
		pDamageInfo->RealDamage = (WORD)beforeLife;

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		AddDamageObject( (CPlayer*)pAttacker, pDamageInfo->RealDamage, 0 );
		
		if( GSTATEMACHINE.IsTargetChange(pAttacker, this) && m_BossState.GetCurState() == eBossState_WalkAround )
		{
			SetTargetObject((CObject*)pAttacker);
			if(m_pTObject)
			{
				VECTOR3 TObjectPos = *CCharMove::GetPosition(m_pTObject);
				OnMove(&TObjectPos);
			}
		}
	}
	else if( pAttacker->GetObjectKind() == eObjectKind_Pet )
	{
		CPlayer* const ownerPlayer = (CPlayer*)g_pUserTable->FindUser(
			pAttacker->GetOwnerIndex());

		if(ownerPlayer &&
			eObjectKind_Player == ownerPlayer->GetObjectKind())
		{
			AddDamageObject(
				ownerPlayer,
				pDamageInfo->RealDamage,
				0);

			if( GSTATEMACHINE.IsTargetChange(ownerPlayer, this) && m_BossState.GetCurState() == eBossState_WalkAround )
			{
				SetTargetObject((CObject*)ownerPlayer);

				if(m_pTObject)
				{
					VECTOR3 TObjectPos = *CCharMove::GetPosition(m_pTObject);
					OnMove(&TObjectPos);
				}
			}
		}
	}
	
	AddAggro(
		pAttacker->GetID(),
		pDamageInfo->RealDamage,
		pDamageInfo->mSkillIndex);

	if(m_BossState.GetCurState() != eBossState_Attack)
	{		
		if(m_BossState.GetStateEndTime() > gCurTime)
			return;
		SetBossState(eBossState_Attack);
	}	
}

void CBossMonster::Process()
{
	CMonster::Process();	//  자동소멸을 확인하기 위해 먼저호출한다.

	if( GetState() == eObjectState_Die ||
		IsShocked() )
		return;

	if(m_BossState.IsStateUpdated())
	{		
		m_BossState.UpdateState();
	}
	
	ySWITCH(m_BossState.GetCurState())
	yCASE(eBossState_Attack)
	Attack();
	yCASE(eBossState_Pursuit)
	Pursuit();
	yCASE(eBossState_WalkAround)
	WalkAround();
	yCASE(eBossState_Stand)
	Stand();
	yCASE(eBossState_Recover)
	Recover();
	yCASE(eBossState_Summon)
	Summon();

	yENDSWITCH

	CCharacterCalcManager::ProcessLifeBoss(
		this);
}

void CBossMonster::Attack()
{	
	m_bDelete = FALSE;

	////////////////////////////////////////////////////////////////////////////////
	/// 06. 08. 2차 보스 - 이영준
	/// 보스의 타겟팅 방식이 매회 랜덤 결정일 경우 타겟을 새로 잡는다
	if( m_pBossMonsterInfo->GetTargetingType() == 1 )
	{
		CObject* pTarget = 	g_pServerSystem->GetGridSystem()->FindPlayerRandom(this);

		if(pTarget)
			SetTargetObject(pTarget);
	}
	////////////////////////////////////////////////////////////////////////////////

	if(( !GetTObject()) || (GetTObject() && ( GetTObject()->GetState() == eObjectState_Die ||  GetTObject()->GetInited() == FALSE ) ) )
	{
		if(m_BossState.GetStateEndTime() < gCurTime)
		{
			SetBossState(eBossState_WalkAround);
		}
		return;
	}
	if(m_BossState.IsStateFirst())
	{
		m_BossState.SetStateFirst(FALSE);
		m_AttackStartTime = gCurTime;
		const cActiveSkillInfo* const pSkillInfo = SKILLMGR->GetActiveInfo(m_CurAttackIdx);
		DWORD delay = pSkillInfo->GetInfo().AnimationTime + 400;

		// 080108 KTH -- 스킬 거리 체크
		VECTOR3 TObjectPos	= *CCharMove::GetPosition(GetTObject());
		if( !pSkillInfo->IsInRange(*this, TObjectPos, GetTObject()->GetRadius() ) )
		{
			SetBossState(eBossState_Pursuit);
			return;
		}	

		//DWORD delay = 30;//SKILLMGR->GetSkillInfo(m_CurAttackIdx);
		m_BossState.SetStateEndTime(gCurTime+delay);
		DoAttack();
		ChangeCurAttackIdx();

		MSG_DWORD msg;
		msg.Category = MP_BOSSMONSTER;
		msg.Protocol = MP_BOSS_STAND_END_NOTIFY;
		msg.dwData = GetID();
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
				
		return;
	}
	else if(m_pBossMonsterInfo->IsAttackEmpty() == FALSE)
	{
		const cActiveSkillInfo* const pSkillInfo = SKILLMGR->GetActiveInfo(m_CurAttackIdx);

		if(m_BossState.GetStateEndTime()  < gCurTime)
		{		
			VECTOR3 TObjectPos	= *CCharMove::GetPosition(GetTObject());
			//DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );
			
			if( !pSkillInfo->IsInRange(*this, TObjectPos, GetTObject()->GetRadius()))
			{
				SetBossState(eBossState_Pursuit);
			}
			else
			{				
				m_AttackStartTime = gCurTime;
				DWORD delay = pSkillInfo->GetInfo().AnimationTime + 400;
				m_BossState.SetStateEndTime(gCurTime+delay);
				DoAttack();
				ChangeCurAttackIdx();				
			}
		}
		else
		{
			// attacking
		}
	}
	else
	{
		SetBossState(eBossState_Stand);		
	}
}

void CBossMonster::Pursuit()
{
	m_bDelete = FALSE;
	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();
	const cActiveSkillInfo* const pSkillInfo = SKILLMGR->GetActiveInfo(m_CurAttackIdx);

	if(pSkillInfo == 0)
	{
		ASSERT(0);
		SetBossState(eBossState_Stand);
		return;
	}

	if( !GetTObject() || (GetTObject() && ( GetTObject()->GetState() == eObjectState_Die ||  GetTObject()->GetInited() == FALSE ) ) )
	{
		SetBossState(eBossState_Stand);
		return;
	}

	if(m_BossState.IsStateFirst()) // 처음 시작한거면 
	{		
		mStateParamter.PursuitForgiveStartTime = gCurTime;
		m_BossState.SetStateStartTime(0);
		m_BossState.SetStateEndTime(1000);
		DoPursuit();
		m_BossState.SetStateFirst(FALSE);

		SYSTEMTIME time;
		char szFile[256] = {0,};
		GetLocalTime( &time );

		sprintf(szFile, "./Log/BossMonsterLog_%02d_%04d%02d%02d_%d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay, this->GetMonsterKind() );
		FILE* fp;
		fp = fopen(szFile, "a+");
		if (fp)
		{
			fprintf(fp, "RegenTime : [%2d:%2d]\n", time.wHour, time.wMinute);
			fclose(fp);
		}
	}
	else
	{
		VECTOR3 ObjectPos	= *CCharMove::GetPosition(this);
		VECTOR3 TObjectPos	= *CCharMove::GetPosition(GetTObject());
		DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );
		if( mStateParamter.prePursuitForgiveTime == 0 && ( mStateParamter.PursuitForgiveStartTime + baseMonsterList.PursuitForgiveTime < gCurTime || baseMonsterList.PursuitForgiveDistance < Distance ) )
		{
			mStateParamter.SearchLastTime = gCurTime;
			SetBossState(eBossState_WalkAround);			
		}	
		else
		{
			if( mStateParamter.prePursuitForgiveTime != 0 && mStateParamter.PursuitForgiveStartTime + mStateParamter.prePursuitForgiveTime < gCurTime )
			{
				mStateParamter.prePursuitForgiveTime = 0;
				mStateParamter.SearchLastTime = gCurTime;
				SetBossState(eBossState_WalkAround);				
			}
			else if (pSkillInfo->IsInRange(*this, TObjectPos, GetTObject()->GetRadius()))
			{
				SetBossState(eBossState_Attack);
			}
			else
			{
				m_BossState.SetStateStartTime(gCurTime);
				DoPursuit();
			}
		}
	}
}

void CBossMonster::SetCurAttackIdx(DWORD AttackIdx)
{
	m_CurAttackIdx = AttackIdx;
}
	
DWORD CBossMonster::GetCurAttackIdx()
{
	return m_CurAttackIdx;
}

void CBossMonster::ChangeCurAttackIdx()
{
	m_CurAttackIdx = m_pBossMonsterInfo->GetNextAttackKind(m_CurAttackIdx);
}

BOOL CBossMonster::SetBossState(int state)
{
	if(GetState() == eObjectState_Die)
		return FALSE;
	if(m_BossState.GetNextState())
	{
		
		return FALSE;
	}

	m_BossState.SetState(state);

	if(state == eBossState_WalkAround || state == eBossState_Pursuit || state == eBossState_RunAway)
	{	
		// 080204 KTH -- 맵이 막혀있어도 무시한다.
		if( !IsNoCheckCollision() )
			SetNoCheckCollision( TRUE );	//Ignore TTB

		MSG_WORD msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_MONSTERMOVE_NOTIFY;
		msg.dwObjectID = GetID();
		msg.wData = (WORD)state;
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
	}
	return TRUE;
}

void CBossMonster::WalkAround()
{
	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();
	
	if( m_pBossMonsterInfo->GetDeleteTime() )
	{
		if( m_bDelete )
		{
			if( m_DeleteStartTime + m_pBossMonsterInfo->GetDeleteTime() <= gCurTime )
			{
				Delete();
			}
		}
		else
		{
			m_bDelete = TRUE;
			m_DeleteStartTime = gCurTime;
		}
	}

	if(m_BossState.IsStateFirst())
	{
		SetTObject(NULL);
		DoWalkAround();
		DWORD EndTime = m_BossState.GetStateStartTime() + CCharMove::GetMoveEstimateTime(this);
		m_BossState.SetStateEndTime(EndTime);
		m_BossState.SetStateFirst(FALSE);
	}
	else
	{				
		if( m_BossState.GetStateEndTime() < gCurTime )
		{
			int rate = rand()%100;
			if(0 <= rate && rate < baseMonsterList.StandRate )
			{
				SetBossState(eBossState_Stand);
			}
			else
			{
				//SetBossState(eBossState_WalkAround);
				DoWalkAround();
			}
		}
	}
}

void CBossMonster::Stand()
{
	/// 06. 09. 2차 보스 - 이영준
	/// 소멸시간
	if( m_pBossMonsterInfo->GetDeleteTime() )
	{
		if( m_bDelete )
		{
			if( m_DeleteStartTime + m_pBossMonsterInfo->GetDeleteTime() <= gCurTime )
			{
				Delete();
			}
		}
		else
		{
			m_bDelete = TRUE;
			m_DeleteStartTime = gCurTime;
		}
	}

	if(m_BossState.IsStateFirst())
	{
		m_BossState.SetStateFirst(FALSE);
		m_LifeRecoverTime.lastCheckTime = gCurTime;

		// 080205 KTH -- 맵에 충돌체크가 해지되어 있으면 다시 초기화 시켜준다.
		if( IsNoCheckCollision() )
		{
			VECTOR3 vPos;
			GetPosition( &vPos );
			if( !g_pServerSystem->GetMap()->CollisionTilePos( vPos.x, vPos.z, g_pServerSystem->GetMapNum() ) )
				SetNoCheckCollision( FALSE );
		}

		if( m_bOpenning )
		{
			switch( GetMonsterKind() )
			{
			case eBOSSKIND_TARINTUS:
				{
					// 080109 KTH -- 오프닝 8초
					m_BossState.SetStateEndTime(gCurTime+8000);
				}
				break;
			case eBOSSKIND_DRAGONIAN:
				{
					// 080109 KTH -- 오프닝 8초
					m_BossState.SetStateEndTime(gCurTime+7000);
				}
				break;
			default:
				m_BossState.SetStateEndTime(gCurTime+3500);
				break;
			}
			m_bOpenning = FALSE;
		}
		//m_BossState.SetStateEndTime(gCurTime+3500);
				
		SetTObject(NULL);
		CMonster::DoStand();
		return;
	}
	if(DoWalkAround() == TRUE)
		return;
	BOSSUNGIJOSIK* pUngi = m_pBossMonsterInfo->GetUngiInfo();
	if(m_BossState.GetStateStartTime() + pUngi->dwStartTime < gCurTime)
	{
		DWORD dwUngiTime = pUngi->dwDelayTime;
	 
	// ungi plus time for character
	// dwUngiTime = dwUngiTime*(1/gUngiSpeed);
		
		DWORD maxlife = GetMaxLife();
		DWORD curlife = GetLife();
		
		if(gCurTime - m_LifeRecoverTime.lastCheckTime > dwUngiTime)
		{
			if(curlife < maxlife)
			{
				DWORD pluslife = (DWORD)(maxlife*pUngi->fLifeRate);
				SetLife(curlife + pluslife, TRUE);
				m_LifeRecoverTime.lastCheckTime = gCurTime;
			}
		}
	}	
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CBossMonster::SetAddMsg(DWORD dwReceiverID,BOOL isLogin, MSGBASE*& sendMessage)
{
	static SEND_MONSTER_TOTALINFO message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_BOSSMONSTER_ADD;
	message.dwObjectID = dwReceiverID;
	GetSendMoveInfo( &message.MoveInfo, &message.AddableInfo );
	GetBaseObjectInfo( &message.BaseObjectInfo);
	message.TotalInfo = m_MonsterInfo;
	message.bLogin = BYTE(isLogin);

	sendMessage = &message;
	return message.GetMsgLength();
}

void CBossMonster::SetEventState(BYTE EventState, DWORD ActionValue)
{
	switch(EventState)
	{
	case eBOSSACTION_RECOVER:
		{
			SetBossNextState(eBossState_Recover);			
		}
		break;
	case eBOSSACTION_SUMMON:
		{
			SetBossNextState(eBossState_Summon);
		}
		break;
	}
	
//	MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("Set EventState : %d", EventState));

	SetEventActionValue(ActionValue);
}

void CBossMonster::Recover()
{
	if(m_BossState.IsStateFirst())
	{		
		m_BossState.SetStateStartTime(gCurTime);
		m_BossState.SetStateEndTime(gCurTime+3500);
		m_BossState.SetStateFirst(FALSE);
		CCharacterCalcManager::StartUpdateLife( this, GetMaxLife()*m_EventActionValue/100, 5, 2500 );
		
		MSG_DWORD msg;
		msg.Category = MP_BOSSMONSTER;
		msg.Protocol = MP_BOSS_REST_START_NOTIFY;
		msg.dwData = GetID();
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));	
		//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("Recover Start"));
	}
	if(m_BossState.GetStateEndTime() < gCurTime)
	{
		m_IsEventStating = FALSE;
		SetBossState(eBossState_Attack);
		
		//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("EndRecover State"));
	}
}

void CBossMonster::Summon()
{
 	if(m_BossState.IsStateFirst())
	{		
		m_BossState.SetStateStartTime(gCurTime);
		m_BossState.SetStateEndTime(gCurTime+3500);
		m_BossState.SetStateFirst(FALSE);

		int GroupID = GetEventActionValue();
		BOSSMONMGR->RegenGroup(this, GetSummonFileNum(), GroupID);

		MSG_DWORD msg;
		msg.Category = MP_BOSSMONSTER;
		msg.Protocol = MP_BOSS_RECALL_NOTIFY;
		msg.dwData = GetID();
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));

		//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("Summon start"));
	}
	if(m_BossState.GetStateEndTime() < gCurTime)
	{
		m_IsEventStating = FALSE;
		SetBossState(eBossState_Attack);
		//MHERROR->OutputFile("Debug.txt", MHERROR->GetStringArg("EndSummon State"));
	}
}

int CBossMonster::GetEventActionValue()
{
	return m_EventActionValue;
}

void CBossMonster::SetEventActionValue(DWORD value)
{
	m_EventActionValue = value;
}

WORD CBossMonster::GetSummonFileNum()
{
	return m_pBossMonsterInfo->GetSummonFileNum();
}

float CBossMonster::DoGetMoveSpeed()
{
	WORD kind = GetMonsterKind();

	float Speed = 0.0;
	switch(m_BossState.GetCurState())
	{
	case eMA_SCRIPT_WALK:
	case eMA_WALKAROUND:
	case eMA_WALKAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->WalkMove;
		}
		break;
	case eMA_RUNAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->RunawayMove;
		}
		break;
	case eMA_SCRIPT_RUN:
	case eMA_PERSUIT:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(kind)->RunMove;
		}
		break;
	}

	float addrateval = GetRateBuffStatus()->MoveSpeed / 100.f;
	float addval = GetBuffStatus()->MoveSpeed;

	m_MoveInfo.AddedMoveSpeed = Speed * addrateval + addval;

	Speed += m_MoveInfo.AddedMoveSpeed;

	if( Speed < 0 ) Speed = 0;

	if( GetAbnormalStatus()->IsMoveStop )
	{
		Speed = 0;
	}

	return Speed;
}

void CBossMonster::OnLifeEvent(DWORD Life)
{
	if(m_IsEventStating == TRUE)
		return;
	if(m_BossState.GetCurState() >= eBossState_Recover)
		return;
	for(int i = 0; i< m_pBossMonsterInfo->GetMaxEventStateNum(); ++i)
	{
		if(m_BossEventState[i].Condition == eBOSSCONDITION_LIFE)
		{
			if(m_BossEventState[i].Count)
			{
				// 091028 ShinJS --- DWORD Overflow 현상 수정
				float rate = ((float)Life / (float)GetMaxLife()) * 100.0f;
				if(rate < (float)m_BossEventState[i].ConditionValue)
				{
					SetEventState(m_BossEventState[i].Action, m_BossEventState[i].ActionValue);
					m_BossEventState[i].Count--;					
					break;
				}
			}
		}
	}	
}

void CBossMonster::SetBossNextState(int state)
{
	m_BossState.SetNextState(state);
	m_IsEventStating = TRUE;
}

void CBossMonster::DoAttack()
{
	OnStop();

	VECTOR3  MonPos /*,TargetPos*/;
	GetPosition(&MonPos);

	SKILLMGR->MonsterAttack(m_CurAttackIdx, this, m_pTObject);
}

BOOL CBossMonster::DoWalkAround()
{
	SetBossState(eBossState_WalkAround);
	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();

	// 091015 pdy 몬스터 선공 채크 변경
	if( m_bForeAttFlag && mStateParamter.SearchLastTime + baseMonsterList.SearchPeriodicTime < gCurTime )
	{
		mStateParamter.SearchLastTime = gCurTime;
		SetTargetObject(
			DoSearch());
		return TRUE;
	}
	// 250 :  충돌시 Search하는 Delay
	else if(m_CurAttackIdx && mStateParamter.CollSearchLastTime + 250 < gCurTime )
	{
		mStateParamter.CollSearchLastTime = gCurTime;		
		SetTargetObject(
			OnCollisionObject());
		return TRUE;
	}
	return FALSE;
}

void CBossMonster::SetTargetObject(CObject* TObject)
{
	if(TObject && TObject->GetState() != eObjectState_Die )
	{
		CPlayer * pPlayer = (CPlayer *)TObject;

		if(eObjectKind_Player != pPlayer->GetObjectKind())
		{
			return;
		}
		else if(FALSE == pPlayer->IsVisible())
		{
			return;
		}
		if(GetGridID() != TObject->GetGridID())
		{
			return;
		}
		
		m_BossState.SetState(eBossState_Pursuit);
		//SetTObject((CPlayer *)TObject);
		SetTObject(pPlayer);
	}
}

void CBossMonster::Delete()
{
	SYSTEMTIME time;
	char szFile[256] = {0,};
	GetLocalTime( &time );

	sprintf(szFile, "./Log/BossMonsterLog_%02d_%04d%02d%02d_%d.txt", g_pServerSystem->GetMapNum(), time.wYear, time.wMonth, time.wDay, this->GetMonsterKind() );
	FILE* fp;
	fp = fopen(szFile, "a+");
	if (fp)
	{
		fprintf(fp, "DeleteTime : [%2d:%2d]\n", time.wHour, time.wMinute);

		fclose(fp);
	}
	// 리젠 정보 등록 080204 KTH -- 잡지 못하였을 경우 리젠 시간을 1시간으로 마춘다.
	BOSSMONMGR->SetBossRandRegenChannel( GetMonsterKind(), GetGridID(), GetMonsterGroupNum(), 60 * 6000 );

	BOSSMONMGR->DeleteBossMonster( this );
	OBJECTSTATEMGR_OBJ->StartObjectState( this, eObjectState_Die, 0 );
	OBJECTSTATEMGR_OBJ->EndObjectState( this, eObjectState_Die, 0 );
	SetTObject(0);
	RemoveAllAggro();
}