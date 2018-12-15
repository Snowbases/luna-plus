// OutPutDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NewPackingTool.h"
#include "OutPutDlg.h"
#include ".\outputdlg.h"

#include "ScriptTestStr.h"
#include "MainFrm.h"
#include "NewPackingToolView.h"

// COutPutDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COutPutDlg, CDialog)
COutPutDlg::COutPutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COutPutDlg::IDD, pParent), m_bInit(FALSE), m_strOut(_T(""))
{
}

COutPutDlg::~COutPutDlg()
{
}

void COutPutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT_EDIT, m_cOutput);
	DDX_Text(pDX, IDC_OUTPUT_EDIT, m_strOut);
}


BEGIN_MESSAGE_MAP(COutPutDlg, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_SCRIPT_TEST, OnScriptTest)
	//ON_NOTIFY(NM_DBLCLK, IDC_OUTPUT_EDIT, OnLButtonDblClk);
END_MESSAGE_MAP()


// COutPutDlg 메시지 처리기입니다.

BOOL COutPutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInit = TRUE;

	m_strOut += "////////////////////////////////////////////////////////////////////////////\r\n\r\n";
	m_strOut += "   Hello!! New Packing Tool by Iros.\r\n\r\n";
	m_strOut += "////////////////////////////////////////////////////////////////////////////\r\n\r\n";

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void COutPutDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_bInit)
		return;

	CRect sz;
	GetClientRect(sz);
	sz.DeflateRect(5,5);

	m_cOutput.MoveWindow(sz);
}

void COutPutDlg::Reset()
{
	m_strOut = _T("");

	UpdateData(FALSE);
}

void COutPutDlg::AddOutStr(LPCTSTR str)
{
	m_strOut += str;

	UpdateData(FALSE);

	m_cOutput.SetSel(-2,-1); 
}

void COutPutDlg::MoveLine()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nCurLine = m_cOutput.LineFromChar();

	TCHAR str[1024];
	int len = m_cOutput.LineLength(m_cOutput.LineIndex(nCurLine));

	if (len == 0)
		return;

	m_cOutput.GetLine(nCurLine, str, len);


	TCHAR *token;
	token = strtok( str, g_seps );

	while (token != NULL)
	{
		if (_tcsstr(str, _T("line#")))
		{
			token = strtok( NULL, g_seps );
			if (token)
			{
				int moveLine = _ttoi(token);

				static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->MoveLine(moveLine);
			}

			return;
		}

		token = strtok( NULL, g_seps );
	}

	//CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL COutPutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( pMsg->message == WM_LBUTTONDBLCLK )
	{
		if (pMsg->hwnd == GetDlgItem(IDC_OUTPUT_EDIT)->m_hWnd)		
		{
			MoveLine();
		}
	}

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		//TranslateAccelerator(pMsg);
		//OnScriptTest();
		// Translate the message using accelerator table
		/*ASSERT(m_hAccel);
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		{
			return TRUE;
		}*/
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void COutPutDlg::OnScriptTest()
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	pFrame->OnScriptTest();
}