#pragma once
#include "INTERFACE\cDialog.h"
#include "Item.h"

class cStatic;
class cButton;
class cPushupButton;
class cEditBox;
class cComboBox;
class cListDialog;

class cIconDialog;

#define DISPLAYNUM_PER_PAGE				5
#define MAX_ITEMNUM_PER_PAGE			10
#define SEARCH_MIN_LEVEL				"0"
#define SEARCH_MAX_LEVEL				"150"

enum eConsignmentCategoryType
{
	eConsignmentCategory1,
	eConsignmentCategory2,
	eConsignmentCategory_Max,
};

struct stCategoryInfo1
{
	DWORD	dwCategory1Value;
	char	szCategory1Name[MAX_ITEMNAME_LENGTH + 1];
};

struct stCategoryInfo2
{
	DWORD	dwItemTypeDetail;
	DWORD	dwCategory1Value;
	DWORD	dwCategory2Value;
	char	szCategory2Name[MAX_ITEMNAME_LENGTH + 1];
};

class CConsignmentDlg :
	public cDialog
{
public:
	enum { eConsignment_Mode_Regist,
		eConsignment_Mode_Buy,
		eConsignment_Mode_Max };

	enum { eConsignment_Reciept_UserCancel,
		eConsignment_Reciept_TimeCancel,
		eConsignment_Reciept_Buy,
		eConsignment_Reciept_SoldOut };

public:
	CConsignmentDlg(void);
	virtual ~CConsignmentDlg(void);

	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive(BOOL val);
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual DWORD ActionKeyboardEvent( CKeyboard * keyInfo ) ;
	void Linking();
	virtual void Render();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );

	// 공통
	void		SetMode(int nMode);
	int			GetMode()	{return m_nMode;}
	void		ClearControls(int nMode);
	DWORD		GetConsignmentIndex();
	void		LoadCategoryInfo();
	DWORD		GetCategory1Value(const char* pName);
	DWORD		GetCategory2Value(const char* pName);
	DWORD		GetCategory2Value(DWORD dwItemTypeDetail);
	void		GetCategoryValueByItemIndex(DWORD dwItemIndex, DWORD& dwCategory1, DWORD& dwCategory2);
	void		UpdateCategory2List();

	// 등록관련
	void		ConfirmCancel(WORD nBtnIndex);
	void		SetRegistItem(ITEMBASE* pBaseInfo, ITEM_OPTION* pOptionInfo, cIcon* pOrigIcon);
	void		SendRegistMsg();
	void		UpdateRegIconList();
	void		UpdateRegList(int nNum, MSG_CONSIGNMENT_SEARCH_RESULT* pSearchResult);


	// 구입관련
	void		ConfirmBuy(DWORD dwConsignmentIndex);
	DWORD		GetBuyItemIndex(DWORD dwConsignmentIndex);
	DWORD		GetBuyDurability(DWORD dwConsignmentIndex);
	DWORD		GetBuyPrice(DWORD dwConsignmentIndex);
	void		SendSearchMsg();
	void		SendSearchIndexMsg();
	void		UpdateBuyIconList();
	void		UpdateBuyList(int nNum, int nCurPage, int nTotalPage, MSG_CONSIGNMENT_SEARCH_RESULT* pSearchResult);
	BOOL		CheckSearchTime();
	void		SearchOnClient();
	void		AddItemToKeywordList(const char* pItemName, DWORD dwColor);
	void		SetControlByItemIndex(char* pItemName);
	void		ResetFilter();

	static void OnBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void* vData2 );
	static void OnCancelBuyItem( LONG iId, void* p, DWORD param1, void * vData1, void* vData2 );
	DWORD		m_dwBuyDurability;
	char		m_szBuyItemName[256];

	void		SetHold(BOOL bVal) {m_bHold = bVal;}


private:

	BOOL		m_bHold; // DB에서 결과 받을때까지는 TRUE값 유지
	int			m_nMode;
	CItem*		m_RegistIcon;
	ITEMBASE	m_EmptyItemBase;
	DWORD		m_dwLastSearchTime;

	WORD		m_wSortType;
	WORD		m_wCurSelPage;
	WORD		m_wMaxSearchPage;
	DWORD		m_dwLastRegistItemIdx;
	DWORD		m_dwLastRegistItemUnitPrice;

	BOOL		m_bOverCount;
	DWORD		m_dwSearchItemList[CONSIGNMENT_SEARCHINDEX_NUM];

	CYHHashTable<stCategoryInfo1>	m_htCategory1Info;
	CYHHashTable<stCategoryInfo2>	m_htCategory2Info;

	CItem*		m_RegistedIcon[DISPLAYNUM_PER_PAGE];
	CItem*		m_SearchIcon[DISPLAYNUM_PER_PAGE];


	MSG_CONSIGNMENT_SEARCH_RESULT	m_RegistedList;
	MSG_CONSIGNMENT_SEARCH_RESULT	m_SearchList;


	///////////////////////////
	/////  UI 관련 변수들...
	///////////////////////////
	cPtrList m_RegistControlListArray;
	cPtrList m_BuyControlListArray;

	// 공통
	cPushupButton*	m_PageBuyBtn;
	cPushupButton*	m_PageRegistBtn;
	cButton*		m_pClose;
	cButton*		m_pHelp;
	cStatic*		m_pHaveMoney;

	// For 등록
	cStatic*	m_pReg_ItemName;
	cStatic*	m_pReg_ItemLevel;
	cStatic*	m_pReg_RemainTime;
	cStatic*	m_pReg_Price;
	cStatic*	m_pReg_Cancel;
	cStatic*	m_pReg_FilterTop;
	cStatic*	m_pReg_FilterMiddle;
	cStatic*	m_pReg_FilterBottom;
	cStatic*	m_pReg_TotalPriceBG;
	cStatic*	m_pReg_TotalPrice;
	cStatic*	m_pReg_DepositBG;
	cStatic*	m_pReg_Deposit;
	cStatic*	m_pReg_CommissionBG;
	cStatic*	m_pReg_Commission;
	cStatic*	m_pReg_Icon;
	cStatic*	m_pReg_PriceLine;
	cStatic*	m_pReg_Dollar1;
	cStatic*	m_pReg_Text1;
	cStatic*	m_pReg_Text2;
	cStatic*	m_pReg_Text3;
	cStatic*	m_pReg_Text4;

	cButton*	m_pReg_Cancel1Btn;
	cButton*	m_pReg_Cancel2Btn;
	cButton*	m_pReg_Cancel3Btn;
	cButton*	m_pReg_Cancel4Btn;
	cButton*	m_pReg_Cancel5Btn;
	cButton*	m_pReg_RegistBtn;
	cEditBox*	m_pReg_UnitPriceEdit;

	cIconDialog*	m_pReg_ItemDlg;
	cIconDialog*	m_pReg_RegistedItemDlg[DISPLAYNUM_PER_PAGE];

	cListDialog*	m_pReg_ItemList;
	cListDialog*	m_pReg_ItemList_Lv;
	cListDialog*	m_pReg_ItemList_Time;
	cListDialog*	m_pReg_ItemList_Price;

	// for 구입
	cStatic*	m_pBuy_SearchBG;
	cStatic*	m_pBuy_FilterTop;
	cStatic*	m_pBuy_FilterMiddle;
	cStatic*	m_pBuy_FilterBottom;
	cStatic*	m_pBuy_PageTop;
	cStatic*	m_pBuy_PageBottom;
	cStatic*	m_pBuy_SellerBG;
	cStatic*	m_pBuy_Category1;
	cStatic*	m_pBuy_Category2;
	cStatic*	m_pBuy_RareItem;
	cStatic*	m_pBuy_LevelMinBG;
	cStatic*	m_pBuy_LevelMaxBG;
	cStatic*	m_pBuy_Text1;
	cStatic*	m_pBuy_Text2;
	cStatic*	m_pBuy_PagePrint;
	
	cButton*	m_pBuy_PageLeftBtn;
	cButton*	m_pBuy_PageRightBtn;
	cButton*	m_pBuy_SortNameBtn;
	cButton*	m_pBuy_SortLevelBtn;
	cButton*	m_pBuy_SortPriceBtn;
	cButton*	m_pBuy_BuyBtn;
	cButton*	m_pBuy_SearchBtn;
	cButton*	m_pBuy_ResetBtn;
	cEditBox*	m_pBuy_SearchEdit;
	cEditBox*	m_pBuy_LevelMinEdit;
	cEditBox*	m_pBuy_LevelMaxEdit;
	cComboBox*	m_pBuy_RareItemCombo;

	cIconDialog*	m_pBuy_ListItemDlg[DISPLAYNUM_PER_PAGE];

	cListDialog*	m_pBuy_KeywordList;
	cListDialog*	m_pBuy_Category1List;
	cListDialog*	m_pBuy_Category2List;
	cListDialog*	m_pBuy_ItemList;
	cListDialog*	m_pBuy_ItemList_Lv;
	cListDialog*	m_pBuy_ItemList_Seller;
	cListDialog*	m_pBuy_ItemList_Price;
};