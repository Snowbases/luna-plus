// ItemShow.cpp: implementation of the CItemShow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemShow.h"
#include "ItemManager.h"
#include "./Interface/cFont.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemShow::CItemShow()
{
	m_type = WT_ITEMSHOW;
	cIcon::Init( 0, 0, DEFAULT_ICONSIZE, DEFAULT_ICONSIZE, NULL, -1 );
	
	m_wItemIdx	= 0;
	m_Dur		= 0;
}

CItemShow::~CItemShow()
{

}

void CItemShow::Init(DWORD wItemIdx, DURTYPE dur)
{
	m_wItemIdx = wItemIdx;
	m_Dur = dur;

	// 090824 ShinJS --- 개수 출력 여부 설정
	mIsVisibleDurability = ITEMMGR->IsDupItem(m_wItemIdx);
	
	if(m_wItemIdx)
	{
		cImage tmpImage;
		ITEMMGR->GetIconImage( wItemIdx, &tmpImage );
		SetBasicImage(&tmpImage);
	}
	else
	{
		SetCurImage(NULL);
	}
}
void CItemShow::Render()
{
	cIcon::Render();

	// 090824 ShinJS --- 출력 여부를 Init시 판단, 이후 설정시 Render 할수 있도록 수정
	if( mIsVisibleDurability )
	{
		static char nums[5];
		wsprintf(nums,"%2d", m_Dur);
		RECT rect={(LONG)m_absPos.x+23, (LONG)m_absPos.y+25, 1,1};
		CFONT_OBJ->RenderFont(0,nums,strlen(nums),&rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));		// color hard coding : taiyo 
	}
}