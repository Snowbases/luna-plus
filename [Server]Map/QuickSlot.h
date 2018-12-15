#pragma once

// 071206 LYW --- QuickSlot : 퀵슬롯에 등록되는 퀵아이템 종류를 선언한다.
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

class cQuickSlot
{
	DWORD		mCharacterIdx;
	BYTE		mTabNum;
	SLOT_INFO	mSlot[10];
public:
	cQuickSlot(void);
	virtual ~cQuickSlot(void);

	void Init( DWORD characterIdx, BYTE num );
	BOOL Update( WORD pos, SLOT_INFO* pSlot );
	SLOT_INFO*	GetQuick( WORD pos );
	void DBUpdate();
};
