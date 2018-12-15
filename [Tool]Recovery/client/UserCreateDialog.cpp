// UserCreateDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "UserCreateDialog.h"
#include ".\usercreatedialog.h"
//#include "UserDialog.h"


// CUserCreateDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUserCreateDialog, CDialog)
CUserCreateDialog::CUserCreateDialog( CclientApp& application, CWnd* pParent /*=NULL*/)	
	: CDialog(CUserCreateDialog::IDD, pParent)
	, mApplication( application )

{
	EnableAutomation();
}

CUserCreateDialog::~CUserCreateDialog()
{
}

void CUserCreateDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CUserCreateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER_CREATE_RACE_COMBO, mRaceCombo);
	DDX_Control(pDX, IDC_USER_CREATE_JOB_COMBO, mJobCombo);
	DDX_Control(pDX, IDC_USER_CREATE_JOB_GENDER_COMBO, mGenderCombo);
	DDX_Control(pDX, IDC_USER_CREATE_HAIR_COMBO, mHairCombo);
	DDX_Control(pDX, IDC_USER_CREATE_FACE_COMBO, mFaceCombo);
	DDX_Control(pDX, IDC_USER_CREATE_NAME_EDIT, mNameEdit);
}


BEGIN_MESSAGE_MAP(CUserCreateDialog, CDialog)
	//ON_BN_CLICKED(IDC_USER_CREATE_BUTTON, OnBnClickedUserCreateButton)
	//ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CUserCreateDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IUserCreateDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {10193BC3-1521-440A-92DB-FBDEF26FDF85}
static const IID IID_IUserCreateDialog =
{ 0x10193BC3, 0x1521, 0x440A, { 0x92, 0xDB, 0xFB, 0xDE, 0xF2, 0x6F, 0xDF, 0x85 } };

BEGIN_INTERFACE_MAP(CUserCreateDialog, CDialog)
	INTERFACE_PART(CUserCreateDialog, IID_IUserCreateDialog, Dispatch)
END_INTERFACE_MAP()


// CUserCreateDialog 메시지 처리기입니다.

BOOL CUserCreateDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// edit box
	{
		mNameEdit.SetLimitText( MAX_NAME_LENGTH );
	}

	// 콤보 박스 세팅
	{
		CRect		rect;

		// 종족
		{
			mRaceCombo.GetWindowRect( rect );
			mRaceCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			// 080819 LUJ, 메시지를 게임 리소스에서 가져옴
			const TCHAR* textHuman = mApplication.GetInterfaceMessage( 247 );
			mRaceCombo.AddString( textHuman );
			// 080819 LUJ, 메시지를 게임 리소스에서 가져옴
			const TCHAR* textElf = mApplication.GetInterfaceMessage( 248 );
			mRaceCombo.AddString( textElf );

			mRaceCombo.SetCurSel( 0 );
		}

		// 클래스
		{
			mJobCombo.GetWindowRect( rect );
			mJobCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			CString textFighter;
			textFighter.LoadString( IDS_STRING207 );
			mJobCombo.AddString( textFighter );

			CString textMage;
			textMage.LoadString( IDS_STRING208 );
			mJobCombo.AddString( textMage );

			CString textRogue;
			textRogue.LoadString( IDS_STRING209 );
			mJobCombo.AddString( textRogue );

			mJobCombo.SetCurSel( 0 );
		}

		// 성별
		{
			mGenderCombo.GetWindowRect( rect );
			mGenderCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			CString textMale;
			textMale.LoadString( IDS_STRING178 );
			mGenderCombo.AddString( textMale );

			CString textFemale;
			textFemale.LoadString( IDS_STRING177 );
			mGenderCombo.AddString( textFemale );

			mGenderCombo.SetCurSel( 0 );
		}

		// 머리
		{
			mHairCombo.GetWindowRect( rect );
			mHairCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			CString textHead;
			textHead.LoadString( IDS_STRING210 );

			mHairCombo.AddString( textHead + _T( "1" ) );
			mHairCombo.AddString( textHead + _T( "2" ) );
			mHairCombo.AddString( textHead + _T( "3" ) );
			mHairCombo.AddString( textHead + _T( "4" ) );
			mHairCombo.AddString( textHead + _T( "5" ) );
			mHairCombo.AddString( textHead + _T( "6" ) );
			mHairCombo.AddString( textHead + _T( "7" ) );
			mHairCombo.AddString( textHead + _T( "8" ) );
			mHairCombo.AddString( textHead + _T( "9" ) );
			mHairCombo.AddString( textHead + _T( "10" ) );
			
			mHairCombo.SetCurSel( 0 );
		}

		// 얼굴
		{
			mFaceCombo.GetWindowRect( rect );
			mFaceCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			CString textFace;
			textFace.LoadString( IDS_STRING211 );

			mFaceCombo.AddString( textFace + _T( "1" ) );
			mFaceCombo.AddString( textFace + _T( "2" ) );
			mFaceCombo.AddString( textFace + _T( "3" ) );
			mFaceCombo.AddString( textFace + _T( "4" ) );
			mFaceCombo.AddString( textFace + _T( "5" ) );
			mFaceCombo.AddString( textFace + _T( "6" ) );
			mFaceCombo.AddString( textFace + _T( "7" ) );
			mFaceCombo.AddString( textFace + _T( "8" ) );
			mFaceCombo.AddString( textFace + _T( "9" ) );
			mFaceCombo.AddString( textFace + _T( "10" ) );

			mFaceCombo.SetCurSel( 0 );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CUserCreateDialog::OnOK()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString name;
	mNameEdit.GetWindowText( name );

	if( name.IsEmpty() )
	{
		CString textEmptyValueExist;
		textEmptyValueExist.LoadString( IDS_STRING185 );

		MessageBox( textEmptyValueExist, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CDialog::OnOK();

	mRace	= mRaceCombo.GetCurSel();
	mJob	= mJobCombo.GetCurSel() + 1;
	mGender	= mGenderCombo.GetCurSel();
	mHair	= mHairCombo.GetCurSel();
	mFace	= mFaceCombo.GetCurSel();
	mName	= name;
}


int CUserCreateDialog::GetRace() const
{
	return mRace;
}


int CUserCreateDialog::GetJob() const
{
	return mJob;
}


int CUserCreateDialog::GetGender() const
{
	return mGender;
}


int CUserCreateDialog::GetHair() const
{
	return mHair;
}


int	CUserCreateDialog::GetFace() const
{
	return mFace;
}


const CString& CUserCreateDialog::GetName() const
{
	return mName;
}