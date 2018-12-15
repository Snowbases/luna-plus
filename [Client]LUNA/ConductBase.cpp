#include "stdafx.h"
#include ".\conductbase.h"

#include "interface/cScriptManager.h"

cConductBase::cConductBase(void)
{
}

cConductBase::~cConductBase(void)
{
}

void cConductBase::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * lowImage, cImage * highImage, LONG ID)
{
	cIcon::Init(x,y,wid,hei,lowImage,ID);
	m_type = WT_CONDUCT;
	if(highImage)
		m_pHighLayerImage = *highImage;
	
	cImage bimg,gimg;
	cImageRect rect;
	SetRect(&rect,1010,216,1011,226);
	SCRIPTMGR->GetImage(0,&bimg,&rect);	//basicimage
	SetRect(&rect,1006,216,1007,226);
	SCRIPTMGR->GetImage(0,&gimg,&rect); //guageimage

	m_dwFontColor = 0xffffffff;

	SetAlpha(255);
}

void cConductBase::Render()
{
	cIcon::Render();
	if(!m_pHighLayerImage.IsNull())
		m_pHighLayerImage.RenderSprite( NULL, NULL, 0, &m_absPos, RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	/*
	if(m_SkillBaseInfo.Level > 0)
	{
		static char nums[3];
		wsprintf(nums,"%d", m_SkillBaseInfo.Level);
		if( IsMovedWnd() )
		{
			m_rect.left		= (LONG)m_absPos.x+1 ;
			m_rect.top		= (LONG)m_absPos.y+28 ;
			m_rect.right	= 1 ;
			m_rect.bottom	= 1 ;

			SetMovedWnd( FALSE ) ;
		}
		CFONT_OBJ->RenderFont( 0, nums, strlen(nums), &m_rect, m_dwFontColor );
	}
	*/
}
