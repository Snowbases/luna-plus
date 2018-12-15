// ScriptViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegenTool.h"
#include "ScriptViewDlg.h"
#include ".\scriptviewdlg.h"


// CScriptViewDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CScriptViewDlg, CDialog)
CScriptViewDlg::CScriptViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScriptViewDlg::IDD, pParent)
	, m_strScriptEdit(_T(""))
{
}

CScriptViewDlg::~CScriptViewDlg()
{
}

void CScriptViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCRIPT_EDIT, m_strScriptEdit);
}


BEGIN_MESSAGE_MAP(CScriptViewDlg, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CScriptViewDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	PostMessage(
		WM_PAINT);
}

void CScriptViewDlg::OnPaint()
{
	if(IsWindowVisible())
	{
		CDialog::OnPaint();
		return;
	}

	CWnd* const window = GetDlgItem(
		IDC_SCRIPT_EDIT);

	if(0 == window)
	{
		return;
	}
	else if(GetFocus() == window)
	{
		return;
	}

	window->SetFocus();
}

void CScriptViewDlg::OnOK()
{
	PostMessage(
		WM_KEYDOWN,
		VK_ESCAPE);

	CDialog::OnOK();
}