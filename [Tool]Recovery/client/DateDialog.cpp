// DateDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "DateDialog.h"
#include ".\datedialog.h"


// CDateDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC( CDateDialog, CDialog)
CDateDialog::CDateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDateDialog::IDD, pParent)
{
	EnableAutomation();
}

CDateDialog::~CDateDialog()
{}

void CDateDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.
	
	CDialog::OnFinalRelease();
}

void CDateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATE_PREVIOUS_DATA_STATIC, mPreviousDateStatic);
	DDX_Control(pDX, IDC_DATE_YEAR_COMBO, mYearCombo);
	DDX_Control(pDX, IDC_DATE_MONTH_COMBO, mMonthCombo);
	DDX_Control(pDX, IDC_DATE_DAY_COMBO, mDayCombo);
	DDX_Control(pDX, IDC_DATE_HOUR_COMBO, mHourCombo);
	DDX_Control(pDX, IDC_DATE_MINUTE_COMBO, mMinuteCombo);
	DDX_Control(pDX, IDC_DATE_SECOND_COMBO, mSecondCombo);
}


BEGIN_MESSAGE_MAP(CDateDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDateDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IDateDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {6D446E54-0FAF-4065-B28D-B21497A84B11}
static const IID IID_IDateDialog =
{ 0x6D446E54, 0xFAF, 0x4065, { 0xB2, 0x8D, 0xB2, 0x14, 0x97, 0xA8, 0x4B, 0x11 } };

BEGIN_INTERFACE_MAP(CDateDialog, CDialog)
	INTERFACE_PART(CDateDialog, IID_IDateDialog, Dispatch)
END_INTERFACE_MAP()


// CDateDialog 메시지 처리기입니다.

BOOL CDateDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 날짜 콤보 세팅
	{
		const DWORD minute	= 60;
		const DWORD hour	= 60 * minute;
		const DWORD day		= 24 * hour;
		const DWORD month	= 30 * day;
		const DWORD year	= 12 * month;

		CRect	rect;
		CString text;

		{
			mYearCombo.GetWindowRect( rect );
			mYearCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxYear = 5;

			for( int i = 0; i <= maxYear; ++i )
			{
				text.Format( _T( "%d" ), i );

				mYearCombo.AddString( text );
			}

			mYearCombo.SetCurSel( mSecond / year );

			mSecond %= year;
		}

		{
			mMonthCombo.GetWindowRect( rect );
			mMonthCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxMonth = 12;

			for( int i = 0; i <= maxMonth; ++i )
			{
				text.Format( _T( "%d" ), i );

				mMonthCombo.AddString( text );
			}

			mMonthCombo.SetCurSel( mSecond / month );

			mSecond %= month;
		}

		{
			mDayCombo.GetWindowRect( rect );
			mDayCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxDay = 30;

			for( int i = 0; i <= maxDay; ++i )
			{
				text.Format( _T( "%d" ), i );

				mDayCombo.AddString( text );
			}

			mDayCombo.SetCurSel( mSecond / day );

			mSecond %= day;
		}

		{
			mHourCombo.GetWindowRect( rect );
			mHourCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxHour = 24;

			for( int i = 0; i <= maxHour; ++i )
			{
				text.Format( _T( "%d" ), i );

				mHourCombo.AddString( text );
			}

			mHourCombo.SetCurSel( mSecond / hour );

			mSecond %= hour;
		}

		{
			mMinuteCombo.GetWindowRect( rect );
			mMinuteCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxMinute = 60;

			for( int i = 0; i <= maxMinute; ++i )
			{
				text.Format( _T( "%d" ), i );

				mMinuteCombo.AddString( text );
			}

			mMinuteCombo.SetCurSel( mSecond / minute );

			mSecond %= minute;
		}

		{
			mSecondCombo.GetWindowRect( rect );
			mSecondCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const int maxSecond = 60;

			for( int i = 0; i <= maxSecond; ++i )
			{
				text.Format( _T( "%d" ), i );

				mSecondCombo.AddString( text );
			}

			mSecondCombo.SetCurSel( mSecond );
		}
	}

	{
		CString text;
		text.LoadString( IDS_DATE_DIALOG_TEXT1 );

		mPreviousDateStatic.SetWindowText( text + mDateString );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


INT_PTR CDateDialog::DoModal( DWORD second, const CString& dateString )
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	mSecond		= second;
	mDateString	= dateString;

	return CDialog::DoModal();
}


void CDateDialog::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	const DWORD second	= 1;
	const DWORD minute	= 60 * second;
	const DWORD hour	= 60 * minute;
	const DWORD day		= 24 * hour;
	const DWORD month	= 30 * day;
	const DWORD year	= 12 * month;
	
	CString text;
	mSecond	= 0;

	mYearCombo.GetLBText( mYearCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) ) * year;

	mMonthCombo.GetLBText( mMonthCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) ) * month;

	mDayCombo.GetLBText( mDayCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) ) * day;

	mHourCombo.GetLBText( mHourCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) ) * hour;

	mMinuteCombo.GetLBText( mMinuteCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) ) * minute;

	mSecondCombo.GetLBText( mSecondCombo.GetCurSel(), text );
	mSecond += ( text.IsEmpty() ? 0 : _ttoi( text ) );

	CDialog::OnOK();
}


DWORD CDateDialog::GetSecond() const
{
	return mSecond;
}