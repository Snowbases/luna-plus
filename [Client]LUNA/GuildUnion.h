#pragma once


#define GUILDUNION USINGTON(CGuildUnion)


class CPlayer;


class CGuildUnion  
{
public:
	CGuildUnion();
	virtual ~CGuildUnion();

	//DWORD		GetIndex()	const;
	//const char*	GetName()	const;

	const sGUILDIDXNAME*	GetData() const	{ return mData; }
	BOOL	IsMaster( CPlayer* pPlayer );
	const char*	GetNameFromListIndex( int nIdx );
	void	Clear();
	
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );
	

	//void	SendCreateGuildUnion( const char* pName );
	void	SendDestroyGuildUnion();
	//void	SendInviteGuildUnion( CPlayer* pPlayer );
	void	SendInviteAcceptDeny( BYTE Protocol );
	void	SendRemoveGuildUnion();
	//void	SendSecedeGuildUnion();

	BOOL	CheckGuildUnionCondition( int nCondition );
	//BOOL	IsEmptyPosition();
	//BOOL	IsUnion( const char* guildName ) const;
	//BOOL	IsMasterGuild( DWORD guildIndex ) const;

	DWORD	GetSize() const;


//private:
//	void	MsgGuildUnionPlayerInfo( void* pMsg );
//	void	MsgGuildUnionInfo( void* pMsg );
//	void	MsgGuildUnionCreateAck( void* pMsg );
//	void	MsgGuildUnionDestroyAck( void* pMsg );
//	void	MsgGuildUnionInvite( void* pMsg );
//	void	MsgGuildUnionInviteDeny( void* pMsg );
//	void	MsgGuildUnionAddAck( void* pMsg ); 
//	void	MsgGuildUnionRemoveAck( void* pMsg );
//	void	MsgGuildUnionSecedeAck( void* pMsg );
//	void	MsgGuildUnionMarkRegistAck( void* pMsg );
//	void	MsgGuildUnionMarkRequestAck( void* pMsg );
//
//	void	MsgGuildUnionCreateNack( void* pMsg );
//	void	MsgGuildUnionDestroyNack( void* pMsg );
//	void	MsgGuildUnionInviteNack( void* pMsg );
//	void	MsgGuildUnionRemoveNack( void* pMsg );
//	void	MsgGuildUnionSecedeNack( void* pMsg );
//	void	MsgGuildUnionMarkRegistNack( void* pMsg );


private:
	DWORD			mIndex;
	char			mName[ MAX_GUILD_NAME+1 ];
	sGUILDIDXNAME	mData[ MAX_GUILD_UNION_NUM ];

	DWORD			m_dwTempPlayerId;
	DWORD			m_dwTempGuildUnionIdx;
	DWORD			m_dwTempGuildIdx;
};


EXTERNGLOBALTON(CGuildUnion)