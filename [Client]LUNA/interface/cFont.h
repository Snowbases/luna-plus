#pragma once

interface IDIFontObject;

#define CFONT_OBJ USINGTON(cFont)

struct FONT_DESC
{
	WORD wWidth;
	WORD wHeight;
};

class cFont  
{
public:
	enum FONT_KIND
	{
		FONT0,
		FONT1,
		FONT2,
		FONT3,
		FONT4,
		FONT5,
		FONT6,
		FONT7,
		FONT8,
		FONTMAX,
	};

	cFont();
	virtual ~cFont();
	void Release();
	BOOL CreateFontObject(LOGFONT *pFont, WORD idx);
	void RenderFont(WORD wFontIdx, char * text, int size, RECT * pRect, DWORD color);
	void RenderFontAlpha(WORD wFontIdx, char * text, int size, RECT * pRect, DWORD color);
	BOOL IsCreateFont(WORD idx) const;
	LONG GetTextExtent(WORD hFIdx, char *str, LONG strSize);
	LONG GetTextHeight(WORD hFIdx) { return m_FontDesc[hFIdx].wHeight; }
	LONG GetTextWidth(WORD hFIdx) { return m_FontDesc[hFIdx].wWidth; }
	LONG GetTextExtentEx( WORD hFIdx, char* str, int strLen );
	// 070624 LYW --- cFont : Add function to return text extent.
	// 현재 자간사이를 계산하지 않고 리턴하는 함수를 계산하는 함수로 처리.
	LONG GetTextExtentWidth( WORD hFIdx, char* str, int strLen ) ;

	void RenderFontShadow(WORD wFontIdx, int nShadowDistance, char*, int size, RECT*, DWORD dwColor) ;
	void RenderFontShadowAlpha(WORD wFontIdx, int nDistance, char*, int size, RECT*, DWORD dwTopColor, DWORD dwShadowColor ,DWORD dwAlpha) ;
	void RenderNoticeMsg(WORD wFontIdx, char* text, int size, RECT*, DWORD frontColor, DWORD backColor) ;

private:
	IDIFontObject * m_pFont[FONTMAX];
	FONT_DESC m_FontDesc[FONTMAX];
	HFONT m_hFont[FONTMAX];
};

EXTERNGLOBALTON(cFont);