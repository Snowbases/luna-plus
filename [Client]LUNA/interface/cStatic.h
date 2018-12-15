//////////////////////////////////////////////////////////////////////
// class name : cStatic 
// Lastest update : 2002.10.29. by taiyo
//////////////////////////////////////////////////////////////////////

#ifndef _cSTATIC_H_
#define _cSTATIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cWindow.h"

// 070412 LYW --- cStatic : Define count of extend color.
#define MAX_EXTENDCOLOR	10

class cStatic : public cWindow  
{
public:
	cStatic();
	virtual ~cStatic();

	// 070412 LYW --- cStatic : Add Copy Constructor.
	cStatic( const cStatic &_static ) ;

	virtual void Render();
	void SetFontIdx(WORD fontIdx);

	void SetStaticText( const char* );

	LONG GetStaticValue()	const		{	return atoi(m_szStaticText);	}
	const char* GetStaticText()			{	return m_szStaticText;				}
	void SetStaticValue(LONG text)		{	itoa(text, m_szStaticText, 10);		}
	void SetMultiLine(BOOL val=TRUE)	{	m_fMultiLine = val;					}
	void InitMultiLine();
	void SetTextXY(LONG x, LONG y)		{	m_textXY.x = x; m_textXY.y = y;		}
	void SetFGColor(DWORD color)		{	m_fgColor = color;					}


	void SetShadowTextXY(LONG x, LONG y)	{	m_shadowTextXY.x = x; m_shadowTextXY.y = y;		}
	void SetShadowColor(DWORD color)		{	m_shadowColor = color;							}
	void SetShadow(BOOL val)				{	m_bShadow = val;								}

//KES INTERFACE 031028	
	void SetAlign( int nAlign )				{ m_nAlign = nAlign; }
	// 070209 LYW --- cStatic : Add function to setting area of render part.
	void SetRenderArea() ;

	// 070328 LYW --- cStatic : Add function to setting whether apply alpha or not.
	void SetTextAlpha( BOOL val ) { m_bTextAlpha = val ; }
	void Add( cWindow* );
//
protected:
	cCoord			m_textXY;
	cMultiLineText	m_StaticText;
	char			m_szStaticText[MAX_TEXT_SIZE];

	BOOL			m_fMultiLine;
	DWORD			m_fgColor;
	
	BOOL			m_bShadow;
	cCoord			m_shadowTextXY;
	DWORD			m_shadowColor;

//KES INTERFACE 031028	
	int				m_nAlign;
//
	// 070328 LYW --- cStatic : Add member variable for text render to alpha.
	BOOL	m_bTextAlpha ;

////////////////////////////////////////////////////////////////////////////////

	// 070412 LYW --- cStatic : Add extend text color.
protected :
	BOOL	m_bHasExtendColor ;

	BYTE	m_byExtendColorCount ;

	DWORD*	m_pExtendColor ;

public :
	void SetHasExtendColor( BOOL val ) { m_bHasExtendColor = val ; }
	BOOL IsHasExtendColor() { return m_bHasExtendColor ; }

	void SetExtendColorCount( BYTE byCount ) { m_byExtendColorCount = byCount ; }
	BYTE GetExtendColorCount() { return m_byExtendColorCount ; }

	void SetExtendColorPointer( DWORD* pExtendColor ) { m_pExtendColor = pExtendColor ; }
	DWORD* GetExtendColorPointer() { return m_pExtendColor ; }

	void SetExtendColor( BYTE idx, DWORD dwColor ) ;
	DWORD GetExtendColor( BYTE idx ) ;

	void InitializeExtendColor() ;
};

#endif // _cSTATIC_H_
