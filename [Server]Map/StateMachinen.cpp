#include "StdAfx.h"
#include "StateMachinen.h"
#include "MsgRouter.h"
#include "Monster.h"
#include "CharMove.h"
#include "AISystem.h"
#include "PackedData.h"
#include "../[cc]skill/server/info/activeskillinfo.h"
#include "AIGroupManager.h"
#include "Player.h"
#include "TileManager.h"
#include "cMonsterSpeechManager.h"
#include "UserTable.h"
#include "Finite State Machine/Machine.h"
#include "Finite State Machine/Memory.h"

CStateMachinen GSTATEMACHINE;

CStateMachinen::CStateMachinen()
{}

CStateMachinen::~CStateMachinen()
{}

void CStateMachinen::RecvMsg( CObject * pSrcObject, CObject * pDestObject, MSG_OBJECT * msg)
{
	switch(msg->msgID)
	{
	case eMK_HelpShout:
		{
			CMonster* const pTargetMonster = (CMonster *)pDestObject;
			const BASE_MONSTER_LIST& monsterList = pTargetMonster->GetSMonsterList();

			if(FALSE == monsterList.bHearing)
			{
				break;
			}
			else if(monsterList.bDetectSameRace)
			{
				if(monsterList.MonsterRace != ((CMonster*)pSrcObject)->GetSMonsterList().MonsterRace)
				{
					break;
				}
			}

			VECTOR3 ObjectPos	= *CCharMove::GetPosition(pSrcObject);
			VECTOR3 TObjectPos	= *CCharMove::GetPosition(pDestObject);
			DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );

			if(Distance > monsterList.HearingDistance)
			{
				break;
			}

			const MonSpeechInfo* const monsterSpeech = MON_SPEECHMGR->GetCurStateSpeechIndex(
				pTargetMonster->GetMonsterKind(),
				eMon_Speech_AboutHelp);

			if(0 == monsterSpeech)
			{
				break;
			}

			((CMonster *)pDestObject)->AddSpeech(
				monsterSpeech->SpeechType,
				monsterSpeech->SpeechIndex );
			break;
		}
	case eMK_HelpRequest:
		{
			StateParameter& stateParamter = ((CMonster*)pDestObject)->mStateParamter;

			if(eMA_STAND != stateParamter.stateCur &&
				eMA_WALKAROUND != stateParamter.stateCur)
			{
				break;
			}

			CMonster* const pTargetMonster = (CMonster *)pDestObject;
			const BASE_MONSTER_LIST& monsterList = pTargetMonster->GetSMonsterList();

			if(FALSE == monsterList.bHearing)
			{
				break;
			}
			else if(monsterList.bDetectSameRace)
			{
				if(monsterList.MonsterRace != ((CMonster*)pSrcObject)->GetSMonsterList().MonsterRace)
				{
					break;
				}
			}

			VECTOR3 ObjectPos	= *CCharMove::GetPosition(pSrcObject);
			VECTOR3 TObjectPos	= *CCharMove::GetPosition(pDestObject);
			DWORD	Distance	= (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );

			if(Distance > monsterList.HearingDistance)
			{
				break;
			}

			CMonster* const sourceObject = (CMonster*)pSrcObject;

			SetHelperMonster(
				sourceObject,
				pTargetMonster,
				sourceObject->GetTObject());
			break;
		}
	}
}
void CStateMachinen::SetState(CObject * obj, eMONSTER_ACTION newState)
{
	StateParameter& stateParamter = ((CMonster*)obj)->mStateParamter;

	if( stateParamter.stateCur == newState )
	{
		return;
	}
    else if(newState != eMA_STAND || newState != eMA_ATTACK)
	{
		MSG_WORD msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_MONSTERMOVE_NOTIFY;
		msg.dwObjectID = obj->GetID();
		msg.wData = (WORD)newState;
		PACKEDDATA_OBJ->QuickSend(obj,&msg,sizeof(msg));
	}

	stateParamter.stateNew = newState;
	stateParamter.bStateUpdate = TRUE;
}

void CStateMachinen::Process(CObject* obj, eStateEvent evt)
{
	CMonster* const monster = (CMonster*)obj;
	StateParameter& stateParamter = monster->mStateParamter;

	if( monster->GetState() == eObjectState_Die ||
		monster->IsShocked() )
	{
		return;
	}
	else if(stateParamter.bStateUpdate)
	{
		stateParamter.bStateUpdate = FALSE;
		Update(
			*monster,
			eSEVENT_Leave);
		stateParamter.stateOld = stateParamter.stateCur;
		stateParamter.stateCur = stateParamter.stateNew;
		stateParamter.stateNew = eMA_STAND;
		Update(
			*monster,
			eSEVENT_Enter);
	}

	Update(
		*monster,
		evt);
	SubProcess(
		*monster,
		evt);
}

void CStateMachinen::SubProcess(CMonster& monster, eStateEvent evt)
{
	const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
	StateParameter& stateParamter = monster.mStateParamter;

	if( monster.GetAbnormalStatus()->IsStun ||
		monster.GetAbnormalStatus()->IsFreezing ||
		monster.GetAbnormalStatus()->IsStone ||
		monster.GetAbnormalStatus()->IsSlip ||
		monster.GetAbnormalStatus()->IsParalysis )
	{
		return;
	}

	switch(stateParamter.stateCur)
	{
	case eMA_STAND:
	case eMA_WALKAROUND:
		{
			const cActiveSkillInfo* const skillInfo = baseMonsterList.SkillInfo[monster.mStateParamter.CurAttackKind];

			if(0 == skillInfo)
			{
				break;
			}

			CObject* object = 0;

			if(monster.IsForeAttack() &&
				(stateParamter.SearchLastTime < gCurTime))
			{
				stateParamter.SearchLastTime = gCurTime + baseMonsterList.SearchPeriodicTime;
				CObject* const ownerObject = g_pUserTable->FindUser(monster.GetOwnerIndex());

				if(0 == ownerObject)
				{
					object = monster.DoSearch();
				}
				else if(eObjectKind_Player == ownerObject->GetObjectKind())
				{
					object = monster.DoFriendSearch(
						skillInfo->GetInfo().Range);
				}
				else if(eObjectKind_Monster & ownerObject->GetObjectKind())
				{
					object = monster.DoSearch();
				}
			}
			else if(baseMonsterList.AttackNum &&
				(stateParamter.CollSearchLastTime < gCurTime))
			{
				stateParamter.CollSearchLastTime = gCurTime + 500;
				CObject* const ownerObject = g_pUserTable->FindUser(monster.GetOwnerIndex());

				if(0 == ownerObject)
				{
					break;
				}
				else if(eObjectKind_Player == ownerObject->GetObjectKind())
				{
					object = monster.DoFriendSearch(
						skillInfo->GetInfo().Range);
				}
				else if(eObjectKind_Monster & ownerObject->GetObjectKind())
				{
					object = monster.DoSearch();
				}
			}

			if(0 == object)
			{
				break;
			}
			else if(FALSE == monster.SetTObject(object))
			{
				break;
			}

			SetState(
				&monster,
				eMA_PERSUIT);
		}
		break;
	}
}

void CStateMachinen::Update(CMonster& monster, eStateEvent evt)
{
	switch(monster.mStateParamter.stateCur)
	{
	case eMA_STAND:
		{
			DoStand(
				monster,
				evt);
			break;
		}
	case eMA_WALKAROUND:
		{
			DoWalkAround(
				monster,
				evt);
			break;
		}
	case eMA_PERSUIT:
		{
			DoPursuit(
				monster,
				evt);
			break;
		}
	case eMA_WALKAWAY:
	case eMA_RUNAWAY:
		{
			DoRunAway(
				monster,
				evt);
			break;
		}
	case eMA_ATTACK:
		{
			DoAttack(
				monster,
				evt);
			break;
		}
	case eMA_SCRIPT_RUN:
	case eMA_SCRIPT_WALK:
		{
			DoScriptMove(
				monster,
				evt);
			break;
		}
	case eMA_PAUSE:
		{
			DoPause(
				monster,
				evt);
			break;
		}
	}
}

void CStateMachinen::DoStand(CMonster& monster, eStateEvent evt)
{
	const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
	StateParameter& stateParamter = monster.mStateParamter;
	
	switch(evt)
	{
	case eSEVENT_Enter:
		{
			// 소유주가 전투중인 경우 도와주러 간다
			if(CObject* const ownerObject = g_pUserTable->FindUser(monster.GetOwnerIndex()))
			{
				if(ownerObject->GetObjectBattleState() &&
					TRUE == monster.SetTObject(ownerObject->GetTObject()))
				{
					SetState(
						&monster,
						eMA_PERSUIT);
					stateParamter.nextTime = gCurTime + 500;
					break;
				}
			}

			stateParamter.nextTime	= gCurTime + baseMonsterList.StandTime;
			monster.DoStand();

			if(FALSE == monster.IsNoCheckCollision())
			{
				break;
			}

			VECTOR3 vPos = {0};
			monster.GetPosition(&vPos);

			if(g_pServerSystem->GetMap()->CollisionTilePos(
				vPos.x,
				vPos.z,
				g_pServerSystem->GetMapNum()))
			{
				break;
			}

			monster.SetNoCheckCollision(FALSE);
			break;
		}
	case eSEVENT_Process:
		{
			if(stateParamter.nextTime > gCurTime)
			{
				break;
			}

			Mon_SpeechState speechState = eMon_Speech_MAX;
			const BYTE rate = BYTE(rand() % 100);

			if(rate < baseMonsterList.StandRate)
			{
				SetState(
					&monster,
					eMA_STAND);
				speechState = eMon_Speech_KeepWalkAround;
			}
			else
			{
				SetState(
					&monster,
					eMA_WALKAROUND);
				speechState = eMon_Speech_WalkAround;
			}

			// 100617 ONS 정지상태에서 타겟이 죽었을 경우, 배회상태로 변경한다.
			if( monster.GetTObject() && monster.GetTObject()->GetState() == eObjectState_Die )
			{
				monster.SetTObject(0);
				SetState(
					&monster,
					eMA_WALKAROUND);
			}

			const MonSpeechInfo* const speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex(
				monster.GetMonsterKind(),
				speechState);

			if(0 == speechInfo)
			{
				break;
			}

			monster.AddSpeech(
				speechInfo->SpeechType,
				speechInfo->SpeechIndex);
			break;
		}
	}
}

void CStateMachinen::DoWalkAround(CMonster& monster, eStateEvent evt)
{	
	const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
	StateParameter& stateParamter = monster.mStateParamter;	
	
	if( monster.GetAbnormalStatus()->IsStun ||
		monster.GetAbnormalStatus()->IsFreezing ||
		monster.GetAbnormalStatus()->IsStone ||
		monster.GetAbnormalStatus()->IsSlip ||
		monster.GetAbnormalStatus()->IsMoveStop ||
		monster.GetAbnormalStatus()->IsParalysis )

	{
		SetState(
			&monster,
			eMA_STAND);
		return;
	}

	switch(evt)
	{
	case eSEVENT_Enter:
		{
			// 소유주가 있을 경우 그 중심으로 행동한다
			if(CObject* const ownerObject = g_pUserTable->FindUser(monster.GetOwnerIndex()))
			{
				if(ownerObject->GetObjectBattleState())
				{
					SetState(
						&monster,
						eMA_STAND);
					break;
				}

				VECTOR3 ownerPosition = {0};
				ownerObject->GetPosition(
					&ownerPosition);

				const float distance = ownerObject->GetRadius() * 3;
				VECTOR3 monsterPosition = {0};
				monster.GetPosition(
					&monsterPosition);

				if(distance > CalcDistanceXZ(&ownerPosition, &monsterPosition))
				{
					SetState(
						&monster,
						eMA_STAND);
					break;
				}

				const float randomRateX = float(rand()) / RAND_MAX;
				const float randomRateZ = float(rand()) / RAND_MAX;
				const float randomAxisX = (randomRateX < 0.5f ? -1.0f : 1.0f) * (ownerObject->GetRadius() * (1.0f + randomRateX));
				const float randomAxisZ = (randomRateZ < 0.5f ? -1.0f : 1.0f) * (ownerObject->GetRadius() * (1.0f + randomRateZ));

				monster.GetFiniteStateMachine().GetMemory().SetVariable(
					"__move_x__",
					int(ownerPosition.x + randomAxisX));
				monster.GetFiniteStateMachine().GetMemory().SetVariable(
					"__move_z__",
					int(ownerPosition.z + randomAxisZ));
				SetState(
					&monster,
					eMA_SCRIPT_RUN);

				// 이렇게 하지 않으면 eSEVENT_Process 루틴으로 들어가서 상태를 바꿔버린다
				stateParamter.nextTime = gCurTime + 500;
				break;
			}

            monster.SetTObject(0);
			monster.DoWalkAround();
			
			stateParamter.nextTime	= gCurTime + CCharMove::GetMoveEstimateTime(&monster);
			break;
		}

	case eSEVENT_Process:
		{
			if( stateParamter.nextTime > gCurTime )
			{
				break;
			}

			Mon_SpeechState speechState = eMon_Speech_MAX;
			const int rate = rand() % 100;

			if(0 <= rate && rate < baseMonsterList.StandRate )
			{
				SetState(
					&monster,
					eMA_STAND);
				speechState = eMon_Speech_Stand;
			}
			else
			{
				SetState(
					&monster,
					eMA_WALKAROUND);
				speechState = eMon_Speech_KeepStand;
			}

			const MonSpeechInfo* const speechInfo = MON_SPEECHMGR->GetCurStateSpeechIndex(
				monster.GetMonsterKind(),
				speechState);

			if(0 == speechInfo)
			{
				break;
			}

			monster.AddSpeech(
				speechInfo->SpeechType,
				speechInfo->SpeechIndex);
			break;
		}
	}
}

void CStateMachinen::DoPursuit(CMonster& monster, eStateEvent evt)
{
	const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
	StateParameter& stateParamter = monster.mStateParamter;
	
	if(0 == baseMonsterList.SkillInfo[stateParamter.CurAttackKind])
	{
		SetState(
			&monster,
			eMA_STAND);
		return;
	}
	else if(0 == monster.GetTObject() ||
		(monster.GetTObject() && (eObjectState_Die == monster.GetTObject()->GetState())) ||
		FALSE == monster.GetTObject()->GetInited())
	{
		SetState(
			&monster,
			eMA_STAND);
		return;
	}
	else if( monster.GetAbnormalStatus()->IsStun ||
		monster.GetAbnormalStatus()->IsFreezing ||
		monster.GetAbnormalStatus()->IsStone ||
		monster.GetAbnormalStatus()->IsSlip ||
		monster.GetAbnormalStatus()->IsMoveStop ||
		monster.GetAbnormalStatus()->IsParalysis )
	{
		return;
	}

	switch(evt)
	{
	case eSEVENT_Enter:
		{
			stateParamter.PursuitForgiveStartTime = gCurTime + baseMonsterList.PursuitForgiveTime;
			stateParamter.nextTime = gCurTime + 1000;
			monster.DoPursuit();
			RandCurAttackKind(
				baseMonsterList,
				stateParamter);
			monster.SetNoCheckCollision(TRUE);
			break;
		}
	case eSEVENT_Process:
		{
			VECTOR3 ObjectPos = *CCharMove::GetPosition(&monster);
			VECTOR3 TObjectPos = *CCharMove::GetPosition(monster.GetTObject());
			DWORD Distance = 0;

			float fDist = CalcDistanceXZ( &ObjectPos, &TObjectPos ) - monster.GetRadius();

			if( fDist > 0.f )
			{
				Distance = (DWORD)fDist;
			}
			
			if( stateParamter.prePursuitForgiveTime == 0 && ( stateParamter.PursuitForgiveStartTime < gCurTime ||
				baseMonsterList.PursuitForgiveDistance < Distance ) )
			{
				monster.RemoveAllAggro();

				stateParamter.SearchLastTime = gCurTime + 3000; //3초간 선공몹 서치를 안하게 하기 위해서.
				SetState(
					&monster,
					eMA_WALKAROUND);

				MonSpeechInfo* pTemp = MON_SPEECHMGR->GetCurStateSpeechIndex( monster.GetMonsterKind(), eMon_Speech_ForgivePursuit );
				if( pTemp )
					monster.AddSpeech( pTemp->SpeechType, pTemp->SpeechIndex );	
			}
			else if( Distance < baseMonsterList.SkillInfo[stateParamter.CurAttackKind]->GetInfo().Range )
			{
				SetState(
					&monster,
					eMA_ATTACK);
				break;
			}
			else if( stateParamter.nextTime > gCurTime )
			{
				break;
			}

			stateParamter.nextTime = gCurTime + 1000;
			monster.DoPursuit();
			RandCurAttackKind(
				baseMonsterList,
				stateParamter);
			break;
		}
	}
}

void CStateMachinen::DoAttack(CMonster& monster, eStateEvent evt)
{
	const BASE_MONSTER_LIST& baseMonsterList = monster.GetSMonsterList();
	StateParameter& stateParamter = monster.mStateParamter;

	if( monster.GetAbnormalStatus()->IsStun ||
		monster.GetAbnormalStatus()->IsFreezing ||
		monster.GetAbnormalStatus()->IsStone ||
		monster.GetAbnormalStatus()->IsSlip ||
		monster.GetAbnormalStatus()->IsParalysis )
	{
		return;
	}

	switch(evt)
	{
	case eSEVENT_Enter:
		{
			stateParamter.AttackStartTime = 0;
			stateParamter.nextTime = 0;
			break;
		}
	case eSEVENT_Process:
		{
			if(stateParamter.AttackStartTime > gCurTime)
			{
				break;
			}
			else if(stateParamter.CurAttackKind >= baseMonsterList.AttackNum)
			{
				SetState(
					&monster,
					eMA_STAND);
				break;
			}
			else if(0 == monster.GetTObject() ||
				(monster.GetTObject() && ( monster.GetTObject()->GetState() == eObjectState_Die || monster.GetTObject()->GetInited() == FALSE )))
			{
				SetState(
					&monster,
					eMA_WALKAROUND);
				break;
			}
			else if(monster.IsInvalidTarget(*monster.GetTObject()))
			{
				monster.SetTObject(
					0);
				SetState(
					&monster,
					eMA_STAND);
				break;
			}

			const size_t maxSkillSize = sizeof(baseMonsterList.SkillInfo) / sizeof(*baseMonsterList.SkillInfo);

			if(maxSkillSize <= stateParamter.CurAttackKind)
			{
				break;
			}

			cActiveSkillInfo* const activeSkillInfo = baseMonsterList.SkillInfo[stateParamter.CurAttackKind];

			if(0 == activeSkillInfo)
			{
				break;
			}

			VECTOR3 ObjectPos = *CCharMove::GetPosition(&monster);
			VECTOR3 TObjectPos = *CCharMove::GetPosition(monster.GetTObject());
			const DWORD	Distance = (DWORD)CalcDistanceXZ( &ObjectPos, &TObjectPos );

			if(Distance > activeSkillInfo->GetInfo().Range)
			{
				SetState(
					&monster,
					eMA_PERSUIT);
				break;
			}

			stateParamter.AttackStartTime = gCurTime + baseMonsterList.SkillInfo[stateParamter.CurAttackKind]->GetInfo().CoolTime;
			monster.DoAttack(baseMonsterList.AttackIndex[stateParamter.CurAttackKind]);
			RandCurAttackKind(
				baseMonsterList,
				stateParamter);
			break;
		}
	}
}

void CStateMachinen::DoRunAway(CMonster& monster, eStateEvent evt)
{
	StateParameter& stateParamter = monster.mStateParamter;
	
	if( monster.GetAbnormalStatus()->IsStun ||
		monster.GetAbnormalStatus()->IsFreezing ||
		monster.GetAbnormalStatus()->IsStone ||
		monster.GetAbnormalStatus()->IsSlip ||
		monster.GetAbnormalStatus()->IsMoveStop ||
		monster.GetAbnormalStatus()->IsParalysis )
	{
		return;
	}

	switch(evt)
	{
	case eSEVENT_Enter:
		{
			monster.RemoveAllAggro();
			monster.SetTObject(0);

			VECTOR3 position = {0};
			position.x = float(monster.GetFiniteStateMachine().GetMemory().GetVariable(
				"__runaway_x__"));
			position.z = float(monster.GetFiniteStateMachine().GetMemory().GetVariable(
				"__runaway_z__"));
			monster.OnMove(
				&position);

			stateParamter.nextTime = gCurTime + CCharMove::GetMoveEstimateTime(&monster) + 1000;
			break;
		}
	case eSEVENT_Process:
		{
			if(gCurTime > stateParamter.nextTime)
			{
				SetState(
					&monster,
					eMA_STAND);
				break;
			}

			break;
		}
	}
}

void CStateMachinen::DoScriptMove(CMonster& monster, eStateEvent event)
{
	StateParameter& stateParamter = monster.mStateParamter;

	switch(event)
	{
	case eSEVENT_Enter:
		{
			VECTOR3 position = {
				float(monster.GetFiniteStateMachine().GetMemory().GetVariable("__move_x__")),
				0,
				float(monster.GetFiniteStateMachine().GetMemory().GetVariable("__move_z__")),
			};
			monster.OnMove(
				&position);

			stateParamter.nextTime = gCurTime + CCharMove::GetMoveEstimateTime(
				&monster);
			break;
		}
	case eSEVENT_Process:
		{
			if( stateParamter.nextTime > gCurTime )
			{
				break;
			}

			SetState(
				&monster,
				eMA_STAND);
			break;
		}
	}
}

void CStateMachinen::DoPause(CMonster& monster, eStateEvent event)
{
	StateParameter& stateParamter = monster.mStateParamter;

	switch(event)
	{
	case eSEVENT_Enter:
		{
			const DWORD pausedTick = monster.GetFiniteStateMachine().GetMemory().GetVariable(
				"__tick__");

			stateParamter.nextTime = gCurTime + pausedTick;
			break;
		}
	case eSEVENT_Process:
		{
			if(gCurTime > stateParamter.nextTime)
			{
				const eMONSTER_ACTION pausedAction = eMONSTER_ACTION(monster.GetFiniteStateMachine().GetMemory().GetVariable(
					"__lastState__"));
                SetState(
					&monster,
					pausedAction);
			}

			break;
		}
	}
}

void CStateMachinen::SetHelperMonster(CMonster* pAsker, CMonster* pHelper, CObject* pTargeter)
{
	if( pAsker == NULL && pHelper == NULL || pTargeter == 0)
		return;

	if(FALSE == (pHelper->GetObjectKind() & eObjectKind_Monster))
		return;

	if( pHelper->GetGridID() != pTargeter->GetGridID())
		return;

	if ( pHelper->mStateParamter.stateOld == eMA_PERSUIT ) return;

	if(pHelper->mStateParamter.stateCur == eMA_RUNAWAY ||
		pHelper->mStateParamter.stateCur == eMA_WALKAWAY)
		return;

	if( IsTargetChange(pTargeter, pHelper) )
	{
		GSTATEMACHINE.SetState(pHelper, eMA_ATTACK);
		pHelper->SetTObject( pTargeter );
		pHelper->mStateParamter.pHelperMonster = pAsker;

		MonSpeechInfo* pTemp = MON_SPEECHMGR->GetCurStateSpeechIndex( pAsker->GetMonsterKind(), eMon_Speech_Help );
		if( pTemp )
			pAsker->AddSpeech( pTemp->SpeechType, pTemp->SpeechIndex );
	}
}

DWORD CStateMachinen::RandCurAttackKind(const BASE_MONSTER_LIST& baseMonsterList, StateParameter& stateParamter)
{
	DWORD sRate = 0;
	DWORD eRate = 0;
	DWORD rate = rand() % 99;

	for(DWORD i = 0; i < baseMonsterList.AttackNum ; ++i)
	{
		eRate += baseMonsterList.AttackRate[i];

		if( sRate <= rate && rate < eRate )
		{
			return (stateParamter.CurAttackKind = i);
		}

		sRate = eRate;
	}

	return stateParamter.CurAttackKind = 0;
}

BOOL CStateMachinen::IsTargetChange(CObject* pAttacker, CMonster* pDefender)
{
	CObject* const pTObject = pDefender->GetTObject();

	if(pAttacker == pTObject)
	{
		return FALSE;
	}
	else if(0 == pTObject)
	{
		return TRUE;
	}

	const BASE_MONSTER_LIST& baseMonsterList = pDefender->GetSMonsterList();

	if(FALSE == baseMonsterList.TargetChange)
	{
		return FALSE;
	}

	VECTOR3 AttackerPos	= *CCharMove::GetPosition(pAttacker);
	VECTOR3 TObjectPos	= *CCharMove::GetPosition(pTObject);
	VECTOR3 DefenderPos	= *CCharMove::GetPosition(pDefender);
	DWORD FormerDistance1 = (DWORD)CalcDistanceXZ( &TObjectPos, &DefenderPos );
	DWORD NewDistance2 = (DWORD)CalcDistanceXZ( &AttackerPos, &DefenderPos );

	LONG DistGab = FormerDistance1-NewDistance2;

	if(-50 <= DistGab && DistGab <= 50)
	{
		return pAttacker->GetLife() < pTObject->GetLife();
	}

	return DistGab > 0;
}