// cImage.cpp: implementation of the cImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cImage.h"
#include "cResourceManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------------------------
// cImageBase class member
// ------------------------------------------------------------------------------------------
cImageBase::cImageBase()
{
	m_pSurface = NULL;
}
cImageBase::~cImageBase()
{
	//SAFE_RELEASE(m_pSurface);// ¸®¼Ò½º ¸Å´ÏÀú ¿¡¼­ 
}


// ------------------------------------------------------------------------------------------
// cImage class member
// ------------------------------------------------------------------------------------------
cImage::cImage()
{
}
cImage::~cImage()
{
}

void cImage::RenderSprite(VECTOR2* pv2Scaling,VECTOR2* pv2RotCenter,float fRot,
						  VECTOR2* pv2Trans, DWORD dwColor)
{
	VECTOR2 sc;
	if(pv2Scaling)
	{
		sc.x = (float)(m_srcImageRect.right-m_srcImageRect.left)*pv2Scaling->x/m_srcImageSize.x;
		sc.y = (float)(m_srcImageRect.bottom-m_srcImageRect.top)*pv2Scaling->y/m_srcImageSize.y;
	}
	else
	{
		sc.x = (float)(m_srcImageRect.right-m_srcImageRect.left)/m_srcImageSize.x;
		sc.y = (float)(m_srcImageRect.bottom-m_srcImageRect.top)/m_srcImageSize.y;
	}
	
	cImageBase::RenderSprite( &sc, pv2RotCenter, fRot, pv2Trans, &m_srcImageRect, dwColor );
}

BOOL cImage::HasAlpha( LONG x, LONG y )
{
	if( m_pSurface == NULL )
		return TRUE;

	int srcX = m_srcImageRect.left + x + 1;
	int srcY = m_srcImageRect.top + y + 1;

	if( srcX < m_srcImageRect.left || srcX > m_srcImageRect.right ||
		srcY < m_srcImageRect.top || srcY > m_srcImageRect.bottom )
		return TRUE;

	LOCKED_RECT lockrect={0};
	RECT needRect={0};
	SetRect( &needRect, srcX, (int)m_srcImageSize.y - srcY, srcX + 1, (int)m_srcImageSize.y - srcY + 1 );
	if( m_pSurface->LockRect( &lockrect, &needRect, TEXTURE_FORMAT_A8R8G8B8 ) == FALSE )
		return TRUE;

	BYTE* imageData = (BYTE*)lockrect.pBits;

	//const BYTE blue		= imageData[ 0 ];
	//const BYTE green	= imageData[ 1 ];
	//const BYTE red		= imageData[ 2 ];
	const BYTE alpha	= imageData[ 3 ];

	m_pSurface->UnlockRect();

	if( alpha < 255 )
		return TRUE;

	return FALSE;
}