// NewPackingToolView.cpp : CNewPackingToolView 클래스의 구현
//

#include "stdafx.h"
#include "NewPackingTool.h"

#include "NewPackingToolDoc.h"
#include "NewPackingToolView.h"
#include ".\newpackingtoolview.h"

#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNewPackingToolView

IMPLEMENT_DYNCREATE(CNewPackingToolView, CEditView)

BEGIN_MESSAGE_MAP(CNewPackingToolView, CEditView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CNewPackingToolView 생성/소멸

CNewPackingToolView::CNewPackingToolView()
	: m_curIndex(-1)
{
}

CNewPackingToolView::~CNewPackingToolView()
{
}

BOOL CNewPackingToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CEditView::PreCreateWindow(cs);
}

// CNewPackingToolView 그리기

void CNewPackingToolView::OnDraw(CDC* /*pDC*/)
{
	CNewPackingToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}


// CNewPackingToolView 인쇄

BOOL CNewPackingToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CNewPackingToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CNewPackingToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CNewPackingToolView 진단

#ifdef _DEBUG
void CNewPackingToolView::AssertValid() const
{
	CView::AssertValid();
}

void CNewPackingToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNewPackingToolDoc* CNewPackingToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewPackingToolDoc)));
	return (CNewPackingToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CNewPackingToolView 메시지 처리기

void CNewPackingToolView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
	{ // 엔터키가 눌려졌을 때
		((CNewPackingToolApp *)AfxGetApp())->m_bTabFlag = TRUE;
	}

	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNewPackingToolView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();

	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	if (pFrame->m_ReservationViewIndex > -1)
	{
		SelChange(pFrame->m_ReservationViewIndex);
		pFrame->m_ReservationViewIndex = -1;
	}

	if (m_curIndex == -1)
		EnableWindow(FALSE);
}

void CNewPackingToolView::SelChange(int index)
{
	BOOL bmodify = FALSE;

	if (m_curIndex != -1)
	{
		CString str;
		GetWindowText(str);

		GetDocument()->SaveData(m_curIndex, str.GetBuffer(0));

		// 편집 상태 저장
        
	}

	SetWindowText( GetDocument()->GetData( index ) );
	m_curIndex = index;

	GetDocument()->SelChange(index, bmodify);

	EnableWindow(TRUE);
}

void CNewPackingToolView::MoveLine(int line)
{
	static CEdit& edit   = GetEditCtrl();

	int nBegin, nEnd;

	nBegin = edit.LineIndex(line-1);

	if (nBegin != -1)
	{
		nEnd = nBegin + edit.LineLength(nBegin);
		edit.SetSel(nBegin, nEnd);
	}
}

int CNewPackingToolView::CheckScript(UINT modeType)
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();
	CString outStr;

	cScriptTestMode* pTestmode = GetScriptTestMode(modeType);

	if (!pTestmode)
	{
		pFrame->AddOutStr(" Can't test script \r\n ");
		return 1;
	}

	pTestmode->StartTest();

	int nErr = 0;
	int groupLine[3];
	int curGroupLine = -1;
	ZeroMemory(groupLine, sizeof(groupLine));

	UINT operType[3];
	ZeroMemory(operType, sizeof(operType));

	CEdit& edit = GetEditCtrl();
	int nLineCount = edit.GetLineCount();
	int line, len;
	for (line=0; line<nLineCount; ++line)
	{
		TCHAR str[1024];
		len = edit.LineLength(edit.LineIndex(line));

		if (len == 0)
			continue;

		edit.GetLine(line, str, len);
		str[len] = '\0';

		TCHAR *token;
		token = strtok( str, g_seps );

		int numTok = 0;

		CStringArray tokenArr;

		while( token != NULL )
		{
			++numTok;

			if (!_tcscmp(token, _T("{")))
			{
				groupLine[++curGroupLine] = line;
				pTestmode->StartGroup();
			}
			else if (!_tcscmp(token, _T("}")))
			{
				if (curGroupLine >= 0)
				{
					groupLine[curGroupLine--] = 0;
					pTestmode->EndGroup();
				}
				else
				{
					// err
					outStr.Format("line# %5d : error : '}' \r\n", line+1);
					pFrame->AddOutStr(outStr);
					++nErr;
				}
			}
			else
			{
				if (pTestmode->CommentTest(token))
					break;

				tokenArr.Add(token);
			}

			//else if (token[0] == '#')
			//{
			//	if (curGroupLine>-1)
			//	{
			//		operType[curGroupLine+1];
			//		paramCheck = true;
			//	}
			//	else
			//	{
			//		CStringArray errStrArr;
			//		if (1 /*CheckBeffOpr(str, nErr, errStrArr)*/)
			//		{
			//			//operType[0] = StrToBeffOpr(token);
			//			paramCheck = true;
			//			break;
			//		}
			//		else
			//		{
			//			// err
			//			int ErrNum;
			//			for (ErrNum=0; ErrNum<errStrArr.GetCount(); ++ErrNum)
			//			{
			//				outStr.Format("line# %5d : %s \r\n", line+1, errStrArr[ErrNum]);
			//				pFrame->AddOutStr(outStr);
			//			}
			//		}
			//	}
			//}
			//else if (!paramCheck)
			//{
			//	// err
			//	outStr.Format("line# %5d : error : '%s' : is undefined symbol \r\n", line+1, token);
			//	pFrame->AddOutStr(outStr);
			//	++nErr;
			//}

			/* Get next token: */
			token = strtok( NULL, g_seps );
		}


		if (tokenArr.GetSize() > 0)
		{
			if (pTestmode->TestLine(tokenArr))
			{
				continue;
			}
			else
			{
				const CStringArray& lastErrArr = pTestmode->GetLastErr();

				if (lastErrArr.GetSize() != 0)
				{
					int nArr;

					for (nArr=0; nArr<lastErrArr.GetSize(); ++nArr)
					{
						outStr.Format("line# %5d : %s \r\n", line+1, lastErrArr.GetAt(nArr));
						pFrame->AddOutStr(outStr);
						++nErr;
					}
				}
			}
		}
	}

	if (curGroupLine > 0)
	{
		while (curGroupLine != -1)
		{
			// err
			outStr.Format("line# %5d : error : '{' is not closed. need '}' \r\n", groupLine[curGroupLine]+1);
			pFrame->AddOutStr(outStr);
			++nErr;
			curGroupLine--;
		}
	}

	return nErr;
}

void CNewPackingToolView::ScriptTest(UINT testType)
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	if (m_curIndex < 0)
	{
		pFrame->AddOutStr(_T("Do not Select File !"));
		return;
	}

	CString outStr;

	// 출력창 초기화
	outStr.Format("Test Start : %s (Type : %s) . \r\n\r\n", GetDocument()->GetTitle(), _T("eSt_Effect"));
	pFrame->AddOutStr(outStr);

	int nErr = 0;
	nErr = CheckScript(testType);

	// 결과 출력
	outStr.Format("----- Complete -----\r\n\r\n 오류 : %d 개\r\n", nErr);
	pFrame->AddOutStr(outStr);
}

void CNewPackingToolView::OnSize(UINT nType, int cx, int cy)
{
	CEditView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//RECT rect;

	//CEditView::GetWindowRect(&rect);

	//this->SetWindowPos(&CWnd::wndBottom, rect.left+20, 0, 0, 0,
	//	SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

}
