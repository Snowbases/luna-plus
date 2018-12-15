#pragma once
#include "./interface/cDialog.h"

class cButton;

class cAdditionalButtonDlg : public cDialog
{
	// 인던, 데이트매칭 등 미니맵이 표시되지 않는 맵에서 
	// 동영상녹화시 관련버튼을 출력하도록 한다.
	cButton* m_pButtonRecordStart;
	cButton* m_pButtonRecordStop;

public:
	cAdditionalButtonDlg();
	virtual ~cAdditionalButtonDlg();

	void Linking();
	virtual void OnActionEvent(LONG lId, void* p, DWORD we) ;
	virtual void SetActive(BOOL val);

	void ShowRecordStartBtn( BOOL val );
};