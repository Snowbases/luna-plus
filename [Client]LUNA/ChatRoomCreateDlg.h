#pragma once
//=================================================================================================
//	FILE		: CChatRoomMainDlg.h
//	DESC		: Dialog interface to create chatting room.
//	PROGRAMMER	: Yongs Lee
//	DATE		: APRIL 3, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
//#define _MB_CRCD_																						// If you want to see error message box, then use this definition.





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./Interface/cDialog.h"																		// Include header file of cDialog calss.

class cButton ;
class cComboBox ;
class cEditBox ;
class cPushupButton ;





//-------------------------------------------------------------------------------------------------
//		The class CChatRoomCreateDlg.
//-------------------------------------------------------------------------------------------------
class CChatRoomCreateDlg : public cDialog
{
	cPushupButton*		m_pOpen ;					// 공개/비공개 모드 버튼.
	cPushupButton*		m_pClose ;

	cEditBox*			m_pCodeBox ;				// 비밀번호 입력 에디트박스.

	cComboBox*			m_pGuestCount ;				// 최대 참여자 수 콤보박스.
	cComboBox*			m_pRoomType ;				// 채팅방 분류 콤보박스.

	cEditBox*			m_pTitleBox ;				// 채팅방 제목 입력 에디트박스.

	cButton*			m_pOKBtn ;					// 확인 버튼.
	cButton*			m_pCancelBtn ;				// 취소 버튼.

	BYTE				m_bySecretMode ;			// 공개/비공개 모드 변수.

	BYTE				m_byGeustCount ;			// 최대 참여자 타입을 받을 변수.
	BYTE				m_byRoomType ;				// 채팅방 분류를 받을 변수.
	
	// 100205 ONS 채팅방제목이 입력되었는지 여부를 확인.
	BOOL				m_bIsWritedTitle;			

public:
	CChatRoomCreateDlg(void) ;						// 생성자 함수.
	virtual ~CChatRoomCreateDlg(void) ;				// 소멸자 함수.

	void Linking() ;								// 하위 컨트롤들을 링크하는 함수.

	virtual void SetActive(BOOL val) ;				// 활성/비활성화 함수.

	void SetDefaultDlg() ;							// 채팅방 생성 다이얼로그를 기본 상태로 세팅하는 함수.

	// EventFunc로 부터 넘어온 Event를 처리하는 함수.
	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;

	void Chk_Open() ;								// 공개모드로 세팅하는 함수.
	void Chk_Close() ;								// 비공개 모드로 세팅하는 함수.

	void Cmb_Guest() ;								// 최대 참여자 수를 세팅하는 함수.
	void Cmb_Roomtype() ;							// 채팅방 분류를 세팅하는 함수.

	void Btn_Ok() ;									// 확인 버튼 이벤트를 처리하는 함수.

public :
	cEditBox* GetCodeBox()	{ return m_pCodeBox ; }	// 비밀번호 입력 에디트 박스 반환 함수.
};
