// QuestManager.h: interface for the CQuestManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_)
#define AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\[CC]Quest\QuestDefines.h"															// 퀘스트정의 해더파일을 불러온다.
#include "..\[CC]Quest\QuestEvent.h"															// 퀘스트 이벤트 해더파일을 불러온다.
#include "QuestItemManager.h"																	// 퀘스트 아이템 매니져 해더 파일을 불러온다.
#include "MiniMapIcon.h"
//#include "NewbieGuide.h"																		// 초보자 가이드 헤더 파일을 불러온다.

enum ENUM_QUEST_TYPE
{
	e_QT_ERROR_TYPE = -1,
	e_QT_MULTY_HUNT = 0,
	e_QT_MEET_OTHER,
} ;


class CQuestInfo;																				// 퀘스트 정보 클래스를 선언한다.
class CSubQuestInfo;																			// 서브 퀘스트 정보 클래스를 선언한다.
class CQuestString;																				// 퀘스트 스트링 클래스를 선언한다.
class CQuest;																					// 퀘스트 클래스를 선언한다.
class CQuestUpdater;																			// 퀘스트 업데이터 클래스를 선언한다.
//class CNewbieGuide;																				// 초보자 가이드 클래스를 선언한다.
class CNpc;																						// NPC클래스를 선언한다.
class CQuestDialog ;
class CQuestQuickViewDialog ;

struct sQUEST_INFO ;

struct stNPCIDNAME																				// 퀘스트 창에서 필요한 npc 정보 구조체.
{
	char name[64] ;																				// npc 이름을 담을 스트링.
	WORD wId ;																					// npc 아이디를 담을 변수.

	stNPCIDNAME()
	{
		memset(name, 0, sizeof(name)) ;
		wId = 0 ;
	}
} ;

#define QUESTMGR	USINGTON(CQuestManager)														// 퀘스트 매니져를 글로벌로 선언.

#define MAX_NPCARRAY	32

#ifdef _TESTCLIENT_
	#define MAX_NPCIDNAME	2000
#else
	#define MAX_NPCIDNAME	500
#endif

class CQuestManager																				// 퀘스트 매니져 클래스.
{
protected:
	CYHHashTable<CQuestInfo>	m_QuestInfoTable;												// 퀘스트 정보를 담는 테이블.	// script information
	CYHHashTable<CQuestString>	m_QuestStringTable;												// 퀘스트 스트링을 담는 테이블.
	CYHHashTable<CQuest>		m_QuestTable;													// 퀘스트를 담는 테이블.		// quest information
	CYHHashTable<QUESTNPCINFO>	m_QuestNpcTable;												// 퀘스트 NPC를 담는 테이블.	// quest npc

	typedef std::set< DWORD >	NpcHasQuestList;
	std::map< DWORD, NpcHasQuestList >	m_NpcHasQuestList;										// NPC가 소유한 퀘스트Index
	
	CQuestUpdater*				m_QuestUpdater;													// 퀘스트 업데이터.
	CQuestItemManager			m_QuestItemMGR;													// 퀘스트 아이템 매니져.

	CNpc*						m_pNpcArray[MAX_NPCARRAY];										// NCP 정보.					// npc information
	DWORD						m_dwMaxNpc;														// 최대 NPC 인덱스.	

	//CNewbieGuide				m_NewbieGuide;													// 초보자 가이드.				// newbieguide
	
	DWORD						m_dwLastMsg;													// 마지막 메시지.

	stNPCIDNAME					m_stNPCIDNAME[MAX_NPCIDNAME] ;									// 퀘스트 창에서 필요한 NPC 정보 구조체 배열 100개.

	DWORD						m_QuickViewQuestID[MAX_QUICKVIEW_QUEST] ;						// 퀵 뷰로 등록 되어 있는 퀘스트 아이디 배열.

	CQuestString*				m_TestString ;

	CQuestDialog*				m_QuestDialog ;
	CQuestQuickViewDialog*		m_QuickView ;

	// 100405 ShinJS --- 시간제한 퀘스트
	std::set< DWORD > m_setDateLimitQuest;
public:
	CQuestManager();																			// 생성자 함수.
	virtual ~CQuestManager();																	// 소멸자 함수.

	void	Release();																			// 해제 함수.
	// script
	CQuestInfo*		GetQuestInfo( DWORD dwQuestIdx )	{ return m_QuestInfoTable.GetData( dwQuestIdx ); }		// 퀘스트 정보를 리턴하는 함수.
	CQuestString*	GetQuestString( DWORD dwQuestIdx )	{ return m_QuestStringTable.GetData( dwQuestIdx );	}	// 퀘스트 스트링 정보를 리턴하는 함수.
	CQuest*			GetQuest( DWORD dwQuestIdx )		{ return m_QuestTable.GetData( dwQuestIdx ); }			// 퀘스트를 리턴하는 함수.

	CYHHashTable<CQuest>*	GetQuestTable()				{ return &m_QuestTable; }								// 퀘스트를 담고 있는 퀘스트 테이블을 리턴하는 함수.

	char*			GetQuestTitle( DWORD dwQuestIdx );															// 퀘스트 타이틀을 리턴하는 함수.
	CQuestString* GetQuestString(DWORD dwMainQuestIdx, DWORD dwSubQuestIdx)	;									// 퀘스트 번호와 서브 퀘 번호로 퀘스트 스트링을 반환하는 함수.

	BOOL	LoadQuestScript( char* strFileName );																// 퀘스트 스크립트를 로드하는 함수.
	BOOL	LoadQuestString( char* strFileName );																// 퀘스트 스트링을 로드하는 함수.

	//void	LoadNewbieGuide( char* strFileName );																// 초보자 가이드를 로딩하는 함수.

	void	LoadQuestNpc( char* strFileName );																	// 퀘스트 npc를 로딩하는 함수.

	void	CreateAllQuestFromScript();																			// script로 부터 모든 퀘스트를 생성하는 함수.

	// quest information clear
	void	InitializeQuest();																					// 퀘스트를 초기화 하는 함수.

	// quest set
	void	SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t regisTime, BYTE bCheckType, DWORD dwCheckTime );	// 메인 퀘스트를 세팅하는 함수.
	void	SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime );										// 서브 퀘스트를 세팅하는 함수.

	void	DeleteQuest( DWORD dwQuestIdx, BOOL bCheckTime = FALSE );																	// 퀘스트를 삭제하는 함수.
	void	StartQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime );									// 퀘스트를 시작하는 함수.
	void	EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime );									// 퀘스트를 종료하는 함수.
	void	StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime );				// 서브 퀘스트를 시작하는 함수.
	void	EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime );				// 서브 퀘스트를 종료하는 함수.
	void	UpdateSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime );			// 서브 퀘스트 데이터를 업데이트 하는 함수.

	void	QuestItemAdd( DWORD dwItemIdx, DWORD dwCount );							// 퀘스트 아이템을 추가하는 함수.
	void	QuestItemDelete( DWORD dwItemIdx );										// 퀘스트 아이템을 삭제하는 함수.
	void	QuestItemUpdate( DWORD dwType, DWORD dwItemIdx, DWORD dwCount );		// 퀘스트 아이템을 업데이트 하는 함수.
	QUEST_ITEM_INFO* GetQuestItem( DWORD dwItemIdx );
	
	// NpcScript
	BOOL	IsNpcRelationQuest( DWORD dwNpcIdx );									// npc 관련 퀘스트 인지 체크하는 함수.
	DWORD	GetNpcScriptPage( DWORD dwNpcIdx );										// npc 스크립트 페이지를 리턴하는 함수.
	void	SendNpcTalkEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx, DWORD dwRequitalIndex = 0, DWORD dwRequitalCount = 0 );				// npc 대화 이벤트를 보내는 함수.
	void	SendNpcTalkEventForEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx );		// 이벤트용 npc 대화 이벤트를 보내는 함수.
	// NpcMark
	void	SetNpcData( CNpc* pNpc );												// npc 데이터를 세팅하는 함수.
	void	ProcessNpcMark();														// npc 마크 프로세스 함수.
	// newbieguide
	//void	CheckGameEvent( GAMEEVENT* pGE )	{ m_NewbieGuide.CheckEvent( pGE ); }	// 게임 이벤트를 체크하는 함수.
	// quest npc
	void	AddQuestNpc( CSubQuestInfo* pSubQuestInfo );								// 퀘스트 npc를 추가하는 함수.
	
	BOOL	IsQuestStarted( DWORD dwQuestIdx );											// 퀘스트가 시작되었는지 체크하는 함수.

	BOOL IsQuestComplete(DWORD dwQuestIdx);												// 퀘스트가 완료되었는지 체크하는 함수.

	// Network
	void	NetworkMsgParse( BYTE Protocol, void* pMsg );								// 네트워케 메시지 파싱 함수.
	void	QuestAbandon();																// 퀘스트 포기 처리 함수.
	void	QuestAbandon( DWORD dwQuestIdx );											// 퀘스트 포기 처리 함수.
	void	QuestTimeLimit( DWORD dwQuestIdx, DWORD dwTime );							// 퀘스트 시간 제한 처리 함수.
	void	RegistCheckTime( DWORD dwQuestIdx, BYTE bCheckType, DWORD dwCheckTime );	// 퀘스트 시간 등록 함수.
	void	UnRegistCheckTime( DWORD dwQuestIdx );										// 퀘스트 시간 해제 함수.

	void	QuestEventAck( DWORD dwQuestIdx );											// 퀘스트 이벤트 수락 함수.
	void	QuestEventNack( DWORD dwQuestIdx, DWORD dwError );							// 퀘스트 이벤트 실해 함수.
	// 070205 LYW --- Add functions to process network msg.
public :
	void Quest_Maindata_Load( void* pMsg ) ;											// 메인 데이터 로딩 함수.
	void Quest_Subdata_Load( void* pMsg ) ;												// 서브 데이터 로딩 함수.
	void Quest_Item_Load( void* pMsg ) ;												// 퀘스트 아이템 로딩 함수.
	void Quest_Start_Ack( void* pMsg ) ;												// 퀘스트 스타트 허가 함수.
	void SubQuest_Start_Ack( void* pMsg ) ;												// 서브 퀘스트 스타트 허가 함수.
	void Quest_End_Ack( void* pMsg ) ;													// 퀘스트 종료 허가 함수.
	void SubQuest_End_Ack( void* pMsg ) ;												// 서브 퀘스트 종료 허가 함수.
	void SubQuest_Update( void* pMsg ) ;												// 서브 퀘스트 업데이트 함수.
	void QuestItem_Give( void* pMsg ) ;													// 퀘스트 아이템 반환 함수.
	void QuestItem_Take( void* pMsg ) ;													// 퀘스트 아이템 획득 함수.
	void QuestItem_Update( void* pMsg ) ;												// 퀘스트 아이템 업데이트 함수.
	void Quest_Delete_Ack( void* pMsg ) ;												// 퀘스트 삭제 허가 함수.
	void Quest_TakeItem_Ack( void* pMsg ) ;												// 퀘스트 아이템 획득 허가 함수.
	void Quest_Takemoney_Ack( void* pMsg ) ;											// 퀘스트 머니 획득 허가 함수.
	void Quest_TakeExp_Ack( void* pMsg ) ;												// 퀘스트 경험치 획득 허가 함수.
	void Quest_TakesExp_Ack( void* pMsg ) ;												// 퀘스트 sp 포인트 획득 허가 함수.
	void Quest_GiveItem_Ack( void* pMsg ) ;												// 퀘스트 아이템 반환 허가 함수.
	// 071026 LYW --- QuestManager : Add function to decrease item.
	void Quest_GiveItems_Ack( void* pMsg ) ;											// 퀘스트 스택 아이템 개수 변환 함수.
	void Quest_GiveMoney_Ack( void* pMsg ) ;											// 퀘스트 머니 반환 허가 함수.
	void Quest_Time_Limit( void* pMsg ) ;												// 퀘스트 시간 제한 함수.
	void Quest_Regist_Checktime( void* pMsg ) ;											// 퀘스트 시간 체크 등록 함수.
	void Quest_Unregist_Checktime( void* pMsg ) ;										// 퀘스트 시간 체크 해제 함수.
	void QuestEvent_NpcTalk_Ack( void* pMsg ) ;											// 퀘스트 이벤트 npc 대화 허가 함수.
	void QuestEvent_NpcTalk_Nack( void* pMsg ) ;										// 퀘스트 이벤트 npc 대화 실패 함수.
	void Quest_Execute_Error( void* pMsg ) ;											// 퀘스트 에러 실행 함수.
	void Quest_Error_Ext( void* pMsg ) ;												// 퀘스트 기타 에러 처리 함수.
	void Quest_Error_FullQuest( void* pMsg ) ;											// 퀘스트 목록이 가득차서 더이상 진행할 수 없다는 에러 메시지 처리.

	// 071028 LYW --- CQuestManager : Add function to process give item error.
	void Quest_GiveItem_Error( void* pMsg ) ;											// 퀘스트 아이템 반납에 실패한 에러 처리를 하는 함수.

	// 070329 LYW --- QuestManager : Add function to make string.
	void CompositString( char* inputStr, char* str, ... ) ;								// 퀘스트 출력 문장 조합 함수.

	void AddMapTree(int nMapNum) ;														// 맵 트리를 추가하는 함수.
	void DeleteMapTree(int nMapNum) ;													// 맵 트리를 삭제하는 함수.

	void AddQuestTitle(int nMapNum, DWORD dwQuestIdx) ;									// 퀘스트 다이얼로그에 퀘스트 제목을 추가하는 함수.
	void DeleteQuestTitle(int nMapNum, DWORD dwQuestIdx) ;								// 퀘스트 리스트에서 퀘스트 제목을 삭제하는 함수.

	void SetNpcIDName(WORD wIdx, stNPCIDNAME* pInfo) ;									// npc 이름과 아이디를 세팅하는 함수.

	stNPCIDNAME* GetNpcIdName(DWORD npcIdx) ;

	QUESTNPCINFO* GetQusetNpcInfo(DWORD npcIdx);
	void LoadQuickViewInfoFromFile(DWORD* pIdx);
	int CheckQuestType(DWORD wQuestIdx, DWORD wSubQuestIdx);
	void SetQuestDialog(CQuestDialog* pQuestDialog) { m_QuestDialog = pQuestDialog ; }
	void SetQuickViewDialog(CQuestQuickViewDialog* pQuickView) { m_QuickView = pQuickView ; }

	// 100410 ShinJS --- 시간 설정 기능 추가
	void NotifyChangedDateTime( const DWORD dwQuestIdx );								// 유효 시간 상태가 변경된 경우를 알려준다.
	void Process();
	void InitDateTime();																// 현재 서버시간으로 시간설정 퀘스트들을 초기화한다.
	BOOL IsValidDateTime( DWORD dwQuestIdx );

	// 100517 ShinJS --- 퀘스트 요구사항을 완료했는지 검사(일반 아이템 등)
	BOOL IsCompletedDemandQuestExecute( DWORD dwQuestIdx );

	// 100414 ONS 퀘스트 마지막트리거 정보를 가져온다.
	cPtrList*	GetLastSubQuestTrigger( DWORD dwQuestIdx );

	eQUEST_MARK GetQuestMarkState( DWORD dwUniqueNpcIdx, DWORD dwNpcObjectIdx );

	void AddNpcHasQuest( DWORD dwUniqueNpcIdx, DWORD dwQuestIdx );
};

EXTERNGLOBALTON(CQuestManager);

#endif // !defined(AFX_QUESTMANAGER_H__AC4F30D2_DA87_4A14_8DF7_C2AFCBF24758__INCLUDED_)
