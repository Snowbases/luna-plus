#pragma once

#include "stdAfx.h"
#include "interface/cDialog.h"

class cButton;
class cGuageBar;


//가구 설치셋팅 UI (이동 회전 삭제 배치)
class CHousingRevolDlg : public cDialog
{
protected:
	cButton* m_BtReset;
	cButton* m_BtSet;
	cButton* m_BtDel;
	cButton* m_BtLArrow;
	cButton* m_BtRArrow;

	cGuageBar* m_Bar;

public:
	CHousingRevolDlg();
	virtual ~CHousingRevolDlg();

	void Linking();

	virtual DWORD ActionEvent(CMouse *mouseInfo);
	void OnActionEvent(LONG lId, void* p, DWORD we);
	void SetActive(BOOL val);
	void SetBarRateFromAngle(float fAngle);
	void StartRotate();
	float GetRotateAngleFromBarRate();

};