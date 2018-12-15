// cMultiLineText.h: interface for the cMultiLineText class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _cMULTILINETEXT_H_
#define _cMULTILINETEXT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cImage.h"


#define TOOLTIP_MOUSESIZE_X	34
#define TOOLTIP_MOUSESIZE_Y	28


class LINE_NODE
{
public:
	LINE_NODE()
	{
		memset(line, 0, 256);
		nextLine	= NULL;
		len			= 0;
		color		= 0xffffffff;
		nextPatial = NULL;
	}
	~LINE_NODE(){}

	DWORD	color;
	char	line[256];
	DWORD	len;
	LINE_NODE * nextLine;
	// 091012 pdy 한줄에 여러개 ADD되는 툴팁 추가  
	LINE_NODE * nextPatial;
};

/*
struct LINE_NODE
{
	
	char		strLine[256];
	DWORD		dwLen;
	LINE_NODE*	next;
};
*/

class cMultiLineText  
{
public:
	cMultiLineText();
	virtual ~cMultiLineText();
	void Init(WORD fontIdx, DWORD fgColor, cImage * bgImage=NULL, DWORD imgColor = 0xffffffff);

	void Release();
	void Render();
	
	BOOL IsValid() { return m_fValid; }
	void SetText( const char* );
	virtual BOOL SetText( VECTOR3*, const char* );

	void AddLine( const char* text, DWORD dwColor = 0xffffffff );	
	void AddNamePannel( DWORD dwLength );

	void SetFontIdx(WORD fontIdx){ m_wFontIdx = fontIdx; }
	// 070209 LYW --- cMultiLineText : Modified function SetXY().
	//void SetXY(LONG x, LONG y){	m_m_leftTopPos.left = x; m_m_leftTopPos.top = y; }
	void SetXY(LONG x, LONG y){	m_m_leftTopPos.left = x; m_m_leftTopPos.top = y; m_bMovedText = TRUE; }
	void operator=( const char* text){ SetText(text); }

//	BOOL IsSurvive(){ return m_fSurvive; }

	void SetImageRGB( DWORD color )				{ m_imgColor = color; }
	void SetImageAlpha( DWORD dwAlpha )			{ m_alpha = dwAlpha; }
	void SetOptionAlpha( DWORD dwOptionAlpha )	{ m_dwOptionAlpha = dwOptionAlpha; }

	// 070209 LYW --- cMultiLineText : Add function to setting area of render part.
	void SetRenderArea(VECTOR2* scaleRate, VECTOR2* Pos) ;

	// 070328 LYW --- cMultiLineText : Add function to setting whether apply alpha or not.
	void SetTextAlpha( BOOL val ) { m_bTextAlpha = val ; }

	// 091012 pdy 한줄에 여러개 ADD되는 툴팁 추가  
	void AddPatialLine(const char* text, DWORD dwColor,BOOL bNewLine );

private:
	LINE_NODE* topLine;

//	BOOL	m_fSurvive;
	BOOL	m_fValid;
	WORD	m_wFontIdx;
	cImage  m_bgImage;
	DWORD	m_fgColor;
	DWORD	m_imgColor;

	RECT	m_m_leftTopPos;
	int		m_line_idx;
	int		m_max_line_width;


//KES 030826  세팅 함수도 만들어야 함
	DWORD	m_alpha;
	DWORD	m_dwOptionAlpha;
//
	
	// 공성전 이름판넬
	BOOL	m_bNamePannel;

	// 070209 LYW --- cMultiLineText : Add variable.
	BOOL	m_bMovedText ;
	RECT	m_renderRect ;

	// 070328 LYW --- cMultiLineText : Add member variable for render text to alpha.
	BOOL	m_bTextAlpha ;
};

#endif // _cMULTILINETEXT_H_

