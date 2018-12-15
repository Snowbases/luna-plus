#pragma once

class cSkillTree : public CYHHashTable< SKILL_BASE >
{
	CPlayer* mpPlayer;

public:
	cSkillTree(void);
	virtual ~cSkillTree(void);
	void Init(CPlayer*);
	void Release();
	void Update(const SKILL_BASE&);
};