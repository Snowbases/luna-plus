#include "StdAfx.h"
#include "cFont.h"
#include "../Engine/GraphicEngine.h"
#include "../4DyuchiGRX_common/IGeometry.h"

extern HWND _g_hWnd;

GLOBALTON(cFont);
cFont::cFont()
{
	ZeroMemory(
		m_pFont,
		sizeof(m_pFont));
	ZeroMemory(
		m_hFont,
		sizeof(m_hFont));
}

cFont::~cFont()
{
	Release();
}
void cFont::Release()
{
	for(int i=0;i<FONTMAX;i++)
	{
		SAFE_RELEASE(m_pFont[i]);
//KES 030915
		if( m_hFont[i] )
			DeleteObject( m_hFont[i] );
	}
}

BOOL cFont::CreateFontObject(LOGFONT * pFont, WORD idx)
{
	if(FONTMAX < idx)
	{
		return FALSE;
	}
	
	m_pFont[idx] = g_pExecutive->GetRenderer()->CreateFontObject(pFont, D3DX_FONT);//SS3D_FONT);//D3DX_FONT);
	m_hFont[idx] = CreateFontIndirect( pFont );

	HDC hdc = GetDC( _g_hWnd );
	HFONT OldFont = (HFONT)SelectObject( hdc, m_hFont[idx] );

	SIZE size1,height;
	GetTextExtentPoint32( hdc, "W", 1, &size1 );
	GetTextExtentPoint32( hdc, "Gg", 2, &height );
	
	SelectObject( hdc, OldFont );

	ReleaseDC( _g_hWnd, hdc );

	m_FontDesc[idx].wHeight = (WORD)height.cy;	
	m_FontDesc[idx].wWidth = (WORD)size1.cx;	

	return TRUE;
}

LONG cFont::GetTextExtent(WORD hFIdx, char * str, LONG strSize)
{
	HDC hdc = GetDC( _g_hWnd );
	HFONT OldFont = (HFONT)SelectObject( hdc, m_hFont[hFIdx] );

	SIZE size;
	GetTextExtentPoint32( hdc, str, strSize, &size );

	SelectObject( hdc, OldFont );

	ReleaseDC( _g_hWnd, hdc );

	return size.cx;
}


LONG cFont::GetTextExtentEx( WORD hFIdx, char* str, int strLen )
{
	HDC hdc = GetDC( _g_hWnd );
	HFONT OldFont = (HFONT)SelectObject( hdc, m_hFont[hFIdx] );

	SIZE size;
	GetTextExtentPoint32( hdc, str, strLen, &size );

	SelectObject( hdc, OldFont );

	ReleaseDC( _g_hWnd, hdc );

	return size.cx;
}

// 070624 LYW --- cFont : Add function to return text extent.
// 현재 자간사이를 계산하지 않고 리턴하는 함수를 계산하는 함수로 처리.
LONG cFont::GetTextExtentWidth( WORD hFIdx, char* str, int strLen )
{
	HDC hdc = GetDC(_g_hWnd) ;

	HFONT OldFont = (HFONT)SelectObject( hdc, m_hFont[hFIdx] ) ;

	SIZE size ;
	GetTextExtentPoint32( hdc, str, strLen, &size ) ;

	SelectObject( hdc, OldFont ) ;

	ReleaseDC( _g_hWnd, hdc ) ;

	return size.cx ;
}


void cFont::RenderFont(WORD wFontIdx, char * text, int size, RECT * pRect, DWORD color)
{
	if(FALSE == IsCreateFont(wFontIdx))
	{
		return;
	}

	//KES가 수정했음 040722
	if( wFontIdx == 5 )
	{
		pRect->right = pRect->left + GetTextExtentWidth( wFontIdx, text, size ) + GetTextWidth( wFontIdx ) ;
	}
	else
	{
		pRect->right = pRect->left + GetTextExtent( wFontIdx, text, size ) + GetTextWidth( wFontIdx );
	}

	pRect->bottom = pRect->top + GetTextHeight( wFontIdx ) + 5;	//+1안하면 밑줄이 안나온다.

	if(*text) 
	{
		DWORD dwColor = 0xff000000 | color;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,dwColor,CHAR_CODE_TYPE_ASCII,0,0);
	}

}

void cFont::RenderFontShadow(WORD wFontIdx, int nShadowDistance, char* text, int size, RECT* pRect, DWORD dwColor)
{
	if(FALSE == IsCreateFont(wFontIdx))
	{
		return;
	}

	if( wFontIdx == 5 )
	{
		pRect->right = pRect->left + GetTextExtentWidth( wFontIdx, text, size ) + GetTextWidth( wFontIdx ) ;
	}
	else
	{
		pRect->right = pRect->left + GetTextExtent( wFontIdx, text, size ) + GetTextWidth( wFontIdx );
	}

	pRect->bottom = pRect->top + GetTextHeight( wFontIdx ) + 5;	

	if(*text) 
	{
		// render shadow text.
		DWORD dwShadowColor = 0xff000000 | RGB(10, 10, 10) ;
		RECT shadowRect = { pRect->left + nShadowDistance, pRect->top + nShadowDistance, pRect->right, pRect->bottom } ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,&shadowRect,dwShadowColor,CHAR_CODE_TYPE_ASCII,0,0);

		// render front text.
		DWORD dwFrontColor = 0xff000000 | dwColor ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,dwFrontColor,CHAR_CODE_TYPE_ASCII,0,0);
	}
}

// 071025 LYW --- Add function to render shadowfont to alpha.
void cFont::RenderFontShadowAlpha(WORD wFontIdx, int nDistance, char* text, int size, RECT* pRect, DWORD dwTopColor, DWORD dwShadowColor ,DWORD dwAlpha)
{
	RenderFontAlpha(
		wFontIdx,
		text,
		size,
		pRect,
		RGBA_MERGE(dwShadowColor, dwAlpha));

	RECT rect = *pRect;
	rect.left += nDistance;
	rect.top += nDistance;

	RenderFontAlpha(
		wFontIdx,
		text,
		size,
		&rect,
		RGBA_MERGE(dwTopColor, dwAlpha));
}

void cFont::RenderFontAlpha(WORD wFontIdx, char * text, int size, RECT * pRect, DWORD color)
{
	if(FALSE == IsCreateFont(wFontIdx))
	{
		return;
	}

	pRect->right = pRect->left + GetTextExtent( wFontIdx, text, size ) + GetTextWidth( wFontIdx ) / 4;
	pRect->bottom = pRect->top + GetTextHeight( wFontIdx ) + 1;	//+1안하면 밑줄이 안나온다.

	if(*text)
	{
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,color,CHAR_CODE_TYPE_ASCII,0,0);
	}
}

void cFont::RenderNoticeMsg(WORD wFontIdx, char * text, int size, RECT * pRect, DWORD frontColor, DWORD backColor)
{
	if( FALSE == IsCreateFont(wFontIdx) )
	{
		return;
	}

	pRect->right = pRect->left + GetTextExtent( wFontIdx, text, size ) + GetTextWidth( wFontIdx );
	pRect->bottom = pRect->top + GetTextHeight( wFontIdx ) + 5;
	
	if(*text) 
	{
		pRect->left -= 1 ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,backColor,CHAR_CODE_TYPE_ASCII,0,0);

		pRect->left += 2 ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,backColor,CHAR_CODE_TYPE_ASCII,0,0);

		pRect->left -= 1 ;

		pRect->top -= 1 ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,backColor,CHAR_CODE_TYPE_ASCII,0,0);

		pRect->top += 2 ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,backColor,CHAR_CODE_TYPE_ASCII,0,0);

		pRect->top -= 1 ;
		g_pExecutive->GetGeometry()->RenderFont(m_pFont[wFontIdx],text,size,pRect,frontColor,CHAR_CODE_TYPE_ASCII,0,0);
	}
}

BOOL cFont::IsCreateFont(WORD idx) const
{
	if(FONTMAX < idx)
	{
		return FALSE;
	}

	return 0 < m_pFont[idx];
}