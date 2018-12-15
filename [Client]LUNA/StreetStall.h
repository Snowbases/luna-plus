// StreetStall.h: interface for the CStreetStall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREETSTALL_H__0553CA73_956D_4CA4_95ED_3F8280AF8C6B__INCLUDED_)
#define AFX_STREETSTALL_H__0553CA73_956D_4CA4_95ED_3F8280AF8C6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "./Interface/cDialog.h"

// 071012 LYW --- StreetStall : Define title text number of owner static.
#define TEXTNUM_OWNERTITLE	207

class cIcon;
class cIconGridDialog;
class cEditBox;
class cTextArea;
class cButton;

// 071012 LYW --- StreetStall : Include static class.
class cStatic ;

enum STALL_DLG_STATE
{
	eSDS_NOT_OPENED,
	eSDS_OPENED,
	eSDS_BUY,
};

enum STALL_OPTION
{
	eSO_DIVE,
	eSO_INPUTMONEY,
	eSO_INPUTMONEY_DUP,
};

// 070329 LYW --- StreetStall : Modified default message number.
//#define DEFAULT_TITLE_TEXT	CHATMGR->GetChatMsg(366)
#define DEFAULT_TITLE_TEXT	CHATMGR->GetChatMsg(486)

class CExchangeItem;

// LBS 수정 03.09.19
class CStreetStall : public cDialog  
{
	DWORD m_DlgState;
	
	cEditBox* m_pTitleEdit;
//	cTextArea* m_pTitle;
	cEditBox* m_pMoneyEdit;
	cButton* m_pEnterBtn;
	cButton* m_pBuyBtn;
	cButton* m_pEditBtn;
	//cButton* m_pRegistBtn;
	cIconGridDialog * m_pStallGrid;

	// 071012 LYW --- StreetStall : Add static control for owner name of stall dialog.
	cStatic*	m_pOwnerName ;

	DWORD m_MoneyArray[SLOT_STREETSTALL_NUM];
	DWORD m_TotalMoney;
	char m_OldTitle[MAX_STREETSTALL_TITLELEN+1];
	WORD m_nCurSelectedItem;
	DWORD m_dwCurSelectedItemDBIdx;
	
	DWORD m_dwOwnnerId;									// 상점주인의 아이디

	void* m_pData;										// 여분의 이용 데이터 ( 현재 영약아이템 나눠사기에 사용 )

public:
	CStreetStall();
	virtual ~CStreetStall();

	void Linking();

	void SetDisable( BOOL val );

	void ShowSellStall();
	void ShowBuyStall();
	
	void OnCloseStall(BOOL bDelOption = FALSE);
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon * icon);
	virtual void FakeMoveItem(LONG mouseX, LONG mouseY, cIcon * icon) {}
	void FakeDeleteItem( POSTYPE pos );

	void OnActionEvnet(LONG lId, void * p, DWORD we);
	virtual DWORD ActionEventWindow(CMouse * mouseInfo);
	virtual void SetActive( BOOL val );

	void RegistMoney();
	void RegistMoney( POSTYPE pos, DWORD dwMoney );
	void RegistTitle( char* title, BOOL bSucess );
	void EditTitle( char* title );

public:
	void GetTitle( char* pStrTitle );
	WORD GetCurSelectedItemNum() const { return m_nCurSelectedItem; }
	void SetCurSelectedItemNum(WORD num) { m_nCurSelectedItem = num; }
	cIcon* GetCurSelectedItem();

	DWORD GetCurSelectedItemDBidx();
	DWORD GetCurSelectedItemDur();

	DWORD GetCurSelectedItemIdx();

	DWORD GetCheckDBIdx() { return m_dwCurSelectedItemDBIdx; }
	
	cIcon* GetItem( POSTYPE pos );
	cIcon* GetItem( DWORD dwDBIdx );

	DWORD GetItemMoney( POSTYPE pos ) { return m_MoneyArray[pos]; }

	int AddItem( cIcon* pItem );
	void DeleteItem( ITEMBASE* pbase );
	void DeleteItemAll(BOOL bDelOption = FALSE);
	void ResetItemInfo( DWORD dwDBIdx, DWORD count );
	CExchangeItem* FindItem( DWORD dwDBIdx );

	void ChangeItemStatus( POSTYPE pos, DWORD money, BOOL bLock );
	void ChangeItemStatus( ITEMBASE* pbase, DWORD money, BOOL bLock, int nDivideKind = 0 );

	BOOL SelectedItemCheck();
	BOOL MoneyEditCheck();

	DWORD GetDlgState() { return m_DlgState; }

	DWORD GetStallOwnerId() { return m_dwOwnnerId; }
	void SetStallOwnerId( DWORD dwId ) { m_dwOwnnerId = dwId; }

	void ResetDlgData();
	int GetGridposForItemIdx(WORD wIdx);

	// 나누기창 생성
	void ShowDivideBox(DWORD dwOption = eSO_DIVE, int x = 0, int y = 0, int nDivideKind = 0 );
	static void OnDivideItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnDivideItemCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnFakeRegistItem( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	static void OnRegistItemCancel( LONG iId, void* p, DWORD param1, void * vData1, void * vData2 );
	
	// 추가 데이터
	void* GetData() { return m_pData; }
	void SetData( void* pData ) { m_pData = pData; }

	cIconGridDialog* GetGridDialog() { return m_pStallGrid; }

	// 071012 LYW --- StreetStall : Add function to return owner name static.
	cStatic* GetOwnerTitle() { return m_pOwnerName ; }
};

#endif // !defined(AFX_STREETSTALL_H__0553CA73_956D_4CA4_95ED_3F8280AF8C6B__INCLUDED_)
