#include "stdafx.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "ObjectStateManager.h"
#include "GameResourceManager.h"
#include "GameIn.h"
#include ".\Engine\EngineObject.h"
#include "MHMap.h"
#include "MovePoint.h"
#include "CAction.h"
#include "ActionTarget.h"
#include "TileManager.h"
#include "PartyIconManager.h"
#include "MacroManager.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "NpcScriptDialog.h"
#include "PetManager.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "./FadeDlg.h"
#include "cWindowManager.h"
#include "ChatManager.h"
#include "cMsgBox.h"
#include "WindowIDEnum.h"
#include "VehicleManager.h"
#include "cHousingMgr.h"

GLOBALTON(CMoveManager)
CMoveManager::CMoveManager()
{

}

CMoveManager::~CMoveManager()
{}

void CMoveManager::InitMove(CObject* pObject,BASEMOVE_INFO* pMoveInfo,float AngleDeg)
{
	memset( &pObject->m_MoveInfo, 0, sizeof(pObject->m_MoveInfo) );

	pObject->m_RotateInfo.Angle.SetAngleDeg(AngleDeg);
	pObject->m_RotateInfo.bRotating = FALSE;

	if(pMoveInfo)
	{
		memcpy(&pObject->m_MoveInfo,pMoveInfo,sizeof(BASEMOVE_INFO));
		if(pMoveInfo->bMoving)
		{
			//StartMove(pObject,gCurTime,&pMoveInfo->TargetPosition);
			StartMoveEx(pObject,NULL, gCurTime);
		}
	}

	pObject->m_MoveInfo.m_bLastMoving = FALSE;

	pObject->SetPosition(&pObject->m_MoveInfo.CurPosition);

	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		((CPlayer*)pObject)->SetBaseMotion();
	}
}

void CMoveManager::SetPosition(CObject* pObject,VECTOR3* pPos)
{
	pObject->m_MoveInfo.CurPosition = *pPos;
	pObject->SetPosition(pPos);
}

void CMoveManager::EndYMove(CObject* pObject)
{
	pObject->m_bIsYMoving = FALSE;
	pObject->GetEngineObject()->ApplyHeightField(TRUE);
}

void CMoveManager::EndMove(CObject* pObject)
{
	if(pObject->GetState() == eObjectState_Move)
		OBJECTSTATEMGR->EndObjectState(pObject,eObjectState_Move);

	pObject->m_MoveInfo.m_bEffectMoving = FALSE;
	pObject->m_MoveInfo.bMoving = FALSE;
	pObject->m_MoveInfo.InitTargetPosition();

	if( pObject == HERO )
	{
		HeroMoveStop();
	}
}

// 부분 타겟 업데이트 
int CMoveManager::TargetUpdate(CObject* pObject, MOVE_INFO * pMoveInfo, VECTOR3 * TargetPos)
{
	//VECTOR3 * TargetPos = &pMoveInfo->TargetPositions[pMoveInfo->CurTargetPositionIdx];
	// 타겟 거리의 합
	float dx = TargetPos->x - pMoveInfo->CurPosition.x;
	float dz = TargetPos->z - pMoveInfo->CurPosition.z;
	float Distance = sqrtf(dx*dx+dz*dz);
	if(Distance <= 0.01)
	{
		EndMove(pObject);
		return 1;
	}

	float Speed = pObject->GetMoveSpeed();
	float ttt = Speed / Distance;

	pMoveInfo->Move_StartTime			= gCurTime;
	pMoveInfo->Move_StartPosition		= pMoveInfo->CurPosition;
	pMoveInfo->Move_Direction.x			= dx*ttt;
	pMoveInfo->Move_Direction.y			= 0;
	pMoveInfo->Move_Direction.z			= dz*ttt;
	pMoveInfo->Move_EstimateMoveTime	= Speed==0 ? 999999999 : Distance / Speed;

	return 0;
}
void CMoveManager::StartMoveEx(CObject* pObject, VECTOR3* pStartPos ,DWORD CurTime)
{
	if( !	pObject->GetMoveSpeed() &&
			IsMoving(pObject) )
	{
		EndMove(pObject);
		return;
	}

	ASSERT( pObject->m_MoveInfo.GetCurTargetPosIdx() <= pObject->m_MoveInfo.GetMaxTargetPosIdx() );	//0000
	StartMoveEx(pObject, CurTime, pObject->m_MoveInfo.GetTargetPosition(pObject->m_MoveInfo.GetCurTargetPosIdx()));

	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* pPlayer = (CPlayer*)pObject;

		// 090914 ShinJS --- 자신소유의 탈것에 탑승중인 경우 탈것의 이동정보를 일치시킨다.
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
		if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->GetOwnerIndex() == pPlayer->GetID() && pVehicle->IsGetOn( pPlayer ) )
		{
			pVehicle->m_MoveInfo = pPlayer->m_MoveInfo;
		}
	}
}


void CMoveManager::StartMoveEx(CObject* pObject, DWORD CurTime, VECTOR3 * pTargetPos)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;

	if(pObject->GetState() == eObjectState_Die)
	{
		pMoveInfo->bMoving = FALSE;
		return;
	}

	if(pObject->m_bIsYMoving)
	{
		EndYMove(pObject);
	}

	pMoveInfo->bMoving = TRUE;
	pObject->m_MoveInfo.m_bEffectMoving = FALSE;
	
	if(TargetUpdate(pObject, pMoveInfo, pTargetPos) != 0)
	{
		return;
	}
	
	float TargetAngle = RADTODEG(VECTORTORAD(pMoveInfo->Move_Direction));
	pObject->m_RotateInfo.Angle.SetTargetDeg(TargetAngle);
	float fChangedAngle = pObject->m_RotateInfo.Angle.GetChangeAmountDeg();
	if(fChangedAngle < 0)
		fChangedAngle*=-1.f;
	pObject->m_RotateInfo.EstimatedRotateTime = 0.1f + fChangedAngle * 0.2f / 180.f;
	pObject->m_RotateInfo.Rotate_StartTime = CurTime;
	pObject->m_RotateInfo.bRotating = TRUE;

	// 071122 웅주. 스킬 동작이 끝나지 않은 채로 이동되는 경우가 있다. 이때 이동 동작으로
	//	바로 잡아주지 않으면, 미끄러지기/잘못된 방향으로 뛰기 등이 나타난다.StartMoveEx()를
	//	실행한다는 것이 이미 해당 오브젝트에 이동이 승인되었다는 뜻이므로 모션을 변경해준다.
	if( pObject->GetState() == eObjectState_SkillSyn )
	{
		pObject->SetState( eObjectState_Move );
	}
	else if( pObject->GetState() == eObjectState_Move )
	{
		pObject->SetMotionInState(eObjectState_Move);
		OBJECTSTATEMGR->EndObjectState(pObject,eObjectState_Move,(DWORD)(pMoveInfo->Move_EstimateMoveTime*1000));
	}
	else
	{
		if(OBJECTSTATEMGR->StartObjectState(pObject,eObjectState_Move) == TRUE)
			OBJECTSTATEMGR->EndObjectState(pObject,eObjectState_Move,(DWORD)(pMoveInfo->Move_EstimateMoveTime*1000));		
	}
}

#define MOVESKIPCOUNT2	1
BOOL CMoveManager::CalcPositionEx(CObject* pObject,DWORD CurTime)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	
	if(pMoveInfo->bMoving)
	{
		if(IsGameSlow() == TRUE && pObject != HERO)
		{
			if(pObject->m_bMoveSkipCount < MOVESKIPCOUNT2)
			{
				++pObject->m_bMoveSkipCount;
				return FALSE;
			}
			else
				pObject->m_bMoveSkipCount = 0;
		}

		BOOL bVal = FALSE;
		float fMoveTime = ((CurTime - pMoveInfo->Move_StartTime)*0.001f);

		if(pMoveInfo->Move_EstimateMoveTime < fMoveTime)
		{
			// 다음 타겟으로 변경

			//---KES FindPatch Fix 071020
			//if( pMoveInfo->GetMaxTargetPosIdx() > 0 && pMoveInfo->GetCurTargetPosIdx() < pMoveInfo->GetMaxTargetPosIdx()- 1)
			if(pMoveInfo->GetCurTargetPosIdx() < pMoveInfo->GetMaxTargetPosIdx()- 1)	//뒤집힐 경우가 있을 수 있을 것 같은데...
			//------------------------
			{
				ASSERT( pMoveInfo->GetCurTargetPosIdx() <= MAX_CHARTARGETPOSBUF_SIZE );	//0000
				pMoveInfo->CurPosition.x = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x;
				pMoveInfo->CurPosition.z = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z;
				pMoveInfo->SetCurTargetPosIdx(pMoveInfo->GetCurTargetPosIdx()+1);
				ASSERT( pMoveInfo->GetCurTargetPosIdx() <= MAX_CHARTARGETPOSBUF_SIZE );	//0000
				StartMoveEx(pObject, CurTime, pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx()));
			}
			else
			{
				ASSERT( pMoveInfo->GetCurTargetPosIdx() <= MAX_CHARTARGETPOSBUF_SIZE );	//0000
				pMoveInfo->CurPosition.x = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x;
				pMoveInfo->CurPosition.z = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z;
				bVal = TRUE;
			}
		}
		else
		{
			pMoveInfo->CurPosition.x = pMoveInfo->Move_StartPosition.x + pMoveInfo->Move_Direction.x * fMoveTime;
			pMoveInfo->CurPosition.z = pMoveInfo->Move_StartPosition.z + pMoveInfo->Move_Direction.z * fMoveTime;
		}

		//---KES FindPatch Fix 071020
		//---(0,0) force block
		//pObject->SetPosition(&pObject->m_MoveInfo.CurPosition);
		if( pObject->m_MoveInfo.CurPosition.x != 0 && pObject->m_MoveInfo.CurPosition.z != 0 )
		{
			pObject->SetPosition(&pObject->m_MoveInfo.CurPosition);
		}
		else
		{
			ASSERT(0);
		}
		//----------------------------------
        
		return bVal;
	}


	return FALSE;
}

BOOL CMoveManager::CalcYPosition(CObject* pObject,DWORD CurTime)
{
	if(pObject->m_bIsYMoving)
	{
		MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;

		BOOL bVal = FALSE;
		float fMoveTime = ((CurTime - pMoveInfo->Move_StartTime)*0.001f);
		if(pMoveInfo->Move_EstimateMoveTime < fMoveTime)
		{
			pMoveInfo->CurPosition.y = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->y;
			bVal = TRUE;
		}
		else
		{
			pMoveInfo->CurPosition.y = pMoveInfo->Move_StartPosition.y + pMoveInfo->Move_Direction.y * fMoveTime;
		}
		
		pObject->SetPosition(&pMoveInfo->CurPosition);

		return bVal;
	}

	return FALSE;
}

void CMoveManager::CalcAngle(CObject* pObject,DWORD CurTime)
{	
	if(pObject->m_RotateInfo.bRotating)
	{
		float fRotateTime = ((CurTime - pObject->m_RotateInfo.Rotate_StartTime)*0.001f);
		if(fRotateTime >= pObject->m_RotateInfo.EstimatedRotateTime)
		{
			pObject->m_RotateInfo.Angle.Process(1);
			pObject->m_RotateInfo.bRotating = FALSE;
		}
		else
		{
			pObject->m_RotateInfo.Angle.Process(fRotateTime / pObject->m_RotateInfo.EstimatedRotateTime);
		}
		pObject->SetAngle(pObject->m_RotateInfo.Angle.ToRad());
	}
}

void CMoveManager::MoveProcess(CObject* pObject)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	if(pObject->m_bIsYMoving)
	{
		if(CalcYPosition(pObject,gCurTime) == TRUE)
			EndYMove(pObject);
	}
	else
	{
		if(CalcPositionEx(pObject,gCurTime) == TRUE)
			EndMove(pObject);
	}

	// 중력 적용
	if( pMoveInfo->bApplyGravity )
	{
		// 중력 가속도 적용
		pMoveInfo->fGravitySpeed = pMoveInfo->fGravitySpeed + pMoveInfo->fGravityAcceleration * gTickTime * 0.001f * pObject->GetWeight();

		VECTOR3 curPosition={0};
		pObject->GetPosition( &curPosition );
		curPosition.y = curPosition.y + (pMoveInfo->fGravitySpeed * gTickTime * 0.001f) * 100.f;
		pObject->SetPosition( &curPosition );

		pMoveInfo->CurPosition.y = curPosition.y;

		if( pMoveInfo->dwValidGravityTime > 0 &&
			pMoveInfo->dwValidGravityTime < gCurTime )
		{
			pObject->ApplyGravity( FALSE, 0.f, 0 );
		}
	}

	CalcAngle(pObject,gCurTime);
	pMoveInfo->m_bLastMoving = pMoveInfo->bMoving;
}

BOOL CMoveManager::SetHeroTarget(VECTOR3* pPos,BOOL bMousePointDisplay)
{
	CHero* pHero = OBJECTMGR->GetHero();

	if( pHero->GetAbnormalStatus()->IsStun ||
		pHero->GetAbnormalStatus()->IsFreezing ||
		pHero->GetAbnormalStatus()->IsStone ||
		pHero->GetAbnormalStatus()->IsSlip ||
		pHero->GetAbnormalStatus()->IsParalysis ||
		pHero->IsShocked() )
	{
		return FALSE;
	}

	if(pHero->GetState() ==  eObjectState_Rest )
	{
		OBJECTSTATEMGR->StartObjectState(pHero, eObjectState_None);
		OBJECTSTATEMGR->EndObjectState(pHero, eObjectState_Rest, 2500);

		MSG_BYTE msg ;

		msg.Category	= MP_EMOTION ;
		msg.Protocol	= MP_EMOTION_SITSTAND_SYN ;
		msg.dwObjectID	= gHeroID ;
		msg.bData		= eObjectState_None ;

		NETWORK->Send(&msg, sizeof(MSG_BYTE));
	}

	if( pHero->GetState() != eObjectState_None &&
		pHero->GetState() != eObjectState_Move &&
		pHero->GetState() != eObjectState_TiedUp_CanMove &&
		pHero->GetState() != eObjectState_Immortal &&
		pHero->GetState() != eObjectState_ImmortalDefense &&
		pHero->GetState() != eObjectState_HouseRiding)
	{
		if( pHero->GetState() == eObjectState_Society )
		{
			if( pHero->EndSocietyAct() == FALSE )
			{
				CAction act;
				act.InitMoveAction(pPos);
				pHero->SetNextAction(&act);
				pHero->DisableAutoAttack();
				return FALSE;
			}
		}
		else
		{
			CAction act;
			act.InitMoveAction(pPos);
			pHero->SetNextAction(&act);
			pHero->DisableAutoAttack();
			return FALSE;
		}
	}
	if(pHero->GetState() == eObjectState_HouseRiding &&
		FALSE == HOUSINGMGR->RequestAction_GetOff())
		return FALSE;

	if( pHero->GetState() == eObjectState_Move &&
		pHero->GetEngineObject()->GetCurMotion() == pHero->m_ObjectState.State_End_Motion)
		return FALSE;

	// 090422 ShinJS --- Hero가 탈것에 탑승중이고 이동 가능하지 않은 경우
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pHero->GetVehicleID() );
	if( pVehicle && 
		pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
		pVehicle->IsGetOn( pHero ) &&
		!pVehicle->CanControl( pHero ) )
		return FALSE;
	
	// 090907 ShinJS --- 길찾기를 이용한 이동처리 (5팀 Source 적용)
	// 091207 ShinJS --- 지형 클릭 이동시 이동불가지역인 경우 이동처리 추가
	return HERO->Move_UsePath( pPos, TRUE, bMousePointDisplay, TRUE );
}

void CMoveManager::SetHeroPetTarget()
{
	CHero* pHero = OBJECTMGR->GetHero();
	CHeroPet* pHeroPet = OBJECTMGR->GetHeroPet();
	VECTOR3 pos;
	
	if( IsMoving( pHero ) )
		CalcPositionEx( pHero, gCurTime );
	if( IsMoving( pHeroPet ) )
		CalcPositionEx( pHeroPet, gCurTime );

	int ran=0;
	float dx=0,dz=0;
	ran = rand();
	dx = float( ran % ( PETMGR->GetAreaDistance() ) ) * ( ran % 2 ? 1 : -1 );
	ran = rand();
	dz = float( ran % ( PETMGR->GetAreaDistance() ) ) * ( ran % 2 ? 1 : -1 );
	pos.x = pHero->m_MoveInfo.CurPosition.x + dx / 2;
	pos.z = pHero->m_MoveInfo.CurPosition.z + dz / 2;

	if( pos.x < 0 )				pos.x = 0;
	else if( pos.x > 51199 )	pos.x = 51199;

	if( pos.z < 0 )				pos.z = 0;
	else if( pos.z > 51199 )	pos.z = 51199;

	pos.x = ((DWORD)(pos.x/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);
	pos.z = ((DWORD)(pos.z/TILECOLLISON_DETAIL)*TILECOLLISON_DETAIL);


	MOVE_ONETARGETPOS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_PET_MOVE;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = pHeroPet->GetID();
	msg.SetStartPos( &pHeroPet->m_MoveInfo.CurPosition );
	msg.SetTargetPos( &pos );

	NETWORK->Send( &msg,msg.GetSize() );
}

void CMoveManager::SetHeroPetTarget(VECTOR3* pPos)
{
	CHeroPet* pHeroPet = OBJECTMGR->GetHeroPet();

	if( pHeroPet->GetAbnormalStatus()->IsStun ||
		pHeroPet->GetAbnormalStatus()->IsFreezing ||
		pHeroPet->GetAbnormalStatus()->IsStone ||
		pHeroPet->GetAbnormalStatus()->IsSlip ||
		pHeroPet->GetAbnormalStatus()->IsParalysis )
	{
		return;
	}

	if( pHeroPet->GetState() != eObjectState_None &&
		pHeroPet->GetState() != eObjectState_Move &&
		pHeroPet->GetState() != eObjectState_TiedUp_CanMove &&
		pHeroPet->GetState() != eObjectState_Immortal  )
	{
		CAction act;
		act.InitMoveAction(pPos);
		pHeroPet->SetNextAction(&act);
		pHeroPet->DisableAutoAttack();
		return;
		
	}
	
	if(IsMoving(pHeroPet))
		CalcPositionEx(pHeroPet,gCurTime);
	
	//////////////////////////////////////////////////////////////////////////
	// 50cm 타일의 중앙에 서게 한다.
	pPos->x = (int(pPos->x / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	pPos->y = 0;
	pPos->z = (int(pPos->z / TILECOLLISON_DETAIL) + 0.5f) * TILECOLLISON_DETAIL;
	//////////////////////////////////////////////////////////////////////////

	MOVE_ONETARGETPOS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_PET_MOVE;
	msg.dwObjectID = gHeroID;
	msg.dwMoverID = pHeroPet->GetID();
	msg.SetStartPos( &pHeroPet->m_MoveInfo.CurPosition );
	msg.SetTargetPos( pPos );

	NETWORK->Send(&msg,msg.GetSize());

	StartMoveEx(pHeroPet,gCurTime,pPos);
}

/////////////////////////////////////////////////////////////////////////////////////
/// 06. 08. 2차 보스 - 이영준
/// 기존 SetLookatPos 함수 마지막 인자에 추가 회전각을 추가했다.
void CMoveManager::SetLookatPos(CObject* pObject, VECTOR3* pPos,float TurnDuration, DWORD CurTime, int AddDeg)
{
	if( 0 == pObject )
	{
		return;
	}
	else if( eObjectKind_Player == pObject->GetObjectKind() )
	{
		CPlayer* const player = ( CPlayer* )pObject;
		CObject* const vehicleObject = OBJECTMGR->GetObject( player->GetVehicleID() );

		if( vehicleObject &&
			0 < player->GetVehicleSeatPos() )
		{
			pObject = vehicleObject;
		}
	}

	// 100422 ONS 자기자신이 타겟인 스킬 사용시 특정방향으로 바뀌는 버그 수정.
	pPos->y = 0;
	pObject->m_MoveInfo.CurPosition.y = 0;
	VECTOR3 PositionGap = *pPos - pObject->m_MoveInfo.CurPosition;
	float Distance = VECTOR3Length(&PositionGap);
	if(Distance == 0)
		return;
	VECTOR3 TDirection = PositionGap / Distance;
	/// 추가 회전각을 더해준다
	float angle = RADTODEG(VECTORTORAD(TDirection)) + AddDeg;
	SetAngle(pObject,angle,TurnDuration);
}
/////////////////////////////////////////////////////////////////////////////////////

void CMoveManager::SetAngle(CObject* pObject,float AngleDeg,float TurnDuration)
{
	if(TurnDuration == 0)
	{
		pObject->m_RotateInfo.Angle.SetAngleDeg(AngleDeg);
		pObject->m_RotateInfo.bRotating = FALSE;
		pObject->SetAngle(pObject->m_RotateInfo.Angle.ToRad());
	}
	else
	{
		pObject->m_RotateInfo.Angle.SetAngleDeg(AngleDeg);
		pObject->m_RotateInfo.EstimatedRotateTime = TurnDuration;
		pObject->m_RotateInfo.Rotate_StartTime = gCurTime;
		pObject->m_RotateInfo.bRotating = TRUE;
	}
}

void CMoveManager::SetHeroActionMove(CActionTarget* pTarget,CAction* pAction)
{
	VECTOR3* pTargetPos = pTarget->GetTargetPosition();
	if(pTargetPos == NULL)
		return;
	if( SetHeroTarget(pTargetPos,FALSE) == FALSE )
		return;

	CHero* pHero = OBJECTMGR->GetHero();
	pHero->SetMovingAction(pAction);
	
	pHero->SetCurComboNum(0);

//---KES 따라가기
	pHero->SetFollowPlayer( 0 );
//---------------
}

void CMoveManager::SetHeroPetActionMove(CActionTarget* pTarget,CAction* pAction)
{
	VECTOR3* pTargetPos = pTarget->GetTargetPosition();
	if(pTargetPos == NULL)
		return;
	SetHeroPetTarget(pTargetPos);

	CHeroPet* pHeroPet = OBJECTMGR->GetHeroPet();
	pHeroPet->SetMovingAction(pAction);
}

void CMoveManager::MoveStop(CObject* pObject,VECTOR3* pStopPos)
{
	if(pObject->m_MoveInfo.bMoving)
		EndMove(pObject);
	if(pStopPos)
	{
		pObject->m_MoveInfo.CurPosition = *pStopPos;
		pObject->SetPosition(pStopPos);
	}
	else
	{
		pObject->SetPosition(&pObject->m_MoveInfo.CurPosition);
	}

	// 090422 ShinJS --- 소유주이면서 탑승 중인 경우 탈것도 Stop시킨다.
	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* const playerObject = ( CPlayer* )pObject;
		CVehicle* const vehicleObject = ( CVehicle* )OBJECTMGR->GetObject( playerObject->GetVehicleID() );

		if( ! vehicleObject ||
			vehicleObject->GetObjectKind() != eObjectKind_Vehicle )
		{
			return;
		}

		if(const BOOL isOwnerDriver = ( vehicleObject->GetOwnerIndex() == playerObject->GetID() && vehicleObject->IsGetOn( playerObject ) ))
		{
			vehicleObject->MoveStop( pStopPos );
		}
	}
}

void CMoveManager::HeroMoveStop()
{

	CHero* pHero = OBJECTMGR->GetHero();
	//if(pHero->m_MoveInfo.bMoving == FALSE)
	//	return;
	
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.26	2007.09.13
	if (pHero->m_MoveInfo.bMoving)
	{
  		if(CSHFarmRenderObj* const renderObject = g_csFarmManager.CLI_GetTargetObj())
 		{
			// ..농장 팻말을 클릭해서 팻말 가까이 가서 멈춘 상태라면 농장 구입 창 열기
			if (renderObject->GetInfo()->nKind == CSHFarmManager::RENDER_SALEBOARD_KIND)
			{
				GAMEIN->GetFarmBuyDlg()->SetActive(TRUE);
			}
			// ..집사를 클릭해서 다가가 멈춘 상태라면 집사 대화 창 열기
			// ..집사는 NPC가 아니고 농장 오브젝트로 등록되어 있기 때문에 다른 NPC와는 다르게 여기서 직접 대화 창 오픈
			else if (renderObject->GetInfo()->nKind == CSHFarmManager::RENDER_STEWARD_KIND)
			{
				VECTOR3 pos;																		
				HERO->GetPosition( &pos );															

				MOVEMGR->SetLookatPos(renderObject,&pos,0,gCurTime);									
 				renderObject->ChangeMotion(5, FALSE);

				GAMEIN->GetNpcScriptDialog()->OpenDialog( 999, renderObject->GetInfo()->nKind );
			}
			// 080411 KTH ..가축 슬롯을 클릭 했을 경우
			else if( renderObject->GetInfo()->nKind >= CSHFarmManager::RENDER_ANIMAL_KIND &&
					 renderObject->GetInfo()->nKind < CSHFarmManager::RENDER_ANIMAL_KIND + CSHFarmManager::RENDER_ANIMAL_KIND_NUM )
			{
				VECTOR3 pos;
				HERO->GetPosition( &pos );

				MOVEMGR->SetLookatPos(renderObject, &pos, 0, gCurTime);
				renderObject->ChangeMotion(5, FALSE);

				GAMEIN->GetAnimalDialog()->SetActive(TRUE);
				GAMEIN->GetAnimalDialog()->SetAnimal(
					g_csFarmManager.CLI_GetAnimalInfo(CSHFarmZone::FARM_ZONE(g_csFarmManager.CLI_GetTargetObjID()->nFarmZone), WORD(g_csFarmManager.CLI_GetTargetObjID()->nFarmID), WORD(g_csFarmManager.CLI_GetTargetObjID()->nAnimalCageID), WORD(g_csFarmManager.CLI_GetTargetObjID()->nAnimalID)));
			}
		}
	}
	// E 농장시스템 추가 added by hseos 2007.08.26	2007.09.13

	CalcPositionEx(pHero,gCurTime);
	
	MoveStop(pHero,&pHero->m_MoveInfo.CurPosition);
	// 상태 변경
	if(pHero->GetState() == eObjectState_Die)
		return;

	if( !OBJECTSTATEMGR->StartObjectState(pHero, eObjectState_None) )
		return;	

	// 090428 --- 탈것에 탑승중이고 소유주가 아닌경우 MP_MOVE_STOP 보내지 않는다
	if( pHero->IsGetOnVehicle() )
	{
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pHero->GetVehicleID() );
		if( pVehicle && 
			pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->GetOwnerIndex() != gHeroID )
		{	
			// MP_MOVE_STOP 보내지 않는다
			return;
		}
	}

	// 090907 ShinJS --- 남아있는 이동정보가 있는경우 Stop정보를 보내지 않는다
	if( HERO->NextMove() )
		return;

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_STOP;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = HEROID;
	msg.cpos.Compress(&pHero->m_MoveInfo.CurPosition);

	NETWORK->Send(&msg,sizeof(MOVE_POS));
}

void CMoveManager::HeroPetMoveStop()
{
	CHeroPet* pHeroPet = OBJECTMGR->GetHeroPet();
	CalcPositionEx( pHeroPet,gCurTime );
	
	MoveStop( pHeroPet,&pHeroPet->m_MoveInfo.CurPosition );
	// 상태 변경
	if(pHeroPet->GetState() == eObjectState_Die)
		return;

	if( !OBJECTSTATEMGR->StartObjectState( pHeroPet, eObjectState_None ) )
		return;	

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_PET_STOP;
	msg.dwObjectID = HEROID;
	msg.dwMoverID = pHeroPet->GetID();
	msg.cpos.Compress( &pHeroPet->m_MoveInfo.CurPosition );

	NETWORK->Send( &msg,sizeof( MOVE_POS ) );
}

float CMoveManager::CalcDistance(CObject* pObject1,CObject* pObject2)
{
	float tx = pObject1->m_MoveInfo.CurPosition.x - pObject2->m_MoveInfo.CurPosition.x;
	float tz = pObject1->m_MoveInfo.CurPosition.z - pObject2->m_MoveInfo.CurPosition.z;

	return sqrtf(tx*tx+tz*tz);
}


void CMoveManager::SetWalkMode(CObject* pObject)
{
	pObject->m_MoveInfo.MoveMode = eMoveMode_Walk;
	
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		pPlayer->SetBaseMotion();

		// 091212 ShinJS --- 자신 소유의 탈것을 가지고 있는 경우 탈것의 MoveMode를 변경시켜 준다
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
		if( pVehicle &&
			pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->GetOwnerIndex() == pPlayer->GetID() )
		{
			pVehicle->m_MoveInfo.MoveMode = eMoveMode_Walk;
		}
	}
}
void CMoveManager::SetRunMode(CObject* pObject)
{
	pObject->m_MoveInfo.MoveMode = eMoveMode_Run;
	
	if(pObject->GetObjectKind() == eObjectKind_Player)
	{
		CPlayer* pPlayer = (CPlayer*)pObject;
		pPlayer->SetBaseMotion();

		// 091212 ShinJS --- 자신 소유의 탈것을 가지고 있는 경우 탈것의 MoveMode를 변경시켜 준다
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
		if( pVehicle &&
			pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
			pVehicle->GetOwnerIndex() == pPlayer->GetID() )
		{
			pVehicle->m_MoveInfo.MoveMode = eMoveMode_Run;
		}
	}
}

BOOL CMoveManager::SetMonsterMoveType(CObject* pObject,WORD KGIdx)
{
	if(!pObject) return FALSE;
	if((pObject->GetObjectKind() & eObjectKind_Monster) == FALSE)
	{
		ASSERT(0);
		return FALSE;
	}
	
	pObject->m_MoveInfo.KyungGongIdx = KGIdx;
	return TRUE;
}

void CMoveManager::EffectYMove(CObject* pObject,float StartY,float EndY,DWORD Duration)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;

	pMoveInfo->bMoving = TRUE;
	pObject->m_bIsYMoving = TRUE;

	float Speed;	
	
	pMoveInfo->Move_StartTime = gCurTime;

	VECTOR3 CurPos;
	pObject->GetPosition(&CurPos);

	pMoveInfo->Move_StartPosition.x = CurPos.x;
	pMoveInfo->Move_StartPosition.z = CurPos.z;
	pMoveInfo->Move_StartPosition.y = CurPos.y + StartY;

	ASSERT( pMoveInfo->GetCurTargetPosIdx() <= MAX_CHARTARGETPOSBUF_SIZE );	//0000
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x = CurPos.x;
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z = CurPos.z;
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->y = CurPos.y + EndY;
	//taiyoe
	/*
	pMoveInfo->TargetPosition.x = CurPos.x;
	pMoveInfo->TargetPosition.z = CurPos.z;
	pMoveInfo->TargetPosition.y = CurPos.y + EndY;
	*/

	float Distance = EndY - StartY;
	if(Duration == 0)
		Duration = 1;
	float fTime = Duration * 0.001f;
	Speed = Distance / fTime;
	pMoveInfo->Move_Direction.x = 0;
	pMoveInfo->Move_Direction.y = Speed;
	pMoveInfo->Move_Direction.z = 0;
	pMoveInfo->Move_EstimateMoveTime = fTime;
	
	pObject->GetEngineObject()->ApplyHeightField(FALSE);
}

void CMoveManager::EffectMove(CObject* pObject,VECTOR3* TargetPos,DWORD Duration)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	if(pObject->m_bIsYMoving)
	{
		EndYMove(pObject);
	}

	VECTOR3 repos;
	if( MAP->CollisonCheck(&pMoveInfo->CurPosition,TargetPos,&repos,pObject) == TRUE )
		*TargetPos = repos;



	pMoveInfo->bMoving = TRUE;
	pMoveInfo->m_bEffectMoving = TRUE;

	float Speed;	
	
	pMoveInfo->Move_StartTime = gCurTime;

	pMoveInfo->SetCurTargetPosIdx(0);
	pMoveInfo->SetMaxTargetPosIdx(1);

	pMoveInfo->Move_StartPosition.x = pMoveInfo->CurPosition.x;
	pMoveInfo->Move_StartPosition.z = pMoveInfo->CurPosition.z;
	pMoveInfo->Move_StartPosition.y = 0;

	ASSERT( pMoveInfo->GetCurTargetPosIdx() <= MAX_CHARTARGETPOSBUF_SIZE );	//0000
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x = TargetPos->x;
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z = TargetPos->z;
	pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->y = 0;
/*
	pMoveInfo->TargetPosition.x = TargetPos->x;
	pMoveInfo->TargetPosition.z = TargetPos->z;
	pMoveInfo->TargetPosition.y = 0;
*/
	

	float dx = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x - pMoveInfo->Move_StartPosition.x;
	float dz = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z - pMoveInfo->Move_StartPosition.z;
	float Distance = sqrtf(dx*dx+dz*dz);
	if(Distance <= 0.01)
	{
		EndMove(pObject);
		return;
	}
	if(Duration == 0)
		Duration = 1;
	float fTime = Duration * 0.001f;
	Speed = Distance / fTime;
	float ttt = Speed / Distance;
	pMoveInfo->Move_Direction.x = dx*ttt;
	pMoveInfo->Move_Direction.y = 0;
	pMoveInfo->Move_Direction.z = dz*ttt;
	pMoveInfo->Move_EstimateMoveTime = fTime;
}

void CMoveManager::HeroEffectMove(CObject* pObject,VECTOR3* TargetPos,DWORD Duration,CObject* pOperator)
{
	if( pObject->GetObjectKind() == eObjectKind_SkillObject ||
		pObject->GetObjectKind() == eObjectKind_Item )
		return;

	if(pObject != HERO && pOperator != HERO)
	{
		EffectMove(pObject,TargetPos,Duration);
		return;
	}

	if(pObject == HERO)
	{
		HeroMoveStop();
	}
	
	pObject->m_MoveInfo.SetCurTargetPosIdx(0);
	pObject->m_MoveInfo.SetMaxTargetPosIdx(1);
	EffectMove(pObject,TargetPos,Duration);

	if(pObject->IsDied() == FALSE)
	{
		MOVE_POS msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_EFFECTMOVE;
		msg.dwObjectID = HEROID;
		msg.dwMoverID = pObject->GetID();
		msg.cpos.Compress(TargetPos);
		NETWORK->Send(&msg,sizeof(msg));	

		// 071204 LUJ 인터페이스 토글 상태에서 처리할 작업 처리
		MACROMGR->CheckToggleState( pObject );
	}
}

void CMoveManager::EffectMoveInterpolation(CObject* pObject,VECTOR3* TargetPos)
{
	if(pObject->IsDied())
		return;

	
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	DWORD dwTime = (DWORD)(pMoveInfo->Move_EstimateMoveTime * 1000);
	DWORD ElapsedTime = gCurTime - pMoveInfo->Move_StartTime;
	if(ElapsedTime >= dwTime)
	{
		if(pObject->m_MoveInfo.bMoving == FALSE)
			SetPosition(pObject,TargetPos);
	}
	else
	{
		DWORD RemainTime = dwTime - ElapsedTime;
		EffectMove(pObject,TargetPos,RemainTime);
	}
	
}

void CMoveManager::ToggleHeroMoveMode()
{
	VECTOR3 TargetPos;
	BOOL bTarget = FALSE;
	
	if(IsMoving(HERO))
	{
		bTarget = TRUE;
		TargetPos = *HERO->m_MoveInfo.GetTargetPosition(HERO->m_MoveInfo.GetCurTargetPosIdx());
		MoveStop( HERO, 0 );
	}

	MSGBASE msg;
	msg.Category = MP_MOVE;
	msg.dwObjectID = HEROID;
	if(HERO->m_MoveInfo.MoveMode == eMoveMode_Walk)
	{
		msg.Protocol = MP_MOVE_RUNMODE;
	}
	else
	{
		msg.Protocol = MP_MOVE_WALKMODE;
	}
	NETWORK->Send(&msg,sizeof(msg));

	if(HERO->m_MoveInfo.MoveMode == eMoveMode_Walk)
		SetRunMode(HERO);
	else
		SetWalkMode(HERO);

	if(bTarget)
	{
		SetHeroTarget(&TargetPos,FALSE);
	}
}

BOOL CMoveManager::IsMoving(CObject* pObject) const
{
	return pObject->m_MoveInfo.bMoving;
}


void CMoveManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_MOVE_TARGET:							Move_Target( pMsg ) ;							break;
	case MP_MOVE_ONETARGET:							Move_OneTarget( pMsg ) ;						break;
	case MP_MOVE_STOP:								Move_Stop( pMsg ) ;								break;
	case MP_MOVE_WALKMODE:							Move_WalkMode( pMsg ) ;							break;	
	case MP_MOVE_RUNMODE:							Move_RunMode( pMsg ) ;							break;
	case MP_MOVE_MONSTERMOVE_NOTIFY:				Move_MonsterMove_Notify( pMsg ) ;				break;
	case MP_MOVE_EFFECTMOVE:
		{
			/*
			MOVE_POS* pmsg = (MOVE_POS*)pMsg;
			CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);
			if(pObject == NULL)
			{
				return;
			}
			VECTOR3 pos;
			pmsg->cpos.Decompress(&pos);
			EffectMoveInterpolation(pObject,&pos);
			*/
			//ASSERT(0);
		}
		break;
	case MP_MOVE_CORRECTION:						Move_Correction( pMsg ) ;						break;
	case MP_MOVE_WARP:								Move_Warp( pMsg ) ;								break;
	case MP_MOVE_PET_MOVE:							Move_OneTarget( pMsg ) ;						break;
	case MP_MOVE_PET_STOP:							Move_Stop( pMsg ) ;								break;
	// 090316 LUJ, 탈것이 쫓아오는 처리
	case MP_MOVE_VEHICLE_BEGIN_ACK:					VEHICLEMGR->Follow( pMsg ) ;					break;	
	// 080930 LYW --- MoveManager : 페이드 무브 추가.
	case MP_FADEMOVE_ACK :							Move_FadeInOut( pMsg ) ;						break ;
		// 081031 LUJ, 소환 여부에 대해 응답 메시지를 보낸다
	case MP_MOVE_RECALL_ASK_SYN:
		{
			const MSG_RECALL_SYN*	message = ( MSG_RECALL_SYN* )pMsg;
			const TCHAR*			mapName	= GetMapName( message->mRecallPlayer.mMapType );

			CMoveManager::TemporaryRecallData temporaryData = { 0 };
			temporaryData.mKey			= message->mKey;
			temporaryData.mChannelIndex	= message->mChannelIndex;
			SetTemporaryRecall( temporaryData );

			WINDOWMGR->MsgBox(
				MBI_RECALL_ASK,
				MBT_YESNO,
				CHATMGR->GetChatMsg( 1771 ),
				message->mRecallPlayer.mName,
				message->mTargetPlayer.mName,
				mapName ? mapName : _T( "?" ) );
			break;
		}
	case MP_MOVE_RECALL_ASK_NACK:
		{
			const MSG_RECALL_SYN* message = ( MSG_RECALL_SYN* )pMsg;

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1772 ),
				message->mTargetPlayer.mName );
			break;
		}
	case MP_MOVE_RECALL_ACK:
		{
			const MSG_RECALL_SYN* message = ( MSG_RECALL_SYN* )pMsg;

			CHATMGR->AddMsg(
				CTC_SYSMSG,
				CHATMGR->GetChatMsg( 1773 ),
				message->mTargetPlayer.mName );
			break;
		}
	case MP_MOVE_RECALL_NACK:
		{
			const MSG_RECALL_NACK*	message				= ( MSG_RECALL_NACK* )pMsg;
			const TCHAR*			recallPlayerName	= ( _tcslen( message->mRecallPlayerName ) ? message->mRecallPlayerName : _T( "?" ) );

			switch( message->mType )
			{
			case MSG_RECALL_NACK::TypePlayerIsNotExist:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1784));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsNoMember:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(578));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerLocateOnBlockedMap:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1790));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerHasNoSkill:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1770));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsInCoolTimeForSkill:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1776));
#ifdef _GMTOOL_
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						_T( "%s is in cool time for skill" ),
						recallPlayerName );
#endif
					break;
				}
			case MSG_RECALL_NACK::TypePlayerFailedUsingSkill:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1776));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerCannotRecallOneself:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1777));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerDied:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1778));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsDealing:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1779));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsExchanging:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1780));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsGuestInStall:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1781));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsOwnerInStall:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1782));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsInPlayerKillMode:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1783));
					break;
				}
			case MSG_RECALL_NACK::TypePlayerIsOnShowdown:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1788));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsNotExist:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1784));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetLocateOnBlockedMap:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1785));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsInPlayerKillMode:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1783));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsLooting:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1786));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetDoAutoNote:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1787));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsDealing:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1779));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsExchanging:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1780));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetDied:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1778));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsGuestInStall:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1781));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsOwnerInStall:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1782));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsOnShowdown:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1788));
					break;
				}
			case MSG_RECALL_NACK::TypeTargetIsOnBattle:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1817));
					break;
				}
			case MSG_RECALL_NACK::TypeSkillIsNotExist:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1770));
					break;
				}
			case MSG_RECALL_NACK::TypeTimeOver:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1791));
					break;
				}
			case MSG_RECALL_NACK::TypeSkillNeedsMoreFamilyPoint:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1750));
					break;
				}
			case MSG_RECALL_NACK::TypeSkillNeedsMoreGuildScore:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1789));
					break;
				}
			default:
				{
					CHATMGR->AddMsg(
						CTC_SYSMSG,
						CHATMGR->GetChatMsg(1750));
					break;
				}
			}

			break;
		}
		// 100612 ShinJS --- 이동연출
	case MP_MOVE_KNOCKBACK_ACK:
		{
			MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
			CObject* pOperator = OBJECTMGR->GetObject( pmsg->dwData1 );
			CObject* pTarget = OBJECTMGR->GetObject( pmsg->dwData2 );
			if( !pOperator || !pTarget )
				break;

			if( pTarget->GetID() == HEROID )
				HERO->ClearWayPoint();

			VECTOR3 targetPos={0}, operatorPos={0};
			targetPos.x = (float)pmsg->dwData3;
			targetPos.z = (float)pmsg->dwData4;
			const DWORD dwDuration = pmsg->dwData5;

			pOperator->GetPosition( &operatorPos );			
			SetLookatPos( pTarget, &operatorPos, 0, gCurTime );

			EffectMove( pTarget, &targetPos, dwDuration );
			pTarget->SetShockTime( dwDuration );
			break;
		}
	}
}

void CMoveManager::RefreshHeroMove()
{
	if(IsMoving(HERO) == FALSE)
		return;

	VECTOR3 TargetPos = *HERO->m_MoveInfo.GetTargetPosition(HERO->m_MoveInfo.GetCurTargetPosIdx());
	SetHeroTarget(&TargetPos,FALSE);
}


void CMoveManager::Move_Target( void* pMsg )
{
	MOVE_TARGETPOS* pmsg = (MOVE_TARGETPOS*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);
	if(pObject)
	{
		CalcPositionEx(pObject,gCurTime);
		pmsg->GetTargetInfo(&pObject->m_MoveInfo);
		pObject->m_MoveInfo.AddedMoveSpeed = pmsg->AddedMoveSpeed;
		VECTOR3 spos;
		pmsg->spos.Decompress(&spos);
		StartMoveEx(pObject, &spos, gCurTime);
	}
	else
	{
#ifdef _GMTOOL_
		//쓸데없는 메세지를 보내나 확인해보자.
		++GMTOOLMGR->m_nNullMove;
#endif				
	}

	// 071204 LUJ 인터페이스 토글 상태에서 처리할 작업 처리
	MACROMGR->CheckToggleState( pObject );
}


void CMoveManager::Move_OneTarget( void* pMsg )
{
	MOVE_ONETARGETPOS_FROMSERVER* pmsg = (MOVE_ONETARGETPOS_FROMSERVER*)pMsg;

	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwObjectID);

	if( ! pObject )
	{
#ifdef _GMTOOL_
		//쓸데없는 메세지를 보내나 확인해보자.
		++GMTOOLMGR->m_nNullMove;
#endif
		return;
	}

	pObject->m_MoveInfo.AddedMoveSpeed = pmsg->AddedMoveSpeed;
	CalcPositionEx(pObject,gCurTime);
	pmsg->GetTargetInfo(&pObject->m_MoveInfo);
	VECTOR3 spos;
	pmsg->GetStartPos(&spos);
	StartMoveEx(pObject, &spos, gCurTime);

	// 090316 LUJ, 소유주이면서 탑승 중인 경우 탈것도 이동시킨다.
	if( pObject->GetObjectKind() == eObjectKind_Player )
	{
		CPlayer* const playerObject = ( CPlayer* )pObject;
		CVehicle* const vehicleObject = ( CVehicle* )OBJECTMGR->GetObject( playerObject->GetVehicleID() );

		if( ! vehicleObject ||
			vehicleObject->GetObjectKind() != eObjectKind_Vehicle )
		{
			return;
		}

		if(const BOOL isOwnerDriver = ( vehicleObject->GetOwnerIndex() == playerObject->GetID() && vehicleObject->IsGetOn( playerObject ) ))
		{
			vehicleObject->Move_OneTarget( pmsg );
		}
	}
}


void CMoveManager::Move_Stop( void* pMsg )
{
	const MOVE_POS* const pmsg = ( MOVE_POS* )pMsg;

	CObject* const pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);

	if( 0 == pObject )
	{
		return;
	}

	VECTOR3 pos = { 0 };
	pmsg->cpos.Decompress( &pos );
	CMoveManager::MoveStop( pObject, &pos );
}

// 080930 LYW --- MoveManager : 페이드 무브 추가.
void CMoveManager::Move_FadeInOut( void* pMsg )
{
	// 원본 메시지 변환.
	MOVE_POS* pmsg = NULL ;
	pmsg = (MOVE_POS*)pMsg ;

	if( !pmsg ) return ;


	// 페이드 인/아웃 다이얼로그를 호출한다.
	CFadeDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetFadeDlg() ;

	if( !pDlg ) return ;

	pDlg->FadeIn() ;


	// 위치를 설정해 준다.
	VECTOR3 pos ;
	pmsg->cpos.Decompress(&pos) ;

	BASEMOVE_INFO info ;
	info.bMoving = FALSE ;
	info.KyungGongIdx = 0 ;
	info.MoveMode = eMoveMode_Run ;
	info.CurPosition = pos ;
	MOVEMGR->InitMove(HERO,&info) ;
}


void CMoveManager::Move_WalkMode( void* pMsg )
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( pPlayer )
		SetWalkMode(pPlayer);

	// 090428 ShinJS --- 자신 소유의 탈것을 가지고 있는 경우 탈것의 MoveMode를 변경시켜 준다
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
	if( pVehicle &&
		pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
		pVehicle->GetOwnerIndex() == pPlayer->GetID() )
	{
		pVehicle->m_MoveInfo.MoveMode = eMoveMode_Walk;
	}

	// 091110 ONS 펫 애니메이션 추가 : Walk
	if(HEROPET)
	{
		HEROPET->m_MoveInfo.MoveMode = eMoveMode_Walk;
	}

	PICONMGR->SetMoveState(pmsg->dwObjectID, eMoveMode_Walk);
}


void CMoveManager::Move_RunMode( void* pMsg )
{
	MSGBASE* pmsg = (MSGBASE*)pMsg;
	CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	if( pPlayer )
		SetRunMode(pPlayer);

	// 090428 ShinJS --- 자신 소유의 탈것을 가지고 있는 경우 탈것의 MoveMode를 변경시켜 준다
	CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( pPlayer->GetVehicleID() );
	if( pVehicle &&
		pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
		pVehicle->GetOwnerIndex() == pPlayer->GetID() )
	{
		pVehicle->m_MoveInfo.MoveMode = eMoveMode_Run;
	}

	// 091110 ONS 펫 애니메이션 추가 : Run
	if(HEROPET)
	{
		HEROPET->m_MoveInfo.MoveMode = eMoveMode_Run;
	}

	PICONMGR->SetMoveState(pmsg->dwObjectID, eMoveMode_Run);
}


void CMoveManager::Move_MonsterMove_Notify( void* pMsg )
{
	MSG_WORD* pmsg = (MSG_WORD*)pMsg;
	CMonster* pMob = (CMonster*)OBJECTMGR->GetObject(pmsg->dwObjectID);
	SetMonsterMoveType(pMob, pmsg->wData);
}

void CMoveManager::Move_Correction( void* pMsg )
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);
	if(pObject == NULL)
	{
		return;
	}

	VECTOR3 pos;
	pmsg->cpos.Decompress(&pos);

	if(IsMoving(pObject))
		MoveStop(pObject,&pos);
	else
		SetPosition(pObject,&pos);

	// 090908 ShinJS --- HERO가 이동실패한 경우 WayPoint를 Clear 한다
	if( HERO &&	pObject->GetID() == HEROID )
	{
		HERO->ClearWayPoint();
	}
}


void CMoveManager::Move_Warp( void* pMsg )
{
	MOVE_POS* pmsg = (MOVE_POS*)pMsg;
	CObject* pObject = OBJECTMGR->GetObject(pmsg->dwMoverID);
	if(pObject == NULL)
		return;

	VECTOR3 pos;
	pmsg->cpos.Decompress(&pos);
	MoveStop( pObject, &pos );

	// 090316 LUJ, 탈것인 경우 승객 위치도 동기화시켜주어야 한다
	if( pObject->GetObjectKind() == eObjectKind_Vehicle )
	{
		CVehicle* const vehicleObject = ( CVehicle* )pObject;
		const VehicleScript& vehicleScript = GAMERESRCMNGR->GetVehicleScript( vehicleObject->GetMonsterKind() );

		for( DWORD seatIndex = 1; seatIndex <= vehicleScript.mSeatSize; ++seatIndex )
		{
			CObject* const passengerObject = OBJECTMGR->GetObject( vehicleObject->GetRiderID( seatIndex ) );

			if( passengerObject )
			{
				InitMove(
					passengerObject,
					vehicleObject->GetBaseMoveInfo(),
					vehicleObject->GetAngleDeg() );
			}
		}
	}
}

// 081031 LUJ, 리콜을 승인하기 전에 정보를 임시 보관한다.
void CMoveManager::SetTemporaryRecall( const TemporaryRecallData& data )
{
	mTemporaryRecallData = data;
}
