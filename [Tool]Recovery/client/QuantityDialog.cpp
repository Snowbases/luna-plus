// QuantityDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "QuantityDialog.h"
#include ".\quantitydialog.h"


// CQuantityDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuantityDialog, CDialog)
CQuantityDialog::CQuantityDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CQuantityDialog::IDD, pParent)
{
	EnableAutomation();

}

CQuantityDialog::~CQuantityDialog()
{
}

void CQuantityDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CQuantityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QUANTITY_COMBO, mQuantityCombo);
}


BEGIN_MESSAGE_MAP(CQuantityDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CQuantityDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IQuantityDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {274BE6CD-CD99-4766-A494-7D8A2C249616}
static const IID IID_IQuantityDialog =
{ 0x274BE6CD, 0xCD99, 0x4766, { 0xA4, 0x94, 0x7D, 0x8A, 0x2C, 0x24, 0x96, 0x16 } };

BEGIN_INTERFACE_MAP(CQuantityDialog, CDialog)
	INTERFACE_PART(CQuantityDialog, IID_IQuantityDialog, Dispatch)
END_INTERFACE_MAP()


// CQuantityDialog 메시지 처리기입니다.

void CQuantityDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString quantity;

	//mQuantityCombo.GetLBText( mQuantityCombo.GetCurSel(), quantity );
	mQuantityCombo.GetWindowText( quantity );
	
	mQuantity = _ttoi( quantity );

	OnOK();
}


BOOL CQuantityDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// combo box
	{
		CRect rect;

		mQuantityCombo.GetWindowRect( rect );
		mQuantityCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

		CString text;

		for( int i = 1; MAX_YOUNGYAKITEM_DUPNUM >= i; ++i )
		{			
			text.Format( _T( "%d" ), i );

			mQuantityCombo.AddString( text );
		}

		mQuantityCombo.SetCurSel( 0 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


int CQuantityDialog::GetQuantity() const
{
	return mQuantity;
}