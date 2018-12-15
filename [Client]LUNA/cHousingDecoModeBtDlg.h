#pragma once

#include "stdAfx.h"
#include ".\interface\cWindowHeader.h"
#include "interface/cDialog.h"

class cHousingDecoModeBtDlg : public cDialog  
{
	cButton* m_pDecoMode_On_Btn;
	cButton* m_pDecoMode_Off_Btn;
	
public:
	cHousingDecoModeBtDlg();
	virtual ~cHousingDecoModeBtDlg();
	void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	void Linking();
	virtual void SetActive(BOOL val);

	void OnActionEvent(LONG lId, void* p, DWORD we);
	void OnChangeDecoMode( BOOL bDecemode);
};