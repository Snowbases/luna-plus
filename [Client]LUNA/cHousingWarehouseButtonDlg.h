#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

//090409 pdy 하우징 하우스 검색 UI추가
class cHousingWarehouseButtonDlg : public cDialog  
{
	cButton* m_pWarehouseBtn;
	
public:
	cHousingWarehouseButtonDlg();
	virtual ~cHousingWarehouseButtonDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);

	void OnActionEvent(LONG lId, void* p, DWORD we);
};
