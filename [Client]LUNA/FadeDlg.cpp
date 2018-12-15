// 080930 LYW --- FadeDlg : 페이드 인/아웃 다이얼로그 추가.
#include "stdafx.h"
#include ".\fadedlg.h"
#include "./Interface/cScriptManager.h"
#include "./GameResourceManager.h"

CFadeDlg::CFadeDlg(void)
{
	mScale.x = 1 ;
	mScale.y = 1 ;

	mPos.x = 0 ;
	mPos.y = 0 ;

	dwColor = RGBA_MAKE(10, 10, 10, 255) ;

	m_byAlpha = 200 ;

	m_byFadeMode = eFadeNone ;
}

CFadeDlg::~CFadeDlg(void)
{
}

void CFadeDlg::Initialize()
{
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	m_nDispWidth  = dispInfo.dwWidth ;
	m_nDispHeight = dispInfo.dwHeight ;

	SCRIPTMGR->GetImage( 143, &m_ImgBackGround, PFT_HARDPATH ) ;

	mScale.x = m_nDispWidth / 2.0f;
	mScale.y = m_nDispHeight / 2.0f;
}

void CFadeDlg::FadeIn()
{
	m_byAlpha = 255 ;
	m_byFadeMode = eFadeIn ;
	SetActive(TRUE) ;
}

void CFadeDlg::FadeOut()
{
	m_byAlpha = 200 ;
	m_byFadeMode = eFadeOut ;
	SetActive(TRUE) ;
}

void CFadeDlg::Render()
{
	if( !m_bActive ) return ;

	if( m_byFadeMode != eFadeNone )
	{
		if( m_byFadeMode == eFadeIn )
		{
			if( m_byAlpha > 200 ) --m_byAlpha ;
			else 
			{
				m_byFadeMode = eFadeNone ;
				SetActive(FALSE) ;
			}
		}
		else if( m_byFadeMode == eFadeOut )
		{
			if( m_byAlpha < 255 ) ++m_byAlpha ;
			else
			{
				m_byFadeMode = eFadeNone ;
				SetActive(FALSE) ;
			}
		}

		dwColor = RGBA_MAKE(10, 10, 10, m_byAlpha) ;
	}

	m_ImgBackGround.RenderSprite( &mScale, NULL, 0, &mPos, dwColor ) ;
}