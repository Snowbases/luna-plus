#pragma once
#include "./interface/cdialog.h"

class cStatic;
class cButton;

class CPopupMenuDlg :
	public cDialog
{
	
	cStatic* mFace;
	cStatic* mName;

	cButton* mWhisper;
	cButton* mTrade;
	cButton* mFallow;
	cButton* mFriend;
	cButton* mParty;
	cButton* mGuild;
	cButton* mFamily;
	cButton* mVersus;

	//090615 pdy 하우징 캐릭터 우클릭 링크 하우스입장 기능 추가
	cButton* mVisitHouse;
	cButton* mQuickDate;

	// 100121 ONS PC방리스트에서 팝업메뉴를 활성화시켰는지 여부.
	BOOL mPCRoomDlgOpen;			
	char mSelectedName[MAX_NAME_LENGTH + 1];
	DWORD mSelectedIndex;

public:
	CPopupMenuDlg(void);
	virtual ~CPopupMenuDlg(void);

	void Linking();
	virtual void OnActionEvent(LONG lId, LPVOID, DWORD we) ;
	virtual void SetActive(BOOL);
	virtual DWORD ActionEvent(CMouse*);
	void SetActivePCRoomDlg(DWORD dwPlayerIdx, const char* pObjectName, BOOL val);
};
