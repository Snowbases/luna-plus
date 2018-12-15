#include "stdafx.h"
#include "BattleTeam_Showdown.h"
#include "Player.h"



CBattleTeam_Showdown::CBattleTeam_Showdown()
{
	m_dwTeamDie = 0;
}

CBattleTeam_Showdown::~CBattleTeam_Showdown()
{

}

void CBattleTeam_Showdown::SetCharacterID( DWORD id )
{
	m_CharacterID = id;
}
	
BOOL CBattleTeam_Showdown::IsAddableTeamMember(CObject* pObject)
{
	if( m_CharacterID == pObject->GetID() )
		return TRUE;

	return FALSE;
}


void CBattleTeam_Showdown::DoDeleteTeamMember(CObject* pObject)
{
	++m_dwTeamDie;

#ifdef _MAPSERVER_
	pObject->SetBattle(pObject->GetGridID(), 0);
#endif
}

/*
void CBattleTeam_Showdown::DoAddTeamMember(CObject* pObject)
{

}*/