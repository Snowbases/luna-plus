#include "stdafx.h"
#include "fieldsubmonster.h"
#include "PackedData.h"

CFieldSubMonster::CFieldSubMonster(void)
{
}

CFieldSubMonster::~CFieldSubMonster(void)
{
}

void CFieldSubMonster::DoDie(CObject* pAttacker)
{
	CMonster::DoDie(pAttacker);	

	// CFieldBossMonsterManager에 죽었다는것을 통보해준다
	FIELDBOSSMONMGR->SubDead(this);
}

void CFieldSubMonster::SetLife(DWORD Life,BOOL bSendMsg)
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

BOOL CFieldSubMonster::IsBattle()
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

BOOL CFieldSubMonster::IsDistruct()
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

void CFieldSubMonster::Recover()	
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
			}
		}
	}	
}