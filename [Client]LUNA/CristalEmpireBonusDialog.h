#pragma once

#include "interface/cDialog.h"

class cIconDialog;
class cStatic;

class CCristalEmpireBonusDialog : public  cDialog
{
public:
	CCristalEmpireBonusDialog();
	virtual ~CCristalEmpireBonusDialog();
	virtual void OnActionEvent(LONG, LPVOID, DWORD);
	virtual DWORD ActionEvent(CMouse*);
	virtual void SetActive(BOOL);
	virtual BOOL FakeMoveIcon(LONG x, LONG y, cIcon*);
	void Ready(const MSG_ITEM_OPTION&);
	void Request();
	void Finish(const MSG_ITEM_OPTION&);

private:
	void Linking();
	void Clear();
	cIcon* GetIcon(cIconDialog&, LONG x, LONG y) const;

private:	
    cIconDialog* mTargetIconDialog;
	cIconDialog* mListIconDialog;
	cStatic* mResultStatic;
	cWindow* mSubmitButton;
};