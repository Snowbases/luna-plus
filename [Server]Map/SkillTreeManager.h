#pragma once


#define SKILLTREEMGR CSkillTreeManager::GetInstance()


class CPlayer;


class CSkillTreeManager  
{
	CSkillTreeManager();
public:
	virtual ~CSkillTreeManager();

	static CSkillTreeManager* GetInstance();
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	void UpdateSkill( SKILL_BASE*, CPlayer* pPlayer );
	void UpdateAck( CPlayer * pPlayer, SKILL_BASE* pSkillBase );
	void UpdateNack( CPlayer * pPlayer, BYTE result );

	void AddSkill( CPlayer * pPlayer, SKILL_BASE* pSkillBase );
	void AddSkillDBResult( CPlayer * pPlayer, SKILL_BASE* pSkillBase );	
};