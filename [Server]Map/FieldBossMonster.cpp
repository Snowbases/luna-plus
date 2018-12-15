#include "stdafx.h"
#include "fieldbossmonster.h"
#include "PackedData.h"
#include "ObjectStateManager.h"

CFieldBossMonster::CFieldBossMonster(void)
{
}

CFieldBossMonster::~CFieldBossMonster(void)
{
}
/*
void CFieldBossMonster::AddStatus(CStatus* pStatus)
{
}
*/
BOOL CFieldBossMonster::Init(EObjectKind kind,DWORD AgentNum, BASEOBJECT_INFO* pBaseObjectInfo)
{
	CMonster::Init(kind, AgentNum, pBaseObjectInfo);
	
	// 분배 시스템 초기화
	DistributeDamageInit();
	
	return TRUE;
}

void CFieldBossMonster::DoDie(CObject* pAttacker)
{
	// 필드보스 사냥시 경험치1, 골드1 분배되는 버그
	// 081223 NYJ - CDistributer::Distribute()에서 처리하므로 여기서는 경험치/골드를 주지 않는다.
	/*
	// 수련치 경험치 돈을 분배한다
	DistributePerDamage();
	*/
	// 아이템을 분배한다
	DistributeItemPerDamage();

	// 일단 클라이언트에 죽음을 연출해준다
	DWORD AttackerID;
	if(pAttacker == NULL)
		AttackerID = 0;
	else 
		AttackerID = pAttacker->GetID();

	OBJECTSTATEMGR_OBJ->StartObjectState(this,eObjectState_Die,AttackerID);
	OBJECTSTATEMGR_OBJ->EndObjectState(this,eObjectState_Die,MONSTERREMOVE_TIME+3000);
	SetTObject(0);
	RemoveAllAggro();
//-------------------

	// CFieldBossMonsterManager에 죽었다는것을 통보해준다
	FIELDBOSSMONMGR->BossDead(this);

	//CMonster::DoDie(pAttacker);	
}

void CFieldBossMonster::SetLife(DWORD Life,BOOL bSendMsg)
{
	CMonster::SetLife(Life, bSendMsg);	
	
	if(bSendMsg)
	{
		MSG_DWORD3 msg;
		msg.Category = MP_BOSSMONSTER;
		msg.Protocol = MP_FIELD_LIFE_NOTIFY;
		msg.dwData1 = GetLife();
		msg.dwData2 = GetMonsterKind();
		msg.dwData3 = GetID();
		PACKEDDATA_OBJ->QuickSend(this,&msg,sizeof(msg));
	}
}

BOOL CFieldBossMonster::IsBattle()
{
	// 평화 모드 일때 FALSE 리턴
	if( mStateParamter.stateCur < eMA_PERSUIT )
		return FALSE;

	// 전투 모드 일때 소멸시간 카운트 리셋
	m_Info.m_dwCountTime = m_Info.m_dwDistructTime;
	// 회복시간도 리셋
	m_Info.m_dwPieceTime = gCurTime;

	// TRUE 리턴
	return TRUE;
}

BOOL CFieldBossMonster::IsDistruct()
{
	BOOL rt = FALSE;
	
	// 마지막 체크 시간이 0일 경우 현재 시간으로 설정하고 FALSE 리턴
	if( m_Info.m_dwLastCheckTime != 0 )
	{
		if( ( gCurTime - m_Info.m_dwLastCheckTime )  <  m_Info.m_dwCountTime )
		{
			m_Info.m_dwCountTime -= ( gCurTime - m_Info.m_dwLastCheckTime );
		}
		else
		{
			m_Info.m_dwCountTime = 0;

			rt = TRUE;
		}
	}

	m_Info.m_dwLastCheckTime = gCurTime;

	return rt;
}

void CFieldBossMonster::Recover()	
{
	if(m_Info.m_dwPieceTime + m_Info.m_dwRecoverStartTime < gCurTime)
	{
		DWORD maxlife = GetMaxLife();
		DWORD curlife = GetLife();
		
		if(gCurTime - m_LifeRecoverTime.lastCheckTime > m_Info.m_dwRecoverDelayTime)
		{
			if(curlife < maxlife)
			{
				DWORD pluslife = (DWORD)(maxlife * m_Info.m_fLifeRate);
				SetLife(curlife + pluslife, TRUE);
				m_LifeRecoverTime.lastCheckTime = gCurTime;

				// Life를 모두 회복했으면
				if(curlife + pluslife >= maxlife)
					// 분배 시스템 초기화
					DistributeDamageInit();
			}
		}
	}	
}