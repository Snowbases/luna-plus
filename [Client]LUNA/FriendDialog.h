// FriendDialog.h: interface for the CFriendDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRIENDDIALOG_H__9B871F1D_D035_41A6_9BD5_31F24628CDA8__INCLUDED_)
#define AFX_FRIENDDIALOG_H__9B871F1D_D035_41A6_9BD5_31F24628CDA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/ctabdialog.h"
#include ".\interface\cWindowHeader.h"

//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가( Tab다이얼로그로 변경 )
class CFriendDialog : public cTabDialog  
{
	cListCtrl * m_pFriendListLCtl;
	cPushupButton* m_FriendPageBtn[MAX_FRIEND_PAGE];
	cButton* m_AddFriendBtn;
	cButton* m_DelFriendBtn;
	cButton* m_SendNoteBtn;
	cButton* m_SendWhisperBtn;
	cCheckBox* m_pFriendChkBox[MAX_FRIEND_LIST];

	//090623 pdy 하우징 기능추가 (친구목록 링크 하우스입장) 
	cButton* m_LinkHouseBtnArr[MAX_FRIEND_LIST];
	
	WORD m_SelectedFriendPge;
		
	BOOL m_LogInGroup;
	BOOL m_LogOutGroup;

	cPushupButton*	m_PCRoomPageBtn[MAX_PCROOM_MEMBER_NUM/MAX_PCROOM_MEMBER_LIST];
	cPushupButton*	m_pFriendTabBtn;
	cPushupButton*	m_pPCRoomTabBtn;
	cListCtrl*		m_pPCRoomListCtrl;
	cStatic*		m_pPCRoomRegisted;
	cStatic*		m_pPCRoomBenefitLevel;
	cImage			m_pClassImg[MAX_PCROOM_MEMBER_LIST];
private:
//	CFriend* m_Friend;
	
public:
	CFriendDialog();
	virtual ~CFriendDialog();

	void Linking();
	virtual void SetActive( BOOL val );

	void SetFriendList(int gotopage);
	void ShowFriendPageBtn(BYTE TotalPage);

	WORD GetSelectedFriendPge() { return m_SelectedFriendPge;	}
	void SetSelectedFriendPge(WORD FPge) { m_SelectedFriendPge = FPge;	}
	
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	void CheckedDelFriend();
	
	char* GetChkedName();

	//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
	virtual void Add( cWindow* );
	virtual void Render();
	void ShowPCRoomPageBtn(BYTE TotalPage);
	void SetPCRoomMemberList( int gotopage );
	void SetPCRoomInfo( BOOL bIsPCRoom, WORD level = 0, WORD MemberMin = 0 );

};

#endif // !defined(AFX_FRIENDDIALOG_H__9B871F1D_D035_41A6_9BD5_31F24628CDA8__INCLUDED_)
