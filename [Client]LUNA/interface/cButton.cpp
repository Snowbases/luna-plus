// cButton.cpp: implementation of the cButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cButton.h"
#include "..\Audio\MHAudioManager.h"
#include "../Input/Mouse.h"
#include "cFont.h"
#include "ObjectManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"

cButton::cButton()
{
	*m_szText=0;
	m_fgBasicColor=RGB_HALF(255,255,255);
	m_fgOverColor=RGB_HALF(255,255,255);
	m_fgPressColor=RGB_HALF(255,255,255);
	m_fgCurColor=RGB_HALF(255,255,255);
	m_nCurTextLen = 0;
	m_type = WT_BUTTON;
	m_textXY.x = 5;
	m_textXY.y = 4;
	
	m_bClickInside	= FALSE;
	m_BtnTextAniX	= 1;
	m_BtnTextAniY	= 1;
	
	m_bSound = FALSE;
	m_bOSound = FALSE ;

	m_shadowTextXY.x = 1;
	m_shadowTextXY.y = 0;
	m_shadowColor = RGB_HALF(10,10,10);
	m_bShadow = FALSE;

	// 0701004 LYW --- cButton : Setting sound index of button.
	m_sndIdx  = CLICK_SOUND_IDX ;
	m_osndIdx = OVER_SOUND_IDX ;

	m_bPlayOverSound = FALSE ;

	m_BtnState = eBtn_State_None;
	m_BtnMouseEvent = eBtn_Mouse_Evt_None;

	m_bIsActiveAnimated = FALSE;
	m_FirstDrawAnimateImage = FALSE;

	m_dwAniImgFrameTime = 0;
	m_dwOverAniImgFrameTime = 0;
	m_dwPressAniImgFrameTime = 0;
	m_dwNextChangeFrameTime = 0;
	m_dwCurAniImgFrame = 0;

	m_validXYAniImage.x = m_validXYAniImage.y = 0.f;
	m_validXYOverAniImage.x = m_validXYOverAniImage.y = 0.f;
	m_validXYPressAniImage.x = m_validXYPressAniImage.y = 0.f;
}

cButton::~cButton()
{
	// 리소스 릴리즈는 리소스 메니저에서
	//SAFE_DELETE(m_pPressImage);
	//SAFE_DELETE(m_pOverImage);
}

void cButton::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, cImage * overImage, cImage * pressImage, cbFUNC Func, LONG ID)
{
	cWindow::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_BUTTON;

	if( overImage )		m_OverImage = *overImage;
	if( pressImage )	m_PressImage = *pressImage;

	if(Func != NULL)
		cbWindowFunc = Func;
}

void cButton::SetAnimationImageValidXY( const VECTOR2& validxy, const VECTOR2& validxyOver, const VECTOR2& validxyPress )
{
	m_validXYAniImage.x = validxy.x;
	m_validXYAniImage.y = validxy.y;
	m_validXYOverAniImage.x = validxyOver.x;
	m_validXYOverAniImage.y = validxyOver.y;
	m_validXYPressAniImage.x = validxyPress.x;
	m_validXYPressAniImage.y = validxyPress.y;
}

DWORD cButton::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we |= cWindow::ActionEvent(mouseInfo);
	
	if( m_bDisable )
	{
		m_bClickInside	= FALSE;
		m_pCurImage		= &m_BasicImage;
		m_fgCurColor	= m_fgBasicColor;
		return we;
	}
	

	LONG x			= mouseInfo->GetMouseX();
	LONG y			= mouseInfo->GetMouseY();

	BOOL LeftUp		= mouseInfo->LButtonUp();
	BOOL LeftPress	= mouseInfo->LButtonPressed();
	BOOL RightUp	= mouseInfo->RButtonUp();

	BOOL bMouseOver = FALSE ;

	if( PtInWindow( x, y ) )
	{
		switch( m_BtnMouseEvent )
		{
		case eBtn_Mouse_Evt_Use_Normal_Image:
			{
				bMouseOver = ! m_BasicImage.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;
		case eBtn_Mouse_Evt_Use_Over_Image:
			{
				bMouseOver = ! m_OverImage.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;
		case eBtn_Mouse_Evt_Use_Press_Image:
			{
				bMouseOver = ! m_PressImage.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;
		case eBtn_Mouse_Evt_Use_Normal_Ani_Image:
			{
				std::map< DWORD, cImage >::iterator aniImgIter = m_mapAniImage.find( m_dwCurAniImgFrame );
				if( aniImgIter == m_mapAniImage.end() )
					break;

				cImage& curFrameImg = aniImgIter->second;
				bMouseOver = ! curFrameImg.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;
		case eBtn_Mouse_Evt_Use_Over_Ani_Image:
			{
				std::map< DWORD, cImage >::iterator aniImgIter = m_mapOverAniImage.find( m_dwCurAniImgFrame );
				if( aniImgIter == m_mapOverAniImage.end() )
					break;

				cImage& curFrameImg = aniImgIter->second;
				bMouseOver = ! curFrameImg.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;
		case eBtn_Mouse_Evt_Use_Press_Ani_Image:
			{
				std::map< DWORD, cImage >::iterator aniImgIter = m_mapPressAniImage.find( m_dwCurAniImgFrame );
				if( aniImgIter == m_mapPressAniImage.end() )
					break;

				cImage& curFrameImg = aniImgIter->second;
				bMouseOver = ! curFrameImg.HasAlpha( (LONG)(x - m_absPos.x), (LONG)(y - m_absPos.y) );
			}
			break;

		default:
			{
				bMouseOver = TRUE;
			}
			break;
		}
	}

	if( bMouseOver == TRUE &&
		(we & ( WE_LBTNCLICK | WE_LBTNDBLCLICK | WE_RBTNCLICK | WE_RBTNDBLCLICK)) )
	{
		m_bClickInside	= TRUE;
	}
	
	if( LeftUp | RightUp )
	{
		if( m_bClickInside && bMouseOver )
		{
			AUDIOMGR->Play(
				m_sndIdx,
				gHeroID);

			if( LeftUp )
			{
				(*cbWindowFunc)(m_ID, m_pParent, WE_BTNCLICK );
				we |= WE_BTNCLICK;
				WINDOWMGR->SetMouseDownUsed();
			}
			else
				(*cbWindowFunc)(m_ID, m_pParent, WE_RBTNCLICK );
		}

		m_bClickInside = FALSE;
	}

	if( bMouseOver )
	{
		if( LeftPress && m_bClickInside )
		{
			m_pCurImage		= &m_PressImage;
			m_fgCurColor	= m_fgPressColor;
			m_BtnState		= eBtn_State_Press;
		}
		else
		{
			m_pCurImage		= &m_OverImage;
			m_fgCurColor	= m_fgOverColor;
			m_BtnState		= eBtn_State_Over;
		}

		if( !m_bPlayOverSound )
		{
			AUDIOMGR->Play(
				m_osndIdx,
				gHeroID);

			m_bPlayOverSound = TRUE ;
		}
	}
	else
	{
		if( LeftPress && m_bClickInside ) //안에서 클릭하여 밖으로 나간경우
		{
			m_pCurImage		= &m_OverImage;
			m_fgCurColor	= m_fgOverColor;
			m_BtnState		= eBtn_State_Over;
		}
		else
		{
			m_pCurImage		= &m_BasicImage;
			m_fgCurColor	= m_fgBasicColor;
			m_BtnState		= eBtn_State_None;
		}

		if( m_bPlayOverSound )
		{
			m_bPlayOverSound = FALSE ;
		}
	}

	return we;
}

void cButton::Render()
{
	if( m_bActive == FALSE ) return;
	
	if( m_FirstDrawAnimateImage )
	{
		RenderAnimationImage();
		cWindow::Render();
	}
	else
	{
		cWindow::Render();
		RenderAnimationImage();
	}

	if( m_nCurTextLen != 0 )
	{
		// 070209 LYW --- cButton : Modified render part.
		if( IsMovedWnd() )
		{
			SetRenderArea() ;
			SetMovedWnd( FALSE ) ;
		}

		if( m_bShadow )
		{
			CFONT_OBJ->RenderFont(m_wFontIdx, m_szText, m_nCurTextLen,&m_renderShadowRect,RGBA_MERGE(m_shadowColor, m_alpha * m_dwOptionAlpha / 100));
		}

		CFONT_OBJ->RenderFont(m_wFontIdx, m_szText,m_nCurTextLen,&m_renderRect,RGBA_MERGE(m_fgCurColor, m_alpha * m_dwOptionAlpha / 100));

	}
}


// 070209 LYW --- cButton : Add function to setting area of render part.
void cButton::SetRenderArea()
{
	LONG TxtPosX;

	if( m_nAlign & TXT_LEFT )
	{
		TxtPosX = (LONG)m_absPos.x + m_textXY.x;
	}
	else if( m_nAlign & TXT_RIGHT )
	{
		TxtPosX = (LONG)m_absPos.x + m_width - m_textXY.x 
				    - CFONT_OBJ->GetTextExtentEx( m_wFontIdx, m_szText, strlen(m_szText) );
	}
	else //TXT_CENTER
	{
		TxtPosX = (LONG)m_absPos.x + ( m_width
				    - CFONT_OBJ->GetTextExtentEx( m_wFontIdx, m_szText, strlen(m_szText) ) ) / 2;
	}

	if( m_pCurImage == (&m_PressImage) )
	{
		OffsetRect( &m_renderRect, m_BtnTextAniX, m_BtnTextAniY );
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

void cButton::RenderAnimationImage()
{
	if( !m_bIsActiveAnimated )
		return;

	switch( m_BtnState )
	{
	case eBtn_State_None:
		{
			RenderAnimationImage( m_mapAniImage, m_validXYAniImage );
		}
		break;
	case eBtn_State_Over:
		{
			RenderAnimationImage( m_mapOverAniImage, m_validXYOverAniImage );
		}
		break;
	case eBtn_State_Press:
		{
			RenderAnimationImage( m_mapPressAniImage, m_validXYPressAniImage );
		}
		break;
	}
}

void cButton::RenderAnimationImage( std::map< DWORD, cImage >& mapAniImg, const VECTOR2& validxy )
{
	if( mapAniImg.empty() )
		return;

	// Frame 변경
	if( m_dwNextChangeFrameTime < gCurTime )
	{
		m_dwNextChangeFrameTime = m_dwAniImgFrameTime + gCurTime;
		m_dwCurAniImgFrame = (m_dwCurAniImgFrame + 1) % mapAniImg.size();
	}

	const DWORD alpha = m_alpha * m_dwOptionAlpha / 100;
	const DWORD dwRGBA = RGBA_MERGE( m_dwImageRGB, alpha );

	std::map< DWORD, cImage >::iterator aniImgIter = mapAniImg.find( m_dwCurAniImgFrame );
	if( aniImgIter == mapAniImg.end() )
		return;

	VECTOR2 absPos = m_absPos;
	absPos.x += validxy.x;
	absPos.y += validxy.y;

	cImage& curFrameImg = aniImgIter->second;
	curFrameImg.RenderSprite( &m_scale, NULL, 0, &absPos, dwRGBA );

}

void cButton::SetText(char * text, DWORD basicColor, DWORD overColor, DWORD pressColor)
{
	ASSERT(MAX_TEXT_SIZE>strlen(text));
	m_nCurTextLen = strlen(text);
	strncpy(m_szText, text, m_nCurTextLen + 1);
	m_fgCurColor = m_fgBasicColor = m_fgOverColor = m_fgPressColor = basicColor;
	if(overColor)
		m_fgOverColor = overColor;
	if(pressColor)
		m_fgPressColor = pressColor;
}

void cButton::SetTextValue(DWORD value)
{
	wsprintf(m_szText, "%d", value);
	m_nCurTextLen = strlen(m_szText);
}


void cButton::Add( cWindow* )
{
	ASSERT( 0 && "It never contain any child control" );
}

void cButton::AddAnimationImage( eBtn_State state, const std::vector< std::pair< DWORD, cImageRect > >& aniImgList, const DWORD dwFrameTime )
{
	if( aniImgList.empty() )
		return;

	struct _AddAnimation{
		void AddImage( const std::vector< std::pair< DWORD, cImageRect > >& aniImgList, const DWORD dwFrameTime, std::map< DWORD, cImage >& saveMap, DWORD& saveFrameTime )
		{
			for( std::vector< std::pair< DWORD, cImageRect > >::const_iterator iterList = aniImgList.begin() ; iterList != aniImgList.end() ; ++iterList )
			{
				const DWORD dwImgIdx = iterList->first;
				const cImageRect& rect = iterList->second;

				cImage& aniImg = saveMap[ saveMap.size() ];
				SCRIPTMGR->GetImage( dwImgIdx, &aniImg, (cImageRect*)&rect );
			}

			if( dwFrameTime )
			{
				saveFrameTime = (DWORD)(dwFrameTime / saveMap.size());
			}
			else
			{
				saveFrameTime = (DWORD)(1000 / saveMap.size());
			}
		}
	}AddAnimation;

	switch( state )
	{
	case eBtn_State_None:
		{
			AddAnimation.AddImage( aniImgList, dwFrameTime, m_mapAniImage, m_dwAniImgFrameTime );
		}
		break;
	case eBtn_State_Over:
		{
			AddAnimation.AddImage( aniImgList, dwFrameTime, m_mapOverAniImage, m_dwOverAniImgFrameTime );
		}
		break;
	case eBtn_State_Press:
		{
			AddAnimation.AddImage( aniImgList, dwFrameTime, m_mapPressAniImage, m_dwPressAniImgFrameTime );
		}
		break;
	}
}

void cButton::SetBtnState( eBtn_State state )
{
	if( state == m_BtnState )
		return;

	m_BtnState = state;

	switch( state )
	{
	case eBtn_State_None:
		{
			m_pCurImage		= &m_BasicImage;
			m_fgCurColor	= m_fgBasicColor;
		}
		break;
	case eBtn_State_Over:
		{
			m_pCurImage		= &m_OverImage;
			m_fgCurColor	= m_fgOverColor;
		}
		break;
	case eBtn_State_Press:
		{
			m_pCurImage		= &m_PressImage;
			m_fgCurColor	= m_fgPressColor;
		}
		break;
	}
}
