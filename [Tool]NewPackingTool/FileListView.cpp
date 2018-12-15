// FileListView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NewPackingTool.h"
#include "FileListView.h"


// CFileListView

IMPLEMENT_DYNCREATE(CFileListView, CListView)

CFileListView::CFileListView()
{
}

CFileListView::~CFileListView()
{
}

BEGIN_MESSAGE_MAP(CFileListView, CListView)
END_MESSAGE_MAP()


// CFileListView 진단입니다.

#ifdef _DEBUG
void CFileListView::AssertValid() const
{
	CListView::AssertValid();
}

void CFileListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CFileListView 메시지 처리기입니다.
