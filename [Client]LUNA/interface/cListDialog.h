#pragma once


#include "cDialog.h"
#include "cListItem.h"

enum
{
	LD_TOP,
	LD_MIDDLE,
	LD_DOWN
};

enum
{
	LDS_NORMAL,
	LDS_SERVERLIST,
	LDS_FONTSHADOW,	// 리스트 다이얼로그에 추가되는 폰트를 지정해 줄 수 있고, 그림자가 적용된 텍스트를 출력한다.
};

enum
{
	LD_EXTEND = 0,
	LD_REDUCTION,
} ;

class cGuageBar;
class cIcon;

struct sGAMENOTIFY_MSG;

// 090820 ShinJS --- 정렬 추가
enum eListItemSortType
{
	eListItemSortType_None,
	eListItemSortType_StringLess,
	eListItemSortType_StringGreater,
	eListItemSortType_SortDataLess,
	eListItemSortType_SortDataGreater,
};

class cListDialog : public cDialog, public cListItem
{
public:
	cListDialog();
	virtual ~cListDialog();

	virtual void Init(LONG x, LONG y, WORD wid, cImage * topImage, WORD topHei, cImage * middleImage, WORD middleHei, cImage * downImage, WORD downHei, cImage * overImage, BYTE num, LONG ID);
	virtual void InitList(WORD wMaxLine, RECT * textClippingRect);
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void Add(cWindow * btn);
	virtual void Render();

	void RenderNormalList();
	void RenderServerList();
	void RenderShadowList();
	virtual void SetActive(BOOL);
	BOOL IsMaxLineOver();

	BOOL Resize(BYTE middleNum);
	WORD GetHeight(BYTE i)
	{
		if(i == LD_TOP) return m_topHeight;
		if(i == LD_MIDDLE) return m_middleHeight;
		if(i == LD_DOWN) return m_downHeight;
		return 0;
	}
	BYTE GetMiddleImageNum(){ return m_middleNum; }
	void SetMinMaxMiddleNum(BYTE min, BYTE max) { m_minMiddleNum = min; m_maxMiddleNum = max; }
	void SetOverImageScale(float x, float y) { m_OverImageScale.x = x; m_OverImageScale.y = y; }

	// 070529 LYW --- Add function to return min num.
	BYTE GetMinMiddleNum() { return m_minMiddleNum ; }
	BYTE GetMaxMiddleNum() { return m_maxMiddleNum ; }

	void OnUpwardItem();
	void OnDownwardItem();
	int PtIdxInRow(LONG x, LONG y);

	virtual void AddItem( const char * text, DWORD color, int line=-1, DWORD dwData=0, DWORD dwSortData=0 );

	// 080225 LUJ, 툴팁과 함께 리스트 아이템을 추가한다
	typedef std::list< std::string >	ToolTipTextList;
	void AddItem( const char* text, DWORD color, const ToolTipTextList&, int line=-1, DWORD dwData=0, WORD wImageIdx = 0 );

	// 080225 LUJ, 툴팁을 삭제하면서 툴팁도 갱신한다
	virtual void RemoveItem( const char* );
	virtual void RemoveItem( DWORD index );	

	void AddItemWithFont(char* str, DWORD color, int fontIdx, int line=-1) ;
	void AddNoticeItem(sGAMENOTIFY_MSG* pGameNotifyMsg) ;

	void SetExtendReduction(int nSetMode) ;										// 리스트 다이얼로그를 확대/축소 하는 함수.(퀘스트 알림이 다이얼로그에서 사용)

	void onSize(BOOL fSize);
	//	virtual void ListMouseCheck( LONG x, LONG y, DWORD we );
	void ListMouseCheck( CMouse * mouseInfo );

	int GetCurSelectedRowIdx();
	void SetCurSelectedRowIdx(int Idx);
	void SetCurSelectedRowIdxWithoutRowMove(int Idx);							// 091124 ShinJS --- List의 위치를 변경하지 않고 선택 아이템을 변경
	void ResetGuageBarPos();
	void SetLineHeight( LONG height );

	void SetAutoScroll( BOOL bAuto ) { m_bAutoScroll = bAuto; }
	int GetClickedRowIdx()	const		{ return m_ClickSelected ; }
	void SetClickedRowIdx( int row )	{ m_ClickSelected = row; }

	// 071128 웅주. 선택된 아이템 문자열 반환
	const char* GetClickedItem();
	int GetSelectRowIdx() const { return mSelectedRowIdx; }
	LONG GetVisibleLineSize() const { return m_lLineNum; }
	int GetTopListItemIdx()	const { return m_lTopListItemIdx; }
	void SetTopListItemIdx(int Idx);	

	cPtrList* GetListItem()		{ return &m_ListItem; }

	BOOL PtInWindow(LONG x, LONG y);

	void SetShowSelect(BOOL val);
	// 061209 LYW --- Setting to show scroll part.
	void SetShowScroll( BOOL val );
	BOOL IsShowScrool() { return m_bActive ? m_bShowScroll : FALSE; }
	// 061209 LYW --- Setting to disable scroll part.
	void SetDisableScroll() ;
	// 061209 LYW --- Setting style.
	void SetStyle( BYTE style, LONG height=10 ) { m_bStyle = style ; m_lLineHeight += height ; }
	// 061209 LYW --- Setting align text.
	void SetAlign( int align ) { m_nAlign = align ; }
	// 061225 LYW --- Add function to return upward button.
	cButton* GetUpwardBtn() { return m_pUpwardBtn ; }
	cButton* GetDownwardBtn() { return m_pDownwardBtn ; }
	// 061225 LYW --- Add unction to return guagebar.
	cGuageBar* GetGuageBar() { return m_pGuageBar ; }

	// 070620 LYW --- cListDialog : Add function to setting color for column of this dialog.
	void SetColumnColor(DWORD dwColor) { m_dwColumnColor = dwColor ; }
	void SetSelectColumnColor(DWORD dwColor) { m_dwSelectColumnColor = dwColor ; }

	// 070709 웅주, 리스트 위치를 조정(범위0~1)
	void SetPosition( float ratio );
	void SetTextRect( const RECT& );

	// 자동으로 줄바꿈해준다
	void AddItemAutoLine( const char*, DWORD color );

	// 071017 LYW --- cListDialog : Add function to setting and return font index to render shadow.
	void Set_ShadowFontIndex(WORD wIndex) { m_wShadowFont = wIndex ; }
	WORD Get_ShadowFontIndex() { return m_wShadowFont ; }

	//  071018 LYW --- cListDialog : Add function to setting and return shadow distance.
	void Set_ShadowDistance(int nDistance) { m_nShadowDistance = nDistance ; }
	int  Get_ShadowDistance() { return m_nShadowDistance ; }

	// 071128 웅주
	virtual void RemoveAll();

	// 090812 ShinJS --- 정렬 설정
	void SetSortType( BYTE bySortType ) { m_bySortType = bySortType; }
	BYTE GetSortType() const { return m_bySortType; }

	// 090827 ShinJS --- SortType 의 기준으로 추가될수 있는 위치 반환
	int UpperBoundString( int nLower, int nUpper, const char* str, eListItemSortType eSortType );		// m_ListItem에서 String 대소 비교로 인자보다 큰 String의 위치를 반환
	int UpperBoundData( DWORD dwSortData, eListItemSortType eSortType );								// m_ListItem에서 dwSortData 대소 비교로 인자보다 큰 dwSortData의 위치를 반환
	int GetLowerBoundOfEqualData( int nPos, DWORD dwSortData );											// dwSortData와 같은 값을 가진 처음 위치를 반환
	int GetUpperBoundOfEqualData( int nPos, DWORD dwSortData );											// dwSortData와 같은 값을 가진 마지막 위치를 반환

	void SetRemainOverImage( BOOL bVal ) { m_bRemainOverImage = bVal; }

	// 091201 ShinJS --- Text Rect에서 Width 값을 구한다.
	LONG GetTextWidth() const { return m_textRelRect.right - m_textRelRect.left; }

	void SetUseTopRowChangeEvent( BOOL bVal ) { m_bUseTopRowChangeEvent = bVal; }
	LONG GetLineHeight() const { return m_lLineHeight; }

protected:

	LONG m_lLineHeight;
	LONG m_lLineNum;
	LONG m_lTopListItemIdx;
	cImage m_TopImage;		WORD m_topHeight;
	cImage m_MiddleImage;	WORD m_middleHeight;
	cImage m_DownImage;		WORD m_downHeight;

	BYTE m_middleNum;
	BYTE m_minMiddleNum;
	BYTE m_maxMiddleNum;
	RECT m_textRelRect;						// only left,top,bottom field is used

	cButton * m_pUpwardBtn;
	cButton * m_pDownwardBtn;
	cButton * m_pLengthLongBtn;
	cButton * m_pLengthShortBtn;
	cWindow* m_pComboBtn;
	cGuageBar * m_pGuageBar;

	// 마우스 클릭에 대한 처리: 마우스 커서가 위치한 셀 번호
	int		mSelectedRowIdx;

	//int		mPressedRowIndex;

	int		m_ClickSelected;

	cImage			m_OverImage;
	cImageScale		m_OverImageScale;
	float			m_fGap;

	BOOL			m_bAutoScroll;
	BOOL			m_bShowSelect;

	// 061209 LYW --- Add boolean variable wether show scroll part or not.
	BOOL			m_bShowScroll ;

	// 061209 LYW --- Add new style for list dialog.
	BYTE			m_bStyle ;
	int				m_nAlign;

	// 070620 LYW --- cListDialog : Add columncolor for list dialog.
	DWORD			m_dwColumnColor ;
	DWORD			m_dwSelectColumnColor ;

	// list bar
	//cImage * m_pBarImage;
	//VECTOR2	m_barListPos;
	//cSize	m_barListSize;
	//int		m_interval;
	//BOOL	m_fBarDrag;
	//LONG	m_dragGabY;
	RECT			mTextRect;

	cPtrList		m_NoticeList ;

	// 071017 LYW --- cListDialog : Add font index to render shadow.
	WORD			m_wShadowFont ;

	// 071018 LYW --- cListDialog : Add shadow distance to use render shadowfont.
	int				m_nShadowDistance ;

	// 080225 LUJ, 툴팁용 아이콘을 보관한다. 키: 리스트 아이템 인덱스
	struct ToolTip
	{
		int nPos;
		cIcon*		mIcon;
	};
	typedef std::list< ToolTip >	ToolTipList;
	ToolTipList						mToolTipList;

	// 080225 LUJ, 렌더링할 툴팁
	cIcon*	mToolTipIcon;

	// 090820 ShinJS --- 정렬 추가
	BYTE			m_bySortType;

	// 091125 ShinJS --- 클릭Item에 대하여 마우스 OverImage Render 여부
	BOOL			m_bRemainOverImage;

	// 091209 ShinJS --- TopListItemIdx 변경 Event 사용 여부
	BOOL			m_bUseTopRowChangeEvent;
};