#pragma once
#include "afxwin.h"
#include "afxcmn.h"


class CclientApp;
class CSkillReportDialog;


// CSkillDialog 대화 상자입니다.

class CSkillDialog : public CDialog
{
	friend class CSkillReportDialog;

	DECLARE_DYNAMIC(CSkillDialog)

public:
	CSkillDialog( CclientApp&, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSkillDialog();

	void Parse( DWORD serverIndex, const MSGROOT* );

	virtual void OnFinalRelease();

	void Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex );


protected:
	void PutSkill();
	
protected:
	DWORD		mServerIndex;
	DWORD		mPlayerIndex;
	CString		mPlayerName;
	//CListCtrl	mMyListCtrl;
	CclientApp&	mApplication;
	CComboBox mLevelCombo;
	CButton mUpdateButton;
	CStatic mEnableSkillStatic;
	CStatic mMySkillStatic;
	CListCtrl mEnableListCtrl;
	// 090303 LUJ, 스킬 설명 툴팁
	CToolTipCtrl mSkillDescriptionToolTipCtrl;

	// 키: 스킬 인덱스
	typedef	stdext::hash_map< DWORD, SKILL_BASE >	SkillMap;
	SkillMap										mSkillMap;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKILLDIALOG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSkillUpdateButton();
	afx_msg void OnLvnItemchangedSkillDialogList(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnBnClickedSkillRemoveButton();
	afx_msg void OnBnClickedSkillAddButton();
	afx_msg void OnLvnItemchangedSkillEnableList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedSkillMyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSkillLogButton();
	CListCtrl mMyListCtrl;
	CButton mReportButton;
	afx_msg void OnBnClickedSkillReportButton();
	CListCtrl mBuffListCtrl;
	CComboBox mSkillFindComboBox;
	afx_msg void OnCbnEditchangeSkillFindCombo();
	CStatic mSkillFindStatic;
	CEdit mSkillFindEdit;
	afx_msg void OnEnChangeSkillFindEdit();
	afx_msg void OnCbnSelchangeSkillFindCombo();
	afx_msg void OnBnClickedSkillBuffRemoveButton();
	virtual BOOL PreTranslateMessage( MSG* );
};