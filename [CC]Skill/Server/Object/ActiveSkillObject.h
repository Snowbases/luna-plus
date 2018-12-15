#pragma once
#include "skillobject.h"

class cBuffSkillObject;
class cActiveSkillInfo;
class cSkillUnit;
class CTargetList;
class cTarget;

class cActiveSkillObject :
	public cSkillObject
{
protected:
	friend class cSkillManager;
	/// 버프 스킬 테이블
	CYHHashTable<cBuffSkillObject>	m_BuffSkillTable;

	DWORD mCastingTick;
	DWORD mProcessTick;
	DWORD mCoolingTick;
	cSkillUnit* mpSkillUnit;
	float mAttackSpeedRate;
	const std::auto_ptr< cTarget > mTarget;
	const cActiveSkillInfo* const mSkillInfo;

protected:
	void CastingStart();
	void CastingState();
	void ProcessStart();
	void ProcessState();
	virtual void Excute();
	cSkillUnit* CreateUnit(const ACTIVE_SKILL_INFO&);
	BOOL Consume(CObject&) const;

public:
	cActiveSkillObject(const cActiveSkillInfo&);
	virtual ~cActiveSkillObject();
	virtual void Init(sSKILL_CREATE_INFO*);
	virtual SKILL_STATE	Update();
	virtual void RemoveAllBuffSkillObject();
	virtual DWORD SetAddMsg(DWORD dwReceiverID,BOOL isLogin, MSGBASE*&);
	BOOL IsCasting() const;
	cTarget& GetTarget() { return *(mTarget.get()); }
	virtual void SetMultiTarget(CTargetList*);
	const ACTIVE_SKILL_INFO& GetInfo() const;
};