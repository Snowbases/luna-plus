#pragma once
#include ".\interface\cdialog.h"					// 다이얼로그 클래스 헤더를 불러온다.

enum ENUM_SUB_QUEST_TYPE
{
	e_SQT_NORMAL = 0,
	e_SQT_HUNT,
	e_SQT_COLLECTION,
	e_SQT_COMPLETED,
} ;

struct sQUEST_INFO ;															// 퀘스트 정보를 저장할 구조체를 선언한다.

struct QuestTree ;																// 퀘스트 정보를 담는 트리 구조체를 선언한다.

class cListDialog ;																// 리스트 다이얼로그 클래스를 선언한다.
class CQuestString ;															// 퀘스트 스트링 클래스를 선언한다.
class cStatic;

class CQuestQuickViewDialog : public cDialog									// 다이얼로그 클래스를 상속받은 퀘스트 알림이 다이얼로그 클래스.
{
	cPtrList		m_QuestTreeList ;											// 퀘스트 트리를 담을 리스트.

	cListDialog*	m_pViewList ;												// 퀘스트 내용을 보여 줄 리스트 다이얼로그.

	cStatic*		m_pQuickViewTop;
	cStatic*		m_pQuickViewMiddle;
	cStatic*		m_pQuickViewBottom;

	cButton*		m_pQuickViewRemoveBtn[MAX_QUICKVIEW_QUEST];
	cButton*		m_pQuickViewRemoveAllBtn;
	WORD			m_LineHeight;
	BOOL			m_bIsUpdate;

public:
	CQuestQuickViewDialog(void);												// 생성자 함수.
	virtual ~CQuestQuickViewDialog(void);										// 소멸자 함수.

	virtual DWORD ActionEvent(CMouse * mouseInfo);							// 이벤트 처리 함수.
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;

	void Release() ;															// 해제 함수.

	void Linking() ;															// 하위 컨트롤러들을 링크하는 함수.


	cPtrList* GetViewList() { return &m_QuestTreeList ; }						// 퀘스트 트리를 담고 있는 리스트를 반환하는 함수.


	BOOL CheckSameQuest(DWORD dwQuestIdx) ;										// 퀘스트 리스트 내에 같은 퀘스트가 있는지 여부를 반환하는 함수.

	void RegistQuest(DWORD dwQuestIdx) ;										// 알림이 창에 퀘스트를 등록 시키는 함수.
	void DeleteQuest(DWORD dwQuestIdx) ;										// 알림이 창에 있는 퀘스트를 삭제 하는 함수.

	void ResetQuickViewDialog()	;												// 퀘스트 리스트를 새로고침하는 함수.

	void AddMainString(DWORD dwQuestIdx, DWORD dwSubQuestIdx) ;					// 퀘스트 제목을 추가하는 함수.
	void AddSubString(DWORD dwQuestIdx, DWORD dwSubQuestIdx, int nType) ;		// 서브 퀘스트 제목을 추가하는 함수.
	void AddRemandQuestExecuteString( DWORD dwQuestIdx );

	void GetTitleString( char* pBuf, CQuestString* pQuestString ) ;				// 퀘스트 스트링의 제목 스트링을 얻는 함수.
	void UpdateQuickView();
};



//#pragma once
//#include "d:\luna\alpha_dev\[client]luna\interface\cdialog.h"
//
//#define MAX_QUESTTREECOUNT	5
//
//struct sQUEST_INFO ;
//
//class cListDialog ;
////class cStatic ;
//
//class CQuestQuickViewDialog : public cDialog
//{
//	BYTE			m_byAlpha ;
//
//	cPtrList		m_QuestTreeList ;
//
//	//cStatic*		m_pTitle ;
//	cListDialog*	m_pViewList ;
//
//public:
//	CQuestQuickViewDialog(void);
//	virtual ~CQuestQuickViewDialog(void);
//
//	void Release() ;
//
//	void Linking() ;
//
//	virtual DWORD ActionEvent(CMouse * mouseInfo);								// 이벤트 처리 함수.
//	
//	BOOL CheckSameQuest(DWORD dwQuestIdx) ;										// 퀘스트 리스트 내에 같은 퀘스트가 있는지 여부를 반환하는 함수.
//
//	void RegistQuest(DWORD dwQuestIdx) ;										// 알림이 창에 퀘스트를 등록 시키는 함수.
//	void DeleteQuest(DWORD dwQuestIdx) ;										// 알림이 창에 있는 퀘스트를 삭제 하는 함수.
//	void EndQuest(SEND_QUEST_IDX* msg) ;										// 알림이 창에 있는 퀘스트를 종료하는 함수.
//
//	void StartSubQuest(SEND_QUEST_IDX* pmsg) ;									// 알림이 창에 등록 된 서브 퀘스트를 시작하는 함수.
//	void EndSubQuest(SEND_QUEST_IDX* msg) ;										// 알림이 창에 등록 된 서브 퀘스트를 종료하는 함수.
//	void UpdateQuest(SEND_SUBQUEST_UPDATE* msg) ;								// 알림이 창에 등록 된 퀘스트를 업데이트 하는 함수.
//
//	void ResetLoadedQuickViewDialog() ;
//	void ResetQuickViewDialog()	;												// 퀘스트 리스트를 새로고침하는 함수.
//
//	void CheckTreeState(int nSelectedLine) ;									// 트리가 열린 상태인지 닫힌 상태인지 체크해서 리스트를 갱신하는 함수.
//
//	//void MainDataLoad() ;														// 퀵 뷰에 등록 되어 있는 퀘스트 데이터를 로딩하는 함수.
//
//	cPtrList* GetViewList() { return &m_QuestTreeList ; }
//
//	void LoadQuickViewInfoFromFile(sQUEST_INFO* pInfo) ;
//};
