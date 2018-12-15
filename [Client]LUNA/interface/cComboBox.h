// cComboBox.h: interface for the cComboBox class.
// Lastest update : 2003.1.10. by taiyo
//////////////////////////////////////////////////////////////////////

#ifndef _cCOMBOBOX_H_
#define _cCOMBOBOX_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cWindow.h"
#include "cListItem.h"

class cPushupButton;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ¸®½ºÆ® ÀÌ¹ÌÁöÀÇ Å©±â(image height)ÇÏ³ª°¡ ÁÙÀÇ Å©±â(line height)¿Í µ¿ÀÏÇÏ´Ù´Â °¡Á¤  ( m_middleHeight )·Î ¸ðµç line height¸¦ °è»ê
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=+++++++++++++++++++++++++++++++++++++++++

class cComboBox : public cWindow , public cListItem
{
public:
	cComboBox();
	virtual ~cComboBox();

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, cbFUNC Func, LONG ID=0);
	virtual void InitComboList(WORD wid, cImage * topImage, WORD topHei, cImage * middleImage, WORD middleHei, cImage * downImage, WORD downHei, cImage * overImage );
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void Render();
	virtual void Add(cWindow * pushupBtn);
	virtual void SetAbsXY(LONG x, LONG y);
	virtual void ListMouseCheck( CMouse * mouseInfo );
	void SetComboTextColor(DWORD color){ m_ComboTextColor = color; }
	void SetOverImageScale(float x, float y) { m_OverImageScale.x = x; m_OverImageScale.y = y; }
	void SetMargin(RECT textClippingRect);
	WORD PtIdxInComboList(LONG x, LONG y);
	const char* GetComboText() const { return m_szComboText; }
	void SelectComboText(WORD idx)			// [5/14/2003] : ÃÊ±â°ª ¼±ÅÃÇÏ´Â ÇÔ¼ö Ãß°¡
	{
		if(idx < GetItemCount())
		{
			PTRLISTPOS p = m_ListItem.FindIndex(idx);
			ITEM * item = (ITEM*)m_ListItem.GetAt(p);
			strcpy(m_szComboText, item->string);	
		}
	}

	void DeleteComboText() { ZeroMemory(m_szComboText, sizeof(m_szComboText)) ; }

	int GetCurSelectedIdx() { return m_nCurSelectedIdx; }
	void SetCurSelectedIdx(int idx) { m_nCurSelectedIdx = idx ; }
	LONG GetItemCount() const { return m_ListItem.GetCount(); }
	void ForceListFold();
	
	void InsertComboItem( char* pItemText );
		
protected:

	cPushupButton * m_pComboBtn;
	char m_szComboText[MAX_COMBOTEXT_SIZE];
	DWORD m_ComboTextColor;
	
	cImage m_TopImage;			WORD m_topHeight;
	cImage m_MiddleImage;		WORD m_middleHeight;
	cImage m_DownImage;			WORD m_downHeight;

	cImage m_OverImage;
	cImageScale m_OverImageScale;
	int	m_nOverIdx;

	WORD m_listWidth;
	RECT m_textClippingRect;
	
	int	m_nCurSelectedIdx;
	BOOL m_bComboBoxSelected;
};

#endif // _cCOMBOBOX_H_
