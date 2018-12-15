// BattleTeam_GTournament.h: interface for the CBattleTeam_GTournament class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLETEAM_GTOURNAMENT_H__9D04FBCB_2F3A_47E9_9101_2AF4FC0B29CF__INCLUDED_)
#define AFX_BATTLETEAM_GTOURNAMENT_H__9D04FBCB_2F3A_47E9_9101_2AF4FC0B29CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BattleTeam.h"



class CGuild;




class CBattleTeam_GTournament : public CBattleTeam
{
	// Guild Info
	DWORD			m_GuildIdx;
	DWORD			m_GuildLevel;
	DWORD			m_MemberCount;
	DWORD			m_DeadMemberCount;
	DWORD			m_DeadMemberLeavecount;

	DWORD			m_KillPoint;


	virtual void DoAddTeamMember(CObject* pObject);
	virtual void DoDeleteTeamMember(CObject* pObject);

public:
	CBattleTeam_GTournament();
	virtual ~CBattleTeam_GTournament();

	//
	void Init( GTTEAM_MEMBERINFO* pInfo, DWORD Team );
	void SetGuildInfo( DWORD Idx, DWORD Level )		{m_GuildIdx = Idx; m_GuildLevel=Level;}
	DWORD GetGuildIdx()					{	return m_GuildIdx;	}
	DWORD GetGuildLevel()				{	return m_GuildLevel;}

	virtual BOOL IsAddableTeamMember(CObject* pObject);

	BOOL IsMemberAllDead();
	int GetRemainMember();
	void IncreaseMemberCount()		{	++m_MemberCount;	}
	void IncreaseDieCount()			{	++m_DeadMemberCount;	}
	DWORD GetDieCount()				{	return m_DeadMemberCount;	}
	DWORD GetTotalLevel();
	DOUBLE GetTotalLevelExp();

	void AddKillPoint(WORD point)	{	m_KillPoint += point;	}
	DWORD GetKillPoint()			{	return m_KillPoint;	}

	void AllPlayerOut();
	void AliveTeamMember();
	
	void ReturnToMap();
	void ReturnToMap( DWORD playerID );
	char* GetGuildName();

	void SetResult( BOOL bWin );
};

#endif // !defined(AFX_BATTLETEAM_GTOURNAMENT_H__9D04FBCB_2F3A_47E9_9101_2AF4FC0B29CF__INCLUDED_)
