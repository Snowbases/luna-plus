#pragma once
#include "baseitem.h"

class cActiveSkillInfo;

class cSkillBase :
	public CBaseItem
{
	SKILL_BASE m_SkillBaseInfo;
	cImage m_pHighLayerImage;
	cImage m_DelayImage;
	cActiveSkillInfo* mSkillInfo;

public:
	cSkillBase(void);
	virtual ~cSkillBase(void);

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * lowImage, cImage * highImage, LONG ID=0);
	virtual void Render();
	virtual DWORD GetDBIdx() const { return m_SkillBaseInfo.dwDBIdx; }
	virtual DWORD GetSkillIdx() const;
	virtual void SetLevel(BYTE level) { m_SkillBaseInfo.Level = level; }
	virtual LEVELTYPE GetLevel() const { return m_SkillBaseInfo.Level; }
	void SetSkillBase(const SKILL_BASE&);
	SKILL_BASE* GetSkillBase() { return &m_SkillBaseInfo; }
};
