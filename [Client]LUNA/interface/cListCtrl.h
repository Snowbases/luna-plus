// cListCtrl.h: interface for the cListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _cLISTCTRL_H_
#define _cLISTCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cWindow.h"
#include "cReportItem.h"


class cListCtrl : public cWindow, public cReportItem
{
public:
	cListCtrl();
	virtual ~cListCtrl();

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID);
	virtual void InitListCtrl(WORD wMaxColumns, WORD wLinePerPage);
	virtual void InitListCtrlImage(cImage * HeadImage, BYTE HHeight, cImage * BodyImage, BYTE BHeight, cImage * OverImage );
	virtual void Render();
	virtual DWORD ActionEvent(CMouse * mouseInfo);

	void SetMargin(const RECT * textRelRect);
	WORD PtIdxInRow(LONG x, LONG y);
	int GetSelectedRowIdx(){ return m_SelectedRowIdx; }
	void SetSelectedRowIdx( int Idx )		{	m_SelectedRowIdx = Idx;	}

	void SetOverImageScale( float x, float y ) { m_OverImageScale.x = x; m_OverImageScale.y = y; }
	void SetSelectOption( WORD wOption ) { m_wSelectOption = wOption; }
	virtual void Add( cWindow* );

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.14
	// ..리스트에 추가될 ROW의 텍스트를 스크립트에서 InterfaceMsg ID 로 설정할 수 있도록 추가
	void SetRowsMsgID(int nRow, int nMsg)		{ m_nRowsMsgID[nRow] = nMsg; }
	int  GetRowsMsgID(int nRow)					{ return m_nRowsMsgID[nRow]; }

	int	 GetRowsNum()							{ return m_wLinePerPage; }
	// E 데이트 존 추가 added by hseos 2007.11.14

	// 080325 LYW --- cListCtrl : Add a function to return m_bHeadLineHeight.
	BYTE GetHeadLineHeight() { return m_bHeadLineHeight ; }

	// 080326 LYW --- cListCtrl : Add a function to setting start index of printed items.
	void SetStartIdxOfPrintedItems(WORD wIdx) { m_wStartIdxOfPrintedItems = wIdx ; }

	// 080326 LYW --- cListCtrl : Add a function to return start index of printed items.
	WORD GetStartIdxOfPrintedItems() { return m_wStartIdxOfPrintedItems ; }

protected:

	cImage m_HeadImage;
	cImage m_BodyImage;
	cImage m_OverImage;

	cImageScale		m_OverImageScale;

	BYTE m_bHeadLineHeight;
	BYTE m_bBodyLineHeight;

	WORD m_wLinePerPage;
	RECT m_textRelRect;				// right, bottom field is not used

	int m_SelectedRowIdx;					//현재 선택된 RITEM의 dwData값을 저장
	LONG m_lTopListItemIdx;

	int	m_nOverIdx;
	float m_fGap;
	
	WORD	m_wSelectOption;	// 오버 이미지를 사용할때 옵션값이다. ( 0: 기본, 1: 선택시 )

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.14
	// ..리스트에 추가될 ROW의 텍스트를 스크립트에서 InterfaceMsg ID 로 설정할 수 있도록 추가
	int	m_nRowsMsgID[10];
	// E 데이트 존 추가 added by hseos 2007.11.14

	std::list< cWindow* > m_childList;

	// 080326 LYW --- cListCtrl : Add a variable to contain the start index of printed items.
	// 리스트 컨트롤에 휠 스크롤 기능을 추가하면서, 리스트 상에 출력되는 첫 아이템의 인덱스를
	// 담고 있는 변수를 하나 추가함.
	WORD m_wStartIdxOfPrintedItems ;
};


#endif // _cLISTCTRL_H_
