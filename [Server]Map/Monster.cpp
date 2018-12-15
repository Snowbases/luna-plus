#include "StdAfx.h"
#include "Monster.h"
#include "CharMove.h"
#include "PackedData.h"
#include "GridSystem.h"
#include "Player.h"
#include "UserTable.h"
#include "ObjectStateManager.h"
#include "AISystem.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[cc]skill/server/delay/delay.h"
#include "../[cc]skill/server/info/activeskillinfo.h"
#include "TileManager.h"
#include "PathManager.h"
#include "../[CC]Skill/Server/Info/BuffSkillInfo.h"
#include "../[CC]Skill/Server/Object/ActiveSkillObject.h"
#include "..\[CC]Header\CommonGameFunc.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "RegenManager.h"
#include "Battle.h"
#include "cMonsterSpeechManager.h"
#include "GuildManager.h"
#include "StateMachinen.h"
#include "../hseos/Date/SHDateManager.h"
#include "Finite State Machine\Machine.h"
#include "Finite State Machine\Memory.h"
#include "Pet.h"
#include "AIGroupManager.h"
#include "AIGroupPrototype.h"
#include "./Siegerecallmgr.h"
#include "Trigger\Manager.h"
#include "..\4DyuchiGXGFunc\global.h"
#include "Distributer.h"

CMonster::CMonster() :
mFiniteStateMachine(new FiniteStateMachine::CMachine),
m_Distributer(new CDistributer)
{
	mpBattleStateDelay = new cDelay;
	mpBattleStateDelay->Init( 10000 );
}

CMonster::~CMonster()
{
	delete mpBattleStateDelay;
}

BOOL CMonster::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	CObject::Init(kind, AgentNum, pBaseObjectInfo);
	
	//m_MonsterState = new CStateNPC;
	m_pTObject = NULL;
	m_DropItemId = 0;
	m_dwDropItemRatio = 100;
	
	m_bEventMob = FALSE;
	m_bNoCheckCollision = FALSE;
	m_DieTime = 0;
	GetFiniteStateMachine().Clear();
	return TRUE;
}
void CMonster::Release()
{
	SetTObject(NULL);
	RemoveAllAggro();
	g_pAISystem.ReleaseMonsterID(GetID());
	m_BaseObjectInfo.dwObjectID = 0;
	SetSubID(0);
	m_bEventMob = FALSE;
	m_Distributer.get()->Release();
	
	CObject::Release();
}

void CMonster::InitMonster(MONSTER_TOTALINFO* pTotalInfo)
{
	mKillerObjectIndex = 0;
	m_MonsterInfo = *pTotalInfo;

	GAMERESRCMNGR->GetMonsterStats(GetMonsterKind(), &m_mon_stats);
	ZeroMemory(
		&mStateParamter,
		sizeof(mStateParamter));
	mStateParamter.SearchLastTime = gCurTime + 5000;
	mStateParamter.CollSearchLastTime = gCurTime + 5000;

	GSTATEMACHINE.SetState( this, eMA_STAND );
	m_MoveInfo.KyungGongIdx = eMA_STAND;
	memset( &mBuffStatus, 0, sizeof( Status ) );
	memset( &mRateBuffStatus, 0, sizeof( Status ) );
	memset( &mAbnormalStatus, 0, sizeof( AbnormalStatus ) );

	if( GetSMonsterList().SpecialType == 2 )
	{
		mAbnormalStatus.IsGod = TRUE;
	}

	// 091015 pdy 몬스터 선공 채크 변경
	SetForeAttack( GetSMonsterList().bForeAttack ) ;
	// 100104 LUJ, 리젠 지점이 있으면 그곳을 중심으로 배회하도록 값을 설정한다
	mDomainPosition = GROUPMGR->GetDomain(
		m_MonsterInfo.Group,
		GetBattleID(),
		m_SubID);

	m_bIsFixedPosition = FALSE;
}

// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
DWORD CMonster::SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*& sendMessage)
{
	static SEND_MONSTER_TOTALINFO message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_MONSTER_ADD;
	message.dwObjectID = dwReceiverID;
	GetSendMoveInfo( &message.MoveInfo, &message.AddableInfo );
	GetBaseObjectInfo( &message.BaseObjectInfo );
	message.TotalInfo = m_MonsterInfo;
	message.bLogin = BYTE(bLogin);

	sendMessage = &message;
	return message.GetMsgLength();
}

void CMonster::MoveStop()
{
	if(CCharMove::IsMoving(this) == FALSE)
		return;
	
	VECTOR3* pos = CCharMove::GetPosition(this);
	CCharMove::EndMove(this,gCurTime,pos);
	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_STOP;
	msg.dwMoverID = GetID();
	msg.cpos.Compress(pos);

	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
}

// 데미지 입힌 객체 관리 //////////////////////////////
void CMonster::AddDamageObject(CPlayer* pPlayer, DWORD damage, DWORD plusdamage)
{
	if( damage )
		m_Distributer.get()->AddDamageObject(pPlayer, damage, plusdamage);
}

void CMonster::AddChat(LPCTSTR str)
{
	TESTMSG pmsg;
	pmsg.Category = MP_CHAT;
	pmsg.Protocol = MP_CHAT_FROMMONSTER_ALL;
	pmsg.dwObjectID = GetID();
	SafeStrCpy( pmsg.Msg, str, MAX_CHAT_LENGTH+1 );
	PACKEDDATA_OBJ->QuickSend(this,&pmsg,pmsg.GetMsgLength());	//CHATMSG 040324
}

void CMonster::AddSpeech( DWORD SpeechType, DWORD SpeechIdx )
{
	ASSERT(SpeechIdx>=0);

	MSG_DWORD4 msg;
	msg.Category = MP_CHAT;
	msg.Protocol = MP_CHAT_MONSTERSPEECH;
	msg.dwObjectID = GetID();

	if( SpeechType == eMSpch_Balloon || SpeechType == eMSpch_BallonChat )	//말풍선 or 말풍선+채팅창
	{
		msg.dwData1 = SpeechType;
		msg.dwData2 = SpeechIdx;
		msg.dwData3 = 0;	//사용안함
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(MSG_DWORD4));

		return;
	}
	else if( SpeechType == eMSpch_SmallShout )	//외치기 SMALLSHOUT
	{
		msg.dwData1 = SpeechType;
		msg.dwData2 = SpeechIdx;
		msg.dwData3 = GetID();
		msg.dwData4 = GetMonsterKind();

		g_pUserTable->SetPositionUserHead();
		CObject* pObject = NULL ;
		while( (pObject = g_pUserTable->GetUserData() )!= NULL)
		{
			if( pObject->GetObjectKind() != eObjectKind_Player ) continue;
			
			CPlayer* pReceiver = (CPlayer*)pObject;
			if( GetGridID() == pReceiver->GetChannelID() )
			{
				pReceiver->SendMsg( &msg, sizeof(MSG_DWORD4) );
			}
		}
		return;
	}
	else
	{
		ASSERT(0);
		return;
	}
}

void CMonster::OnStartObjectState(EObjectState State,DWORD dwParam)
{
	switch(State)
	{
	case eObjectState_Die:
		mKillerObjectIndex = dwParam;
		break;
	case eObjectState_TiedUp_CanSkill:	// 이동 결박
	case eObjectState_TiedUp:	// 이동,공격 모두 결박
		OnStop();
		break;
	}
}

void CMonster::OnEndObjectState(EObjectState State)
{
	switch(State)
	{
	case eObjectState_Die:
		g_pServerSystem->RemoveMonster(GetID());		
		break;
	}
}

float CMonster::GetRadius() const
{	
	return (float)GetSMonsterList().MonsterRadius;
}

void CMonster::StateProcess()
{
	if( m_BaseObjectInfo.ObjectBattleState )
	{
		ThinkAggro();

		if( !mpBattleStateDelay->Check() )
		{
			SetObjectBattleState( eObjectBattleState_Peace );
		}
	}

	switch(GetState())
	{
	case eObjectState_Die:
		{
			DWORD RemainTime;
			if(m_ObjectState.State_End_Time >= gCurTime)
				RemainTime = m_ObjectState.State_End_Time - gCurTime;
			else
				RemainTime = 0;

			if(RemainTime < MONSTERREMOVE_TIME - 1500 )
			{
				if(CObject* pKiller = g_pUserTable->FindUser(mKillerObjectIndex))
				{
					GetBattle()->OnMonsterDistribute(this,pKiller);
				}

				mKillerObjectIndex = 0;
			}
		}
		break;
	}

	switch(mStateParamter.stateCur)
	{
	case eMA_PAUSE:
	case eMA_PERSUIT:
		{
			break;
		}
	default:
		{
			GetFiniteStateMachine().Run();
			break;
		}
	}
}

int CMonster::GetObjectTileSize()
{
	float radius = GetRadius();
	if(radius < 50)
		return 0;
	else if(radius < 150)
		return 1;
	else if(radius < 250)
		return 2;
	else
		return 2;
}

BOOL CMonster::SetTObject(CObject * pNewTPlayer)
{
	if(m_pTObject == pNewTPlayer)
	{
		return FALSE;
	}
	else if(this == pNewTPlayer)
	{
		return FALSE;
	}

	CObject* const m_pOldTPlayer = m_pTObject;
	
	if( m_pOldTPlayer )
	{
		switch(m_pOldTPlayer->GetObjectKind())
		{
		case eObjectKind_Player:
			{
				CPlayer* const playerObject = (CPlayer*)m_pOldTPlayer;

				playerObject->RemoveFollowList(
					GetID());
				break;
			}
		case eObjectKind_Pet:
			{
				CPet* const petObject = (CPet*)m_pOldTPlayer;

				petObject->RemoveFollowList(
					GetID());
				break;
			}
		}
	}

	if(0 == pNewTPlayer)
	{
		m_pTObject = 0;
		return TRUE;
	}
	else if(IsInvalidTarget(*pNewTPlayer))
	{
		return FALSE;
	}

	switch(pNewTPlayer->GetObjectKind())
	{
	case eObjectKind_Player:
		{
			m_pTObject = pNewTPlayer;
			CPlayer* player = ( CPlayer* )pNewTPlayer;

			// 검색된 몬스터가 유저 추적
			if( !player->GetInited() || player->GetState() == eObjectState_Die )
			{
				break;
			}
			else if(FALSE == player->AddFollowList(this))
			{
				break;
			}

			const DWORD maxGravity = 100;

			if(maxGravity < player->GetGravity())
			{
				if( player->RemoveFollowAsFarAs(player->GetGravity(), this))
				{
					break;
				}
			}

			const MonSpeechInfo* speechInfo = 0;

			if(m_bForeAttFlag)	// 091015 pdy 몬스터 선공 채크 변경
			{
				speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex( GetMonsterKind(), eMon_Speech_ForeAtk );
			}
			else if( mStateParamter.stateNew == eMA_ATTACK )//!비선공이면서 헬퍼몹이면.. 헬퍼 구분을 어떻게 해야할것인가..
			{
				speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex( GetMonsterKind(), eMon_Speech_AboutHelp );
			}

			if(speechInfo)
			{
				AddSpeech(
					speechInfo->SpeechType,
					speechInfo->SpeechIndex );
			}

			// 용병이 도울 수 있도록 타겟에 잡히는 순간 어그로 수치가 적용될 수 있도록한다
			player->AddToAggroed(
				GetID());
			return TRUE;
		}
	case eObjectKind_Pet:
		{
			m_pTObject = pNewTPlayer;
			CPet* petObject = ( CPet* )pNewTPlayer;

			// 검색된 몬스터가 유저 추적
			if( !petObject->GetInited() || petObject->GetState() == eObjectState_Die )
			{
				break;
			}
			else if(FALSE == petObject->AddFollowList(this))
			{
				break;
			}

			DWORD dwGravity = petObject->GetGravity();
			if(dwGravity > 100)
			{
				if( petObject->RemoveFollowAsFarAs(dwGravity, this) ) //떨어져 나간 몬스터가 자신(this)일 수도 있다!
					break;
			}

			const MonSpeechInfo* speechInfo = 0;

			if(m_bForeAttFlag)	// 091015 pdy 몬스터 선공 채크 변경
			{
				speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex( GetMonsterKind(), eMon_Speech_ForeAtk );
			}
			else if( mStateParamter.stateNew == eMA_ATTACK )//!비선공이면서 헬퍼몹이면.. 헬퍼 구분을 어떻게 해야할것인가..
			{
				speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex( GetMonsterKind(), eMon_Speech_AboutHelp );
			}

			if(speechInfo)
			{
				AddSpeech(
					speechInfo->SpeechType,
					speechInfo->SpeechIndex );
			}

			return TRUE;
		}
	case eObjectKind_Monster:
	case eObjectKind_BossMonster:
	case eObjectKind_SpecialMonster:
	case eObjectKind_FieldBossMonster:
	case eObjectKind_FieldSubMonster:
	case eObjectKind_ToghterPlayMonster:
	case eObjectKind_ChallengeZoneMonster:
		{
			m_pTObject = pNewTPlayer;
			return TRUE;
		}
	}

	return FALSE;
}

// 091026 LUJ, 코드 정리
void CMonster::DoDamage(CObject* pAttacker,RESULTINFO* pDamageInfo,DWORD beforeLife)
{
	if( pAttacker->GetGridID() != GetGridID() )
	{
		return;
	}
	else if(eMA_RUNAWAY == mStateParamter.stateCur ||
		eMA_WALKAWAY == mStateParamter.stateCur)
	{
		return;
	}

	SetObjectBattleState(eObjectBattleState_Battle);
	pAttacker->SetObjectBattleState(eObjectBattleState_Battle);

	if( GetAbnormalStatus()->IsSlip )
	{
		EndBuffSkillByStatus( eStatusKind_Slip );
	}

	if( pAttacker->GetObjectKind() == eObjectKind_Player )
	{
		AddDamageObject(
			(CPlayer*)pAttacker,
			pDamageInfo->RealDamage,
			0);
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
		}
	}

	AddAggro(
		pAttacker->GetID(),
		pDamageInfo->RealDamage,
		pDamageInfo->mSkillIndex);

	if(eMA_RUNAWAY == mStateParamter.stateCur ||
		eMA_WALKAWAY == mStateParamter.stateCur)
	{
		return;
	}
	
	RequestHelp(*pDamageInfo);
	
	GSTATEMACHINE.SetState(this, eMA_ATTACK);
	cActiveSkillObject* skillObject = (cActiveSkillObject*)SKILLMGR->GetSkillObject(GetCurrentSkill());

	if(skillObject &&
		SKILL_STATE_CASTING == skillObject->GetSkillState())
	{
		const WORD rate = WORD(rand() % 100);

		if(rate < skillObject->GetInfo().Cancel)
		{
			MSG_DWORD message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_SKILL;
			message.Protocol = MP_SKILL_CANCEL_NOTIFY;
			message.dwObjectID = GetID();
			message.dwData = GetCurrentSkill();
			PACKEDDATA_OBJ->QuickSend(
				this,
				&message,
				sizeof(message));
			skillObject->SetEndState();
			skillObject->EndState();
		}
	}

}
void CMonster::SetLife(DWORD Life,BOOL bSendMsg)
{
//	ASSERT(Life <= GetSMonsterList().Life);
	DWORD maxlife = GetSMonsterList().Life;
	if(Life > maxlife)
		Life = maxlife;
	m_MonsterInfo.Life = Life;
}

void CMonster::DoDie(CObject* pAttacker)
{
	EObjectKind	attackerKind	= eObjectKind_None;
	DWORD		AttackerID		= 0;

	// 080616 LUJ, 포인터가 유효한 경우에 값을 설정하도록 함
	if( pAttacker )
	{
		AttackerID		= pAttacker->GetID();
		attackerKind	= EObjectKind( pAttacker->GetObjectKind() );
	}

	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,AttackerID);
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die,MONSTERREMOVE_TIME);

	if( eObjectKind_Player == attackerKind )
	{
		GUILDMGR->AddHuntedMonster( (CPlayer*)pAttacker, this );
		((CPlayer*)pAttacker)->ProcMonstermeterKillMon();
		g_csDateManager.SRV_ProcMonsterDie((CPlayer*)pAttacker);
	}

	//090213 pdy 데이트던젼 미션 성공/몬스터리젠 관련버그 수정 (펫이 킬했을때 카운팅이 안되서 미션성공/몬스터리젠이 안되는 문제)
	if( eObjectKind_Pet == attackerKind)
	{
		if(g_csDateManager.IsChallengeZone(g_pServerSystem->GetMapNum()) )
		{
			CPlayer* const ownerPlayer = (CPlayer*)g_pUserTable->FindUser(
				pAttacker->GetOwnerIndex());

			if(ownerPlayer &&
				eObjectKind_Player == ownerPlayer->GetObjectKind())
			{
				g_csDateManager.SRV_ProcMonsterDie(
					ownerPlayer);
			}
		} 

	}
	
	//SW050902
	MonSpeechInfo* pTemp = MON_SPEECHMGR->GetCurStateSpeechIndex( this->GetMonsterKind(), eMon_Speech_Death );
	if( pTemp )
		this->AddSpeech( pTemp->SpeechType, pTemp->SpeechIndex );

	RemoveAllAggro();
	SIEGERECALLMGR->Check_ObjectDie( pAttacker, GetID(), FALSE ) ;
}

void CMonster::GetSendMoveInfo(SEND_MOVEINFO* pRtInfo,CAddableInfoList* pAddInfoList)
{
	CObject::GetSendMoveInfo(pRtInfo,pAddInfoList);
	pRtInfo->KyungGongIdx = (WORD)mStateParamter.stateCur;	
}

float CMonster::DoGetMoveSpeed()
{
	if(GetAbnormalStatus()->IsMoveStop)
	{
		return 0;
	}

	float Speed = 0;

	switch(mStateParamter.stateCur)
	{
	case eMA_SCRIPT_WALK:
	case eMA_WALKAROUND:
	case eMA_WALKAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->WalkMove;
		}
		break;
	case eMA_RUNAWAY:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->RunawayMove;
		}
		break;
	case eMA_SCRIPT_RUN:
	case eMA_PERSUIT:
		{
			Speed = (float)GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind())->RunMove;
		}
		break;
	}

	m_MoveInfo.AddedMoveSpeed = Speed * GetRateBuffStatus()->MoveSpeed / 100.f + GetBuffStatus()->MoveSpeed;

	return max(0, Speed + m_MoveInfo.AddedMoveSpeed);
}

void CMonster::OnStop()
{
	if(CCharMove::IsMoving(this) == FALSE) return;
	
	VECTOR3* pos = CCharMove::GetPosition(this);
	CCharMove::EndMove(this, gCurTime, pos);
	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_STOP;
	msg.dwMoverID = GetID();
	msg.cpos.Compress(pos);

	PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
}

void CMonster::OnMove( VECTOR3 * pPos )
{

	ASSERT(GetState() != eObjectState_Die);

	// 결박 상태이기때문에 못 움직이게 함
	if( GetState() == eObjectState_TiedUp_CanSkill ||
		GetState() == eObjectState_TiedUp )
		return;

	if( DoGetMoveSpeed() <= 0 )
		return;

	VECTOR3* curpos = CCharMove::GetPosition(this);
	if(m_MoveInfo.bMoving)
	{
		if(m_MoveInfo.GetMaxTargetPosIdx() != 0 && m_MoveInfo.GetTargetPosition(m_MoveInfo.GetMaxTargetPosIdx()-1)->x == pPos->x && m_MoveInfo.GetTargetPosition(m_MoveInfo.GetMaxTargetPosIdx()-1)->z == pPos->z)
			return;
	}
	if(curpos->x == pPos->x && curpos->z == pPos->z)
		return;

	m_MoveInfo.InitTargetPosition();

	if( IsNoCheckCollision() == TRUE )			//Ignore TTB
	{
		m_MoveInfo.GetTargetPositionArray()[0] = *pPos;
		m_MoveInfo.GetMaxTargetPosIdxRef() = 1;
	}
	else
	{
		PATHMGR->GetPath(
			&m_MoveInfo.CurPosition,
			pPos,
			m_MoveInfo.GetTargetPositionArray(),
			MAX_CHARTARGETPOSBUF_SIZE,
			m_MoveInfo.GetMaxTargetPosIdxRef(),
			this,
			FALSE);
	}

	WORD count = m_MoveInfo.GetMaxTargetPosIdx();

	// jsd 
	if( count == 0 )
		return;
	else if( count == 1 )
	{
		MOVE_ONETARGETPOS_FROMSERVER msg;
		SetProtocol(&msg,MP_MOVE,MP_MOVE_ONETARGET);
		msg.AddedMoveSpeed = GetMoveInfo()->AddedMoveSpeed;
		msg.dwObjectID = GetID();
		msg.SetStartPos(curpos);
		msg.SetTargetPos(m_MoveInfo.GetTargetPosition(0));

		PACKEDDATA_OBJ->QuickSend(this,&msg,msg.GetSize());
	}
	else
	{
		MOVE_TARGETPOS msg;
		msg.Init();
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_TARGET;
		msg.dwMoverID = GetID();
		msg.SetStartPos(&m_MoveInfo.CurPosition);
		msg.AddedMoveSpeed = m_MoveInfo.AddedMoveSpeed;

		for(BYTE i=0;i<count;++i)
			msg.AddTargetPos(m_MoveInfo.GetTargetPosition(i));

		PACKEDDATA_OBJ->QuickSend(this,&msg,msg.GetSize());
	}	

	m_MoveInfo.SetCurTargetPosIdx(0);
	ASSERT(m_MoveInfo.GetCurTargetPosIdx()< m_MoveInfo.GetMaxTargetPosIdx());
	CCharMove::StartMoveEx(this,gCurTime,curpos,m_MoveInfo.GetTargetPosition(0));

}
void CMonster::DoStand()
{
	OnStop();
}
void CMonster::DoRest(BOOL bStart)
{
	OnStop();

}
BOOL CMonster::DoAttack( DWORD attackNum )
{
	if( !m_pTObject ) return FALSE;

	OnStop();

	if( GetState() == eObjectState_TiedUp_CanMove ||
		GetState() == eObjectState_TiedUp )
		return TRUE;

	if( GetGridID() != m_pTObject->GetGridID() )
	{
		return TRUE;
	}

	SKILLMGR->MonsterAttack(attackNum, this, m_pTObject);

	return TRUE;
}

BOOL CMonster::DoWalkAround()
{
	VECTOR3* pThisPos = CCharMove::GetPosition(this);	
	VECTOR3 emptyPosition = {0};
	VECTOR3 domainPoint = (emptyPosition == mDomainPosition ? *pThisPos : mDomainPosition);
	VECTOR3 decisionPoint = {0};

	// 100617 ONS 배회위치를 고정시켰을 경우, 지정한 도메인지점으로 이동시킨다.
	if( m_bIsFixedPosition )
	{
		OnMove(&domainPoint);
		return TRUE;
	}

	AdjacentPointForOriginPoint(
		pThisPos,
		&domainPoint,
		10000.0f,
		&decisionPoint);
	
	int ran = rand();
	const float dx = float(ran%GetSMonsterList().DomainRange) * (ran%2?1:-1);
	ran = rand();
	const float dz = float(ran%GetSMonsterList().DomainRange) * (ran%2?1:-1);

	VECTOR3 Target = {0};
	Target.x = decisionPoint.x + dx;
	Target.z = decisionPoint.z + dz;

	VECTOR3 CollisonPos = {0};
	
	if(g_pServerSystem->GetMap()->CollisionCheck(pThisPos,&Target,&CollisonPos,this) == TRUE)
	{
		Target.x = decisionPoint.x - dx;
		Target.z = decisionPoint.z - dz;
	}
	if(g_pServerSystem->GetMap()->CollisionCheck(pThisPos,&Target,&CollisonPos,this) == TRUE)
	{
		Target.x = decisionPoint.x + dx;
		Target.z = decisionPoint.z - dz;
	}
	if(g_pServerSystem->GetMap()->CollisionCheck(pThisPos,&Target,&CollisonPos,this) == TRUE)
	{
		Target.x = decisionPoint.x - dx;
		Target.z = decisionPoint.z + dz;
	}

	//YH2DO
	if(Target.x < 10)
		Target.x = 10;
	else if(Target.x > 51200)
		Target.x = 51200;
	if(Target.z < 10)
		Target.z = 10;
	else if(Target.z > 51200)
		Target.z = 51200;
	
	OnMove(&Target);

	return TRUE;
}

BOOL CMonster::DoPursuit()
{
	if( !m_pTObject )	return FALSE;

	// 대상과 겹쳐지지 않도록 한다
	{
		VECTOR3 targetPosition = *CCharMove::GetPosition(
			m_pTObject);
		VECTOR3 sourcePosition = {0};
		GetPosition(
			&sourcePosition);
		VECTOR3 directionVector = sourcePosition - targetPosition;

		if(targetPosition == sourcePosition)
		{
			directionVector.x = (rand() < RAND_MAX / 2 ? -1.0f : 1.0f);
			directionVector.z = (rand() < RAND_MAX / 2 ? -1.0f : 1.0f);
		}

		VECTOR3 normalVector = {0};
		Normalize(
			&normalVector,
			&directionVector);
		targetPosition = targetPosition + normalVector * m_pTObject->GetRadius();
		OnMove(&targetPosition);
	}

	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();

	if(BASE_MONSTER_LIST::HelpLifeIsUnder30Percent != baseMonsterList.mHelpType)
	{
		return TRUE;
	}
	else if(GetGroupAlias() &&
		mStateParamter.stateCur != eMA_RUNAWAY &&
		mStateParamter.stateCur != eMA_WALKAWAY &&
		mStateParamter.stateCur != eMA_ATTACK &&

		NULL == mStateParamter.pHelperMonster)
	{
		DWORD friendsList[32] = {0,};
		DWORD dwCount = DoFriendGroupSearch(&friendsList[0]);

		for(WORD i=0; i<dwCount; i++)
		{
			CObject* object = g_pUserTable->FindUser(friendsList[i]);
			if(! object)
				break;

			if(object->GetGridID() != GetGridID())
			{
				continue;
			}
			else if(object->GetObjectKind() != eObjectKind_Monster)
			{
				continue;
			}
			else if(object->GetState() == eObjectState_Die || object->GetState() == eObjectState_Immortal)
			{
				continue;
			}
			else if( ((CMonster*)object)->mStateParamter.stateCur == eMA_PERSUIT || 
					((CMonster*)object)->mStateParamter.stateCur == eMA_RUNAWAY ||
					((CMonster*)object)->mStateParamter.stateCur == eMA_WALKAWAY ||
					((CMonster*)object)->mStateParamter.stateCur == eMA_ATTACK )
			{
				continue;
			}
			else if(GetGroupAlias() != object->GetGroupAlias())
			{
				continue;
			}

			mStateParamter.pHelperMonster = this;

			GSTATEMACHINE.SetHelperMonster(
				this,
				(CMonster*)object,
				m_pTObject);

			if(! m_pTObject)
			{
				break;
			}
		}
	}

	return TRUE;
}

CMonster * CMonster::DoFriendSearch(DWORD Range)
{
	return (CMonster *)g_pServerSystem->GetGridSystem()->FindMonsterInRange(CCharMove::GetPosition(this), (float)Range, this, FIND_FIRST );
}

WORD CMonster::DoFriendGroupSearch(DWORD* pFriendsList)
{
	if(! pFriendsList)
		return 0;

	return g_pServerSystem->GetGridSystem()->FindMonsterGroupInRange(pFriendsList, CCharMove::GetPosition(this), this);
}

CObject * CMonster::DoSearch()
{
	return g_pServerSystem->GetGridSystem()->FindPlayerInRange(CCharMove::GetPosition(this), (float)GetSMonsterList().SearchRange, this,(BYTE)GetSMonsterList().TargetSelect );
}

CObject * CMonster::OnCollisionObject()
{
	return g_pServerSystem->GetGridSystem()->FindPlayerInRange(CCharMove::GetPosition(this), (float)GetSMonsterList().MonsterRadius, this, FIND_FIRST );
}

void CMonster::Drop()
{
	if( m_MonsterInfo.Group == (WORD)QUESTRECALL_GROUP_ID )	return;

	m_Distributer.get()->SetInfoToDistribute(
		mKillerObjectIndex,
		m_DropItemId,
		m_dwDropItemRatio,
		*this);
	m_Distributer.get()->Distribute();
}

void CMonster::DistributePerDamage()
{
	m_Distributer.get()->DistributePerDamage(
		*this);
}

// 필드보스 - 05.12 이영준
// 필드보스용 아이템 분배 함수 호출함
void CMonster::DistributeItemPerDamage()
{
	m_Distributer.get()->DistributeItemPerDamage(
		*this);
}

void CMonster::DistributeDamageInit()
{
	m_Distributer.get()->DamageInit();
}

void CMonster::DistributeDeleteDamagedPlayer(DWORD CharacterID)
{
	m_Distributer.get()->DeleteDamagedPlayer(
		CharacterID);
}


DWORD CMonster::DoGetPhyAttackPowerMin()
{
	double physicalAttackPower = m_mon_stats.PhysicalAttackMin1;
	int tmp = m_mon_stats.PhysicalAttackMax1-m_mon_stats.PhysicalAttackMin1;
	if(tmp) physicalAttackPower += rand()%tmp;

	return (DWORD)physicalAttackPower;
}


DWORD CMonster::DoGetPhyAttackPowerMax()
{
	double physicalAttackPower = m_mon_stats.PhysicalAttackMin1;
	int tmp = m_mon_stats.PhysicalAttackMax1-m_mon_stats.PhysicalAttackMin1;
	if(tmp) physicalAttackPower += rand()%tmp;

	return (DWORD)physicalAttackPower;
}

void CMonster::RemoveFromAggro( DWORD dwObjectID )
{
	mAggroContainer.erase(
		dwObjectID);

	if(CObject* const object = g_pUserTable->FindUser(dwObjectID))
	{
		switch(object->GetObjectKind())
		{
		case eObjectKind_Player:
			{
				((CPlayer*)object)->RemoveFromAggroed(
					GetID());
				break;
			}
		case eObjectKind_Pet:
			{
				((CPet*)object)->RemoveFromAggroed(
					GetID());
				break;
			}
		}
	}
}

void CMonster::RemoveAllAggro()
{
	for( AggroContainer::const_iterator aggroIter = mAggroContainer.begin() ; aggroIter != mAggroContainer.end() ; ++aggroIter )
	{
		const DWORD dwObjectIndex = aggroIter->first;
		CObject* const pObject = g_pUserTable->FindUser( dwObjectIndex );
		if( !pObject )
			continue;

		switch( pObject->GetObjectKind() )
		{
		case eObjectKind_Player:
			{
				((CPlayer*)pObject)->RemoveFromAggroed( GetID() );
				break;
			}
		case eObjectKind_Pet:
			{
				((CPet*)pObject)->RemoveFromAggroed( GetID() );
				break;
			}
		}
	}

	mAggroContainer.clear();
	mStateParamter.pHelperMonster = NULL;
	GetAbnormalStatus()->Attract = 0;
}

void CMonster::SetObjectBattleState(eObjectBattleState state)
{ 
	m_BaseObjectInfo.ObjectBattleState = state; 

	if( state )
	{
		mpBattleStateDelay->Start();
	}
}

void CMonster::SetDieTime(DWORD time)
{
	m_DieTime = time;
}

void CMonster::Process()
{
	if(m_DieTime)
	{
		if(m_DieTime < gCurTime)
		{			
			MSG_DWORD2 msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_MONSTER_DIE;
			msg.dwObjectID = GetID();
			msg.dwData1	= 0;
			msg.dwData2	= GetID();
			PACKEDDATA_OBJ->QuickSend(
				this,
				&msg,
				sizeof(msg));

			Die(NULL);
			m_DieTime = 0;
		}
	}
}

void CMonster::ProceedToTrigger()
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

DWORD CMonster::Damage( CObject* attackedObject, RESULTINFO* resultInfo )
{
	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();

	if( 1 == baseMonsterList.SpecialType )
	{
		resultInfo->RealDamage = 1;
	}
	else if(IsInvalidTarget(*attackedObject))
	{
		resultInfo->RealDamage = 0;
	}

	// 091113 LUJ, 모든 몬스터가 메시지를 보낼 경우 양이 많으므로, 피해를 입은 몬스터만 보내도록 한다
	Trigger::CMessage* const message = TRIGGERMGR->AllocateMessage(GetGridID());
	message->AddValue(Trigger::eProperty_ObjectIndex, GetID());
	message->AddValue(Trigger::eProperty_ObjectKind, GetObjectKind());
	message->AddValue(Trigger::eProperty_Event, Trigger::eEvent_CheckSelf);
	message->AddValue(Trigger::eProperty_MonsterKind, GetMonsterKind());

	return CObject::Damage( attackedObject, resultInfo );
}

void CMonster::RequestHelp(const RESULTINFO& resultInfo)
{
	if(0 == resultInfo.RealDamage)
	{
		return;
	}

	const BASE_MONSTER_LIST& baseMonsterList = GetSMonsterList();

	// 100104 LUJ, 도움을 요청하는 메시지를 표시해준다
	if(BASE_MONSTER_LIST::HelpNone != baseMonsterList.mHelpType)
	{
		g_pAISystem.SendMsg(
			eMK_HelpShout,
			GetID(),
			0,
			0,
			0);

		const MonSpeechInfo* const monsterSpeech = MON_SPEECHMGR->GetCurStateSpeechIndex(
			GetMonsterKind(),
			eMon_Speech_Help);

		if(monsterSpeech)
		{
			AddSpeech(
				monsterSpeech->SpeechType,
				monsterSpeech->SpeechIndex );
		}
	}

	switch(baseMonsterList.mHelpType)
	{
	case BASE_MONSTER_LIST::HelpLifeIsUnder50Percent: 
		{
			if(GetLife() > GetMaxLife() / 2)
			{
				break;
			}

			g_pAISystem.SendMsg(
				eMK_HelpRequest,
				GetID(),
				0,
				0,
				0);
			break;
		}
	case BASE_MONSTER_LIST::HelpLifeIsUnder30Percent: 
		{
			if(GetLife() > GetMaxLife() / 3)
			{
				break;
			}

			g_pAISystem.SendMsg(
				eMK_HelpRequest,
				GetID(),
				0,
				0,
				0);
			break;
		}
	case BASE_MONSTER_LIST::HelpDie: 
		{ 
			if(0 < GetLife())
			{
				break;
			}

			g_pAISystem.SendMsg(
				eMK_HelpRequest,
				GetID(),
				0,
				0,
				0);
			break;
		}
	case BASE_MONSTER_LIST::HelpAlways: 
		{
			g_pAISystem.SendMsg(
				eMK_HelpRequest,
				GetID(),
				0,
				0,
				0);
			break;
		}
	}
}

const BASE_MONSTER_LIST& CMonster::GetSMonsterList() const
{
	const BASE_MONSTER_LIST* const baseMonsterList = GAMERESRCMNGR->GetMonsterListInfo(GetMonsterKind());

	if(0 == baseMonsterList)
	{
		static const BASE_MONSTER_LIST emptyMonsterList = {0};
		return emptyMonsterList;
	}

	return *baseMonsterList;
}

DWORD CMonster::GetGravity()
{
	if(GetAbnormalStatus()->Attract)
	{
		return min(
			DWORD(GetAbnormalStatus()->Attract),
			GetSMonsterList().Gravity);
	}

	return GetSMonsterList().Gravity;
}

LEVELTYPE CMonster::GetLevel()
{
	return GetSMonsterList().Level;
}

DWORD CMonster::DoGetMaxLife()
{
	return GetSMonsterList().Life;
}

void CMonster::AddAggro(ObjectIndex objectIndex, AggroPoint aggroPoint, DWORD skillIndex)
{
	if(0 == aggroPoint)
	{
		return;
	}
	else if(objectIndex == GetID())
	{
		return;
	}
	else if(const cActiveSkillInfo* const skillInfo = SKILLMGR->GetActiveInfo(skillIndex))
	{
		if(const float rate = GetFiniteStateMachine().GetMemory().GetAggroRate(skillInfo->GetInfo().Unit))
		{
			const AggroPoint previousAggorPoint = (mAggroContainer.end() == mAggroContainer.find(objectIndex) ? 0 : mAggroContainer[objectIndex]);

			OutputDebug(
				"%s aggro: %s, %u + %0.0f(%0.0f%%)",
				GetObjectName(),
				skillInfo->GetInfo().Name,
				previousAggorPoint,
				aggroPoint * rate,
				rate * 100.0f);

			aggroPoint = AggroPoint(aggroPoint * rate);
		}
	}
	else if(const cBuffSkillInfo* const skillInfo = SKILLMGR->GetBuffInfo(skillIndex))
	{
		if(const float rate = GetFiniteStateMachine().GetMemory().GetAggroRate(skillInfo->GetInfo().Status))
		{
			const AggroPoint previousAggorPoint = (mAggroContainer.end() == mAggroContainer.find(objectIndex) ? 0 : mAggroContainer[objectIndex]);

			OutputDebug(
				"%s aggro: %s, %u + %0.0f(%0.0f%%)",
				GetObjectName(),
				skillInfo->GetInfo().Name,
				previousAggorPoint,
				aggroPoint * rate,
				rate * 100.0f);

			aggroPoint = AggroPoint(aggroPoint * rate);
		}
	}

	// 최초로 어그로가 들어온 경우 등록해준다
	if(mAggroContainer.end() == mAggroContainer.find(objectIndex))
	{
		if(CObject* const object = g_pUserTable->FindUser(objectIndex))
		{
			switch(object->GetObjectKind())
			{
			case eObjectKind_Player:
				{
					((CPlayer*)object)->AddToAggroed(
						GetID());
					break;
				}
			case eObjectKind_Pet:
				{
					((CPet*)object)->AddToAggroed(
						GetID());
					break;
				}
			}
		}
	}

	mAggroContainer[objectIndex] += aggroPoint;
}

void CMonster::AddToAggroed( ObjectIndex objectIndex )
{
	if( mAggroContainer.end() == mAggroContainer.find( objectIndex ) )
	{
		mAggroContainer.insert( AggroContainer::value_type( objectIndex, 0 ) );
	}
}

void CMonster::ThinkAggro()
{
	ObjectIndex targetObjectIndex = 0;
	AggroPoint maxAggroPoint = INT_MIN;

	for(AggroContainer::const_iterator iterator = mAggroContainer.begin();
		mAggroContainer.end() != iterator;
		++iterator)
	{
		const ObjectIndex objectIndex = iterator->first;
		const AggroPoint aggroPoint = iterator->second;

		if(aggroPoint < maxAggroPoint)
		{
			continue;
		}
		else if(aggroPoint > maxAggroPoint)
		{
			maxAggroPoint = aggroPoint;
			targetObjectIndex = objectIndex;
		}
		// 어그로 수치가 같을 경우 임의로 대상을 변경한다
		else if(rand() > RAND_MAX / 2)
		{
			targetObjectIndex = objectIndex;
		}
	}

	CObject* const object = g_pUserTable->FindUser(
		targetObjectIndex);

	if(0 == object)
	{
		return;
	}
	else if(GetTObject() == object)
	{
		return;
	}
	else if(SetTObject(object))
	{
		GSTATEMACHINE.SetState(
			this,
			eMA_ATTACK);
	}
}

BOOL CMonster::IsInvalidTarget(CObject& target) const
{
	if(GetOwnerIndex() == target.GetID())
	{
		return TRUE;
	}
	else if(eObjectKind_Monster & target.GetObjectKind())
	{
		CMonster& monsterTarget = (CMonster&)target;

		if(const BOOL isBamboo = (1 == monsterTarget.GetSMonsterList().SpecialType))
		{
			return TRUE;
		}
		else if(const BOOL isPuppy = (2 == monsterTarget.GetSMonsterList().SpecialType))
		{
			return TRUE;
		}
	}

	if(CObject* const myOwner = g_pUserTable->FindUser(GetOwnerIndex()))
	{
		const BOOL myOwnerIsMonster = (eObjectKind_Monster & myOwner->GetObjectKind());
		const BOOL targetIsMonster = (eObjectKind_Monster & target.GetObjectKind());

		if(CObject* const targetOwner = g_pUserTable->FindUser(target.GetOwnerIndex()))
		{
			if(myOwner->GetObjectKind() == targetOwner->GetObjectKind())
			{
				return TRUE;
			}

			const BOOL targetOwnerIsMonster = (eObjectKind_Monster & targetOwner->GetObjectKind());

			if(targetOwnerIsMonster & myOwnerIsMonster)
			{
				return TRUE;
			}
		}
		else if(myOwner->GetObjectKind() == target.GetObjectKind())
		{
			return TRUE;
		}
		else if(myOwnerIsMonster & targetIsMonster)
		{
			return TRUE;
		}
	}

	return FALSE;
}