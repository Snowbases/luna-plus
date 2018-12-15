// ActionTarget.cpp: implementation of the CActionTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionTarget.h"

#include "ObjectManager.h"
#include "Hero.h"
#include "MoveManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IS_PK	(m_TargetFlag & ACTIONTARGET_FLAG_PK)

ObjectArray<CObject*> CActionTarget::m_ObjectArray;

CActionTarget::CActionTarget()
{
	Clear();

	// 080804 LUJ, 멤버 변수가 정적이므로, 초기화하지 않으면 이전 정보가 그대로 남아있다
	m_ObjectArray.Init();
}

CActionTarget::~CActionTarget()
{

}

void CActionTarget::InitActionTarget(CObject* pObject,DWORD flag)
{
	m_TargetKind = eActionTargetKind_Object;
	m_TargetID = pObject->GetID();
	m_TargetFlag = flag;
}
void CActionTarget::InitActionTarget(VECTOR3* pPos,DWORD flag)
{
	m_TargetKind = eActionTargetKind_Position;
	m_TargetPosition = *pPos;
	m_TargetFlag = flag;
}

void CActionTarget::Clear()
{
	m_TargetKind = eActionTargetKind_None;
}

void CActionTarget::CopyFrom(CActionTarget* pSrc)
{
	if(pSrc == NULL)
	{
		m_TargetKind = eActionTargetKind_Object;
		m_TargetID = 0;
	}
	else
		memcpy(this,pSrc,sizeof(CActionTarget));
}

VECTOR3* CActionTarget::GetTargetPosition()
{
	switch(m_TargetKind)
	{
	case eActionTargetKind_None:
		ASSERTMSG(0,"GetTargetPosition is Called, When TargetKind is None");
		break;
		
	case eActionTargetKind_Object:
		{
			CObject* pObject = OBJECTMGR->GetObject(m_TargetID);
			if(pObject == NULL)
			{
				return NULL;
			}
			pObject->GetPosition(&m_TargetPosition);
			return &m_TargetPosition;
		}

	case eActionTargetKind_Position:		
		return &m_TargetPosition;
	}
	return NULL;
}

void CActionTarget::SetTargetObjectsInRange(VECTOR3* pCenterPos,float Radius,WORD PARAM)
{
	OBJECTMGR->GetTargetInRange(pCenterPos,Radius,&m_ObjectArray,IS_PK,PARAM);
}
void CActionTarget::SetPositionFirstTargetObject()
{
	m_ObjectArray.SetPositionHead();
}
CObject* CActionTarget::GetNextTargetObject()
{
	return m_ObjectArray.GetNextObject();
}

float CActionTarget::GetRadius()
{
	switch(m_TargetKind)
	{
	case eActionTargetKind_None:
		ASSERTMSG(0,"GetTargetPosition is Called, When TargetKind is None");
		break;
		
	case eActionTargetKind_Object:
		{
			CObject* pObject = OBJECTMGR->GetObject(m_TargetID);
			if(pObject == NULL)
				return 0;
			return pObject->GetRadius();			
		}

	case eActionTargetKind_Position:		
		return 0;
	}
	return 0;
}

DWORD CActionTarget::GetTargetID()
{
	switch(m_TargetKind)
	{
	case eActionTargetKind_None:
		//ASSERTMSG(0,"GetTargetID is Called, When TargetKind is None");
		break;
		
	case eActionTargetKind_Object:
		{
			CObject* pObject = OBJECTMGR->GetObject(m_TargetID);
			if(pObject == NULL)
				return 0;
			return pObject->GetID();			
		}

	case eActionTargetKind_Position:		
		return 0;
	}
	return 0;	
}


void CActionTarget::GetMainTarget(MAINTARGET* pRtMainTarget)
{
	switch(m_TargetKind)
	{
	case eActionTargetKind_None:
		ASSERTMSG(0,"GetTargetID is Called, When TargetKind is None");
		break;
		
	case eActionTargetKind_Object:
		{
			pRtMainTarget->MainTargetKind = MAINTARGET::MAINTARGETKIND_OBJECTID;

			CObject* pObject = OBJECTMGR->GetObject(m_TargetID);
			if(pObject == NULL)
			{
				pRtMainTarget->dwMainTargetID = 0;
				return;
			}			
			pRtMainTarget->dwMainTargetID = pObject->GetID();
			
			VECTOR3 pos;

			pObject->GetPosition( &pos );
		}
		break;

	case eActionTargetKind_Position:
		{
			pRtMainTarget->MainTargetKind = MAINTARGET::MAINTARGETKIND_POS;
			pRtMainTarget->cpTargetPos.Compress(GetTargetPosition());
		}
		break;
	}	
}

void CActionTarget::GetTargetArray(CTargetList* pTList)
{
	pTList->Clear();
	CTargetListIterator iter(pTList);

	SetPositionFirstTargetObject();

	for(CObject* pObject = GetNextTargetObject();
		0 < pObject;
		pObject = GetNextTargetObject())
	{
		iter.AddTargetWithNoData(pObject->GetID(),SKILLRESULTKIND_NONE);
	}

	iter.Release();
}

void CActionTarget::SetOneTarget(CObject* pObject)
{
	m_ObjectArray.Init();
	if(pObject == NULL)
	{
		CObject* pTempObj = OBJECTMGR->GetObject(GetTargetID());
//		ASSERT(pTempObj)
		if(pTempObj)
			m_ObjectArray.AddObject(pTempObj);
	}
	else
		m_ObjectArray.AddObject(pObject);
}

void CActionTarget::ConvertMainTargetToPosition(CObject* pAttacker,float AttackDist)
{
	VECTOR3 pos = *GetTargetPosition();
	VECTOR3 apos;
	pAttacker->GetPosition(&apos);
	float dist = CalcDistanceXZ(&pos,&apos);
	if(dist > AttackDist)
	{
		float gap = dist - AttackDist + 50.f;	// 50은 그냥 오차가 있을지 모르니 준것
		VECTOR3 dir = apos - pos;
		Normalize(&dir,&dir);
		dir = dir * gap;
		pos = pos + dir;
	}
	InitActionTarget(&pos,m_TargetFlag);
}