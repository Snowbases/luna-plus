// NameInputDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "UserRestoreDialog.h"
//#include "UserDialog.h"
//#include ".\userrestoredialog.h"


// CUserRestoreDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUserRestoreDialog, CDialog)
CUserRestoreDialog::CUserRestoreDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CUserRestoreDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();
}

CUserRestoreDialog::~CUserRestoreDialog()
{
}

void CUserRestoreDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CUserRestoreDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME_INPUT_EDIT, mNameEdit);
}


BEGIN_MESSAGE_MAP(CUserRestoreDialog, CDialog)
	//ON_BN_CLICKED(IDC_USER_RESTORE_SUBMIT_BUTTON, OnBnClickedUserRestoreSubmitButton)
	//ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CUserRestoreDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_INameInputDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {3484BB64-9DB7-45D4-AF86-758A2EFC25E5}
static const IID IID_INameInputDialog =
{ 0x3484BB64, 0x9DB7, 0x45D4, { 0xAF, 0x86, 0x75, 0x8A, 0x2E, 0xFC, 0x25, 0xE5 } };

BEGIN_INTERFACE_MAP(CUserRestoreDialog, CDialog)
	INTERFACE_PART(CUserRestoreDialog, IID_INameInputDialog, Dispatch)
END_INTERFACE_MAP()


// CUserRestoreDialog 메시지 처리기입니다.


//CString CUserRestoreDialog::GetName() const
//{
//	//CString name;
//
//	//mNameEdit.GetWindowText( name );
//
//	//return name;
//	return mPlayerName;
//}


INT_PTR CUserRestoreDialog::DoModal( const CString& playerName )
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	mPlayerName = playerName;

	return CDialog::DoModal();
}


BOOL CUserRestoreDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	{
		mNameEdit.SetLimitText( MAX_NAME_LENGTH );
		mNameEdit.SetWindowText( mPlayerName );
		mNameEdit.SetSel( 0, mNameEdit.GetWindowTextLength() );
		mNameEdit.SetFocus();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUserRestoreDialog::OnOK()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString name;

	mNameEdit.GetWindowText( name );

	if( name.IsEmpty() )
	{
		CString textEmptyValueFounded;
		textEmptyValueFounded.LoadString( IDS_STRING185 );

		MessageBox( textEmptyValueFounded, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	mPlayerName = name;
	
	CDialog::OnOK();
}


const CString& CUserRestoreDialog::GetName() const
{
	return mPlayerName;
}