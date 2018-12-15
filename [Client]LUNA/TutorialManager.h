#pragma once

#define TUTORIALMGR	USINGTON(cTutorialManager)

enum ENUM_TUTORIAL_TYPE
{
	e_TT_FirstGameIn = 0 ,
	e_TT_LevelUp,
	e_TT_Max,
} ;

class CItem;
class cTutorial;
class cTutorial_Msg;
class cTutorialDlg;
class cHelperDlg;
class cTutorialBtnDlg;
class CMHFile;

struct HELPLINK																// 도움말 링크 구조체를 정의한다.
{	
	int nIndex ;															// 출력 될 리스트 인덱스.
	int nTutorial_Idx ;														// 클릭시 진행 할 튜토리얼 인덱스.
	char Title[128] ;														// 출력 될 리스트 타이틀.
} ;

class cTutorialManager														// 튜토리얼을 관리할 매니져 클래스를 정의한다.
{
	cPtrList			m_Tutorial_Msg ;									// 튜토리얼 메시지를 관리하는 리스트.
	cPtrList			m_Tutorial_List ;									// 튜토리얼 정보를 관리하는 리스트.
	cPtrList			m_Helper_List ;										// 도움말 클릭으로 진행하는 튜토리얼 정보를 관리하는 리스트.

	BOOL				m_bOnOff_Tutorial ;									// 튜토리얼의 보이기 여부 변수.

	cTutorialDlg*		m_pTutorialDlg ;									// 튜토리얼 진행을 위한 다이얼로그 포인터.
	cHelperDlg*			m_pHelperDlg ;										// 도움말 모드 진행을 위한 다이얼로그 포인터.
	cTutorialBtnDlg*	m_pBtnDlg ;											// 튜토리얼 알림 버튼 다이얼로그 포인터.

	BOOL				m_bActiveTutorial ;									// 튜토리얼의 보이기 여부.
	BOOL				m_bUpdated ;										// 튜토리얼 이벤트가 업데이트 되었는지 체크하는 변수.

	int					m_nCurTutorialIdx ;									// 현재 진행중인 튜토리얼 인덱스.
	int					m_nCompletedIdx ;
	SNDIDX				m_nShow_BtnSndIdx;
	SNDIDX				m_nEndTutorial_SndIdx;

public:
	// BASE PART.
	cTutorialManager(void);													// 생성자 함수.
	virtual ~cTutorialManager(void);										// 소멸자 함수.

	void Init() ;															// 초기화 함수.
	void Release() ;														// 해제 함수.


	// TUTORIAL PART.
	BOOL LoadTutorial(char* strFileName) ;									// 튜토리얼 정보를 로딩하는 함수.
	void ReleaseTutorial() ;												// 튜토리얼 정보를 해제하는 함수.

	cTutorial* LoadMainTutorial(CMHFile* pFile) ;							// 메인 튜토리얼 정보를 로딩하는 함수.
	BOOL LoadSubTutorial(CMHFile* pFile, cTutorial* pTutorial) ;			// 서브 튜토리얼 정보를 로딩하는 함수.

	cTutorial* Get_Tutorial(int nIndex) ;									// 인덱스에 해당하는 튜토리얼 정보를 리턴하는 함수.


	// TUTORIAL MESSAGE PART.
	BOOL LoadMsg(char* strFileName) ;										// 튜토리얼 메시지를 로딩하는 함수.
	void ReleaseMsg() ;														// 튜토리얼 메시지를 해제하는 함수.

	BOOL ReadMsgLine(CMHFile* pFile, cTutorial_Msg* pMsg) ;					// 튜토리얼 메시지의 라인을 읽는 함수.

	cTutorial_Msg* Get_TutorialMsg(int nIndex) ;							// 인덱스에 해당하는 튜토리얼 메시지 정보를 리턴하는 함수.


	// HELPER PART.
	BOOL LoadHelperInfo(char* strFileName) ;								// 도움말 정보를 로딩하는 함수.
	void ReleaseHelperInfo() ;												// 도움말 정보를 해제하는 함수.

	HELPLINK* Get_HelpLink(int nIndex) ;									// 인덱스에 해당하는 도움말 링크를 리턴하는 함수.

	void Start_HelpTutorial(cTutorial* pTutorial) ;							// 도움말 링크에서 선택 된 튜토리얼을 실행하는 함수.


	// NETWORK PART.
	void NetworkMsgParse(BYTE Protocol,void* pMsg) ;						// 네트워크 메시지 파싱 함수.


	// PROCESS PART.
	//void Process() ;														// 프로세스 함수.
	BOOL Check_StartCondition(cTutorial* pTutorial) ;						// 튜토리얼 발동 조건을 체크하는 함수.

	BOOL Check_NpcTalk(void* pMsg) ;										// npc와 대화를 하는 조건을 체크하는 함수.
	BOOL Check_NpcTalk_StartCondition(DWORD dwNpcIdx) ;						// npc와 대화 조건 중, 시작 조건을 체크하는 함수.
	BOOL Check_NpcTalk_EndCondition(DWORD dwNpcIdx) ;						// npc와 대화 조건 중, 완료 조건을 체크하는 함수.
	// 080506 LYW --- TutorialManager : 레벨업 시, 진행 가능한 tutorial이 있는지 확인 후, 있으면 진행.
	void Check_LevelUp() ;

	void Check_GetItem(CItem* pItem) ;										// 아이템을 습득했을 때 튜토리얼 진행 여부를 체크하는 함수.

	void Check_OpenDialog(DWORD dwWindowID, BOOL bVal) ;					// 창을 여는 완료 조건을 체크하는 함수.


	// ETC
	void Set_ActiveTutorial(BOOL bActive) { m_bActiveTutorial = bActive ; }	// 튜토리얼의 활성화 여부를 세팅하는 함수.
	BOOL Is_ActiveTutorial() { return m_bActiveTutorial ; }					// 튜토리얼의 활성화 여부를 반환하는 함수.

	void Set_CurTutorialIdx(int nIndex) { m_nCurTutorialIdx = nIndex ; }	// 현재 튜토리얼 인덱스를 세팅하는 함수.
	int  Get_CurTutorialIdx() { return m_nCurTutorialIdx ; }				// 현재 튜토리얼 인덱스를 반환하는 함수.

	void Set_TutorialDlg(cTutorialDlg* pDlg) { m_pTutorialDlg = pDlg ; }	// 멤버 튜토리얼 다이얼로그를 세팅하는 함수.
	void Set_HelperDlg(cHelperDlg* pDlg) { m_pHelperDlg = pDlg ; }			// 멤버 도움말 다이얼로그를 세팅하는 함수.
	void Set_TutorialBtnDlg(cTutorialBtnDlg* pDlg) { m_pBtnDlg = pDlg ; }	// 멤버 튜토리얼 알림 버튼 다이얼로그를 세팅하는 함수.
	cTutorialBtnDlg* Get_TutorialBtnDlg() { return m_pBtnDlg ; }			// 튜토리얼 알림 버튼 다이얼로그를 반환하는 함수.
	cPtrList& GetHelperList() { return m_Helper_List; }
	void UpdateTutorial(int nCompleteIndex);
	void Start_CurTutorial(cTutorial* pTutorial);
	void Reset_MemberDlg();
	SNDIDX Get_ShowBtnSndIdx() const { return m_nShow_BtnSndIdx ; }
	SNDIDX Get_EndTutorialSndIdx() const { return m_nEndTutorial_SndIdx; }
	BYTE IsCompleteTutorial(BYTE byTutorialType);
	// 090417 ONS 튜토리얼 다이얼로그 On/Off 상태 설정/반환 
	BOOL CheckTutorialByIndex(int TutorialIndex);
	void SetOnOffTutorial(BOOL val) { m_bOnOff_Tutorial = val; }
	BOOL GetOnOffTutorial() { return m_bOnOff_Tutorial; }

	// 080502 LYW --- TutorialManager : 지정 된 타입의 튜토리얼을 시작하는 함수 추가.
	void Start_Specification_Tutorial(BYTE byTutorialType) ;
};





EXTERNGLOBALTON(cTutorialManager)