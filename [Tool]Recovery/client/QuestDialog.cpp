// QuestDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "QuestDialog.h"
#include "QuestItemAddDialog.h"
#include "QuantityDialog.h"
// 080403 LUJ, 퀘스트 로그 처리
#include "LogDialog.h"
#include "QuestLogCommand.h"


// CQuestDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CQuestDialog, CDialog)
CQuestDialog::CQuestDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CQuestDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();

}

CQuestDialog::~CQuestDialog()
{
}

void CQuestDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CQuestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control( pDX, IDC_QUEST_MAIN_LIST, mMainListCtrl );
	DDX_Control( pDX, IDC_QUEST_SUB_LIST, mSubListCtrl );
	DDX_Control( pDX, IDC_QUEST_ITEM_LIST, mItemListCtrl );
	DDX_Control( pDX, IDC_QUEST_MONSTER_LIST, mMonsterListCtrl );
	DDX_Control( pDX, IDC_QUEST_DESCRIPTION, mDescrptionStatic );
	DDX_Control(pDX, IDC_QUEST_SELECTED_ONE_FINISH_BUTTON, mFinishSubQuestButton);
	DDX_Control(pDX, IDC_QUEST_MAIN_LIST2, mMainListCtrl);
	DDX_Control(pDX, IDC_QUEST_ITEM_TOTAL_LIST, mTotalItemListCtrl);
	DDX_Control(pDX, IDC_QUEST_ITEM_STATIC, mItemDescriptionStatic);
	//DDX_Control(pDX, IDC_QUEST_ITEM_SIZE_COMBO, mItemSizeCombo);
	DDX_Control(pDX, IDC_QUEST_ITEM_UPDATE_BUTTON, mItemUpdateButton);
	DDX_Control(pDX, IDC_QUEST_ITEM_SIZE_EDIT, mItemSizeEdit);
	DDX_Control(pDX, IDC_QUEST_REPORT_STATIC, mReportStatic);
	DDX_Control(pDX, IDC_QUEST_MAIN_TITLE_STATIC, mMainQuestStatic);
}


BEGIN_MESSAGE_MAP(CQuestDialog, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEST_MAIN_LIST, OnLvnItemchangedQuestMainList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEST_SUB_LIST, OnLvnItemchangedQuestSubList)
	ON_BN_CLICKED(IDC_QUEST_SELECTED_ONE_FINISH_BUTTON, OnBnClickedQuestSelectedOneFinishButton)
	ON_BN_CLICKED(IDC_QUEST_MAIN_DELETE_BUTTON, OnBnClickedQuestMainDeleteButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_QUEST_ITEM_TOTAL_LIST, OnLvnItemchangedQuestItemTotalList)
	ON_BN_CLICKED(IDC_QUEST_ITEM_UPDATE_BUTTON, OnBnClickedQuestItemUpdateButton)
	//ON_BN_CLICKED(IDC_QUEST_ITEM_UPDATE_BUTTON2, OnBnClickedQuestItemUpdateButton2)
	ON_BN_CLICKED(IDC_QUEST_ITEM_DELETE_BUTTON, OnBnClickedQuestItemDeleteButton)
	ON_BN_CLICKED(IDC_QUEST_LOG_BUTTON, OnBnClickedQuestLogButton)
	ON_BN_CLICKED(IDC_QUEST_ITEM_ADD_BUTTON, OnBnClickedQuestItemAddButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CQuestDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IQuestDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {4ABE8824-9060-4AF4-9274-8179C7E9E31C}
static const IID IID_IQuestDialog =
{ 0x4ABE8824, 0x9060, 0x4AF4, { 0x92, 0x74, 0x81, 0x79, 0xC7, 0xE9, 0xE3, 0x1C } };

BEGIN_INTERFACE_MAP(CQuestDialog, CDialog)
	INTERFACE_PART(CQuestDialog, IID_IQuestDialog, Dispatch)
END_INTERFACE_MAP()


// CQuestDialog 메시지 처리기입니다.
void CQuestDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_QUEST_GET_MAIN_DATA_ACK:
		{
			const SEND_MAINQUEST_DATA* m = ( SEND_MAINQUEST_DATA* )message;

			for( DWORD i = 0; i < m->wCount; ++i )
			{
				const QMBASE& data = m->QuestList[ i ];

				++mCheckMap[ data.QuestIdx ];

				mMainQuestMap.insert( std::make_pair( data.QuestIdx, data ) );
			}

			{
				mMainListCtrl.DeleteAllItems();

				CString text;

				for( MainQuestMap::const_iterator it = mMainQuestMap.begin(); mMainQuestMap.end() != it; ++it, ++i )
				{
					const DWORD		row		= mMainListCtrl.GetItemCount();
					const QMBASE&	data	= it->second;					

					text.Format( _T( "%d" ), data.QuestIdx );
					mMainListCtrl.InsertItem( row, text, 0 );

					if( mSubQuestMap.end() == mSubQuestMap.find( data.QuestIdx ) )
					{
						mMainListCtrl.SetItemText( row, 1, _T( "v" ) );
					}
					
					mMainListCtrl.SetItemText( row, 2, mApplication.GetQuestTitle( data.QuestIdx, 0 ) );
					mMainListCtrl.SetItemText( row, 3, GetTimeText( data.registTime ) );
				}
			}

			// 중복 퀘스트 검사
			{
				// 값: 퀘스트 인덱스
				typedef std::list< DWORD >	DuplicatedList;
				DuplicatedList				duplicatedList;

				for(
					CheckMap::const_iterator it = mCheckMap.begin();
					mCheckMap.end() != it;
					++it )
				{
					const DWORD index	= it->first;
					const DWORD size	= it->second;

					if( 1 < size )
					{
						duplicatedList.push_back( index );
					}
				}

				if( ! duplicatedList.empty() )
				{
					CString textSomeQuestAreDuplicated;
					textSomeQuestAreDuplicated.LoadString( IDS_STRING187 );

					CString text;
					text.Format( textSomeQuestAreDuplicated + _T( ": " ), duplicatedList.size() );

					for(
						DuplicatedList::const_iterator it = duplicatedList.begin();
						duplicatedList.end() != it;
						++it )
					{
						CString index;
						index.Format( _T( "[%d]" ), *it );

						text += index;
					}

					mReportStatic.SetWindowText( text );
				}

				// 080901 LUJ, 수행 중인 퀘스트 개수 표시
				{
					CString textMainQuest;
					textMainQuest.LoadString( IDS_STRING289 );
					
					CString text;
					text.Format(
						_T( "%s(%u/20)" ),
						textMainQuest,
						mSubQuestMap.size() );

					mMainQuestStatic.SetWindowText( text );
				}
			}

			mMainListCtrl.EnableWindow( TRUE );

			break;
		}
	case MP_RM_QUEST_GET_SUB_DATA_ACK:
		{
			const SEND_SUBQUEST_DATA* m = ( SEND_SUBQUEST_DATA* )message;

			for( DWORD i = 0; i < m->wCount; ++i )
			{
				const QSUBASE& data	= m->QuestList[ i ];

				// 080901 LUJ, 유효한 번호만 맵에 넣도록 한다
				if( ! data.QuestIdx )
				{
					continue;
				}

				SubQuest& subQuest				= mSubQuestMap[ data.QuestIdx ];
				subQuest[ data.SubQuestIdx ]	= data;
			}

			mSubListCtrl.EnableWindow( TRUE );

			break;
		}
	case MP_RM_QUEST_GET_ITEM_DATA_ACK:
		{
			mItemMap.clear();
			
			const SEND_QUESTITEM* m = ( SEND_QUESTITEM* )message;

			for( DWORD i = 0; i < m->wCount; ++i )
			{
				const QITEMBASE& data = m->ItemList[ i ];

				mItemMap[ data.ItemIdx ] = data;

				mItemMap.insert( std::make_pair( data.ItemIdx, data ) );
			}

			RefreshItem();

			mItemListCtrl.EnableWindow( TRUE );
			break;
		}
	case MP_RM_QUEST_ITEM_ADD_ACK:
		{
			const SEND_QUESTITEM* m = ( SEND_QUESTITEM* )message;

			for( DWORD i = 0; i < m->wCount; ++i )
			{
				const QITEMBASE& data = m->ItemList[ i ];

				mItemMap[ data.ItemIdx ] = data;

				mItemMap.insert( std::make_pair( data.ItemIdx, data ) );
			}

			RefreshItem();
			break;
		}
	case MP_RM_QUEST_FINISH_SUB_ACK:
		{
			const MSG_DWORD3*	m		= ( MSG_DWORD3* )message;

			const DWORD			mainQuestIndex	= m->dwData1;
			const DWORD			subQuestIndex	= m->dwData2;
			const QSTATETYPE	state			= m->dwData3;
						
			if( mMainQuestMap.end() == mMainQuestMap.find( mainQuestIndex ) )
			{
				ASSERT( 0 );
			}

			QMBASE&	data		= mMainQuestMap[ mainQuestIndex ];
			data.state.value	= state;

			if( mSubQuestMap.end() == mSubQuestMap.find( mainQuestIndex ) )
			{
				ASSERT( 0 );
				return;
			}

			// 서브 퀘스트 지우고
			{
				SubQuest& subQuest = mSubQuestMap[ mainQuestIndex ];
				subQuest.erase( subQuestIndex );

				if( subQuest.empty() )
				{
					mSubQuestMap.erase( mainQuestIndex );
				}
			}			

			// 초기화
			{
				mMainListCtrl.SetSelectionMark( -1 );

				mSubListCtrl.DeleteAllItems();
				mItemListCtrl.DeleteAllItems();
				mMonsterListCtrl.DeleteAllItems();

				mDescrptionStatic.SetWindowText( _T( "" ) );
			}

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );

			break;
		}
	case MP_RM_QUEST_REMOVE_ACK:
		{
			const MSG_DWORD*	m				= ( MSG_DWORD* )message;
			const DWORD			mainQuestIndex	= m->dwData;

			mMainQuestMap.erase( mainQuestIndex );
			mSubQuestMap.erase( mainQuestIndex );

			{
				POSITION position = mMainListCtrl.GetFirstSelectedItemPosition();
				mMainListCtrl.DeleteItem( mMainListCtrl.GetNextSelectedItem( position ) );

				mItemListCtrl.DeleteAllItems();
				mSubListCtrl.DeleteAllItems();
				mMonsterListCtrl.DeleteAllItems();

				mMainListCtrl.SetSelectionMark( -1 );
				mSubListCtrl.SetSelectionMark( - 1 );
				mItemListCtrl.SetSelectionMark( -1 );
				mMonsterListCtrl.SetSelectionMark( -1 );
			}

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_QUEST_FINISH_SUB_NACK:
	case MP_RM_QUEST_ITEM_UPDATE_NACK:
	case MP_RM_QUEST_REMOVE_NACK:
	case MP_RM_QUEST_ITEM_DELETE_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_QUEST_ITEM_UPDATE_ACK:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			const DWORD questItemIndex	= m->dwData1;
			const WORD	size			= WORD( m->dwData2 );
            
			if( mItemMap.end() == mItemMap.find( questItemIndex ) )
			{
				ASSERT( 0 );
				break;
			}

			QITEMBASE& item = mItemMap[ questItemIndex ];
			item.Count	= size;

			RefreshItem();

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_QUEST_ITEM_DELETE_ACK:
		{
			const MSG_DWORD* m = ( MSG_DWORD* )message;

			const DWORD questItemIndex	= m->dwData;

			mItemMap.erase( questItemIndex );
			RefreshItem();

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_QUEST_NACK_BY_AUTH:
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


BOOL CQuestDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CDataExchange dx( this, TRUE );
	DoDataExchange( &dx );

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// list control initialization
	{
		CRect rect;
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;

		// main quest
		{			
			mMainListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mMainListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textIsOver;
			textIsOver.LoadString( IDS_STRING188 );
			mMainListCtrl.InsertColumn( 1, textIsOver, LVCFMT_LEFT, int( rect.Width() * 0.17f ) );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMainListCtrl.InsertColumn( 2, textName, LVCFMT_LEFT, int( rect.Width() * 0.36f ) );

			CString textCreatedDate;
			textCreatedDate.LoadString( IDS_STRING535 );
			mMainListCtrl.InsertColumn( 3, textCreatedDate, LVCFMT_LEFT, int( rect.Width() * 0.46f ) );
			
			mMainListCtrl.SetExtendedStyle( mMainListCtrl.GetExtendedStyle() | addStyle );
		}

		// sub quest
		{
			mSubListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mSubListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textInformation;
			textInformation.LoadString( IDS_STRING69 );
			mSubListCtrl.InsertColumn( 1, textInformation, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mSubListCtrl.InsertColumn( 2, textName, LVCFMT_LEFT, int( rect.Width() * 0.5f ) );

			CString textCreatedDate;
			textCreatedDate.LoadString( IDS_STRING535 );
			mSubListCtrl.InsertColumn( 3, textCreatedDate, LVCFMT_LEFT, int( rect.Width() * 0.49f ) );

			mSubListCtrl.SetExtendedStyle( mSubListCtrl.GetExtendedStyle() | addStyle );
		}

		// item
		{
			mItemListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mItemListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mItemListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );
			mItemListCtrl.InsertColumn( 2, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.29f ) );

			mItemListCtrl.SetExtendedStyle( mItemListCtrl.GetExtendedStyle() | addStyle );
		}

		// monster
		{
			mMonsterListCtrl.GetClientRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mMonsterListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMonsterListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );
			mMonsterListCtrl.InsertColumn( 2, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.29f ) );

			mMonsterListCtrl.SetExtendedStyle( mMonsterListCtrl.GetExtendedStyle() | addStyle );
		}

		// 전체 아이템 목록
		{
			mTotalItemListCtrl.GetClientRect( rect );

			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );

			CString textQuest;
			textQuest.LoadString( IDS_STRING190 );

			CString textMain;
			textMain.LoadString( IDS_STRING191 );

			CString textSub;
			textSub.LoadString( IDS_STRING192 );

			mTotalItemListCtrl.InsertColumn( 0, textItem + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );
			mTotalItemListCtrl.InsertColumn( 1, textQuest + _T( " " ) + textMain + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );
			mTotalItemListCtrl.InsertColumn( 2, textQuest + _T( " " ) + textSub + _T( " " ) + textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mTotalItemListCtrl.InsertColumn( 3, textName, LVCFMT_LEFT, int( rect.Width() * 0.70f ) );

			CString textQuantity;
			textQuantity.LoadString( IDS_STRING42 );
			mTotalItemListCtrl.InsertColumn( 4, textQuantity, LVCFMT_LEFT, int( rect.Width() * 0.29f ) );

			mTotalItemListCtrl.SetExtendedStyle( mTotalItemListCtrl.GetExtendedStyle() | addStyle );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CQuestDialog::OnLvnItemchangedQuestMainList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 메인 퀘스트를 선택했을 때 들어옴

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    const DWORD mainQuestIndex = _ttoi( mMainListCtrl.GetItemText( pNMLV->iItem, 0 ) );

	RefreshSubQuest();
}


void CQuestDialog::OnLvnItemchangedQuestSubList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 서브 퀘스트를 선택했을 때 들어옴

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION			position		= mMainListCtrl.GetFirstSelectedItemPosition();

	const DWORD			mainQuestIndex	= _ttoi( mMainListCtrl.GetItemText( mMainListCtrl.GetNextSelectedItem( position ), 0 ) );
	const DWORD			subQuestIndex	= _ttoi( mSubListCtrl.GetItemText( pNMLV->iItem, 0 ) );

	const QuestScript*	script			= mApplication.GetQuestScript( mainQuestIndex, subQuestIndex );

	if( ! script )
	{
		ASSERT( 0 );
		return;
	}

	{
		mItemListCtrl.DeleteAllItems();

		CString text;

		// 아이템 리스트에 값을 넣자
		for(	QuestScript::ItemMap::const_iterator it = script->mItemMap.begin();
				script->mItemMap.end() != it;
				++it )
		{
			const DWORD		itemIndex	= it->first;
			const DURTYPE	quantity	= it->second;
			const int		row			= mItemListCtrl.GetItemCount();

			text.Format( _T( "%d" ), itemIndex );
			mItemListCtrl.InsertItem( row, text, 0 );
			mItemListCtrl.SetItemText( row, 1, mApplication.GetQuestItemName( itemIndex ) );

			if( mItemMap.end() == mItemMap.find( itemIndex ) )
			{
				text.Format( _T( "%d/%d" ), 0, quantity );
			}
			else
			{
				const QITEMBASE& item = mItemMap[ itemIndex ];

				text.Format( _T( "%d/%d" ), item.Count, quantity );
			}

			mItemListCtrl.SetItemText( row, 2, text );
		}
	}	

	{
		mMonsterListCtrl.DeleteAllItems();

		// 몬스터 리스트에 값을 넣자
		if( script->mMonsterHuntCount &&
			mSubQuestMap.end() != mSubQuestMap.find( mainQuestIndex ) )
		{
			CString text;

			text.Format( _T( "%d" ), script->mMonsterIndex );
			mMonsterListCtrl.InsertItem( 0, text, 0 );

			CString textHuntingTarget;
			textHuntingTarget.LoadString( IDS_STRING193 );

			mMonsterListCtrl.SetItemText( 0, 1, textHuntingTarget );

			const DWORD huntedCount = _ttoi( mSubListCtrl.GetItemText( pNMLV->iItem, 1 ) );
			text.Format( _T( "%d/%d" ), huntedCount, script->mMonsterHuntCount );
			mMonsterListCtrl.SetItemText( 0, 2, text );
		}
	}
	
	// 설명 추가
	mDescrptionStatic.SetWindowText( mApplication.GetQuestDescription( mainQuestIndex, subQuestIndex ) );

	mFinishSubQuestButton.EnableWindow( TRUE );
}

LPCTSTR CQuestDialog::GetTimeText(__time64_t time) const
{
	static TCHAR text[MAX_PATH] = {0};
	ZeroMemory(
		text,
		sizeof(text));

	if(0 > time)
	{
		stTIME timeData;
		timeData.SetTime(
			DWORD(time));
		_sntprintf(
			text,
			_countof(text),
			_T("%d.%d.%d %02d:%02d:%02d"),			
			2000 + timeData.GetYear(),
			timeData.GetMonth(),
			timeData.GetDay(),
			timeData.GetHour(),
			timeData.GetMinute(),
			timeData.GetSecond());
	}
	else if(0 == time)
	{
		;
	}
	else
	{
		_tcsftime(
			text,
			_countof(text),
			_T("%Y %B %d %A"),
			_localtime64(&time));
	}

	return text;
}


BOOL CQuestDialog::IsEndSubQuest( DWORD subQuestIndex, DWORD state ) const
{
	const DWORD bit = ( 1 << ( 31 - subQuestIndex ) );

	return subQuestIndex & bit;
}


void CQuestDialog::OnBnClickedQuestSelectedOneFinishButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION		position		= mSubListCtrl.GetFirstSelectedItemPosition();
	const DWORD		row				= mSubListCtrl.GetNextSelectedItem( position );
	const DWORD		subQuestIndex	= _ttoi( mSubListCtrl.GetItemText( row, 0 ) );
	const CString	subQuestTitile	= mSubListCtrl.GetItemText( row, 2 );

	if( subQuestTitile.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString textWarningAboutSubQuestFinishForcely;
	textWarningAboutSubQuestFinishForcely.LoadString( IDS_STRING194 );

	CString text;
	text.Format( textWarningAboutSubQuestFinishForcely, subQuestTitile );

	if( IDNO == MessageBox( text, _T( "" ), MB_ICONQUESTION | MB_YESNO ) )
	{
		return;
	}

	position					= mMainListCtrl.GetFirstSelectedItemPosition();
	const DWORD	mainQuestIndex	= _ttoi( mMainListCtrl.GetItemText( mMainListCtrl.GetNextSelectedItem( position ), 0 ) );

	if( ! mainQuestIndex )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
	}
	else if( mMainQuestMap.end() == mMainQuestMap.find( mainQuestIndex ) )
	{
		ASSERT( 0 && "Selected main quest is invalid" );
		return;
	}

	// 메인 퀘스트의 서브 퀘스트 비트 플래그에 세팅하고 전송하자
	{
		MSG_DWORD3 message;
		{
			message.Category	= MP_RM_QUEST;
			message.Protocol	= MP_RM_QUEST_FINISH_SUB_SYN;

			message.dwObjectID	= mPlayerIndex;
			message.dwData1		= mainQuestIndex;
			message.dwData2		= subQuestIndex;
		}

		// 플래그 세팅
		{
			const QMBASE& data = mMainQuestMap[ mainQuestIndex ];
			
			message.dwData3 = data.state.value | ( 1 << ( 31 - subQuestIndex ) );
		}

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}


void CQuestDialog::Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex )
{
	mPlayerIndex	= playerIndex;
	mPlayerName		= playerName;
	mServerIndex	= serverIndex;

	{
		CString textTitleFormat;
		textTitleFormat.LoadString( IDS_STRING195 );

		CString	text;
		text.Format( textTitleFormat, mApplication.GetServerName( serverIndex ), playerName );

		SetWindowText( text );
	}

	{
		mMainQuestMap.clear();
		mSubQuestMap.clear();
		mItemMap.clear();
		mCheckMap.clear();

		mMainListCtrl.DeleteAllItems();
		mSubListCtrl.DeleteAllItems();
		mItemListCtrl.DeleteAllItems();
		mTotalItemListCtrl.DeleteAllItems();

		mDescrptionStatic.SetWindowText( _T( "" ) );
		mFinishSubQuestButton.EnableWindow( FALSE );

		CString textThereIsNoDuplicatedQuest;
		textThereIsNoDuplicatedQuest.LoadString( IDS_STRING196 );

		mReportStatic.SetWindowText( textThereIsNoDuplicatedQuest );
	}

	// 데이터가 오기전까지 컨트롤을 잠근다
	{
		mMainListCtrl.EnableWindow( FALSE );
		mSubListCtrl.EnableWindow( FALSE );
		mItemListCtrl.EnableWindow( FALSE );
	}

	{
		MSGBASE message;

		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_GET_MAIN_DATA_SYN;
		message.dwObjectID	= playerIndex;

		mApplication.Send( serverIndex, message, sizeof( message ) );
	}
}


void CQuestDialog::OnBnClickedQuestMainDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION position = mMainListCtrl.GetFirstSelectedItemPosition();

	const CString index = mMainListCtrl.GetItemText( mMainListCtrl.GetNextSelectedItem( position ), 0 );

	if( index.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString textWarningAboutDeletingMainQuest;
	textWarningAboutDeletingMainQuest.LoadString( IDS_STRING197 );

	if( IDNO == MessageBox( textWarningAboutDeletingMainQuest, _T( "" ), MB_ICONWARNING | MB_YESNO ))
	{
		return;
	}

	MSG_DWORD message;
	message.Category	= MP_RM_QUEST;
	message.Protocol	= MP_RM_QUEST_REMOVE_SYN;
	message.dwObjectID	= mPlayerIndex;
	message.dwData		= _ttoi( index );

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CQuestDialog::OnLvnItemchangedQuestItemTotalList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	const int selectedRow = pNMLV->iItem;

	const DWORD questItemIndex	= _ttoi( mTotalItemListCtrl.GetItemText( selectedRow, 0 ) );
	const DWORD questMainIndex	= _ttoi( mTotalItemListCtrl.GetItemText( selectedRow, 1 ) );
	const DWORD questSubIndex	= _ttoi( mTotalItemListCtrl.GetItemText( selectedRow, 2 ) );

	mItemDescriptionStatic.SetWindowText( mApplication.GetQuestItemDescription( questItemIndex ) );

	// 해당 메인퀘스트가 선택되도록 한다
	for( int row = 0; row < mMainListCtrl.GetItemCount(); ++row )
	{
		if( questMainIndex == _ttoi( mMainListCtrl.GetItemText( row, 0 ) ) )
		{
			// 081202 LUJ, 참조 http://kongmks.cafe24.com/282
			mMainListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
			mMainListCtrl.EnsureVisible( row, FALSE );
			break;
		}
	}
	
	// 서브퀘스트 목록 업데이트
	{
		RefreshSubQuest();

		for( int row = 0; row < mSubListCtrl.GetItemCount(); ++row )
		{
			if( questSubIndex == _ttoi( mSubListCtrl.GetItemText( row, 0 ) ) )
			{
				// 081202 LUJ, 참조 http://kongmks.cafe24.com/282
				mSubListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
				mSubListCtrl.EnsureVisible( row, FALSE );
				break;
			}
		}
	}
	
	// 아이템 개수 세팅
	mItemSizeEdit.SetWindowText( mTotalItemListCtrl.GetItemText( selectedRow, 4 ) );
	
	RedrawWindow();
}


void CQuestDialog::RefreshItem()
{
	mTotalItemListCtrl.DeleteAllItems();

	// 전체 퀘스트 아이템 목록에 표시
	for( ItemMap::const_iterator it = mItemMap.begin(); mItemMap.end() != it; ++it )
	{
		const QITEMBASE& item = it->second;

		const int row = mTotalItemListCtrl.GetItemCount();

		CString text;
		text.Format( _T( "%d" ), item.ItemIdx );

		mTotalItemListCtrl.InsertItem( row, text, 0);

		text.Format( _T( "%d" ), mApplication.GetMainQuestIndex( item.ItemIdx	) );
		mTotalItemListCtrl.SetItemText( row, 1, text );

		text.Format( _T( "%d" ), mApplication.GetSubQuestIndex( item.ItemIdx) );
		mTotalItemListCtrl.SetItemText( row, 2, text );

		mTotalItemListCtrl.SetItemText( row, 3, mApplication.GetQuestItemName( item.ItemIdx ) );

		text.Format( _T( "%d" ), item.Count );
		mTotalItemListCtrl.SetItemText( row, 4, text );;					
	}
}


void CQuestDialog::RefreshSubQuest()
{
	mItemListCtrl.DeleteAllItems();
	mMonsterListCtrl.DeleteAllItems();
	mSubListCtrl.DeleteAllItems();

	POSITION	position		= mMainListCtrl.GetFirstSelectedItemPosition();
	const int	row				= mMainListCtrl.GetNextSelectedItem( position );
	const DWORD mainQuestIndex	= _ttoi( mMainListCtrl.GetItemText( row, 0 ) );

	if( mSubQuestMap.end() == mSubQuestMap.find( mainQuestIndex ) )
	{
		// 완료된 것은 서브 퀘스트가 없다
		//ASSERT( 0 );
		return;
	}

	const SubQuest& subQuest = mSubQuestMap[ mainQuestIndex ];

	CString text;

	for( SubQuest::const_iterator it = subQuest.begin(); subQuest.end() != it; ++it )
	{
		const QSUBASE& SubQuestMap = it->second;

		if( mMainQuestMap.end() == mMainQuestMap.find( SubQuestMap.QuestIdx ) )
		{
			continue;
		}

		// QFLAG::IsSet()이 상수 연산을 하지 않으므로 값 복사
		const TCHAR* title = mApplication.GetQuestTitle( SubQuestMap.QuestIdx, SubQuestMap.SubQuestIdx );

		if( ! title )
		{
			// 퀘스트 문자열이 없는 경우 다음 퀘스트로 넘어가기 위한 장치적 퀘스트이다.
			continue;
		}

		const DWORD row = mSubListCtrl.GetItemCount();

		const QMBASE& MainQuestMap = mMainQuestMap[ SubQuestMap.QuestIdx ];

		text.Format( _T( "%d" ), SubQuestMap.SubQuestIdx );
		mSubListCtrl.InsertItem( row, text, 0 );

		text.Format( _T( "%d" ), SubQuestMap.state );
		mSubListCtrl.SetItemText( row, 1, text );

		mSubListCtrl.SetItemText( row, 2, title );
		mSubListCtrl.SetItemText( row, 3, GetTimeText( SubQuestMap.time ) );
	}
	
	mDescrptionStatic.SetWindowText( _T( "" ) );
	mFinishSubQuestButton.EnableWindow( mSubListCtrl.GetItemCount() );
}


void CQuestDialog::OnBnClickedQuestItemUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION		position		= mTotalItemListCtrl.GetFirstSelectedItemPosition();
	const int		row				= mTotalItemListCtrl.GetNextSelectedItem( position );

	const CString	itemIndex		= mTotalItemListCtrl.GetItemText( row, 0 );
	const CString	questMainIndex	= mTotalItemListCtrl.GetItemText( row, 1 );
	
	CString size;
	mItemSizeEdit.GetWindowText( size );

	{
		MSG_DWORD3	message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_UPDATE_SYN;
		message.dwObjectID	= mPlayerIndex;
		message.dwData1		= _ttoi( questMainIndex );
		message.dwData2		= _ttoi( itemIndex );
		message.dwData3		= _ttoi( size );

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CQuestDialog::OnBnClickedQuestItemDeleteButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION		position		= mTotalItemListCtrl.GetFirstSelectedItemPosition();
	const int		row				= mTotalItemListCtrl.GetNextSelectedItem( position );

	const DWORD		itemIndex		= _ttoi( mTotalItemListCtrl.GetItemText( row, 0 ) );
	const DWORD		questMainIndex	= _ttoi( mTotalItemListCtrl.GetItemText( row, 1 ) );

	{
		CString textMayIDeleteItem;
		textMayIDeleteItem.LoadString( IDS_STRING108 );

		if( IDNO == MessageBox( textMayIDeleteItem, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
		{
			return;
		}
	}

	{
		MSG_DWORD2	message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_DELETE_SYN;
		message.dwObjectID	= mPlayerIndex;
		message.dwData1		= questMainIndex;
		message.dwData2		= itemIndex;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}


void CQuestDialog::OnBnClickedQuestLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textQuest;
		textQuest.LoadString( IDS_STRING190 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textQuest,
			textLog,
			mPlayerName,
			mPlayerIndex );

		dialog->DoModal(
			mServerIndex,
			CQuestLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


void CQuestDialog::OnBnClickedQuestItemAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DWORD itemIndex;
	{
		CQuestItemAddDialog dialog( mApplication );

		if( IDOK != dialog.DoModal() )
		{
			return;
		}

		itemIndex = dialog.GetQuestItemIndex();
	}
	
	const CclientApp::QuestItemMap&					itemMap = mApplication.GetQuestItemMap();
	const CclientApp::QuestItemMap::const_iterator	it		= itemMap.find( itemIndex );

	if( itemMap.end() == it )
	{
		return;
	}

	const CclientApp::QuestItem& item = it->second;

	// check item is existed already
	{
		for( int row = 0; mTotalItemListCtrl.GetItemCount() > row; ++row)
		{
			if( itemIndex == _ttoi( mTotalItemListCtrl.GetItemText( row, 0 ) ) )
			{
				CString textThereIsExistedAlready;
				textThereIsExistedAlready.LoadString( IDS_STRING180 );

				MessageBox( textThereIsExistedAlready, 0, MB_OK | MB_ICONERROR );
				return;
			}
		}
	}

	DWORD quantity;
	{
		CQuantityDialog dialog;
		
		if( IDOK != dialog.DoModal() )
		{
			return;
		}

		quantity = dialog.GetQuantity();
	}

	{
		MSG_DWORD3 message;
		message.Category	= MP_RM_QUEST;
		message.Protocol	= MP_RM_QUEST_ITEM_ADD_SYN;
		message.dwObjectID	= mPlayerIndex;
		message.dwData1		= item.mMainQuestIndex;
		message.dwData2		= itemIndex;
		message.dwData3		= quantity;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}	
}