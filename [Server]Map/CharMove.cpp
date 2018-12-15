// CharMove.cpp: implementation of the CCharMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CharMove.h"

#include "PackedData.h"
#include "Player.h"
#include "TileManager.h"
#include "GridSystem.h"
#include "VehicleManager.h"

CCharMove::CCharMove()
{

}

CCharMove::~CCharMove()
{

}

void CCharMove::InitMove(CObject* pObject,VECTOR3* pPos)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	pMoveInfo->CurPosition = *pPos;
	pMoveInfo->bMoving = FALSE;
	pMoveInfo->m_bMoveCalculateTime = FALSE;
	pMoveInfo->MoveMode = eMoveMode_Run;
	pMoveInfo->KyungGongIdx = 0;
	pMoveInfo->AddedMoveSpeed = 0.0f;
	/*어빌리티 삭제 - 이영준 pMoveInfo->AbilityKyungGongLevel = 0;*/
	pMoveInfo->Move_EstimateMoveTime = 0;
	pMoveInfo->InitTargetPosition();
	
	g_pServerSystem->m_pGridSystem->AddObject(pObject,pPos);
}

void CCharMove::ReleaseMove(CObject* pObject)
{
	g_pServerSystem->m_pGridSystem->RemoveObject(pObject);
}

VECTOR3* CCharMove::GetPosition(CObject* pObject)
{
	return CalcPositionEx(pObject,gCurTime);
}

void CCharMove::SetPosition(CObject* pObject,VECTOR3* pPos)
{
	if(!pObject)
		return;

	//ASSERTVALID_POSITION(*pPos);
	if(CheckValidPosition(*pPos) == FALSE)
	{
		return;
	}

	if(pObject->m_MoveInfo.bMoving)
		pObject->m_MoveInfo.bMoving = FALSE;

	pObject->m_MoveInfo.CurPosition = *pPos;
	pObject->m_bNeededToCalcGrid = TRUE;
}

//BOOL CCharMove::CheckVaildStop(CPlayer* pPlayer,DWORD CurTime,VECTOR3* EndPos)
BOOL CCharMove::CheckVaildStop(CObject* pObject,DWORD CurTime,VECTOR3* EndPos)
{	
	CalcPositionEx(pObject,CurTime);
	if(CalcDistanceXZ(&pObject->m_MoveInfo.CurPosition,EndPos) > MOVE_LIMIT_ERRORDISTANCE)
	{
		CorrectPlayerPosToServer(pObject);
		return FALSE;
	}
	return TRUE;
}

// 현재 위치를 계산해서 EndPos와 차이가 많이 나면 조치-_-;를 취한다.
// 이동이 끝났음을 표시한다.
void CCharMove::EndMove(CObject* pObject,DWORD CurTime,VECTOR3* EndPos)
{	
	//ASSERTVALID_POSITION(*EndPos);
	if(CheckValidPosition(*EndPos) == FALSE)
	{
		pObject->m_MoveInfo.bMoving = FALSE;
		pObject->m_bNeededToCalcGrid = TRUE;
		pObject->m_MoveInfo.InitTargetPosition();
		return;
	}
	
	pObject->m_MoveInfo.CurPosition = *EndPos;
	pObject->m_MoveInfo.bMoving = FALSE;
	pObject->m_MoveInfo.InitTargetPosition();
	pObject->m_bNeededToCalcGrid = TRUE;
}

void CCharMove::StartMoveEx(CObject* pObject,DWORD CurTime,VECTOR3* StartPos,VECTOR3* pTargetPos)
{	
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());

	CalcPositionEx(pObject,CurTime);	//---KES 그 전단계의 이동을 끝마치려는 의도인듯.

	//KES HACK CHECK
	if( pObject->GetObjectKind() == eObjectKind_Player )
	if( CalcDistanceXZ(&pMoveInfo->CurPosition,StartPos) > 10*100 ) //5m 자주 걸렸네 그럼 10m
	{
		((CPlayer*)pObject)->SpeedHackCheck();		
	}
	
	pMoveInfo->Move_StartTime = CurTime;

	//YH2DO
	float Speed = pObject->GetMoveSpeed();
	if(Speed == 0)
	{
		EndMove(pObject,CurTime,StartPos);
		return;
	}

	pMoveInfo->CurPosition.x = StartPos->x;
	pMoveInfo->CurPosition.z = StartPos->z;
	pMoveInfo->Move_StartPosition.x = StartPos->x;
	pMoveInfo->Move_StartPosition.z = StartPos->z;

	float Distance = CalcDistanceXZ(pTargetPos,&pMoveInfo->Move_StartPosition);
	pMoveInfo->Move_Direction.x = pTargetPos->x - pMoveInfo->Move_StartPosition.x;
	pMoveInfo->Move_Direction.z = pTargetPos->z - pMoveInfo->Move_StartPosition.z;

	if(Distance)
	{
		float tt = (Speed / Distance);
		pMoveInfo->Move_Direction.x = pMoveInfo->Move_Direction.x * tt;
		pMoveInfo->Move_Direction.z = pMoveInfo->Move_Direction.z * tt;
	}
	else
	{
		pMoveInfo->Move_Direction.x = 0;
		pMoveInfo->Move_Direction.z = 0;
	}
	
	pObject->m_bNeededToCalcGrid = TRUE;

	pMoveInfo->Move_EstimateMoveTime = Distance / Speed;
	pMoveInfo->m_bMoveCalculateTime = g_pServerSystem->m_pGridSystem->GetGridChangeTime(
		pObject,StartPos,pTargetPos,Speed,pMoveInfo->m_dwMoveCalculateTime);
	pMoveInfo->bMoving = TRUE;
	ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());
}

VECTOR3* CCharMove::CalcPositionEx(CObject* pObject,DWORD CurTime)
{
	//지금 object가 정지상태인데도  pMoveInfo->bMovingd= TRUE인 경우가 있다.
	MOVE_INFO * pMoveInfo = &pObject->m_MoveInfo;
	//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
	if(pMoveInfo->bMoving)
	{
		ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());
		if(CurTime == pMoveInfo->Move_LastCalcTime)	//---KES 시간이 흐르지 않았다면
		{
			if(pMoveInfo->CurPosition.x < 0)			pMoveInfo->CurPosition.x = 0;
			else if(pMoveInfo->CurPosition.x > 51200)	pMoveInfo->CurPosition.x = 51200;

			if(pMoveInfo->CurPosition.z < 0)			pMoveInfo->CurPosition.z = 0;
			else if(pMoveInfo->CurPosition.z > 51200)	pMoveInfo->CurPosition.z = 51200;
			
			return &pMoveInfo->CurPosition;
		}
		
		pMoveInfo->Move_LastCalcTime = CurTime;

		float fMoveTime = ((CurTime - pMoveInfo->Move_StartTime)*0.001f); //---KES 한프레임에 한번만 계산되어야 한다. =_= 진짜 꼬여있구만.

		if(pMoveInfo->Move_EstimateMoveTime < fMoveTime)	//시간이 지났다.
		{
			//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
			// 다음 타겟으로 변경
			if(pMoveInfo->GetCurTargetPosIdx() < pMoveInfo->GetMaxTargetPosIdx()- 1)
			{				
				//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
				pMoveInfo->CurPosition.x = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x;
				pMoveInfo->CurPosition.z = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z;
				pMoveInfo->SetCurTargetPosIdx(pMoveInfo->GetCurTargetPosIdx()+1);


				//ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());
				StartMoveEx(pObject, CurTime, &pMoveInfo->CurPosition, pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx()));
				//---KES 이곳이 이상하다.. 시간이 오래 흘러 2칸 이상을 건너야 할 필요가 있다면?
			}
			else
			{
				//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
				ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());
				pMoveInfo->CurPosition.x = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->x;
				pMoveInfo->CurPosition.z = pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx())->z;
				pMoveInfo->bMoving = FALSE;
			}
			//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
		}
		else
		{
			//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
			pMoveInfo->CurPosition.x = pMoveInfo->Move_StartPosition.x + pMoveInfo->Move_Direction.x * fMoveTime;
			pMoveInfo->CurPosition.z = pMoveInfo->Move_StartPosition.z + pMoveInfo->Move_Direction.z * fMoveTime;
			//ASSERTVALID_POSITION(pMoveInfo->CurPosition);
		}
	}
	
	if(pMoveInfo->CurPosition.x < 0)	pMoveInfo->CurPosition.x = 0;
	else if(pMoveInfo->CurPosition.x > 51100)	pMoveInfo->CurPosition.x = 51100;

	if(pMoveInfo->CurPosition.z < 0)	pMoveInfo->CurPosition.z = 0;	
	else if(pMoveInfo->CurPosition.z > 51100)	pMoveInfo->CurPosition.z = 51100;

	return &pMoveInfo->CurPosition;
}

void CCharMove::MoveProcess(CObject* pObject)
{
	MOVE_INFO* pMoveInfo = &pObject->m_MoveInfo;
	if(pMoveInfo->bMoving == FALSE)
		goto MoveProcessEnd;

	if(pMoveInfo->m_bMoveCalculateTime)
	{
		if(pMoveInfo->m_dwMoveCalculateTime <= gTickTime)
		{
			ASSERT(pMoveInfo->GetCurTargetPosIdx()< pMoveInfo->GetMaxTargetPosIdx());
			//StartMoveEx(pObject,gCurTime,&pMoveInfo->CurPosition,&pMoveInfo->TargetPositions[pMoveInfo->CurTargetPositionIdx]);
			StartMoveEx(pObject,gCurTime,&pMoveInfo->CurPosition, pMoveInfo->GetTargetPosition(pMoveInfo->GetCurTargetPosIdx()));
		}
		else
		{
			pMoveInfo->m_dwMoveCalculateTime -= gTickTime;
		}
			
	}

MoveProcessEnd:
	if(pObject->m_bNeededToCalcGrid == TRUE)
	{		
		if(pMoveInfo->CurPosition.x < 0)	pMoveInfo->CurPosition.x = 0;
		if(pMoveInfo->CurPosition.z < 0)	pMoveInfo->CurPosition.z = 0;
		if(pMoveInfo->CurPosition.x > 51100)	pMoveInfo->CurPosition.x = 51100;
		if(pMoveInfo->CurPosition.z > 51100)	pMoveInfo->CurPosition.z = 51100;
		
		g_pServerSystem->m_pGridSystem->ChangeGrid(pObject,&pMoveInfo->CurPosition);
		pObject->m_bNeededToCalcGrid = FALSE;
	}
}

BOOL CCharMove::IsMoving(CObject* pObject)
{
	if(pObject->m_MoveInfo.bMoving)
		CalcPositionEx(pObject,gCurTime);

	return pObject->m_MoveInfo.bMoving;
}

void CCharMove::SetWalkMode(CObject* pObject)
{
	pObject->m_MoveInfo.MoveMode = eMoveMode_Walk;
}
void CCharMove::SetRunMode(CObject* pObject)
{
	pObject->m_MoveInfo.MoveMode = eMoveMode_Run;
}

void CCharMove::CorrectPlayerPosToServer(CObject* pObject)
{
	// 090316 LUJ, 소환한 탈것이 있을 경우, 같이 보정해주어야 한다
	if( TRUE == VEHICLEMGR->Recall( pObject->GetID() ) )
	{
		return;
	}

	VECTOR3* pCurPos = CCharMove::GetPosition(pObject);
	EndMove(pObject,gCurTime,pCurPos);

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_CORRECTION;
	msg.dwMoverID = pObject->GetID();
	msg.cpos.Compress(pCurPos);

	PACKEDDATA_OBJ->QuickSend(pObject,&msg,sizeof(msg));

}

DWORD CCharMove::GetMoveEstimateTime(CObject* pObject)
{
	DWORD dwEstimateTime = 0;
	float Speed = pObject->GetMoveSpeed();
	CalcPositionEx(pObject, gCurTime);
	VECTOR3 * pVPos = pObject->m_MoveInfo.GetTargetPosition(0);
	if( 0 == pObject->m_MoveInfo.GetMaxTargetPosIdx()) return 0;
	ASSERT(pObject->m_MoveInfo.GetCurTargetPosIdx() < pObject->m_MoveInfo.GetMaxTargetPosIdx());
	for(int i = pObject->m_MoveInfo.GetCurTargetPosIdx() ; i < pObject->m_MoveInfo.GetMaxTargetPosIdx()-1; ++i)
	{
		float Distance = CalcDistanceXZ(&pVPos[i],&pVPos[i+1]);
		dwEstimateTime += (DWORD)(Distance/Speed*1000);
	}

	dwEstimateTime += (DWORD)(pObject->m_MoveInfo.Move_EstimateMoveTime*1000);

	return dwEstimateTime;
}

void CCharMove::Warp(CObject* pObject, VECTOR3* vPos )
{
	SetPosition(pObject, vPos);

	MOVE_POS msg;
	msg.Category = MP_MOVE;
	msg.Protocol = MP_MOVE_WARP;
	msg.dwMoverID = pObject->GetID();
	msg.cpos.Compress(vPos);

	PACKEDDATA_OBJ->QuickSend(pObject,&msg,sizeof(msg));
}
