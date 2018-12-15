// cWindow.cpp: implementation of the cWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cWindow.h"
#include "cFont.h"
#include "cWindowManager.h"
#include "../Input/Mouse.h"
#include "MouseCursor.h"
#include "MHAudioManager.h"

void defaultWindowFunc(LONG lId, void * p, DWORD we){}

cWindow::cWindow()
{
	m_ID = 0;
	m_wFontIdx=0;
	m_type=WT_WINDOW;
	m_bActive=TRUE;
	m_bHide = FALSE;		//090119 pdy hide window
	m_bDisable = FALSE;
	m_bFocus=TRUE;
	m_bMovable=FALSE;
	m_width=0;
	m_height=0;
	m_pCurImage=NULL;
	m_pToolTipImage=NULL;
	m_bDepend=FALSE;
	m_pParent=NULL;
	m_dwViewCurTime=m_dwViewLastTime=0;
	mOpenSound = 0;
	mCloseSound = 0;
	m_dwImageRGB = 0xffffff;
	m_alpha = 255;
	m_dwOptionAlpha = 100;
	m_validPos.x = 0;
	m_validPos.y = 0;
	cbWindowFunc=defaultWindowFunc;
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	m_bAlwaysTop = FALSE;	
	m_bDestroy = FALSE;
	m_bSetTopOnActive = TRUE;

	// 070202 LYW --- Add cPoint for display point.
	m_bCheckDispPoint = FALSE ;

	// 070209 LYW --- cWindow : Add RECT for render part.
	m_renderRect.left	= 0 ;
	m_renderRect.top	= 0 ;
	m_renderRect.right	= 0 ;
	m_renderRect.bottom	= 0 ;

	m_renderShadowRect = m_renderRect ;

	m_bIsMovedWnd = TRUE ;

	// desc_hseos_몬스터미터01
	// S 몬스터미터 추가 added by hseos 2007.05.30
	m_bOwnAlpha = FALSE;
	ZeroMemory(
		&m_vDeltaPos,
		sizeof(m_vDeltaPos));
	ZeroMemory(
		&m_vDispPoint,
		sizeof(m_vDispPoint));
	m_bSavePosition = FALSE;
}

cWindow::~cWindow()
{
	toolTip.Release();
}

// 070202 LYW --- Add function to save display position.
void cWindow::SaveDispPoint( VECTOR2 point1, VECTOR2 point2, VECTOR2 point3 )
{
	m_bCheckDispPoint = TRUE ;

	memcpy( &m_vDispPoint[0], &point1, sizeof(VECTOR2) ) ;
	memcpy( &m_vDispPoint[1], &point2, sizeof(VECTOR2) ) ;
	memcpy( &m_vDispPoint[2], &point3, sizeof(VECTOR2) ) ;
}

void cWindow::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * pBasicImage, LONG ID)
{
	if( m_bCheckDispPoint )
	{
		POINT standardPosition = {0};
		SIZE standardResolution = {0};
		SIZE currentResolution = {0};

		GetVariablePostion(
			standardResolution,
			standardPosition,
			currentResolution);
		SetAbsXY(
			standardPosition.x,
			standardPosition.y);

		// 091228 LUJ, CharMake.bin에서만 예외적으로 사용하고 있다
		//			이 창 자체가 변칙적으로 만들어져서 해상도에
		//			따라 조정하기 위해 사용되고 있다
		switch(currentResolution.cy)
		{
		case 600:
			{
				if(800 != currentResolution.cx)
				{
					break;
				}
				
				SetAbsXY(
					LONG(GetAbsX() + m_vDeltaPos.x),
					LONG(GetAbsY() + m_vDeltaPos.y));
				break;
			}
		case 768:
			{
				if(1024 != currentResolution.cx)
				{
					break;
				}

				SetAbsXY(
					LONG(GetAbsX() + m_vDeltaPos.x),
					LONG(GetAbsY() + m_vDeltaPos.y));
				break;
			}
		}

		SetRelXY(
			LONG(GetAbsX()),
			LONG(GetAbsY()));

		m_bCheckDispPoint = FALSE ;
	}
	else
	{
		SHORT_RECT rect = {0};
		WORD value = 0;
		g_pExecutive->GetRenderer()->GetClientRect(
			&rect,
			&value,
			&value);

		switch(rect.right)
		{
		case 800:
		case 1024:
			{
				x += (LONG)m_vDeltaPos.x;
				y += (LONG)m_vDeltaPos.y;
			}
			break;
		}
		SetAbsXY(x,y);
		SetRelXY(x,y);
		SetWH(wid,hei);
	}

	m_ID=ID;
	if(pBasicImage)
	{
		m_BasicImage = *pBasicImage;
		m_pCurImage = &m_BasicImage;
	}

	// 070209 LYW --- cWindow : Add RECT for render part.
	m_renderRect.left	= x ;
	m_renderRect.top	= y ;
	m_renderRect.right	= x + wid ;
	m_renderRect.bottom	= y + hei ;

	m_renderShadowRect = m_renderRect ;
}

BOOL cWindow::PtInWindow(LONG x, LONG y)
{
	if(m_absPos.x+m_validPos.x > x || m_absPos.y+m_validPos.y > y || m_absPos.x+m_validPos.x+m_width < x || m_absPos.y+m_validPos.y+m_height < y)
		return FALSE;
	else
		return TRUE;
}


DWORD cWindow::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive || m_bDestroy ) return we;

	if( !WINDOWMGR->IsMouseOverUsed() && PtInWindow(mouseInfo->GetMouseX(), mouseInfo->GetMouseY() ) )
	{
		//내가 드래그중인 윈도우면 over가 아니다. //다른윈도우에 오버로 무언가 처리해야 할때가 있다.(드래그엔드랍)
		if( !(WINDOWMGR->IsDragWindow() && (cWindow*)WINDOWMGR->GetDragDlg() == this ) )
		{
			we |= WE_MOUSEOVER;
			
			//툴팁이 있는지 먼저 검사...최적화때
			
			m_dwViewCurTime = GetTickCount();
			if(m_dwViewCurTime - m_dwViewLastTime > TOOLTIP_DELAY_TIME)
			{
				m_ttX = mouseInfo->GetMouseX();
				m_ttY = mouseInfo->GetMouseY();
			}
		}
		
		if( !WINDOWMGR->IsDragWindow() )
		{
			if( m_type & WT_ICON )
				CURSOR->SetCursor( eCURSOR_ITEMSELECT );
			else
				CURSOR->SetCursor( eCURSOR_DEFAULT );
		}
	}
	else
	{
		m_dwViewLastTime = m_dwViewCurTime = 0;
	}


	if( !WINDOWMGR->IsMouseDownUsed() )
	if( PtInWindow(mouseInfo->GetMouseEventX(), mouseInfo->GetMouseEventY() ) )
	{
		if( mouseInfo->LButtonDown() )
			we |= WE_LBTNCLICK;
		if( mouseInfo->RButtonDown() )
			we |= WE_RBTNCLICK;
		if( mouseInfo->LButtonDoubleClick() )
			we |= WE_LBTNDBLCLICK;
		if( mouseInfo->RButtonDoubleClick() )
			we |= WE_RBTNDBLCLICK;
	}

	return we;
}

void cWindow::ExcuteCBFunc( DWORD we )
{
	if( cbWindowFunc )
		(*cbWindowFunc)(m_ID, m_pParent, we);
}

void cWindow::Render()
{
	if( m_bActive )
	if( m_pCurImage )
	{
		DWORD alpha = m_alpha * m_dwOptionAlpha / 100;
		DWORD dwRGBA = RGBA_MERGE( m_dwImageRGB, alpha );
		
		m_pCurImage->RenderSprite( &m_scale, NULL, 0, &m_absPos, dwRGBA );

		ToolTipRender();
	}
}

void cWindow::ToolTipRender()
{
	if(toolTip.IsValid() && m_dwViewCurTime - m_dwViewLastTime > TOOLTIP_DELAY_TIME)
	{
		//여기서 윈도우 메니져에게 보낼까?
		WINDOWMGR->SetToolTipWindow( &toolTip );

		toolTip.SetXY(m_ttX+TOOLTIP_MOUSESIZE_X, m_ttY+TOOLTIP_MOUSESIZE_Y);
	}
}

void cWindow::SetToolTip( char* msg, DWORD color, cImage * image, DWORD imgColor )
{ 
	if( msg )
	{
		toolTip.Init( 0, color, image, imgColor );	//폰트 아이디는? 무조건 0인가
		toolTip.SetImageAlpha( TOOLTIP_ALPHA );	//200대충.. 조정하기.
		toolTip = msg;
	}
}


void cWindow::AddToolTipLine( const char* msg, DWORD color )
{
	toolTip.AddLine( msg, color );
}

// 091012 pdy 한줄에 여러개 ADD되는 툴팁 추가  
void cWindow::AddToolTipPartialLine( BOOL bNewLine , const char* msg, DWORD color)
{
	toolTip.AddPatialLine( msg , color, bNewLine ) ;
}

void cWindow::SetAutoLineToolTip( const char* text, DWORD color, cImage* image, DWORD width )
{
	SetToolTip( "", color, image );

	BYTE line[ MAX_TEXT_SIZE ];
	int length = _mbslen( ( BYTE* )text );

	for(	const BYTE* ch = ( BYTE* )text;
		0 < length;
		)
	{
		_mbsnbcpy( line, ch, width );

		line[ width ] = 0;

		AddToolTipLine( ( char* )line, color );

		const DWORD lineLength = _mbslen( line );

		for( DWORD i = 0; i < lineLength; ++i )
		{
			ch = _mbsinc( ch );
		}

		length -= lineLength;
	}
}

void cWindow::GetVariablePostion(SIZE& standardResolution, POINT& standardPosition, SIZE& currentResolution)
{
	const SIZE standardResolutionArray[] = {
		{800, 600},
		{1024, 768},
		{1280, 1024},
	};
	LONG minimumSpace = LONG_MAX;

	SHORT_RECT rect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&rect,
		&value,
		&value);

	currentResolution.cx = rect.right;
	currentResolution.cy = rect.bottom;

	// 091221 LUJ, 가장 비슷한 좌표를 선택한다
	for(int i = sizeof(standardResolutionArray) / sizeof(*standardResolutionArray);
		0 < i--;)
	{
		const SIZE& resolution = standardResolutionArray[i];
		const VECTOR2& position = m_vDispPoint[i];
		const VECTOR2 emptyPosition = {-1, -1};

		if(0 == memcmp(
			&position,
			&emptyPosition,
			sizeof(position)))
		{
			continue;
		}

		const LONG space = abs(resolution.cy - LONG(rect.bottom));

		if(minimumSpace < space)
		{
			continue;
		}

		minimumSpace = space;
		standardPosition.x = LONG(position.x);
		standardPosition.y = LONG(position.y);
		standardResolution = resolution;
	}

	// 091228 LUJ, 선택된 해상도가 표준 해상도보다 작을 경우. 비율만큼 줄인다
	if(standardResolution.cy > rect.right)
	{
		const float widthRatio = float(rect.bottom) / standardResolution.cx;
		const float heightRatio = float(rect.right) / standardResolution.cy;

		standardPosition.x = LONG(float(standardPosition.x) * widthRatio);
		standardPosition.y = LONG(float(standardPosition.y) * heightRatio);
		standardResolution.cx = rect.bottom;
		standardResolution.cy = rect.right;
	}
}

void cWindow::SetActive(BOOL isActive)
{
	if(IsActive() != isActive)
	{
		AUDIOMGR->Play(
			isActive ? mOpenSound : mCloseSound,
			0);
	}

	cObject::SetActive(
		isActive);
}