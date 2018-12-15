#pragma once
#include "baseitem.h"

enum QuickKind
{
	QuickKind_None,
	QuickKind_SingleItem,
	QuickKind_MultiItem,
	QuickKind_Skill,
	// 070216 LYW --- cQuickItem : Add enum for conduct to QuickKind.
	QuickKind_Conduct,
	QuickKind_PetItem,
	QuickKind_PetSkill,
};

class cImage;

class cQuickItem :
	public CBaseItem
{
	cImage m_HighLayerImage;
	cImage m_LinkImage;
	cImage m_DelayImage;
	SLOT_INFO mInfo;
	WORD	mTab;
	WORD	mPos;
	char	mData[ 17 ];
public:
	cQuickItem( WORD tab, WORD pos );
	virtual ~cQuickItem(void);
	void SetSlotInfo(const SLOT_INFO& info) { mInfo = info; }
	const SLOT_INFO& GetSlotInfo() const { return mInfo; }
	void SetSlotData(WORD dur);
	void GetPosition(WORD& tab, WORD& pos) { tab = mTab; pos = mPos; }
	void ClearLink();

	void SetImage2(cImage * low, cImage * high);

	virtual void Render();
	void PutSkillTip();
};
