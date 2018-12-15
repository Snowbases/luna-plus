// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
#pragma once
#include "./interface/cDialog.h"

class cEditBox;
class cButton;

class CInputNameDlg : public cDialog
{
	cEditBox*		m_CharName;

public:
	CInputNameDlg();
	virtual ~CInputNameDlg();

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);
};
