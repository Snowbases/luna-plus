#pragma once
//=================================================================================================
//	FILE		: CChatRoomDlg.h
//	DESC		: Dialog interface chatting room.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
//#define _MB_CRD_																// Use this definition to show messagebox.





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./interface/cDialog.h"												// Include header file of cDialog calss.

class cButton ;
class cEditBox ;
class cListDialog ;
class cStatic ;
//class cTextArea ;





//-------------------------------------------------------------------------------------------------
//		The class CChatRoomDlg.
//-------------------------------------------------------------------------------------------------
class CChatRoomDlg : public cDialog
{
    cStatic*			m_pRoomTitle ;						// 채팅방 제목 출력용 스태틱.

	cListDialog*		m_pChatList ;						// 채팅 내용 출력용 리스트 다이얼로그.

	cEditBox*			m_pInputBox ;						// 채팅 내용 입력용 에디트 박스.
	//cTextArea*			m_pInputBox ;

	cButton*			m_pBtn_GuestList ;					// 대화상대 버튼.
	cButton*			m_pBtn_Option ;						// 채팅옵션 버튼.

	cButton*			m_pBtn_Close ;						// 채팅방 닫기 버튼.

public:
	CChatRoomDlg(void) ;									// 생성자 함수.
	virtual ~CChatRoomDlg(void) ;							// 소멸자 함수.

	void Linking() ;										// 하위 컨트롤 링크 함수.

	// EventFunc로 부터 넘어온 Event를 처리하는 함수.
	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;
	//virtual void SetActive(BOOL val) ;						// 채팅방 활성/비활성화 함수.

	void AddMsg(char* pMsg, BYTE byType) ;					// 채팅방 내에 메시지를 추가하는 함수.
	void AddCommonMsg(BYTE byType, char* pMsg) ;			// 메시지 타입과 함께 메시지를 추가하는 함수.

	void SetRoomTitle(ST_CR_ROOM_CLT* pInfo) ;				// 채팅방 제목 세팅함수.
	void SetDlgToDefault() ;								// 채팅방을 기본 상태로 세팅하는 함수.

	cEditBox* GetInputBox() { return m_pInputBox ; }		// 채팅방 내용 입력용 에디트 박스 반환 함수.
	cListDialog* GetListDlg() { return m_pChatList ; }		// 채팅방 내용 출력용 리스트 다이얼로그 반환 함수.

	DWORD GetMsgColor(BYTE byColorType) ;					// 메시지 타입에 따라 색상을 반환하는 함수.

	void CloseChatRoom() ;									// 채팅방을 닫는 처리를 하는 함수.




	//virtual DWORD ActionEvent(CMouse* mouseInfo) ;
	//virtual void Render() ;
	//virtual DWORD ActionKeyboardEvent( CKeyboard * keyInfo ) ;
	//void AddMsg(char* pMsg, DWORD dwColor) ;
	//void SetRoomTitle(char* pMsg) ;
	//cTextArea* GetInputBox() { return m_pInputBox ; }
};
