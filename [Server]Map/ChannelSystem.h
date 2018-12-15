#pragma once

#include "Channel.h"

#define CHANNELSYSTEM CChannelSystem::GetInstance()

class CChannelSystem  
{
	static const int	MAX_QUEST_OR_CHALLENGE_CHANNEL_NUM	=	2000;

	CChannel*	m_Channel[MAX_CHANNEL_NUM];
	char		m_ChannelName[MAX_CHANNEL_NAME+1];
	DWORD		m_dwCount;
	
public:
	CChannelSystem();
	virtual ~CChannelSystem();
	GETINSTANCE(CChannelSystem);

	void	LoadChannelInfo();
	void	CreateChannel(BYTE Kind, BYTE num);
	void	IncreasePlayerNum(DWORD ChannelID);
	void	DecreasePlayerNum(DWORD ChannelID);
	void	SendChannelInfo(MSG_DWORD* pInfo, DWORD dwConnectionIndex);
	DWORD	GetChannelID(int num);
	DWORD	GetChannelCount() const { return m_dwCount;	}
	void	SendChannelInfoToMS( DWORD dwConnectionIndex );

protected:
	CChannel	m_QMChannel[MAX_QUEST_OR_CHALLENGE_CHANNEL_NUM];
	BOOL		m_bQuestMap;
	DWORD		m_nChallengeZoneLastChannelID;
	DWORD		m_nChallengeZoneAllocatedCH[1000][2];
	DWORD		m_nChallengeZoneAgentChannelID[1000];
	DWORD		m_nChallengeZoneAllocatedCHNum;
	DWORD		m_nChallengeZonePlayerAgentChannelID;

public:
	void	InitQuestMapChannel();
	DWORD	CreateQuestMapChannel(DWORD nFirstEnterPlayerID);
	void	DestroyQuestMapChannel( DWORD dwChannel );
	BOOL	IsQuestMap()		{ return m_bQuestMap; }
	DWORD	GetChallengeZoneLastChannalID()						{ return m_nChallengeZoneLastChannelID; }
	BOOL	GetChallengeZonePartnerCH(DWORD nChallengeZoneAgentChannelID);
	void	SetChallengeZoneAgentChannelID(DWORD nChannelID)	{ m_nChallengeZonePlayerAgentChannelID = nChannelID; }
	void	Init( WORD wMapNum, WORD wChannelNum );
	DWORD	GetPlayerNumInChannel(int index);
};