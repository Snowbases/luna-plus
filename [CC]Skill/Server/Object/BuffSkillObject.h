#pragma once
#include "skillobject.h"

class cBuffSkillInfo;
class CTargetList;
class cDelay;
class CEvent;

class cBuffSkillObject :
	public cSkillObject
{
	/// 지속시간
	cDelay*		mpDelay;
	cDelay*		mpInterval;
	cDelay*		mpUpdate;
	cDelay*		mpConsumption;
	const cBuffSkillInfo* const mSkillInfo;
	BOOL		mDelayCheck;
	BOOL		mActive;
	BOOL		mDestroy;
	// 090204 LUJ, 조건에 따라 스킬 효과를 적용/해제한다
	BOOL		mIsTurnOn;
	BOOL		mIsUpdate;
	// 090204 LUJ, 값의 표현 범위를 확장
	int			mCount;
	// 090204 LUJ, 타입을 명확히 함
	eBuffSkillCountType mCountType;

protected:
	void Consumption();
	void ProcessStart();
	void ProcessState();
	void SetActive(BOOL);
	void ResetDelay();
	DWORD GetDelayTime(CObject*, const BUFF_SKILL_INFO*) const;
	DWORD GetDelayTime() const;
	virtual void Execute(const CEvent&);
	void AddAggro(CObject&);

public:
	cBuffSkillObject(const cBuffSkillInfo&);
	virtual ~cBuffSkillObject();
	virtual void Init(sSKILL_CREATE_INFO*);
	virtual SKILL_STATE	Update();
	virtual void SetEndState();
	virtual void EndState();
	virtual DWORD SetAddMsg(DWORD dwReceiverID, BOOL bLogin, MSGBASE*&);
	void RemoveBuffCount(eBuffSkillCountType, int count);
	virtual void SetMultiTarget(CTargetList*) {}
	const BUFF_SKILL_INFO& GetInfo() const;
};
