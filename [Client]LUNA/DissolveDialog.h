#pragma once

#include "Interface/cTabDialog.h"

class cIconDialog;
class cIconGridDialog;
class CItem;
class CMouse;

class CDissolveDialog : public cTabDialog
{
public:
	CDissolveDialog();
	virtual ~CDissolveDialog();
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon*);
	virtual void SetActive(BOOL);
	virtual void Add(cWindow*);
	void OnActionEvent(LONG lId, void* p, DWORD we);
	DWORD ActionEvent(CMouse*);
	void Render();
	void Succeed(const MSG_ITEM_DISSOLVE_ACK&);
	// 100625 ONS 우클릭 분해 아이템 이동 처리 함수.
	void MoveIcon( CItem* pItem );
	
private:
	enum Error
	{
		ErrorNone,
		ErrorIsNoInventory,
		ErrorInInvalidSource,
		ErrorInsufficientInventory,
		ErrorInsufficientResult,
		ErrorInsufficientDynamicResult,
		ErrorNoScript,
		ErrorInvalidLevel,
		ErrorNoHero,
	};
	Error IsEnableUpdate(const ITEMBASE&);
	void AddSource(const ITEMBASE&, POSTYPE);
	void RemoveSource(const ITEMBASE&, POSTYPE);
	void AddResult(const ITEMBASE&);
	void RemoveResult(const ITEMBASE&);
	void Linking();
	void Clear();
	void Clear(CItem&) const;
	void ClearResult();
	CItem* GetSourceItem(const POINT&);
	Error Submit();
	void PutEffect();
	void PutTip();
	void PutTip(CItem&, DWORD sourceItemIndex, DURTYPE sourceItemSize, DURTYPE resultItemSize) const;
	void PutError(Error, DWORD itemIndex) const;

private:
	cIconDialog* mSourceDialog;
	cIconDialog* mRandomResultDialog;
	cWindow* mSubmitButton;
	BOOL mIsEffectStart;
	struct SourceData
	{
		DWORD mSourceItemIndex;
		DURTYPE mSourceItemQuantity;
		DURTYPE mResultItemQuantity;
		DURTYPE mRandomSourceItemQuantity;
		DURTYPE mRandomResultItemQuantity;
	};
};