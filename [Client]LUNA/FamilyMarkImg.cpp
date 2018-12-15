// FamilyMarkImg.cpp: implementation of the CFamilyMarkImg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FamilyMarkImg.h"

#include "cImageSelf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyMarkImg::CFamilyMarkImg(MARKNAMETYPE markname,cImageSelf* pImg)
{
	m_MarkName = markname;
	m_pImg = pImg;
	cImageRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = 16;
	rect.bottom = 12;

	if( m_pImg )
		m_pImg->SetImageSrcRect(&rect);
}

CFamilyMarkImg::~CFamilyMarkImg()
{

}

void CFamilyMarkImg::Render(VECTOR2* pos, DWORD dwColor)
{
	VECTOR2 scale;
	scale.x = 1;
	scale.y = 0.75;
	m_pImg->RenderSprite(&scale,NULL,0,pos,dwColor);
}

void CFamilyMarkImg::Release()
{

	SAFE_DELETE( m_pImg );
}
