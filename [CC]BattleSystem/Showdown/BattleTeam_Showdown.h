#ifndef BATTLETEAM_SHOWDOWN_H
#define BATTLETEAM_SHOWDOWN_H


#include "BattleTeam.h"


class CBattleTeam_Showdown : public CBattleTeam
{

protected:

	DWORD	m_CharacterID;
	DWORD	m_dwTeamDie;

public:

	CBattleTeam_Showdown();
	virtual ~CBattleTeam_Showdown();

	void SetCharacterID(DWORD id);
	DWORD GetCharacterID() { return m_CharacterID; }
	virtual BOOL IsAddableTeamMember(CObject* pObject);	

	virtual void DoDeleteTeamMember(CObject* pObject);
//	virtual void DoAddTeamMember(CObject* pObject);

///// TEST
	DWORD GetTeamDieNum() { return m_dwTeamDie; }
	void AddTeamDieNum() { ++m_dwTeamDie; }

};



#endif