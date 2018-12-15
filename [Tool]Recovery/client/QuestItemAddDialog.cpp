// QuestItemAddDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "QuestItemAddDialog.h"
#include ".\questitemadddialog.h"


// QuestItemAddDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuestItemAddDialog, CDialog)
CQuestItemAddDialog::CQuestItemAddDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CQuestItemAddDialog::IDD, pParent)
	, mApplication( application )
	, mQuestItemIndex( 0 )
{
	EnableAutomation();
}


CQuestItemAddDialog::~CQuestItemAddDialog()
{
}

void CQuestItemAddDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CQuestItemAddDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_QUEST_ITEM_ADD_QUEST_RADIO, mQuestRadio );
	DDX_Control( pDX, IDC_QUEST_ITEM_ADD_ITEM_RADIO, mItemRadio );
	DDX_Control(pDX, IDC_QUEST_ITEM_FINDED_QUEST_LIST, mQuestListCtrl);
	DDX_Control(pDX, IDC_QUEST_ITEM_FINDED_ITEM_LIST, mitemListCtrl);
	DDX_Control(pDX, IDC_QUEST_ITEM_ADD_EDIT, mKeywordEdit);
	DDX_Control(pDX, IDC_QUEST_ITEM_ADD_QUEST_DESC_STATIC, mQuestDescriptionStatic);
	DDX_Control(pDX, IDC_QUEST_ITEM_ADD_ITEM_DESC_STATIC, mItemDescriptionStatic);
}


BEGIN_MESSAGE_MAP(CQuestItemAddDialog, CDialog)
	ON_EN_CHANGE(IDC_QUEST_ITEM_ADD_EDIT, OnEnChangeQuestItemAddEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEST_ITEM_FINDED_QUEST_LIST, OnLvnItemchangedQuestItemFindedQuestList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEST_ITEM_FINDED_ITEM_LIST, OnLvnItemchangedQuestItemFindedItemList)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CQuestItemAddDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IQuestItemAddDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {6033C4D5-88A6-4350-8A8C-54DF74D456F8}
static const IID IID_IQuestItemAddDialog =
{ 0x6033C4D5, 0x88A6, 0x4350, { 0x8A, 0x8C, 0x54, 0xDF, 0x74, 0xD4, 0x56, 0xF8 } };

BEGIN_INTERFACE_MAP(CQuestItemAddDialog, CDialog)
	INTERFACE_PART(CQuestItemAddDialog, IID_IQuestItemAddDialog, Dispatch)
END_INTERFACE_MAP()


// QuestItemAddDialog 메시지 처리기입니다.

DWORD CQuestItemAddDialog::GetQuestItemIndex() const
{
	return mQuestItemIndex;
}


void CQuestItemAddDialog::OnEnChangeQuestItemAddEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우 이 컨트롤은
	// CDialog::마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여
	// CRichEditCtrl().SetEventMask()를 호출하도록 OnInitDialog() 함수를 재지정하지 않으면
	// 이 알림을 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString keyword;

	mKeywordEdit.GetWindowText( keyword );

	if( keyword.IsEmpty() )
	{
		return;
	}

	mQuestListCtrl.DeleteAllItems();
	mitemListCtrl.DeleteAllItems();

	if( mQuestRadio.GetCheck() )
	{
		const CclientApp::QuestStringMap& stringMap = mApplication.GetQuestStringMap();

		CString text;

		for(
			CclientApp::QuestStringMap::const_iterator it = stringMap.begin();
			stringMap.end() != it;
			++it )
		{
			const int row = mQuestListCtrl.GetItemCount();

			const CclientApp::QuestString& string = it->second;

			const std::wstring name( string.mTitle );

			if( std::wstring::npos == name.find( keyword ) )
			{
				continue;
			}

			text.Format( _T( "%u" ), string.mMainQuestIndex );
			mQuestListCtrl.InsertItem( row, text, 0 );

			text.Format( _T( "%u" ), string.mSubQuestIndex );
			mQuestListCtrl.SetItemText( row, 1, text );

			mQuestListCtrl.SetItemText( row, 2, string.mTitle );
		}
	}
	else if( mItemRadio.GetCheck() )
	{
		const CclientApp::QuestItemMap& itemMap = mApplication.GetQuestItemMap();

		CString text;

		for(
			CclientApp::QuestItemMap::const_iterator it = itemMap.begin();
			itemMap.end() != it;
			++it )
		{
			const int row = mitemListCtrl.GetItemCount();

			const CclientApp::QuestItem& item = it->second;

			const std::wstring name( item.mName );

			if( std::wstring::npos == name.find( keyword ) )
			{
				continue;
			}

			text.Format( _T( "%d" ), it->first );
			mitemListCtrl.InsertItem( row, text, 0 );

			mitemListCtrl.SetItemText( row, 1, item.mName );
		}
	}	
}


void CQuestItemAddDialog::OnLvnItemchangedQuestItemFindedQuestList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION position = mQuestListCtrl.GetFirstSelectedItemPosition();

	const int row = mQuestListCtrl.GetNextSelectedItem( position );

	if( 0 > row )
	{
		return;
	}

	const DWORD mainQuestIndex	= _ttoi( mQuestListCtrl.GetItemText( row, 0 ) );
	const DWORD subQuestIndex	= _ttoi( mQuestListCtrl.GetItemText( row, 1 ) );

	mQuestDescriptionStatic.SetWindowText(
		mApplication.GetQuestDescription( mainQuestIndex, subQuestIndex ) );
	
	// add quest item
	{
		mitemListCtrl.DeleteAllItems();

		const CclientApp::QuestItemMap& itemMap = mApplication.GetQuestItemMap();

		CString text;

		for(
			CclientApp::QuestItemMap::const_iterator it = itemMap.begin();
			itemMap.end() != it;
			++it )
		{
			const CclientApp::QuestItem& item = it->second;

            if( item.mMainQuestIndex != mainQuestIndex )
			{
				continue;
			}

			const int	row			= mitemListCtrl.GetItemCount();
			const DWORD	itemIndex	= it->first;

			text.Format( _T( "%d" ), itemIndex );

            mitemListCtrl.InsertItem( row, text, 0 );
			mitemListCtrl.SetItemText( row, 1, item.mName );
		}
	}	
}


void CQuestItemAddDialog::OnLvnItemchangedQuestItemFindedItemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION position = mitemListCtrl.GetFirstSelectedItemPosition();

	const int row = mitemListCtrl.GetNextSelectedItem( position );

	mQuestItemIndex = _ttoi( mitemListCtrl.GetItemText( row, 0 ) );

	mItemDescriptionStatic.SetWindowText(
		mApplication.GetQuestItemDescription( mQuestItemIndex ) );
}


void CQuestItemAddDialog::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}


BOOL CQuestItemAddDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	// list control
	{
		CRect rect;
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;

		{
			int step = -1;

			mitemListCtrl.GetWindowRect( &rect );
			mitemListCtrl.SetExtendedStyle( mitemListCtrl.GetExtendedStyle() | addStyle );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mitemListCtrl.InsertColumn( ++step, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mitemListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 1.0f ) );
		}

		{
			int step = -1;

			mQuestListCtrl.GetWindowRect( &rect );
			mQuestListCtrl.SetExtendedStyle( mQuestListCtrl.GetExtendedStyle() | addStyle );

			CString textMain;
			textMain.LoadString( IDS_STRING191 );

			CString textSub;
			textSub.LoadString( IDS_STRING192 );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );

			mQuestListCtrl.InsertColumn( ++step, textMain + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );
			mQuestListCtrl.InsertColumn( ++step, textSub + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mQuestListCtrl.InsertColumn( ++step, textName, LVCFMT_LEFT, int( rect.Width() * 1.0f ) );
		}
	}

	// radio button
	{
		mItemRadio.SetCheck( BST_CHECKED );
		mQuestRadio.SetCheck( BST_UNCHECKED );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}