// ItemFindDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "ItemFindDialog.h"
#include "ItemDialog.h"
#include ".\itemfinddialog.h"


// CItemFindDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CItemFindDialog, CDialog)
CItemFindDialog::CItemFindDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CItemFindDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();

}

CItemFindDialog::~CItemFindDialog()
{
}

void CItemFindDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CItemFindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_ITEM_FIND_MORE_SEARCH_BUTTON, mMoreFindButton );
	DDX_Control( pDX, IDC_ITEM_FIND_DATA_LIST2, mItemListctrl );
	DDX_Control( pDX, IDC_ITEM_FIND_RESULT_LIST, mResultListCtrl );
	DDX_Control( pDX, IDC_ITEM_FIND_SERVER_COMBO, mServerComboBox );	
	DDX_Control( pDX, IDC_ITEM_FIND_KEYWORD, mKeywordEdit );
	DDX_Control(pDX, IDC_ITEM_FIND_VIEW_PLAYER, mViewPlayerButton);
	DDX_Control(pDX, IDC_ITEM_FIND_PAGE_STATIC, mPageStatic);
	DDX_Control(pDX, IDC_ITEM_FIND_PREV_PAGE_BUTTON, mPagePrevButton);
	DDX_Control(pDX, IDC_ITEM_FIND_NEXT_PAGE_BUTTON, mPageNextButton);
	DDX_Control(pDX, IDC_ITEM_FIND_RESULT_STATIC, mResultStatic);
	DDX_Control(pDX, IDC_ITEM_FIND_SEARCH_BUTTON, mFindButton);
}


BEGIN_MESSAGE_MAP(CItemFindDialog, CDialog)
	ON_BN_CLICKED(IDC_ITEM_FIND_PREV_PAGE_BUTTON, OnBnClickedItemFindPrevPageButton)
	ON_BN_CLICKED(IDC_ITEM_FIND_NEXT_PAGE_BUTTON, OnBnClickedItemFindNextPageButton)
	ON_BN_CLICKED(IDC_ITEM_FIND_MORE_SEARCH_BUTTON, OnBnClickedItemFindMoreSearchButton)
	ON_BN_CLICKED(IDC_ITEM_FIND_VIEW_PLAYER, OnBnClickedItemFindViewPlayer)
	ON_BN_CLICKED(IDC_ITEM_FIND_SEARCH_BUTTON, OnBnClickedItemFindSearchButton)
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_ITEM_FIND_KEYWORD, OnEnChangeItemFindKeyword)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemFindDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IItemFindDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {29008F9C-A7AA-496A-9588-1BA2DC2D6FE5}
static const IID IID_IItemFindDialog =
{ 0x29008F9C, 0xA7AA, 0x496A, { 0x95, 0x88, 0x1B, 0xA2, 0xDC, 0x2D, 0x6F, 0xE5 } };

BEGIN_INTERFACE_MAP(CItemFindDialog, CDialog)
	INTERFACE_PART(CItemFindDialog, IID_IItemFindDialog, Dispatch)
END_INTERFACE_MAP()


// CItemFindDialog 메시지 처리기입니다.


BOOL CItemFindDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// 서버 목록을 채운다
	{
		CRect rect;
		mServerComboBox.GetWindowRect( rect );
		mServerComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}	

	// 리스트 초기화
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;

		// 아이템 목록을 채운다
		{
			mItemListctrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mItemListctrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mItemListctrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mItemListctrl.InsertColumn( 2, textLevel, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );

			mItemListctrl.SetExtendedStyle( mItemListctrl.GetExtendedStyle() | addStyle );			
		}

		// 결과 목록을 초기화
		{
			mResultListCtrl.GetClientRect( rect );

			CString textUserIndex;
			textUserIndex.LoadString( IDS_STRING301 );
			mResultListCtrl.InsertColumn( 0, textUserIndex, LVCFMT_LEFT, 0 );
			CString textPlayerIndex;
			textPlayerIndex.LoadString( IDS_STRING54 );
			mResultListCtrl.InsertColumn( 1, textPlayerIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mResultListCtrl.InsertColumn( 2, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );
			mResultListCtrl.InsertColumn( 3, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.18f ) );

			mResultListCtrl.SetExtendedStyle( mResultListCtrl.GetExtendedStyle() | addStyle );
		}
	}
	
	// 결과 정보 초기화
	{
		mPagePrevButton.EnableWindow( FALSE );
		mPageNextButton.EnableWindow( FALSE );
		mPageStatic.SetWindowText( _T( "" ) );

		mViewPlayerButton.EnableWindow( FALSE );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemFindDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	//ShowWindow( SW_SHOW );

	switch( message->Protocol )
	{
	case MP_RM_ITEM_FIND_OWNER_ACK:
		{
			const MSG_RM_ITEM_OWNER* m = ( MSG_RM_ITEM_OWNER* )message;

			if( mResultListCtrl.GetItemCount() )
			{
				if( ! m->mSize )
				{
					CString textThereIsNoResult;
					textThereIsNoResult.LoadString( IDS_STRING1 );

					MessageBox( textThereIsNoResult, _T( "" ), MB_ICONINFORMATION | MB_OK );
					return;
				}				
			}
			else if( ! m->mSize )
			{
				mViewPlayerButton.EnableWindow( FALSE );

				mResultStatic.SetWindowText( _T( "" ) );

				CString textThereIsNoResult;
				textThereIsNoResult.LoadString( IDS_STRING1 );

				MessageBox( textThereIsNoResult, _T( "" ), MB_ICONINFORMATION | MB_OK );
				return;
			}
			
			// add page
			{
				Page& page = mPageMap[ DWORD( mPageMap.size() ) ];

				const ItemScript& itemScript = mApplication.GetItemScript(
					m->mItemIndex);

				for( DWORD i = 0; i < m->mSize; ++i )
				{
					const MSG_RM_ITEM_OWNER::Player& player = m->mPlayer[ i ];

					if( page.end() != page.find( player.mIndex ) )
					{
						ASSERT( 0 && "Duplicated index is found" );
						continue;
					}

					Player& data	= page[ player.mIndex ];
					data.mItemIndex	= m->mItemIndex;
					data.mItemSize	= (itemScript.Stack ? player.mQuantity : player.mSlotSize);
					data.mName		= player.mName;
					data.mUserIndex	= player.mUserIndex;
				}
			}

			PutPage( DWORD( mPageMap.size() - 1 ) );

			const DWORD maxSize = sizeof( m->mPlayer ) / sizeof( m->mPlayer[ 0 ] );

			// 정보/컨트롤 갱신
			{
				mMoreFindButton.EnableWindow( maxSize == mResultListCtrl.GetItemCount() );

				CString text;
				text.Format( _T( "%d/%d" ), mPageMap.size(), mPageMap.size() );
				mPageStatic.SetWindowText( text );

				mPageNextButton.EnableWindow( FALSE );
				mPagePrevButton.EnableWindow( 1 < mPageMap.size() );

				mViewPlayerButton.EnableWindow( TRUE );
			}

			{
				const size_t size = max( mPageMap.size() - 1, 0 ) * maxSize + mResultListCtrl.GetItemCount();

				CString textFormat;
				textFormat.LoadString( IDS_STRING116 );

				CString text;
				text.Format( textFormat, mApplication.GetItemName( m->mItemIndex ), mSelectedServerName, size );

				mResultStatic.SetWindowText( _T( " " )+ text );
			}

			break;
		}
	case MP_RM_ITEM_FIND_OWNER_NACK:
		{
			ASSERT( 0 && "system fault detected" );
			break;
		}
	case MP_RM_ITEM_FIND_OWNER_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


void CItemFindDialog::PutPage( DWORD pageIndex )
{
	mResultListCtrl.DeleteAllItems();

	if( mPageMap.end() == mPageMap.find( pageIndex ) )
	{
		ASSERT( 0 );
		return;
	}

	const Page& page = mPageMap[ pageIndex ];

	CString	text;
	int		i = 0;

	for( Page::const_iterator it = page.begin(); page.end() != it; ++it, ++i )
	{
		const DWORD		playerIndex = it->first;
		const Player&	player		= it->second;

		text.Format( _T( "%d" ), player.mUserIndex );
        mResultListCtrl.InsertItem( i, text, 0 );

		text.Format( _T( "%d" ), playerIndex );
		mResultListCtrl.SetItemText( i, 1, text );

		mResultListCtrl.SetItemText( i, 2, player.mName );

		text.Format( _T( "%d" ), player.mItemSize );
		mResultListCtrl.SetItemText( i, 3, text );
	}
}


int CItemFindDialog::GetCurrentPage() const
{
	CString text;
	mPageStatic.GetWindowText( text );

	int i = 0;
	CString token = text.Tokenize( _T( "/ " ), i );

	return token.IsEmpty() ? 0 : _ttoi( token ) - 1;
}



void CItemFindDialog::OnBnClickedItemFindPrevPageButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 페이지로 이동

	int page = GetCurrentPage();

	if( 1 >  page )
	{
		return;
	}

	PutPage( --page );

	if( ! page )
	{
		mPagePrevButton.EnableWindow( FALSE );
	}

	mPageNextButton.EnableWindow( TRUE );

	{
		CString text;
		text.Format( _T( "%d/%d" ), page + 1, mPageMap.size() );
		mPageStatic.SetWindowText( text );
	}
}


void CItemFindDialog::OnBnClickedItemFindNextPageButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 다음 페이지로 이동

	int			page	= GetCurrentPage();
	const int	maxPage	= max( 0, int( mPageMap.size() ) - 1 );

	if( maxPage == page )
	{
		return;
	}

	PutPage( ++page );

	if( maxPage == page )
	{
		mPageNextButton.EnableWindow( FALSE );
	}

	mPagePrevButton.EnableWindow( TRUE );

	{
		CString text;
		text.Format( _T( "%d/%d" ), page + 1, mPageMap.size() );
		mPageStatic.SetWindowText( text );
	}
}

void CItemFindDialog::OnBnClickedItemFindMoreSearchButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 계속 찾기를 클릭

	if( mPageMap.empty() )
	{
		return;
	}

	const Page& page = mPageMap[ DWORD( mPageMap.size() ) - 1 ];

	if( page.empty() )
	{
		return;
	}

	Page::const_reverse_iterator it = page.rbegin();

	const DWORD		playerIndex = it->first;
	const Player	player		= it->second;

	// 전송
	{
		MSG_DWORD2 message;

		message.Category	= MP_RM_ITEM_FIND;
		message.Protocol	= MP_RM_ITEM_FIND_OWNER_SYN;
		//message.dwObjectID	= mApplication.GetPlayerIndex();
		message.dwData1		= player.mItemIndex;
		message.dwData2		= playerIndex;

		mApplication.Send( mSelectedServerName, message, sizeof( message ) );
	}
}
void CItemFindDialog::OnBnClickedItemFindViewPlayer()
{
	POSITION	position	= mResultListCtrl.GetFirstSelectedItemPosition();
	const int	selectedRow	= mResultListCtrl.GetNextSelectedItem( position );

	const DWORD userIndex	= _ttoi( mResultListCtrl.GetItemText( selectedRow, 0 ) );
	const DWORD playerIndex = _ttoi( mResultListCtrl.GetItemText( selectedRow, 1 ) );

	if( ! playerIndex || ! userIndex )
	{
		CString textThereIsNoTarget;
		textThereIsNoTarget.LoadString( IDS_STRING31);

		MessageBox( textThereIsNoTarget, _T( "" ), MB_ICONWARNING | MB_OK );
		return;
	}

	{
		CItemDialog* dialog = mApplication.GetItemDialog();
		ASSERT( dialog );

		const CString playerName = mResultListCtrl.GetItemText( selectedRow, 2 );

		dialog->Request(
			mApplication.GetServerIndex( mSelectedServerName ),
			playerName,
			playerIndex,
			userIndex );
	}
}


void CItemFindDialog::OnBnClickedItemFindSearchButton()
{
	POSITION position = mItemListctrl.GetFirstSelectedItemPosition();
	const int row = mItemListctrl.GetNextSelectedItem(
		position);
	const CString itemIndex = mItemListctrl.GetItemText(
		row,
		0);

	mPagePrevButton.EnableWindow( FALSE );
	mPageNextButton.EnableWindow( FALSE );
	mPageMap.clear();

	// 전송
	{
		MSG_DWORD2 message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_RM_ITEM_FIND;
		message.Protocol = MP_RM_ITEM_FIND_OWNER_SYN;
		message.dwData1	= _ttoi(itemIndex);

		mServerComboBox.GetLBText( mServerComboBox.GetCurSel(), mSelectedServerName );
		mApplication.Send( mSelectedServerName, message, sizeof( message ) );
	}	
}


// 080702 LUJ, 네트워크 초기화가 창 생성 후에 되므로, 창이 표시될 때 서버를 나타내는 콤보 박스 또한 초기화해주어야 한다
void CItemFindDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	static bool isInitialized;

	// 080702 LUJ, 아이템 리스트를 창 생성보다 뒤에 로딩해오기 때문에 값이 설정되지 않는다
	//				아이템 리스트는 매우 무겁기 때문에 한번만 초기화하도록 정적 변수를 선언했다
	if( ! isInitialized )
	{
		mApplication.SetServerComboBox( mServerComboBox );
		mApplication.SetItemList( mItemListctrl );

		isInitialized = true;
	}	
}
void CItemFindDialog::OnEnChangeItemFindKeyword()
{
	static DWORD lastTick = 0;
	
	if(lastTick > GetTickCount())
	{
		return;
	}

	lastTick = GetTickCount() + 1000;

	CString keyword;
	mKeywordEdit.GetWindowText(
		keyword);

	mApplication.SetItemList(
		mItemListctrl,
		keyword);
}
