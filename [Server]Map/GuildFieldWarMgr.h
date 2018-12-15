/**********************************************************************

이름: GuildWarMgr
목적: 길드 간의 전쟁을 관리한다

수정: 070830 웅주: 길드전 제한 시간(1일 1번) 주석 처리
                                                                     
***********************************************************************/
#pragma once


class CGuild;
class CGuildUnion;


struct sGFWENEMY
{
	DWORD	dwEnemyGuildIdx;
	DWORD	dwGFWMoney;
};

class CGuildWar 
{
protected:
	DWORD				m_dwGuildIdx;
	CYHHashTable<sGFWENEMY>	m_EnemyGuildTable;	
	DWORD				m_dwVictoryCount;
	DWORD				m_dwDrawCount;
	DWORD				m_dwLooseCount;

public:
	CGuildWar();
	virtual ~CGuildWar();

	void	Init( DWORD dwGuildIdx )	{ m_dwGuildIdx = dwGuildIdx; }
	void	Release();
	
	void	RegistGuildWar( DWORD dwEnemyGuildIdx, DWORD dwMoney );
	void	UnregistGuildWar( DWORD dwEnemyGuildIdx );
	BOOL	IsEnemy( DWORD dwEnemyGuildIdx );
	void	AddPlayer( CPlayer* pPlayer );
	BOOL	IsGuildWar();
	void	SetGuildWarRecord( DWORD dwVictory, DWORD dwDraw, DWORD dwLoose )
	{	m_dwVictoryCount = dwVictory;	m_dwDrawCount = dwDraw;	m_dwLooseCount = dwLoose;	}
	void	VictoryGuildWar()	{ ++m_dwVictoryCount; }
	void	DrawGuildWar()		{ ++m_dwDrawCount; }
	void	LooseGuildWar()	{ ++m_dwLooseCount; }
	void	UpdateGuildWarRecord();
	DWORD	GetWarMoney( DWORD dwEnemyGuildIdx );

	void	GetEnemy( std::list< sGFWENEMY* >& );
};

//struct sGFWTIME
//{
//	DWORD		dwEnemyGuildIdx;
//	stTIME		Time;
//	DWORD		dwCount;
//};

//class CGFWCheck
//{
//protected:
//	DWORD					m_dwGuildIdx;
//	CYHHashTable<sGFWTIME>	m_GFWDenyTable;
//	CYHHashTable<sGFWTIME>	m_GFWEndTable;
//
//public:
//	CGFWCheck();
//	virtual ~CGFWCheck();
//
//	void	Init( DWORD dwGuildIdx )	{ m_dwGuildIdx = dwGuildIdx; }
//	void	Release();
//
//	int		CanGuildWar( DWORD dwEnemyGuildIdx );
//	void	RegistDenyTime( DWORD dwEnemyGuildIdx );
//	void	RegistEndTime( DWORD dwEnemyGuildIdx );
//};

#define GUILDWARMGR CGuildWarManager::GetInstance()

class CGuildWarManager  
{
protected:
	CYHHashTable<CGuildWar>	m_GuildWarTable;
	//CYHHashTable<CGFWCheck>			m_GFWCheckTable;
	
public:
	MAKESINGLETON( CGuildWarManager );

	CGuildWarManager();
	virtual ~CGuildWarManager();

	void	Init();
	void	Release();

	void	RegistGuildWarFromRecord( DWORD dwGuildIdx, DWORD dwVictory, DWORD dwDraw, DWORD dwLoose );
	void	UpdateGuildWarRecord( BOOL bDB, WORD wFlag, DWORD dwGuildIdx );
	BOOL	RegistGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2, DWORD dwMoney );
	BOOL	UnregistGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2 );
	BOOL	JudgeGuildWar( CPlayer* pPlayer, CPlayer* pAttacker );
	BOOL	IsEnemy( CPlayer* pOper, CPlayer* pTarget );
	BOOL	IsEnemyInField( CPlayer* pOper, CPlayer* pTarget );
	void	AddPlayer( CPlayer* );
	BOOL	IsGuildWar( DWORD dwGuildIdx1, DWORD dwGuildIdx2 );
	BOOL	IsGuildWar( DWORD dwGuildIdx );
	DWORD	GetWarMoney( DWORD dwGuildIdx, DWORD dwEnemyGuildIdx );
	
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );

private:
	void	Declare( void* pMsg );
	void	DeclareAccept( void* pMsg );
	void	DeclareDeny( void* pMsg );
	void	DeclareDenyNotifyToMap( void* pMsg );

	void	StartNotifyToMap( void* pMsg );
	//void	End( DWORD dwGuildIdxWinner, DWORD dwGuildIdxLoser );
	
	void	SuggestEnd( void* pMsg );
	void	SuggestEndAccept( void* pMsg );
	void	SuggestEndAcceptNotifyToMap( void* pMsg );
	void	SuggestEndDeny( void* pMsg );
	void	SuggestEndDenyNotifyToMap( void* pMsg );
	
	void	Surrender( void* pMsg );
	void	SurrenderNotifyToMap( void* pMsg );

	void	AddMoneyToMap( void* pMsg );
	void	SendDeclareNackMsg( DWORD dwReceiver, DWORD dwState );
	void	SendMoneyMsg( CPlayer* pPlayer1, CPlayer* pPlayer2, DWORD dwMoney, BYTE Protocol );
	void	SendToAgentAddMoneyMsg( DWORD dwPlayerIdx, DWORD dwMoney );

	// 071009 웅주, 길드전 시 연합 관리 코드
public:
	// 키: 길드 포인터, 값: 선포금
	typedef std::map< CGuild*, MONEYTYPE >	Alliance;

	// 동맹이 가진 적을 길드에게도 등록한다
	void AddUnionEnemyToGuild( CGuildUnion*, CGuild* );

	// 동맹이 가진 적을 길드에게서 제거한다.
	void RemoveUnionEnemyFromGuild( CGuildUnion*, CGuild* );

	void SendToAllUser( DWORD dwFlag, const char* pGuildName1, const char* pGuildName2 );

private:
	// 적길드 목록을 가져온다
	void GetEnemy( DWORD guildIndex, std::list< sGFWENEMY* >& );

	// 유틸리티 함수. 해당 길드가 동맹에 속해있으면, 동맹의 길드 전체를 Alliance에 담는다
	void AddGuildToGroup( CGuildUnion*, Alliance& );

	// 길드 그룹을 서로 적으로 등록시키고 다른 맵 서버에도 메시지를 보내 갱신하게 한다.
	void AddEnemyToFriendly( Alliance& friendlyMap, Alliance& enemyMap );
};