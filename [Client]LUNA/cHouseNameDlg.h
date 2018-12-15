#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

//090410 pdy 하우징 하우스 이름설정 UI
class cHouseNameDlg : public cDialog  
{
	cStatic * m_pHouseName;
	cEditBox * m_pHouseNameEdit;
	cButton * m_pHouseNameOkBtn;

public:
	cHouseNameDlg();
	virtual ~cHouseNameDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);
	void SetName(char* Name);
	char* GetName();

	void OnActionEvent(LONG lId, void* p, DWORD we);
};
