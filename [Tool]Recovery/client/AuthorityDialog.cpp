// 081021 LUJ, 권한 관리 창 추가
// AuthorityDialog.cpp : 구현 파일입니다.

#include "stdafx.h"
#include "client.h"
#include "AuthorityDialog.h"
#include ".\authoritydialog.h"

// CAuthorityDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAuthorityDialog, CDialog)
CAuthorityDialog::CAuthorityDialog( CclientApp& application, CWnd* pParent /*=NULL*/) :
CDialog(CAuthorityDialog::IDD, pParent),
mApplication( application ),
mServerIndex( 0 )
{
	EnableAutomation();
}

CAuthorityDialog::~CAuthorityDialog()
{}

void CAuthorityDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CAuthorityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUTHORITY_TAB, mLevelTabCtrl);
	DDX_Control(pDX, IDC_AUTHORITY_READ_01_CHECK, mPlayerLevelReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_01_CHECK, mPlayerLevelWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_02_CHECK, mPlayerCreateReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_02_CHECK, mPlayerCreateWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_03_CHECK, mPlayerDeleteReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_03_CHECK, mPlayerDeleteWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_04_CHECK, mPlayerRecoverReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_04_CHECK, mPlayerRecoverWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_05_CHECK, mPlayerUpdateReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_05_CHECK, mPlayerUpdateWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_06_CHECK, mSkillReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_06_CHECK, mSkillWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_07_CHECK, mQuestReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_07_CHECK, mQuestWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_08_CHECK, mGuildReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_08_CHECK, mGuildWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_09_CHECK, mFamilyReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_09_CHECK, mFamilyWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_10_CHECK, mPlayerLogReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_10_CHECK, mPlayerLogWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_11_CHECK, mOperatorReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_11_CHECK, mOperatorWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_12_CHECK, mAllowIpReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_12_CHECK, mAllowIpWriteButton);
	DDX_Control(pDX, IDC_AUTHORITY_READ_13_CHECK, mOperatorLogReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_13_CHECK, mOperatorLogWriteButton);	
	DDX_Control(pDX, IDC_AUTHORITY_READ_14_CHECK, mItemReadButton);
	DDX_Control(pDX, IDC_AUTHORITY_WRITE_14_CHECK, mItemWriteButton);
}

BEGIN_MESSAGE_MAP(CAuthorityDialog, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_AUTHORITY_TAB, OnTcnSelchangeAuthorityTab)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAuthorityDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IAuthorityDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {0633EBEA-6AD6-478A-8350-F715ECD30D50}
static const IID IID_IAuthorityDialog =
{ 0x633EBEA, 0x6AD6, 0x478A, { 0x83, 0x50, 0xF7, 0x15, 0xEC, 0xD3, 0xD, 0x50 } };

BEGIN_INTERFACE_MAP(CAuthorityDialog, CDialog)
	INTERFACE_PART(CAuthorityDialog, IID_IAuthorityDialog, Dispatch)
END_INTERFACE_MAP()

// CAuthorityDialog 메시지 처리기입니다.

void CAuthorityDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	switch( message->Protocol )
	{
	case MP_RM_AUTHORITY_GET_ACK:
		{
			ShowWindow( SW_SHOW );

			const MSG_RM_AUTHORITY* m = ( MSG_RM_AUTHORITY* )message;

			for( DWORD i = 0; i < m->mSize; ++ i)
			{
				const MSG_RM_AUTHORITY::Authority& authority = m->mAuthority[ i ];				
				mAuthorityMap[ authority.mPower ].push_back( authority );
			}

			OnTcnSelchangeAuthorityTab( 0, 0 );
			break;
		}
	case MP_RM_AUTHORITY_SET_ACK:
		{
			CString textUpdateSucceed;
			textUpdateSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateSucceed, _T( "" ), MB_ICONINFORMATION | MB_OK );
			break;
		}
	case MP_RM_AUTHORITY_NACK_BY_AUTH:
		{
			OnCancel();

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

BOOL CAuthorityDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 081021 LUJ, 탭 초기화
	for(
		int value = eGM_POWER_MASTER;
		eGM_POWER_MAX > value;
		++value )
	{
		const eGM_POWER power = eGM_POWER( value );

		if( eGM_POWER_MASTER == power )
		{
			continue;
		}

		mLevelTabCtrl.InsertItem(
			mLevelTabCtrl.GetItemCount(),
			mApplication.GetText( power ) );
	}

	mAuthorityMap.clear();

	// 081021 LUJ, DB에서 결과가 오기전까지 창을 표시하지 않는다
	ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

INT_PTR CAuthorityDialog::DoModal( DWORD serverIndex )
{
	mServerIndex = serverIndex;

	MSGBASE message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_AUTHORITY;
	message.Protocol	= MP_RM_AUTHORITY_GET_SYN;

	mApplication.Send(
		serverIndex,
		message,
		sizeof( message ),
		false );

	return CDialog::DoModal();
}

void CAuthorityDialog::OnOK()
{
	// 081021 LUJ, 저장 여부 문의

	const eGM_POWER power = GetRankFromTab();

	if( eGM_POWER_MAX == power )
	{
		ASSERT( 0 );
		return;
	}

	// 081021 LUJ, 라디오 박스에 설정된 내용을 담는다
	MSG_RM_AUTHORITY message;
	ZeroMemory( &message, sizeof( message ) );
	message.Category	= MP_RM_AUTHORITY;
	message.Protocol	= MP_RM_AUTHORITY_SET_SYN;

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerLevel;
		authority.mReadable		= ( 0 < mPlayerLevelReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerLevelWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerCreate;
		authority.mReadable		= ( 0 < mPlayerCreateReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerCreateWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerDelete;
		authority.mReadable		= ( 0 < mPlayerDeleteReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerDeleteWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerRecover;
		authority.mReadable		= ( 0 < mPlayerRecoverReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerRecoverWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerUpdate;
		authority.mReadable		= ( 0 < mPlayerUpdateReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerUpdateWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypePlayerLog;
		authority.mReadable		= ( 0 < mPlayerLogReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mPlayerLogWriteButton.GetCheck() );
	}
	
	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeSkill;
		authority.mReadable		= ( 0 < mSkillReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mSkillWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeQuest;
		authority.mReadable		= ( 0 < mQuestReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mQuestWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeGuild;
		authority.mReadable		= ( 0 < mGuildReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mGuildWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeFamily;
		authority.mReadable		= ( 0 < mFamilyReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mFamilyWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeOperator;
		authority.mReadable		= ( 0 < mOperatorReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mOperatorWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeAllowIp;
		authority.mReadable		= ( 0 < mAllowIpReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mAllowIpWriteButton.GetCheck() );
	}

	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeOperatorLog;
		authority.mReadable		= ( 0 < mOperatorLogReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mOperatorLogWriteButton.GetCheck() );
	}

	// 090219 LUJ, 권한 관리 항목에 [아이템] 추가
	{
		MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ message.mSize++ ];
		authority.mPower		= power;
		authority.mType			= AuthorityTypeItem;
		authority.mReadable		= ( 0 < mItemReadButton.GetCheck() );
		authority.mWriteable	= ( 0 < mItemWriteButton.GetCheck() );
	}

	// 081021 LUJ, 서버에 요청한 정보를 메모리에 저장해놓는다
	{
		AuthorityList& authorityList = mAuthorityMap[ power ];
		authorityList.clear();

		for( DWORD i = 0; i < message.mSize; ++i )
		{
			const MSG_RM_AUTHORITY::Authority& authority = message.mAuthority[ i ];
			authorityList.push_back( authority );
		}
	}	

	// 081021 LUJ, 주의사항: 모든 서버에서 DB 처리가 생기지 않도록,
	//			DB를 처리할 서버를 지정해야한다. 일단 나중에 하자
	mApplication.Send(
		mServerIndex,
		message,
		message.GetSize() );
}

void CAuthorityDialog::OnTcnSelchangeAuthorityTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( pResult )
	{
		*pResult = 0;
	}

	// 081021 LUJ, 초기화
	{
		mPlayerLevelReadButton.SetCheck( false );
		mPlayerLevelWriteButton.SetCheck( false );

		mPlayerCreateReadButton.SetCheck( false );
		mPlayerCreateWriteButton.SetCheck( false );

		mPlayerDeleteReadButton.SetCheck( false );
		mPlayerDeleteWriteButton.SetCheck( false );

		mPlayerRecoverReadButton.SetCheck( false );
		mPlayerRecoverWriteButton.SetCheck( false );

		mPlayerRecoverReadButton.SetCheck( false );
		mPlayerRecoverWriteButton.SetCheck( false );

		mPlayerUpdateReadButton.SetCheck( false );
		mPlayerUpdateWriteButton.SetCheck( false );

		mPlayerLogReadButton.SetCheck( false );
		mPlayerLogWriteButton.SetCheck( false );

		mSkillReadButton.SetCheck( false );
		mSkillWriteButton.SetCheck( false );

		mQuestReadButton.SetCheck( false );
		mQuestWriteButton.SetCheck( false );

		mGuildReadButton.SetCheck( false );
		mGuildWriteButton.SetCheck( false );

		mFamilyReadButton.SetCheck( false );
		mFamilyWriteButton.SetCheck( false );

		mOperatorReadButton.SetCheck( false );
		mOperatorWriteButton.SetCheck( false );

		mAllowIpReadButton.SetCheck( false );
		mAllowIpWriteButton.SetCheck( false );

		mOperatorLogReadButton.SetCheck( false );
		mOperatorLogWriteButton.SetCheck( false );

		mItemReadButton.SetCheck( false );
		mItemWriteButton.SetCheck( false );
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	const AuthorityMap::const_iterator map_it = mAuthorityMap.find( GetRankFromTab() );

	// 081021 LUJ, 권한 정보에 정보가 없을 경우 초기화시킨다
	if( mAuthorityMap.end() == map_it )
	{
		return;
	}
	
	const AuthorityList& authorityList = map_it->second;

	// 081021 LUJ, 체크 박스를 설정한다
	for(
		AuthorityList::const_iterator it = authorityList.begin();
		authorityList.end() != it;
		++it )
	{
		const MSG_RM_AUTHORITY::Authority& authority = *it;

		switch( authority.mType )
		{
		case AuthorityTypePlayerLevel:
			{
				mPlayerLevelReadButton.SetCheck( authority.mReadable );
				mPlayerLevelWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerCreate:
			{
				mPlayerCreateReadButton.SetCheck( authority.mReadable );
				mPlayerCreateWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerDelete:
			{
				mPlayerDeleteReadButton.SetCheck( authority.mReadable );
				mPlayerDeleteWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerRecover:
			{
				mPlayerRecoverReadButton.SetCheck( authority.mReadable );
				mPlayerRecoverWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerUpdate:
			{
				mPlayerUpdateReadButton.SetCheck( authority.mReadable );
				mPlayerUpdateWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypePlayerLog:
			{
				mPlayerLogReadButton.SetCheck( authority.mReadable );
				mPlayerLogWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeSkill:
			{
				mSkillReadButton.SetCheck( authority.mReadable );
				mSkillWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeQuest:
			{
				mQuestReadButton.SetCheck( authority.mReadable );
				mQuestWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeGuild:
			{
				mGuildReadButton.SetCheck( authority.mReadable );
				mGuildWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeFamily:
			{
				mFamilyReadButton.SetCheck( authority.mReadable );
				mFamilyWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeOperator:
			{
				mOperatorReadButton.SetCheck( authority.mReadable );
				mOperatorWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeAllowIp:
			{
				mAllowIpReadButton.SetCheck( authority.mReadable );
				mAllowIpWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeOperatorLog:
			{
				mOperatorLogReadButton.SetCheck( authority.mReadable );
				mOperatorLogWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		case AuthorityTypeItem:
			{
				mItemReadButton.SetCheck( authority.mReadable );
				mItemWriteButton.SetCheck( authority.mWriteable );
				break;
			}
		}
	}
}

eGM_POWER CAuthorityDialog::GetRankFromTab() const
{
	// 081021 LUJ, 선택한 탭으로 어떤 권한인지 알아낸다
	switch( mLevelTabCtrl.GetCurSel() )
	{
	case 0:
		{
			return eGM_POWER_MONITOR;
		}
	case 1:
		{
			return eGM_POWER_PATROLLER;
		}
	case 2:
		{
			return eGM_POWER_AUDITOR;
		}
	case 3:
		{
			return eGM_POWER_EVENTER;
		}
	case 4:
		{
			return eGM_POWER_QA;
		}
	}

	return eGM_POWER_MAX;
}