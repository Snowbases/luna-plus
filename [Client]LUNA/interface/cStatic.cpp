// cStatic.cpp: implementation of the cStatic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cStatic.h"
#include "cFont.h"
#include "MHFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


cStatic::cStatic()
{
	m_type = WT_STATIC;
	m_fMultiLine = FALSE;
	*m_szStaticText = 0;
	m_textXY.x = 0;
	m_textXY.y = 0;
	m_fgColor = RGB_HALF(255,255,255);
	m_bShadow = FALSE;
	m_shadowTextXY.x = 1;
	m_shadowTextXY.y = 1;
	m_shadowColor = RGB_HALF(10,10,10);

	// 070328 LYW --- cStatic : Initialize m_bTextAlpha.
	m_bTextAlpha = FALSE ;

	// 070412 LYW --- cStatic : Add extend text color.
	m_bHasExtendColor	 = FALSE ;
	m_byExtendColorCount = 0 ;
	m_pExtendColor		 = NULL ;
}

// 070412 LYW --- cStatic : Add Copy Constructor.
cStatic::cStatic( const cStatic &_static )
{
	if( _static.m_pExtendColor != NULL )
	{
		m_pExtendColor = new DWORD[_static.m_byExtendColorCount] ;

		if( !m_pExtendColor )
		{
			char buff[256];
			sprintf(buff, "Failed!! - Create CopyConstructor.");
			LOGEX(buff, PT_MESSAGEBOX);
		}

		for( BYTE count = 0 ; count < _static.m_byExtendColorCount ; ++count )
		{
			m_pExtendColor[count] = _static.m_pExtendColor[count] ;
		}
	}
}

cStatic::~cStatic()
{
	m_StaticText.Release();

	// 070412 LYW --- cStatic : Add extend color,
	if( m_pExtendColor )
	{
		delete [] m_pExtendColor ;

		m_pExtendColor = NULL ;
	}
}
void cStatic::InitMultiLine()
{
	if(m_fMultiLine)
	{
		m_StaticText.Init(m_wFontIdx, m_fgColor, NULL);
	}
}

void cStatic::SetFontIdx(WORD fontIdx)
{ 
	m_wFontIdx = fontIdx;
	m_StaticText.SetFontIdx(fontIdx);
}

void cStatic::Render()
{
	cWindow::Render();

	if(!m_fMultiLine)
	{	
		if(*m_szStaticText != 0)
		{
			// 070209 LYW --- cStatic : Modified render part.
			if( IsMovedWnd() )
			{
				SetRenderArea() ;
				SetMovedWnd( FALSE ) ;
			}

			if( m_bShadow )
			{
				// 070328 LYW --- cStatic : Modified render part.
				/*
				CFONT_OBJ->RenderFont(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderShadowRect,RGBA_MERGE(m_shadowColor, m_alpha * m_dwOptionAlpha / 100));
							*/
				// 070501 LYW --- cStatic : Setting render area.
				SetRenderArea() ;
				if(m_bTextAlpha)
				{
					CFONT_OBJ->RenderFontAlpha(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderRect,RGBA_MERGE(m_fgColor, m_alpha * m_dwOptionAlpha / 100));
				}
				else
				{
					CFONT_OBJ->RenderFont(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderShadowRect,RGBA_MERGE(m_shadowColor, m_alpha * m_dwOptionAlpha / 100));
				}
			}

			//foreground color
			// 070328 LYW --- cStatic : Modified render part.
			/*
			CFONT_OBJ->RenderFont(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderRect,RGBA_MERGE(m_fgColor, m_alpha * m_dwOptionAlpha / 100));
							*/
				// 070501 LYW --- cStatic : Setting render area.
				SetRenderArea() ;
				if(m_bTextAlpha)
				{
					CFONT_OBJ->RenderFontAlpha(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderRect,RGBA_MERGE(m_fgColor, m_alpha * m_dwOptionAlpha / 100));
				}
				else
				{
					CFONT_OBJ->RenderFont(m_wFontIdx,m_szStaticText,lstrlen(m_szStaticText),
							&m_renderRect,RGBA_MERGE(m_fgColor, m_alpha * m_dwOptionAlpha / 100));
				}
		}
	}
	else
	{
		if(m_StaticText.IsValid())
		{
			if(CFONT_OBJ->IsCreateFont(m_wFontIdx))
			{
				m_StaticText.SetXY((LONG)m_absPos.x+m_textXY.x, (LONG)m_absPos.y+m_textXY.y);

				// 070501 LYW --- cStatic : Setting render area.
				SetRenderArea() ;

				// 070328 LYW --- cStatic : Modified render part of multiLine.
				//m_StaticText.Render();
				if( m_bTextAlpha )
				{
					m_StaticText.SetTextAlpha( TRUE ) ;
					m_StaticText.Render() ;
				}
				else
				{
					m_StaticText.Render();
				}
			}
		}
	}
}

// 070209 LYW --- cStatic : Add function to setting area of render part.
void cStatic::SetRenderArea()
{
	LONG TxtPosX;

	if( m_nAlign & TXT_LEFT )
	{
		TxtPosX = (LONG)m_absPos.x + m_textXY.x;
	}
	else if( m_nAlign & TXT_RIGHT )
	{
		TxtPosX = (LONG)m_absPos.x + m_width - m_textXY.x 
					- CFONT_OBJ->GetTextExtentEx( m_wFontIdx, m_szStaticText, strlen(m_szStaticText) );
	}
	else //TXT_CENTER
	{
		TxtPosX = (LONG)m_absPos.x + ( m_width
					- CFONT_OBJ->GetTextExtentEx( m_wFontIdx, m_szStaticText, strlen(m_szStaticText) ) ) / 2;
	}

	m_renderRect.left	= TxtPosX ;
	m_renderRect.top	= (LONG)m_absPos.y + m_textXY.y ;
	m_renderRect.right	= 1 ;
	m_renderRect.bottom	= 1 ;

	
	if( m_bShadow )
	{
		m_renderShadowRect = m_renderRect;
		OffsetRect( &m_renderShadowRect, m_shadowTextXY.x, m_shadowTextXY.y );
	}
}

void cStatic::SetStaticText( const char* text )
{
	if(!m_fMultiLine)
	{
		SafeStrCpy(
			m_szStaticText,
			text,
			sizeof(m_szStaticText) / sizeof(*m_szStaticText));
	}
	else
	{
		m_StaticText = text;
	}

}

/////////////////////////////////////////////////////////////////////////////

// 070412 LYW --- cStatic : Add extend text color.

void cStatic::SetExtendColor( BYTE idx, DWORD dwColor )
{
	if( m_pExtendColor )
	{
		m_pExtendColor[idx] = dwColor ;
	}
}

DWORD cStatic::GetExtendColor( BYTE idx )
{
	if( m_pExtendColor )
	{
		return m_pExtendColor[idx] ;
	}

	return 0 ;
}

void cStatic::InitializeExtendColor()
{
	if( m_byExtendColorCount > 0 )
	{
		m_pExtendColor = new DWORD[m_byExtendColorCount] ;
	}
}


void cStatic::Add( cWindow* )
{
	ASSERT( 0 && "It never contain any child control" );
}