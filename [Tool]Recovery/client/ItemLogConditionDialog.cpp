// ItemLogConditionDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "ItemLogConditionDialog.h"
#include "ItemAddDialog.h"


// CItemLogConditionDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemLogConditionDialog, CDialog)
CItemLogConditionDialog::CItemLogConditionDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CItemLogConditionDialog::IDD, pParent),
mApplication( application )
{
	ZeroMemory( &mInputData, sizeof( mInputData ) );

	EnableAutomation();
}

CItemLogConditionDialog::~CItemLogConditionDialog()
{
}

void CItemLogConditionDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CItemLogConditionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEM_CONDITION_INDEX_EDIT, mItemIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_DB_IDX_EDIT, mItemDbIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_PLAYER_IDX_EDIT, mPlayerIndexEdit);
	DDX_Control(pDX, IDC_ITEM_CONDITION_LOG_TYPE_COMBO, mLogTypeCombo);
}


BEGIN_MESSAGE_MAP(CItemLogConditionDialog, CDialog)
	ON_BN_CLICKED(IDC_ITEM_LOG_CONDITION_ITEM_IDX_BUTTON, OnBnClickedItemLogConditionItemIdxButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemLogConditionDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IItemLogConditionDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {2406325B-DCF7-4930-8408-D7296E73CAFA}
static const IID IID_IItemLogConditionDialog =
{ 0x2406325B, 0xDCF7, 0x4930, { 0x84, 0x8, 0xD7, 0x29, 0x6E, 0x73, 0xCA, 0xFA } };

BEGIN_INTERFACE_MAP(CItemLogConditionDialog, CDialog)
	INTERFACE_PART(CItemLogConditionDialog, IID_IItemLogConditionDialog, Dispatch)
END_INTERFACE_MAP()


// CItemLogConditionDialog 메시지 처리기입니다.

BOOL CItemLogConditionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	mPlayerIndexEdit.SetWindowText( _T( "0" ) );
	mItemDbIndexEdit.SetWindowText( _T( "0" ) );
	mItemIndexEdit.SetWindowText( _T( "0" ) );
	
	// 080716 LUJ, 콤보 박스 세팅
	{
		CString text;

		CString textEmpty;
		textEmpty.LoadString( IDS_STRING203 );
		text.Format( _T( "%s" ), textEmpty );
		mLogTypeCombo.InsertString( mLogTypeCombo.GetCount(), text );

		for( int logType = 0; logType < eLog_Max; ++logType )
		{
			const CString logText = mApplication.GetText( eLogitemmoney( logType ) );

			int i = 0;
			const CString token = logText.Tokenize( _T( "()" ), i );

			if( _T( "?" ) == token )
			{
				continue;
			}

			text.Format( _T( "%s" ), logText );
			mLogTypeCombo.InsertString(
				mLogTypeCombo.GetCount(),
				text );

			mLogDataList.push_back(
				std::make_pair( text, eLogitemmoney( logType ) ) );
		}

		CRect rect;
		mLogTypeCombo.GetWindowRect( rect );
		mLogTypeCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		mLogTypeCombo.SetCurSel( 0 );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CItemLogConditionDialog::OnOK()
{
	CString text;

	mItemDbIndexEdit.GetWindowText( text );
	mInputData.mItemDbIndex	= _ttoi( text );

	mItemIndexEdit.GetWindowText( text );
	mInputData.mItemIndex = _ttoi( text );

	mPlayerIndexEdit.GetWindowText( text );
	mInputData.mPlayerIndex = _ttoi( text );

	mLogTypeCombo.GetLBText( mLogTypeCombo.GetCurSel(), text );

	for(
		LogDataList::const_iterator it = mLogDataList.begin();
		mLogDataList.end() != it;
		++it )
	{
		const CString& logText = it->first;

		if( logText != text )
		{
			continue;
		}

		mInputData.mLogType	= eLogitemmoney( it->second );
	}
	
	CDialog::OnOK();
}


void CItemLogConditionDialog::OnBnClickedItemLogConditionItemIdxButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CItemAddDialog dialog( mApplication );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CString text;
	text.Format( _T( "%d" ), dialog.GetSelectedItemIndex() );
	mItemIndexEdit.SetWindowText( text );
}
