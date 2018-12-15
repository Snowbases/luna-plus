#include "stdafx.h"
#include "GuildMarkImg.h"
#include "cImageSelf.h"


CGuildMarkImg::CGuildMarkImg(MARKNAMETYPE markname,cImageSelf* pImg) :
m_MarkName( markname ),
m_pImg( pImg )
{
	cImageRect rect;
	rect.left	= 0;
	rect.top	= 0;
	rect.right	= GUILDMARKWIDTH - 1;
	rect.bottom = GUILDMARKHEIGHT - 1;

	if( m_pImg )
	{
		m_pImg->SetImageSrcRect( &rect );
	}
}


CGuildMarkImg::~CGuildMarkImg()
{}


void CGuildMarkImg::Render(VECTOR2* pos, DWORD dwColor )
{
	VECTOR2 scale;
	scale.x = 1.0f;
	scale.y = 1.0f;

	m_pImg->RenderSprite( &scale, 0, 0, pos, dwColor );
}


void CGuildMarkImg::Release()
{
	SAFE_DELETE( m_pImg );
}