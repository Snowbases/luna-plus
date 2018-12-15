#pragma once


class CGuild;

class CGuildUnion  
{
protected:
	DWORD			m_dwGuildUnionIdx;
	char			m_sGuildUnionName[MAX_GUILD_NAME+1];
	sGUILDIDXNAME	m_GuildInfo[MAX_GUILD_UNION_NUM];
	DWORD			m_dwGuildUnionMarkIdx;
	
	void			SetGuildInfo( int nIndex, DWORD dwGuildIdx );
public:
	CGuildUnion();
	virtual ~CGuildUnion();

	void	LoadUnionInfo( DWORD dwUnionIdx, char* pName, DWORD dwGuild0, DWORD dwGuild1, DWORD dwGuild2,
						   DWORD dwGuild3, DWORD dwGuild4, DWORD dwGuild5, DWORD dwGuild6, DWORD dwMarkIdx );

	DWORD	GetIndex()			{ return m_dwGuildUnionIdx; }
	char*	GetName()			{ return m_sGuildUnionName; }
	DWORD	GetMarkIndex()		{ return m_dwGuildUnionMarkIdx; }
	DWORD	GetMasterGuildIdx()			{ return m_GuildInfo[0].dwGuildIdx; }
	void	GetGuildUnionInfo( SEND_GUILD_UNION_INFO* pMsg );
	void	SetMarkIndex( DWORD dwMarkIdx );

	void	CreateGuildUnion( DWORD dwGuildUnionIdx, char* pGuildUnionName, DWORD dwMasterGuildIdx );
	void	Destroy();

	BOOL	IsEmptyPosition();
	BOOL	IsMasterGuild( DWORD dwGuildIdx )	{ return m_GuildInfo[0].dwGuildIdx == dwGuildIdx; }
	BOOL	IsGuildUnion( DWORD dwGuildIdx );
	BOOL	IsGuildUnion( CGuild* pGuild );
	BOOL	IsGuildWarWithUnionGuild( DWORD dwGuildIdx );
	//int		AddGuild( DWORD dwGuildIdx );
	int		AddGuild( CGuild* pGuild );
	int		RemoveGuild( DWORD dwGuildIdx );
	int		RemoveGuild( CGuild* pGuild );	

	void	SendMsgToGuildUnionAll( MSGBASE* pMsg, int nSize );

	// SiegeWar Service Function
	void	GetGuildListInUnion( cPtrList* pList );
	CGuild*	GetMasterGuild();
	void SendAllNote(CPlayer *pPlayer, char* note);

	DWORD	GetSize() const;
};