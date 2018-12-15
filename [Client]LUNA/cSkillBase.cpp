#include "stdafx.h"
#include ".\cskillbase.h"
#include "./Interface/cFont.h"
#include "./Input/Mouse.h"
#include "interface/cScriptManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "cSkillTreeManager.h"
#include "../[CC]Skill/Client/Info/ActiveSkillInfo.h"
#include "../[CC]Skill/Client/Manager/SkillManager.h"
#include "input/UserInput.h"
#include "cWindowManager.h"

cSkillBase::cSkillBase(void) :
mSkillInfo(0)
{
	SCRIPTMGR->GetImage( 2, &m_DelayImage  );	
}

cSkillBase::~cSkillBase(void)
{
}

void cSkillBase::SetSkillBase(const SKILL_BASE& skillBase)
{
	m_SkillBaseInfo = skillBase;

	const LEVELTYPE skillLevel = min(
		LEVELTYPE(SKILLMGR->GetSkillSize(skillBase.wSkillIdx)),
		skillBase.Level);
	mSkillInfo = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(
		skillBase.wSkillIdx - 1 + skillLevel);

	if(0 == mSkillInfo)
	{
		return;
	}

	SCRIPTMGR->GetImage(
		mSkillInfo->GetImage(),
		&m_pHighLayerImage,
		PFT_SKILLPATH);
}

void cSkillBase::Init( LONG x, LONG y, WORD wid, WORD hei, cImage * lowImage, cImage * highImage, LONG ID )
{
	cIcon::Init(x,y,wid,hei,lowImage,ID);
	m_type = WT_SKILL;
	if(highImage)
		m_pHighLayerImage = *highImage;
	
	cImage bimg,gimg;
	cImageRect rect;
	SetRect(&rect,1010,216,1011,226);
	SCRIPTMGR->GetImage(0,&bimg,&rect);	//basicimage
	SetRect(&rect,1006,216,1007,226);
	SCRIPTMGR->GetImage(0,&gimg,&rect); //guageimage

	if(0 < m_SkillBaseInfo.wSkillIdx &&
		0 == m_SkillBaseInfo.Level)
	{
		SetAlpha(120);
	}
	else
	{
		SetAlpha(255);
	}
}

void cSkillBase::Render()
{
	cIcon::Render();
	if(!m_pHighLayerImage.IsNull())
		m_pHighLayerImage.RenderSprite( NULL, NULL, 0, &m_absPos, RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100));

	if(mSkillInfo->GetCoolTime() > gCurTime)
	{
		const float ratio = float(mSkillInfo->GetCoolTime() - gCurTime) / mSkillInfo->GetSkillInfo()->CoolTime;
		VECTOR2 scale = {
			1.0f,
			m_height * ratio};
		VECTOR2 position = {
			m_absPos.x,
			m_absPos.y + m_height * (1 - ratio)};

		m_DelayImage.RenderSprite(
			&scale,
			0,
			0,
			&position,
			0xff000000);
	}

	if(m_SkillBaseInfo.Level > 0)
	{
		RECT rect = {
			LONG(m_absPos.x + 1),
				LONG(m_absPos.y + 22),
				1,
				1};
			TCHAR nums[MAX_PATH] = {0};
			_stprintf(
				nums,
				_T("%d"),
				mSkillInfo->GetLevel());
			CFONT_OBJ->RenderFontShadow(
				0,
				1,
				nums,
				_tcslen(nums),
				&rect,
				RGBA_MERGE(m_dwImageRGB, m_alpha * m_dwOptionAlpha / 100 ));
	}

	if(WINDOWMGR->IsMouseOverUsed() &&
		PtInWindow(g_UserInput.GetMouse()->GetMouseX(), g_UserInput.GetMouse()->GetMouseY()))
	{
		SKILLTREEMGR->SetToolTipIcon(
			this,
			GetSkillIdx());
	}
}

DWORD cSkillBase::GetSkillIdx() const
{
	return mSkillInfo ? mSkillInfo->GetIndex() : 0;
}