#include "StdAfx.h"
#include "skilltree.h"
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[cc]skill/server/info/skillinfo.h"
#include "Player.h"
#include "CharacterCalcManager.h"

CPool< SKILL_BASE > skillPool(20, 20, "SkillBase");

cSkillTree::cSkillTree(void) :
mpPlayer(0)
{
	CYHHashTable< SKILL_BASE >::Initialize(
		20);
}

cSkillTree::~cSkillTree(void)
{
	Release();
}

void cSkillTree::Init( CPlayer* pPlayer )
{
	mpPlayer = pPlayer;
}

void cSkillTree::Release()
{
	CYHHashTable< SKILL_BASE >::SetPositionHead();

	for(SKILL_BASE* skill = CYHHashTable< SKILL_BASE >::GetData();
		0 < skill;
		skill = CYHHashTable< SKILL_BASE >::GetData())
	{
		skillPool.Free(
			skill);
	}

	CYHHashTable< SKILL_BASE >::RemoveAll();
}

void cSkillTree::Update(const SKILL_BASE& updatedSkill)
{
	SKILL_BASE* skill = 0;
	SKILL_BASE* const oldSkill = CYHHashTable< SKILL_BASE >::GetData(
		updatedSkill.wSkillIdx);

	if(oldSkill)
	{
		const cSkillInfo* const skillInfo = SKILLMGR->GetSkillInfo(
			oldSkill->wSkillIdx - 1 + oldSkill->Level);

		if(skillInfo &&
			skillInfo->GetKind() == SKILLKIND_PASSIVE)
		{
			SKILLMGR->RemovePassive(
				mpPlayer,
				skillInfo->GetIndex());
			mpPlayer->PassiveSkillCheckForWeareItem();
			CHARCALCMGR->CalcCharStats(
				mpPlayer);
		}

		if(0 == updatedSkill.Level ||
			MAX_SKILL_TREE < updatedSkill.Level)
		{
			CYHHashTable< SKILL_BASE >::Remove(
				oldSkill->wSkillIdx);
			skillPool.Free(
				oldSkill);
			return;
		}

		skill = oldSkill;
	}
	else
	{
		skill = skillPool.Alloc();

		if(0 == skill)
		{
			return;
		}

		CYHHashTable< SKILL_BASE >::Add(
			skill,
			updatedSkill.wSkillIdx);
	}
	
	*skill = updatedSkill;

	LEVELTYPE skillLevel = min(
		LEVELTYPE(SKILLMGR->GetSkillSize(updatedSkill.wSkillIdx)),
		updatedSkill.Level);
	const cSkillInfo* const skillInfo = SKILLMGR->GetSkillInfo(
		updatedSkill.wSkillIdx - 1 + skillLevel);

	if(0 == skillInfo)
	{
		return;
	}
	else if(SKILLKIND_PASSIVE == skillInfo->GetKind())
	{
		SKILLMGR->AddPassive(
			mpPlayer,
			skillInfo->GetIndex());
		mpPlayer->PassiveSkillCheckForWeareItem();
		CHARCALCMGR->CalcCharStats(
			mpPlayer);
	}
}