// ExtractSelectedDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PackExtract.h"
#include "PackExtractDlg.h"
#include "ExtractSelectedDlg.h"
#include ".\extractselecteddlg.h"

extern CPackExtractDlg* g_pPackExtractDlg;

// ExtractSelectedDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ExtractSelectedDlg, CDialog)
ExtractSelectedDlg::ExtractSelectedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExtractSelectedDlg::IDD, pParent)
	, m_bClearDir(FALSE)
{
}

ExtractSelectedDlg::~ExtractSelectedDlg()
{
	g_pPackExtractDlg->EndExtractPackFileWithSelectListDlg();
}

void ExtractSelectedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_SeletList);
	DDX_Check(pDX, IDC_CLEAR_DIR_CHECK, m_bClearDir);
}


BEGIN_MESSAGE_MAP(ExtractSelectedDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_DELTE_BUTTON, OnBnClickedDelteButton)
END_MESSAGE_MAP()


// ExtractSelectedDlg 메시지 처리기입니다.

BOOL ExtractSelectedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	g_pPackExtractDlg->AddListFromSelectedPackFile( m_SeletList );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void ExtractSelectedDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCount = m_SeletList.GetSelCount();

	if( nCount )
	{
		UpdateData( TRUE );

		CArray<int,int> aryListBoxSel;

		aryListBoxSel.SetSize( nCount );
		m_SeletList.GetSelItems( nCount, aryListBoxSel.GetData() );

		g_pPackExtractDlg->SetPackProgressRange( nCount );
		g_pPackExtractDlg->StartExtractPackFileWithSelectListDlg( m_bClearDir );

		for( int i=0; i < nCount ; ++i )
		{
			CString strFileName;
			m_SeletList.GetText( aryListBoxSel[i], strFileName );

			g_pPackExtractDlg->ExtractPackFile( (LPSTR)(LPCTSTR)strFileName );

			g_pPackExtractDlg->SetPackProgressPos( i + 1 );
		}

		g_pPackExtractDlg->EndExtractPackFileWithSelectListDlg();

		AfxMessageBox( "압축해제 완료!" );
	}	

	OnOK();
}

void ExtractSelectedDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_pPackExtractDlg->EndExtractPackFileWithSelectListDlg();

	OnCancel();
}

void ExtractSelectedDlg::OnBnClickedDelteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nCount = m_SeletList.GetSelCount();

	if( nCount )
	{
		UpdateData( TRUE );

		CArray<int,int> aryListBoxSel;
		CArray<CString> arySelectedFileName;

		aryListBoxSel.SetSize( nCount );
		m_SeletList.GetSelItems( nCount, aryListBoxSel.GetData() );

		g_pPackExtractDlg->SetPackProgressRange( nCount );
		if( !g_pPackExtractDlg->StartDeletePackFileWithSelectListDlg() )
			return;

		for( int i=0; i < nCount ; ++i )
		{
			CString strFileName;
			m_SeletList.GetText( aryListBoxSel[i], strFileName );

			g_pPackExtractDlg->DeletePackFile( (LPSTR)(LPCTSTR)strFileName );

			g_pPackExtractDlg->SetPackProgressPos( i + 1 );

			arySelectedFileName.Add( strFileName );
		}

		for( int i=0 ; i < arySelectedFileName.GetCount() ; ++i )
		{
			int nIndex = 0;
			nIndex = m_SeletList.FindString( nIndex, arySelectedFileName[i] );

			m_SeletList.DeleteString( nIndex );
		}

		g_pPackExtractDlg->EndDeletePackFileWithSelectListDlg();

		AfxMessageBox( "파일 제거 완료!" );
	}
}
