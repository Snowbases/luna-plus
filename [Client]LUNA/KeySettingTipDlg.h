#pragma once
#include ".\interface\cdialog.h"
#include "cImageSelf.h"

#include "./Interface/cButton.h"

class CKeySettingTipDlg :
	public cDialog
{
	//cImageSelf		m_KeyImage[2];
	cImage			m_KeyImage ;
	cImage			m_KeyImage2 ;
	WORD			m_wMode;

	// 070209 LYW --- KeySettingTipDlg : Add VECTOR2.
	VECTOR2 m_scale ;

	cImage* m_pRenderImage ;

	cButton*		m_pCloseButton ;

public:
	CKeySettingTipDlg(void);
	~CKeySettingTipDlg(void);
	
	void Linking();
	void SetMode(WORD mode) { m_wMode = mode; }
	virtual void Render();

	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;

	// 071022 LYW --- KeySettingTipDlg : Add function to setting active or deactive dialog.
	virtual void SetActive(BOOL val);
};
