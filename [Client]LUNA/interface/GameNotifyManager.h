#pragma once

#define GAMENOTIFYMGR USINGTON(CGameNotifyManager)

#define MAX_LINE_LIMIT	5																// 공지 출력 가능 최대 라인 수.

class CGameNotifyDlg ;																	// 게임 공지 다이얼로그 클래스를 선언한다.
class CItem ;

enum GAMENOTIFY_STYLE																	// 게임 공지 스타일 이넘 코드.
{
	eSTYLE_DEFAULT = 0 ,																// 기본 스타일.
	eSTYLE_QUEST,																		// 퀘스트 공지용.
} ;

enum QUEST_COMPOSIT																		// 퀘스트 공지에서 메시지 추가시 사용하는 조합 타입 이넘 코드.
{
	eQC_DEFAULT_STR = 0,																// 퀘스트 기본 스트링.
    eQC_MAINQUEST_NAME,																	// 메인 퀘스트 이름.
	eQC_SUBQUEST_NAME,																	// 서브 퀘스트 이름.
	eQC_COUNT,																			// 카운트.
	eQC_COMPLETE,																		// 완료.
	eQC_EXPERIENCE,																		// 경험치.
	eQC_MONEY,																			// 머니.
	eQC_ITEM,																			// 아이템.

	eQC_MAX,																			// 퀘스트 공지 조합 최대 값.
} ;

struct sGAMENOTIFY_MSG																	// 공지 메시지 구조체.
{
	int nNotifyType ;																	// 공지 타입.

	//int nMinAlpha ;																		// 최소 알파 값.

	/*int red ;

	int gree ;

	int blue ;*/

	//int nAlpha ;

	RECT rect ;

	int nDelayTime ;																	// 딜레이 시간.

	cPtrList itemList ;																	// 스트링 아이템 리스트.

	sGAMENOTIFY_MSG()																	// 생성자 함수.
	{
		nNotifyType = eSTYLE_DEFAULT ;													// 공지 타입을 기본 타입으로 세팅한다.

		//nMinAlpha = 255 ;																	// 최소 알파 값을 0으로 세팅한다.

		/*red = 255 ;

		gree = 255 ;

		blue = 255 ;*/

		//nAlpha = 255 ;

		memset(&rect, 0, sizeof(RECT)) ;

		nDelayTime = 170 ;																// 딜레이 타임을 3초로 세팅한다.

		itemList.RemoveAll() ;															// 아이템 리스트를 모두 비운다.
	}
} ;

const DWORD dwQuestStringColor[eQC_MAX] =												// 퀘스트 스트링 색상을 위한 배열.
{
	//RGBA_MAKE( 0, 255, 255, 255 ),														// 퀘스트 기본 스트링 색상.
	//RGBA_MAKE(255, 255, 0, 255),														// 메인 퀘스트 이름 색상.
	//RGBA_MAKE(0, 255, 0, 255),															// 서브 퀘스트 이름 색상.
	//RGBA_MAKE(255, 255, 255, 255),														// 카운트 색상.
	//RGBA_MAKE(255, 255, 255, 255),														// 완료 색상.
	//RGBA_MAKE(255, 10, 10, 255),														// 경험치 색상.
	//RGBA_MAKE(255, 255, 10, 255),															// 머니 색상.
	//RGBA_MAKE(10, 10, 255, 255),															// 아이템 색상.

	RGB( 0, 255, 255 ),														// 퀘스트 기본 스트링 색상.
	RGB(255, 255, 0),														// 메인 퀘스트 이름 색상.
	RGB(0, 255, 0),															// 서브 퀘스트 이름 색상.
	RGB(255, 255, 255),														// 카운트 색상.
	RGB(255, 255, 255),														// 완료 색상.
	RGB(255, 10, 10),														// 경험치 색상.
	RGB(255, 255, 10),															// 머니 색상.
	RGB(10, 10, 255),															// 아이템 색상.
} ;

class CGameNotifyManager																// 게임 공지 매니져 클래스.
{
	CGameNotifyDlg*		m_pNotifyDlg ;													// 게임 공지 출력용 다이얼로그 포인터.

	cPtrList			m_MsgList ;

public:
	CGameNotifyManager(void);															// 생성자 함수.
	virtual ~CGameNotifyManager(void);													// 소멸자 함수.

	void Init() ;																		// 초기화 함수.

	void SetNotifyDlg(CGameNotifyDlg* pDlg) { m_pNotifyDlg = pDlg ; }

	void AddMsg(sGAMENOTIFY_MSG* pGameNotifyMsg) ;										// 게임 공지 메시지 추가 함수.

	void Render() ;

	// QUEST PART //
	//void StartQuest(char* pQuestTitle) ;												// 퀘스트 시작 공지를 하는 함수.
	//void EndQuest(char* pQuestTitle) ;													// 퀘스트 종료 공지를 하는 함수.
	void UpdateSubQuest(SEND_SUBQUEST_UPDATE* msg) ;									// 퀘스트 업데이트 내용을 공지하는 함수.
	void EndSubQuest(SEND_QUEST_IDX* msg) ;												// 서브 퀘스트 종료시 내용을 공지하는 함수.
	//void TakeItem(CItem* pItem) ;														// 퀘스트 아이템을 획득 한 공지를 출력하는 함수.
	//void TakeMoney(char* pMoney) ;														// 퀘스트 보상금을 공지하는 함수.
	//void TakeExp(DWORD dwExp) ;															// 퀘스트 보상 경험치를 공지하는 함수.

	// 081020 LYW --- GameNotifyManager : 메시지 리스트의 아이템 수를 반환하는 함수 추가.
	int GetMsgCount() { return m_MsgList.GetCount() ; }
};

EXTERNGLOBALTON(CGameNotifyManager)