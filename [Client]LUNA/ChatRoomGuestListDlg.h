#pragma once
//=================================================================================================
//	FILE		: CChatRoomGuestListDlg.h
//	DESC		: Dialog interface to show guest list.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 25, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
//#define _MB_CRGLD_																// Use this definition to show messagebox.
//#define MAX_ROW_GUESTLIST		10





//-------------------------------------------------------------------------------------------------
//		Include header files.
//-------------------------------------------------------------------------------------------------
#include "./interface/cDialog.h"												// Include header file of cDialog calss.

class cButton ;
//class cImage ;
//class cListCtrl ;
class cListDialog ;




class CChatRoomGuestListDlg : public cDialog
{
	// 100107 ONS 대확상대목록을 닉네임, 상태, 레벨의 세개의 리스트로 변경
	cListDialog*		m_pGuestNicknameList ;						// 참여자 닉네임 리스트 다이얼로그.
	cListDialog*		m_pGuestStateList;							// 참여자 상태 리스트 
	cListDialog*		m_pGuestLevelList;							// 참여자 레벨 리스트

	cPtrList*			m_plGuest ;									// 참여자를 담고있는 리스트를 받을 포인터.

	cButton*			m_pBtn_ChangeOwner ;						// 방장위임 버튼.
	cButton*			m_pBtn_FreezeGuest ;						// 차단 버튼.
	cButton*			m_pBtn_KickGuest ;							// 강제퇴장 버튼.
	cButton*			m_pBtn_AddFriend ;							// 친구추가 버튼.

	ST_CR_USER			m_SelectedUser ;							// 선택 된 유저 정보를 담을 구조체.

	//DWORD				m_dwFreezedID[MAX_USER_PER_ROOM] ;
	//cImage			m_MarkImg[MAX_ROW_GUESTLIST] ;
	//ST_CR_USER		m_MyUserInfo ;
public:
	CChatRoomGuestListDlg(void) ;									// 생성자 함수.
	virtual ~CChatRoomGuestListDlg(void) ;							// 소멸자 함수.

	void Linking() ;												// 하위 컨트롤들을 링크하는 함수.

	virtual DWORD ActionEvent( CMouse* mouseInfo ) ;				// 마우스 이벤트를 처리하는 함수.
	// EventFunc로 부터 넘어온 Event를 처리하는 함수.
	virtual void OnActionEvent(LONG id, void* p, DWORD event) ;

	void InitGuestList(ST_CR_USER* pUser, BYTE byCount) ;			// 참여자 리스트를 세팅하는 함수.

	BOOL AddGuest(ST_CR_USER* pUser) ;								// 참여자 추가 처리를 하는 함수.
	void DeleteGuest(DWORD dwPlayerID) ;							// 참여자 삭제 처리를 하는 함수.

	void SetClassMark() ;											// 참여자 정보를 리스트 다이얼로그에 세팅하는 함수.

	void SetDlgToDefault() ;										// 참여자 리스트를 기본 상태로 세팅하는 함수.


	void ChangeOwner() ;											// 방장위임 버튼이 눌렸을 때 처리를 하는 함수.
	void FreezeGuest() ;											// 차단 버튼이 눌렸을 때 처리를 하는 함수.
	void KickGuest() ;												// 강제퇴장 버튼이 눌렸을 때 처리를 하는 함수.
	void AddFriend() ;												// 친구추가 버튼이 눌렸을 때 처리를 하는 함수.

	//void ChangeOwner(DWORD dwBeforOwner, DWORD dwCurOwner) ;
	//virtual void Render() ;
	//BOOL IsFreezed(DWORD dwPlayerID) ;
	//void FreezeUser(DWORD dwPlayerID) ;
	//void UnFreezeUser(DWORD dwPlayerID) ;
	//void ClearFreeze(DWORD dwPlayerID) ;
};
