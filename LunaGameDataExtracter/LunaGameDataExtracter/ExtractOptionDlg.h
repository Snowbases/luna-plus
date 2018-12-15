#pragma once
#include "afxwin.h"


// CExtractOptionDlg 대화 상자입니다.

class CExtractOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CExtractOptionDlg)

public:
	CExtractOptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CExtractOptionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EXTRACT_OPT };


	int GetTotalExtractDataCnt() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	// Item icon image
	BOOL m_b2DItemIconImageCheckBox;
	BOOL m_b2DItemIconImageALLCheckBox;
	DWORD m_dw2DItemIconImageRangeMin;
	DWORD m_dw2DItemIconImageRangeMax;

	CButton m_2DItemIconImageALLCheckBox;
	CEdit m_2DItemIconImageRangeMin;
	CEdit m_2DItemIconImageRangeMax;


	// Monster image
	BOOL m_b3DMonsterImageCheckBox;
	BOOL m_b3DMonsterImageCheckBoxALL;
	DWORD m_dw3DMonsterImageRangeMin;
	DWORD m_dw3DMonsterImageRangeMax;

	CButton m_3DMonsterImageCheckBoxALL;
	CEdit m_3DMonsterImageRangeMin;
	CEdit m_3DMonsterImageRangeMax;


	// Active Skill image
	BOOL m_b2DActiveSkillIconImageCheckBox;
	BOOL m_b2DActiveIconImageALLCheckBox;
	DWORD m_dw2DActiveIconImageRangeMin;
	DWORD m_dw2DActiveIconImageRangeMax;

	CButton m_2DActiveIconImageALLCheckBox;
	CEdit m_2DActiveIconImageRangeMin;
	CEdit m_2DActiveIconImageRangeMax;


	// Buff Skill image
	BOOL m_b2DBuffSkillIconImageCheckBox;
	BOOL m_b2DBuffIconImageALLCheckBox;
	DWORD m_dw2DBuffIconImageRangeMin;
	DWORD m_dw2DBuffIconImageRangeMax;

	CButton m_2DBuffIconImageALLCheckBox;
	CEdit m_2DBuffIconImageRangeMin;
	CEdit m_2DBuffIconImageRangeMax;

	// File Out Type
	BOOL m_bExtractFileText;
	BOOL m_bExtractFileExcel;

	// Data
	BOOL m_bExtractItemInfo;
	BOOL m_bExtractMonsterInfo;
	BOOL m_bExtractSkillInfo;
	BOOL m_bExtractQuestInfo;

protected:

	void ClickedImageBtn( BOOL& bBtn, BOOL& bAllBtn, DWORD& dwMin, DWORD& dwMax, CButton& allBtn, CEdit& min, CEdit& max );
	void ClickedImageALLBtn( BOOL& bBtn, BOOL& bAllBtn, DWORD& dwMin, DWORD& dwMax, CButton& allBtn, CEdit& min, CEdit& max );

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck2ditemIconImage();
	afx_msg void OnBnClickedCheck2ditemIconImageAll();
	afx_msg void OnBnClickedCheck3dmonsterImage();
	afx_msg void OnBnClickedCheck3dmonsterImageAll();
	afx_msg void OnBnClickedCheck2dactiveskillImage();
	afx_msg void OnBnClickedCheck2dactiveskillImageAll();
	afx_msg void OnBnClickedCheckIdcEdit2dbuffskillImage();
	afx_msg void OnBnClickedCheck2dbuffskillImageAll();
	afx_msg void OnBnClickedOk();
};
