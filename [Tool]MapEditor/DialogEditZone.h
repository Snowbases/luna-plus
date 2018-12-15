#pragma once
#include "afxcmn.h"

#include "./DataGrid/DataGrid.h"

// CDialogEditZone 대화 상자입니다.

class CDialogEditZone : public CDialog
{
	DECLARE_DYNAMIC(CDialogEditZone)

public:
	CDialogEditZone(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogEditZone();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ZONE_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnShow();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();

	void SetZoneDesc(GXZONE_DESC* pDesc);

	void UpdateData();
	void UpdateGrid();

	void Update(BOOL bSave);
	void EnableView();
	void DisableView();

	inline BOOL IsShowing()	{	return m_bShow;	}

	BOOL		m_bShow;

public:

	GXZONE_DESC* m_pZoneDesc;
	CDataGrid    m_cGridZone;
};
