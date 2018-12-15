#pragma once

#include "interface/cDialog.h"

class cCheckBox;
class cEditBox;

class CRideOptionDlg : public cDialog
{
private:
	cCheckBox* mAllowCheckBox;
	cEditBox* mMoneyEditBox;
	DWORD mIsUpdateComma;

public:
	CRideOptionDlg();
	virtual ~CRideOptionDlg();
	virtual void SetActive(BOOL);
	virtual void OnActionEvent(LONG windowIndex, LPVOID, DWORD event);
	virtual void Render();

private:
	void Linking();
};