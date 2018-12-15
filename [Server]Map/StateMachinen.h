#pragma once

class MSG_OBJECT;
enum eStateEvent;

class CStateMachinen  
{
public:
	CStateMachinen();
	virtual ~CStateMachinen();
	void SetState(CObject*, eMONSTER_ACTION);
	void Process(CObject*, eStateEvent);
	void RecvMsg(CObject* pSrcObject, CObject* pDestObject, MSG_OBJECT*);
	void SetHelperMonster(CMonster* pAsker, CMonster* pHelper, CObject* pTargeter);
	BOOL IsTargetChange(CObject* pAttacker, CMonster* pDefender);

private:
	void Update(CMonster&, eStateEvent);
	void SubProcess(CMonster&, eStateEvent);
	void DoStand(CMonster&, eStateEvent);
	void DoWalkAround(CMonster&, eStateEvent);
	void DoPursuit(CMonster&, eStateEvent);
	void DoAttack(CMonster&, eStateEvent);
	void DoRunAway(CMonster&, eStateEvent);
	void DoScriptMove(CMonster&, eStateEvent);
	void DoPause(CMonster&, eStateEvent);
	DWORD RandCurAttackKind(const BASE_MONSTER_LIST&, StateParameter&);
};

extern CStateMachinen GSTATEMACHINE;