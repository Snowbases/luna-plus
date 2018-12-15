#pragma once


#define GUILDWARMGR CGuildWarManager::GetInstance()


class CPlayer;

class CGuildWarManager  
{
public:
	static CGuildWarManager* GetInstance();

	CGuildWarManager();
	virtual ~CGuildWarManager();

	void	Init();
	void	Release();

	BOOL	IsEnemy		( CPlayer* );
	BOOL	IsEnemy		( const char* guildName ) const;
	void	AddEnemy	( const MSG_GUILD_LIST::Data& );
	void	RemoveEnemy	( DWORD guildIndex );
	DWORD	GetEnemySize() const;
	const MSG_GUILD_LIST::Data*	GetEnemy				( DWORD guildIndex )	const;
	const MSG_GUILD_LIST::Data*	GetEnemyFromListIndex	( int index );

	BOOL	IsValid( DWORD dwKind );
	void	GetVictory( char* temp );

	void	NetworkMsgParse( BYTE Protocol, void* pMsg );

	void	SendDeclare( char* pName, char* pMoney );
	void	SendDeclareAcceptDeny( BYTE Protocol );
	void	SendSurrender();
	void	SendSuggest();
	void	SendSuggestAcceptDeny( BYTE Protocol );

	const MSG_GUILD_LIST::Data*	GetUnionMasterEnemy( DWORD unionIndex ) const;

	// testinferface
	//void	TestSuggestAcceptDeny();


public:
	typedef std::list< MSG_GUILD_LIST::Data >	EnemyList;

	const EnemyList&	GetEnemyList() const;

private:
	EnemyList					mEnemyList;

	const MSG_GUILD_LIST::Data* mSelectedGuild;
	DWORD						mSelectedEnemyGuildIndex;
	DWORD						mTempMoney;

	DWORD						mVictoryCount;
	DWORD						mDrawCount;
	DWORD						mLooseCount;
};