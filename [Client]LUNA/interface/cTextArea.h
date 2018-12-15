#pragma once


#include "cDialog.h"

class cIMEex;
class cGuageBar;


class cTextArea : public cDialog
{

protected:

	cButton*	m_pUpwardBtn;
	cButton*	m_pDownwardBtn;
	cGuageBar*	m_pGuageBar;

	cIMEex*		m_pIMEex;

	int			m_nTopLineIdx;
	int			m_nLineNum;
	int			m_nLineHeight;
	RECT		m_rcTextRelRect;

	BOOL		m_bReadOnly;
	
	BOOL		m_bCaret;

	DWORD		m_dwCaretCurTick;
	DWORD		m_dwCaretLastTick;
	
	cImage m_TopImage;			WORD m_topHeight;
	cImage m_MiddleImage;		WORD m_middleHeight;
	cImage m_DownImage;			WORD m_downHeight;

	cImageScale m_MiddleScale;

	BOOL		m_bUseBorderImage;

	// 061227 LYW --- Add color value for font of this textarea control.
	DWORD		m_dwFontColor ;

public:

	cTextArea();
	virtual ~cTextArea();

	void InitTextArea( RECT* pTextRelRect, int nBufSize, cImage* topimg, WORD tophei, cImage* middleimg, WORD middlehei, cImage* downimg, WORD downhei );
	void InitTextArea( RECT* pTextRelRect, int nBufSize );
	virtual DWORD ActionEvent(CMouse * mouseInfo);
	virtual void Render();
	virtual void Add(cWindow * window);
	
	virtual void SetActive( BOOL val );

	virtual void SetFocusEdit(BOOL val);
	void SetFocus( BOOL val );
	void SetMiddleScale( cImageScale scale ) { m_MiddleScale = scale; }
	
	void OnUpwardItem();
	void OnDownwardItem();
	
	void GetScriptText(char * outText);
	void SetScriptText( const char * inText);

	void SetReadOnly(BOOL val) { m_bReadOnly = val;	}
	
	BOOL SetLimitLine( int nMaxLine );
	
	void SetCaretMoveFirst();

	void SetEnterAllow( BOOL bAllow );
	
//	void SetValidCheck( int nSetNum );

	void GetCaretPos( LONG* X, LONG* Y );

	// 061227 LYW --- Add function to setting or return font color.
	void SetFontColor( DWORD color ) { m_dwFontColor = color ; }
	DWORD GetFontColor() { return m_dwFontColor ; }

	// desc_hseos_메세지박스01
	// S 메세지박스 추가 added by hseos 2007.10.16
	cIMEex* GetIMEex()	{ return m_pIMEex; }
	// E 메세지박스 추가 added by hseos 2007.10.16

	void SetLineHeight( int height ) { m_nLineHeight = height; }
};