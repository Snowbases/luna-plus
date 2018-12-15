#pragma once
#include "skillobject.h"

class cBuffSkillInfo;

class cBuffSkillObject :
	public cSkillObject
{
	/// 상태효과 리스트
	cPtrList	mStatusList;
	BOOL		mIsTurnOn;

public:
	cBuffSkillObject(cBuffSkillInfo*);
	virtual ~cBuffSkillObject();
	virtual void Init(const SKILLOBJECT_INFO&);
	virtual void Update();
	virtual void OnReceiveSkillResult(MSG_SKILL_RESULT*);
	void ResetDelay();
	void SetCount(int count);

private:
	void ReleaseSkillObject();
	BOOL IsHiddenCount();

	// 080318 LUJ,	스킬이 설정된 시각 보관.	
private:
	DWORD			mBeginTime;
public:
	inline DWORD GetBeginTime() const { return mBeginTime; }
	// 0901222 NYJ - 스킬이 시작된 시간 설정
	void SetBeginTime(DWORD dwTime) {mBeginTime = dwTime;}
};
