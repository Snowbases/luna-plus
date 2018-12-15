// NewPackingToolDoc.cpp : CNewPackingToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "NewPackingTool.h"

#include "NewPackingToolDoc.h"
#include ".\newpackingtooldoc.h"

#include "MainFrm.h"
#include "NewPackingToolView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNewPackingToolDoc

IMPLEMENT_DYNCREATE(CNewPackingToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CNewPackingToolDoc, CDocument)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
END_MESSAGE_MAP()


// CNewPackingToolDoc 생성/소멸

CNewPackingToolDoc::CNewPackingToolDoc()
	: m_nFileNum(0)
{
}

CNewPackingToolDoc::~CNewPackingToolDoc()
{

}

BOOL CNewPackingToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

// CNewPackingToolDoc serialization

void CNewPackingToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CNewPackingToolDoc 진단

#ifdef _DEBUG
void CNewPackingToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNewPackingToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CNewPackingToolDoc 명령

BOOL CNewPackingToolDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	TCHAR strDir[_MAX_DIR];
	TCHAR strDrv[_MAX_DRIVE];
	TCHAR strFileName[200];
	TCHAR strExt[20];

	_tsplitpath( lpszPathName, strDrv, strDir, strFileName, strExt );

	strupr(strExt);

	if ( strcmp( strExt, ".BIN" ) == 0 || 
		strcmp( strExt, ".BEFF" ) == 0 || 
		strcmp( strExt, ".BEFL" ) == 0 || 
		strcmp( strExt, ".BMHM" ) == 0 || 
		strcmp( strExt, ".BSAD" ) == 0 )
	{
		if ( m_FileMng.AddFile( lpszPathName, "rb" ) )
		{
			pFrame->AddFile(m_FileMng.GetFileName( m_nFileNum ));
			++m_nFileNum;
		}
	}
	else if ( strcmp( strExt, ".DOF" ) == 0 )
	{
		if ( m_FileMng.AddFile( lpszPathName, "dof" ) )
		{
			pFrame->AddFile(m_FileMng.GetFileName( m_nFileNum ));
			++m_nFileNum;
		}
	}
	else
	{
		if ( m_FileMng.AddFile( lpszPathName, "rt" ) )
		{
			pFrame->AddFile(m_FileMng.GetFileName( m_nFileNum ));
			++m_nFileNum;
		}
	}

	return TRUE;
}

void CNewPackingToolDoc::OnFileNew()
{
	CFileDialog dlg( FALSE, NULL, "*.bin", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Binary 파일|*.bin; *.beff; *.befl; *.bmhm; *.bsad|모든 파일 (*.*)|*.*||", NULL );

	if ( dlg.DoModal() == IDOK )
	{
		CString lpszPathName = dlg.GetPathName();

		m_FileMng.New( lpszPathName );

		static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->AddFile(m_FileMng.GetFileName( m_nFileNum ));

		++m_nFileNum;
	}
}

void CNewPackingToolDoc::OnFileSave()
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	int index = pFrame->GetCurIndex();

	if (index == -1)
		return;

	CNewPackingToolView* pView = (CNewPackingToolView*)pFrame->GetActiveView();

	CString str;
	static CEdit* pEdit   = &(pView->GetEditCtrl());

	if (pEdit->GetModify())       // 문서가 변경되었으면 
	{
		str = pView->GetDocument()->GetTitle();

		if (str.Right(1) == '*')  // '*' 표시가 되어 있으면
		{
			str = str.Left(str.GetLength()-1);
			pView->GetDocument()->SetTitle(str);
		}
		pEdit->SetModify(FALSE);

		// 현재 화면 View 저장.
		pView->GetWindowText(str);
		SaveData(index, str.GetBuffer(0));

		// 파일 저장.
		m_FileMng.Save(index, pFrame->m_bUseAES);

		// 결과출력
		CString outStr;
		outStr.Format("Save File ( %s ) is Complete. \r\n", m_FileMng.GetFileName(index));
		pFrame->AddOutStr(outStr);
	}
}

void CNewPackingToolDoc::OnFileSaveAs()
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	int index = pFrame->GetCurIndex();

	if (index == -1)
		return;

	CNewPackingToolView* pView = (CNewPackingToolView*)pFrame->GetActiveView();

	// 현재 화면 View 저장.
	CString str;
	pView->GetWindowText(str);
	SaveData(index, str.GetBuffer(0));

	CFileDialog dlg( FALSE, NULL, "*.bin", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Binary 파일|*.bin; *.beff; *.befl; *.bmhm; *.bsad|모든 파일 (*.*)|*.*||", NULL );

	if ( dlg.DoModal() == IDOK )
	{
		CString lpszPathName = dlg.GetPathName();

		// 파일 저장.
		m_FileMng.SaveAs(index, lpszPathName, pFrame->m_bUseAES);
		pFrame->ChangeFile(index, m_FileMng.GetFileName(index));

		// 결과출력
		CString outStr;
		outStr.Format("Save File ( %s ) is Complete. \r\n", dlg.GetFileName());
		pFrame->AddOutStr(outStr);

		SetPathName(dlg.GetPathName(), TRUE);
		SetModifiedFlag(FALSE);

		static CEdit* pEdit   = &(pView->GetEditCtrl());

		pEdit->SetModify(FALSE);
		pView->GetDocument()->SetTitle(dlg.GetFileName());

	}
}


char* CNewPackingToolDoc::GetData(int index)
{
	CMHFileEx* pActiveFile = m_FileMng.GetFile( index );

	if (pActiveFile == NULL)
		return NULL;

	return pActiveFile->GetData();
}

void CNewPackingToolDoc::SaveData(int index, LPTSTR str)
{
	// desc_hseos_파일끝의 공백문자 제거_01
	// S 파일끝의 공백문자 제거 추가 added by hseos 2007.05.22
	// ..파일끝에 공백문자나 리턴이 들어가면 문제가 되는 경우가 있기 때문에 툴에서 제거한다.
	int nLen = lstrlen(str);
	for (int i=nLen-1; i>= 0; i--)
	{
		if (str[i] == 0x20 || 
			str[i] == 0x09)
		{
			str[i] = NULL;
		}
		else if (str[i-1] == 0x0d && str[i] == 0x0a)
		{
			str[i-1] = NULL;
			str[i] = NULL;
			i--;
		}
		else
		{
			break;
		}
	}

	m_FileMng.SetFileData( index, str );
}

void CNewPackingToolDoc::SelChange(int index, BOOL modified)
{
	SetPathName(m_FileMng.GetFullFileName(index), FALSE);
	SetModifiedFlag(modified);
}

BOOL CNewPackingToolDoc::OnSaveDocument(LPCTSTR /*lpszPathName*/)
{
	OnFileSave();

	return TRUE;
//	return CDocument::OnSaveDocument(lpszPathName);
}
