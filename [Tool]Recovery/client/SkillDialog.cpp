// SkillDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "client.h"
#include "SkillDialog.h"
#include "SkillReportDialog.h"
// 080403 LUJ, 스킬 로그 처리
#include "LogDialog.h"
#include "SkillLogCommand.h"

IMPLEMENT_DYNAMIC(CSkillDialog, CDialog)
CSkillDialog::CSkillDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CSkillDialog::IDD, pParent)
	, mApplication( application )

{
	EnableAutomation();
}

CSkillDialog::~CSkillDialog()
{
}

void CSkillDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CSkillDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control( pDX, IDC_SKILL_MY_LIST, mMyListCtrl );
	DDX_Control(pDX, IDC_SKILL_LEVEL_COMBO, mLevelCombo);
	DDX_Control(pDX, IDC_SKILL_UPDATE_BUTTON, mUpdateButton);
	DDX_Control(pDX, IDC_SKILL_ENABLE_LIST, mEnableListCtrl);
	DDX_Control(pDX, IDC_SKILL_ENABLE_TIP_STATIC, mEnableSkillStatic);
	DDX_Control(pDX, IDC_SKILL_MY_TIP_STATIC, mMySkillStatic );
	DDX_Control(pDX, IDC_SKILL_MY_LIST, mMyListCtrl);
	DDX_Control(pDX, IDC_SKILL_REPORT_BUTTON, mReportButton);
	DDX_Control(pDX, IDC_SKILL_BUFF_LIST, mBuffListCtrl);
	DDX_Control(pDX, IDC_SKILL_FIND_COMBO, mSkillFindComboBox);
	DDX_Control(pDX, IDC_SKILL_FIND_STATIC, mSkillFindStatic);
	DDX_Control(pDX, IDC_SKILL_FIND_EDIT, mSkillFindEdit);
}


BEGIN_MESSAGE_MAP(CSkillDialog, CDialog)
	ON_BN_CLICKED(IDC_SKILL_UPDATE_BUTTON, OnBnClickedSkillUpdateButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SKILL_DIALOG_LIST, OnLvnItemchangedSkillDialogList)
	ON_BN_CLICKED(IDC_SKILL_REMOVE_BUTTON, OnBnClickedSkillRemoveButton)
	ON_BN_CLICKED(IDC_SKILL_ADD_BUTTON, OnBnClickedSkillAddButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SKILL_ENABLE_LIST, OnLvnItemchangedSkillEnableList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SKILL_MY_LIST, OnLvnItemchangedSkillMyList)
	ON_BN_CLICKED(IDC_SKILL_LOG_BUTTON, OnBnClickedSkillLogButton)
	ON_BN_CLICKED(IDC_SKILL_REPORT_BUTTON, OnBnClickedSkillReportButton)
	ON_EN_CHANGE(IDC_SKILL_FIND_EDIT, OnEnChangeSkillFindEdit)
	ON_CBN_SELCHANGE(IDC_SKILL_FIND_COMBO, OnCbnSelchangeSkillFindCombo)
	ON_BN_CLICKED(IDC_SKILL_BUFF_REMOVE_BUTTON, OnBnClickedSkillBuffRemoveButton)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSkillDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_ISkillDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {29F98D55-AAD8-4CFC-87D7-48E62C581E64}
static const IID IID_ISkillDialog =
{ 0x29F98D55, 0xAAD8, 0x4CFC, { 0x87, 0xD7, 0x48, 0xE6, 0x2C, 0x58, 0x1E, 0x64 } };

BEGIN_INTERFACE_MAP(CSkillDialog, CDialog)
	INTERFACE_PART(CSkillDialog, IID_ISkillDialog, Dispatch)
END_INTERFACE_MAP()


// CSkillDialog 메시지 처리기입니다.


void CSkillDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_SKILL_GET_ALL_ACK:
		{
			mSkillMap.clear();

			const MSG_RM_SKILL* m = ( MSG_RM_SKILL* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const SKILL_BASE&	data	= m->mData[ i ];
				
				if( mSkillMap.end() == mSkillMap.find( data.wSkillIdx ) )
				{
					mSkillMap.insert( std::make_pair( data.wSkillIdx, data ) );
				}
				else
				{
					ASSERT( 0 );
				}
			}

			PutSkill();

			break;
		}
	case MP_RM_SKILL_SET_LEVEL_ACK:
		{
			const MSG_DWORD3* m = ( MSG_DWORD3* )message;

			const DWORD skillDbIndex	= m->dwData1;
			const DWORD	skillIndex		= m->dwData2;
			const BYTE level			= BYTE( m->dwData3 );

			if( mSkillMap.end() == mSkillMap.find( skillIndex ) )
			{
				ASSERT( 0 && "Index is not exist" );
				return;
			}

			SKILL_BASE& data = mSkillMap[ skillIndex ];

			ASSERT( data.dwDBIdx == skillDbIndex );

			data.Level	= level;

			// 리스트 컨트롤 데이터 수정
			for( int i = 0; i < mMyListCtrl.GetItemCount(); ++i )
			{
				const CString dbIndex = mMyListCtrl.GetItemText( i, 0 );

				if( skillDbIndex == _ttoi( dbIndex ) )
				{
					CString text;
					text.Format( _T( "%d" ), level );

					mMyListCtrl.SetItemText( i, 3, text );
					break;
				}
			}

			// 메시지 박스 표시
			{
				CString textUpdateWasSucceed;
				textUpdateWasSucceed.LoadString( IDS_STRING28 );
				
				MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			}
			
			break;
		}
	case MP_RM_SKILL_ADD_ACK:
		{
			const MSG_RM_SKILL* m = ( MSG_RM_SKILL* )message;

			for( DWORD i = 0; i < m->mSize; ++i )
			{
				const SKILL_BASE& data = m->mData[ i ];

				mSkillMap.insert( std::make_pair( data.wSkillIdx, data ) );
			}

			PutSkill();

			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );

			break;
		}
	case MP_RM_SKILL_REMOVE_ACK:
		{
			const MSG_DWORD2* m = ( MSG_DWORD2* )message;

			mSkillMap.erase( m->dwData2 );

			PutSkill();
			
			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_SKILL_ADD_NACK:
	case MP_RM_SKILL_REMOVE_NACK:
	case MP_RM_SKILL_SET_LEVEL_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_ICONERROR | MB_OK );
			break;
		}
	case MP_RM_SKILL_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
		// 080731 LUJ, 버프 스킬 리스트 표시
	case MP_RM_SKILL_BUFF_ACK:
		{
			const MSG_RM_SKILL_BUFF* m = ( MSG_RM_SKILL_BUFF* )message;

			const CItemDialog* const itemDialog = mApplication.GetItemDialog();

			for( DWORD row = 0; row < m->mSize; ++row )
			{
				const MSG_RM_SKILL_BUFF::Buff& buff = m->mBuff[ row ];

				int		step = -1;
				CString text;

				text.Format( _T( "%d" ), buff.mSkillIndex );
				mBuffListCtrl.InsertItem( row, text, ++step );

				const ActiveSkillScript* const script = mApplication.GetActiveSkillScript( buff.mSkillIndex );
				mBuffListCtrl.SetItemText( row, ++step, ( script ? script->mName : _T( "?" ) ) );
				
				mBuffListCtrl.SetItemText( row, ++step, mApplication.GetDateString( buff.mRemainTime ) );
				text.Format( _T( "%d" ), buff.mRemainCount );
				mBuffListCtrl.SetItemText( row, ++step, text );
				mBuffListCtrl.SetItemText( row, ++step, CA2WEX< sizeof( buff.mEndDate ) >( buff.mEndDate ) );
			}

			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


BOOL CSkillDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// list control initialization
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;

		{
			mMyListCtrl.GetWindowRect( rect );

			CString textDbIndex;
			textDbIndex.LoadString( IDS_STRING13 );
			mMyListCtrl.InsertColumn( 0, textDbIndex,	LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mMyListCtrl.InsertColumn( 1, textIndex,		LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mMyListCtrl.InsertColumn( 2, textName,		LVCFMT_LEFT, int( rect.Width() * 0.5f ) );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );
			mMyListCtrl.InsertColumn( 3, textLevel,		LVCFMT_LEFT, int( rect.Width() * 0.15f ) );

			CString textPassive;
			textPassive.LoadString( IDS_STRING197 );
			mMyListCtrl.InsertColumn( 4, textPassive,		LVCFMT_LEFT, int( rect.Width() * 0.14f ) );
			
			CString textSkillPoint;
			textSkillPoint.LoadString( IDS_STRING96 );
			mMyListCtrl.InsertColumn( 5, textSkillPoint,		LVCFMT_LEFT, int( rect.Width() * 0.14f ) );

			CString textCost;
			textCost.LoadString( IDS_STRING198 );
			mMyListCtrl.InsertColumn( 6, textCost,	LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			mMyListCtrl.SetExtendedStyle( mMyListCtrl.GetExtendedStyle() | addStyle );
		}

		// 080731 LUJ, 버프 리스트 초기화
		{
			mBuffListCtrl.SetExtendedStyle( mBuffListCtrl.GetExtendedStyle() | addStyle );
			mBuffListCtrl.GetWindowRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mBuffListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mBuffListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.4f ) );

			CString textTime;
			textTime.LoadString( IDS_STRING102 );
			mBuffListCtrl.InsertColumn( 2, textTime, 	LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			CString textCount;
			textCount.LoadString( IDS_STRING42 );
			mBuffListCtrl.InsertColumn( 3, textCount, LVCFMT_LEFT, int( rect.Width() * 0.2f ) );

			CString textEndDate;
			textEndDate.LoadString( IDS_STRING292 );
			mBuffListCtrl.InsertColumn( 4, textEndDate,	LVCFMT_LEFT, int( rect.Width() * 0.2f ) );
		}

		{
			mEnableListCtrl.GetWindowRect( rect );

			CString textIndex;
			textIndex.LoadString( IDS_STRING22 );
			mEnableListCtrl.InsertColumn( 0, textIndex, LVCFMT_LEFT, 0 );

			CString textName;
			textName.LoadString( IDS_STRING20 );
			mEnableListCtrl.InsertColumn( 1, textName, LVCFMT_LEFT, int( rect.Width() * 0.7f ) );

			CString textPassive;
			textPassive.LoadString( IDS_STRING197 );
			mEnableListCtrl.InsertColumn( 2, textPassive, LVCFMT_LEFT, int( rect.Width() * 0.14f ) );

			mEnableListCtrl.SetExtendedStyle( mEnableListCtrl.GetExtendedStyle() | addStyle );
		}		
	}

	// combo box initialization
	{
		CRect rect;
		mLevelCombo.GetWindowRect( rect );
		mLevelCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		// 081202 LUJ, 스킬 찾기 콤보 초기화
		mSkillFindComboBox.GetWindowRect( rect );
		mSkillFindComboBox.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	// static initialization
	{
		mEnableSkillStatic.SetWindowText( _T( "" ) );
		mMySkillStatic.SetWindowText( _T( "" ) );
		mSkillFindStatic.SetWindowText( _T( "0" ) );
	}

	// 090219 LUJ, 툴팁
	{
		mSkillDescriptionToolTipCtrl.Create( this );
		mSkillDescriptionToolTipCtrl.Activate( TRUE );
		mSkillDescriptionToolTipCtrl.SetMaxTipWidth( 200 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSkillDialog::Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex )
{
	// 080731 LUJ, 초기화
	mBuffListCtrl.DeleteAllItems();

	mPlayerIndex	= playerIndex;
	mPlayerName		= playerName;
	mServerIndex	= serverIndex;

	{
		CString textTitleFormat;
		textTitleFormat.LoadString( IDS_STRING200 );

		CString text;
		text.Format( textTitleFormat, mApplication.GetServerName( serverIndex ), playerName );

		SetWindowText( text );
	}

	{
		MSGBASE message;

		message.Category	= MP_RM_SKILL;
		message.Protocol	= MP_RM_SKILL_GET_ALL_SYN;
		message.dwObjectID	= playerIndex;

		mApplication.Send( serverIndex, message, sizeof( message ) );
	}
}


void CSkillDialog::OnBnClickedSkillUpdateButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION position = mMyListCtrl.GetFirstSelectedItemPosition();

	const CString dbIndex = mMyListCtrl.GetItemText( mMyListCtrl.GetNextSelectedItem( position ), 0 );

	if( dbIndex.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString level;
	mLevelCombo.GetLBText( mLevelCombo.GetCurSel(), level );

	MSG_DWORD2 message;

	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_SET_LEVEL_SYN;
	message.dwObjectID	= mPlayerIndex;
	message.dwData1		= _ttoi( dbIndex );
	message.dwData2		= _ttoi( level );

	// TODO: 현재 레벨과 같으면 업데이트하지 않는 코드 추가할 것

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CSkillDialog::OnLvnItemchangedSkillDialogList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	*pResult = 0;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const DWORD row = pNMLV->iItem;
	
	// 스킬의 최대 레벨 개수만큼 콤보 박스를 세팅한다
	{
		const CString index = mMyListCtrl.GetItemText( row, 1 );

		const DWORD size = mApplication.GetSkillSize( _ttoi( index ) );
		CString text;

		mLevelCombo.ResetContent();

		for( DWORD i = 1; i <= size; i++ )
		{
			text.Format( _T( "%d" ), i );
			mLevelCombo.AddString( text );
		}
	}

	// 현재 레벨로 선택해놓는다
	{
		const CString level = mMyListCtrl.GetItemText( row, 3 );

		mLevelCombo.SetCurSel( _ttoi( level ) - 1 );
	}

	// 설명
	{
		const CString index	= mMyListCtrl.GetItemText( pNMLV->iItem, 1 );
		const TCHAR* const toolTipText = mApplication.GetTip( _ttoi( index ) );

		mMySkillStatic.SetWindowText( toolTipText );
	}
}


void CSkillDialog::OnBnClickedSkillRemoveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION	position	= mMyListCtrl.GetFirstSelectedItemPosition();
	CString		dbIndex		= mMyListCtrl.GetItemText( mMyListCtrl.GetNextSelectedItem( position ), 0 );

	if( dbIndex.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString textDoYouDeleteIt;
	textDoYouDeleteIt.LoadString( IDS_STRING108 );

	if( IDNO == MessageBox( textDoYouDeleteIt, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
	{
		return;
	}

	MSG_DWORD message;

	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_REMOVE_SYN;
	message.dwObjectID	= mPlayerIndex;
	message.dwData		= _ttoi( dbIndex );

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}

void CSkillDialog::OnBnClickedSkillAddButton()
{
	POSITION	position	= mEnableListCtrl.GetFirstSelectedItemPosition();
	CString		index		= mEnableListCtrl.GetItemText( mEnableListCtrl.GetNextSelectedItem( position ), 0 );

	const ActiveSkillScript* script = mApplication.GetActiveSkillScript( _ttoi( index ) );

	if( ! script )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}
	
	CString textDoYouAddIt;
	textDoYouAddIt.LoadString( IDS_STRING201 );

	if( IDNO == MessageBox( textDoYouAddIt, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
	{
		return;
	}

	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_ADD_SYN;
	message.dwObjectID	= mPlayerIndex;
	message.dwData		= script->mIndex;

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CSkillDialog::PutSkill()
{
	// 보유 스킬을 표시한다
	{
		mMyListCtrl.DeleteAllItems();

		DWORD usedMoney = 0;
		DWORD userPoint = 0;

		for( SkillMap::const_iterator it = mSkillMap.begin(); mSkillMap.end() != it; ++it )
		{
			const DWORD			index	= it->first;
			const SKILL_BASE&	data	= it->second;
			const ActiveSkillScript*	script	= mApplication.GetActiveSkillScript( index );

			if( ! script )
			{
				// 081021 LUJ, 로그를 표시하도록 한다
				mApplication.Log(
					_T( "skill script is not found. Index is %d" ),
					index );
				continue;
			}

			const DWORD row		= mMyListCtrl.GetItemCount();
			int			step	= 0;

			CString text;
			text.Format( _T( "%d" ), data.dwDBIdx );
			mMyListCtrl.InsertItem( row, text, 0 );

			text.Format( _T( "%d" ), data.wSkillIdx );
			mMyListCtrl.SetItemText( row, ++step, text );

			mMyListCtrl.SetItemText( row, ++step, script->mName );

			text.Format( _T( "%d" ), data.Level );
			mMyListCtrl.SetItemText( row, ++step, text );
			mMyListCtrl.SetItemText( row, ++step, script->mIsBuff ? _T( "*" ) : _T( "" ) );

			DWORD point	= 0;
			DWORD money	= 0;
			{
				for( int level = 1; level <= data.Level; ++level )
				{
					const ActiveSkillScript* ActiveSkillScript = mApplication.GetActiveSkillScript( index / 100 * 100 + level );

					if( ActiveSkillScript )
					{
						point += ActiveSkillScript->mPoint;
						money += ActiveSkillScript->mMoney;
					}
				}

				usedMoney += money;
				userPoint += point;
			}

			text.Format( _T( "%d" ), point );
            mMyListCtrl.SetItemText( row, ++step, text );

			text.Format( _T( "%d" ), money );
			mMyListCtrl.SetItemText( row, ++step, text );
		}

		CString textYouConsumeToLearnSkillHowManyPointAndGold;
		textYouConsumeToLearnSkillHowManyPointAndGold.LoadString( IDS_STRING202 );

		CString text;
		text.Format(
			textYouConsumeToLearnSkillHowManyPointAndGold,
			userPoint,
			usedMoney );

		mReportButton.SetWindowText( text );
	}
	
	// 추가 가능한 스킬을 표시한다
	{
		mEnableListCtrl.DeleteAllItems();

		// 이 맵에는 순수한 스킬 인덱스만 들어있다
		const CclientApp::SkillSizeMap& skillSizeMap = mApplication.GetSkillSizeMap();

		for( CclientApp::SkillSizeMap::const_iterator it = skillSizeMap.begin(); skillSizeMap.end() != it; ++it )
		{
			const DWORD index = it->first;

			if( mSkillMap.end() != mSkillMap.find( index ) )
			{
				continue;
			}

			const ActiveSkillScript* script = mApplication.GetActiveSkillScript( index );

			if( ! script )
			{
				continue;
			}
			// 080804 LUJ, 버프 스킬은 표시하지 않는다
			else if( script->mIndex < 1000000 )
			{
				continue;
			}

			const DWORD row		= mEnableListCtrl.GetItemCount();
			int			step	= 0;

			CString text;
			text.Format( _T( "%d" ), index );
			mEnableListCtrl.InsertItem( row, text, 0 );

			mEnableListCtrl.SetItemText( row, ++step, script->mName );

			if(script->mIsBuff)
			{
				mEnableListCtrl.SetItemText( row, ++step, _T( "*" ) );
			}
		}
	}
}


void CSkillDialog::OnLvnItemchangedSkillEnableList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	{
		const CString		index	= mEnableListCtrl.GetItemText( pNMLV->iItem, 0 );
		const ActiveSkillScript*	script	= mApplication.GetActiveSkillScript( _ttoi( index ) );

		// LUJ, 툴팁 표시 방식이 복잡해져서 일단 제거
		//mEnableSkillStatic.SetWindowText( mApplication.GetTip( _ttoi( index ) ) );
		mSkillFindComboBox.SetWindowText( script ? script->mName : _T( "" ) );
	}
}


void CSkillDialog::OnLvnItemchangedSkillMyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	*pResult = 0;

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const DWORD row = pNMLV->iItem;

	// 스킬의 최대 레벨 개수만큼 콤보 박스를 세팅한다
	{
		const CString index = mMyListCtrl.GetItemText( row, 1 );

		const DWORD size = mApplication.GetSkillSize( _ttoi( index ) );
		CString text;

		mLevelCombo.ResetContent();

		for( DWORD i = 1; i <= size; i++ )
		{
			text.Format( _T( "%d" ), i );
			mLevelCombo.AddString( text );
		}
	}

	// 현재 레벨로 선택해놓는다
	{
		const CString level = mMyListCtrl.GetItemText( row, 3 );

		mLevelCombo.SetCurSel( _ttoi( level ) - 1 );
	}

	// 설명
	{
		const CString index	= mMyListCtrl.GetItemText( pNMLV->iItem, 1 );
		const TCHAR* const description = mApplication.GetTip( _ttoi( index ) );

		mMySkillStatic.SetWindowText( description );
		mSkillDescriptionToolTipCtrl.AddTool( this, description );
	}
}

void CSkillDialog::OnBnClickedSkillLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textSkill;
		textSkill.LoadString( IDS_STRING365 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textSkill,
			textLog,
			mPlayerName,
			mPlayerIndex );
		
		dialog->DoModal(
			mServerIndex,
			CSkillLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


void CSkillDialog::OnBnClickedSkillReportButton()
{
	CSkillReportDialog dialog( mApplication );

	dialog.DoModal();
}

void CSkillDialog::OnEnChangeSkillFindEdit()
{
	CString keyword;
	mSkillFindEdit.GetWindowText( keyword );

	const int keywordMinimumLength = 2;

	if( keywordMinimumLength > keyword.GetLength() )
	{
		return;
	}

	const size_t count = mApplication.SetSkillCombo( mSkillFindComboBox, keyword );

	CString text;
	text.Format(
		_T( "%d" ),
		mSkillFindComboBox.GetCount() );
	mSkillFindStatic.SetWindowText( text );

	OnCbnSelchangeSkillFindCombo();
}

void CSkillDialog::OnCbnSelchangeSkillFindCombo()
{
	CString skillName;
	mSkillFindComboBox.GetWindowText( skillName );

	for(	int row = 0;
			row < mEnableListCtrl.GetItemCount();
			++row )
	{
		const CString name = mEnableListCtrl.GetItemText(
			row,
			1 );

		if( name == skillName )
		{
			// 081202 LUJ, 참조 http://kongmks.cafe24.com/282
			mEnableListCtrl.SetItemState( row, LVIS_SELECTED, LVIS_SELECTED );
			mEnableListCtrl.EnsureVisible( row, FALSE );
			break;
		}
	}
}

void CSkillDialog::OnBnClickedSkillBuffRemoveButton()
{
	POSITION	position	= mBuffListCtrl.GetFirstSelectedItemPosition();
	CString		index		= mBuffListCtrl.GetItemText( mBuffListCtrl.GetNextSelectedItem( position ), 0 );

	if( index.IsEmpty() )
	{
		CString textSelectTarget;
		textSelectTarget.LoadString( IDS_STRING31 );

		MessageBox( textSelectTarget, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	CString textDoYouDeleteIt;
	textDoYouDeleteIt.LoadString( IDS_STRING108 );

	if( IDNO == MessageBox( textDoYouDeleteIt, _T( "" ), MB_ICONWARNING | MB_YESNO ) )
	{
		return;
	}

	// 090211 LUJ, 서버에 메시지를 보낸다
	MSG_DWORD message;
	ZeroMemory( &message, sizeof( message ) );
    message.Category	= MP_RM_SKILL;
	message.Protocol	= MP_RM_SKILL_BUFF_REMOVE_SYN;
	message.dwObjectID	= mPlayerIndex;
	message.dwData		= _ttoi( index );
	mApplication.Send(
		mServerIndex,
		message,
		sizeof( message ),
		false );

	for( int row = 0; row < mBuffListCtrl.GetItemCount(); ++row )
	{
		if( mBuffListCtrl.GetItemText( row, 0 ) == index )
		{
			mBuffListCtrl.DeleteItem( row );
			break;
		}
	}

	// 090211 LUJ, 메시지를 표시한다
	CString textUpdateWasSucceed;
	textUpdateWasSucceed.LoadString( IDS_STRING28 );
	MessageBox( textUpdateWasSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
}

BOOL CSkillDialog::PreTranslateMessage( MSG* pMsg )
{
	// 090219 LUJ, 툴팁 표시. 참조: http://kalguksu.egloos.com/130355
	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		{
			mSkillDescriptionToolTipCtrl.RelayEvent( pMsg );
			break;
		}
	case WM_MOUSEHOVER:
		{
			if( pMsg->hwnd == mMySkillStatic.GetSafeHwnd() )
			{
				mSkillDescriptionToolTipCtrl.RelayEvent( pMsg );
			}

			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}