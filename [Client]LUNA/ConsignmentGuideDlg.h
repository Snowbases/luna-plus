#pragma once
#include "INTERFACE\cDialog.h"

class CConsignmentGuideDlg :
	public cDialog
{
public:
	CConsignmentGuideDlg(void);
	virtual ~CConsignmentGuideDlg(void);

	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void SetActive(BOOL val);
	void Linking();
};
