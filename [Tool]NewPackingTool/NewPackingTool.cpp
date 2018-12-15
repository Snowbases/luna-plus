// NewPackingTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "NewPackingTool.h"
#include "MainFrm.h"

#include "NewPackingToolDoc.h"
#include "NewPackingToolView.h"
#include ".\newpackingtool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNewPackingToolApp

BEGIN_MESSAGE_MAP(CNewPackingToolApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	//ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_SCRIPT_TEST, OnScriptTest)
END_MESSAGE_MAP()


// CNewPackingToolApp 생성

CNewPackingToolApp::CNewPackingToolApp()
	: m_nTabCount(0), m_bTabFlag(FALSE)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CNewPackingToolApp 개체입니다.

CNewPackingToolApp theApp;

// CNewPackingToolApp 초기화

BOOL CNewPackingToolApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.

	InitCommonControls();

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	// 문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.

	// @@@ 응용프로그램의 시작 폴더에서 찾도록 하자.
	// Path 설정!

	CString ScriptTestPath;

	TCHAR Init_path[MAX_PATH];
	GetModuleFileName(NULL, Init_path, MAX_PATH);

	TCHAR strDir[_MAX_DIR];
	TCHAR strDrv[_MAX_DRIVE];

	_tsplitpath( Init_path, strDrv, strDir, NULL, NULL );

	ScriptTestPath = strDrv;
	ScriptTestPath += strDir;
	ScriptTestPath += _T("ScriptTest.xml");

	g_ScriptTest.LoadXml(ScriptTestPath);

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNewPackingToolDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CNewPackingToolView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// 명령줄에 지정된 명령을 디스패치합니다. 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	char cmd[MAX_PATH] = {0,};
	size_t nCmdLen = strlen( AfxGetApp()->m_lpCmdLine );
	if ( nCmdLen )
	{
		strcpy( cmd, AfxGetApp()->m_lpCmdLine+1 );
		cmd[nCmdLen-2] = 0;

		//OpenFile(cmd);

		//UpdateData( FALSE );
	}

	m_pMainWnd->DragAcceptFiles(TRUE);

	CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();
	CNewPackingToolView* pView   = (CNewPackingToolView*)pFrame->GetActiveView();
	CEdit*  pEdit   = &(pView->GetEditCtrl());

	const UINT max_uint_size = (UINT)-1;
	pEdit->SetLimitText( max_uint_size );

	return TRUE;
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CNewPackingToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CNewPackingToolApp 메시지 처리기


BOOL CNewPackingToolApp::OnIdle(LONG lCount)
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();  // 프레임
	static CStatusBar*  pStatusBar =  (CStatusBar*)pFrame->GetDescendantWindow(AFX_IDW_STATUS_BAR); // 상태바에 대한 포인터  
	//static CChildFrame* pChild;  // 차일드 프레임에 대한 포인터
	static CNewPackingToolView*    pView;   // 뷰에 대한 포인터
	static CEdit*           pEdit;   // 에디트 컨트롤에 대한 포인터

	static CString str;       
	static int posStart;     // Sel의 시작 글자 위치
	static int posEnd;      // Sel의 끝 글자 위치
	static int posFirst;     // 라인의 첫 글자 위치
	static int posPrev;     // 이전 글자 위치, 문자열이 선택된 경우
	// 왼쪽 방향인지 오른쪽 방향인지 알기위해 필요  
	static int nCurLine;   // 현재 라인 수
	static int nCurCol;    // 현재 컬럼 수
	static char buf[1024];

	int i;
	int n;

	//pChild  = (CChildFrame*)pFrame->GetActiveFrame();
	pView   = (CNewPackingToolView*)pFrame->GetActiveView();
	pEdit   = &(pView->GetEditCtrl());

	switch(lCount)
	{
	case 0:
	case 1:

		CWinApp::OnIdle(lCount);
		return TRUE;

	case 2:  // 상태바에 라인 수와 컬럼 수를 표시한다.

		if (!pStatusBar) return TRUE;
		if (!pEdit)
		{
			str.Format("Ln ??, Col ??");
			pStatusBar->SetPaneText(1, str);

			return TRUE;
		}

		pEdit->GetSel(posStart, posEnd);
		if (posStart == posEnd)        // 문자열이 선택되지 않았으면...
		{
			posFirst = pEdit->LineIndex();
			nCurLine = pEdit->LineFromChar() + 1;
			nCurCol = posStart - posFirst + 1;
			posPrev = posStart;
		}
		else                                  // 문자열이 선택돼 있으면
		{
			if (posStart < posPrev)    // 왼쪽 방향으로 선택했으면
			{
				nCurLine = pEdit->LineFromChar(posStart) + 1;
				posFirst = pEdit->LineIndex(nCurLine - 1);
				nCurCol = posStart - posFirst + 1;    // 현재 컬럼은
			}
			// 시작 문자 위치
			else     // 오른쪽 방향으로 선택했으면
			{
				nCurLine = pEdit->LineFromChar(posEnd) + 1;
				posFirst = pEdit->LineIndex(nCurLine - 1);
				nCurCol = posEnd - posFirst + 1;      // 현재 컬럼은
			}
			// 끝 문자 위치
		}

		str.Format(_T("Ln %d, Col %d"), nCurLine, nCurCol);
		pStatusBar->SetPaneText(1, str);
		// 상태바에 현재 라인, 컬럼 표시
		return TRUE;

	case 3:     // 자동 들여쓰기

		if (!pEdit) return TRUE;

		pEdit->GetSel(posStart, posEnd);
		posFirst = pEdit->LineIndex();

		if ((m_bTabFlag == TRUE) && (posStart == posFirst))
		{
			for(i = 0; i < m_nTabCount; i++)    // 이전 라인의 탭 수만큼
			{                                               // 탭을 채운다.
				pEdit->ReplaceSel("\t");
			}
			m_nTabCount = 0;
			m_bTabFlag = FALSE;               // 다시 플래그를 거짓으로
		}
		else       // 탭 플래그가 거짓이면 현재 라인을 읽어와서
		{           // 앞의 탭 문자가 몇 개 인지를 계산한다.
			n = pEdit->GetLine(pEdit->LineFromChar(), buf, 1024);
			i = 0;
			while(i < n && buf[i] == '\t') i++;

			m_nTabCount = i;
		}

		return TRUE;

	case 4:     // 타이틀에 '*' 표시

		if (!pEdit) return FALSE;

		if (pEdit->GetModify())       // 문서가 변경되었으면 
		{
			str = pView->GetDocument()->GetTitle();
			if (str.Right(1) != '*')  // '*' 표시가 되어있지 않으면
			{
				str += '*';
				pView->GetDocument()->SetTitle(str);
			}
		}

		return FALSE;   // 더 이상 Idle 타임 작업이 없음
	}

	return FALSE;


	//return CWinApp::OnIdle(lCount);
}

void CNewPackingToolApp::OnScriptTest()
{
	static CMainFrame*  pFrame = (CMainFrame*)AfxGetMainWnd();

	pFrame->OnScriptTest();
}