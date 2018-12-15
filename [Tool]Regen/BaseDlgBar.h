#pragma once


// CBaseDlgBar 대화 상자입니다.
#include "DlgBarSheet.h"

class CBaseDlgBar : public CDialog
{
	DECLARE_DYNAMIC(CBaseDlgBar)

	CDlgBarSheet* m_pSheet;

public:
	CBaseDlgBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CBaseDlgBar();

	CWnd* GetMonsterPage() { return (m_pSheet == NULL ? NULL : m_pSheet->GetMonsterPage()); }

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLGBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
