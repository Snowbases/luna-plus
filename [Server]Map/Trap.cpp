#include "StdAfx.h"
#include "Trap.h"

CTrap::CTrap()
{}

CTrap::~CTrap()
{}

void CTrap::InitMonster(MONSTER_TOTALINFO* totalInfo)
{
	CMonster::InitMonster(
		totalInfo);

	mTriggeredTime = gCurTime + 2000 + rand() % 5000;
	SetDieTime(
		GetSMonsterList().Life * 1000 + gCurTime);
}

DWORD CTrap::SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*& sendMessage)
{
	// 함정은 시간에 따라 HP가 줄어드는 개념이다
	m_MonsterInfo.Life = (m_DieTime - gCurTime) / 1000;

	return CMonster::SetAddMsg(dwReceiverID, bLogin, sendMessage);
}

// 090204 LUJ, 공격 후 자살하기 위해 상속처리함
BOOL CTrap::DoAttack( DWORD attackNum )
{
	if(mTriggeredTime > gCurTime)
	{
		return FALSE;
	}
	else if(FALSE == CMonster::DoAttack( attackNum ) )
	{
		return FALSE;
	}

	// 090204 LUJ, 공격 스킬이 약간의 지연 후 실행된다. 따라서 여유 시간을
	//			두고 함정을 자살시켜야 한다.
	SetDieTime(
		gCurTime + 500);
	return TRUE;
}