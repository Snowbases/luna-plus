#include "stdafx.h"
#include "client.h"
#include "MainFrm.h"
#include "LoginDialog.h"
#include "NewUserDialog.h"
#include "Network.h"
#include "PlayerDialog.h"
#include "ItemDialog.h"
#include "WaitDialog.h"
#include "SkillDialog.h"
#include "ItemFindDialog.h"
#include "FamilyDialog.h"
#include "QuestDialog.h"
#include "GuildDialog.h"
#include "UserDialog.h"
#include "MemoDialog.h"
#include "PermissionDialog.h"
#include "OperatorDialog.h"
#include "PasswordDialog.h"
#include "IndexInputDialog.h"
#include "LogDialog.h"
#include "FarmLogCommand.h"
#include "ItemShopLogCommand.h"
#include "ItemDropOptionLogCommand.h"
#include "ItemOptionLogCommand.h"
#include "OperatorLogCommand.h"
#include "ItemLogCommand.h"
#include "StdioFileEx.h"
#include "FishLogCommand.h"
#include "LivestockLogCommand.h"
#include "ItemGlobalLogCommand.h"
#include "ItemLogConditionDialog.h"
#include "AuthorityDialog.h"
#include "TournamentDialog.h"
#include "SiegeLogCommand.h"
#include "ChatLogCommand.h"
#include "FamilyLogCommand.h"
#include "ScriptHackLogCommand.h"
#include "NameLogCommand.h"
#include "ItemChangeDialog.h"
#include "HouseLogCommand.h"
#include "MoneyLogCommand.h"
#include "CookLogCommand.h"
#include "RecipeLogCommand.h"
#include "ConsignLogCommand.h"
#include "GameRoomPointLogCommand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CclientApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_MENU_NEWUSER_DIALOG, OnMenuNewUser)
	ON_COMMAND(ID_MENU_ITEM_SEARCH_DIALOG, OnMenuItemSearch)
	ON_COMMAND(ID_MENU_GUILD_DIALOG, OnMenuGuild)
	ON_COMMAND(ID__MENU_ITEM_LOG_DIALOG, OnMenuItemLog)
	ON_COMMAND(ID_MENU_ITEM_OPTION_LOG, OnMenuItemOptionLog)
	// 080320 LUJ, 드롭 옵션 로그 메뉴
	ON_COMMAND(ID_MENU_ITEM_DROP_OPTION_LOG, OnMenuItemDropOptionLog)
	ON_COMMAND(ID_MENU_FAMILY_DIALOG, OnMenuFamliy)
	ON_COMMAND(ID_MENU_PERMISSION_DIALOG, OnMenuPermission)
	ON_COMMAND(ID_MENU_OPERATOR_DIALOG, OnMenuOperator)
	ON_COMMAND(ID_MENU_PASSWORD_DIALOG, OnMenuPassword)
	ON_COMMAND(ID_MENU_OPERATOR_LOG, OnMenuOperatorLog)
	// 080401 LUJ, 농장 로그
	ON_COMMAND(ID_MENU_FARM_LOG_DIALOG, OnMenuFarmLog)
	// 080403 LUJ, 유료 아이템 구입 로그
	ON_COMMAND(ID_MENU_ITEM_SHOP_LOG, OnMenuItemShopLog)
	// 080523 LUJ, 낚시 로그
	ON_COMMAND(ID_MENU_FISH_LOG, OnMenuFishLog)
	// 080630 LUJ, 가축 로그
	ON_COMMAND(ID_MENU_LIVESTOCK_DIALOG, OnMenuLivestockLog)
	// 080716 LUJ, 전역 아이템 로그
	ON_COMMAND(ID_MENU_ITEM_GLOBAL_LOG, OnMenuItemGlobalLog)
	// 081021 LUJ, 권한 관리
	ON_COMMAND(ID_MENU_AUTHORITY, OnMenuAuthority)
	// 081027 LUJ, 토너먼트 정보
	ON_COMMAND(ID_MENU_TOURNAMENT, OnMenuTournament)
	// 081027 LUJ, 공성 로그
	ON_COMMAND(ID_MENU_SIEGE_LOG, OnMenuSiegeLog)
	// 081027 LUJ, 공성 로그
	ON_COMMAND(ID_MENU_CHAT_LOG, OnMenuChatLog)
	// 090122 LUJ, 스크립트 조작 로그
	ON_COMMAND(ID_MENU_SCRIPT_HACK_LOG, OnMenuScriptHackLog)
	// 090122 LUJ, 이름 변경 로그
	ON_COMMAND(ID_MENU_ITEM_RENAME_LOG, OnMenuRenameLog)
	// 090304 LUJ, 아이템 변환 시뮬레이션
	ON_COMMAND(ID_MENU_ITEM_CHANGE, OnMenuSimulationItemChange)
	// 090406 LUJ, 돈 로그
	ON_COMMAND(ID_MENU_MONEY_LOG, OnMenuMoneyLog)
	ON_COMMAND(ID_MENU_HOUSE_LOG, OnHouseLog)
	ON_COMMAND(ID_MENU_COOK_LOG, OnCookLog)
	ON_COMMAND(ID_MENU_RECIPE_LOG, OnRecipeLog)
	ON_COMMAND(ID_MENU_CONSIGN_LOG, OnConsignLog)
	ON_COMMAND(ID_MENU_GAMEROOM_LOG, OnGameRoomLog)
END_MESSAGE_MAP()


// CclientApp 생성

CclientApp::CclientApp() :
mIsWaiting( FALSE ),
mWaitDialog( 0 ),
mLoginDialog( 0 ),
mNewUserDialog( 0 ),
mPlayerDialog( 0 ),
mItemDialog( 0 ),
mSkillDialog( 0 ),
mItemFindDialog( 0 ),
mFamilyDialog( 0 ),
mQuestDialog( 0 ),
mGuildDialog( 0 ),
mUserDialog( 0 ),
mPermissionDialog( 0 ),
mOperatorDialog( 0 ),
mLogDialog( 0 ),
// 081021 LUJ, 권한 관리 창
mAuthorityDialog( 0 ),
mMemoDialog(0),
mTournamentDialog( 0 )
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.	
}


CclientApp::~CclientApp()
{
	for(
		DialogList::iterator it = mDialogList.begin();
		mDialogList.end() != it;
		++it )
	{
		CDialog* dialog = *it;

		SAFE_DELETE( dialog );
	}
}


// 유일한 CclientApp 개체입니다.

CclientApp theApp;

// CclientApp 초기화

BOOL CclientApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow( SW_SHOW );
	pFrame->UpdateWindow();
	// 080630 LUJ, 로그인이 성공하면 CLoginDialog에서 활성화시킨다.
	pFrame->EnableWindow( FALSE );
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	// 080702 LUJ, 네트워크를 초기화한다
	if( ! CNetwork::GetInstance().Initialise( pFrame->GetSafeHwnd() ) )
	{
		pFrame->SendMessage( WM_CLOSE );
		return 1;
	}

	// 080630 LUJ, 로그인창부터 만들고 리소스를 스레드로 로딩하여 실행 시간을 줄인다
	{
		mDialogList.push_back( mLoginDialog	= new CLoginDialog( *this ) );

		mLoginDialog->Create( IDD_LOGINDIALOG );
		mLoginDialog->ShowWindow( SW_SHOW );
		mLoginDialog->SetProgressValue( 0, _T( "loading..." ) );

		// 080630 LUJ, 리소스 로딩
		AfxBeginThread(
			CclientApp::LoadScript,
			( LPVOID )this );
	}

	// 리스트 컨테이너에 담아 소멸에 대한 부담을 줄이자
	{
		mDialogList.push_back( mWaitDialog			= new CWaitDialog);		
		mDialogList.push_back( mNewUserDialog		= new CNewUserDialog( *this ) );
		mDialogList.push_back( mPlayerDialog 		= new CPlayerDialog( *this ) );
		mDialogList.push_back( mItemDialog 			= new CItemDialog( *this ) );
		mDialogList.push_back( mSkillDialog 		= new CSkillDialog( *this ) );
		mDialogList.push_back( mItemFindDialog 		= new CItemFindDialog( *this ) );
		mDialogList.push_back( mFamilyDialog 		= new CFamilyDialog( *this ) );
		mDialogList.push_back( mQuestDialog 		= new CQuestDialog( *this ) );
		mDialogList.push_back( mGuildDialog 	 	= new CGuildDialog( *this ) );
		mDialogList.push_back( mUserDialog			= new CUserDialog( *this ) );
		mDialogList.push_back( mPermissionDialog 	= new CPermissionDialog( *this ) );
		mDialogList.push_back( mOperatorDialog		= new COperatorDialog( *this ) );
		mDialogList.push_back( mLogDialog			= new CLogDialog( *this ) );
		// 081021 LUJ, 권한 관리 추가
		mDialogList.push_back( mAuthorityDialog		= new CAuthorityDialog( *this ) );
		mDialogList.push_back( mMemoDialog			= new CMemoDialog );
		mDialogList.push_back( mTournamentDialog	= new CTournamentDialog( *this ) );
	}

	// 주의: 모달 속성이 아닌 윈도우만 Create()를 호출해준다
	{
		mNewUserDialog->Create( IDD_NEWUSERDIALOG );
		mPlayerDialog->Create( IDD_PLAYERDIALOG );
		mItemDialog->Create( IDD_ITEMDIALOG );
		mSkillDialog->Create( IDD_SKILLDIALOG );
		mItemFindDialog->Create( IDD_ITEMFINDDIALOG );
		mFamilyDialog->Create( IDD_FAMILYDIALOG );
		mQuestDialog->Create( IDD_QUESTDIALOG );
		mGuildDialog->Create( IDD_GUILDDIALOG );
		mUserDialog->Create( IDD_USERDIALOG );
		mPermissionDialog->Create( IDD_PERMISSIONDIALOG );
		mOperatorDialog->Create( IDD_OPERATORDIALOG );
		// 081027 LUJ, 토너먼트 창
		mTournamentDialog->Create( IDD_TOURNAMENTDIALOG );
		mMemoDialog->Create(
			IDD_MEMODIALOG);
	}
	
	return TRUE;
}


// CclientApp 메시지 처리기

int CclientApp::ExitInstance() 
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CclientApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


BOOL CclientApp::PreTranslateMessage( MSG* message) 
{
	switch( message->message )
	{
	case WM_SOCKEVENTMSG:
		{
			ISC_BaseNetwork* lpNet = CNetwork::GetInstance().GetBaseNetwork();

			if( lpNet )
			{
				lpNet->SocketEventProc();
			}

			break;
		}
	}

	return CWinApp::PreTranslateMessage( message );
}


void CclientApp::Parse( DWORD serverIndex, const MSGROOT* message )
{
	if( mWaitDialog->GetSafeHwnd() )
	{
		mWaitDialog->SendMessage( WM_CLOSE );

		mIsWaiting = FALSE;
	}

	switch( message->Category )
	{
	case MP_RM_CONNECT:
		{
			mLoginDialog->Parse( serverIndex, message );
			break;
		}
	case MP_RM_SEARCH:
		{
			mNewUserDialog->Parse( serverIndex, message );
			break;
		}
	case MP_RM_PLAYER:
		{
			mPlayerDialog->Parse( serverIndex, message );
			mPlayerDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mPlayerDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_ITEM:
		{
			mItemDialog->Parse( serverIndex, message );
			mItemDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mItemDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_ITEM_FIND:
		{
			mItemFindDialog->Parse( serverIndex, message );
			break;
		}
	case MP_RM_SKILL:
		{
			mSkillDialog->Parse( serverIndex, message );
			mSkillDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mSkillDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_FAMILY:
		{
			mFamilyDialog->Parse( serverIndex, message );
			mFamilyDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mFamilyDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_QUEST:
		{
			mQuestDialog->Parse( serverIndex, message );
			mQuestDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mQuestDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_GUILD:
		{
			mGuildDialog->Parse( serverIndex, message );
			mGuildDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mGuildDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_USER:
		{
			mUserDialog->Parse( serverIndex, message );
			mUserDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mUserDialog->ShowWindow( SW_SHOW );
			break;
		}
	case MP_RM_PERMISSION:
		{
			mPermissionDialog->ShowWindow( SW_SHOW );
			mPermissionDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mPermissionDialog->Parse( serverIndex, message );
			break;
		}
	case MP_RM_OPERATOR:
		{
			mOperatorDialog->ShowWindow( SW_SHOW );
			mOperatorDialog->SetWindowPos( &CWnd::wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE );
			mOperatorDialog->Parse( serverIndex, message );
			break;
		}
	case MP_RM_PASSWORD:
		{
			const MSGROOT* m = ( MSGROOT* )message;

			switch( m->Protocol )
			{
			case MP_RM_PASSWORD_SET_ACK:
				{
					CString textUpdateWasSucceed;
					textUpdateWasSucceed.LoadString( IDS_STRING28 );

					MessageBox( 0, textUpdateWasSucceed, _T( "" ),MB_OK | MB_ICONINFORMATION );
					break;
				}
			case MP_RM_PASSWORD_SET_NACK:
				{
					CString textUpdateWasSucceed;
					textUpdateWasSucceed.LoadString( IDS_STRING29 );

					MessageBox( 0, textUpdateWasSucceed, _T( "" ),MB_OK | MB_ICONERROR );
					break;
				}
			default:
				{
					ASSERT( 0 );
					break;
				}
			}

			break;
		}
		// 080403 LUJ, 공통 로그 창 처리
	case MP_RM_FARM_LOG:
		// 080630 LUJ, 가축 로그
	case MP_RM_LIVESTOCK_LOG:
		// 080716 LUJ, 펫 로그
	case MP_RM_PET_LOG:
	case MP_RM_ITEM_SHOP_LOG:
	case MP_RM_QUEST_LOG:
	case MP_RM_SKILL_LOG:
	case MP_RM_EXP_LOG:
	case MP_RM_STAT_LOG:
	case MP_RM_NAME_LOG:
	case MP_RM_JOB_LOG:
	case MP_RM_ITEM_DROP_OPTION_LOG:
	case MP_RM_GUILD_LOG:
	case MP_RM_GUILD_WAREHOUSE_LOG:
	case MP_RM_GUILD_SCORE_LOG:
	case MP_RM_ITEM_OPTION_LOG:
	case MP_RM_FAMILY_POINT_LOG:
	case MP_RM_OPERATOR_LOG:
	case MP_RM_ITEM_LOG:
	case MP_RM_FAMILY_CROP_LOG:
		// 080523 LUJ, 낚시 로그 추가
	case MP_RM_FISH_LOG:		
		// 080716 LUJ, 전역 아이템 로그 추가
	case MP_RM_ITEM_GLOBAL_LOG:
		// 081027 LUJ, 공성 로그
	case MP_RM_SIEGE_LOG:
		// 081205 LUJ, 채팅 로그
	case MP_RM_CHAT_LOG:
		// 081205 LUJ, 패밀리 로그
	case MP_RM_FAMILY_LOG:
		// 090122 LUJ, 스크립트 변조 로그
	case MP_RM_SCRIPT_HACK_LOG:
		// 090406 LUJ, 돈 로그
	case MP_RM_MONEY_LOG:
		// 090609 ShinJS, AutoNote 로그
	case MP_RM_AUTONOTE_LOG:
	case MP_RM_HOUSE_LOG:
	case MP_RM_COOK_LOG:
	case MP_RM_CONSIGN_LOG:
	case MP_RM_GAMEROOM_POINT_LOG:
	case MP_RM_FORBID_CHAT_LOG:
		{
			if( mLogDialog->GetSafeHwnd() )
			{
				mLogDialog->Parse( serverIndex, message );
			}

			break;
		}
		// 081021 LUJ, 권한 관리 추가
	case MP_RM_AUTHORITY:
		{
			if( mAuthorityDialog->GetSafeHwnd() )
			{
				mAuthorityDialog->Parse( serverIndex, message );
			}

			break;
		}
		// 081027 LUJ, 토너먼트 창
	case MP_RM_TOURNAMENT:
		{
			mTournamentDialog->Parse(
				serverIndex,
				message );
			break;
		}
	default:
		{
			ASSERT( 0 );
			break;
		}
	}
}


// 080328 LUJ, 필드 추가에 관계없이 파싱할 수 있도록 함
void CclientApp::LoadItemScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/ItemList.bin", "rb"))
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );
		char buffer[MAX_PATH] = {0};
		strncpy(
			buffer,
			text,
			_countof(buffer));

		const char* separator	= "\t";
		const char* token		= strtok( text, separator );

		if( ! token )
		{
			continue;
		}

		const DWORD index = atoi( token );

		if( mItemMap.end() != mItemMap.find( index ) )
		{
			Put(
				_T( "Duplicated item index found: %d" ),
				index );
			continue;
		}

		ItemScript& item = mItemMap[ index ];
		ZeroMemory( &item, sizeof( item ) );

		// 080328 LUJ, 필드 추가에 관계없이 파싱할 수 있도록 함
		{
			item.ItemIdx = index;

			// 080401 LUJ, 특수 공백 문자를 제거하고 유니코드 이름에 복사한다
			_tcsncpy(
				item.mUnicodeName,
				RemoveString( strtok( 0, separator ), "^s" ),
				sizeof( item.mUnicodeName ) / sizeof( *item.mUnicodeName ) );
			
			item.ItemTooltipIdx = atoi( strtok( 0, separator ) );
			item.Image2DNum		= atoi( strtok( 0, separator ) );	
			item.Part3DType		= atoi( strtok( 0, separator ) );
			item.Part3DModelNum = atoi( strtok( 0, separator ) );	

			item.Shop		= atoi( strtok( 0, separator ) ); // 아이템몰 여부
			item.Category	= eItemCategory(atoi( strtok( 0, separator ) ));

			item.Grade = atoi( strtok( 0, separator ) );			

			item.LimitRace		= LIMIT_RACE(atoi( strtok( 0, separator ) ));
			item.LimitGender	= atoi( strtok( 0, separator ) );		
			item.LimitLevel		= atoi( strtok( 0, separator ) );	

			item.Stack		= atoi( strtok( 0, separator ) );
			item.Trade		= atoi( strtok( 0, separator ) );
			item.Deposit	= atoi( strtok( 0, separator ) );
			item.Sell		= atoi( strtok( 0, separator ) );
			item.Decompose	= atoi( strtok( 0, separator ) );

			item.DecomposeIdx	= strtoul( strtok( 0, separator ), 0, 10 );
			item.Time			= strtoul( strtok( 0, separator ), 0, 10 );

			item.BuyPrice	= strtoul( strtok( 0, separator ), 0, 10 );			
			item.SellPrice	= strtoul( strtok( 0, separator ), 0, 10 );
			item.EquipType = eEquipType(atoi(strtok(0, separator)));
			item.EquipSlot = EWEARED_ITEM(atoi(strtok(0, separator)));
			item.Durability = strtoul( strtok( 0, separator ), 0, 10 );
			strtok( 0, separator );

			item.Repair = atoi( strtok( 0, separator ) );

			item.WeaponAnimation = eWeaponAnimationType(atoi( strtok( 0, separator ) ));
			item.WeaponType		 = eWeaponType(atoi( strtok( 0, separator ) ));

			item.PhysicAttack	= atoi( strtok( 0, separator ) );
			item.MagicAttack	= atoi( strtok( 0, separator ) );
			item.ArmorType = eArmorType(atoi(strtok(0, separator)));
			item.AccessaryType = eAccessaryType(atoi(strtok(0, separator)));
			item.PhysicDefense	= atoi( strtok( 0, separator ) );
			item.MagicDefense	= atoi( strtok( 0, separator ) );

			item.IsEnchant		= atoi( strtok( 0, separator ) );
			item.Improvement	= atoi( strtok( 0, separator ) );

			item.EnchantValue	= strtoul( strtok( 0, separator ), 0, 10 );
			item.EnchantDeterm	= strtoul( strtok( 0, separator ), 0, 10 );

			item.ImprovementStr		= atoi( strtok( 0, separator ) );			
			item.ImprovementDex		= atoi( strtok( 0, separator ) );			
			item.ImprovementVit		= atoi( strtok( 0, separator ) );		//10
			item.ImprovementInt		= atoi( strtok( 0, separator ) );
			item.ImprovementWis		= atoi( strtok( 0, separator ) );			
			item.ImprovementLife	= strtoul( strtok( 0, separator ), 0, 10 );			
			item.ImprovementMana	= atoi( strtok( 0, separator ) );			

			item.ImprovementSkill	= strtoul( strtok( 0, separator ), 0, 10 );
			item.Skill				= strtoul( strtok( 0, separator ), 0, 10 );
			item.SupplyType			= ITEM_KIND(atoi( strtok( 0, separator ) ));
			item.SupplyValue		= strtoul( strtok( 0, separator ), 0, 10 );
			item.kind				= ITEM_INFO::eKind( atoi( strtok( 0, separator ) ) );

			item.RequiredBuff	= strtoul( strtok( 0, separator ), 0, 10 );
			item.Battle			= atoi( strtok( 0, separator ) );
			item.Peace			= atoi( strtok( 0, separator ) );
			item.Move			= atoi( strtok( 0, separator ) );
			item.Stop			= atoi( strtok( 0, separator ) );
			item.Rest			= atoi( strtok( 0, separator ) );

			item.wSeal			= ITEM_SEAL_TYPE(atoi( strtok( 0, separator ) ));
			item.nTimeKind		= ITEM_TIMEKIND(atoi( strtok( 0, separator ) ));
			item.dwUseTime		= strtoul( strtok( 0, separator ), 0, 10 );
		}
	}
}


void CclientApp::LoadEnchantScript()
{
	CMHFile file;

	if( ! file.Init( "system/resource/itemEnchant.bin", "rb" ) )
	{
		return;
	}

	// 090713 ShinJS --- 인챈트시 골드소비 추가, 인챈트 레벨에 대한 보너스적용 수치 추가
	// 090917 ShinJS --- 인챈트 실패시에 대한 아이템파괴, 인챈트 레벨 감소 설정 추가
	enum Type
	{
		TypeNone,
		TypeProtection,
		TypeResult,
		TypeGoldRate,
		TypeLvBonusRate,
		TypeFailSetting,
	}
	type = TypeNone;

	while( ! file.IsEOF() )
	{
		char buffer[ MAX_PATH * 5 ] = { 0 };
		
		file.GetLine( buffer, sizeof( buffer ) );

		const int length = int( strlen( buffer ) );

		if( ! length )
		{
			continue;
		}

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = buffer[ i ];
			const char b = buffer[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				buffer[ i ] = 0;
				break;
			}
		}

		const char*		separator	= "~\n\t(), ";
		const char*		token		= strtok( buffer, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "#protection", token ) )
		{
			type = TypeProtection;
			continue;
		}
		else if( ! stricmp( "#result", token ) )
		{
			type = TypeResult;
			continue;
		}
		else if( ! stricmp( "#GoldRate", token ) )
		{
			type = TypeGoldRate;
			continue;
		}
		else if( ! stricmp( "#LevelBonusRate", token ) )
		{
			type = TypeLvBonusRate;
			continue;
		}
		else if(0 == _stricmp("#Slot", token))
		{
			type = TypeNone;
			continue;
		}
		else if(0 == _stricmp("#FailSetting", token))
		{
			type = TypeNone;
			continue;
		}
		else if(0 == _stricmp("#BreakRate", token))
		{
			type = TypeNone;
			continue;
		}
		else if(0 == _stricmp("#SuccessRate", token))
		{
			type = TypeNone;
			continue;
		}

		switch( type )
		{
		case TypeProtection:
			{
				break;
			}
		case TypeResult:
			{
				const DWORD itemIndex = atoi( token );

				ASSERT( mEnchantScriptMap.end() == mEnchantScriptMap.find( itemIndex ) );

				EnchantScript& script = mEnchantScriptMap[ itemIndex ];
				script.mItemIndex = itemIndex;

				const char* const textMinimumLevel = strtok(
					0,
					separator);
				const char* const textMaximumLevel = strtok(
					0,
					separator);
				const char* const textEnchantMaximumLevel = strtok(
					0,
					separator);

				if(0 == textMinimumLevel ||
					0 == textMaximumLevel ||
					0 == textMinimumLevel)
				{
					break;
				}

				script.mItemLevel.mMin	= atoi(textMinimumLevel);
				script.mItemLevel.mMax	= atoi(textMaximumLevel);
				script.mEnchantMaxLevel	= atoi(textEnchantMaximumLevel);

				for(
					DWORD abilitySize = 2;
					abilitySize--;
				)
					// 강화 가능한 능력 설정
				{
					token = strtok( 0, separator );

					if(0 == token)
					{
						break;
					}
					else if( ! strcmpi( "strength", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeStrength );
					}
					else if( ! strcmpi( "dexterity", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeDexterity );
					}
					else if( ! strcmpi( "vitality", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeVitality );
					}
					else if( ! strcmpi( "intelligence", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeIntelligence );
					}
					else if( ! strcmpi( "wisdom", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeWisdom );
					}
					else if( ! strcmpi( "mana", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeMana );
					}
					else if( ! strcmpi( "recovery_mana", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeManaRecovery );
					}
					else if( ! strcmpi( "life", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeLife );
					}
					else if( ! strcmpi( "recovery_life", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeLifeRecovery );
					}
					else if( ! strcmpi( "physic_defense", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypePhysicDefence );
					}
					else if( ! strcmpi( "physic_attack", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypePhysicAttack );
					}
					else if( ! strcmpi( "magic_defense", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeMagicDefence );
					}
					else if( ! strcmpi( "magic_attack", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeMagicAttack );
					}
					else if( ! strcmpi( "move_speed", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeMoveSpeed );
					}
					else if( ! strcmpi( "evade", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeEvade );
					}
					else if( ! strcmpi( "accuracy", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeAccuracy );
					}
					else if( ! strcmpi( "critical_rate", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeCriticalRate );
					}
					else if( ! strcmpi( "critical_damage", token ) )
					{
						script.mAbility.insert( EnchantScript::eTypeCriticalDamage );
					}
					else if( ! strcmpi( "none", token ) )
					{
						// to do nothing
					}
					else
					{
						mEnchantScriptMap.erase( itemIndex );
					}
				}

				// 업글 가능한 아이템 종류 세팅
				{
					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Weapon);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Shield);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Dress);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Hat);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Glove);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Shoes);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Ring1);
						script.mSlot.insert(
							eWearedItem_Ring1);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Necklace);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Earring1);
						script.mSlot.insert(
							eWearedItem_Earring2);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Belt);
					}

					if( ! strcmpi( "1", strtok( 0, separator ) ) )
					{
						script.mSlot.insert(
							eWearedItem_Band);
					}
				}

				break;
			}
		}
	}
}


const ItemScript& CclientApp::GetItemScript( DWORD itemIndex ) const
{
	static const ItemScript empty;

	ItemMap::const_iterator it = mItemMap.find( itemIndex );

	return mItemMap.end() == it ? empty : it->second;
}


const TCHAR* CclientApp::GetItemName( DWORD itemIndex ) const
{
	const ItemScript& item = GetItemScript( itemIndex );

	return item.mUnicodeName;
}


const EnchantScript* CclientApp::GetEnchantScript( DWORD itemIndex ) const
{
	EnchantScriptMap::const_iterator it = mEnchantScriptMap.find( itemIndex );

	return mEnchantScriptMap.end() == it ? 0 : &( it->second );
}

BOOL CclientApp::IsDuplicateItem( DWORD itemIndex ) const
{
	const ItemScript& item = GetItemScript( itemIndex );

	return item.Stack;
}


void CclientApp::CreateGrid( CWnd* parent, CGridCtrl* grid, UINT id, const CRect& rect, const CString* title, BOOL autoHeight ) const
{
	std::list< CString > titleList;

	if( ! title )
	{
		return;
	}

	// string must end with ""(empty string)
	for( ; ; ++title )
	{
		if( title->IsEmpty() )
		{
			break;
		}

		titleList.push_back( *title );
	}

	const DWORD rowHeight = 25;

	// make grid control
	{
		CRect gridRect( rect );

		if( autoHeight )
		{
			gridRect.bottom = gridRect.top + int( titleList.size() ) * rowHeight + 5;
		}

		grid->Create( gridRect, parent, id, AFX_WS_DEFAULT_VIEW );
		grid->SetEditable( TRUE );
		grid->SetListMode( FALSE );
		grid->EnableDragAndDrop( FALSE );
		grid->SetRowCount( int( titleList.size() ) );
		grid->SetColumnCount( 2 );
		//grid->SetFixedRowCount( 0 );
		grid->SetFixedColumnCount( 1 );
		//grid->SetItemState(1,1, grid->GetItemState(1,1) | GVIS_READONLY);
		grid->SetRowResize( FALSE );
		grid->SetColumnResize( FALSE );
	}

	// set grid with title
	{
		GV_ITEM	item	= { 0 };
		{
			item.mask			= GVIF_TEXT|GVIF_FORMAT;
			item.nFormat		= DT_LEFT|DT_WORDBREAK;
			item.row			= 0;
		}		

		for( std::list< CString >::const_iterator it = titleList.begin(); titleList.end() != it; ++it )
		{
			const CString& text = *it;

			item.strText.Format( _T( "%s" ), text );

			grid->SetItem( &item );
			grid->SetRowHeight( item.row, rowHeight );

			++item.row;
		}
	}
}


DWORD CclientApp::GetServerIndex( const TCHAR* serverName ) const
{
	return CNetwork::GetInstance().GetServerIndex( serverName );
}


const TCHAR* CclientApp::GetServerName( DWORD serverIndex ) const
{
	return CNetwork::GetInstance().GetServerName( serverIndex );
}


// 080320 LUJ, 실수값 표시위해 추가
void CclientApp::SetCellValue( float value, CGridCtrl& grid, GV_ITEM& cell, int state )
{
	cell.strText.Format( _T( "%0.3f" ), value );
	grid.SetItem( &cell );
	grid.SetItemState( cell.row, cell.col, state );
}


// 080320 LUJ, 정수값 표시위해 추가
void CclientApp::SetCellValue( int value, CGridCtrl& grid, GV_ITEM& cell, int state )
{
	cell.strText.Format( _T( "%d" ), value );
	grid.SetItem( &cell );
	grid.SetItemState( cell.row, cell.col, state );
}


void CclientApp::SetCellValue( DWORD value, CGridCtrl& grid, GV_ITEM& cell, int state )
{
	cell.strText.Format( _T( "%d" ), value );
	grid.SetItem( &cell );
	grid.SetItemState( cell.row, cell.col, state );
}


void CclientApp::SetCellValue( const TCHAR* value, CGridCtrl& grid, GV_ITEM& cell, int state )
{
	cell.strText.Format( _T( "%s" ), value );
	grid.SetItem( &cell );
	grid.SetItemState( cell.row, cell.col, state );
}


void CclientApp::SetWait( BOOL isWait )
{
	if( mIsWaiting = isWait &&
		! mWaitDialog->GetSafeHwnd() )
	{
		mWaitDialog->DoModal();
	}
	else
	{
		mWaitDialog->EndModalState();
	}	
}


void CclientApp::LoadActiveSkillScript()
{
	// normal skill
	{
		CMHFile file;

		if( ! file.Init( "System/Resource/SkillList.bin","rb" ) )
		{
			ASSERT( 0 );
			return;
		}

		while( ! file.IsEOF() )
		{
			char buffer[ MAX_PATH * 5 ] = { 0 };
			
			file.GetLine( buffer, sizeof( buffer ) );

			const char* separator = "\t ";

			const char* token1 = strtok( buffer, separator );
			const char* token2 = strtok( 0, separator );

			if( ! token1 || ! token2 )
			{
				continue;
			}

			const DWORD skillIndex = atoi( token1 );

			if( mActiveSkillScriptMap.end() != mActiveSkillScriptMap.find( skillIndex ) )
			{
				Put(
					_T( "Duplicated skill index found: %d" ),
					skillIndex );
				continue;
			}

			ActiveSkillScript& script = mActiveSkillScriptMap[ skillIndex ];

			// 080401 LUJ, 특수 공백 문자를 제거하고 유니코드 이름에 복사한다
			script.mName = RemoveString( token2, "^s" );
			script.mIndex = skillIndex;
			script.mIsBuff = FALSE;

			{
				const DWORD index = skillIndex / 100 * 100 + 1;

				++mSkillSizeMap[ index ];
			}

			// level
			strtok( 0, separator );

			// image
			strtok( 0, separator );
			// tip
			strtok( 0, separator );

			// operator effect
			strtok( 0, separator );
			// target effect
			strtok( 0, separator );
			// skill effect
			strtok( 0, separator );

			// animation time
			strtok( 0, separator );
			// casting time
			strtok( 0, separator );
			// cool time
			strtok( 0, separator );
			// delay time
			strtok( 0, separator );
			// delay type
			strtok( 0, separator );

			// battle
			strtok( 0, separator );
			// peace
			strtok( 0, separator );
			// move
			strtok( 0, separator );
			// stop
			strtok( 0, separator );
			// rest
			strtok( 0, separator );

			// required buff
			strtok( 0, separator );

			// target
			strtok( 0, separator );

			// kind
			strtok( 0, separator );
			// attribute
			strtok( 0, separator );

			// equip item #0
			strtok( 0, separator );
			// equip item #1
			strtok( 0, separator );

			// area
			strtok( 0, separator );
			// area target
			strtok( 0, separator );
			// area data #0
			strtok( 0, separator );
			// area data #1
			strtok( 0, separator );
			// area data #2
			strtok( 0, separator );

			// unit
			strtok( 0, separator );
			// unit data type
			strtok( 0, separator );
			// unit data
			strtok( 0, separator );

			// success
			strtok( 0, separator );
			// cancel
			strtok( 0, separator );

			// range
			strtok( 0, separator );
			
			// item count #0
			strtok( 0, separator );
			// item count #1
			strtok( 0, separator );
			// item #0
			strtok( 0, separator );
			// item #1			
			strtok( 0, separator );
			// money
			strtok( 0, separator );
			// mana
			strtok( 0, separator );
			// life
			strtok( 0, separator );

			// train point
			script.mPoint = atoi( strtok( 0, separator ) );
			// train money
			script.mMoney = atoi( strtok( 0, separator ) );
			
			// buff 0 .. MAX_BUFF_COUNT 
			for( int i = 0; i < MAX_BUFF_COUNT; ++i )
			{
				// buff
				const DWORD buffIndex	= atoi( strtok( 0, separator ) );

				// buff rate
				const DWORD buffRate	= atoi( strtok( 0, separator ) );

				if( buffIndex || buffRate )
				{
					script.mIsBuff = TRUE;
				}
			}
		}
	}

	// buff skill
	{
		CMHFile file;

		if( ! file.Init("System/Resource/Skill_Buff_List.bin","rb") )
		{
			//ASSERTMSG(0,"SkillList를 로드하지 못했습니다.");
			return;
		}

		char buffer[ MAX_PATH * 5 ];

		while( ! file.IsEOF() )
		{
			file.GetLine( buffer, sizeof( buffer ) );

			const char* separator = "\t ";

			const char* token1 = strtok( buffer, separator );
			const char* token2 = strtok( 0, separator );

			if( ! token1 || ! token2 )
			{
				continue;
			}

			const DWORD index = atoi( token1 );

			if( mActiveSkillScriptMap.end() != mActiveSkillScriptMap.find( index ) )
			{
				Put(
					_T( "Duplicated buff skill index found: %d" ),
					index );
				continue;
			}

			ActiveSkillScript& script = mActiveSkillScriptMap[ index ];
			script.mIndex = index;
			script.mIsBuff = TRUE;

			{
				CString text( token2 );
				CString token;
				CString name;

				int start = 0;

				do 
				{
					token = text.Tokenize( _T( "^s" ), start );

					name += token + _T( " " );

				} while( ! token.IsEmpty() );

				script.mName	= name;
			}

			{
				const DWORD index = script.mIndex / 100 * 100 + 1;

				++mSkillSizeMap[ index ];
			}
		}
	}
}


const ActiveSkillScript* CclientApp::GetActiveSkillScript( DWORD skillIndex ) const
{
	ActiveSkillScriptMap::const_iterator it = mActiveSkillScriptMap.find( skillIndex );

	return mActiveSkillScriptMap.end() == it ? 0 : &it->second;
}


DWORD CclientApp::GetSkillSize( DWORD skillIndex ) const
{
	const DWORD index = skillIndex / 100 * 100 + 1;

	SkillSizeMap::const_iterator it = mSkillSizeMap.find( index );

	return mSkillSizeMap.end() == it ? 0 : it->second;
}


void CclientApp::OnMenuNewUser()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	mNewUserDialog->ShowWindow( mNewUserDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW );
}


void CclientApp::OnMenuItemSearch()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	mItemFindDialog->ShowWindow( mItemFindDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW );
}


void CclientApp::OnMenuGuild()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	mGuildDialog->ShowWindow( mGuildDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW );
}


void CclientApp::OnMenuItemLog()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnableItemDbIndex = true;
	inputConfiguration.mIsEnableItemIndex	= true;
	inputConfiguration.mIsEnablePlayerIndex	= true;

	CIndexInputDialog dialog( *this, inputConfiguration );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CItemLogCommand::Configuration findConfiguration = { 0 };
	findConfiguration.mItemDbIndex	= dialog.GetItemDbIndex();
	findConfiguration.mItemIndex	= dialog.GetItemIndex();
	findConfiguration.mPlayerIndex	= dialog.GetPlayerIndex();

	if( mLogDialog )
	{
		CString textItem;
		textItem.LoadString( IDS_STRING189 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textName;
		textName.LoadString( IDS_STRING20 );

		CString textDbIndex;
		textDbIndex.LoadString( IDS_STRING13 );

		CString textPlayer;
		textPlayer.LoadString( IDS_STRING54 );

		CString textIndex;
		textIndex.LoadString( IDS_STRING22 );

		const TCHAR* itemName = GetItemName( findConfiguration.mItemIndex );

		CString text;
		text.Format(
			_T( "%s: %s / %s: %d / %s: %d" ),
			textName,
			*itemName ? itemName : _T( "?" ),
			textDbIndex,
			findConfiguration.mItemDbIndex,
			textPlayer + _T( " " ) + textIndex,
			findConfiguration.mPlayerIndex );

		const CString title( textItem + _T( " " ) + textLog + _T( ": " ) + text );

		mLogDialog->DoModal(
			UINT_MAX,
			CItemLogCommand( *this, title, findConfiguration ) );
	}
}


// 080523 LUJ, 낚시 로그
void CclientApp::OnMenuFishLog()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( mLogDialog )
	{
		CString textFishing;
		textFishing.LoadString( IDS_STRING379 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s" ),
			textFishing,
			textLog );

		mLogDialog->DoModal(
			UINT_MAX,
			CFishLogCommand( *this, title, 0 ) );
	}
}


// 080630 LUJ, 가축 로그
void CclientApp::OnMenuLivestockLog()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( mLogDialog )
	{
		CString textLivestock;
		textLivestock.LoadString( IDS_STRING388 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s" ),
			textLivestock,
			textLog );

		mLogDialog->DoModal(
			UINT_MAX,
			CLivestockLogCommand( *this, title, 0 ) );
	}
}


// 080716 LUJ, 아이템 전역 로그
void CclientApp::OnMenuItemGlobalLog()
{
	CItemLogConditionDialog dialog( *this );
	
	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	const CItemLogConditionDialog::InputData& inputData = dialog.GetInputData();

	CItemGlobalLogCommand::Configuration configruation;
	ZeroMemory( &configruation, sizeof( configruation ) );
	configruation.mItemDbIndex	= inputData.mItemDbIndex;
	configruation.mItemIndex	= inputData.mItemIndex;
	configruation.mPlayerIndex	= inputData.mPlayerIndex;
	configruation.mLogType			= inputData.mLogType;

	{
		CString textItem;
		textItem.LoadString( IDS_STRING189 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s" ),
			textItem,
			textLog );

		if( ! mLogDialog )
		{
			return;
		}

		mLogDialog->DoModal(
			UINT_MAX,
			CItemGlobalLogCommand( *this, title, configruation ) );
	}
}


void CclientApp::OnMenuItemOptionLog()
{
	CIndexInputDialog::Configuration configuration = { 0 };
	configuration.mIsEnableItemDbIndex = true;

	CIndexInputDialog dialog( *this, configuration );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	const DWORD itemDbIndex = dialog.GetItemDbIndex();

	if( itemDbIndex &&
		mLogDialog )
	{
		CString textItemOption;
		textItemOption.LoadString( IDS_STRING314 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textIndex;
		textIndex.Format( _T( "%d" ), itemDbIndex );

		const CString title( textItemOption + _T( " " ) + textLog + _T( ": ?(" ) + textIndex + _T( ")" ) );

		mLogDialog->DoModal(
			UINT_MAX,
			CItemOptionLogCommand( *this, title, itemDbIndex ) );
	}
}


// 080320 LUJ, 드롭 옵션 로그 메뉴
void CclientApp::OnMenuItemDropOptionLog()
{
	CIndexInputDialog::Configuration configuration = { 0 };
	configuration.mIsEnableItemDbIndex = true;

	CIndexInputDialog dialog( *this, configuration );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	const DWORD itemDbIndex = dialog.GetItemDbIndex();

	if( itemDbIndex &&
		mLogDialog )
	{
		CString textItem;
		textItem.LoadString( IDS_STRING189 );

		CString textDropOption;
		textDropOption.LoadString( IDS_STRING354 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textIndex;
		textIndex.Format( _T( "%d" ), itemDbIndex );

		const CString title( textItem + _T( " " ) + textDropOption + _T( " " ) + textLog + _T( ": ?(" ) + textIndex + _T( ")" ) );

		mLogDialog->DoModal(
			UINT_MAX,
			CItemDropOptionLogCommand( *this, title, itemDbIndex ) );
	}
}


void CclientApp::OnMenuFamliy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	mFamilyDialog->ShowWindow( mFamilyDialog->IsWindowVisible() ? SW_HIDE : SW_SHOW );
}


void CclientApp::OnMenuOperator()
{
	mOperatorDialog->Request( CNetwork::GetInstance().GetServerIndex() );
}


void CclientApp::OnMenuPassword()
{
	CPasswordDialog dialog;

	if( IDOK != dialog.DoModal() )
	{
		return;
	}

	const TCHAR* previousPassword	= dialog.GetPreviousPassword();
	const TCHAR* nextPassword		= dialog.GetNextPassword();

	if( ! previousPassword ||
		! nextPassword )
	{
		return;
	}

	{
		MSG_NAME2 message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_PASSWORD;
		message.Protocol	= MP_RM_PASSWORD_SET_SYN;
		
		StringCopySafe( message.str1, CW2AEX< MAX_PATH >( previousPassword ),	sizeof( message.str1 ) );
		StringCopySafe( message.str2, CW2AEX< MAX_PATH >( nextPassword ),		sizeof( message.str2 ) );
		
		Send(
			CNetwork::GetInstance().GetServerIndex(),
			message,
			sizeof( message ) );
	}
}


void CclientApp::OnMenuOperatorLog()
{
	if( mLogDialog )
	{
		CString textOperator;
		// 080716 LUJ, 잘못된 리소스 연결 수정
		textOperator.LoadString( IDS_STRING306 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		const CString title( textOperator + _T( " " ) + textLog );

		mLogDialog->DoModal(
			UINT_MAX,
			COperatorLogCommand( *this, title ) );
	}
}


void CclientApp::OnMenuPermission()
{
	CPermissionDialog* dialog = GetPermissionDialog();

	if( dialog )
	{
		dialog->Request( CNetwork::GetInstance().GetServerIndex() );
	}
}


// 080401 LUJ, 농장 로그
void CclientApp::OnMenuFarmLog()
{
	if( mLogDialog )
	{
		CString textFarm;
		textFarm.LoadString( IDS_STRING362 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		const CString title( textFarm + _T( " " ) + textLog );

		mLogDialog->DoModal(
			UINT_MAX,
			CFarmLogCommand( *this, title ) );
	}
}


// 080403 LUJ, 유료 아이템 구입 로그
void CclientApp::OnMenuItemShopLog()
{
	if( mLogDialog )
	{
		CString textCashItem;
		textCashItem.LoadString( IDS_STRING271 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		const CString title( textCashItem + _T( " " ) + textLog );

		mLogDialog->DoModal(
			UINT_MAX,
			CItemShopLogCommand( *this, title ) );
	}
}

// 081021 LUJ, 권한 관리
void CclientApp::OnMenuAuthority()
{
	CAuthorityDialog* dialog = GetAuthorityDialog();

	if( dialog )
	{
		dialog->DoModal( CNetwork::GetInstance().GetServerIndex() );		
	}
}

// 081027 LUJ, 토너먼트
void CclientApp::OnMenuTournament()
{
	CTournamentDialog* dialog = GetTournamentDialog();

	if( dialog )
	{
		dialog->ShowWindow( dialog->IsWindowVisible() ? SW_HIDE : SW_SHOW  );
	}
}

const QuestScript* CclientApp::GetQuestScript( DWORD mainQuestIndex, DWORD subQuestIndex ) const
{
	const DWORD index = MAKELONG( mainQuestIndex, subQuestIndex );

	QuestScriptMap::const_iterator it = mQuestScriptMap.find( index );

	return mQuestScriptMap.end() == it ? 0 : &( it->second );
}


void CclientApp::LoadQuestScript()
{
	CMHFile file;

	if( ! file.Init( "./system/Resource/QuestScript.bin", "rb") )
	{
		ASSERT( 0 );
		return;
	}

	DWORD mainIndex = 0;

	QuestScript* script = 0;

	while( ! file.IsEOF() )
	{
		char keyword[ MAX_PATH ] = { 0 };
		StringCopySafe( keyword, file.GetString(), sizeof( keyword ) );
		
		if( ! strcmpi( "$QUEST", keyword ) )
		{
			mainIndex = file.GetDword();
		}
		else if( ! strcmpi( "$SUBQUEST", keyword ) )
		{
			const DWORD subIndex	= file.GetDword();
			const DWORD	index		= MAKELONG( mainIndex, subIndex );

			if( mQuestScriptMap.end() != mQuestScriptMap.find( index ) )
			{
				Put(
					_T( "Duplicated quest index found: %d(main), %d(sub)" ),
					mainIndex, subIndex );
				return;
			}

			// 초기화
			{
				script = &( mQuestScriptMap[ index ] );

				script->mMonsterIndex		= 0;
				script->mMonsterHuntCount	= 0;
			}
		}
		else if( ! strcmpi( "@HUNT", keyword ) )
		{
			script->mMonsterIndex = file.GetDword();
		}
		else if( ! strcmpi( "*TAKEQUESTITEM", keyword ) )
		{
			const DWORD itemIndex = file.GetDword();
			
			script->mItemMap[ itemIndex ] = 0;
		}
		else if( ! strcmpi( "@COUNT", keyword ) )
		{
			file.GetDword();

			const DURTYPE count = file.GetDword();

			for( QuestScript::ItemMap::iterator it = script->mItemMap.begin();
				script->mItemMap.end() != it;
				++it )
			{
				it->second = count;
			}

			if( script->mMonsterIndex )
			{
				script->mMonsterHuntCount = count;
			}
		}
		else if( ! strcmpi( "*GIVEQUESTITEM", keyword ) )
		{
			ASSERT( script );

			const DWORD itemIndex	= file.GetDword();
			const DWORD quantity	= file.GetDword();

			script->mItemMap[ itemIndex ] = quantity;
		}
	}
}


void CclientApp::LoadQuestItemScript()
{
	CMHFile file;

	if( ! file.Init( "./system/Resource/QuestItemList.bin", "rb") )
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char line[ MAX_PATH * 5 ] = { 0 };		
		file.GetLine( line, sizeof( line ) );

		const char* separator = "\t ";

		const char* token1 = strtok( line, separator );
		const char* token2 = strtok( 0, separator );
		
		if( ! token1 || ! token2  )
		{
			continue;
		}

		const DWORD itemIndex	= atoi( token1 );
		//const char* name		= token2;

		QuestItem& item = mQuestItemMap[ itemIndex ];

		// 080401 LUJ, 특수 공백 문자를 제거하고 유니코드 이름에 복사한다
		item.mName				= RemoveString( token2, "^s" );

		item.mMainQuestIndex	= atoi( strtok( 0, separator ) );
		item.mSubQuestIndex		= atoi( strtok( 0, separator ) );
		strtok( 0, separator );
		strtok( 0, separator );

		char buffer[ MAX_PATH ] = { 0 };

		for(
			const char* token;
			token = strtok( 0, separator ); )
		{
			strcat( buffer, token );
		}

		// 080401 LUJ, 특수 공백 문자를 제거하고 유니코드 이름에 복사한다
		item.mDescription = RemoveString( buffer, "^s" );
	}
}


void CclientApp::LoadQuestStringScript()
{
	CMHFile file;

	if( ! file.Init( "./system/Resource/QuestString.bin", "rb") )
	{
		ASSERT( 0 );
		return;
	}

	QuestString* data = 0;

	while( ! file.IsEOF() )
	{
		char buffer	[ MAX_PATH * 5 ] = { 0 };
		char line	[ MAX_PATH * 5 ] = { 0 };

		file.GetLine( line, sizeof( line ) );
		strncpy( buffer, line, sizeof( buffer ) );
		
		const char* separator	= "\t ";
		const char* token		= strtok( line, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! strcmpi( token, "$SUBQUESTSTR" ) )
		{
			const DWORD mainQuestIndex	= atoi( strtok( 0, separator ) );
			const DWORD subQuestIndex	= atoi( strtok( 0, separator ) );
			const DWORD index			= MAKELONG( mainQuestIndex, subQuestIndex );

			data = &( mQuestStringMap[ index ] );

			data->mMainQuestIndex	= mainQuestIndex;
			data->mSubQuestIndex	= subQuestIndex;
		}
		else if( ! strcmpi( token, "#TITLE" ) )
		{
			ASSERT( data );

			for(
				const char* character = line;
				*character;
				++character )
			{
				const char* keyword	= "#TITLE";

				if( ! strncmp( keyword, character, sizeof( keyword ) ) )
				{
					char name[ MAX_PATH ] = { 0 };

					strncpy( name, buffer + strlen( keyword ) + 1, sizeof( name ) );
					
					data->mTitle = CA2WEX< sizeof( name ) >( name );
					break;
				}
			}
		}
		else if( ! strcmpi( token, "#DESC" ) )
		{
			ASSERT( data );

			file.GetLine( buffer, sizeof( buffer ) );
			file.GetLine( buffer, sizeof( buffer ) );

			// 문자열 앞단의 탭 문자를 제거하자
			data->mDescription = strtok( buffer, "\t" );
		}
	}
}


const CclientApp::QuestStringMap& CclientApp::GetQuestStringMap() const
{
	return mQuestStringMap;
}


const TCHAR* CclientApp::GetQuestTitle( DWORD mainQuestIndex, DWORD subQuestIndex ) const
{
	const DWORD index = MAKELONG( mainQuestIndex, subQuestIndex );

	QuestStringMap::const_iterator it = mQuestStringMap.find( index );

	return mQuestStringMap.end() == it ? _T( "?" ) : it->second.mTitle;
}


const TCHAR* CclientApp::GetQuestDescription( DWORD mainQuestIndex, DWORD subQuestIndex ) const
{
	const DWORD index = MAKELONG( mainQuestIndex, subQuestIndex );

	QuestStringMap::const_iterator it = mQuestStringMap.find( index );

	return mQuestStringMap.end() == it ? _T( "?" ) : it->second.mDescription;
}


const TCHAR* CclientApp::GetQuestItemName( DWORD questItemIndex ) const
{
	QuestItemMap::const_iterator it = mQuestItemMap.find( questItemIndex );

	if( mQuestItemMap.end() == it )
	{
		return _T( "?" );
	}

	const QuestItem& item = it->second;

	return item.mName;
}


const TCHAR* CclientApp::GetQuestItemDescription( DWORD questItemIndex ) const
{
	QuestItemMap::const_iterator it = mQuestItemMap.find( questItemIndex );

	if( mQuestItemMap.end() == it )
	{
		return _T( "?" );
	}

	const QuestItem& item = it->second;

	return item.mDescription;
}


const CclientApp::QuestItemMap& CclientApp::GetQuestItemMap() const
{
	return mQuestItemMap;
}


DWORD CclientApp::GetMainQuestIndex( DWORD questItemIndex ) const
{
	QuestItemMap::const_iterator it = mQuestItemMap.find( questItemIndex );

	if( mQuestItemMap.end() == it )
	{
		return 0;
	}

	const QuestItem& item = it->second;

	return item.mMainQuestIndex;
}


DWORD CclientApp::GetSubQuestIndex( DWORD questItemIndex ) const
{
	QuestItemMap::const_iterator it = mQuestItemMap.find( questItemIndex );

	if( mQuestItemMap.end() == it )
	{
		return 0;
	}

	const QuestItem& item = it->second;

	return item.mSubQuestIndex;
}

void CclientApp::SetItemList(CListCtrl& listCtrl, LPCTSTR keyword) const
{
	listCtrl.DeleteAllItems();

	for(ItemMap::const_iterator iterator = mItemMap.begin();
		mItemMap.end() != iterator;
		++iterator)
	{
		const ItemScript& itemScript = iterator->second;
		const std::wstring name(itemScript.mUnicodeName);

		if(std::wstring::npos == name.find(keyword))
		{
			continue;
		}

		const int row = listCtrl.GetItemCount();
		int column = -1;

		TCHAR text[MAX_PATH] = {0};
		listCtrl.InsertItem(
			row,
			_itot(itemScript.ItemIdx, text, 10),
			++column);
		listCtrl.SetItemText(
			row,
			++column,
			itemScript.mUnicodeName);
		listCtrl.SetItemText(
			row,
			++column,
			_itot(itemScript.LimitLevel, text, 10));
	}
}

void CclientApp::SetItemList( CListCtrl& listCtrl ) const
{
	listCtrl.DeleteAllItems();

	CString text;
	int index = 0;

	for( ItemMap::const_iterator it = mItemMap.begin(); mItemMap.end() != it; ++it, ++index )
	{
		const ItemScript& item = it->second;
		
		text.Format( _T( "%d" ), item.ItemIdx );
		listCtrl.InsertItem( index, text, 0 );
		listCtrl.SetItemText( index, 1, item.mUnicodeName );

		text.Format( _T( "%d" ), item.LimitLevel );
		listCtrl.SetItemText( index, 2, text );
	}
}


void  CclientApp::SetItemList( CListCtrl& listCtrl, eItemCategory category ) const
{
	listCtrl.DeleteAllItems();

	CString text;
	int index = 0;

	for( ItemMap::const_iterator it = mItemMap.begin(); mItemMap.end() != it; ++it )
	{
		const ItemScript& item = it->second;

		if( item.Category != category )
		{
			continue;
		}

		const DWORD row		= listCtrl.GetItemCount();
		int			step	= 0;

		text.Format( _T( "%d" ), item.ItemIdx );
		listCtrl.InsertItem( row, text, 0 );
		listCtrl.SetItemText( row, ++step, item.mUnicodeName );

		text.Format( _T( "%d" ), item.LimitLevel );
		listCtrl.SetItemText( row, ++step, text );
	}
}


void CclientApp::SetServerComboBox( CComboBox& combo ) const
{
	const CNetwork::ServerMap&		serverMap	= CNetwork::GetInstance().GetServerMap();
	const CNetwork::ConnectionMap&	connection	= CNetwork::GetInstance().GetConnectionMap();

	combo.ResetContent();

	for( CNetwork::ConnectionMap::const_iterator it = connection.begin(); connection.end() != it; ++it )
	{
		const DWORD							serverIndex = it->first;
		CNetwork::ServerMap::const_iterator find_it		= serverMap.find( serverIndex );

		if( serverMap.end() == find_it )
		{
			ASSERT( 0 && "It's no defined server" );
			continue;
		}

		const CNetwork::Server& server = find_it->second;

		combo.AddString( server.mName );
	}

	if( connection.size() )
	{
		combo.SetCurSel( 0 );
	}
}

// 081021 LUJ, 잠금 변수가 거짓일 경우, 대기 창이 표시되지 않도록 함
void CclientApp::Send( const TCHAR* serverName, const MSGROOT& message, size_t size, bool isLock )
{
	CNetwork& network = CNetwork::GetInstance();

	const DWORD serverIndex = network.GetServerIndex( serverName );

	Send( serverIndex, message, size, isLock );
}

// 081021 LUJ, 잠금 변수가 거짓일 경우, 대기 창이 표시되지 않도록 함
void CclientApp::Send( DWORD serverIndex, const MSGROOT& message, size_t size, bool isLock )
{
	CNetwork& network = CNetwork::GetInstance();

	if( ! network.GetServerName( serverIndex ) )
	{
		CString textAnyServerNotFoundToRequest;
		textAnyServerNotFoundToRequest.LoadString( IDS_STRING227 );

		MessageBox( 0, textAnyServerNotFoundToRequest, _T( "" ),MB_ICONERROR | MB_OK );
		return;
	}

	network.Send( serverIndex, message, size );

	if( isLock )
	{
		SetWait( TRUE );
	}
}

// 081021 LUJ, 잠금 변수가 거짓일 경우, 대기 창이 표시되지 않도록 함
void CclientApp::SendAll( const MSGROOT& message, size_t size, bool isLock )
{
	CNetwork::GetInstance().SendAll( message, size );

	if( isLock )
	{
		SetWait( TRUE );
	}
}


void CclientApp::ConvertWildCardForDb( char* text )
{
	// DB는 와일드카드 문자가 윈도우 운영체제와 다르므로 바꿔주자

	for( char* character = text; *character; ++character )
	{
		char& c = *character;

		if( '?' == c )
		{
			c = '_';
		}
		else if( '*' == c )
		{
			c = '%';
		}
	}
}


void CclientApp::LoadInterfaceMessage()
{
	CMHFile file;

	if( ! file.Init( "data/interface/windows/interfaceMsg.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	char line[ MAX_PATH * 5 ];

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

		const char* separator	= "\t ";
		const char* token		= strtok( line, separator );

		if( ! token )
		{
			continue;
		}

		const DWORD index = atoi( token );

		if( mInterfaceMessage.end() != mInterfaceMessage.find( index ) )
		{
			ASSERT( 0 && "Duplicated index is found" );
			continue;
		}

		CString& text = mInterfaceMessage[ index ];

        while( token = strtok( 0, separator ) )
		{
			text += token;
			text += ' ';
		}
	}
}


LPCTSTR CclientApp::GetInterfaceMessage( DWORD index ) const
{
	InterfaceMessage::const_iterator it = mInterfaceMessage.find( index );

	return  mInterfaceMessage.end() == it ? _T( "" ) : it->second;
}


const TCHAR* CclientApp::GetJobName( DWORD code ) const
{
	DWORD index = 0;

	// [Client]Luna의 CProfileDlg::UpdateProfile()에서 발췌
	switch( code )
	{
	case 1111:	index = 364;	break;
	case 1121:	index = 365;	break;
	case 1122:	index = 366;	break;
	case 1131:	index = 367;	break;
	case 1132:	index = 368;	break;
	case 1133:	index = 369;	break;
	case 1141:	index = 370;	break;
	case 1142:	index = 371;	break;
	case 1143:	index = 372;	break;
	case 1144:	index = 373;	break;
	case 1151:	index = 374;	break;
	case 1152:	index = 375;	break;
	case 1153:	index = 376;	break;
	case 1154:	index = 377;	break;
	case 1155:	index = 378;	break;
	case 1156:	index = 379;	break;
	case 1161:	index = 380;	break;
	case 1162:	index = 381;	break;
	case 1163:	index = 382;	break;

	case 1211:	index = 364;	break;
	case 1221:	index = 365;	break;
	case 1222:	index = 366;	break;
	case 1231:	index = 367;	break;
	case 1232:	index = 368;	break;
	case 1233:	index = 369;	break;
	case 1241:	index = 370;	break;
	case 1242:	index = 371;	break;
	case 1243:	index = 372;	break;
	case 1244:	index = 373;	break;
	case 1251:	index = 374;	break;
	case 1252:	index = 375;	break;
	case 1253:	index = 376;	break;
	case 1254:	index = 377;	break;
	case 1255:	index = 378;	break;
	case 1256:	index = 379;	break;
	case 1261:	index = 380;	break;
	case 1262:	index = 381;	break;
	case 1263:	index = 382;	break;

	case 2111:	index = 383;	break;
	case 2121:	index = 384;	break;
	case 2122:	index = 385;	break;
	case 2131:	index = 386;	break;
	case 2132:	index = 387;	break;
	case 2133:	index = 388;	break;
	case 2141:	index = 389;	break;
	case 2142:	index = 390;	break;
	case 2143:	index = 391;	break;
	case 2144:	index = 392;	break;
	case 2151:	index = 393;	break;
	case 2152:	index = 394;	break;
	case 2153:	index = 395;	break;
	case 2154:	index = 396;	break;
	case 2155:  index = 1004;	break;

	case 2161:	index = 397;	break;
	case 2162:	index = 398;	break;
	case 2163:	index = 399;	break;

	case 2211:	index = 383;	break;
	case 2221:	index = 384;	break;
	case 2222:	index = 385;	break;
	case 2231:	index = 386;	break;
	case 2232:	index = 387;	break;
	case 2233:	index = 388;	break;
	case 2241:	index = 389;	break;
	case 2242:	index = 390;	break;
	case 2243:	index = 391;	break;
	case 2244:	index = 392;	break;
	case 2251:	index = 393;	break;
	case 2252:	index = 394;	break;
	case 2253:	index = 395;	break;
	case 2254:	index = 396;	break;
	case 2255:	index = 1004;	break;
	case 2261:	index = 397;	break;
	case 2262:	index = 398;	break;
	case 2263:	index = 399;	break;

	case 3111:	index = 400;	break;
	case 3121:	index = 401;	break;
	case 3122:	index = 402;	break;
	case 3131:	index = 403;	break;
	case 3132:	index = 404;	break;
	case 3133:	index = 405;	break;
	case 3141:	index = 406;	break;
	case 3142:	index = 407;	break;
	case 3143:	index = 408;	break;
	case 3144:	index = 409;	break;
	case 3151:	index = 410;	break;
	case 3152:	index = 411;	break;
	case 3153:	index = 412;	break;
	case 3154:	index = 413;	break;
	case 3155:	index = 414;	break;
	case 3161:	index = 415;	break;
	case 3162:	index = 416;	break;
	case 3163:	index = 417;	break;

	case 3211:	index = 400;	break;
	case 3221:	index = 401;	break;
	case 3222:	index = 402;	break;
	case 3231:	index = 403;	break;
	case 3232:	index = 404;	break;
	case 3233:	index = 405;	break;
	case 3241:	index = 406;	break;
	case 3242:	index = 407;	break;
	case 3243:	index = 408;	break;
	case 3244:	index = 409;	break;
	case 3251:	index = 410;	break;
	case 3252:	index = 411;	break;
	case 3253:	index = 412;	break;
	case 3254:	index = 413;	break;
	case 3255:	index = 414;	break;
	case 3261:	index = 415;	break;
	case 3262:	index = 416;	break;
	case 3263:	index = 417;	break;
	case 4311:	index = 1132;	break;
	case 4321:	index = 1133;	break;
	case 4331:	index = 1134;	break;
	case 4341:	index = 1135;	break;
	case 4351:	index = 1136;	break;
	}

	return GetInterfaceMessage( index );
}


void CclientApp::LoadLicenseScript()
{
	char buffer[ MAX_PATH * 5 ];

	CMHFile file;

	if( ! file.Init( "system/resource/ResidentRegist.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	// 성향
	while( ! file.IsEOF() )
	{
		char keyword[ MAX_PATH ] = { 0 };
		StringCopySafe( keyword, file.GetString(), sizeof( keyword ) );
		
		if( ! strcmpi( "END_KIND", keyword ) )
		{
			break;
		}
		else if( ! strcmpi( "//", keyword ) )
		{
			file.GetLine( buffer, sizeof( buffer ) );
			continue;
		}

		const DWORD index = atoi( keyword );
		ASSERT( mLicenseFavoriteMap.end() == mLicenseFavoriteMap.find( index ) );

		mLicenseFavoriteMap.insert( std::make_pair( index, file.GetString() ) );
	}

	// 지역
	while( ! file.IsEOF() )
	{
		char keyword[ MAX_PATH ] = { 0 };
		StringCopySafe( keyword, file.GetString(), sizeof( keyword ) );

		if( ! strcmpi( "END_KIND", keyword ) )
		{
			break;
		}
		else if( ! strcmpi( "//", keyword ) )
		{
			file.GetLine( buffer, sizeof( buffer ) );
			continue;
		}

		const DWORD index = atoi( keyword );
		ASSERT( mLicenseAreaMap.end() == mLicenseAreaMap.find( index ) );

		mLicenseAreaMap.insert( std::make_pair( index, file.GetString() ) );
	}

	// 나이
	while( ! file.IsEOF() )
	{
		char keyword[ MAX_PATH ] = { 0 };
		StringCopySafe( keyword, file.GetString(), sizeof( keyword ) );

		if( ! strcmpi( "END_KIND", keyword ) )
		{
			break;
		}
		else if( ! strcmpi( "//", keyword ) )
		{
			file.GetLine( buffer, sizeof( buffer ) );
			continue;
		}

		const DWORD index = atoi( keyword );
		ASSERT( mLicenseAgeMap.end() == mLicenseAgeMap.find( index ) );

		CString text;
		text.Format( _T( "%d~%d" ), atoi( file.GetString() ), atoi( file.GetString() ) );

		mLicenseAgeMap.insert( std::make_pair( index, text ) );
	}
}


const CclientApp::LicenseFavoriteMap& CclientApp::GetLicenseFavoriteMap() const
{
	return mLicenseFavoriteMap;
}


const CclientApp::LicenseAreaMap& CclientApp::GetLicenseAreaMap() const
{
	return mLicenseAreaMap;
}


const CclientApp::LicenseAgeMap& CclientApp::GetLicenseAgeMap() const
{
	return mLicenseAgeMap;
}


const CclientApp::SkillSizeMap& CclientApp::GetSkillSizeMap() const
{
	return mSkillSizeMap;
}


void CclientApp::LoadTip()
{
	CMHFile file;

	if( ! file.Init( "data/interface/windows/ToolTipMsg.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	char buffer[ MAX_PATH * 5 ];

	while( ! file.IsEOF() )
	{
		char keyword[ MAX_PATH ] = { 0 };
		StringCopySafe( keyword, file.GetString(), sizeof( keyword ) );

		if( ! _strcmpi( "#Msg", keyword ) )
		{
			const DWORD index = file.GetDword();

			file.GetLine( buffer, sizeof( buffer ) );
			file.GetLine( buffer, sizeof( buffer ) );
			file.GetLine( buffer, sizeof( buffer ) );

			mSkillTipMap.insert( std::make_pair( index, buffer ) );

			file.GetLine( buffer, sizeof( buffer ) );
		}
	}
}

// 090204 LUJ, 오해의 소지가 있는 변수명을 쓰임에 맞게 변경
const TCHAR* CclientApp::GetTip( DWORD tipIndex ) const
{
	SkillTipMap::const_iterator it = mSkillTipMap.find( tipIndex );

	return mSkillTipMap.end() == it ? _T( "" ) : it->second;
}


void CclientApp::SetDateCombo( CComboBox& yearCombo, CComboBox& monthCombo, CComboBox& dayCombo, BOOL isToday )
{
	CRect rect;

	{
		yearCombo.ResetContent();
		yearCombo.GetWindowRect( rect );
		yearCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

		monthCombo.ResetContent();
		monthCombo.GetWindowRect( rect );
		monthCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

		dayCombo.ResetContent();
		dayCombo.GetWindowRect( rect );
		dayCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
	}

	// 값 세팅
	{
		CString text;

		for( DWORD year = 7; 20 >= year; ++year )
		{
			text.Format( _T( "%02d" ), year );

			yearCombo.AddString( text );
		}

		for( DWORD month = 1; 12 >= month; ++month )
		{
			text.Format( _T( "%d" ), month );

			monthCombo.AddString( text );
		}

		for( DWORD day = 1; 31 >= day; ++day )
		{
			text.Format( _T( "%d" ), day );

			dayCombo.AddString( text );
		}
	}

	if( isToday )
	{
		const CTime time = CTime::GetCurrentTime();

		yearCombo.SetCurSel( time.GetYear() - 2007 );
		monthCombo.SetCurSel( time.GetMonth() - 1 );
		dayCombo.SetCurSel( time.GetDay() - 1 );
	}
	else
	{
		yearCombo.SetCurSel( 0 );
		monthCombo.SetCurSel( 5 );
		dayCombo.SetCurSel( 14 );
	}
}

// 081020 LUJ, 엑셀 저장 코드를 MFC가 아닌 API 사용으로 변경. 참조: http://support.microsoft.com/kb/216686/
void CclientApp::SaveToExcel( const TCHAR* fileName, const TCHAR* tableName, const CListCtrl& listCtrl )
{
	CFileDialog fileDialog(
		FALSE,
		_T( "xls" ),
		fileName,
		OFN_OVERWRITEPROMPT,
		_T( "*.xls" ) );

	if( fileDialog.DoModal() != IDOK )
	{
		return;
	}
	// 081017 LUJ, 파일이 이미 존재할 경우 삭제해야 한다
	else
	{
		CFile file;

		if( file.Open( fileDialog.GetPathName(), CFile::modeRead ) )
		{
			file.Close();

			if( ! DeleteFile( fileDialog.GetPathName() ) )
			{
				MessageBox(
					0,
					_T( "It can't delete file. Close file please" ),
					_T( "Error" ),
					MB_ICONERROR );
				return;
			}
		}
	}

	// 081017 LUJ, 엑셀 기능 호출을 단순화하는 내부 클래스
	class
	{
	public:
		HRESULT operator()( int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs... )
		{
			va_list marker;
			va_start(marker, cArgs);

			if(!pDisp) {
				MessageBox(NULL, _T( "NULL IDispatch passed to AutoWrap()" ), _T( "Error" ), 0x10010);
				return S_FALSE;
			}

			// Variables used...
			DISPPARAMS dp = { NULL, NULL, 0, 0 };
			DISPID dispidNamed = DISPID_PROPERTYPUT;
			DISPID dispID;
			HRESULT hr;
			TCHAR buf[ MAX_PATH ]		= { 0 };
			TCHAR szName[ MAX_PATH ]	= { 0 };

			// Get DISPID for name passed...
			hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
			if(FAILED(hr))
			{
				return hr;
			}

			// Allocate memory for arguments...
			VARIANT *pArgs = new VARIANT[cArgs+1];
			// Extract arguments...
			for(int i=0; i<cArgs; i++)
			{
				pArgs[i] = va_arg(marker, VARIANT);
			}

			// Build DISPPARAMS
			dp.cArgs = cArgs;
			dp.rgvarg = pArgs;

			// Handle special-case for property-puts!
			if(autoType & DISPATCH_PROPERTYPUT) {
				dp.cNamedArgs = 1;
				dp.rgdispidNamedArgs = &dispidNamed;
			}

			// Make the call!
			hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
			if(FAILED(hr))
			{
				va_end(marker);
				delete [] pArgs;
				return hr;
			}

			// End variable-argument section...
			va_end(marker);
			delete [] pArgs;

			return hr;
		}
	}
	AutoWrap;

	// Initialize COM for this thread...
	CoInitialize(NULL);

	// Get CLSID for our server...
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);

	if(FAILED(hr))
	{
		::MessageBox(NULL, _T( "Excel must be installed" ), _T( "Error" ), 0x10010);
		return;
	}

	// Start server and get IDispatch...
	IDispatch *pXlApp;
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pXlApp);
	if(FAILED(hr))
	{
		::MessageBox(NULL, _T( "Excel not registered properly" ), _T( "Error" ), 0x10010);
		return;
	}

	// Make it visible (i.e. app.visible = 1)
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlApp, L"Visible", 0, x);
	}

	// Get Workbooks collection
	IDispatch *pXlBooks;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"Workbooks", 0);
		pXlBooks = result.pdispVal;
	}

	// Call Workbooks.Add() to get a new workbook...
	IDispatch *pXlBook;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlBooks, L"Add", 0);
		pXlBook = result.pdispVal;
	}

	typedef std::pair< CString, CString > Field;
	typedef std::list< Field > FieldList;

	const CHeaderCtrl* pHeaderCtrl = ( CHeaderCtrl* )listCtrl.GetDlgItem( 0 );

	if( ! pHeaderCtrl )
	{
		return;
	}

	const int columnSize = pHeaderCtrl->GetItemCount();

	// 헤더를 조립한다
	typedef std::list< CString >	HeaderList;
	HeaderList						headerList;
	{
		TCHAR buffer[ MAX_PATH * 5 ] = { 0 };

		HDITEM item;
		{
			item.mask		= HDI_TEXT;
			item.pszText	= buffer;
			item.cchTextMax = sizeof( buffer );
		}

		for( int column = 0; column < columnSize; ++column )
		{
			pHeaderCtrl->GetItem( column, &item );

			// 공백 제거
			{
				CString text( item.pszText );
				CString token;
				CString name;

				int start = 0;

				do 
				{
					token = text.Tokenize( _T( " " ), start );

					name += token;

				} while( ! token.IsEmpty() );

				name.TrimRight();

				headerList.push_back( name );
			}
		}
	}

	// Create a 15x15 safearray of variants...
	VARIANT variantArray = { 0 };
	variantArray.vt = VT_ARRAY | VT_VARIANT;
	{
		SAFEARRAYBOUND sab[2];
		sab[0].lLbound		= 1;
		sab[0].cElements	= listCtrl.GetItemCount() + 1;
		sab[1].lLbound		= 1;
		sab[1].cElements	= columnSize;
		variantArray.parray = SafeArrayCreate( VT_VARIANT, 2, sab );
	}

	// 081020 LUJ, 헤더를 메모리에 복사한다
	{
		int column = 0;

		for(
			HeaderList::const_iterator it = headerList.begin();
			headerList.end() != it;
			++it )
		{
			VARIANT variant = { 0 };
			variant.vt		= VT_BSTR;
			variant.bstrVal	= ::SysAllocString( *it );
			
			long indices[] = { 1, ++column };
			SafeArrayPutElement( variantArray.parray, indices, ( void* )&variant );
		}
	}

	// 081020 LUJ, 내용을 메모리에 복사한다
	for( int row = 0; listCtrl.GetItemCount() > row; ++row )
	{
		CString statement;

		for( int column = 0; column < columnSize; ++column )
		{
			VARIANT variant = { 0 };
			variant.vt		= VT_BSTR;
			variant.bstrVal	= ::SysAllocString( listCtrl.GetItemText( row, column ) );

			long indices[] = { row + 2, column + 1 };
			SafeArrayPutElement( variantArray.parray, indices, ( void* )&variant );
		}
	}
	
	// Get ActiveSheet object
	IDispatch *pXlSheet = 0;
	{
		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlApp, L"ActiveSheet", 0);
		pXlSheet = result.pdispVal;
	}

	// Get Range object for the Range A1:O15...
	IDispatch *pXlRange = 0;
	{
		CString columnText;
		{
			int			size			= columnSize - 1;
			const int	alphabetSize	= abs( 'z' - 'a' ) + 1;

			// 081017 LUJ, 엑셀의 주소는 A ... Z -> AA ... ZZ -> AAA ... ZZZ 이런 식으로 증가된다
			//			따라서 끝 주소를 만드는데는 조금 복잡한 식이 필요하다
			do
			{
				const char text = 'A' + size;
				columnText += CString( text );

				size -= alphabetSize;
			}
			while( alphabetSize < size );
		}

		// 081017 LUJ, 셀 좌표 생성. 남게 지정하면 빈칸에 N/A값이 셀에 표시된다. 모자라면 내용이 적게 표시된다
		CString coordinate;
		coordinate.Format(
			_T( "A1:%s%u" ),
			columnText,
			listCtrl.GetItemCount() + 1 );

		VARIANT parm;
		parm.vt = VT_BSTR;
		parm.bstrVal = ::SysAllocString( coordinate );

		VARIANT result;
		VariantInit(&result);
		AutoWrap(DISPATCH_PROPERTYGET, &result, pXlSheet, L"Range", 1, parm);
		VariantClear(&parm);

		pXlRange = result.pdispVal;
	}

	// Set range with our safearray...
	AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, variantArray);

	{
		VARIANT result;
		VariantInit(&result);
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 0;
		VARIANT fname;
		fname.vt = VT_BSTR;
		fname.bstrVal= ::SysAllocString( fileDialog.GetPathName() );
		AutoWrap(DISPATCH_METHOD, &result, pXlBook, L"SaveAs", 1, fname );
	}

	// Set .Saved property of workbook to TRUE so we aren't prompted
	// to save when we tell Excel to quit...
	{
		VARIANT x;
		x.vt = VT_I4;
		x.lVal = 1;
		AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlBook, L"Saved", 1, x);
	}

	// Tell Excel to quit (i.e. App.Quit)
	AutoWrap(DISPATCH_METHOD, NULL, pXlApp, L"Quit", 0);

	// Release references...
	pXlRange->Release();
	pXlSheet->Release();
	pXlBook->Release();
	pXlBooks->Release();
	pXlApp->Release();
	VariantClear(&variantArray);

	// Uninitialize COM for this thread...
	CoUninitialize();

	{
		CString textLogSavedToWhere;
		textLogSavedToWhere.LoadString( IDS_STRING229 );

		CString text;
		text.Format( textLogSavedToWhere, fileDialog.GetPathName() );

		MessageBox(
			0,
			text,
			_T( "" ),
			MB_ICONINFORMATION | MB_OK );
	}
}


// 080401 LUJ, 인수를 상수로 변경
// 081020 LUJ, 전체 항목을 복사하는 플래그 추가
void CclientApp::SaveToClipBoard( const CListCtrl& listCtrl, BOOL isWholeCopy )
{
	// 080401 LUJ,	const_cast를 이용하여 상수 속성을 비상수 속성으로 바꾼다.
	//				값을 바꾸지 않지만 변경할 수 없는 멤버가 비상수 속성을 쓸 경우 사용한다
	const CHeaderCtrl* pHeaderCtrl = const_cast< CListCtrl& >( listCtrl ).GetHeaderCtrl();

	if( ! pHeaderCtrl )
	{
		return;
	}

	const int columnSize = pHeaderCtrl->GetItemCount();

	CString text;

	// 제목 복사
	{
		HDITEM item;
		TCHAR buffer[ MAX_PATH * 5 ] = { 0 };

		item.mask		= HDI_TEXT;
		item.pszText	= buffer;
		item.cchTextMax = sizeof( buffer );

		for( int column = 0; column < columnSize; ++column )
		{			
			pHeaderCtrl->GetItem( column, &item );

			text += CString( item.pszText ) + _T( "\t" );
		}		

		text += _T( "\r\n" );
	}
	
	// 내용 복사
	if( isWholeCopy )
	{
		for( int row = 0; listCtrl.GetItemCount() > row; ++row )
		{
			for( int column = 0; column < columnSize; ++column )
			{
				text += listCtrl.GetItemText( row, column ) + _T( "\t" );
			}

			text += _T( "\r\n" );
		}
	}
	else
	{
		POSITION position = listCtrl.GetFirstSelectedItemPosition();

		while( position )
		{
			const int row = listCtrl.GetNextSelectedItem( position );

			for( int column = 0; column < columnSize; ++column )
			{
				text += listCtrl.GetItemText( row, column ) + _T( "\t" );
			}

			text += _T( "\r\n" );
		}
	}
	
	// 클립보드에 저장
	// http://www.developerfusion.co.uk/forums/p/43117/143995/
	if( OpenClipboard( AfxGetApp()->GetMainWnd()->GetSafeHwnd() ) )
	{		
		EmptyClipboard();

		HGLOBAL global = GlobalAlloc( GMEM_MOVEABLE, sizeof( TCHAR ) * ( text.GetLength() + 1 ) );

		TCHAR* memory = ( TCHAR* )GlobalLock( global );

		_tcscpy( memory, text );

		GlobalUnlock( global );

		SetClipboardData( 
#ifdef _UNICODE
			CF_UNICODETEXT,
#else
			CF_TEXT,
#endif
			global );

		CloseClipboard();
	}
}

void CclientApp::SaveToCSV( const TCHAR* fileName, const CListCtrl& listCtrl )
{
	CFileDialog dialog( FALSE, _T( "csv" ), fileName, OFN_OVERWRITEPROMPT, _T( "*.csv" ) );

	if( dialog.DoModal() != IDOK )
	{
		return;
	}

	typedef std::pair< CString, CString > Field;
	typedef std::list< Field > FieldList;

	const CHeaderCtrl* pHeaderCtrl = ( CHeaderCtrl* )listCtrl.GetDlgItem( 0 );

	if( ! pHeaderCtrl )
	{
		return;
	}

	const int columnSize = pHeaderCtrl->GetItemCount();

	// 헤더를 조립한다
	typedef std::list< CString >	HeaderList;
	HeaderList						headerList;
	{
		TCHAR buffer[ MAX_PATH * 5 ] = { 0 };

		HDITEM item;
		{
			item.mask		= HDI_TEXT;
			item.pszText	= buffer;
			item.cchTextMax = sizeof( buffer );
		}

		for( int column = 0; column < columnSize; ++column )
		{
			pHeaderCtrl->GetItem( column, &item );

			// 공백 제거
			{
				CString text( item.pszText );
				CString token;
				CString name;

				int start = 0;

				do
				{
					token = text.Tokenize( _T( " " ), start );

					name += token;

				} while( ! token.IsEmpty() );

				name.TrimRight();

				headerList.push_back( name );
			}
		}
	}

	CStdioFile file;
	if( ! file.Open(
			CString( fileName ) + _T( ".csv" ),
			CFile::modeWrite | CFile::modeCreate | CFile::typeText ) )
	{
		return;
	}

	// 081020 LUJ, 헤더를 저장한다
	{
		int column = 0;

		for(
			HeaderList::const_iterator it = headerList.begin();
			headerList.end() != it;
			++it )
		{
			const CString& textHeader = *it;

			file.WriteString( textHeader );

			// 필드 끝 처리
			if( columnSize != ++column )
			{
				file.WriteString( _T( ", " ) );
			}
		}

		file.WriteString( _T( "\n" ) );
	}
	
	// 081020 LUJ, 삽입 구문의 값 부분을 조립하고 삽입한다.
	for( int row = 0; listCtrl.GetItemCount() > row; ++row )
	{
		for( int column = 0; column < columnSize; ++column )
		{
			file.WriteString( listCtrl.GetItemText( row, column ) );

			// 필드 끝 처리
			if( columnSize != column + 1 )
			{
				file.WriteString( _T( "," ) );
			}
		}

		file.WriteString( _T( "\n" ) );
	}

	{
		CString textLogSavedToWhere;
		textLogSavedToWhere.LoadString( IDS_STRING229 );

		CString text;
		text.Format( textLogSavedToWhere, dialog.GetPathName() );

		MessageBox(
			0,
			text,
			_T( "" ),
			MB_ICONINFORMATION | MB_OK );
	}
}

CString CclientApp::GetRankName( DWORD rank ) const
{
	switch( rank )
	{
	case GUILD_MASTER:
		{
			CString textMaster;
			textMaster.LoadString( IDS_STRING230 );

			return textMaster;
		}
	case GUILD_VICEMASTER:
		{
			CString textViceMaster;
			textViceMaster.LoadString( IDS_STRING231 );

			return textViceMaster;
		}
	case GUILD_SENIOR:
		{
			CString textOlder;
			textOlder.LoadString( IDS_STRING232 );

			return textOlder;
		}
	case GUILD_JUNIOR:
		{
			CString textSenior;
			textSenior.LoadString( IDS_STRING233 );

			return textSenior;
		}
	case GUILD_MEMBER:
		{
			CString textJunior;
			textJunior.LoadString( IDS_STRING234 );

			return textJunior;
		}
	case GUILD_STUDENT:
		{
			CString textStudent;
			textStudent.LoadString( IDS_STRING235 );

			return textStudent;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), rank );

			return text;
		}
	}
}


void CclientApp::SetRankCombo( CComboBox& combo ) const
{
	combo.ResetContent();

	CString textNoMember;
	textNoMember.LoadString( IDS_STRING236 );

	combo.AddString( textNoMember );

	// 길드 직위 간의 값 차이가 일정한 것을 이용하여 모든 직위를 세팅한다
	for(
		DWORD i = GUILD_STUDENT;
		GUILD_MASTER >= i;
		i += abs( GUILD_MEMBER - GUILD_STUDENT ) )
	{
		combo.AddString( GetRankName( i ) );
	}
}


void CclientApp::SetServerListCtrl( CListCtrl& listCtrl )
{
	listCtrl.DeleteAllItems();

	const CNetwork::ServerMap&		serverMap	= CNetwork::GetInstance().GetServerMap();
	const CNetwork::ConnectionMap&	connection	= CNetwork::GetInstance().GetConnectionMap();

	CString text;

	for( CNetwork::ServerMap::const_iterator it = serverMap.begin(); serverMap.end() != it; ++it )
	{
		const DWORD				serverIndex = it->first;
		const CNetwork::Server& server		= it->second;

		const DWORD row		= listCtrl.GetItemCount();
		int			step	= 0;

		text.Format( _T( "%d" ), serverIndex );
		listCtrl.InsertItem( row, text, 0 );

		listCtrl.SetItemText( row, ++step, server.mName );
		listCtrl.SetItemText( row, ++step, server.mIP );
		listCtrl.SetItemText( row, ++step, connection.end() == connection.find( serverIndex ) ? _T( "x" ) : _T( "o" ) );
	}
}


size_t CclientApp::GetConnectionSize() const
{
	return CNetwork::GetInstance().GetConnectionMap().size();
}

CString CclientApp::GetText(eConsignmentLog type) const
{
	switch(type)
	{
		// 등록
	case eConsignmentLog_Regist:
		{
			CString textAdd;
			textAdd.LoadString(
				IDS_STRING252);
			
			return textAdd;
		}
		// 구입
	case eConsignmentLog_Buy:
		{
			CString textBuy;
			textBuy.LoadString(
				IDS_STRING267);

			return textBuy;
		}
		// 갱신 (부분판매시 원본아이템의 수량변경)
	case eConsignmentLog_SellUpdate:
		{
			CString textUpdate;
			textUpdate.LoadString(
				IDS_STRING295);

			return textUpdate;
		}
		// 판매완료
	case eConsignmentLog_SoldOut:
		{
			CString textSale;
			textSale.LoadString(
				IDS_STRING268);

			return textSale;
		}
		// 유저취소
	case eConsignmentLog_UserCancel:
		{
			CString textCancel;
			textCancel.LoadString(
				IDS_STRING525);

			return textCancel;
		}
		// 기간취소
	case eConsignmentLog_TimeCancel:
		{
			CString textExpire;
			textExpire.LoadString(
				IDS_STRING526);

			return textExpire;
		}
		//소포획득
	case eConsignmentLog_GetPackage:
		{
			CString textGetPackage;
			textGetPackage.LoadString(
				IDS_STRING531);

			return textGetPackage;
		}
		//소포획득실패
	case eConsignmentLog_GetPackageFail:
		{
			CString textGetPackageFail;
			textGetPackageFail.LoadString(
				IDS_STRING542);

			return textGetPackageFail;
		}
		//소포기간삭제
	case eConsignmentLog_DelByDate:
		{
			CString textDelByDate;
			textDelByDate.LoadString(
				IDS_STRING529);

			return textDelByDate;
		}
	default:
		{
			CString text;
			text.Format(
				_T("%d"),
				type);

			return text;
		}
	}
}

CString CclientApp::GetText( eFamilyLog type ) const
{
	switch( type )
	{
	case eLog_FamilyPoint:
	case eLog_FamilyPointAdd:
		{
			CString textGetPoint;
			textGetPoint.LoadString( IDS_STRING238 );

			return textGetPoint;
		}
		// 080610 LUJ, 패밀리 포인트 변경
	case eLog_FamilyPointUpdate:
		{
			CString textSetScore;
			textSetScore.LoadString( IDS_STRING260 );

			return textSetScore;
		}
	case eLog_FamilyFarmPutCrop:
		{
			CString textSeed;
			textSeed.LoadString( IDS_STRING239 );

			return textSeed;
		}
	case eLog_FamilyFarmDieCrop:
		{
			CString textDied;
			textDied.LoadString( IDS_STRING240 );

			return textDied;
		}
	case eLog_FamilyFarmGrowCrop:
		{
			CString textGrow;
			textGrow.LoadString( IDS_STRING241 );

			return textGrow;
		}
	case eLog_FamilyFarmCareCrop:
		{
			CString textCare;
			textCare.LoadString( IDS_STRING242 );

			return textCare;
		}
	case eLog_FamilyFarmHarvestCrop:
		{
			CString textHarvest;
			textHarvest.LoadString( IDS_STRING243 );

			return textHarvest;
		}
	case eLog_FamilyFarmLostRight:	// 권리 상실(패밀리 탈퇴 등으로 인한 권리 상실)
		{
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );

			CString textSecede;
			textSecede.LoadString( IDS_STRING244 );

			return textFamily + _T( ": " ) + textSecede;
		}
	case eLog_FamilyFarmDisband:		// 농장 해체(패밀리 해산으로 인한)
		{
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );

			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textFamily + _T( ": " ) + textDisband;
		}
	case eLog_FamilyFarmTax:
		{
			CString textTaxDelayed;
			textTaxDelayed.LoadString( IDS_STRING310 );
			
			return textTaxDelayed;
		}
	// 080630 LUJ, 가축 로그
	case eLog_FamilyFarmInstallAnimal:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textAdd;
		}
	case eLog_FamilyFarmDieAnimal:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			return textRemove;
		}
	case eLog_FamilyFarmFeedAnimal:
		{
			CString textFeed;
			textFeed.LoadString( IDS_STRING391 );

			return textFeed;
		}
	case eLog_FamilyFarmCleanAnimal:
		{
			CString textClean;
			textClean.LoadString( IDS_STRING392 );

			return textClean;
		}
	case eLog_FamilyFarmAnimalReward:
		{
			CString textReward;
			textReward.LoadString( IDS_STRING393 );

			return textReward;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyCreate:
		{
			CString	textAdd;
			textAdd.LoadString( IDS_STRING252 );
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );
			return textFamily + _T( ": " ) + textAdd;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyAddMember:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );
			CString textMember;
			textMember.LoadString( IDS_STRING45  );

			return textMember + _T( ": " ) + textAdd;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyChangeNick:
		{
			CString textUpdate;
			textUpdate.LoadString( IDS_STRING295 );
			CString textNick;
			textNick.LoadString( IDS_STRING21 );

			return textNick + _T( ": " ) + textUpdate;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyRegistEmblem:
		{
			CString textUpdate;
			textUpdate.LoadString( IDS_STRING295 );
			CString textEmblem;
			textEmblem.LoadString( IDS_STRING430 );

			return textEmblem + _T( ": " ) + textUpdate;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyLeaveMember:
		{
			CString textLeave;
			textLeave.LoadString( IDS_STRING244 );
			CString textMember;
			textMember.LoadString( IDS_STRING45 );

			return textMember + _T( ": " ) + textLeave;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyExpelMember:
		{
			CString textKick;
			textKick.LoadString( IDS_STRING245 );
			CString textMember;
			textMember.LoadString( IDS_STRING45 );

			return textMember + _T( ": " ) + textKick;
		}
		// 081205 LUJ, 로그 추가
	case eLog_FamilyBreakup:
		{
			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );

			return textFamily + _T( ": " ) + textDisband;
		}
		// 패밀리 문장 삭제
	case eLog_FamilyDelEmelem:
		{
			CString textEmblem;
			textEmblem.LoadString(
				IDS_STRING534);
			CString textChange;
			textChange.LoadString(
				IDS_STRING295);

			return textChange + _T( ": " ) + textEmblem;
		}
		// 패밀리장 이양
	case eLog_FamilyChangeMaster:
		{
			CString textMaster;
			textMaster.LoadString(
				IDS_STRING19);
			CString textChange;
			textChange.LoadString(
				IDS_STRING295);

			return textChange + _T( ": " ) + textMaster;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


CString CclientApp::GetText( eGuildLog type ) const
{
	switch( type )
	{
	case eGuildLog_MemberSecede:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textSecede;
			textSecede.LoadString( IDS_STRING244 );

			return textSecede + _T( ": " ) + textGuild;
		}
	case eGuildLog_MemberBan:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textKick;
			textKick.LoadString( IDS_STRING245 );

			return textKick + _T( ": " ) + textGuild;
		}
	case eGuildLog_MemberAdd:
		{
			CString textJoin;
			textJoin.LoadString( IDS_STRING246 );

			CString textJunior;
			textJunior.LoadString( IDS_STRING234 );

			return textJoin + _T( ": " ) + textJunior;
		}
	case eGuildLog_StudentAdd:
		{
			CString textJoin;
			textJoin.LoadString( IDS_STRING246 );

			CString textStudent;
			textStudent.LoadString( IDS_STRING235 );

			return textJoin + _T( ": " ) + textStudent;
		}
	case eGuildLog_MasterChangeRank:
		{
			CString textChangeRank;
			textChangeRank.LoadString( IDS_STRING43 );

			return textChangeRank;
		}
	case eLogGuild_GuildCreate:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textBuild;
			textBuild.LoadString( IDS_STRING247 );

			return textBuild + _T( ": " ) + textGuild;
		}
	case eGuildLog_GuildBreakUp:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textDisband + _T( ": " ) + textGuild;
		}
	case eGuildLog_GuildLevelUp:
		{
			CString textLevelUp;
			textLevelUp.LoadString( IDS_STRING249 );

			return textLevelUp;
		}
	case eGuildLog_GuildLevelDown:
		{
			CString textLevelDown;
			textLevelDown.LoadString( IDS_STRING250 );

			return textLevelDown;
		}
	case eGuildLog_CreateUnion:
		{
			CString textAlliance;
			textAlliance.LoadString( IDS_STRING251 );

			CString textBuild;
			textBuild.LoadString( IDS_STRING247 );

			return textBuild + _T( ": " ) + textAlliance;
		}
	case eGuildLog_DestroyUnion:
		{
			CString textAlliance;
			textAlliance.LoadString( IDS_STRING251 );

			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textDisband + _T( ": " ) + textAlliance;
		}
	case eGuildLog_AddUnion:
		{
			CString textAlliance;
			textAlliance.LoadString( IDS_STRING251 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textAdd + _T( ": " ) + textAlliance;
		}
	case eGuildLog_RemoveUnion:	
		{
			CString textAlliance;
			textAlliance.LoadString( IDS_STRING251 );

			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			return textRemove + _T( ": " ) + textAlliance;
		}
	case eGuildLog_SecedeUnion:
		{
			CString textAlliance;
			textAlliance.LoadString( IDS_STRING251 );

			CString textKick;
			textKick.LoadString( IDS_STRING245 );

			return textKick + _T( ": " ) + textAlliance;
		}
	case eGuildLog_GM_ChangeRank:
		{
			CString textChangeRank;
			textChangeRank.LoadString( IDS_STRING43 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textChangeRank + textRecovery;
		}
	case eGuildLog_TimeScoreAdd:
		{
			CString textGetScore;
			textGetScore.LoadString( IDS_STRING254 );

			CString textRegular;
			textRegular.LoadString( IDS_STRING256 );

			return textGetScore + _T( ": " ) + textRegular;
		}
	case eGuildLog_ScoreAddByKill:
		{
			CString textGetScore;
			textGetScore.LoadString( IDS_STRING254 );

			CString textKill;
			textKill.LoadString( IDS_STRING257 );

			return textGetScore + _T( ": " ) + textKill;
		}
	case eGuildLog_ScoreRemoveByMurder:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );

			CString textKill;
			textKill.LoadString( IDS_STRING257 );

			return textLostScore + _T( ": " ) + textKill;
		}
	case eGuildLog_ScoreAddByWin:
		{
			CString textGetScore;
			textGetScore.LoadString( IDS_STRING254 );

			CString textWinWar;
			textWinWar.LoadString( IDS_STRING258 );

			return textGetScore + _T( ": " ) + textWinWar;
		}
	case eGuildLog_ScoreRemoveByDefeat:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );

			CString textLoseWar;
			textLoseWar.LoadString( IDS_STRING259 );

			return textLostScore + _T( ": " ) + textLoseWar;
		}
	case eGuildLog_ScoreCheat:
		{
			CString textSetScore;
			textSetScore.LoadString( IDS_STRING260 );

			CString textCheat;
			textCheat.LoadString( IDS_STRING262 );

			return textSetScore + _T( ": " ) + textCheat;
		}
	case eGuildLog_ScoreLevelUp:
		{
			CString textUseScore;
			textUseScore.LoadString( IDS_STRING261 );

			CString textLevelUp;
			textLevelUp.LoadString( IDS_STRING249 );

			return textUseScore + _T( ": " ) + textLevelUp;
		}
		// 080417 LUJ, 길드 스킬 획득
	case eGuildLog_SkillAdd:
		{
			CString textSkill;
			textSkill.LoadString( IDS_STRING365 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textSkill + _T( " " ) + textAdd;
		}
		// 080602 LUJ, 스킬 사용 시 길드 포인트 소모
	case eGuildLog_ScoreSkillUse:
		{
			CString textSkillWasAdded;
			textSkillWasAdded.LoadString( IDS_STRING321	);

			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );

			return textLostScore + _T( ": " ) + textSkillWasAdded;
		}
		// 081027 LUJ, 길드 토너먼트 신청 로그
	case eGuildLog_TournamentAdd:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );
			CString textTournament;
			textTournament.LoadString( IDS_STRING413 );
			CString textEntrance;
			textEntrance.LoadString( IDS_STRING414 );
			
			return textGuild + _T( " " ) + textTournament + _T( ": " ) + textEntrance;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


CString CclientApp::GetText( eLogSkill type ) const
{
	switch( type )
	{
	case eLog_SkillLearn:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textAdd;
		}
	case eLog_SkillDiscard:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			return textRemove;
		}
	case eLog_SkillLevelup:
		{
			CString textLevelUp;
			textLevelUp.LoadString( IDS_STRING249 );

			return textLevelUp;
		}
	case eLog_SkillCheatLevelup:
		{
			CString textLevelDown;
			textLevelDown.LoadString( IDS_STRING250 );

			CString textCheat;
			textCheat.LoadString( IDS_STRING262 );
            
			return textLevelDown + _T( ": " ) + textCheat;
		}
	case eLog_SkillLearn_RM:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING262 );
			
			return textAdd + _T( ": " ) + textRecovery;
		}
	case eLog_SkillLevel_RM:
		{
			CString textLevelUpdate;
			textLevelUpdate.LoadString( IDS_STRING263 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING262 );

			return textLevelUpdate + _T( ": " ) + textRecovery;
		}
	case eLog_SkillRemove_RM:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING262 );

			return textRemove + _T( ": " ) + textRecovery;
		}
	case eLog_SkillGetPoint:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textSkillPoint;
			textSkillPoint.LoadString( IDS_STRING96 );

			return textAdd + _T( ": " ) + textSkillPoint;
		}
		// 080819 LUJ, 캐쉬 스킬 추가
	case eLog_SkillCashAdd:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textCashSkill;
			textCashSkill.LoadString( IDS_STRING205 );
			
			return textAdd + _T( ": " ) + textCashSkill;
		}
		// 080819 LUJ, 캐쉬 스킬 해제
	case eLog_SkillCashRemove:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			CString textCashSkill;
			textCashSkill.LoadString( IDS_STRING205 );

			return textRemove + _T( ": " ) + textCashSkill;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


CString CclientApp::GetText( eLogitemmoney type ) const
{
	switch( type )
	{
	case eLog_ItemUse:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			return textUse;
		}
	case eLog_ItemObtainMix:
	case eLog_ItemMixSuccess:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			return textAdd + _T( ": " ) + textMix;
		}
	case eLog_ItemMixUse:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			return textUse+ _T( ": " ) + textMix;
		}
	case eLog_ItemObtainDivide:
		{
			CString textDivide;
			textDivide.LoadString( IDS_STRING288 );

			return textDivide;
		}
	case eLog_ItemSell:
		{
			CString textSell;
			textSell.LoadString( IDS_STRING268 );

			return textSell;
		}
	case eLog_ItemObtainBuy:
		{
			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textBuy;
		}
	case eLog_ItemObtainMonster:
		{
			CString textHunt;
			textHunt.LoadString( IDS_STRING287 );

			return textHunt;
		}
	case eLog_ItemObtainCheat:
		{
			CString textCheat;
			textCheat.LoadString( IDS_STRING262 );

			return textCheat;
		}
	case eLog_ItemObtainMonstermeter:
		{
			CString textMonsterMeter;
			textMonsterMeter.LoadString( IDS_STRING286 );

			return textMonsterMeter;
		}
	case eLog_ItemMixBigFail:
		{
			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			CString textBigFailed;
			textBigFailed.LoadString( IDS_STRING285 );

			return textMix + _T( ": " ) + textBigFailed;
		}
	case eLog_ItemMixFail:
		{
			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			CString textFailure;
			textFailure.LoadString( IDS_STRING265 );

			return textMix + _T( ": " ) + textFailure;
		}
	case eLog_ItemReinforceFail:
		{
			CString textReinforce;
			textReinforce.LoadString( IDS_STRING89 );

			CString textFail;
			textFail.LoadString( IDS_STRING266 );
				 
			return textReinforce + _T( ": " ) + textFail;
		}
	case eLog_ItemDestroyReinforce:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textReinforce;
			textReinforce.LoadString( IDS_STRING89 );

			return textReinforce + _T( ": " ) + textUse;
		}
	case eLog_ItemDiscard:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			return textRemove;
		}
	case eLog_ItemObtainQuest:
		{
			CString textQuest;
			textQuest.LoadString( IDS_STRING190 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textQuest + _T( ": " ) + textAdd;
		}
	case eLog_ItemObtainFromChangeItem:
		{
			CString textChange;
			textChange.LoadString( IDS_STRING284 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textChange + _T( ": " ) + textAdd;
		}
	case eLog_ItemDestroybyChangeItem:
		{
			CString textChange;
			textChange.LoadString( IDS_STRING284 );

			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			return textChange + _T( ": " ) + textUse;
		}
	case eLog_ItemMoveStorageToStorage:
		{
			CString textStorage;
			textStorage.LoadString( IDS_STRING282 );

			return textStorage + _T( "->" ) + textStorage;
		}
	case eLog_ItemMoveInvenToInven:
		{
			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			return textInventory + _T( "->" ) + textInventory;
		}
	case eLog_ItemMoveInvenToStorage:
		{
			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			CString textStorage;
			textStorage.LoadString( IDS_STRING282 );
				 
			return textInventory + _T( "->" ) + textStorage;
		}
	case eLog_ItemMoveInvenToGuild:
		{
			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			return textInventory + _T( "->" ) + textGuild;
		}
	case eLog_ItemMoveInvenToPetInven:
		{
			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			return textInventory + _T( "->" ) + textPet;
		}
	case eLog_ItemMoveStorageToInven:
		{
			CString textStorage;
			textStorage.LoadString( IDS_STRING282 );

			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			return textStorage + _T( "->" ) + textInventory;
		}
	case eLog_ItemMoveGuildToInven:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			return textGuild + _T( "->" ) + textInventory;
		}
	case eLog_ItemMoveGuild:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			return textGuild + _T( "->" ) + textGuild;
		}
	case eLog_ShopItemMoveToInven:
		{
			CString textCashStorage;
			textCashStorage.LoadString( IDS_STRING283 );

			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			return textCashStorage + _T( "->" ) + textInventory;
		}
	case eLog_ItemMovePetInvenToInven:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			CString textInventory;
			textInventory.LoadString( IDS_STRING281 );

			return textPet + _T( "->" ) + textInventory;
		}
	case eLog_ItemDestroyDissolution:
		{
			CString textDissolve;
			textDissolve.LoadString( IDS_STRING277 );

			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			return textDissolve + _T( ": " ) + textUse;
		}
	case eLog_ItemDissolveAddDuplicated:
	case eLog_ItemDissolveSuccess:
	case eLog_ItemObtainDissolve:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textDissolve;
			textDissolve.LoadString( IDS_STRING277 );

			return textDissolve + _T( ": " ) + textAdd;
		}
	case eLog_ItemDestroyDeleteQuest:
	case eLog_ItemDestroyGiveQuest:
		{
			CString textQuest;
			textQuest.LoadString( IDS_STRING190 );

			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			return textQuest + _T( ": " ) + textUse;
		}
	case eLog_ItemObtainPK:
		{
			CString textKill;
			textKill.LoadString( IDS_STRING257 );

			return textKill;
		}
	case eLog_ItemDestroyByWanted:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textWanted;
			textWanted.LoadString( IDS_STRING278 );

			return textWanted + _T( ": " ) + textUse;
		}
	case eLog_ItemDestroyByBadFame:
		{
			CString textBadFame;
			textBadFame.LoadString( IDS_STRING279 );

			return textBadFame;
		}
	case eLog_ItemReinforceSuccess:
		{
			CString textReinforce;
			textReinforce.LoadString( IDS_STRING89 );

			return textReinforce;
		}
	case eLog_ItemEnchantSuccess:
		{
			CString textEnchant;
			textEnchant.LoadString( IDS_STRING91 );

			return textEnchant;
		}
	case eLog_ItemEnchantUse:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textEnchant;
			textEnchant.LoadString( IDS_STRING91 );

			return textEnchant + _T( ": " ) + textUse;
		}
	case eLog_ItemEnchantDestroy:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textEnchant;
			textEnchant.LoadString( IDS_STRING91 );

			CString textFail;
			textFail.LoadString( IDS_STRING266 );

			return textEnchant + _T( ": " ) + textFail + _T( ": " ) + textUse;
		}
	case eLog_ItemEnchantLevelChanged:
		{
			CString textEnchant;
			textEnchant.LoadString(
				IDS_STRING91);
			CString textLevelChanged;
			textLevelChanged.LoadString(
				IDS_STRING264);

			return textEnchant + _T( ": " ) + textLevelChanged;
		}
	case eLog_ItemEnchantProtected:
		{
			CString textEnchant;
			textEnchant.LoadString(
				IDS_STRING91);
			CString textFail;
			textFail.LoadString( IDS_STRING266 );
			CString textProtect;
			textProtect.LoadString(
				IDS_STRING274);

			return textEnchant + textFail + _T( ": " ) + textProtect;
		}
	case eLog_Item:
	case eLog_ItemAdd:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textRecovery + _T( ": " ) + textAdd;
		}
	case eLog_ItemRemove:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textRecovery + _T( ": " ) + textRemove;
		}
	case eLog_ItemOptionAdd_RM:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textOption;
			textOption.LoadString( IDS_STRING55 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textRecovery + _T( ": " ) + textAdd + _T( ": " ) + textOption;
		}
	case eLog_ItemOptionRemove_RM:
		{
			CString textOption;
			textOption.LoadString( IDS_STRING55 );

			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textRecovery + _T( ": " ) + textRemove + _T( ": " ) + textOption;
		}
	case eLog_StreetStallBuyAll:
		{
			CString textStall;
			textStall.LoadString( IDS_STRING269 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textStall + _T( ": " ) + textBuy;
		}
	case eLog_StreetStallBuyDivide:
		{
			CString textStall;
			textStall.LoadString( IDS_STRING269 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			CString textPartial;
			textPartial.LoadString( IDS_STRING276 );

			return textStall + _T( ": " ) + textBuy + _T( ": " ) + textPartial;
		}
	case eLog_StreetStallSellDivide:
		{
			CString textStall;
			textStall.LoadString( IDS_STRING269 );

			CString textSell;
			textSell.LoadString( IDS_STRING268 );

			CString textPartial;
			textPartial.LoadString( IDS_STRING276 );

			return textStall + _T( ": " ) + textSell + _T( ": " ) + textPartial;
		}
	case eLog_Exchange:
		{
			CString textExchange;
			textExchange.LoadString( IDS_STRING275 );

			return textExchange;
		}
	case eLog_ShopItemUse:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textCashItem + _T( ": " ) + textUse;
		}
	case eLog_ShopItemUseEnd:
		{
			CString textFinish;
			textFinish.LoadString( IDS_STRING292 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textCashItem + _T( ": " ) + textFinish;
		}
	case eLog_ShopItemProtectAll:
		{
			CString textProtect;
			textProtect.LoadString( IDS_STRING274 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textCashItem + _T( ": " ) + textProtect;
		}
	case eLog_ShopItemSeal:
		{
			CString textSeal;
			textSeal.LoadString( IDS_STRING272 );

			return textSeal;
		}
	case eLog_ShopItemUnseal:
		{
			CString textUnseal;
			textUnseal.LoadString( IDS_STRING273 );

			CString textSuccess;
			textSuccess.LoadString( IDS_STRING265 );
            
			return textUnseal + _T( ": " ) + textSuccess;
		}
	case eLog_ShopItemUnseal_Failed:
		{
			CString textUnseal;
			textUnseal.LoadString( IDS_STRING273 );

			CString textFail;
			textFail.LoadString( IDS_STRING266 );

			return textUnseal + _T( ": " ) + textFail;
		}
	case eLog_ShopItem_ReinforceReset:
		{
			CString textResetReinforce;
			textResetReinforce.LoadString( IDS_STRING89 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textResetReinforce + _T( ": " ) + textCashItem;
		}
	case eLog_ShopItemGetCheat:
		{
			CString textCheat;
			textCheat.LoadString( IDS_STRING262 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textCheat + _T( ": " ) + textAdd + _T( ": " ) + textCashItem;
		}
	case eLog_ShopItemUse_MixSuccess:
		{
			CString textMix;
			textMix.LoadString( IDS_STRING90 );

			CString textSuccess;
			textSuccess.LoadString( IDS_STRING265 );

			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			return textCashItem + _T( ": " ) + textMix + _T( ": " ) + textSuccess;
		}
	case eLog_ItemSkillLearn:
		{
			CString textSkillAdd;
			textSkillAdd.LoadString( IDS_STRING321 );

			return textSkillAdd;
		}
		// 080509 LUJ, 선물 획득 추가
	case eLog_ItemObtainGift:
		{
			CString textGift;
			textGift.LoadString( IDS_STRING378 );

			return textGift;
		}
		// 080522 LUJ, 물고기 획득 추가
	case eLog_ItemObtainFishing:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );
			CString textFishing;
			textFishing.LoadString( IDS_STRING379 );

			return textFishing + _T( ": " ) + textAdd;
		}
		// 080526 LUJ, PK 로그 추가
	case eLog_MoneyObtainPK:
		{
			CString textMoney;
			textMoney.LoadString( IDS_STRING58 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return _T( "PK " ) + textAdd + _T( ": " ) + textMoney;
		}
		// 080526 LUJ, PK 로그 추가
	case eLog_ExpObtainPK:
		{
			CString textExperience;
			textExperience.LoadString( IDS_STRING9 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return _T( "PK " ) + textAdd + _T( ": " ) + textExperience;
		}
		// 080605 LUJ, 합병 로그 추가
	case eLog_ItemObtainMerge:
		{
			CString textMerge;
			textMerge.LoadString( IDS_STRING380 );

			return textMerge;
		}
		// 080602 LUJ, 스킬 사용 시 소지금 소모
	case eLog_SkillMoneyUse:
		{
			CString textMoney;
			textMoney.LoadString( IDS_STRING95 );

			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textSkillWasAdded;
			textSkillWasAdded.LoadString( IDS_STRING321 );
			
			return textMoney + _T( " " ) + textUse + _T( ": " ) + textSkillWasAdded;
		}
		// 080602 LUJ, 스킬 사용 시 아이템 소모
	case eLog_SkillItemUse:
		{
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			CString textSkillWasAdded;
			textSkillWasAdded.LoadString( IDS_STRING321 );

			return textItem + _T( " " ) + textUse + _T( ": " ) + textSkillWasAdded;
		}
		// 080716 LUJ, 펫 -> 플레이어 간 아이템 이동
	case eLog_ItemMoveFromPetToPlayer:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			CString textPlayer;
			textPlayer.LoadString( IDS_STRING54 );

			return textPet + _T( " > " ) + textPlayer;
		}
		// 080716 LUJ, 플레이어 -> 펫 간 아이템 이동
	case eLog_ItemMoveFromPlayerToPet:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			CString textPlayer;
			textPlayer.LoadString( IDS_STRING54 );

			return textPlayer + _T( " > " ) + textPet;
		}
		// 080814 LUJ, 잘못된 위치에 있는 펫 아이템을 강제로 삭제함
	case eLog_PetItemDiscard:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );
			CString textRemove;
			textRemove.LoadString( IDS_STRING315 );

			return textPet + _T( ": " ) + textRemove;
		}
		// 081027 LUJ, 드롭 옵션 적용
	case eLog_ItemDropOptionAdd:
		{
			CString textDropOption;
			textDropOption.LoadString( IDS_STRING354 );
			CString textAdd;
			textAdd.LoadString( IDS_STRING317 );

			return textDropOption + _T( ": " ) + textAdd;
		}
		// 081027 LUJ, 길드 토너먼트 아이템 지급
	case eLog_ItemObtainGTReward:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );
			CString textTournament;
			textTournament.LoadString( IDS_STRING413 );
			CString textAdd;
			textAdd.LoadString( IDS_STRING317 );

			return textGuild + _T( " " ) + textTournament + _T( ": " ) + textAdd;
		}
		// 081205 LUJ, 농장 세금 납부
	case eLog_FarmTaxPaid:
		{
			CString textFarm;
			textFarm.LoadString( IDS_STRING362 );
			CString textPaidTax;
			textPaidTax.LoadString( IDS_STRING363 );

			return textFarm + _T( ": " ) + textPaidTax;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeSuccess:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textSuccess;
			textSuccess.LoadString( IDS_STRING265 );

			return textCompose + _T( ": " ) + textSuccess;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeSuccessWithConsume:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textUse;
			textUse.LoadString( IDS_STRING270 );

			return textCompose + _T( ": " ) + textUse;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByMemoryMismatch:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textMemoryMismatch;
			textMemoryMismatch.LoadString( IDS_STRING433 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textMemoryMismatch;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByWrongScript:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textWrongScript;
			textWrongScript.LoadString( IDS_STRING431 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textWrongScript;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByWrongUseItemSize:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textWrongUseItemSize;
			textWrongUseItemSize.LoadString( IDS_STRING434 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textWrongUseItemSize;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByInvalidResult:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textInvalidResult;
			textInvalidResult.LoadString( IDS_STRING435 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textInvalidResult;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByInvalidKeyItem:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textInvalidKeyItem;
			textInvalidKeyItem.LoadString( IDS_STRING436 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textInvalidKeyItem;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByNoExistedOption:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textNoExistedOption;
			textNoExistedOption.LoadString( IDS_STRING437 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textNoExistedOption;
		}
		// 090207 LUJ, 아이템 합성 로그
	case eLog_ItemComposeFailureByNoResultItemInfo:
		{
			CString textCompose;
			textCompose.LoadString( IDS_STRING432 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );
			CString textNoResultItemInfo;
			textNoResultItemInfo.LoadString( IDS_STRING438 );

			return textCompose + _T( ": " ) + textFailure + _T( ": " ) + textNoResultItemInfo;
		}
	case eLog_GTournamentReg:
		{
			CString text;
			text.LoadString(
				IDS_STRING472);
			return text;
		}
	case eLog_ItemCookUse:
		{
			CString text;
			text.LoadString(
				IDS_STRING270);
			return text;
		}
	case eLog_ItemCookSuccess:
		{
			CString text;
			text.LoadString(
				IDS_STRING265);
			return text;
		}
	case eLog_ItemVehicleToll:
		{
			CString textToll;
			textToll.LoadString(
				IDS_STRING517);

			return textToll;
		}
	case eLog_ItemVehicleSummon:
		{
			CString textSummon;
			textSummon.LoadString(
				IDS_STRING400);
			return textSummon;
		}
	case eLog_ItemVehicleUnsummon:
		{
			CString textUnsummon;
			textUnsummon.LoadString(
				IDS_STRING518);
			return textUnsummon;
		}
	case eLog_ItemHouseKeep:
		{
			CString text;
			text.LoadString(
				IDS_STRING474);
			return text;
		}
	case eLog_ItemHouseUnKepp:
		{
			CString text;
			text.LoadString(
				IDS_STRING475);
			return text;
		}
	case eLog_ItemUseHouseItem:
		{
			CString text;
			text.LoadString(
				IDS_STRING270);
			return text;
		}
	case eLog_CoinDecreaseByItemBuy:
		{
			CString text;
			text.LoadString(
				IDS_STRING270);
			return text;
		}
	case eLog_CoinDestroyByItemBuy:
		{
			CString text;
			text.LoadString(
				IDS_STRING270);
			return text;
		}
	case eLog_AddItemByTrigger:
		{
			CString text;
			text.LoadString(
				IDS_STRING472);
			return text;
		}
	case eLog_RemoveItemByTrigger:
		{
			CString text;
			text.LoadString(
				IDS_STRING472);
			return text;
		}
	case eLog_DungeonBegin:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING519);
			CString textEnter;
			textEnter.LoadString(
				IDS_STRING414);

			return textDungeon + _T(" ") + textEnter;
		}
	case eLog_DungeonEnd:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING519);
			CString textEnd;
			textEnd.LoadString(
				IDS_STRING292);

			return textDungeon + _T(" ") + textEnd;
		}
	case eLog_LimitDungeonBegin:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING520);
			CString textEnter;
			textEnter.LoadString(
				IDS_STRING414);

			return textDungeon + _T(" ") + textEnter;
		}
	case eLog_LimitDungeonEnd:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING520);
			CString textEnd;
			textEnd.LoadString(
				IDS_STRING292);

			return textDungeon + _T(" ") + textEnd;
		}
	case eLog_DateMatchBegin:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING521);
			CString textEnter;
			textEnter.LoadString(
				IDS_STRING414);

			return textDungeon + _T(" ") + textEnter;
		}
	case eLog_DateMatchEnd:
		{
			CString textDungeon;
			textDungeon.LoadString(
				IDS_STRING521);
			CString textEnd;
			textEnd.LoadString(
				IDS_STRING292);

			return textDungeon + _T(" ") + textEnd;
		}
		// 판매대행등록
	case eLog_Consignment_Regist:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textAdd;
			textAdd.LoadString(
				IDS_STRING252);

			return textConsign + _T(": ") + textAdd;
		}
		// 판매대행구매
	case eLog_Note_Consignmnet_Buy:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textBuy;
			textBuy.LoadString(
				IDS_STRING267);

			return textConsign + _T(": ") + textBuy;
		}
		// 판매대행갱신 (부분판매시 원본아이템의 수량변경)
	case eLog_Note_Consignmnet_SellUpdate:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textSellPartially;
			textSellPartially.LoadString(
				IDS_STRING523);

			return textConsign + _T(": ") + textSellPartially;
		}
		// 판매대행영수증
	case eLog_Note_Consignmnet_Receipt:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textReceipt;
			textReceipt.LoadString(
				IDS_STRING524);

			return textConsign + _T(": ") + textReceipt;
		}
		// 판매대행취소(유저)
	case eLog_Note_Consignmnet_UserCancel:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textCancel;
			textCancel.LoadString(
				IDS_STRING525);

			return textConsign + _T(": ") + textCancel;
		}
		// 판매대행취소(기간)
	case eLog_Note_Consignmnet_TimeCancel:
		{
			CString textConsign;
			textConsign.LoadString(
				IDS_STRING522);
			CString textExpire;
			textExpire.LoadString(
				IDS_STRING526);

			return textConsign + _T(": ") + textExpire;
		}
		// 우편받기삭제(수량성 기존아이템에 합쳐져서 제거함)
	case eLog_Note_CombineDelete:
		{
			CString textMailBox;
			textMailBox.LoadString(
				IDS_STRING527);
			CString textItemRemove;
			textItemRemove.LoadString(
				IDS_STRING315);

			return textMailBox + _T(": ") + textItemRemove;
		}
		// 우편받기지급(아이템)
	case eLog_Note_Obtain_Success:
		{
			CString textMailBox;
			textMailBox.LoadString(
				IDS_STRING527);
			CString textItem;
			textItem.LoadString(
				IDS_STRING189);

			return textMailBox + _T(": ") + textItem;
		}
		// 우편받기지급(골드)
	case eLog_Note_Obtain_Money:
		{
			CString textMailBox;
			textMailBox.LoadString(
				IDS_STRING527);
			CString textIncome;
			textIncome.LoadString(
				IDS_STRING528);

			return textMailBox + _T(": ") + textIncome;
		}
		// 우편유저삭제
	case eLog_Note_UserDelete:
		{
			CString textMail;
			textMail.LoadString(
				IDS_STRING516);
			CString textRemove;
			textRemove.LoadString(
				IDS_STRING253);

			return textMail + _T(": ") + textRemove;
		}
		// 우편기간삭제
	case eLog_Note_TimeDelete:
		{
			CString textMail;
			textMail.LoadString(
				IDS_STRING516);
			CString textExpire;
			textExpire.LoadString(
				IDS_STRING526);

			return textMail + _T(": ") + textExpire;
		}
	case eLog_ItemChangeClass:
		{
			CString textJob;
			textJob.LoadString(
				IDS_STRING365);
			CString textChange;
			textChange.LoadString(
				IDS_STRING295);

			return textChange + _T(": ") + textJob;
		}
	case eLog_ShopItemUseEndFromPetInven:
		{
			CString textMessage;
			textMessage.LoadString(
				IDS_STRING541);

			return textMessage;
		}
	case eLog_Note_Obtain_Fail:
		{
			CString textMessage;
			textMessage.LoadString(
				IDS_STRING542);

			return textMessage;
		}
	case eLog_Note_SendItemFromNPC:
		{
			CString textMessage;
			textMessage.LoadString(
				IDS_STRING543);

			return textMessage;
		}
	case eLog_ItemUse_GTBUFFITEM:
		{
			CString textMessage;
			textMessage.LoadString(
				IDS_STRING544);

			return textMessage;
		}
	default:
		{
			CString text;
			text.Format( _T( "?(%d)" ), type );

			return text;
		}
	}
}


CString CclientApp::GetText( eQuestLog type ) const
{
	switch( type )
	{
	case eQuestLog_GM:
		{
			CString textRecovery;
			textRecovery.LoadString( IDS_STRING263 );

			return textRecovery;
		}
	case eQuestLog_Main_Begin:
		{
			CString textMainQuest;
			textMainQuest.LoadString( IDS_STRING289 );

			CString textBegin;
			textBegin.LoadString( IDS_STRING291 );

			return textMainQuest + _T( ": " ) + textBegin;
		}
	case eQuestLog_Sub_Begin:
		{
			CString textSubQuest;
			textSubQuest.LoadString( IDS_STRING290 );

			CString textBegin;
			textBegin.LoadString( IDS_STRING291 );

			return textSubQuest + _T( ": " ) + textBegin;
		}
	case eQuestLog_Main_End:
		{
			CString textMainQuest;
			textMainQuest.LoadString( IDS_STRING289 );

			CString textEnd;
			textEnd.LoadString( IDS_STRING292 );

			return textMainQuest + _T( ": " ) + textEnd;
		}
	case eQuestLog_Sub_End:
		{
			CString textSubQuest;
			textSubQuest.LoadString( IDS_STRING290 );

			CString textEnd;
			textEnd.LoadString( IDS_STRING292 );

			return textSubQuest + _T( ": " ) + textEnd;
		}
	case eQuestLog_Main_Delete:
		{
			CString textMainQuest;
			textMainQuest.LoadString( IDS_STRING289 );

			CString textGiveUp;
			textGiveUp.LoadString( IDS_STRING293 );

			return textMainQuest + _T( ": " ) + textGiveUp;
		}
	case eQuestLog_Sub_Delete:
		{
			CString textSubQuest;
			textSubQuest.LoadString( IDS_STRING290 );

			CString textGiveUp;
			textGiveUp.LoadString( IDS_STRING293 );

			return textSubQuest + _T( ": " ) + textGiveUp;
		}
	case eQuestLOg_ItemTake:
		{
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			return textAdd + _T( ": " ) + textItem;
		}
	case eQuestLOg_ItemGive:
		{
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textGive;
			textGive.LoadString( IDS_STRING294 );

			return textGive + _T( ": " ) + textItem;
		}
	case eQuestLOg_ItemUpdate:
		{
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			CString textUpdate;
			textUpdate.LoadString( IDS_STRING295 );

			return textUpdate + _T( ": " ) + textItem;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


// 080328 LUJ, 드롭 옵션 키에 대한 문자열 반환
CString CclientApp::GetText( ITEM_OPTION::Drop::Key key ) const
{
	switch( key )
	{
		case ITEM_OPTION::Drop::KeyPlusStrength:
			{
				CString text;
				text.LoadString( IDS_STRING70 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusIntelligence:
			{
				CString text;
				text.LoadString( IDS_STRING73 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusDexterity:
			{
				CString text;
				text.LoadString( IDS_STRING71 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusWisdom:
			{
				CString text;
				text.LoadString( IDS_STRING74 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusVitality:
			{
				CString text;
				text.LoadString( IDS_STRING72 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalAttack:
			{
				CString text;
				text.LoadString( IDS_STRING79 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusPhysicalDefence:
			{
				CString text;
				text.LoadString( IDS_STRING80 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalAttack:
			{
				CString text;
				text.LoadString( IDS_STRING81 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusMagicalDefence:
			{
				CString text;
				text.LoadString( IDS_STRING82 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalRate:
			{
				CString text;
				text.LoadString( IDS_STRING86 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusCriticalDamage:
			{
				CString text;
				text.LoadString( IDS_STRING87 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusAccuracy:
			{
				CString text;
				text.LoadString( IDS_STRING85 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusEvade:
			{
				CString text;
				text.LoadString( IDS_STRING84 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusMoveSpeed:
			{
				CString text;
				text.LoadString( IDS_STRING83 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusLife:
			{
				CString text;
				text.LoadString( IDS_STRING75 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusMana:
			{
				CString text;
				text.LoadString( IDS_STRING76 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusLifeRecovery:
			{
				CString text;
				text.LoadString( IDS_STRING77 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPlusManaRecovery:
			{
				CString text;
				text.LoadString( IDS_STRING78 );

				return text + _T( "(+)" );
			}
		case ITEM_OPTION::Drop::KeyPercentStrength:
			{
				CString text;
				text.LoadString( IDS_STRING70 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentIntelligence:
			{
				CString text;
				text.LoadString( IDS_STRING73 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentDexterity:
			{
				CString text;
				text.LoadString( IDS_STRING71 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentWisdom:
			{
				CString text;
				text.LoadString( IDS_STRING74 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentVitality:
			{
				CString text;
				text.LoadString( IDS_STRING75 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalAttack:
			{
				CString text;
				text.LoadString( IDS_STRING79 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentPhysicalDefence:
			{
				CString text;
				text.LoadString( IDS_STRING80 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalAttack:
			{
				CString text;
				text.LoadString( IDS_STRING81 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentMagicalDefence:
			{
				CString text;
				text.LoadString( IDS_STRING82 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalRate:
			{
				CString text;
				text.LoadString( IDS_STRING86 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentCriticalDamage:
			{
				CString text;
				text.LoadString( IDS_STRING87 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentAccuracy:
			{
				CString text;
				text.LoadString( IDS_STRING85 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentEvade:
			{
				CString text;
				text.LoadString( IDS_STRING84 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentMoveSpeed:
			{
				CString text;
				text.LoadString( IDS_STRING83 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentLife:
			{
				CString text;
				text.LoadString( IDS_STRING75 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentMana:
			{
				CString text;
				text.LoadString( IDS_STRING76 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentLifeRecovery:
			{
				CString text;
				text.LoadString( IDS_STRING77 );

				return text + _T( "(%)" );
			}
		case ITEM_OPTION::Drop::KeyPercentManaRecovery:
			{
				CString text;
				text.LoadString( IDS_STRING78 );

				return text + _T( "(%)" );
			}
		default:
			{
				CString text;
				text.Format( _T( "%d" ), key );

				return text;
			}
	}
}


// 080402 LUJ, 농장에 대한 로그 반환. 농장 작업자가 로그 작성 시 임의로 해서 불가피하게 두 가지 종류가 생겼다...
CString CclientApp::GetText( FARM_LOG_KIND kind ) const
{
	switch( kind )
	{
	case FARM_LOG_KIND_BUY:
		{
			CString textFarm;
			textFarm.LoadString( IDS_STRING362 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textFarm + _T( ": " ) + textBuy;
		}
	case FARM_LOG_KIND_UPGRADE:
		{
			CString textFarm;
			textFarm.LoadString( IDS_STRING362 );

			CString textUpgrade;
			textUpgrade.LoadString( IDS_STRING295 );

			return textFarm + _T( ": " ) + textUpgrade;
		}
	case FARM_LOG_KIND_PAY_TAX:
		{
			CString textFarm;
			textFarm.LoadString( IDS_STRING362 );

			CString textTaxPay;
			textTaxPay.LoadString( IDS_STRING363 );

			return textFarm + _T( ": " ) + textTaxPay;
		}
	case FARM_LOG_KIND_DEL_FAMILY_BREAKUP:
		{
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );

			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textFamily + _T( ": " ) + textDisband;
		}
	case FARM_LOG_KIND_DEL_TAX_ARREARAGE:
		{
			CString textTaxArrearage;
			textTaxArrearage.LoadString( IDS_STRING310 );

			return textTaxArrearage;
		}
	case FARM_LOG_KIND_MYSTERY:
	default:
		{
			CString text;
			text.Format( _T( "%d" ), kind );

			return text;
		}
	}
}


CString CclientApp::GetText( ItemShopLog log ) const
{
	switch( log )
	{
	case ItemShopLogBuy:
		{
			CString textCashItem;
			textCashItem.LoadString( IDS_STRING271 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textCashItem + _T( ":" ) + textBuy;
		}
	case ItemShopLogLuckyBuy:
		{
			CString textLuckyBox;
			textLuckyBox.LoadString( IDS_STRING364 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textLuckyBox + _T( ":" ) + textBuy;
		}
	case ItemShopLogLucky77Buy:
		{
			CString textLuckyBox;
			textLuckyBox.LoadString( IDS_STRING364 );

			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textLuckyBox + _T( " 77:" ) + textBuy;
		}
	case ItemShopLogRegular:
		{
			CString textRegular;
			textRegular.LoadString( IDS_STRING256 );
			CString textItemAdd;
			textItemAdd.LoadString( IDS_STRING317 );

			return textRegular + _T( ": " ) + textItemAdd;
		}
		// 081021 LUJ, 투표로 획득한 유료 아이템
	case ItemShopLogVote:
		{
			CString textItemAdd;
			textItemAdd.LoadString( IDS_STRING317 );
			CString textVote;
			textVote.LoadString( IDS_STRING412 );

			return textVote + _T( ": " ) + textItemAdd;
		}
		// 081021 LUJ, 이벤트로 획득한 유료 아이템
	case ItemShopLogEvent:
		{
			CString textItemAdd;
			textItemAdd.LoadString( IDS_STRING317 );
			CString textEvent;
			textEvent.LoadString( IDS_STRING411 );

			return textEvent + _T( ": " ) + textItemAdd;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}


// 080416 LUJ, 경험치 로그 종류 반환
// 080716 LUJ, 잘못된 타입을 수정
CString CclientApp::GetText( eLogExppoint type ) const
{
	switch( type )
	{
	case eExpLog_LosebyRevivePresent:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			// 080716 LUJ, 설명을 자세히 하기 위해 부활 텍스트 추가
			CString textRevive;
			textRevive.LoadString( IDS_STRING401 );

			return textLostScore + _T( ": " ) + textRevive;
		}
	case eExpLog_LevelUp:
		{
			CString textLevelUp;
			textLevelUp.LoadString( IDS_STRING249 );

			return textLevelUp;
		}
	case eExpLog_GetbyQuest:
		{
			CString textQuest;
			textQuest.LoadString( IDS_STRING190 );

			CString textExperience;
			textExperience.LoadString( IDS_STRING9 );
			
			return textQuest + _T( " " ) + textExperience;
		}
	case eExpLog_ProtectExp:
		{
			CString textProtection;
			textProtection.LoadString( IDS_STRING274 );

			return textProtection;
		}
	case eExpLog_Time:
		{
			CString	 textInterval;
			textInterval.LoadString( IDS_STRING256 );

			CString textLog;
			textLog.LoadString( IDS_STRING10 );

			return textInterval + _T( " " ) + textLog;
		}
		// 080602 LUJ, PK로 인한 경험치 손실
	case eExpLog_LoseByPKMode:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );

			return textLostScore + _T( ": PK" );
		}
		// 080602 LUJ, 스킬 사용으로 인한 경험치 소모
	case eExpLog_LoseBySkillUse:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );

			CString textSkillWasAdded;
			textSkillWasAdded.LoadString( IDS_STRING321 );

			return textLostScore + _T( ": " ) + textSkillWasAdded;
		}
		// 080716 LUJ, PK모드시 사망 때의 텍스트
	case eExpLog_LosebyBadFame:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textDie;
			textDie.LoadString( IDS_STRING240 );
			CString textStatus;
			textStatus.LoadString( IDS_STRING399 );

			return textLostScore + _T( ": " ) + textDie + _T( "(PK " ) + textStatus + _T( ")" );
		}
		// 080716 LUJ, 길드 해산에 따른 경험치 손실 로그
	case eExpLog_LosebyBreakupGuild:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );
			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textLostScore + _T( ": " ) + textDisband + _T( "(" ) + textGuild + _T( ")" );
		}
		// 080716 LUJ, 경험치 보호
	case eExpLog_LosebyReviveExpProtect:
		{
			CString textProtect;
			textProtect.LoadString( IDS_STRING274 );
			CString textRevive;
			textRevive.LoadString( IDS_STRING401 );

			return textProtect + _T( ": " ) + textRevive;
		}
		// 081021 LUJ, 길드전에 의한 손실
	case eExpLog_LosebyGuildFieldWar:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textGuildFieldWar;
			textGuildFieldWar.LoadString( IDS_STRING410 );

			return textLostScore + _T( ": " ) + textGuildFieldWar;
		}
		// 081021 LUJ, 패밀리 해산
	case eExpLog_LosebyBreakupFamily:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );
			CString textDisband;
			textDisband.LoadString( IDS_STRING248 );

			return textLostScore + _T( ": " ) + textDisband + _T( "(" ) + textFamily + _T( ")" );
		}
		// 081021 LUJ, 패밀리 탈퇴
	case eExpLog_LosebyLeaveFamily:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );
			CString textLeave;
			textLeave.LoadString( IDS_STRING244 );
			
			return textLostScore + _T( ": " ) + textLeave + _T( "(" ) + textFamily + _T( ")" );
		}
		// 081021 LUJ, 패밀리에서 추방
	case eExpLog_LosebyExpleFamilyMember:
		{
			CString textLostScore;
			textLostScore.LoadString( IDS_STRING255 );
			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );
			CString textKick;
			textKick.LoadString( IDS_STRING245 );
			
			return textLostScore + _T( ": " ) + textKick + _T( "(" ) + textFamily + _T( ")" );
		}
		// 081021 LUJ, 퀘스트로 획득
	case eExpLog_SExpGetbyQuest:
		{
			CString textAddScore;
			textAddScore.LoadString(IDS_STRING254 );
			CString textQuest;
			textQuest.LoadString( IDS_STRING190 );

			return textAddScore + _T( ": " ) + textQuest;
		}
		// 081027 LUJ, 획득
	case eExpLog_Get:
		{
			CString textAddScore;
			textAddScore.LoadString( IDS_STRING254 );

			return textAddScore;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


// 080430 LUJ, 오브젝트 종류 문자열 반환
CString CclientApp::GetText( EObjectKind kind ) const
{
	switch( kind )
	{
	case eObjectKind_Player:
		{
			CString textPlayer;
			textPlayer.LoadString( IDS_STRING54 );

			return textPlayer;
		}
	case eObjectKind_Npc:
		{
			return _T( "NPC" );
		}
	case eObjectKind_Item:
		{
			CString textItem;
			textItem.LoadString( IDS_STRING189 );

			return textItem;
		}
	case eObjectKind_SkillObject:
		{
			CString textSkill;
			textSkill.LoadString( IDS_STRING365 );

			return textSkill;
		}
	case eObjectKind_Monster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			return textMonster;
		}
	case eObjectKind_BossMonster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			CString textBoss;
			textBoss.LoadString( IDS_STRING372 );

			return textBoss + _T( " " ) + textMonster;
		}
	case eObjectKind_SpecialMonster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			CString textSpecial;
			textSpecial.LoadString( IDS_STRING373 );

			return textSpecial + _T( " " ) + textMonster; ;
		}
	case eObjectKind_FieldBossMonster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			CString textField;
			textField.LoadString( IDS_STRING374 );

			CString textBoss;
			textBoss.LoadString( IDS_STRING372 );

			return textField + _T( " " ) + textBoss + _T( " " ) + textMonster;
		}
	case eObjectKind_FieldSubMonster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			CString textField;
			textMonster.LoadString( IDS_STRING374 );

			CString textSubBoss;
			textSubBoss.LoadString( IDS_STRING375 );

			return textField + _T( " " ) + textSubBoss + _T( " " ) + textMonster;
		}
	case eObjectKind_ToghterPlayMonster:
		{
			return _T( "togetherPlayerMonster" );
		}
	case eObjectKind_ChallengeZoneMonster:
		{
			CString textMonster;
			textMonster.LoadString( IDS_STRING371 );

			CString textChallengeZone;
			textChallengeZone.LoadString( IDS_STRING376 );

			return textChallengeZone + _T( " " ) + textMonster;
		}
	case eObjectKind_MapObject:
		{
			CString textMap;
			textMap.LoadString( IDS_STRING163 );

			return textMap;
		}
	case eObjectKind_CastleGate:
		{
			CString textCastleGate;
			textCastleGate.LoadString( IDS_STRING377 );

			return textCastleGate;
		}
	case eObjectKind_Pet:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			return textPet;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), kind );

			return text;
		}
	}
}


// 080523 LUJ, 낚시 로그 반환
CString CclientApp::GetText( eFishingLog log ) const
{
	switch( log )
	{
	case eFishingLog_SetPointFromCheat:
		{
			CString textPointAdd;
			textPointAdd.LoadString( IDS_STRING238 );
			CString textCheat;
			textCheat.LoadString( IDS_STRING262 );

			return textPointAdd + _T( "(" ) + textCheat + _T( ")" );
		}
	case eFishingLog_GetPointFromFish:
		{
			CString textPointAdd;
			textPointAdd.LoadString( IDS_STRING238 );

			return textPointAdd;
		}
	case eFishingLog_UsePointForBuyItem:
		{
			CString textBuy;
			textBuy.LoadString( IDS_STRING267 );

			return textBuy;
		}
		// 080808 LUJ, 레벨 변경
	case eFishingLog_SetLevel:
		{
			CString textLevelChange;
			textLevelChange.LoadString( IDS_STRING264 );
			
			return textLevelChange;
		}
		// 080808 LUJ, 정기 저장
	case eFishingLog_Regular:
		{
			CString textRegular;
			textRegular.LoadString( IDS_STRING256 );

			return textRegular;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}


// 080716 LUJ, 펫 종류 문자열을 반환한다
CString CclientApp::GetText( ePetType type ) const
{
	switch( type )
	{
	case ePetType_Basic:
		{
			return GetInterfaceMessage( 1000 );
		}
	case ePetType_Physic:
		{
			return GetInterfaceMessage( 997 );
		}
	case ePetType_Magic:
		{
			return GetInterfaceMessage( 998 );
		}
	case ePetType_Support:
		{
			return GetInterfaceMessage( 999 );
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), type );

			return text;
		}
	}
}


// 080716 LUJ: 펫 상태 문자열을 반환한다
CString CclientApp::GetText( ePetState state ) const
{
	switch( state )
	{
	case ePetState_None:
		{
			CString text;
			text.LoadString( IDS_STRING203 );

			return text;
		}
	case ePetState_Summon:
		{
			return GetInterfaceMessage( 790 );
		}
	case ePetState_Die:
		{
			return GetInterfaceMessage( 791 );
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), state );

			return text;
		}
	}
}


// 080716 LUJ, 펫 로그 문자열 반환
CString CclientApp::GetText( ePetLog log ) const
{
	switch( log )
	{
	case ePetLogSummon:
		{
			CString textSummon;
			textSummon.LoadString( IDS_STRING400 );

			return textSummon;
		}
	case ePetLogDie:
		{
			CString textDie;
			textDie.LoadString( IDS_STRING240 );

			return textDie;
		}
	case ePetLogRevive:
		{
			CString textRevive;
			textRevive.LoadString( IDS_STRING401 );
			
			return textRevive;
		}
	case ePetLogChangeGrade:
		{
			CString textChange;
			textChange.LoadString( IDS_STRING295 );
			CString textGrade;
			textGrade.LoadString( IDS_STRING395 );

			return textChange + _T( ": " ) + textGrade;
		}
	case ePetLogChangeLevel:
		{
			CString textChange;
			textChange.LoadString( IDS_STRING295 );
			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			return textChange + _T( ": " ) + textLevel;
		}
	case ePetLogRegular:
		{
			CString	textRegular;
			textRegular.LoadString( IDS_STRING256 );

			return textRegular;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}

CString CclientApp::GetText( eSiegeLog log ) const
{
	switch( log )
	{
	case eSiegeLogChangeState:
		{
			CString textChange;
			textChange.LoadString( IDS_STRING295 );
			CString textStatus;
			textStatus.LoadString( IDS_STRING399 );

			return textChange + _T( ": " ) + textStatus;
		}
	case eSiegeLogDestoryCatapult:
		{
			CString textDestroy;
			textDestroy.LoadString( IDS_STRING240 );
			CString textCatapult;
			textCatapult.LoadString( IDS_STRING415 );

			return textDestroy + _T( ": " ) + textCatapult;
		}
	case eSiegeLogGetCastle:
		{
			CString textGet;
			textGet.LoadString( IDS_STRING24 );
			CString textCastle;
			textCastle.LoadString( IDS_STRING416 );

			return textGet + _T( ": " ) + textCastle;
		}
	case eSiegeLogUseSeed:
		{
			CString textUse;
			textUse.LoadString( IDS_STRING270 );
			CString textWaterseed;
			textWaterseed.LoadString( IDS_STRING417 );

			return textUse + _T( ": " ) + textWaterseed;
		}
	case eSiegeLogSetSeedSucceed:
		{
			CString textWaterSeed;
			textWaterSeed.LoadString( IDS_STRING417 );
			CString textInstall;
			textInstall.LoadString( IDS_STRING418 );
			
			return textInstall + _T( ": " ) + textWaterSeed;
		}
	case eSiegeLogSetSeedFailed:
		{
			CString textWaterSeed;
			textWaterSeed.LoadString( IDS_STRING417 );
			CString textUse;
			textUse.LoadString( IDS_STRING270 );
			CString textFailure;
			textFailure.LoadString( IDS_STRING266 );

			return textFailure + _T( ": " ) + textWaterSeed + _T( " " ) + textUse;
		}
	case eSiegeLogComplete:
		{
			CString textComplete;
			textComplete.LoadString( IDS_STRING188 );

			return textComplete;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}

CString CclientApp::GetText( eAutoNoteLogKind log ) const
{
	switch( log )
	{
	case eAutoNoteLogKind_Report:
		{
			CString textReport;
			textReport.LoadString( IDS_STRING445 );

			return textReport;
		}
	case eAutoNoteLogKind_Regist:
		{
			CString textRegist;
			textRegist.LoadString( IDS_STRING446 );

			return textRegist;
		}
	case eAutoNoteLogKind_Ban:
		{
			CString textBan;
			textBan.LoadString( IDS_STRING447 );

			return textBan;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}

void CclientApp::SetJobCombo( CComboBox& comboBox, DWORD jobType, DWORD race, DWORD jobLevel, DWORD jobIndex )
{
	comboBox.ResetContent();

	comboBox.AddString( _T( "..." ) );
	
	for( DWORD index = 1; index < 9; ++index )
	{
		const DWORD code	= jobType * 1000 + ( race + 1 ) * 100 + jobLevel * 10 + index;

		const CString& name = CclientApp::GetJobName( code );

		if( name.IsEmpty() )
		{
			break;
		}

		comboBox.AddString( name );
	}

	comboBox.SetCurSel( jobIndex );
}


const TCHAR* CclientApp::GetCropName( DWORD cropIndex, DWORD cropLevel ) const
{
	FarmCropMap::const_iterator it = mFarmCropMap.find( MAKELONG( cropIndex, cropLevel ) );

	return mFarmCropMap.end() == it ? _T( "" ) : it->second;
}


// 080630 LUJ, 가축 이름을 반환한다
const TCHAR* CclientApp::GetLivestockName( DWORD kind ) const
{
	LivestockMap::const_iterator it = mLivestockMap.find( kind );

	return mLivestockMap.end() == it ? _T( "" ) : it->second;
}


void CclientApp::LoadFarmScript()
{
	{
		CMHFile file;

		if( ! file.Init("System/Resource/FarmRenderObjName.bin", "rb") )
		{
			ASSERT( 0 );
			return;
		}

		char buffer[ MAX_PATH * 5 ];

		int index = 0;
		int level = 0;

		CString line;

		while( ! file.IsEOF() )
		{
			file.GetLine( buffer, sizeof( buffer ) );

			line = buffer;

			const char* separator	= "\t ";
			const char* token		= strtok( buffer, separator );

			if( !	token )
			{
				++index;
				level = 0;
				continue;
			}
			else if( ! strcmpi( "//", token ) )
			{
				continue;
			}

			mFarmCropMap.insert( std::make_pair( MAKELONG( index, ++level ), line ) );
		}
	}	

	// 080630 LUJ, 가축 이름을 읽어들인다
	{
		CMHFile file;

		if( ! file.Init("System/Resource/farm.bin", "rb") )
		{
			ASSERT( 0 );
			return;
		}

		char buffer[ MAX_PATH * 5 ];

		bool isLivestockName = false;

		while( ! file.IsEOF() )
		{
			file.GetLine( buffer, sizeof( buffer ) );

			CString line( buffer );

			// 080630 LUJ, 스크립트만 보면 어디부터 가축 이름인지 구별할 수 없다.
			//				부득이 주석을 읽어 구분한다
			if( line == _T( "// 가축 이름" ) )
			{
				isLivestockName = true;
				continue;
			}
			// 080630 LUJ, 가축 이름을 읽기 시작한 상태에서만 아래 코드를 실행해야 한다
			else if( ! isLivestockName )
			{
				continue;
			}
			// 080630 LUJ, 가축 이름을 다 읽어들인 상태
			else if( line == _T( "END_KIND" ) )
			{
				break;
			}

			// 080630 LUJ, 읽는 순서가 번호가 된다
			mLivestockMap.insert( std::make_pair( DWORD( mLivestockMap.size() ), line ) );
		}
	}
}


void CclientApp::SetUserLevelCombo( CComboBox& combo ) const
{
	combo.ResetContent();

	CString textGod;
	textGod.LoadString( IDS_STRING296 );

	CString textProgrammer;
	textProgrammer.LoadString( IDS_STRING297 );

	CString textDeveloper;
	textDeveloper.LoadString( IDS_STRING298 );

	CString textManager;
	textManager.LoadString( IDS_STRING299 );

	CString textSuperUser;
	textSuperUser.LoadString( IDS_STRING300 );

	CString textUser;
	textUser.LoadString( IDS_STRING301 );

	CString textSubUser;
	textSubUser.LoadString( IDS_STRING302 );

	CString textBlockedUser;
	textBlockedUser.LoadString( IDS_STRING303 );

	combo.AddString( textGod );
	combo.AddString( textProgrammer );
	combo.AddString( textDeveloper );
	combo.AddString( textManager );
	combo.AddString( textSuperUser );
	combo.AddString( textUser );
	combo.AddString( textSubUser );
	combo.AddString( textBlockedUser );
}


DWORD CclientApp::GetItemIndex( const TCHAR* name ) const
{
	for( ItemMap::const_iterator it = mItemMap.begin(); mItemMap.end() != it; ++it )
	{
		const ItemScript& item = it->second;

		if( ! _tcscmp( item.mUnicodeName, name ) )
		{
			return item.ItemIdx;
		}
	}

	return 0;
}


void CclientApp::SetItemCombo( CComboBox& combo, const TCHAR* keyword ) const
{
	combo.ResetContent();

	for(
		ItemMap::const_iterator it = mItemMap.begin();
		mItemMap.end() != it;
		++it )
	{
		const ItemScript& item = it->second;

		const std::wstring name( item.mUnicodeName );

		if( std::wstring::npos != name.find( keyword ) )
		{
			TCHAR buffer[ MAX_PATH ] = { 0 };
			_stprintf(
				buffer,
				L"%d, %s",
				item.ItemIdx,
				item.mUnicodeName );
			combo.AddString( buffer );
		}
	}

	// 080401 LUJ, 정확히 일치한 것을 택한다
	combo.SetCurSel( max( 0, combo.FindString( 0, keyword ) ) );
}


CString CclientApp::GetItemPositionType( Item::Area area, POSTYPE position ) const
{
	switch( area )
	{
	case Item::AreaInventory:
		{
			if( TP_WEAR_START	<= position	&&
				TP_WEAR_END	> position )
			{
				CString textEquippment;
				textEquippment.LoadString( IDS_STRING53 );

				return textEquippment;
			}
			else if(	TP_INVENTORY_START	<= position	&&
						TP_INVENTORY_END	> position )
			{
				CString textInventory;
				textInventory.LoadString( IDS_STRING281 );

				return textInventory;
			}
			else if(	TP_EXTENDED_INVENTORY1_START	<= position	&&
						TP_EXTENDED_INVENTORY1_END		> position )
			{
				CString textExtendedInventory;
				textExtendedInventory.LoadString( IDS_STRING304 );

				return textExtendedInventory + _T( " 1" );
			}
			else if(	TP_EXTENDED_INVENTORY2_START	<= position	&&
						TP_EXTENDED_INVENTORY2_END		> position )
			{
				CString textExtendedInventory;
				textExtendedInventory.LoadString( IDS_STRING304 );

				return textExtendedInventory + _T( " 2" );
			}

			return _T( "?" );
		}
	case Item::AreaCashStorage:
		{
			CString textCashStorage;
			textCashStorage.LoadString( IDS_STRING283 );

			return textCashStorage;
		}
	case Item::AreaPrivateStorage:
		{
			CString textStroage;
			textStroage.LoadString( IDS_STRING282 );

			return textStroage;
		}
		// 080716 LUJ, 펫 인벤토리
	case Item::AreaPet:
		{
			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			return textPet;
		}
	default:
		{
			return _T( "?" );
		}
	}	
}


CString CclientApp::GetText( eGM_POWER power ) const
{
	switch( power )
	{
	case eGM_POWER_MASTER:
		{
			CString textManager;
			textManager.LoadString( IDS_STRING307 );

			return textManager;
		}
	case eGM_POWER_MONITOR:
		{
			CString textMonitor;
			textMonitor.LoadString( IDS_STRING305 );

			return textMonitor;
		}
	case eGM_POWER_PATROLLER:
		{
			CString textStaff;
			textStaff.LoadString( IDS_STRING306 );

			return textStaff;
		}
	case eGM_POWER_AUDITOR:
		{
			return _T( "Auditor" );
		}
	case eGM_POWER_EVENTER:
		{
			return _T( "Eventer" );
		}
	case eGM_POWER_QA:
		{
			return _T( "QA" );
		}
	default:
		{
			return _T( "?" );
		}
	}
}


CString CclientApp::GetText( RecoveryLog log ) const
{
	switch( log )
	{
	case RecoveryLogSetPlayer:
		{
			CString textPlayerWasChanged;
			textPlayerWasChanged.LoadString( IDS_STRING309 );

			return textPlayerWasChanged;
		}
	case RecoveryLogSetExtendedPlayer:
		{
			CString textPlayerExtendedWasUpdated;
			textPlayerExtendedWasUpdated.LoadString( IDS_STRING311 );

			return textPlayerExtendedWasUpdated;
		}
	case RecoveryLogSetLicense:
		{
			CString textPlayerMatchingWasUpdated;
			textPlayerMatchingWasUpdated.LoadString( IDS_STRING312 );

			return textPlayerMatchingWasUpdated;
		}
	case RecoveryLogSetItem:
		{
			CString textItemWasUpdated;
			textItemWasUpdated.LoadString( IDS_STRING313 );

			return textItemWasUpdated;
		}
	case RecoveryLogSetItemOption:
		{
			CString textItemOptionWasUpdated;
			textItemOptionWasUpdated.LoadString( IDS_STRING314 );

			return textItemOptionWasUpdated;
		}
	case RecoveryLogRemoveItem:
		{
			CString textItemWasRemoved;
			textItemWasRemoved.LoadString( IDS_STRING315 );

			return textItemWasRemoved;
		}
	case RecoveryLogRemoveItemOption:
		{
			CString textItemOptionWasRemoved;
			textItemOptionWasRemoved.LoadString( IDS_STRING316 );

			return textItemOptionWasRemoved;
		}
	case RecoveryLogAddItem:
		{
			CString textItemWasAdded;
			textItemWasAdded.LoadString( IDS_STRING317 );

			return textItemWasAdded;
		}
	case RecoveryLogUpdateItemEndTime:
		{
			CString textItemEndTimeWasUpdated;
			textItemEndTimeWasUpdated.LoadString( IDS_STRING318 );

			return textItemEndTimeWasUpdated;
		}
	case RecoveryLogUpdateItemRemainTime:
		{
			CString textItemRemainedTimeWasUpdated;
			textItemRemainedTimeWasUpdated.LoadString( IDS_STRING319 );

			return textItemRemainedTimeWasUpdated;
		}
	case RecoveryLogSetItemStorage:
		{
			CString textStorageWasUpdated;
			textStorageWasUpdated.LoadString( IDS_STRING320 );

			return textStorageWasUpdated;
		}
	case RecoveryLogRemoveItemdDropOption:
		{
			CString textDropOption;
			textDropOption.LoadString( IDS_STRING354 );

			CString textDelete;
			textDelete.LoadString( IDS_STRING253 );

			return textDropOption + _T( ": " ) + textDelete;
		}
	case RecoveryLogAddSkill:
		{
			CString textSkillWasAdded;
			textSkillWasAdded.LoadString( IDS_STRING321 );

			return textSkillWasAdded;
		}
	case RecoveryLogRemoveSkill:
		{
			CString textSkillWasRemoved;
			textSkillWasRemoved.LoadString( IDS_STRING322 );

			return textSkillWasRemoved;
		}
	case RecoveryLogSetSkill:
		{
			CString textSkillWasUpdated;
			textSkillWasUpdated.LoadString( IDS_STRING323 );

			return textSkillWasUpdated;
		}
	case RecoveryLogSetFamily:
		{
			CString textFamilyWasUpdated;
			textFamilyWasUpdated.LoadString( IDS_STRING324 );

			return textFamilyWasUpdated;
		}
	case RecoveryLogRemoveQuest:
		{
			CString textQuestWasRemoved;
			textQuestWasRemoved.LoadString( IDS_STRING325 );

			return textQuestWasRemoved;
		}
	case RecoveryLogFinishSubQuest:
		{
			CString textQuestWasRemoved;
			textQuestWasRemoved.LoadString( IDS_STRING326 );

			return textQuestWasRemoved;
		}
	case RecoveryLogSetQuestItem:
		{
			CString textQuestItemWasUpdated;
			textQuestItemWasUpdated.LoadString( IDS_STRING327 );

			return textQuestItemWasUpdated;
		}
	case RecoveryLogRemoveQuestItem:
		{
			CString textQuestItemWasRemoved;
			textQuestItemWasRemoved.LoadString( IDS_STRING328 );

			return textQuestItemWasRemoved;
		}
	case RecoveryLogSetGuild:
		{
			CString textGuildWasUpdated;
			textGuildWasUpdated.LoadString( IDS_STRING329 );

			return textGuildWasUpdated;
		}
	case RecoveryLogSetGuildRank:
		{
			CString textGuildRankWasUpdated;
			textGuildRankWasUpdated.LoadString( IDS_STRING330 );

			return textGuildRankWasUpdated;
		}
	case RecoveryLogKickGuildMember:
		{
			CString textGuildMemberWasKicked;
			textGuildMemberWasKicked.LoadString( IDS_STRING331 );

			return textGuildMemberWasKicked;
		}
		// 080425 LUJ, 길드 스킬 삭제
	case RecoveryLogRemoveGuildSkill:
		{
			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );
			CString textSkill;
			textSkill.LoadString( IDS_STRING365 );
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			return textGuild + textSkill + _T( ": " ) + textRemove;
		}
	case RecoveryLogSetUser:
		{
			CString textUseWasUpdated;
			textUseWasUpdated.LoadString( IDS_STRING332 );

			return textUseWasUpdated;
		}
	case RecoveryLogAddPlayerToUser:
		{
			CString textPlayerWasAdded;
			textPlayerWasAdded.LoadString( IDS_STRING333 );

			return textPlayerWasAdded;
		}
	case RecoveryLogRemovePlayerFromUser:
		{
			CString textPlayerWasRemoved;
			textPlayerWasRemoved.LoadString( IDS_STRING334 );

			return textPlayerWasRemoved;
		}
	case RecoveryLogRestorePlayerToUser:
		{
			CString textPlayerWasRecovered;
			textPlayerWasRecovered.LoadString( IDS_STRING335 );

			return textPlayerWasRecovered;
		}
	case RecoveryPermissionAddIp:
		{
			CString textConnectableIpWasAdded;
			textConnectableIpWasAdded.LoadString( IDS_STRING336 );

			return textConnectableIpWasAdded;
		}
	case RecoveryPermissionRemoveIp:
		{
			CString textConnectableIpWasRemoved;
			textConnectableIpWasRemoved.LoadString( IDS_STRING337 );

			return textConnectableIpWasRemoved;
		}
	case RecoveryOperatorAdd:
		{
			CString textOperatorWasAdded;
			textOperatorWasAdded.LoadString( IDS_STRING338 );

			return textOperatorWasAdded;
		}
	case RecoveryOperatorRemove:
		{
			CString textOperatorWasRemoved;
			textOperatorWasRemoved.LoadString( IDS_STRING339 );

			return textOperatorWasRemoved;
		}
	case RecoveryOperatorSet:
		{
			CString textOperatorWasUpdated;
			textOperatorWasUpdated.LoadString( IDS_STRING340 );

			return textOperatorWasUpdated;
		}
	case RecoveryOperatorAddIp:
		{
			CString textOperatorIpWasAdded;
			textOperatorIpWasAdded.LoadString( IDS_STRING341 );

			return textOperatorIpWasAdded;
		}
	case RecoveryOperatorRemoveIp:
		{
			CString textOperatorIpWasRemoved;
			textOperatorIpWasRemoved.LoadString( IDS_STRING342 );

			return textOperatorIpWasRemoved;
		}
		// 080526 LUJ, 낚시 변경
	case RecoveryLogSetFishing:
		{
			CString textFishing;
			textFishing.LoadString( IDS_STRING379 );

			CString textChange;
			textChange.LoadString( IDS_STRING264 );

			return textFishing + _T( ": " ) + textChange;
		}
		// 080805 LUJ, 펫 추가
	case RecoveryLogAddPet:
		{
			CString textAdd;
			textAdd.LoadString( IDS_STRING252 );

			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			return textPet + _T( ": " ) + textAdd;
		}
		// 080805 LUJ, 펫 삭제
	case RecoveryLogRemovePet:
		{
			CString textRemove;
			textRemove.LoadString( IDS_STRING253 );

			CString textPet;
			textPet.LoadString( IDS_STRING280 );

			return textPet + _T( ": " ) + textRemove;
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}

// 090826 ShinJS --- 돈 로그 Text 추가
CString CclientApp::GetText( eMoneyLogtype log ) const
{
	switch( log )
	{
	case eMoneyLog_GetStall:
		{
			CString text;
			text.LoadString( IDS_STRING449 );
			return text;
		}
		break;
	case eMoneyLog_GetStreetStall:
		{
			CString text;
			text.LoadString( IDS_STRING450 );
			return text;
		}
		break;
	case eMoneyLog_GetMonster:
		{
			CString text;
			text.LoadString( IDS_STRING451 );
			return text;
		}
		break;
	case eMoneyLog_GetExchange:
		{
			CString text;
			text.LoadString( IDS_STRING452 );
			return text;
		}
		break;
	case eMoneyLog_GetStorage:
		{
			CString text;
			text.LoadString( IDS_STRING453 );
			return text;
		}
		break;
	case eMoneyLog_GetPKLooting:
		{
			CString text;
			text.LoadString( IDS_STRING454 );
			return text;
		}
		break;
	case eMoneyLog_GetPrize:
		{
			CString text;
			text.LoadString( IDS_STRING455 );
			return text;
		}
		break;
	case eMoneyLog_GetSWProfit:
		{
			CString text;
			text.LoadString( IDS_STRING456 );
			return text;
		}
		break;
	case eMoneyLog_GetFromQuest:
		{
			CString text;
			text.LoadString( IDS_STRING457 );
			return text;
		}
		break;
	case eMoneyLog_GetGMTool:
		{
			CString text;
			text.LoadString( IDS_STRING458 );
			return text;
		}
		break;
	case eMoneyLog_GetMonstermeter:
		{
			CString text;
			text.LoadString( IDS_STRING459 );
			return text;
		}
		break;
	case eMoneyLog_LoseStall:
		{
			CString text;
			text.LoadString( IDS_STRING460 );
			return text;
		}
		break;
	case eMoneyLog_LoseStreetStall:
		{
			CString text;
			text.LoadString( IDS_STRING461 );
			return text;
		}
		break;
	case eMoneyLog_LoseDie:
		{
			CString text;
			text.LoadString( IDS_STRING462 );
			return text;
		}
		break;
	case eMoneyLog_LoseExchange:
		{
			CString text;
			text.LoadString( IDS_STRING463 );
			return text;
		}
		break;
	case eMoneyLog_LoseStorage:
		{
			CString text;
			text.LoadString( IDS_STRING464 );
			return text;
		}
		break;
	case eMoneyLog_LoseStorageBuy:
		{
			CString text;
			text.LoadString( IDS_STRING465 );
			return text;
		}
		break;
	case eMoneyLog_LosePKLooted:
		{
			CString text;
			text.LoadString( IDS_STRING466 );
			return text;
		}
		break;
	case eMoneyLog_LoseGMTool:
		{
			CString text;
			text.LoadString( IDS_STRING467 );
			return text;
		}
		break;
	case eMoneyLog_LoseFarmBuy:
		{
			CString text;
			text.LoadString( IDS_STRING468 );
			return text;
		}
		break;
	case eMoneyLog_LoseFarmUpgrade:
		{
			CString text;
			text.LoadString( IDS_STRING469 );
			return text;
		}
		break;
	case eMoneyLog_LoseFarmTax:
		{
			CString text;
			text.LoadString( IDS_STRING470 );
			return text;
		}
		break;
	case eMoneyLog_GetCheatMoney:
		{
			CString text;
			text.LoadString( IDS_STRING472 );
			return text;
		}
		break;
	default:
		{
			CString text;
			text.Format( _T( "%d" ), log );

			return text;
		}
	}
}

CString CclientApp::GetText(eHouseLog log) const
{
	switch(log)
	{
	case eHouseLog_Keep:
		{
			CString text;
			text.LoadString(
				IDS_STRING474);
			return text;
		}
	case eHouseLog_UnKepp:
		{
			CString text;
			text.LoadString(
				IDS_STRING475);
			return text;
		}
	case eHouseLog_Install:
		{
			CString text;
			text.LoadString(
				IDS_STRING418);
			return text;
		}
	case eHouseLog_UnInstall:
		{
			CString text;
			text.LoadString(
				IDS_STRING476);
			return text;
		}
	case eHouseLog_EndTime:
		{
			CString text;
			text.LoadString(
				IDS_STRING292);
			return text;
		}
	case eHouseLog_Destroy:
		{
			CString text;
			text.LoadString(
				IDS_STRING253);
			return text;
		}
	case eHouseLog_WallPaper:
	case eHouseLog_FloorPaper:
	case eHouseLog_CeillingPaper:
	case eHouseLog_Extend:
		{
			CString text;
			text.LoadString(
				IDS_STRING270);
			return text;
		}
	case eHouseLog_ChangeHouseName:
		{
			CString text;
			text.LoadString(
				IDS_STRING295);
			return text;
		}
	default:
		{
			CString text;
			text.Format(
				_T( "%d"),
				log);
			return text;
		}
	}
}

CString CclientApp::GetText(eCookingLog log) const
{
	switch(log)
	{
	case eCookingLog_CookCount:
		{
			CString text;
			text.LoadString(
				IDS_STRING483);
			return text;
		}
	case eCookingLog_EatCount:
		{
			CString text;
			text.LoadString(
				IDS_STRING484);
			return text;
		}
	case eCookingLog_FireCount:
		{
			CString text;
			text.LoadString(
				IDS_STRING485);
			return text;
		}
	case eCookingLog_RecipeReg:
		{
			CString text;
			text.LoadString(
				IDS_STRING486);
			return text;
		}
	case eCookingLog_RecipeDel_by_Time:
		{
			CString text;
			text.LoadString(
				IDS_STRING487);
			CString textTime;
			textTime.LoadString(
				IDS_STRING102);
			return text;
		}
	case eCookingLog_RecipeDel_by_User:
		{
			CString text;
			text.LoadString(
				IDS_STRING487);
			CString textUser;
			textUser.LoadString(
				IDS_STRING301);
			return text;
		}
	default:
		{
			CString text;
			text.Format(
				_T( "%d"),
				log);
			return text;
		}
	}
}

CString CclientApp::GetText(eCOOKRECIPE type) const
{
	switch(type)
	{
	case eCOOKRECIPE_ADD:
		{
			CString text;
			text.LoadString(
				IDS_STRING252);

			return text;
		}
	case eCOOKRECIPE_DEL:
		{
			CString text;
			text.LoadString(
				IDS_STRING252);

			return text;
		}
	case eCOOKRECIPE_UPDATE:
		{
			CString text;
			text.LoadString(
				IDS_STRING252);

			return text;
		}
	default:
		{
			CString text;
			text.Format(
				_T("%d"),
				type);
			return text;
		}
	}
}

// 080304 LUJ, 로그를 표시한다
void CclientApp::Log( const TCHAR* text, ... )
{
	CMainFrame* frame = dynamic_cast< CMainFrame* >( m_pMainWnd );

	if( ! frame )
	{
		ASSERT( 0 );
		return;
	}

	TCHAR buffer[ MAX_PATH * 5 ] = { 0 };
	{
		va_list argument;
		va_start( argument, text );
		_vstprintf( buffer, text, argument);
		va_end( argument);
	}

	const CTime time( CTime::GetCurrentTime() );

	CString log;

	log.Format(
		_T( "[%04d.%02d.%02d %02d:%02d:%02d] %s\r\n" ),
		time.GetYear(),
		time.GetMonth(),
		time.GetDay(),
		time.GetHour(),
		time.GetMinute(),
		time.GetSecond(),
		buffer );

	frame->Put( log );

	// 파일 출력
	{
		// 080702 LUJ, log 디렉토리에 로그 파일을 저장하도록 한다
		::CreateDirectory( _T( "log" ), 0 );

		CString name;
		{
			const CString prefix( _T( "log\\RecoveryClient" ) );
			CString infix;
			infix.Format(
				_T( "%04d%02d%02d" ),
				time.GetYear(),
				time.GetMonth(),
				time.GetDay() );
			const CString postfix( _T( "log" ) );

			name = prefix + _T( "." ) + infix + _T( "." ) + postfix;
		}		

		CStdioFileEx file;

		// 쓰기 우선으로 열되, 없으면 생성한다
		if( !	file.Open( name, CFile::modeWrite  ) &&
			!	file.Open( name, CFile::modeCreate | CFile::modeWrite ) )
		{
			return;
		}

		file.Seek( file.GetLength(), 0 );
        file.WriteString( log );
	}
}


// 080328 LUJ, 로그 저장하지 않는 화면 출력
void CclientApp::Put( const TCHAR* text, ... )
{
	CMainFrame* frame = dynamic_cast< CMainFrame* >( m_pMainWnd );

	if( ! frame )
	{
		ASSERT( 0 );
		return;
	}

	TCHAR buffer[ MAX_PATH * 5 ] = { 0 };
	{
		va_list argument;
		va_start( argument, text );
		_vstprintf( buffer, text, argument);
		va_end( argument);
	}

	frame->Put( CString( buffer ) + _T( "\n" ) );
}


// 080401 LUJ, 멀티바이트 문자열에 포함된 특수한 문자열을 제거한다
CString CclientApp::RemoveString( const char* source, const char* extractedString ) const
{
	char name[ MAX_PATH ] = { 0 };

	const char* begin	= source;

	for(
		const char* character = source;
		*character;
		++character )
	{
		const char* space = "^s";

		// 080328 LUJ, 공백이 발견되면 실제 공백으로 대치하고 복사한다
		if( ! strncmp( character, space, strlen( space ) ) )
		{
			char facet[ MAX_PATH ] = { 0 };

			if( sizeof( facet ) < strlen( begin ) )
			{
				break;
			}

			strncpy( facet, begin, character - begin );

			sprintf(
				name + strlen( name ),
				"%s ",
				facet );

			begin = character + strlen( space );
		}
	}

	char buffer[ MAX_PATH ] = { 0 };

	if( sizeof( buffer ) < strlen( name ) + strlen( begin ) )
	{
		return _T( "" );
	}

	sprintf(
		buffer,
		"%s%s",
		name,
		begin );

	return CString( CA2WEX< sizeof( buffer ) >( buffer ) );
}

 
// 080630 LUJ, 스크립트를 로딩한다. 스레드로 처리되어, 로그인 창에 로딩 상태를 표시해준다
UINT CclientApp::LoadScript( LPVOID parameter )
{
	if( ! parameter )
	{
		return 1;
	}

	CclientApp&		application = *( static_cast< CclientApp* >( parameter ) );
	CLoginDialog*	dialog		= application.mLoginDialog;

	if( ! dialog )
	{
		return 1;
	}

	typedef void ( CclientApp::*Function )();
	typedef std::pair< Function, CString >	Event;
	typedef std::list< Event >				EventList;
	EventList								eventList;
	{
		eventList.push_back( std::make_pair( &LoadItemScript,			_T( "... ItemList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadEnchantScript,		_T( "... ItemEnchant.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadActiveSkillScript,	_T( "... SkillList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadQuestScript,			_T( "... QuestScript.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadQuestItemScript,		_T( "... QuestItemList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadQuestStringScript,	_T( "... QuestString.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadInterfaceMessage,		_T( "... InterfaceMsg.bin " ) ) );
		eventList.push_back( std::make_pair( &LoadLicenseScript,		_T( "... ResidentRegist.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadTip,					_T( "... ToolTipMsg.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadFarmScript,			_T( "... FarmRenderObjName.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadPetScript,			_T( "... PetList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadPetStatusScript,		_T( "... PetStatusInfot.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadPetBodyScript,		_T( "... PetHPMPInfo.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadPetSystemScript,		_T( "... PetSystemInfo.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadMonsterScript,		_T( "... MonsterList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadFurnitureScript,		_T( "... Housing_FunitureList.bin" ) ) );
		eventList.push_back( std::make_pair( &LoadCookScript,			_T( "... Cooking.bin" ) ) );
	}

	int step = 0;

	{
		CWnd* mainWindow = application.m_pMainWnd;

		if( ! mainWindow )
		{
			return 1;
		}

		CNetwork& network = CNetwork::GetInstance();

		dialog->SetProgressValue( ++step, _T( "... connecting" ) );

		if( ! network.Connect() )
		{
			CString textAnyConnectableServerDoNotExist;
			textAnyConnectableServerDoNotExist.LoadString( IDS_STRING137 );
			::MessageBox( 0, textAnyConnectableServerDoNotExist, _T( "" ), MB_ICONERROR | MB_OK );

			mainWindow->SendMessage( WM_CLOSE );
			return 1;
		}
	}

	{
		// 080716 LUJ, 리소스 로딩외에 다음 동작이 이벤트로 처리됨: ①접속 ②준비
		const int otherEventSize = 2;

		dialog->SetProgressMax( otherEventSize + int( eventList.size() ) );
	}
	
	for(
		EventList::const_iterator it = eventList.begin();
		eventList.end() != it;
		++it )
	{
		const Function	function	= it->first;
		const CString&	name		= it->second;

		dialog->SetProgressValue( ++step, name );
		// 080716 LUJ, Modern C++ Design 한국판 p199~205
		( application.*function )();
	}

	dialog->SetProgressValue( ++step, CString( __DATE__ ) + _T( " " ) + CString( __TIME__ ) );	
	// 080714 LUJ, http://blogs.msdn.com/oldnewthing/archive/2004/08/02/205624.aspx
	dialog->GotoDlgCtrl( dialog);
	return 0;
}

LPCTSTR CclientApp::GetPetGradeText( DWORD grade ) const
{
	switch( grade )
	{
	case 0:
		{
			return GetInterfaceMessage( 1000 );
		}
	case 1:
		{
			return GetInterfaceMessage( 1001 );
		}
	case 2:
		{
			return GetInterfaceMessage( 1002 );
		}
	case 3:
		{
			return GetInterfaceMessage( 1003 );
		}
	default:
		{
			// 080731 LUJ, 문자열이 정의되어 있지 않음을 판단하기 위해 빈 문자열을 반환
			return _T( "" );
		}
	}
}


LPCTSTR CclientApp::GetPetAritificialIntelligenceText( DWORD artificialIntelligence ) const
{
	switch( artificialIntelligence )
	{
	case 0:
		{
			return GetInterfaceMessage( 990 );
		}
	case 1:
		{
			return GetInterfaceMessage( 991 );
		}
	case 2:
		{
			return GetInterfaceMessage( 992 );
		}
	default:
		{
			CString text;
			text.Format( _T( "%d" ), artificialIntelligence );

			return text;
		}
	}
}


// 080716 LUJ, 펫 이름을 읽는다
void CclientApp::LoadPetScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/PetList.bin", "rb"))
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* separator	= "\t";
		const char* index		= strtok( text, separator );
		const char* name		= strtok( 0, separator );

		if( ! index ||
			! name )
		{
			continue;
		}

		mPetNameMap.insert(
			std::make_pair( atoi( index ), RemoveString( name, "^s" ) ) );
	}
}

LPCTSTR	CclientApp::GetPetName( DWORD index ) const
{
	const PetNameMap::const_iterator it = mPetNameMap.find( index );

	return mPetNameMap.end() == it ? _T( "" ) : it->second;
}


// 080716 LUJ, 몬스터 이름을 읽는다
void CclientApp::LoadMonsterScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/MonsterList.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* separator	= "\t";
		const char* index		= strtok( text, separator );
		const char* name		= strtok( 0, separator );

		if( ! index ||
			! name )
		{
			continue;
		}

		mMonsterNameMap.insert(
			std::make_pair( atoi( index ), RemoveString( name, "^s" ) ) );
	}
}

LPCTSTR CclientApp::GetMonsterName( DWORD kind ) const
{
	const MonsterNameMap::const_iterator it = mMonsterNameMap.find( kind );

	return mMonsterNameMap.end() == it ? _T( "?" ) : it->second;
}


// 080731 LUJ, 펫 상태 정보를 읽는다
void CclientApp::LoadPetStatusScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/PetStatusInfo.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* separator		= "\t";
		const char* type			= strtok( text, separator );
		const char* level			= strtok( 0, separator );
		const char* strength		= strtok( 0, separator );
		const char* dexterity		= strtok( 0, separator );
		const char* vitality		= strtok( 0, separator );
		const char* intelligence	= strtok( 0, separator );
		const char* wisdom			= strtok( 0, separator );

		if( ! type			||
			! level			||
			! strength		||
			! dexterity		||
			! vitality		||
			! intelligence	||
			! wisdom )
		{
			continue;
		}

		{
			PetTypeSet& typeSet = mPetStatusMap[ atoi( level ) ];

			typeSet.insert( ePetType( atoi( type ) ) );
		}

		{
			const PetStatusKey key( ePetType( atoi( type ) ), atoi( level ) );

			PetStatusScript& script = mPetStatusScriptMap[ key ];
			ZeroMemory( &script, sizeof( script ) );
			script.mStrength		= atoi( strength );
			script.mDexterity		= atoi( dexterity );
			script.mVitality		= atoi( vitality );
			script.mWisdom			= atoi( wisdom );
			script.mIntelligence	= atoi( intelligence );
		}
	}
}


// 080731 LUJ, 레벨에 따라 가질 수 있는 타입을 반환
const CclientApp::PetTypeSet& CclientApp::GetPetType( DWORD level ) const
{
	const PetStatusMap::const_iterator it = mPetStatusMap.find( level );

	if( mPetStatusMap.end() == it )
	{
		static PetTypeSet typeSet;

		return typeSet;
	}

	return it->second;
}


// 080731 LUJ, 펫 HP/MP 정보를 읽는다
void CclientApp::LoadPetBodyScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/PetHPMPInfo.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* separator	= "\t";
		const char* index		= strtok( text, separator );
		const char* life		= strtok( 0, separator );
		const char* mana		= strtok( 0, separator );

		if( ! index ||
			! life	||
			! mana )
		{
			continue;
		}

		PetBodyScript& body = mPetBodyMap[ atoi( index ) ];
		ZeroMemory( &body, sizeof( body ) );

		body.mHealth	= atoi( life );
		body.mMana		= atoi( mana );
	}
}


// 080731 LUJ, 펫 종류/등급에 따라 정보를 반환한다
const CclientApp::PetBodyScript& CclientApp::GetPetBodyScript( ePetType type, DWORD grade ) const
{
	const DWORD index = int( type ) * 10 + grade;

	const PetBodyMap::const_iterator it = mPetBodyMap.find( index );

	if( mPetBodyMap.end() == it )
	{
		static PetBodyScript body;

		return body;
	}

	return it->second;
}


// 080731 LUJ, 도움말을 위해 유효한 펫 정보 조건을 문자열로 반환한다.
void CclientApp::GetPetBodyConditionText( CString& conditionText ) const
{
	CString textType;
	textType.LoadString( IDS_STRING4 );
	CString	textGrade;
	textGrade.LoadString( IDS_STRING395 );

	conditionText += CString( _T( "\t" ) ) + textGrade + _T( " ... " ) + textType + _T( "\n" );
	conditionText += _T( "\t---------------------\n" );

	for(
		PetBodyMap::const_iterator it = mPetBodyMap.begin();
		mPetBodyMap.end() != it;
		++it )
	{
		const DWORD		index	=			it->first;
		const DWORD		grade	=			index % 10;
		const ePetType	type	= ePetType( index / 10 );
		
		CString text;
		text.Format(
			_T( "\t%s(%d) ... %s(%d)\n" ),
			GetPetGradeText( grade ),
			grade,
			GetText( type ),
			type );

		conditionText += text;
	}
}


// 080731 LUJ, 펫 설정 정보를 반환한다
const CclientApp::PetSystemScript& CclientApp::GetPetSystemScript() const
{
	return mPetSystemScript;
}


// 080731 LUJ, 펫 시스템 정보를 읽는다
void CclientApp::LoadPetSystemScript()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/PetSystemInfo.bin", "rb" ) )
	{
		ASSERT( 0 );
		return;
	}

	ZeroMemory( &mPetSystemScript, sizeof( mPetSystemScript ) );

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH * 10 ] = { 0 };
		file.GetLine( text, sizeof( text ) );

		const char* separator	= "\t ";
		const char* token		= strtok( text, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "#BASESKILLSLOT", token ) )
		{
			const char* min = strtok( 0, separator );
			const char* max	= strtok( 0, separator );

			if( ! min ||
				! max )
			{
				continue;
			}

			mPetSystemScript.mSkillSlot.mMinSize	= atoi( min );
			mPetSystemScript.mSkillSlot.mMaxSize	= atoi( max );
		}
		else if( ! stricmp( "#MAXGRADE", token ) )
		{
			token = strtok( 0, separator );

			mPetSystemScript.mMaxGrade = atoi( token ? token : "" );
		}
		else if( ! stricmp( "#MAXLEVEL", token ) )
		{
			token = strtok( 0, separator );

			mPetSystemScript.mMaxLevel = atoi( token ? token : "" );
		}
	}
}


// 080731 LUJ, 펫 상태 스크립트를 반환한다
const CclientApp::PetStatusScript& CclientApp::GetPetStatusScript( ePetType type, DWORD level ) const
{
	const PetStatusScriptMap::const_iterator it = mPetStatusScriptMap.find( PetStatusKey( type, level ) );

	if( mPetStatusScriptMap.end() == it )
	{
		static PetStatusScript script;

		return script;
	}

	return it->second;
}

// 081027 LUJ, 공성 로그
void CclientApp::OnMenuSiegeLog()
{
	CLogDialog* dialog = GetLogDialog();

	if( ! dialog )
	{
		return;
	}

	CString textSiege;
	textSiege.LoadString( IDS_STRING419 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	mLogDialog->DoModal(
		UINT_MAX,
		CSiegeLogCommand( *this, textSiege + _T( " " ) + textLog ) );
}

// 081202 LUJ, 키워드에 해당하는 스킬 이름을 콤보에 넣는다
size_t CclientApp::SetSkillCombo( CComboBox& comboBox, CString keyword ) const
{
	comboBox.ResetContent();

	size_t count = 0;

	for(	ActiveSkillScriptMap::const_iterator it = mActiveSkillScriptMap.begin();
			mActiveSkillScriptMap.end() != it;
			++it )
	{
		const ActiveSkillScript& script = it->second;
		const std::wstring name( CString( script.mName ).TrimRight() );

		if( -1 < comboBox.FindString( 0, name.c_str() ) )
		{
			continue;
		}
		else if( std::wstring::npos == name.find( keyword ) )
		{
			continue;
		}

		comboBox.AddString( name.c_str() );
		++count;
	}

	// 080401 LUJ, 정확히 일치한 것을 택한다
	comboBox.SetCurSel( max( 0, comboBox.FindString( 0, keyword ) ) );

	return count;
}

// 081202 LUJ, 문자열에 해당하는 스킬 스크립트를 반환한다
const ActiveSkillScript* CclientApp::GetActiveSkillScript( CString skillName ) const
{
	for(	ActiveSkillScriptMap::const_iterator it = mActiveSkillScriptMap.begin();
			mActiveSkillScriptMap.end() != it;
			++it )
	{
		const ActiveSkillScript& script = it->second;

		if( script.mName == skillName )
		{
			return &script;
		}
	}

	return 0;
}

// 081205 LUJ, 채팅 로그
void CclientApp::OnMenuChatLog()
{
	CLogDialog* dialog = GetLogDialog();

	if( ! dialog )
	{
		return;
	}

	CString textChat;
	textChat.LoadString( IDS_STRING429 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	mLogDialog->DoModal(
		UINT_MAX,
		CChatLogCommand( *this, textChat + _T( " " ) + textLog ) );
}

// 090122 LUJ, 스크립트 해킹 로그
void CclientApp::OnMenuScriptHackLog()
{
	CLogDialog* const dialog = GetLogDialog();

	if( ! dialog )
	{
		return;
	}

	CString textScriptHack;
	textScriptHack.LoadString( IDS_STRING431 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	mLogDialog->DoModal(
		UINT_MAX,
		CScriptHackLogCommand( *this, textScriptHack + _T( " " ) + textLog ) );
}

// 090122 LUJ, 이름 변경 로그
void CclientApp::OnMenuRenameLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnablePlayerIndex	= true;
	inputConfiguration.mIsEnableKeyword		= true;

	CIndexInputDialog dialog( *this, inputConfiguration );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CNameLogCommand::Configuration findConfiguration;
	findConfiguration.mKeyword		= dialog.GetKeyword();
	findConfiguration.mPlayerIndex	= dialog.GetPlayerIndex();

	CLogDialog* const logDialog = GetLogDialog();

	if( ! logDialog )
	{
		return;
	}

	CString textPlayer;
	textPlayer.LoadString( IDS_STRING54  );
	CString textName;
	textName.LoadString( IDS_STRING20 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );

	logDialog->DoModal(
		UINT_MAX,
		CNameLogCommand( *this, textPlayer + _T( " " ) + textName + _T( " " ) + textLog, findConfiguration ) );
}

// 090304 LUJ, 아이템 변환 시뮬레이션
void CclientApp::OnMenuSimulationItemChange()
{
	CItemChangeDialog dialog( *this );
	dialog.DoModal();
}

CString CclientApp::GetDateString( DWORD second ) const
{
	CString text;
	const DWORD	minute = 60;
	const DWORD hour = 60 * minute;
	const DWORD day	= 24 * hour;

	{
		const DWORD value = second / day;

		if( value )
		{
			CString textDayFormat;
			textDayFormat.LoadString( IDS_STRING112 );

			CString valueText;
			valueText.Format( textDayFormat, value );

			text += valueText + _T( " " );
		}

		second %= day;
	}

	{
		const DWORD value = second / hour;

		if( value )
		{
			CString textTimeFormat;
			textTimeFormat.LoadString( IDS_STRING113 );

			CString valueText;
			valueText.Format( textTimeFormat, value );

			text += valueText + _T( " " );
		}

		second %= hour;
	}

	{
		const DWORD value = second / minute;

		if( value )
		{
			CString textMinuteFormat;
			textMinuteFormat.LoadString( IDS_STRING114 );

			CString valueText;
			valueText.Format( textMinuteFormat, value );

			text += valueText + _T( " " );
		}

		second %= minute;
	}

	if( second )
	{
		CString textSecondFormat;
		textSecondFormat.LoadString( IDS_STRING115 );

		CString valueText;
		valueText.Format( textSecondFormat, second );

		text += valueText + _T( " " );
	}

	return text;
}

float CclientApp::GetRandomFloat() const
{
	static UINT value = rand();

	value = value * 1103515245 + rand();

	return float( value ) / UINT_MAX;
}

// 090406 LUJ, 돈 로그
void CclientApp::OnMenuMoneyLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnablePlayerIndex = true;
	CIndexInputDialog dialog( *this, inputConfiguration );

	if( IDCANCEL == dialog.DoModal() )
	{
		return;
	}

	CMoneyLogCommand::Configuration findConfiguration;
	findConfiguration.mPlayerIndex	= dialog.GetPlayerIndex();

	CString textMoney;
	textMoney.LoadString( IDS_STRING58 );
	CString textLog;
	textLog.LoadString( IDS_STRING10 );
	
	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		logDialog->DoModal(
			UINT_MAX,
			CMoneyLogCommand( *this, textMoney + _T( " " ) + textLog, findConfiguration ) );
	}
}

void CclientApp::OnHouseLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnableUserIndex = true;
	CIndexInputDialog dialog(
		*this,
		inputConfiguration );

	if(IDCANCEL == dialog.DoModal())
	{
		return;
	}

	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		CString textTitle;
		textTitle.LoadString(
			IDS_STRING476);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CHouseLogCommand::Configuration findConfiguration;
		findConfiguration.mUserIndex = dialog.GetUserIndex();
		logDialog->DoModal(
			UINT_MAX,
			CHouseLogCommand(*this, textTitle + _T( " " ) + textLog, findConfiguration));
	}
}

void CclientApp::OnConsignLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnablePlayerIndex = true;
	inputConfiguration.mIsEnableItemDbIndex = true;
	inputConfiguration.mIsEnableConsignIndex = true;
	CIndexInputDialog dialog(
		*this,
		inputConfiguration );

	if(IDCANCEL == dialog.DoModal())
	{
		return;
	}

	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		CString textTitle;
		textTitle.LoadString(
			IDS_STRING522);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CConsignLogCommand::Configuration findConfiguration;
		findConfiguration.mPlayerIndex = dialog.GetPlayerIndex();
		findConfiguration.mItemDBIndex = dialog.GetItemDbIndex();
		findConfiguration.mConsignIndex = dialog.GetConsignIndex();
		logDialog->DoModal(
			UINT_MAX,
			CConsignLogCommand(*this, textTitle + _T( " " ) + textLog, findConfiguration));
	}
}

void CclientApp::OnGameRoomLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnableUserIndex = true;
	CIndexInputDialog dialog(
		*this,
		inputConfiguration );

	if(IDCANCEL == dialog.DoModal())
	{
		return;
	}

	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		CString textTitle;
		textTitle.LoadString(
			IDS_STRING530);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CGameRoomPointLogCommand::Configuration findConfiguration;
		findConfiguration.mUserIndex = dialog.GetUserIndex();
		logDialog->DoModal(
			UINT_MAX,
			CGameRoomPointLogCommand(*this, textTitle + _T( " " ) + textLog, findConfiguration));
	}
}

void CclientApp::OnCookLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnablePlayerIndex = true;
	CIndexInputDialog dialog(
		*this,
		inputConfiguration );

	if(IDCANCEL == dialog.DoModal())
	{
		return;
	}

	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		CString textTitle;
		textTitle.LoadString(
			IDS_STRING492);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CCookLogCommand::Configuration findConfiguration;
		findConfiguration.mPlayerIndex = dialog.GetPlayerIndex();
		logDialog->DoModal(
			UINT_MAX,
			CCookLogCommand(*this, textTitle + _T( " " ) + textLog, findConfiguration));
	}
}

void CclientApp::OnRecipeLog()
{
	CIndexInputDialog::Configuration inputConfiguration = { 0 };
	inputConfiguration.mIsEnablePlayerIndex = true;
	CIndexInputDialog dialog(
		*this,
		inputConfiguration );

	if(IDCANCEL == dialog.DoModal())
	{
		return;
	}

	CLogDialog* const logDialog = GetLogDialog();

	if( logDialog )
	{
		CString textTitle;
		textTitle.LoadString(
			IDS_STRING491);
		CString textLog;
		textLog.LoadString(
			IDS_STRING10);

		CRecipeLogCommand::Configuration findConfiguration;
		findConfiguration.mPlayerIndex = dialog.GetPlayerIndex();
		logDialog->DoModal(
			UINT_MAX,
			CRecipeLogCommand(*this, textTitle + _T( " " ) + textLog, findConfiguration));
	}
}

void CclientApp::LoadFurnitureScript()
{
	CMHFile file;
	file.Init(
		"System/Resource/Housing_FunitureList.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		char text[MAX_PATH * 10] = {0};
		file.GetLine(
			text,
			sizeof(text));

		const char* separator = "\t";
		const char* token = strtok(
			text,
			separator);

		if(0 == token)
		{
			continue;
		}

		const char* name = strtok(
			0,
			separator);

		if(0 == name)
		{
			continue;
		}		

		mFurnitureNameMap[atoi(token)] = RemoveString(
			name,
			"^s");
	}
}

LPCTSTR CclientApp::GetFurnitureName(DWORD index) const
{
	const FurnitureNameMap::const_iterator iterator = mFurnitureNameMap.find(index);

	return mFurnitureNameMap.end() == iterator ? _T("?") : iterator->second;
}

void CclientApp::LoadCookScript()
{
	CMHFile file;
	file.Init(
		"System/Resource/Cooking.bin",
		"rb");

	LEVELTYPE recipeLevel = 0;

	while(FALSE == file.IsEOF())
	{
		char text[MAX_PATH * 10] = {0};
		file.GetLine(
			text,
			sizeof(text));

		const char* separator = "\t";
		const char* token = strtok(
			text,
			separator);

		if(0 == token)
		{
			continue;
		}
		else if(0 == stricmp(token, "#cookCount"))
		{
			while(const char* textExperience = strtok(0, separator))
			{
				mCookScript.mCookCountContainer[LEVELTYPE(mCookScript.mCookCountContainer.size())] = atoi(
					textExperience);
			}
		}
		else if(0 == stricmp(token, "#eatCount"))
		{
			LEVELTYPE level = 0;

			while(const char* textExperience = strtok(0, separator))
			{
				mCookScript.mEatCountContainer[LEVELTYPE(mCookScript.mEatCountContainer.size())] = atoi(
					textExperience);
			}
		}
		else if(0 == stricmp(token, "#fireCount"))
		{
			LEVELTYPE level = 0;

			while(const char* textExperience = strtok(0, separator))
			{
				mCookScript.mFireCountContainer[LEVELTYPE(mCookScript.mFireCountContainer.size())] = atoi(
					textExperience);
			}
		}
		else if(0 == stricmp(token, "#recipe_lv1"))
		{
			recipeLevel = 1;
		}
		else if(0 == stricmp(token, "#recipe_lv2"))
		{
			recipeLevel = 2;
		}
		else if(0 == stricmp(token, "#recipe_lv3"))
		{
			recipeLevel = 3;
		}
		else if(0 == stricmp(token, "#recipe_lv4"))
		{
			recipeLevel = 4;
		}
		else if(0 == stricmp(token, "#recipe"))
		{
			const char* const textRecipeIndex = strtok(
				0,
				separator);
			const char* const textMinimumExperience = strtok(
				0,
				separator);
			const char* const textMaximumExperience = strtok(
				0,
				separator);
			const char* const textRemainTime = strtok(
				0,
				separator);
			const char* const textResultItemIndex = strtok(
				0,
				separator);

			const RecipeIndex recipeIndex = atoi(
				textRecipeIndex ? textRecipeIndex : "");
			const DWORD resultItemIndex = atoi(
				textResultItemIndex ? textResultItemIndex : "");
			const DWORD remainTime = atoi(
				textRemainTime ? textRemainTime : "");
			const EXPTYPE minimumExperience = atoi(
				textMinimumExperience ? textMinimumExperience : "");
			const EXPTYPE maximumExperience = atoi(
				textMaximumExperience ? textMaximumExperience : "");

			if(mRecipeContainer.end() != mRecipeContainer.find(recipeIndex))
			{
				Log(
					_T("Recipe index is duplicated: %d"),
					recipeIndex);
				continue;
			}

			RecipeScript& script = mRecipeContainer[recipeIndex];
			script.mResultItemIndex = resultItemIndex;
			script.mRemainTime = remainTime;
			script.mMinimumExperience = minimumExperience;
			script.mMaximumExperience = maximumExperience;
			script.mLevel = recipeLevel;

			while(const char* textItemIndex = strtok(0, separator))
			{
				const char* textItemSize = strtok(
					0,
					separator);
				const size_t itemSize = atoi(
					textItemSize ? textItemSize : "");

				script.mMaterialContainer[atoi(textItemIndex)] = itemSize;
			}
		}
	}
}

EXPTYPE CclientApp::GetMaxCookCount(LEVELTYPE level) const
{
	const CookScript::ExperienceContainer& experienceContainer = mCookScript.mCookCountContainer;
	const CookScript::ExperienceContainer::const_iterator iterator = experienceContainer.find(
		level);

	return experienceContainer.end() == iterator ? 0 : iterator->second;
}

EXPTYPE CclientApp::GetMaxEatCount(LEVELTYPE level) const
{
	const CookScript::ExperienceContainer& experienceContainer = mCookScript.mEatCountContainer;
	const CookScript::ExperienceContainer::const_iterator iterator = experienceContainer.find(
		level);

	return experienceContainer.end() == iterator ? 0 : iterator->second;
}

EXPTYPE CclientApp::GetMaxFireCount(LEVELTYPE level) const
{
	const CookScript::ExperienceContainer& experienceContainer = mCookScript.mFireCountContainer;
	const CookScript::ExperienceContainer::const_iterator iterator = experienceContainer.find(
		level);

	return experienceContainer.end() == iterator ? 0 : iterator->second;
}

LPCTSTR CclientApp::GetRecipeName(RecipeIndex recipeIndex) const
{
	const RecipeContainer::const_iterator iterator = mRecipeContainer.find(
		recipeIndex);

	if(mRecipeContainer.end() == iterator)
	{
		return _T("");
	}

	const RecipeScript& script = iterator->second;

	return GetItemName(
		script.mResultItemIndex);
}

LEVELTYPE CclientApp::GetRecipeLevel(RecipeIndex recipeIndex) const
{
	const RecipeContainer::const_iterator iterator = mRecipeContainer.find(
		recipeIndex);

	if(mRecipeContainer.end() == iterator)
	{
		return USHRT_MAX;
	}

	const RecipeScript& script = iterator->second;

	return script.mLevel;
}

void CclientApp::UpdateRecipeListCtrl(const MSG_RM_COOK_RECIPE& message, CListCtrl& learnedListCtrl, CListCtrl& forgottenListCtrl)
{
	learnedListCtrl.DeleteAllItems();
	forgottenListCtrl.DeleteAllItems();

	std::set< RecipeIndex > learnedRecipes;

	for(DWORD i = 0; i < message.mSize; ++i)
	{
		TCHAR text[MAX_PATH] = {0};
		const MSG_RM_COOK_RECIPE::Recipe& recipe = message.mRecipe[i];

		const int row = learnedListCtrl.GetItemCount();
		int step = -1;
		learnedListCtrl.InsertItem(
			row,
			_itot(GetRecipeLevel(recipe.mIndex), text, 10),
			++step);
		learnedListCtrl.SetItemText(
			row,
			++step,
			_itot(recipe.mIndex, text, 10));
		learnedListCtrl.SetItemText(
			row,
			++step,
			GetRecipeName(recipe.mIndex));
		learnedListCtrl.SetItemText(
			row,
			++step,
			_T("0"));
		learnedListCtrl.SetItemText(
			row,
			++step,
			_itot(recipe.mRemainTime, text, 10));

		learnedRecipes.insert(
			recipe.mIndex);
	}

	for(RecipeContainer::const_iterator iterator = mRecipeContainer.begin();
		mRecipeContainer.end() != iterator;
		++iterator)
	{
		const RecipeIndex recipeIndex = iterator->first;

		if(learnedRecipes.end() != learnedRecipes.find(recipeIndex))
		{
			continue;
		}

		TCHAR text[MAX_PATH] = {0};
		const RecipeScript& recipeScript = iterator->second;

		const int row = forgottenListCtrl.GetItemCount();
		int step = -1;
		forgottenListCtrl.InsertItem(
			row,
			_itot(GetRecipeLevel(recipeIndex), text, 10),
			++step);
		forgottenListCtrl.SetItemText(
			row,
			++step,
			_itot(recipeIndex, text,10));
		forgottenListCtrl.SetItemText(
			row,
			++step,
			GetRecipeName(recipeIndex));
		forgottenListCtrl.SetItemText(
			row,
			++step,
			_T("0"));
		forgottenListCtrl.SetItemText(
			row,
			++step,
			_itot(recipeScript.mRemainTime, text, 10));
	}
}

CString CclientApp::GetText(eHOUSEFURNITURE_STATE state) const
{
	switch(state)
	{
	case eHOUSEFURNITURE_STATE_UNKEEP:
		{
			CString text;
			text.LoadString(
				IDS_STRING503);
			return text;
		}
	case eHOUSEFURNITURE_STATE_KEEP:
		{
			CString text;
			text.LoadString(
				IDS_STRING504);
			return text;
		}
	case eHOUSEFURNITURE_STATE_INSTALL:
		{
			CString text;
			text.LoadString(
				IDS_STRING505);
			return text;
		}
	case eHOUSEFURNITURE_STATE_UNINSTALL:
		{
			CString text;
			text.LoadString(
				IDS_STRING506);
			return text;
		}
	default:
		{
			CString text;
			text.Format(
				_T("%d"),
				state);
			return text;
		}
	}
}

CString CclientApp::GetText(HOUSE_HIGHCATEGORY category) const
{
	switch(category)
	{
	case eHOUSE_HighCategory_Furniture:
		{
			CString text;
			text.LoadString(
				IDS_STRING478);
			return text;
		}
	case eHOUSE_HighCategory_Electric:
		{
			CString text;
			text.LoadString(
				IDS_STRING507);
			return text;
		}
	case eHOUSE_HighCategory_Door:
		{
			CString text;
			text.LoadString(
				IDS_STRING508);
			return text;
		}
	case eHOUSE_HighCategory_Wall:
		{
			CString text;
			text.LoadString(
				IDS_STRING509);
			return text;
		}
	case eHOUSE_HighCategory_Carpet:
		{
			CString text;
			text.LoadString(
				IDS_STRING510);
			return text;
		}
	case eHOUSE_HighCategory_Properties:
		{
			CString text;
			text.LoadString(
				IDS_STRING511);
			return text;
		}
	case eHOUSE_HighCategory_Decoration:
		{
			CString text;
			text.LoadString(
				IDS_STRING512);
			return text;
		}
	default:
		{
			CString text;
			text.Format(
				_T("%d"),
				category);
			return text;
		}
	}
}

CString CclientApp::ConvertToText(MONEYTYPE money) const
{
	TCHAR buffer[MAX_PATH] = {0};
	_sntprintf(
		buffer,
		_countof(buffer),
		_T("%u"),
		money);

	TCHAR text[MAX_PATH] = {0};

	for(LPCTSTR character = buffer; *character != 0; character = _tcsinc(character))
	{
		_tcsncat(
			text,
			character,
			1);

		const size_t length = _tcslen(
			character);
		const size_t commaSize = 3;

		if(0 == (length - 1) % commaSize)
		{
			_tcsncat(
				text,
				_T(","),
				sizeof(*character));
		}
	}

	ZeroMemory(
		buffer,
		sizeof(buffer));
	_tcsncpy(
		buffer,
		text,
		_tcslen(text) - 1);

	return CString(buffer);
}