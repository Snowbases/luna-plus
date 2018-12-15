#pragma once
//=================================================================================================
//	FILE		: CChatRoomJoinDlg.h
//	DESC		: Dialog interface to join chatting room.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
//#define _MB_CRJD_																// Use this definition to show messagebox.





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./interface/cDialog.h"												// Include header file of cDialog calss.

class cButton ;																	// Button interface.
class cEditBox ;																// EditBox interface.
class cStatic ;																	// Static interface.





//-------------------------------------------------------------------------------------------------
//		The class CChatRoomJoinDlg.
//-------------------------------------------------------------------------------------------------
class CChatRoomJoinDlg : public cDialog
{
	BYTE			m_byInputMode ;												// 방번호/비밀번호 모드를 담을 변수.

	cEditBox*		m_pEdb_Code ;												// 비밀번호 입력 에디트박스.

	cButton*		m_pBtn_OK ;													// 확인버튼.
	cButton*		m_pBtn_Cancel ;												// 취소버튼.

	cStatic*		m_pStc_RoomIndex ;											// [ 방번호 입력 ]을 출력 할 스태틱.
	cStatic*		m_pStc_SecretCode ;											// [ 비밀번호 입력 ]을 출력 할 스태틱.

	cStatic*		m_pStc_IndexDesc ;											// 방번호 입력 설명을 할 스태틱.
	cStatic*		m_pStc_CodeDesc ;											// 비밀번호 입력 설명을 할 스태틱.

	//BYTE			m_byRoomIdx ;												// 채팅방 인덱스를 담을 변수.

public:
	CChatRoomJoinDlg(void);														// 생성자 함수.
	virtual ~CChatRoomJoinDlg(void);											// 소멸자 함수.

	void Linking() ;															// 하위 컨트롤들을 링크하는 함수.

	virtual DWORD ActionKeyboardEvent( CKeyboard * keyInfo ) ;					// 키보드 이벤트를 처리하는 함수.

	virtual void SetActive(BOOL val) ;											// 참여 다이얼로그 활성/비활성화 함수.

	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;					// EventFunc로 부터 넘어온 Event를 처리하는 함수.

	void SetInputMode(BYTE byMode) ;											// 입력 모드를 설정하는 함수.
	//void SetRoomIdx(BYTE byIdx) { m_byRoomIdx = byIdx ; }						// 
};
