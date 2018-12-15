/*
080616 LUJ, 함정 오브젝트. 스킬로 발동된다. 주기적으로 자신을 생성시킨 스킬이 존재하는지 체크하고
			없을 경우 자동 소멸된다
*/
#pragma once
#include "monster.h"

class CTrap : public CMonster
{
private:
	DWORD mTriggeredTime;

public:
	CTrap(void);
	virtual ~CTrap(void);
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*&);
	virtual BOOL DoAttack(DWORD attackNum);
	virtual void InitMonster(MONSTER_TOTALINFO*);
};
