// QuestGroup.h: interface for the CQuestGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTGROUP_H__0A88AD15_F8B2_4FC9_AADB_26F663A4A647__INCLUDED_)
#define AFX_QUESTGROUP_H__0A88AD15_F8B2_4FC9_AADB_26F663A4A647__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]Quest\QuestDefines.h"
#include "..\[CC]Quest\QuestEvent.h"

// 071028 LYW --- QuestGroup : Define function caption.
#define QG_GSI	0			// Give_StackItem_ItemInven
#define QG_GSW	1			// Give_StackItem_WearInven
#define QG_GNI	2			// Give_NormalItem_ItemInven
#define QG_GNW	3			// Give_NormalItem_WearInven

class CQuestEvent;
class CQuestInfo;
class CQuest;
class CPlayer;

struct GQUESTEVENT
{
	DWORD			dwQuestIdx;
	CQuestEvent		QuestEvent;
	GQUESTEVENT() : dwQuestIdx(0)	{}
};

// 100414 ONS 선택보상 정보 구조체
struct REQUITALINFO
{
	DWORD dwRequitalIndex;
	DWORD dwRequitalCount;
};

class CQuestGroup  
{
protected:
	PLAYERTYPE*				m_pPlayer;
	CYHHashTable<CQuest>	m_QuestTable;
	CYHHashTable<QUESTITEM>	m_QuestItemTable;
	GQUESTEVENT				m_QuestEvent[MAX_QUESTEVENT_PLYER];
	DWORD					m_dwQuestEventNum;

	// 100405 ShinJS --- 시간제한 퀘스트 정보
	std::set<DWORD>			m_setDateLimitQuest;

	// 100414 ONS 선택보상 정보를 저장한다.
	typedef std::map<DWORD, REQUITALINFO>	RequitalIndexMap;
	RequitalIndexMap						m_RequitalIndexMap;

public :
	cPtrList				m_RandomItemList ;

	cPtrList* GetRandomItemList() { return &m_RandomItemList ; }

	void ReleaseRandomItemList() ;

public:
	CQuestGroup();
	CQuestGroup( PLAYERTYPE* pPlayer );
	virtual ~CQuestGroup();

	void	Release();
	void	Initialize( PLAYERTYPE* pPlayer );

	// 맨처음 유저가 접속했을때
	void	CreateQuest( DWORD dwQuestIdx, CQuestInfo* pQuestInfo );
	// 유저 접속 후 DB데이터 세팅 
	void	SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime );
	void	SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime );
	void	SetQuestItem( DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum );

	// event
	void	AddQuestEvent( CQuestEvent* pQuestEvent, DWORD dwQuestIdx );

	// trigger
	void	DeleteQuest( DWORD dwQuestIdx );	// giveup quest
	void	EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	ChangeSubQuestValue( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwKind );
	
	void	AddCount( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount );
	void	AddCountFromQWeapon( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwWeaponKind );
	void	AddCountFromWeapon( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwWeaponKind );
	void	AddCountFromLevelGap( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwMinLevel, DWORD dwMaxLevel );
	void	AddCountFromMonLevel( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwMinLevel, DWORD dwMaxLevel );
	
	//void	GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum );
	BOOL	GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum );
	void	TakeQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx,
						   DWORD dwItemIdx, DWORD dwItemNum, DWORD dwProbability );
	void	TakeQuestItemFromQWeapon( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwItemIdx,
									  DWORD dwItemNum, DWORD dwProbability, DWORD dwWeaponKind );
	BOOL	GiveItem( PLAYERTYPE* pPlayer, DWORD wItemIdx, DWORD dwItemNum, DWORD dwQuestIdx );
	// 071027 LYW --- QuestGroup : Add function to give stack item of item inventory.
	void Give_StackItem_ItemInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx) ;
	// 071027 LYW --- QuestGroup : Add function to give stack item of wear inventory.
	void Give_StackItem_WearInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx) ;
	// 071027 LYW --- QuestGroup : Add function to give normal item of item inventory.
	void Give_NormalItem_ItemInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx) ;
	// 071027 LYW --- QuestGroup : Add function to give normal item of wear inventory.
	void Give_NormalItem_WearInven(PLAYERTYPE* pPlayer, ITEM_TOTALINFO* pInfo, DWORD wItemIdx, int* pGiveNum, DWORD dwQuestIdx) ;
	// 071027 LYW --- QuestGroup : Add functino to write log.
	void Log_GiveItem(PLAYERTYPE* pPlayer, ITEMBASE* pInven) ;
	void	GiveMoney( PLAYERTYPE* pPlayer, DWORD dwMoney );
	BOOL	TakeItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD wItemIdx, DWORD dwItemNum, DWORD dwProbability );
	void	TakeMoney( PLAYERTYPE* pPlayer, DWORD dwMoney );
	void	TakeExp( PLAYERTYPE* pPlayer, DWORD dwExp );
	void	TakeSExp( PLAYERTYPE* pPlayer, DWORD dwSExp );	
	void	TakeMoneyPerCount( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwItemIdx, DWORD dwMoney );
	
	void	RegenMonster( DWORD dwRegenType );
	// 080104 LYW --- QuestGroup : 맵 이동 기능을 수정함.
	void MapChange( MAPTYPE destinationMap, float xPos, float zPos ) ;
	//void	MapChange( WORD wCurMap, WORD wReturnMap );

	// 070414 LYW --- QuestGroup : Active function ChangeStage.
	//void	ChangeStage( BYTE bStage );
	// 070416 LYW --- QuestGroup : Modified function ChangeStage.
	void ChangeStage( DWORD dwGrade, DWORD dwIndex ) ;
	
	void	RegistCheckTime( DWORD dwQuestIdx, DWORD dwSubQuestIdx, BYTE bType, DWORD dwDay, DWORD dwHour, DWORD dwMin );

	// process
	void	Process();
	
	// getfn
	CQuest*	GetQuest( DWORD dwQuestIdx )	{ return m_QuestTable.GetData( dwQuestIdx ); }	
	QUESTITEM* GetQuestItem(DWORD dwItemIdx) { return m_QuestItemTable.GetData(dwItemIdx) ; }

	int GetQuestCount() { return m_QuestTable.GetDataNum() ; }

	BOOL IsQuestComplete( DWORD dwQuestIdx );

	DWORD	GetSubQuestValue( DWORD dwQuestIdx, DWORD dwSubQuestIdx );

	BOOL IsValidDateTime( DWORD dwQuestIdx );

	// 100414 ONS 선택보상처리 관련함수 선언.
	void	SetQuestRequital( DWORD dwQuestIdx, DWORD dwRequitalIndex, DWORD dwRequitalCount );
	void	DeleteQuestRequital( DWORD dwQuestIdx );
	BOOL	TakeSelectItem( PLAYERTYPE* pPlayer, CQuestGroup* pQuestGroup, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	REQUITALINFO*	FindRequitalIndex( DWORD dwQuestIdx );

};

#endif // !defined(AFX_QUESTGROUP_H__0A88AD15_F8B2_4FC9_AADB_26F663A4A647__INCLUDED_)
