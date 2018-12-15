#include "stdafx.h"
#include "client.h"
#include "ItemChangeDialog.h"
#include "ItemAddDialog.h"

CItemChangeDialog::ItemChangeScriptMap CItemChangeDialog::mItemChangeScriptMap;

const int _maxCount = 1000000000;

IMPLEMENT_DYNAMIC(CItemChangeDialog, CDialog)
CItemChangeDialog::CItemChangeDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CItemChangeDialog::IDD, pParent),
mApplication( application ),
mMaxChangeCount( _maxCount ),
mDefaultChangecount( _maxCount / 1000000 )
{
	EnableAutomation();	

	static bool initializationIsNeeded = true;

	if( initializationIsNeeded )
	{
		LoadScript();

		initializationIsNeeded = false;
	}
}

CItemChangeDialog::~CItemChangeDialog()
{}

void CItemChangeDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CItemChangeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEMCHANGE_RESULT_LIST, mResultListCtrl);
	DDX_Control(pDX, IDC_ITEMCHANGE_ITEM_LIST, mItemComboBox);
	DDX_Control(pDX, IDC_ITEM_CHANGE_DESCRIPTION, mItemDescriptionStatic);
	DDX_Control(pDX, IDC_ITEM_CHANGE_SUBMIT_BUTTON, mSubmitButton);
	DDX_Control(pDX, IDC_ITEM_CHANGE_COUNT_EDIT, mCountEdit);
	DDX_Control(pDX, IDC_ITEM_CHANGE_COUNT_MAX_BUTTON, mCountMaxButton);
}


BEGIN_MESSAGE_MAP(CItemChangeDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_ITEMCHANGE_ITEM_LIST, OnCbnSelchangeItemchangeItemList)
	ON_BN_CLICKED(IDC_ITEM_CHANGE_SUBMIT_BUTTON, OnBnClickedItemChangeSubmitButton)
	ON_BN_CLICKED(IDC_ITEM_CHANGE_COPY_CLIPBOARD_BUTTON, OnBnClickedItemChangeCopyClipboardButton)
	ON_BN_CLICKED(IDC_ITEM_CHANGE_FIND_BUTTON, OnBnClickedItemChangeFindButton)
	ON_BN_CLICKED(IDC_ITEM_CHANGE_COUNT_MAX_BUTTON, OnBnClickedItemChangeCountMaxButton)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CItemChangeDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IItemChangeDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {7719EC75-05EC-4164-862E-B7BB36056711}
static const IID IID_IItemChangeDialog =
{ 0x7719EC75, 0x5EC, 0x4164, { 0x86, 0x2E, 0xB7, 0xBB, 0x36, 0x5, 0x67, 0x11 } };

BEGIN_INTERFACE_MAP(CItemChangeDialog, CDialog)
	INTERFACE_PART(CItemChangeDialog, IID_IItemChangeDialog, Dispatch)
END_INTERFACE_MAP()


// CItemChangeDialog 메시지 처리기입니다.
void CItemChangeDialog::LoadScript()
{
	CMHFile file;
	file.Init( "System/Resource/ChangeItem.bin", "rb" );

	ItemIndex currentItemIndex = 0;

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* const comment	= "//";
		const char* const separator	= "\t";
		const char* const keyword1	= strtok( text, separator );
		const char* const keyword2	= strtok( 0, separator );
		const char* const keyword3	= strtok( 0, separator );
		const char* const keyword4	= strtok( 0, separator );
		const char* const keyword5	= strtok( 0, separator );

		if( ! keyword1 )
		{
			continue;
		}
		else if( ! strnicmp( comment, keyword1, strlen( comment ) ) )
		{
			continue;
		}
		else if( atoi( keyword1 ) )
		{
			currentItemIndex = atoi( keyword1 );

			ChangeScriptMapList& scriptMapList = mItemChangeScriptMap[ currentItemIndex ];
			scriptMapList.push_back( ChangeScriptMap() );
		}
		else if( ! stricmp( "ITEM", keyword1 ) )
		{
			if(0 == keyword2 ||
				0 == keyword3 ||
				0 == keyword4 ||
				0 == keyword5)
			{
				continue;
			}

			ChangeScript script;
			script.mItemIndex		= atoi( keyword2 );
			script.mCount.first		= atoi( keyword3 );
			script.mCount.second	= atoi( keyword4 );
			script.mRate			= float( atof( keyword5 ) / 1000000.0f );

			ChangeScriptMapList& scriptMapList = mItemChangeScriptMap[ currentItemIndex ];

			if( scriptMapList.empty() )
			{
				continue;
			}

			ChangeScriptMap& scriptMap = scriptMapList.back();
			
			// 090304 LUJ, 맵에 누적된 확률 값을 얻는다
			const Rate finalRate = ( scriptMap.empty() ? 0 : scriptMap.rbegin()->first );
			// 090304 LUJ, 서로 확률 위치에 존재한다
			scriptMap[ finalRate + script.mRate ] = script;
		}
		else if( ! stricmp( "-", keyword1 ) )
		{
			ChangeScriptMapList& scriptMapList = mItemChangeScriptMap[ currentItemIndex ];
			scriptMapList.push_back( ChangeScriptMap() );
		}
	}
}

void CItemChangeDialog::SetItemCombo( CComboBox& comboBox )
{
	for(	ItemChangeScriptMap::const_iterator it = mItemChangeScriptMap.begin();
			mItemChangeScriptMap.end() != it;
			++it )
	{
		const ItemIndex& itemIndex = it->first;

		comboBox.AddString( mApplication.GetItemName( itemIndex ) );
	}
}

BOOL CItemChangeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 090304 LUJ, 툴팁
	{
		mItemToolTipCtrl.Create( this );
		mItemToolTipCtrl.Activate( TRUE );
		mItemToolTipCtrl.SetMaxTipWidth( 200 );
	}

	// 090304 LUJ, 아이템 콤보 박스
	{
		CRect rect;
		mItemComboBox.GetWindowRect( rect );
		mItemComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		SetItemCombo( mItemComboBox );
	}

	// 090304 LUJ, 아이템 변환 개수 입력창
	{
		CString text;
		text.Format( L"%d", mDefaultChangecount );
		mCountEdit.SetWindowText( text );
	}

	// 090304 LUJ, 결과 표시
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		mResultListCtrl.SetExtendedStyle( mResultListCtrl.GetExtendedStyle() | addStyle );
		CRect rect;
		mResultListCtrl.GetWindowRect( rect );

		int position = -1;

		CString textIndex;
		textIndex.LoadString( IDS_STRING22 );
		mResultListCtrl.InsertColumn( ++position, textIndex, LVCFMT_LEFT, 0 );
		CString textName;
		textName.LoadString( IDS_STRING20 );
		mResultListCtrl.InsertColumn( ++position, textName, LVCFMT_LEFT, int( rect.Width() * 0.38 ) );
		CString textCount;
		textCount.LoadString( IDS_STRING42 );
		mResultListCtrl.InsertColumn( ++position, textCount, LVCFMT_CENTER, int( rect.Width() * 0.15 ) );
		CString textRate = _T( "%" );
		mResultListCtrl.InsertColumn( ++position, textRate, LVCFMT_RIGHT, int( rect.Width() * 0.15 ) );
		CString textResultCount;
		textResultCount.LoadString( IDS_STRING284 );
		mResultListCtrl.InsertColumn( ++position, textResultCount + L"(" + textCount + L")", LVCFMT_RIGHT, int( rect.Width() * 0.15 ) );
		CString textResultPercent = textResultCount + L"(%)";
		mResultListCtrl.InsertColumn( ++position, textResultPercent, LVCFMT_RIGHT, int( rect.Width() * 0.15 ) );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemChangeDialog::OnCbnSelchangeItemchangeItemList()
{
	const ItemIndex itemIndex = GetSelectedItemIndex();

	// 090304 LUJ, 툴팁 설정
	{
		const ItemScript& script = mApplication.GetItemScript( itemIndex );
		const TCHAR* const description = mApplication.GetTip( script.ItemTooltipIdx );
		mItemDescriptionStatic.SetWindowText( description );
		mItemToolTipCtrl.AddTool( this, description );
	}

	CString textGroupMark;

	// 090304 LUJ, 결과 상자 갱신
	if( mItemChangeScriptMap.end() != mItemChangeScriptMap.find( itemIndex ) )
	{
		mResultListCtrl.DeleteAllItems();

		const ChangeScriptMapList& scriptMapList = mItemChangeScriptMap[ itemIndex ];

		for(	ChangeScriptMapList::const_iterator scriptMap_it = scriptMapList.begin();
				scriptMapList.end() != scriptMap_it;
				++scriptMap_it )
		{
			const ChangeScriptMap& scriptMap = *scriptMap_it;
			CString textMark;

			if( 1 < scriptMap.size() )
			{
				textGroupMark += L"*";
				textMark = textGroupMark;
			}
			
			for(	ChangeScriptMap::const_iterator script_it = scriptMap.begin();
					scriptMap.end() != script_it;
					++script_it )
			{
				const ChangeScript& script = script_it->second;
				const ItemScript& itemScript = mApplication.GetItemScript( script.mItemIndex );

				const int row = mResultListCtrl.GetItemCount();
				int step = 0;

				CString text;
				text.Format( _T( "%d" ), itemScript.ItemIdx );
				mResultListCtrl.InsertItem( row, text, 0 );
				mResultListCtrl.SetItemText( row, ++step, textMark + itemScript.mUnicodeName );

				text.Format(
					script.mCount.second ? _T( "%d~%d" ) : _T( "%d" ),
					script.mCount.first,
					script.mCount.second );
				mResultListCtrl.SetItemText( row, ++step, text );

				text.Format( _T( "%0.2f%%" ), script.mRate * 100.0f );
				mResultListCtrl.SetItemText( row, ++step, text );
				mResultListCtrl.SetItemText( row, ++step, _T( "0" ) );
			}
		}
	}

	mSubmitButton.EnableWindow( 0 < mResultListCtrl.GetItemCount() );
}

CItemChangeDialog::ItemIndex CItemChangeDialog::GetSelectedItemIndex() const
{
	CString textItemName;
	mItemComboBox.GetWindowText( textItemName );

	return mApplication.GetItemIndex( textItemName );
}

void CItemChangeDialog::OnBnClickedItemChangeSubmitButton()
{
	OnCbnSelchangeItemchangeItemList();

	CString textCount;
	mCountEdit.GetWindowText( textCount );
	const int maxChangedCount = _ttoi( textCount );
	const ItemIndex changeItemIndex = GetSelectedItemIndex();

	if( mItemChangeScriptMap.end() == mItemChangeScriptMap.find( changeItemIndex ) )
	{
		CString textThereIsNoResult;
		textThereIsNoResult.LoadString( IDS_STRING1 );
		MessageBox( textThereIsNoResult );
		return;
	}

	class Toggle
	{
	private:
		CButton& mButton;
	public:
		Toggle( CButton& button )
		: mButton( button )
		{
			mButton.EnableWindow( FALSE );
		}
		
		~Toggle()
		{
			mButton.EnableWindow( TRUE );
		}
	}
	toggleButton( mSubmitButton );

	const ChangeScriptMapList& scriptMapList = mItemChangeScriptMap[ changeItemIndex ];

	// 090304 LUJ, 가속을 위해 아이템 번호와 값이 있는 레코드 열을 매핑한다
	typedef int Row;
	typedef stdext::hash_map< ItemIndex, Row > ItemIndexRowMap;
	ItemIndexRowMap itemIndexRowMap;
	// 090304 LUJ, 결과가 리스트 박스에 표시된 상태이어야 정상적으로 값을 얻는다
	for( int row = 0; row < mResultListCtrl.GetItemCount(); ++row )
	{
		const CString textItemIndex = mResultListCtrl.GetItemText( row, 0 );

		itemIndexRowMap[ _ttoi( textItemIndex ) ] = row;
	}

	for( int count = 0; count < maxChangedCount; ++count )
	{
		for(	ChangeScriptMapList::const_iterator scriptMap_it = scriptMapList.begin();
				scriptMapList.end() != scriptMap_it;
				++scriptMap_it )
		{
			const ChangeScriptMap& scriptMap = *scriptMap_it;
			const ChangeScriptMap::const_iterator script_it = scriptMap.lower_bound( mApplication.GetRandomFloat() );

			if( scriptMap.end() == script_it )
			{
				continue;
			}

			const ChangeScript& script = script_it->second;
			const ItemIndexRowMap::const_iterator row_it = itemIndexRowMap.find( script.mItemIndex );

			if( itemIndexRowMap.end() == row_it )
			{
				continue;
			}

			const Row row = row_it->second;
			const CString textCount = mResultListCtrl.GetItemText( row, 4 );

			CString textResult;
			textResult.Format( _T( "%d" ), _ttoi( textCount ) + 1 );
			mResultListCtrl.SetItemText( row, 4, textResult );

			CRect rect;
			mResultListCtrl.GetSubItemRect( row, 4, 0, rect );
			mResultListCtrl.RedrawWindow( rect );
		}

		CString textCount;
		textCount.Format( L"%d", maxChangedCount - count );
		mCountEdit.SetWindowText( textCount );
		mCountEdit.RedrawWindow();
		Sleep( 0 );
	}

	// 090304 LUJ, 확률 갱신
	for( int row = 0; row < mResultListCtrl.GetItemCount(); ++row )
	{
		const CString itemCount	= mResultListCtrl.GetItemText( row, 4 );

		CString text;
		text.Format(
			L"%0.4f%%",
			_tstof( itemCount ) / maxChangedCount * 100.0f );
		mResultListCtrl.SetItemText( row, 5, text );
	}

	// 090304 LUJ, 변환 회수 기본값 설정
	{
		CString textCount;
		textCount.Format( L"%d", mDefaultChangecount );
		mCountEdit.SetWindowText( textCount );
	}
}

void CItemChangeDialog::OnBnClickedItemChangeCopyClipboardButton()
{
	const bool isWholeCopy = ( ! mResultListCtrl.GetSelectedCount() );
	mApplication.SaveToClipBoard( mResultListCtrl, isWholeCopy );
}

BOOL CItemChangeDialog::PreTranslateMessage( MSG* pMsg )
{
	// 090304 LUJ, 툴팁 표시. 참조: http://kalguksu.egloos.com/130355
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		{
			mItemToolTipCtrl.RelayEvent( pMsg );
			break;
		}
	case WM_MOUSEHOVER:
		{
			if( pMsg->hwnd == mItemDescriptionStatic.GetSafeHwnd() )
			{
				mItemToolTipCtrl.RelayEvent( pMsg );
			}

			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CItemChangeDialog::OnBnClickedItemChangeFindButton()
{
	CItemAddDialog dialog( mApplication );
	
	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	const ItemScript& itemScript = mApplication.GetItemScript( dialog.GetSelectedItemIndex() );
	const int findIndexInCombo = mItemComboBox.FindString( 0, itemScript.mUnicodeName );

	if( -1 == findIndexInCombo )
	{
		CString textThereIsNoResult;
		textThereIsNoResult.LoadString( IDS_STRING1 );
		MessageBox( textThereIsNoResult );
		return;
	}

	mItemComboBox.SetCurSel( findIndexInCombo );
	OnCbnSelchangeItemchangeItemList();
}

void CItemChangeDialog::OnBnClickedItemChangeCountMaxButton()
{
	CString text;
	text.Format( L"%d", mMaxChangeCount );
	mCountEdit.SetWindowText( text );
}