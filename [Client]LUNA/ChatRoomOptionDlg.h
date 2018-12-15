#pragma once
//=================================================================================================
//	FILE		: CChatRoomOptionDlg.h
//	DESC		: Dialog interface to show option of chatroom.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
//#define _MB_CROD_																// Use this definition to show messagebox.





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./interface/cDialog.h"												// Include header file of cDialog calss.

class cButton ;
class cComboBox ;
class cPushupButton ;
class cEditBox ;





//-------------------------------------------------------------------------------------------------
//		The class CChatRoomOptionDlg.
//-------------------------------------------------------------------------------------------------
class CChatRoomOptionDlg : public cDialog
{
	cPushupButton*		m_pOpen ;												// 공개 모드 버튼.
	cPushupButton*		m_pClose ;												// 비공개 모드 버튼.

	cEditBox*			m_pCodeBox ;											// 비밀번호 에디트 박스.

	cComboBox*			m_pGuestCount ;											// 최대 참여자 수 콤보 박스.
	cComboBox*			m_pRoomType ;											// 채팅방 분류 콤보 박스.

	cEditBox*			m_pTitleBox ;											// 제목 입력 콤보 박스.

	cButton*			m_pOKBtn ;												// 확인 버튼.
	cButton*			m_pCancelBtn ;											// 취소 버튼.

	BYTE				m_bySecretMode ;										// 공개/비공개 모드 변수.
	//WORD				m_wSecretCode ;											// Store secret code.

	BYTE				m_byGeustCount ;										// 최대 참여자 수 타입 변수.
	BYTE				m_byRoomType ;											// 채팅방 분류 변수.

	//char				m_TitleBuff[128] ;										// Store title of room.

	// 저장 된 값을 담을 변수.
	//BYTE				m_byStoredMode ;										// 공개/비공개 모드 변수.
	////WORD				m_wStoredCode ;											// Store secret code.
	//char				m_code[SECRET_CODE_SIZE+1] ;							// 비밀번호를 담을 문자열.

	//BYTE				m_byStoredGuestCount ;									// 최대 참여자 수 타입 변수.
	//BYTE				m_byStoredRoomType ;									// 채팅방 분류 변수.

	//char				m_StoredTitleBuff[128] ;								// 제목을 담을 문자열.

public:
	CChatRoomOptionDlg(void) ;													// 생성자 함수.
	virtual ~CChatRoomOptionDlg(void) ;											// 소멸자 함수.

	void Linking() ;															// 하위 컨트롤 링크 함수.

	BOOL SetStoredDlg() ;														// 저장 된 값으로 옵션창을 세팅하는 함수.
	void SetDlgToDefault() ;													// 기본 값으로 옵션창을 세팅하는 함수.

	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;					// EventFunc로 부터 넘어온 Event를 처리하는 함수.

	void Chk_Open() ;															// 공개모드로 세팅하는 함수.	
	void Chk_Close() ;															// 비공개 모드로 세팅하는 함수.

	void Cmb_Guest() ;															// 최대 참여자 수를 세팅하는 함수.
	void Cmb_Roomtype() ;														// 채팅방 분류를 세팅하는 함수.
	
	void Btn_Ok() ;																// 확인 버튼이 눌렸을 때 처리를 하는 함수.
	void Btn_Cancel() ;															// 취소 버튼이 눌렸을 때 처리를 하는 함수.

	// 옵션을 적용하는 함수.
	//BOOL SetOption(ST_CR_ROOM_SRV* pInfo) ;										// 옵션을 세팅하는 함수.
};
