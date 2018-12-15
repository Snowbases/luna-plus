// 090422 ShinJS --- 탈것 탑승을 위한 오른쪽 팝업 메뉴 추가
#pragma once
#include "./interface/cDialog.h"

class cButton;

class CRidePopupMenuDlg : public cDialog
{
	cButton* m_GetOn;
	cButton* m_GetOff;
	cButton* m_Seal;

public:
	CRidePopupMenuDlg();
	virtual ~CRidePopupMenuDlg();
	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);
	virtual DWORD ActionEvent(CMouse *mouseInfo);
};
