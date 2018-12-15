#pragma once
#include "skillinfo.h"

class CHero;
class CActionTarget;
class CHeroPet;

class cActiveSkillInfo :
	public cSkillInfo
{
protected:
	ACTIVE_SKILL_INFO mInfo;
	DWORD mCoolTime;

public:
	cActiveSkillInfo(void);
	virtual ~cActiveSkillInfo(void);
	void Parse(LPCTSTR);
	ACTIVE_SKILL_INFO* GetSkillInfo() { return &mInfo; }
	BOOL IsExcutableSkillState(CHero* pHero,int SkillLevel,CActionTarget* pTarget);
	BOOL IsInRange(CObject&, CActionTarget&) const;
	BOOL IsExcutablePetSkillState(CHeroPet* pHeroPet,int SkillLevel,CActionTarget* pTarget);
	BOOL IsValidTarget(CHero* pHero,CActionTarget* pTarget);

	// 071228 LYW --- ActiveSkillInfo : 버프 인덱스를 반환하는 함수 추가.
	DWORD GetBuffSkillIdx(int index) ;

	void SetCooling();
	void ResetCoolTime() { mCoolTime = 0; }
	virtual DWORD GetCoolTime() const { return mCoolTime; }
	virtual SKILLKIND GetKind() const { return mInfo.Kind; }
	virtual DWORD GetIndex() const { return mInfo.Index; }
	virtual LPCTSTR GetName() const { return mInfo.Name; }
	virtual DWORD GetTooltip() const { return mInfo.Tooltip; }
	virtual int GetImage() const { return mInfo.Image; }
	virtual LEVELTYPE GetLevel() const { return mInfo.Level; }
};
