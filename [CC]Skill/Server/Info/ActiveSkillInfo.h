#pragma once
#include "SkillInfo.h"

class cActiveSkillInfo : public cSkillInfo
{
protected:
	ACTIVE_SKILL_INFO mInfo;

public:
	cActiveSkillInfo(void);
	virtual ~cActiveSkillInfo(void);
	void Parse(LPCTSTR);
	BOOL IsExcutableSkillState(CObject&, const ICONBASE&) const;
	virtual cSkillObject* GetSkillObject() const;
	BOOL IsInRange(CObject&, VECTOR3& targetPosition, float TargetRadius) const;
	const ACTIVE_SKILL_INFO& GetInfo() const { return mInfo; }
	virtual SKILLKIND GetKind() const { return mInfo.Kind; }
	virtual DWORD GetIndex() const { return mInfo.Index; }
	virtual LEVELTYPE GetLevel() const { return mInfo.Level; }
};