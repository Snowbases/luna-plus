#pragma once
#include ".\interface\cdialog.h"

#include "../QuickManager.h"

class cIconGridDialog;
class cStatic;
class cQuickItem;
class CItem;
class cSkillBase;

class cConductBase ;

class cQuickSlotDlg :
	public cDialog
{
	cIconGridDialog* mpSheet[ MAX_SLOTPAGE ];

	WORD	mCurSheet;
	void FakeMoveQuickItem(cDialog* pFromDlg, LONG x, LONG y, cQuickItem * icon) ;

	void FakeAddItem(LONG x, LONG y, CItem* icon);
	void FakeAddSkill(LONG x, LONG y, cSkillBase* icon);
	BOOL CanEquip(cIcon*);
	void FakeAddConduct( LONG x, LONG y, cConductBase* icon ) ;

public:
	cQuickSlotDlg(void);
	virtual ~cQuickSlotDlg(void);

	virtual void Add(cWindow * window);
	void Linking();
	void OnActionEvnet(LONG lId, void * p, DWORD we);

	void SelectPage( WORD page );
	void SetActive( BOOL val );

	// 080703 LYW --- QuickSlotDlg : 확장 슬롯 추가에 따른 함수 변경.
	//BOOL FakeMoveIcon(LONG mouseX, LONG mouseY, cIcon * icon);
	BOOL FakeMoveIcon(cDialog* pFromDlg, LONG mouseX, LONG mouseY, cIcon * icon);
	void FakeDeleteQuickItem();
	WORD GetCurSheet() const { return mCurSheet; }
	void SetCoolTime( DWORD itemIndex,  DWORD miliSecond );
	// 080326 NYJ --- 지정값으로 쿨타임갱신.
	void SetCoolTimeAbs( DWORD itemIndex, DWORD dwBegin, DWORD dwEnd, DWORD miliSecond );
};