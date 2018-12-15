#pragma once



// CDlgBarSheet
#include "SheetPageMonster.h"

class CDlgBarSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDlgBarSheet)

	CSheetPageMonster m_PageMonster;

public:
	CDlgBarSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDlgBarSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CDlgBarSheet();

	CWnd* GetMonsterPage() { return &m_PageMonster; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


