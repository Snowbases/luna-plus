// QuestManager.h: interface for the CQuestManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTMANAGER_H__AFB737EA_8F09_4500_A7C7_F85CF054476B__INCLUDED_)
#define AFX_QUESTMANAGER_H__AFB737EA_8F09_4500_A7C7_F85CF054476B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]Quest\QuestDefines.h"
#include "..\[CC]Quest\QuestEvent.h"

class CPlayer;
class CQuest;
class CQuestInfo;
class CQuestEvent;
class CQuestString;
class CQuestUpdater;

#define QUESTMGR	USINGTON(CQuestManager)

class CPlayer;
class CQuestInfo;
class CQuestEvent;

struct MQUESTEVENT
{
	PLAYERTYPE*		pPlayer;
	DWORD			dwQuestIdx;
	CQuestEvent		QuestEvent;	
	MQUESTEVENT() : pPlayer(NULL), dwQuestIdx(0)	{}
};

class CQuestManager  
{
protected:
	CYHHashTable<CQuestInfo>	m_QuestInfoTable;	// script Á¤º¸

	MQUESTEVENT					m_QuestEvent[MAX_QUESTEVENT_MGR];
	DWORD						m_dwQuestEventCount;
	
	// RaMa ( 04.07.12 )
	CQuestUpdater*				m_QuestUpdater;
	
public:
	CQuestManager();
	virtual ~CQuestManager();
	
	void	Release();

	// script
	BOOL	LoadQuestScript();
	BOOL	LoadQuestString( char* strFileName );
	CQuestInfo*	GetQuestInfo( DWORD dwQuestIdx )	{ return m_QuestInfoTable.GetData( dwQuestIdx ); }
	// ¸ÇÃ³À½ À¯Àú°¡ Á¢¼ÓÇßÀ»¶§
	void	CreateQuestForPlayer( PLAYERTYPE* pPlayer );

	// À¯Àú Á¢¼Ó ÈÄ DBµ¥ÀÌÅÍ ¼¼ÆÃ 
	void	SetMainQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime, BYTE bCheckType, DWORD dwCheckTime );
	void	SetSubQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime );
	void	SetQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, int nItemNum );
	void	DeleteQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, BOOL bTime = FALSE, BOOL bSendMsg=TRUE );
	// 0901008 LUJ, 다시 수행할 수 있도록, 수행 여부에 관계없이 퀘스트를 삭제한다
	void	DeleteQuestForcedly(CPlayer&, DWORD questIndex, BOOL bSendMsg=TRUE);
	void	StartQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	EndQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	StartSubQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	EndSubQuest( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx );
	void	UpdateSubQuestData( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount );
	//void	GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwItemIdx, DWORD dwItemNum );
	BOOL	GiveQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum ) ;
	void	TakeQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum );
	void	UpdateQuestItem( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwItemIdx, DWORD dwItemNum );
	void	UpdateQuestCheckTime( PLAYERTYPE* pPlayer, DWORD dwQuestIdx, DWORD dwCheckType, DWORD dwCheckTime );
	
	// event
	void	AddQuestEvent( PLAYERTYPE* pPlayer, CQuestEvent* pQuestEvent, DWORD dwQuestIdx = 0 );
	void	Process();

	BOOL CheckFullQuestCount(CPlayer* pPlayer) ;
	void SendFullQuestMsg(CPlayer* pPlayer) ;

	// Network
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );		

	// 080515 LYW --- QuestManager : 퀘스트 관련 로그를 남기는 함수 추가.
	void WriteLog(char* pMsg) ;

	BOOL	IsCompletedDemandQuestExecute( DWORD dwQuestIdx ) const { return TRUE; }
};

EXTERNGLOBALTON(CQuestManager);

#endif // !defined(AFX_QUESTMANAGER_H__AFB737EA_8F09_4500_A7C7_F85CF054476B__INCLUDED_)
