// PasswordDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "PasswordDialog.h"
#include ".\passworddialog.h"


// CPasswordDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPasswordDialog, CDialog)
CPasswordDialog::CPasswordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDialog::IDD, pParent)
{
	EnableAutomation();

}

CPasswordDialog::~CPasswordDialog()
{
}

void CPasswordDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CPasswordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PASSWORD_PREV_EDIT, mPreviousPasswordEdit);
	DDX_Control(pDX, IDC_PASSWORD_NEXT_EDIT, mNextPasswordEdit);
	DDX_Control(pDX, IDC_PASSWORD_NEXT_CONFIRM_EDIT, mConfirmPasswordEdit);
}


BEGIN_MESSAGE_MAP(CPasswordDialog, CDialog)
	//ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPasswordDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IPasswordDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {75C97B48-E9A7-4909-84F5-1AE4525A95D7}
static const IID IID_IPasswordDialog =
{ 0x75C97B48, 0xE9A7, 0x4909, { 0x84, 0xF5, 0x1A, 0xE4, 0x52, 0x5A, 0x95, 0xD7 } };

BEGIN_INTERFACE_MAP(CPasswordDialog, CDialog)
	INTERFACE_PART(CPasswordDialog, IID_IPasswordDialog, Dispatch)
END_INTERFACE_MAP()


// CPasswordDialog 메시지 처리기입니다.


void CPasswordDialog::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CString previousPassword;
	mPreviousPasswordEdit.GetWindowText( previousPassword );

	CString nextPassword;
	{
		mNextPasswordEdit.GetWindowText( nextPassword );

		CString password;
		mConfirmPasswordEdit.GetWindowText( password );

		if( nextPassword != password )
		{
			mConfirmPasswordEdit.SetWindowText( _T( "" ) );
			mNextPasswordEdit.SetWindowText( _T( "" ) );

			CString textToConfirmPasswordIsWrong;
			textToConfirmPasswordIsWrong.LoadString( IDS_STRING153 );

			MessageBox( textToConfirmPasswordIsWrong, _T( "" ), MB_OK | MB_ICONERROR );
			return;
		}
	}

	const int minimumSize = 6;

	if( minimumSize > nextPassword.GetLength() )
	{
		CString textPasswordMustHaveSpecifiedSize;
		textPasswordMustHaveSpecifiedSize.LoadString( IDS_STRING154 );

		CString text;
		text.Format( textPasswordMustHaveSpecifiedSize, minimumSize );

		MessageBox( text, _T( "" ), MB_OK | MB_ICONERROR );
		return;
	}
	
	{
		char buffer[ MAX_PATH ] = { 0 };
		StringCopySafe( buffer, CW2AEX< MAX_PATH >( nextPassword ), sizeof( buffer ) );

		for(
			const char* ch = buffer;
			*ch;
			++ch )
		{
			if(		IsDBCSLeadByte( *ch )	||
				!	isalnum( *ch ) )
			{
				CString textItIsAllowToUsePassword;
				textItIsAllowToUsePassword.LoadString( IDS_STRING155 );

				MessageBox( textItIsAllowToUsePassword, _T( "" ), MB_OK | MB_ICONERROR );
				return;
			}

			if( ! isalpha( *ch ) &&
				! isdigit( *ch ) )
			{
				CString textYouShouldToMakePasswordWithMixingEnglishAndNumeric;
				textYouShouldToMakePasswordWithMixingEnglishAndNumeric.LoadString( IDS_STRING156 );

				MessageBox( textYouShouldToMakePasswordWithMixingEnglishAndNumeric, _T( "" ), MB_OK | MB_ICONERROR );
				return;
			}
		}
	}

	mPreviousPassword	= previousPassword;
	mNextPassword		= nextPassword;

	CDialog::OnOK();
}


const TCHAR* CPasswordDialog::GetPreviousPassword() const
{
	return mPreviousPassword;
}


const TCHAR* CPasswordDialog::GetNextPassword() const
{
	return mNextPassword;
}