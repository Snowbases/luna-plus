#pragma once
//=================================================================================================
//	FILE		: CChatRoomMainDlg.h
//	DESC		: Dialog interface to show chatting room list.
//	PROGRAMMER	: Yongs Lee
//	DATE		: APRIL 3, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
//#define MAX_ROW_GUESTLIST		10

enum ENUM_CHATROOM_MAINDLG_SEARCH_MODE		// 검색모드 분류 정의.
{
	e_Search_Title = 0,						// 제목 검색.
	e_Search_Name,							// 이름 검색.

	e_Search_Max,
} ;

enum ENUM_CHATROOM_JOIN_DLG_TYPE			// 채팅방 참여 다이얼로그 분류 정의.
{
	e_JoinDlg_Type_RoonIndex = 0,			// 방번호 입력 모드.
	e_JoinDlg_Type_SecretCode,				// 비밀번호 입력 모드.

	e_JoinDlg_Type_Max,
} ;





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./interface/cDialog.h"
#include "./interface/cComboBox.h"

class cButton ;
class cComboBox ;
class cEditBox ;
class cListDialog ;
class cPushupButton ;
class cStatic ;





//-------------------------------------------------------------------------------------------------
//		The class CChatRoomMainDlg.
//-------------------------------------------------------------------------------------------------
class CChatRoomMainDlg : public cDialog
{
	// 채팅방 정보를 담을 Map.
	typedef std::list<ST_CR_ROOM_CLT> LIST_ROOM ;
	LIST_ROOM			m_Room ;

	// 상황에 따라 임시로 채팅방 정보를 담을 Map.
	typedef std::list<ST_CR_ROOM_CLT> LIST_TEMPROOM ;
	LIST_TEMPROOM			m_TempRoom ;

	// 선택 한 채팅방 정보.
	ST_CR_ROOM_CLT	m_SelectRoom ;

//	cPtrList			m_RoomList ;
//
//public :
//	void ReleaseRoom() ;													// 채팅방 리소스 해제 함수.
//
//	BYTE IsInRoomList(BYTE byRoomIdx) ;										// 방 리스트에, 방 정보가 있는지 체크하는 함수.
//
//	BYTE AddRoom(ST_CR_ROOM_CLT* pInfo) ;							// 채팅방 정보를 추가하는 함수.
//	BYTE DeleteRoom(BYTE byRoomIdx) ;										// 채팅방 정보를 삭제하는 함수.
//
//	ST_CR_ROOM_CLT* GetRoomInfo(BYTE byRoomIdx) ;					// 채팅방 정보를 반환하는 함수.
//
//	void UpdateRoom(ST_CR_ROOM_CLT* pInfo) ;

private :	
	cListDialog*		m_pRoomList ;					// 채팅방를 보여줄 리스트 다이얼로그.

	cButton*			m_pSearchBtn ;					// 검색 버튼.
	cEditBox*			m_pSearchBox ;					// 검색 입력 에디트박스.
	cPushupButton*		m_pTitle ;						// 제목 검색 버튼.
	cPushupButton*		m_pName ;						// 이름 검색 버튼.

	cComboBox*			m_pRoomType ;					// 채팅방 분류 콤보박스.

	cButton*			m_pCreateBtn ;					// 채팅방 개설 버튼.
	cButton*			m_pJoinBtn ;					// 채팅방 참여 버튼.
	cButton*			m_pNumJoinBtn ;					// 채팅방 번호입력 참여 버튼.
	cButton*			m_pRefreshBtn ;					// 새로고침 버튼.

	cButton*			m_pPrevBtn ;					// 이전 페이지 버튼.
	cButton*			m_pNextBtn ;					// 다음 페이지 버튼.

	cStatic*			m_pPage ;						// 페이지 출력 스태틱.

	BYTE				m_bySearchMode ;				// 검색 모드를 담을 변수.

	BYTE				m_byRoomType ;					// 채팅방 분류를 담을 변수.

	BYTE				m_byCurPage ;					// 현재 페이지를 담을 변수.
	BYTE				m_byMaxPage ;					// 최대 페이지를 담을 변수.

	//cStatic*			m_pLoad_Chatroom ;

	//char				m_SearchBuff[128] ;				// 

	//BYTE				m_byPrintMode ;		// 현재 출력되고 있는 리스트가 어떤 모드인지 담는 변수.

public:
	CChatRoomMainDlg(void) ;							// 생성자 함수.
	virtual ~CChatRoomMainDlg(void) ;					// 소멸자 함수.

	void Linking() ;									// 하위 컨트롤 링크 함수.

	// 마우스 이벤트 처리 함수.
	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;
	// EventFunc로 부터 넘어온 Event를 처리하는 함수.
	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;

	//virtual void SetActive(BOOL val) ;

	//virtual DWORD ActionKeyboardEvent( CKeyboard * keyInfo ) ;

public :
	void Search_Syn() ;									// 검색 처리를 하는 함수.

	void Check_Title() ;								// 검색모드를 제목 검색으로 세팅하는 함수.
	void Check_Name() ;									// 검색모드를 이름 검색으로 세팅하는 함수.
	
	void Create_Syn() ;									// 채팅방 생성 처리를 하는 함수.
	void Join_Syn() ;									// 채팅방 참여 처리를 하는 함수.
	void NumJoin_Syn() ;								// 방번호 입력 참여 처리를 하는 함수.
	void Refresh_Syn() ;								// 새로고침 처리를 하는 함수.

	void Btn_Prev() ;									// 이전 페이지 처리를 하는 함수.
	void Btn_Next() ;									// 다음 페이지 처리를 하는 함수.

	//void SortRoomByType() ;	

	// 참여창/비밀번호창에서 OK버튼을 눌렀을 때 처리하는 함수.
	void Check_Room_To_Join(BYTE inputMode, char* pCode) ;

	// 채팅방 정보를 담고 있는 리스트를 반환하는 함수.
	cListDialog* GetListDlg() { return m_pRoomList ; }

public :
	void Init_RoomList(void* pMsg) ;					// 채팅방 리스트를 세팅하는 함수.
	//void Init_TypeRoomList(void* pMsg) ;
	void Refresh_RoomList() ;							// 채팅방 리스트를 새로고침하는 함수.
	void Clear_RoomList() ;								// 채팅방 리스트를 비우는 함수.

	BOOL Add_Room(ST_CR_ROOM_CLT* pInfo) ;				// 채팅방 추가함수.
	void Update_RoomInfo(ST_CR_ROOM_CLT* pInfo) ;		// 채팅방 정보를 업데이트 하는 함수.

	void DeleteRoom(BYTE byRoomIdx) ;					// 채팅방을 삭제하는 함수.
	void SetPage(BYTE num1, BYTE num2) ;				// 페이지를 세팅하는 함수.

	//BYTE GetCurRoomType() { return (BYTE)m_pRoomType->GetCurSelectedIdx() ; }

	// 채팅방 정보를 반환하는 함수.
	ST_CR_ROOM_CLT* GetRoomListItem(BYTE byRoomIdx) ;

public : 
	void Err_EmptyRoom() ;								// 채팅방이 없을 때, 개설 된 방이 없다는 공지를 띄우는 함수.
};
