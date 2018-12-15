#pragma once
#include "afxcmn.h"

// CFileListDlg 대화 상자입니다.
struct stFileListItem
{
	BOOL modify;
	int curPos;

	stFileListItem()
		: modify(FALSE), curPos(0)
	{
	}
};

class CFileListDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileListDlg)

public:
	CFileListDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileListDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FILELIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL		m_bInit;
	CListCtrl	m_cFileList;

public:
	void			ResetAll();
	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	void			UpdateList();
	int				AddFile(LPCTSTR filename);
	BOOL			ChangeFile(int index, LPCTSTR filename);
	afx_msg void	OnLvnItemchangedFilelist(NMHDR *pNMHDR, LRESULT *pResult);

	void			SaveEditStats(int index, CEditView* pView);
	void			LoadEditStats(int index, CEditView* pView);

	afx_msg void	OnScriptTest();
};
