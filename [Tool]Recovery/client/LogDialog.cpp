// LogDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "LogDialog.h"
#include "ItemLogCommand.h"


// CLogDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)
CLogDialog::CLogDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CLogDialog::IDD, pParent),
mApplication( application ),
mCommand( 0 ),
mServerIndex( 0 )
{
	EnableAutomation();
}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_RESULT_LIST, mResultListCtrl);
	DDX_Control(pDX, IDC_GUILD_LOG_RESULT_STATIC, mResultStatic);
	DDX_Control(pDX, IDC_LOG_BEGIN_DATE_CTRL, mBeginDateCtrl);
	DDX_Control(pDX, IDC_LOG_BEGIN_TIME_CTRL, mBeginTimeCtrl);
	DDX_Control(pDX, IDC_LOG_END_DATE_CTRL, mEndDateCtrl);
	DDX_Control(pDX, IDC_LOG_END_TIME_CTRL, mEndTimeCtrl);
	DDX_Control(pDX, IDC_LOG_FIND_BUTTON, mFindButton);
	DDX_Control(pDX, IDC_LOG_STOP_BUTTON, mStopButton);
	DDX_Control(pDX, IDC_LOG_RESULT_PROGRESS, mProgressCtrl);
	DDX_Control(pDX, IDC_FARM_LOG_SERVER_COMBO, mServerCombo);
	DDX_Control(pDX, IDC_LOG_MAX_SIZE_STATIC, mMaxLogStatic);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
	ON_BN_CLICKED(IDC_LOG_CLIPBOARD_BUTTON, OnBnClickedLogClipboardButton)
	ON_BN_CLICKED(IDC_LOG_EXCEL_BUTTON, OnBnClickedLogExcelButton)
	ON_BN_CLICKED(IDC_LOG_FIND_BUTTON, OnBnClickedLogFindButton)
	ON_BN_CLICKED(IDC_LOG_STOP_BUTTON, OnBnClickedLogStopButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CLogDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_ILogDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {6DF333F1-26A6-4BFB-AD8B-5510144C9A72}
static const IID IID_ILogDialog =
{ 0x6DF333F1, 0x26A6, 0x4BFB, { 0xAD, 0x8B, 0x55, 0x10, 0x14, 0x4C, 0x9A, 0x72 } };

BEGIN_INTERFACE_MAP(CLogDialog, CDialog)
	INTERFACE_PART(CLogDialog, IID_ILogDialog, Dispatch)
END_INTERFACE_MAP()

void CLogDialog::OnBnClickedLogClipboardButton()
{
	// 090223 LUJ, 선택된 레코드가 없으면 전체 복사를 한다
	const bool isWholeCopy = ( ! mResultListCtrl.GetSelectedCount() );

	mApplication.SaveToClipBoard(
		mResultListCtrl,
		isWholeCopy );

	CString textSucceed;
	textSucceed.LoadString( IDS_STRING439 );
	CString textMessage;
	textMessage.Format(
		textSucceed,
		isWholeCopy ? mResultListCtrl.GetItemCount() : mResultListCtrl.GetSelectedCount() );	
	MessageBox( textMessage, _T( "" ), MB_OK );
}

void CLogDialog::OnBnClickedLogExcelButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( mCommand )
	{
		mCommand->SaveToExcel( mServerIndex, mResultListCtrl );
	}
}

void CLogDialog::OnBnClickedLogFindButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( ! mCommand )
	{
		return;
	}

	CTime beginDate;
	mBeginDateCtrl.GetTime( beginDate );

	CTime beginTime;
	mBeginTimeCtrl.GetTime( beginTime );

	CTime endDate;
	mEndDateCtrl.GetTime( endDate );

	CTime endTime;	
	mEndTimeCtrl.GetTime( endTime );

	// 080425 LUJ, 오늘 날짜보다 시작 시간이 큰 경우 결과가 없음
	if( beginDate	> CTime::GetCurrentTime() &&
		beginTime	> CTime::GetCurrentTime() )
	{
		CString textThereIsNoResult;
		textThereIsNoResult.LoadString( IDS_STRING1 );

		MessageBox( textThereIsNoResult, _T( "" ), MB_OK | MB_ICONERROR );
		return;
	}
	// 080425 LUJ, 시작 시간보다 끝 시간이 항상 커야함(같아도 의미 없음)
	else if( beginDate > endDate )
	{
		CString textEndTimeMustBeSmallerThanBegin;
		textEndTimeMustBeSmallerThanBegin.LoadString( IDS_STRING237 );

		MessageBox( textEndTimeMustBeSmallerThanBegin, _T( "" ), MB_OK | MB_ICONERROR );
		return;
	}

	// 080403 LUJ,	아이템 로그는 검색 테이블이 일별로 분리되어 있어 다른 로그와는 달리 특수하다. 
	//				RTTI를 이용하여 아이템 로그 명령일 경우 별도의 처리를 한다
	{
		CCommand* command = dynamic_cast< CItemLogCommand* >( mCommand );

		if( command )
		{
			endDate = beginDate;

			// 080425 LUJ, 현재 날짜에서 90일 이전이라면 검색할 수 없다. 이 기간을 변경하려면 서버팀과 재협의 필요
			{
				const __time64_t dayOfDuration		= 90;
				const __time64_t secondOfday		= 86400;
				const __time64_t validDuration		= secondOfday * dayOfDuration;
				const __time64_t todayTime			= CTime::GetCurrentTime().GetTime();
				const __time64_t beginSearchTime	= beginDate.GetTime();
				
				if( todayTime > beginSearchTime &&
					validDuration <= ( todayTime - beginSearchTime ) )
				{
					CString textValidDurationOfLogIs;
					textValidDurationOfLogIs.LoadString( IDS_STRING370 );

					CString text;
					text.Format(  textValidDurationOfLogIs, dayOfDuration );

					MessageBox( text, _T( "" ), MB_OK | MB_ICONERROR );
					return;
				}
			}			
		}
	}

	if( CTime() == beginTime )
	{
		beginTime = CTime( beginDate.GetYear(), beginDate.GetMonth(), beginDate.GetDay(), 0, 0, 0 );
	}

	// 080403 LUJ, 끝 시간이 0이면 기본값으로 설정한다
	if( CTime() == endTime )
	{
		endTime = CTime( endDate.GetYear(), endDate.GetMonth(), endDate.GetDay(), 23, 59, 59 );
	}

	CString serverName;
	mServerCombo.GetLBText(
		mServerCombo.GetCurSel(),
		serverName );

	mCommand->Find(
		mApplication.GetServerIndex( serverName ),
		beginDate.Format( _T( "%y%m%d" ) )	+ _T( " " ) + beginTime.Format( _T( "%H:%M:%S" ) ),
		endDate.Format( _T( "%y%m%d" ) )	+ _T( " " ) + endTime.Format( _T( "%H:%M:%S" ) ) );
}

void CLogDialog::OnBnClickedLogStopButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( mCommand )
	{
		mCommand->Stop( mServerIndex );
	}
}


BOOL CLogDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( mCommand )
	{
		mCommand->Initialize( mResultListCtrl );

		SetWindowText( mCommand->GetTitle() );
	}

	// 리스트 컨트롤 초기화
	mResultListCtrl.SetExtendedStyle(
		mResultListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	// 버튼 초기화
	mStopButton.EnableWindow( FALSE );

	// 콤보 박스 초기화
	{
		mApplication.SetServerComboBox( mServerCombo );
		
		{
			CRect rect;
			mServerCombo.GetWindowRect( rect );
			mServerCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			const BOOL isEnableSelectionServer( UINT_MAX == mServerIndex );

			if( isEnableSelectionServer )
			{
				mServerCombo.EnableWindow( TRUE );
			}
			else
			{
				mServerCombo.EnableWindow( FALSE );

				const TCHAR* name = mApplication.GetServerName( mServerIndex );

				mServerCombo.SetCurSel(
					mServerCombo.FindString( 0, name ) );
			}
		}
	}

	// 최대 로그 개수 지정
	{
		CString format;
		mMaxLogStatic.GetWindowText( format );

		CString text;
		text.Format( format, MAX_LOG_SIZE );

		mMaxLogStatic.SetWindowText( text );
	}

	// 080403 LUJ,	아이템 로그는 검색 테이블이 일별로 분리되어 있어 다른 로그와는 달리 특수하다. 
	//				RTTI를 이용하여 아이템 로그 명령일 경우 별도의 처리를 한다
	{
		CCommand* command = dynamic_cast< CItemLogCommand* >( mCommand );

		if( command )
		{
			mEndDateCtrl.ShowWindow( SW_HIDE );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CLogDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	if( mCommand )
	{
		mCommand->Parse(
			message,
			mResultListCtrl,
			mProgressCtrl,
			mResultStatic,
			mFindButton,
			mStopButton );
	}
}


INT_PTR CLogDialog::DoModal( DWORD serverIndex, CCommand& command )
{
	mServerIndex	= serverIndex;
	mCommand		= &command;

	return CDialog::DoModal();
}