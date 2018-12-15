#pragma once

#include "Object.h"
#include "../Effect/SkillEffect.h"

class cSkillInfo;

/// 스킬 생성 정보
struct sSKILL_CREATE_INFO
{
	DWORD skillObjectId;
	LEVELTYPE level;
	cSkillInfo*	pSkillInfo;
	DWORD operatorId;
	MAINTARGET mainTarget;
	VECTOR3	pos;
	VECTOR3	skillDir;
	DWORD remainTime;
	int	count;
	bool isNew;
	ICONBASE usedItem;

	/// 초기화 함수
	sSKILL_CREATE_INFO()
	{
		ZeroMemory(
			this,
			sizeof(*this));
		isNew = true;
	}
};

class cSkillObject	:
	public CObject
{
protected:
	cSkillInfo*	mpSkillInfo;
	sSKILL_CREATE_INFO mSkillObjectInfo;
	CYHHashTable< CObject >	mTargetTable;
	cSkillEffect mOperatorEffect;
	cSkillEffect mSkillEffect;
	cSkillEffect mTargetEffect;
	DWORD mDelayTick;

	void UpdateTarget();

public:
	cSkillObject(cSkillInfo*);
	virtual ~cSkillObject();
	virtual void Init(const SKILLOBJECT_INFO&);
	virtual void OnReceiveSkillResult(MSG_SKILL_RESULT*);
	DWORD GetSkillIdx();
	CObject* GetOperator();
	cSkillInfo* GetSkillInfo();
	sSKILL_CREATE_INFO& GetSkillObjectInfo() { return mSkillObjectInfo; }
	void SetOperatorState();
	void ResetSkillObjectId(DWORD id);
	virtual void Update();

	void Cancel();
};
