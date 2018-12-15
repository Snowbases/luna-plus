// IndexInputDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "IndexInputDialog.h"
#include "ItemAddDialog.h"


// CIndexInputDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CIndexInputDialog, CDialog)
CIndexInputDialog::CIndexInputDialog( CclientApp& application, const Configuration& configuration, CWnd* pParent /*=NULL*/)
	: CDialog(CIndexInputDialog::IDD, pParent)
	, mApplication( application )
	, mItemIndex( 0 )
	, mItemDbIndex( 0 )
	, mPlayerIndex( 0 )
	, mUserIndex(0)
	, mConsignIndex(0)
	, mConfiguration( configuration )
{
	EnableAutomation();
}

CIndexInputDialog::~CIndexInputDialog()
{
}

void CIndexInputDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CIndexInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_INDEX_INPUT_ITEM_RADIO, mItemRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_ITEM_DB_RADIO, mItemDbRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_PLAYER_RADIO, mPlayerRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_KEYWORD_RADIO, mKeywordRadioButton );
	DDX_Control( pDX, IDC_INDEX_INPUT_EDIT, mIndexInputEdit);
	DDX_Control(pDX, IDC_INDEX_INPUT_USER_INDEX_RADIO, mUserIndexButton);
	DDX_Control(pDX, IDC_INDEX_INPUT_CONSIGN_INDEX_RADIO, mConsignIndexButton);
}


BEGIN_MESSAGE_MAP(CIndexInputDialog, CDialog)
	ON_BN_CLICKED(IDC_INDEX_INPUT_ITEM_RADIO, OnBnClickedIndexInputItemRadio)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CIndexInputDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IIndexInputDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {5BE2F299-6851-48E7-86C8-3E17249855C7}
static const IID IID_IIndexInputDialog =
{ 0x5BE2F299, 0x6851, 0x48E7, { 0x86, 0xC8, 0x3E, 0x17, 0x24, 0x98, 0x55, 0xC7 } };

BEGIN_INTERFACE_MAP(CIndexInputDialog, CDialog)
	INTERFACE_PART(CIndexInputDialog, IID_IIndexInputDialog, Dispatch)
END_INTERFACE_MAP()


// CIndexInputDialog 메시지 처리기입니다.

void CIndexInputDialog::OnOK()
{
	CString index;
	mIndexInputEdit.GetWindowText( index );

	if( mItemRadioButton.GetCheck() )
	{
		mItemIndex = _ttoi( index );
	}
	else if( mItemDbRadioButton.GetCheck() )
	{
		mItemDbIndex = _ttoi( index );
	}
	else if( mPlayerRadioButton.GetCheck() )
	{
		mPlayerIndex = _ttoi( index );
	}
	else if( mKeywordRadioButton.GetCheck() )
	{
		mKeyword = index;
	}
	else if(mUserIndexButton.GetCheck())
	{
		mUserIndex = _ttoi(index);
	}
	else if(mConsignIndexButton.GetCheck())
	{
		mConsignIndex = _ttoi(index);
	}

	CDialog::OnOK();
}


BOOL CIndexInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	mItemDbRadioButton.EnableWindow( mConfiguration.mIsEnableItemDbIndex );
	mItemRadioButton.EnableWindow( mConfiguration.mIsEnableItemIndex );
	mPlayerRadioButton.EnableWindow( mConfiguration.mIsEnablePlayerIndex );
	mKeywordRadioButton.EnableWindow( mConfiguration.mIsEnableKeyword );
	mUserIndexButton.EnableWindow(mConfiguration.mIsEnableUserIndex);
	mConsignIndexButton.EnableWindow(mConfiguration.mIsEnableConsignIndex);

	// 080403 LUJ, 활성된 라디오 버튼에 체크 표시한다. 하나도 없으면 에디트 상자를 비활성화한다
	if( mConfiguration.mIsEnableItemDbIndex )
	{
		mItemDbRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnableItemIndex )
	{
		mItemRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnablePlayerIndex )
	{
		mPlayerRadioButton.SetCheck( TRUE );
	}
	else if( mConfiguration.mIsEnableKeyword )
	{
		mKeywordRadioButton.SetCheck( TRUE );
	}
	else if(mConfiguration.mIsEnableUserIndex)
	{
		mUserIndexButton.SetCheck(TRUE);
	}
	else if(mConfiguration.mIsEnableConsignIndex)
	{
		mConsignIndexButton.SetCheck(TRUE);
	}
	else
	{
		mIndexInputEdit.EnableWindow( FALSE );
	}
	
	mIndexInputEdit.SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CIndexInputDialog::OnBnClickedIndexInputItemRadio()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemAddDialog dialog( mApplication );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CString text;
	text.Format( _T( "%d" ), dialog.GetSelectedItemIndex() );
	
	mIndexInputEdit.SetWindowText( text );
}
