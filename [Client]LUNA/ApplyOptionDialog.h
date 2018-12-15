/*
080215 LUJ, 창에 위치한 아이템에 옵션을 적용시킨다.
*/
#pragma once


#include "interface/cDialog.h"


class CItem;
class cStatic;
class cIconDialog;


class CApplyOptionDialog : public  cDialog
{
public:
	CApplyOptionDialog();
	virtual ~CApplyOptionDialog();

	void Linking();
	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );
	
	virtual void OnActionEvent(LONG lId, void* p, DWORD we);
	virtual void SetActive( BOOL );

	void Restore();
	void Succeed();
	void Fail();

	void SetSourceItem( CItem& );

	void Send();

private:
	CItem*			mTargetItem;
	CItem*			mSourceItem;

	cStatic*		mResultStatic;
	cIconDialog*	mIconDialog;
    
	MSG_DWORD2		mMessage;
};