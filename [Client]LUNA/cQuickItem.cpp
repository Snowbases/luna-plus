#include "stdafx.h"
#include "cQuickItem.h"
#include "Interface/cScriptManager.h"
#include "interface/cFont.h"
#include "Interface/cWindowManager.h"
#include "input/UserInput.h"
#include "../[CC]Skill/Client/Manager/SkillManager.h"
#include "../[CC]Skill/Client/Info/ActiveSkillInfo.h"
#include "cSkillBase.h"
#include "cSkillTreeManager.h"

cQuickItem::cQuickItem( WORD tab, WORD pos )
: mTab( tab ), mPos( pos )
{
	SCRIPTMGR->GetImage(
		2,
		&m_DelayImage);
	m_type = WT_QUICKITEM;
	ZeroMemory(
		&mInfo,
		sizeof(mInfo));
	mData[0] = 0;
}

cQuickItem::~cQuickItem()
{}

void cQuickItem::SetSlotData( WORD dur )
{
	mInfo.data = dur;

	if( dur <= 0 )
	{
		SetZeroCount(TRUE) ;
	}
	
	if( mInfo.kind == QuickKind_MultiItem )
	{
		wsprintf(mData,"%2d", mInfo.data);
	}
}

void cQuickItem::ClearLink()
{
	ClearStateImage();
	memset( &mInfo, 0, sizeof( SLOT_INFO ) );
	
	SetCurImage( NULL );
	m_HighLayerImage.SetSpriteObject( NULL );

	SetZeroCount(FALSE) ;

	ZeroMemory(
		mData,
		sizeof(mData));
}

void cQuickItem::SetImage2(cImage * low, cImage * high)
{
	SetBasicImage(low);
	if(high)
	m_HighLayerImage = *high;
	
	SCRIPTMGR->GetImage( 2, &m_DelayImage  );	
}

void cQuickItem::Render()
{
	if( !m_bActive ) return;

	cIcon::Render();

	if(!m_HighLayerImage.IsNull())
		m_HighLayerImage.RenderSprite( NULL, NULL, 0, &m_absPos, RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));

	if(!m_LinkImage.IsNull())
	{
		VECTOR2 pos = { m_absPos.x+1, m_absPos.y+29 };
		m_LinkImage.RenderSprite(NULL, NULL, NULL, &pos, RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));
	}

	if( m_bZeroCount )
	{
		m_ZeroImage.RenderSprite( NULL, NULL, 0, &m_absPos, 
									RGBA_MERGE(0xffffff, 200 * m_dwOptionAlpha / 100));
	}
	
	if(mData[0])		
	{
		//RECT rect={(LONG)m_absPos.x+23, (LONG)m_absPos.y+25, 1,1};
		//CFONT_OBJ->RenderFont(0,mData,strlen(mData),&rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));		// color hard coding : taiyo 
		int len = strlen( mData );

		RECT rect={(LONG)m_absPos.x+38-(len*8), (LONG)m_absPos.y+24, 1,1};
		CFONT_OBJ->RenderFontShadow(0, 1, mData,strlen(mData),&rect,RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));		// color hard coding : taiyo 
	}

	PutSkillTip();
}


void cQuickItem::PutSkillTip()
{
	if(QuickKind_Skill != mInfo.kind)
	{
		return;
	}

	if(WINDOWMGR->IsMouseOverUsed() &&
		PtInWindow(g_UserInput.GetMouse()->GetMouseX(), g_UserInput.GetMouse()->GetMouseY()))
	{
		SKILLTREEMGR->SetToolTipIcon(
			this,
			GetData());
	}

	if(cActiveSkillInfo* const skillInfo = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(GetData()))
	{
		if(skillInfo->GetCoolTime() > gCurTime)
		{
			const float ratio = float(skillInfo->GetCoolTime() - gCurTime) / skillInfo->GetSkillInfo()->CoolTime;
			VECTOR2 scale = {1.0f, m_height * ratio};
			VECTOR2 position = {m_absPos.x, m_absPos.y + m_height * (1 - ratio)};

			m_DelayImage.RenderSprite(
				&scale,
				0,
				0,
				&position,
				0xff000000);
		}
	}
}