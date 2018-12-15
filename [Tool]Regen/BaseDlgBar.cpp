// BaseDlgBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RegenTool.h"
#include "BaseDlgBar.h"
#include ".\basedlgbar.h"


// CBaseDlgBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBaseDlgBar, CDialog)
CBaseDlgBar::CBaseDlgBar(CWnd* pParent /*=NULL*/)
	: CDialog(CBaseDlgBar::IDD, pParent)
{
	m_pSheet = NULL;
}

CBaseDlgBar::~CBaseDlgBar()
{
}

void CBaseDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBaseDlgBar, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CBaseDlgBar 메시지 처리기입니다.

BOOL CBaseDlgBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *pSheetHolder = GetDlgItem(IDC_SHEETHOLDER);
	m_pSheet = new CDlgBarSheet( "DlgBarSheet", pSheetHolder );

	if( !m_pSheet->Create( this, WS_CHILD | WS_VISIBLE, 0) )
	{
		delete m_pSheet;
		m_pSheet = NULL;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CBaseDlgBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect sz;
	GetClientRect( sz );
	sz.DeflateRect( 5,5 );

	CWnd *pSheetHolder = GetDlgItem(IDC_SHEETHOLDER);
	if( !pSheetHolder )
		return;

	pSheetHolder->MoveWindow( sz );
	m_pSheet->MoveWindow( sz );
}
