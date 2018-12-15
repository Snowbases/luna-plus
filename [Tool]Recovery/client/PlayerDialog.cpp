#include "stdafx.h"
#include "client.h"
#include "UserDialog.h"
#include "FamilyDialog.h"
#include "LogDialog.h"
#include "StatLogCommand.h"
#include "ExperienceLogCommand.h"
#include "NameLogCommand.h"
#include "JobLogCommand.h"
#include "ItemLogCommand.h"
#include "FishLogCommand.h"
#include "PetLogCommand.h"
#include "FamilyLogCommand.h"
#include "GuildLogCommand.h"
#include "MoneyLogCommand.h"
#include "AutoNoteLogCommand.h"
#include "HouseLogCommand.h"
#include "CookLogCommand.h"
#include "RecipeLogCommand.h"
#include "ConsignLogCommand.h"
#include "ForbidChatLogCommand.h"
#include "grid_control/GridCell.h"
#include "MemoDialog.h"
#include ".\playerdialog.h"

IMPLEMENT_DYNAMIC(CPlayerDialog, CDialog)
CPlayerDialog::CPlayerDialog( CclientApp& application, CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerDialog::IDD, pParent)
	, mApplication( application )
{
	EnableAutomation();

	ZeroMemory( &mReceivedDataMessage, sizeof( mReceivedDataMessage ) );
}


CPlayerDialog::~CPlayerDialog()
{
}

void CPlayerDialog::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면 OnFinalRelease가
	// 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CDialog::OnFinalRelease();
}

void CPlayerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PLAYER_LICENSE_LOVE_EDIT, mLoveScoreEdit);
	DDX_Control(pDX, IDC_LICENSE_GOOD_TREE, mGoodThingsTree);
	DDX_Control(pDX, IDC_LICENSE_BAD_TREE, mBadThingsTree);
	DDX_Control(pDX, IDC_PLAYER_LICENSE_UPDATE_BUTTNO, mLicenseUpdateButton);
	DDX_Control(pDX, IDC_PLAYER_LICENSE_AGE_EDIT, mAgeEdit);
	DDX_Control(pDX, IDC_PLAYER_USER_NAME_EDIT, mUserNameEdit);
	DDX_Control(pDX, IDC_PLAYER_LICENSE_AREA_COMBO2, mAreaCombo);
	DDX_Control(pDX, IDC_PLAYER_GUILD_BUTTON, mGuildButton);
	DDX_Control(pDX, IDC_PLAYER_JOB1_COMBO, mJob1Combo);
	DDX_Control(pDX, IDC_PLAYER_JOB2_COMBO, mJob2Combo);
	DDX_Control(pDX, IDC_PLAYER_JOB3_COMBO, mJob3Combo);
	DDX_Control(pDX, IDC_PLAYER_JOB4_COMBO, mJob4Combo);
	DDX_Control(pDX, IDC_PLAYER_JOB5_COMBO, mJob5Combo);
	DDX_Control(pDX, IDC_PLAYER_JOB6_COMBO, mJob6Combo);
	DDX_Control(pDX, IDC_PLAYER_DATA_TAB, mTabCtrl);
	DDX_Control(pDX, IDC_PLAYER_ITEM, mDataSubmitButton);
	DDX_Control(pDX, IDC_PLAYER_FAMILY_BUTTON, mFamilyButton);
	DDX_Control(pDX, IDC_PLAYER_RECIPE_LEARNED_LIST, mLearnedRecipeListCtrl);
	DDX_Control(pDX, IDC_PLAYER_RECIPE_LIST, mForgottenRecipeListCtrl);
	DDX_Control(pDX, IDC_PLAYER_MAIL_BOX_LIST_CTRL, mMailBoxListCtrl);
	DDX_Control(pDX, IDC_PLAYER_PARTY_COMBO, mPartyCombo);
}


BEGIN_MESSAGE_MAP(CPlayerDialog, CDialog)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
//	ON_BN_CLICKED(IDC_PLAYER_DIALOG_UPDATE, OnBnClickedPlayerDialogUpdate)
	ON_BN_CLICKED(IDC_PLAYER_ITEM, OnBnClickedPlayerUpdateButton)
	ON_BN_CLICKED(IDC_PLAYER_LICENSE_UPDATE_BUTTNO, OnBnClickedPlayerLicenseUpdateButtno)
	ON_BN_CLICKED(IDC_PLAYER_EXP_LOG_BUTTON, OnBnClickedPlayerExpLogButton)
	ON_BN_CLICKED(IDC_PLAYER_STAT_LOG_BUTTON, OnBnClickedPlayerStatLogButton)
	ON_BN_CLICKED(IDC_PLAYER_GUILD_BUTTON, OnBnClickedPlayerGuildButton)
	ON_BN_CLICKED(IDC_PLAYER_USER_VIEW_BUTTON, OnBnClickedPlayerUserViewButton)
	//ON_BN_CLICKED(IDC_PLAYER_JOB_LOG_BUTTON, OnBnClickedPlayerJobLogButton)
	ON_BN_CLICKED(IDC_PLAYER_GUILD_BUTTON2, OnBnClickedPlayerJobButton)
	ON_BN_CLICKED(IDC_PLAYER_ITEM_LOG_BUTTON, OnBnClickedPlayerItemLogButton)
	//ON_BN_CLICKED(IDC_PLAYER_ITEM_ADD_BUTTON, OnBnClickedPlayerItemAddButton)
	ON_BN_CLICKED(IDC_PLAYER_JOB_SUBMIT_BUTTON, OnBnClickedPlayerJobSubmitButton)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB1_COMBO, OnCbnSelchangePlayerJob1Combo)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB2_COMBO, OnCbnSelchangePlayerJob2Combo)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB3_COMBO, OnCbnSelchangePlayerJob3Combo)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB4_COMBO, OnCbnSelchangePlayerJob4Combo)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB5_COMBO, OnCbnSelchangePlayerJob5Combo)
	ON_CBN_SELCHANGE(IDC_PLAYER_JOB6_COMBO, OnCbnSelchangePlayerJob6Combo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_PLAYER_DATA_TAB, OnTcnSelchangePlayerDataTab)
	ON_BN_CLICKED(IDC_PLAYER_NAME_LOG_BUTTON, OnBnClickedPlayerNameLogButton)
	ON_BN_CLICKED(IDC_PLAYER_FAMILY_BUTTON, OnBnClickedPlayerFamilyButton)
	ON_BN_CLICKED(IDC_FISH_LOG_BUTTON, OnBnClickedFishLogButton)
	ON_BN_CLICKED(IDC_PLAYER_PET_LOG_BUTTON, OnBnClickedPlayerPetLogButton)
	ON_BN_CLICKED(IDC_PLAYER_FAMILY_LOG_BUTTON, OnBnClickedPlayerFamilyLogButton)
	ON_BN_CLICKED(IDC_PLAYER_LOG_BUTTON, OnBnClickedPlayerLogButton)
	ON_BN_CLICKED(IDC_PLAYER_MONEY_LOG_BUTTON, OnBnClickedPlayerMoneyLogButton)
	ON_BN_CLICKED(IDC_PLAYER_AUTONOTE_LOG_BUTTON, OnBnClickedPlayerAutonoteLogButton)
	ON_BN_CLICKED(IDC_PLAYER_HOUSE, OnBnClickedPlayerHouse)
	ON_BN_CLICKED(IDC_PLAYER_COOK_RECIPE_ADD_BUTTON, OnBnClickedPlayerCookRecipeAddButton)
	ON_BN_CLICKED(IDC_PLAYER_COOK_RECIPE_REMOVE_BUTTON, OnBnClickedPlayerCookRecipeRemoveButton)
	ON_BN_CLICKED(IDC_PLAYER_TAB_LOG_BUTTON, OnBnClickedPlayerTabLogButton)
	ON_BN_CLICKED(IDC_PLAYER_TAB_LOG_BUTTON2, OnBnClickedPlayerTabLogButton2)
	ON_BN_CLICKED(IDC_PLAYER_FORBIDCHAT_LOG_BUTTON, OnBnClickedPlayerForbidChatLogButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PLAYER_MAIL_BOX_LIST_CTRL, OnLvnItemchangedPlayerMailBoxListCtrl)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPlayerDialog, CDialog)
END_DISPATCH_MAP()

// 참고: IID_IPlayerDialog에 대한 지원을 추가하여
// VBA에서 형식 안정적인 바인딩을 지원합니다. 
// 이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {3B39A217-C6E8-45A3-813A-7B5AB700101B}
static const IID IID_IPlayerDialog =
{ 0x3B39A217, 0xC6E8, 0x45A3, { 0x81, 0x3A, 0x7B, 0x5A, 0xB7, 0x0, 0x10, 0x1B } };

BEGIN_INTERFACE_MAP(CPlayerDialog, CDialog)
	INTERFACE_PART(CPlayerDialog, IID_IPlayerDialog, Dispatch)
END_INTERFACE_MAP()


BOOL CPlayerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), FALSE );
	SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ), TRUE );

	// combo initialization
	{
		{
			typedef CclientApp::LicenseAreaMap AreaMap;

			const AreaMap& areaMap = mApplication.GetLicenseAreaMap();

			for( AreaMap::const_iterator it = areaMap.begin(); areaMap.end() != it; ++it )
			{
				CString text;
				text.Format( _T( "%02d %s" ), it->first, it->second );

				mAreaCombo.AddString( text );
			}

			CRect rect;
			mAreaCombo.GetWindowRect( rect );
			mAreaCombo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}

		// 직업 콤보
		{
			CRect rect;
			mJob1Combo.GetWindowRect( rect );
			mJob1Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mJob2Combo.GetWindowRect( rect );
			mJob2Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mJob3Combo.GetWindowRect( rect );
			mJob3Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mJob4Combo.GetWindowRect( rect );
			mJob4Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mJob5Combo.GetWindowRect( rect );
			mJob5Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );

			mJob6Combo.GetWindowRect( rect );
			mJob6Combo.SetWindowPos( 0, 0, 0, rect.Width(), 200, SWP_NOMOVE | SWP_NOZORDER );
		}
	}

	// 배운 요리법 리스트 컨트롤
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;
		int column = -1;

		mLearnedRecipeListCtrl.SetExtendedStyle(
			mLearnedRecipeListCtrl.GetExtendedStyle() | addStyle);
		mLearnedRecipeListCtrl.GetWindowRect(
			rect);

		CString text;
		text.LoadString(
			IDS_STRING5);
		mLearnedRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING491);
		mLearnedRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			0);

		text.LoadString(
			IDS_STRING491);
		mLearnedRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.5f));

		text.LoadString(
			IDS_STRING479);
		mLearnedRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.15f));

		text.LoadString(
			IDS_STRING102);
		mLearnedRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.15f));
	}

	// 잊어버린 요리법
	{
		const DWORD addStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
		CRect rect;
		int column = -1;

		mForgottenRecipeListCtrl.SetExtendedStyle(
			mForgottenRecipeListCtrl.GetExtendedStyle() | addStyle);
		mForgottenRecipeListCtrl.GetWindowRect(
			rect);

		CString text;
		text.LoadString(
			IDS_STRING5);
		mForgottenRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.2f));

		text.LoadString(
			IDS_STRING491);
		mForgottenRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			0);

		text.LoadString(
			IDS_STRING491);
		mForgottenRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.5f));

		text.LoadString(
			IDS_STRING479);
		mForgottenRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.15f));

		text.LoadString(
			IDS_STRING102);
		mForgottenRecipeListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(rect.Width() * 0.15f));
	}

	// 우편함
	{
		mMailBoxListCtrl.SetExtendedStyle(
			mMailBoxListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		CRect tabRect;
		mTabCtrl.GetWindowRect(
			tabRect);

		mMailBoxListCtrl.SetWindowPos(
			0,
			0,
			0,
			tabRect.Width() - 25,
			tabRect.Height() - 100,
			SWP_NOMOVE | SWP_NOZORDER);

		int column = -1;
		CString text;
		text.LoadString(
			IDS_STRING22);
		mMailBoxListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			0);

		text.LoadString(
			IDS_STRING3);
		mMailBoxListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(tabRect.Width() * 0.3f));

		text.LoadString(
			IDS_STRING516);
		mMailBoxListCtrl.InsertColumn(
			++column,
			text,
			LVCFMT_LEFT,
			int(tabRect.Width() * 0.7f));
	}

	// tree initialization
	{
		typedef CclientApp::LicenseFavoriteMap FavoriteMap;
		
		const FavoriteMap& favoriteMap = mApplication.GetLicenseFavoriteMap();

		for( FavoriteMap::const_iterator it = favoriteMap.begin(); favoriteMap.end() != it; ++it )
		{
			CString text;
			text.Format( _T( "%d %s" ), it->first, it->second );
			
			mGoodThingsTree.InsertItem( text, 0, 0, TVI_ROOT, TVI_LAST );
			mBadThingsTree.InsertItem( text, 0, 0, TVI_ROOT, TVI_LAST );
		}

		// 체크박스가 최초에 그려지거나 안 그려질 경우가 발생한다.
		// 이를 해결하는 건 일종의 꼼수지만, 다른 방법은 아직 없다.
		// 
		// http://www.codeguru.com/forum/archive/index.php/t-197692.html
		{
			mGoodThingsTree.ModifyStyle( TVS_CHECKBOXES, 0 );
			mGoodThingsTree.ModifyStyle( 0, TVS_CHECKBOXES );

			mBadThingsTree.ModifyStyle( TVS_CHECKBOXES, 0 );
			mBadThingsTree.ModifyStyle( 0, TVS_CHECKBOXES );
		}
	}

	// grid initialization
	{
		CRect gridRect;
		{
			CRect windowRect;
			GetWindowRect( windowRect );

			CRect tabRect;
			mTabCtrl.GetWindowRect( tabRect );

			CRect buttonRect;
			mDataSubmitButton.GetWindowRect( buttonRect );

			gridRect.left	= abs( tabRect.left - windowRect.left );
			gridRect.top	= abs( tabRect.top - windowRect.top );

			gridRect.right	= gridRect.left + tabRect.Width();
			gridRect.bottom	= gridRect.top	+ tabRect.Height() - buttonRect.Height();

			// scaling
			{
				// 90%
				const float scale	= 0.9f;
				const LONG	width	= LONG( gridRect.Width() * scale );
				const LONG	height	= LONG( gridRect.Height() * scale );

				gridRect.left	+= abs( tabRect.Width() - width ) / 2;
				gridRect.right	-= abs( tabRect.Width() - width ) / 2;

				//gridRect.top	-= abs( gridRect.Height() - height ) / 2;
				gridRect.bottom	= gridRect.top + height;
			}
		}

		// create numeric grid
		{
			CString textGender;
			textGender.LoadString( IDS_STRING92 );

			CString textStrength;
			textStrength.LoadString( IDS_STRING70 );

			CString textDexterity;
			textDexterity.LoadString( IDS_STRING71 );

			CString textVitality;
			textVitality.LoadString( IDS_STRING72 );

			CString textIntelligence;
			textIntelligence.LoadString( IDS_STRING73 );

			CString textWisdom;
			textWisdom.LoadString( IDS_STRING74 );

			CString textLife;
			textLife.LoadString( IDS_STRING75 );

			CString textMana;
			textMana.LoadString( IDS_STRING76 );

			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			CString textMaxLevel;
			textMaxLevel.LoadString( IDS_STRING93 );

			CString textExperience;
			textExperience.LoadString( IDS_STRING9 );

			CString textStatPoint;
			textStatPoint.LoadString( IDS_STRING94 );

			CString textGold;
			textGold.LoadString( IDS_STRING95 );

			CString textPlayTime;
			textPlayTime.LoadString( IDS_STRING161 );

			CString textSkillPoint;
			textSkillPoint.LoadString( IDS_STRING96 );

			CString textExtendedInventory;
			textExtendedInventory.LoadString( IDS_STRING162 );

			// 080610 LUJ, PK 추가
			CString	textPoint;
			textPoint.LoadString( IDS_STRING34 );
			CString textPlayerKill( _T( "PK" ) );
			textPlayerKill += textPoint;

			CString textMap;
			textMap.LoadString( IDS_STRING163 );

			CString textLocation;
			textLocation.LoadString( IDS_STRING68 );

			// *은 수정 가능한 값을 나타냄
			const CString title[] ={ 
					textStrength + _T( "*" ),
					textDexterity + _T( "*" ),
					textVitality + _T( "*" ),
					textIntelligence + _T( "*" ),
					textWisdom + _T( "*" ),
					textLife + _T( "*" ),
					textMana + _T( "*" ),
					textLevel + _T( "*" ),
					textMaxLevel + _T( "*" ),
					textExperience + _T( "*" ),
					textStatPoint + _T( "*" ),
					textGold + _T( "*" ),
					textSkillPoint + _T( "*" ),
					textExtendedInventory + _T( "*" ),
					textPlayerKill + _T( "*" ),
					textMap  + _T( "*" ),
					_T( "X" ) + textLocation + _T( "*" ),
					_T( "Y" ) + textLocation + _T( "*" ),
					_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.6f );
			const int valueColumn = int( gridRect.Width() * 0.4f );
			const int columnWidth = titleColumn + valueColumn;

			mApplication.CreateGrid( this, &mDataGrid, IDC_PLAYER_GRID, gridRect, title, FALSE );

			mDataGrid.SetColumnWidth( 0, titleColumn );
			mDataGrid.SetColumnWidth( 1, valueColumn );
			mDataGrid.ExpandToFit();
			mDataGrid.ShowWindow(
				SW_SHOW);
			CString textGameInformation;
			textGameInformation.LoadString(
				IDS_STRING175);
			mTabCtrl.InsertItem(
				mTabCtrl.GetItemCount(),
				textGameInformation);
		}

		// create string grid
		{
			CString textName;
			textName.LoadString( IDS_STRING20 );

			CString textGuild;
			textGuild.LoadString( IDS_STRING164 );

			CString textFamily;
			textFamily.LoadString( IDS_STRING165 );

			CString textNick;
			textNick.LoadString( IDS_STRING21 );

			CString textLastConnection;
			textLastConnection.LoadString( IDS_STRING166 );

			CString textGender;
			textGender.LoadString( IDS_STRING92 );
			CString textRace;
			textRace.LoadString(IDS_STRING513);
			CString textIsEnableConnection;
			textIsEnableConnection.LoadString( IDS_STRING167 );

			CString textCreatedDate;
			textCreatedDate.LoadString( IDS_STRING169 );

			CString textCreatedIp;
			textCreatedIp.LoadString( IDS_STRING170 );

			CString textDeletedDate;
			textDeletedDate.LoadString( IDS_STRING171 );

			CString textDeletedIp;
			textDeletedIp.LoadString( IDS_STRING172 );

			CString textRestoredDate;
			textRestoredDate.LoadString( IDS_STRING173 );

			CString textRestoredIp;
			textRestoredIp.LoadString( IDS_STRING174 );

			// 081020 LUJ, 몬스터 킬 수
			CString textMonsterKillCount;
			textMonsterKillCount.LoadString( IDS_STRING409 );
			// 081020 LUJ, 플레이 시간
			CString textPlayTimeTotal;
			textPlayTimeTotal.LoadString( IDS_STRING161 );

			// *은 수정 가능한 값을 나타냄
			const CString title[] = {
				textName + _T( "*" ),
				textGuild + _T( " " ) + textNick + _T( "*" ),
				textFamily + _T( " " ) + textNick + _T( "*" ),
				textLastConnection,
				textGender,
				textRace,
				textIsEnableConnection,
				textCreatedDate,
				textCreatedIp,
				textDeletedDate,
				textDeletedIp,
				textRestoredDate,
				textRestoredIp,
				textMonsterKillCount,
				textPlayTimeTotal,
				_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.4f );
			const int valueColumn = int( gridRect.Width() * 0.6f );
			const int columnWidth = titleColumn + valueColumn;

			mApplication.CreateGrid( this, &mExtendedDataGrid, IDC_PLAYER_EXTENDED_GRID, gridRect, title, TRUE );

			mExtendedDataGrid.SetColumnWidth( 0, titleColumn );
			mExtendedDataGrid.SetColumnWidth( 1, valueColumn );
			mExtendedDataGrid.ExpandColumnsToFit();
			mExtendedDataGrid.ShowWindow(
				SW_HIDE);

			CString textUsingInformation;
			textUsingInformation.LoadString(
				IDS_STRING176);
			mTabCtrl.InsertItem(
				mTabCtrl.GetItemCount(),
				textUsingInformation);
		}

		// 080526 LUJ, 낚시 정보용 그리드
		{
			CString textLevel;
			textLevel.LoadString( IDS_STRING5 );

			CString textExperience;
			textExperience.LoadString( IDS_STRING9 );

			CString textPoint;
			textPoint.LoadString( IDS_STRING34 );			

			// *은 수정 가능한 값을 나타냄
			const CString title[] = {
				textLevel + _T( "*" ),
				textExperience + _T( "*" ),
				textPoint + _T( "*" ),
				_T( "" )	// it must end with empty string
			};

			const int titleColumn = int( gridRect.Width() * 0.4f );
			const int valueColumn = int( gridRect.Width() * 0.6f );
			const int columnWidth = titleColumn + valueColumn;

			mApplication.CreateGrid( this, &mFishingDataGrid, IDC_PLAYER_EXTENDED_GRID, gridRect, title, TRUE );

			mFishingDataGrid.SetColumnWidth( 0, titleColumn );
			mFishingDataGrid.SetColumnWidth( 1, valueColumn );
			mFishingDataGrid.ExpandColumnsToFit();
			mFishingDataGrid.ShowWindow(
				SW_HIDE );

			CString textFishing;
			textFishing.LoadString(
				IDS_STRING379);
			mTabCtrl.InsertItem(
				mTabCtrl.GetItemCount(),
				textFishing);
		}

		// 요리 정보용 그리드
		{
			CString textAlias;
			textAlias.LoadString(
				IDS_STRING20);
			CString textLevel;
			textLevel.LoadString(
				IDS_STRING5);
			CString textMakingFood;
			textMakingFood.LoadString(
				IDS_STRING488);
			CString textTasteFood;
			textTasteFood.LoadString(
				IDS_STRING489);
			CString textFiring;
			textFiring.LoadString(
				IDS_STRING490);
			CString textRecipe;
			textRecipe.LoadString(
				IDS_STRING491);

			// *은 수정 가능한 값을 나타냄
			const CString title[] = {
				textAlias,
				textLevel + _T("*"),
				textMakingFood + _T("*"),
				textTasteFood + _T("*"),
				textFiring + _T("*"),
				textRecipe + _T("*"),
				_T("")
			};	// it must end with empty string

			const int titleColumn = int( gridRect.Width() * 0.4f );
			const int valueColumn = int( gridRect.Width() * 0.6f );
			const int columnWidth = titleColumn + valueColumn;

			mApplication.CreateGrid(
				this,
				&mCookDataGrid,
				IDC_PLAYER_EXTENDED_GRID,
				gridRect,
				title,
				TRUE);

			mCookDataGrid.SetColumnWidth( 0, titleColumn );
			mCookDataGrid.SetColumnWidth( 1, valueColumn );
			mCookDataGrid.ExpandColumnsToFit();
			mCookDataGrid.ShowWindow(
				SW_HIDE);

			CString textCook;
			textCook.LoadString(
				IDS_STRING492);
			mTabCtrl.InsertItem(
				mTabCtrl.GetItemCount(),
				textCook);
		}

		// 집 그리드
		{
			CString textName;
			textName.LoadString(
				IDS_STRING20);
			CString textExtendedLevel;
			textExtendedLevel.LoadString(
				IDS_STRING493);
			CString textTotalVisitCount;
			textTotalVisitCount.LoadString(
				IDS_STRING494);
			CString	textDailyVisitCount;
			textDailyVisitCount.LoadString(
				IDS_STRING495);
			CString textDecoratedPoint;
			textDecoratedPoint.LoadString(
				IDS_STRING496);
			CString textHousePoint;
			textHousePoint.LoadString(
				IDS_STRING497);

			// *은 수정 가능한 값을 나타냄
			const CString title[] = {
				textName + _T("*"),
				textExtendedLevel + _T("*"),
				textTotalVisitCount,
				textDailyVisitCount,
				textDecoratedPoint + _T("*"),
				textHousePoint + _T("*"),
				_T("")
			};	// it must end with empty string

			const int titleColumn = int( gridRect.Width() * 0.40f );
			const int valueColumn = int( gridRect.Width() * 0.60f );
			const int columnWidth = titleColumn + valueColumn;

			mApplication.CreateGrid(
				this,
				&mHouseDataGrid,
				IDC_PLAYER_EXTENDED_GRID,
				gridRect,
				title,
				TRUE);

			mHouseDataGrid.SetColumnWidth( 0, titleColumn );
			mHouseDataGrid.SetColumnWidth( 1, valueColumn );
			mHouseDataGrid.ExpandColumnsToFit();
			mHouseDataGrid.ShowWindow(
				SW_HIDE);

			CString textHouse;
			textHouse.LoadString(
				IDS_STRING476);
			mTabCtrl.InsertItem(
				mTabCtrl.GetItemCount(),
				textHouse);
		}

		CString textNote;
		textNote.LoadString(
			IDS_STRING527);
		mTabCtrl.InsertItem(
			mTabCtrl.GetItemCount(),
			textNote);
	}

	OnTcnSelchangePlayerDataTab(
		0,
		0);

	return TRUE;
}


// 080328 LUJ, 플레이어의 패밀리 정보를 얻어온다
void CPlayerDialog::Parse( DWORD serverIndex, const MSGROOT* message )
{
	mServerIndex = serverIndex;

	switch( message->Protocol )
	{
	case MP_RM_PLAYER_GET_DATA_ACK:
		{
			mTabCtrl.SetCurSel(
				0);
			OnTcnSelchangePlayerDataTab(
				0,
				0);

			const MSG_RM_PLAYER_DATA* m = ( MSG_RM_PLAYER_DATA* )message;
			mReceivedDataMessage = *m;

			mDataGrid.ClearCells( CCellRange( 0, 1 ) );

			const DWORD style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM		cell	= { 0 };
			
			cell.nFormat	= style;
			cell.col		= 1;
			cell.row		= 0;
			cell.mask		= GVIF_TEXT;

			{
				const MSG_RM_PLAYER_DATA::Player& player = mReceivedDataMessage.mPlayer;

				cell.strText.Format( _T( "%d" ), player.mStrength );
				mDataGrid.SetItem( &cell );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mDexterity );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mVitality );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mIntelligence );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mWisdom );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mLife );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mMana );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), player.mGrade );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mMaxGrade );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mExp );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mGradeUpPoint );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%u" ), player.mMoney );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mSkillPoint );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mExtendedInventorySize );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), player.mBadFame );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;
			}

			{
				const MSG_RM_PLAYER_DATA::Position& position = mReceivedDataMessage.mPosition;

				cell.strText.Format( _T( "%d" ), position.mMap );
				mDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				mDataGrid.SetItem( &cell );
				++cell.row;	

				cell.strText.Format( _T( "%d" ), position.mX );
				mDataGrid.SetItem( &cell );
				++cell.row;

				cell.strText.Format( _T( "%d" ), position.mY );
				mDataGrid.SetItem( &cell );
				++cell.row;
			}

			{
				const MSG_RM_PLAYER_DATA::Job& job = mReceivedDataMessage.mJob;
				const DWORD raceDevil = 2;
				const DWORD raceNormal = 1;
				const DWORD race = (4 == job.mData1 ? raceDevil : raceNormal);
				DWORD step = 0;

				mApplication.SetJobCombo( mJob1Combo, job.mData1, race, ++step, 1 );
				mApplication.SetJobCombo( mJob2Combo, job.mData1, race, ++step, job.mData2 );
				mApplication.SetJobCombo( mJob3Combo, job.mData1, race, ++step, job.mData3 );
				mApplication.SetJobCombo( mJob4Combo, job.mData1, race, ++step, job.mData4 );
				mApplication.SetJobCombo( mJob5Combo, job.mData1, race, ++step, job.mData5 );
				mApplication.SetJobCombo( mJob6Combo, job.mData1, race, ++step, job.mData6 );
			}

			{
				const MSG_RM_PLAYER_DATA::Guild& guild = mReceivedDataMessage.mGuild;

				CString text;

				if( guild.mIndex )
				{
					mGuildButton.SetWindowText( CString( guild.mName ) );
					mGuildButton.EnableWindow( TRUE );
				}				
			}

			{
				mTabCtrl.EnableWindow( TRUE );
			}

			break;
		}
	case MP_RM_PLAYER_GET_EXTENDED_DATA_ACK:
		{
			const MSG_RM_PLAYER_EXTENDED_DATA* m = ( MSG_RM_PLAYER_EXTENDED_DATA* )message;
			mReceivedExtendedMessage = *m;

			const DWORD style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM		cell	= { 0 };

			cell.nFormat	= style;
			cell.col		= 1;
			cell.row		= 0;
			cell.mask		= GVIF_TEXT;

			{
				const MSG_RM_PLAYER_EXTENDED_DATA::Player& player = mReceivedExtendedMessage.mPlayer;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( player.mName ) >( player.mName ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( player.mGuildNick ) >( player.mGuildNick ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( player.mFamilyNick ) >( player.mFamilyNick ) );
				mExtendedDataGrid.SetItem( &cell );
				// 081126 LUJ, 패밀리 변경 가능토록 수정
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_MODIFIED );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( player.mLastPlayedTime ) >( player.mLastPlayedTime ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				CString textFemale;
				textFemale.LoadString( IDS_STRING177 );
				CString textMale;
				textMale.LoadString( IDS_STRING178 );

				cell.strText.Format( _T( "%s" ), player.mGender ? textFemale : textMale );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				switch(player.mRace)
				{
				case 0:
					{
						cell.strText.LoadString(
							IDS_STRING514);
						break;
					}
				case 1:
					{
						cell.strText.LoadString(
							IDS_STRING206);
						break;
					}
				case 2:
					{
						cell.strText.LoadString(
							IDS_STRING515);
						break;
					}
				default:
					{
						cell.strText.Format(
							_T("?(%d)"),
							player.mRace);
						break;
					}
				}

				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;	
			}

			{
				const MSG_RM_PLAYER_EXTENDED_DATA::History& history = mReceivedExtendedMessage.mHistory;

				CString textEnable;
				textEnable.LoadString( IDS_STRING25 );

				CString textUnable;
				textUnable.LoadString( IDS_STRING26 );

				cell.strText.Format( _T( "%s" ), history.mIsEnable ? textEnable : textUnable );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mCreateDate ) >( history.mCreateDate ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mCreateIp ) >( history.mCreateIp ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mDeletedDate ) >( history.mDeletedDate ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mDeletedIp ) >( history.mDeletedIp ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mRecoveredDate ) >( history.mRecoveredDate ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;

				cell.strText.Format( _T( "%s" ), CA2WEX< sizeof( history.mRecoveredIp ) >( history.mRecoveredIp ) );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;
				// 081020 LUJ, 몬스터 킬 카운트
				cell.strText.Format( _T( "%u" ), history.mMonsterKillCount );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;
				// 081020 LUJ, 게임 시간
				cell.strText.Format( _T( "%u" ), history.mPlayTimeTotal );
				mExtendedDataGrid.SetItem( &cell );
				mExtendedDataGrid.SetItemState( cell.row, cell.col, GVIS_READONLY );
				++cell.row;
			}

			break;
		}
	case MP_RM_PLAYER_GET_LICENSE_ACK:
		{
			{
				mAgeEdit.EnableWindow( TRUE );
				mAreaCombo.EnableWindow( TRUE );
				mLoveScoreEdit.EnableWindow( TRUE );
				mGoodThingsTree.EnableWindow( TRUE );
				mBadThingsTree.EnableWindow( TRUE );
				mLicenseUpdateButton.EnableWindow( TRUE );
			}

			memcpy( &mReceivedLicenseMessage, message, sizeof( mReceivedLicenseMessage ) );

			const DATE_MATCHING_INFO& data	= mReceivedLicenseMessage.mDateMatching;

			{
 				CString text;
				text.Format( _T( "%d" ), data.dwAge );
				mAgeEdit.SetWindowText( text );

				//mLicenseAgeEdit.SetCurSel( data.dwAge );
				mAreaCombo.SetCurSel( data.dwRegion );

				text.Format( _T( "%d" ), data.dwGoodFeelingDegree );
				mLoveScoreEdit.SetWindowText( text );
			}

			// 트리 컨트롤의 체크 박스 세팅
			{
				typedef CclientApp::LicenseFavoriteMap FavoriteMap;
				
				struct
				{
					void operator() ( const FavoriteMap& favoriteMap, CTreeCtrl& tree, DWORD index ) const
					{
						FavoriteMap::const_iterator it = favoriteMap.find( index );

						if( favoriteMap.end() == it )
						{
							return;
						}

						CString text;
						text.Format( _T( "%d %s" ), it->first, it->second );

						for(	HTREEITEM item = tree.GetFirstVisibleItem();
								item;
								item = tree.GetNextVisibleItem( item ) )
						{
							const CString currentText = tree.GetItemText( item );

							if( text == currentText )
							{
								tree.SetCheck( item );

								return;
							}
						}
					}
				}
				SetTreeCheck;

				const FavoriteMap& favoriteMap = mApplication.GetLicenseFavoriteMap();

				SetTreeCheck( favoriteMap, mGoodThingsTree, data.pdwGoodFeeling[ 0 ] );
				SetTreeCheck( favoriteMap, mGoodThingsTree, data.pdwGoodFeeling[ 1 ] );
				SetTreeCheck( favoriteMap, mGoodThingsTree, data.pdwGoodFeeling[ 2 ] );
				SetTreeCheck( favoriteMap, mBadThingsTree, data.pdwBadFeeling[ 0 ] );
				SetTreeCheck( favoriteMap, mBadThingsTree, data.pdwBadFeeling[ 1 ] );

				mGoodThingsTree.InvalidateRect( 0, FALSE );
			}

			break;
		}
	case MP_RM_PLAYER_GET_USER_ACK:
		{
			const MSG_NAME2_DWORD* m = (MSG_NAME2_DWORD*)message;

			if(0 < strlen(m->Name2))
			{
				TCHAR text[MAX_PATH] = {0};
				_sntprintf(
					text,
					_countof(text),
					0 < strlen(m->Name1) ?  _T("%s (이전: %s)") : _T("%s%s"),
					CString(m->Name2),
					CString(m->Name1));
				mUserNameEdit.SetWindowText(
					text);
			}
			else
			{
				mUserNameEdit.SetWindowText(
					CString(m->Name1));
			}

			mUserNameEdit.SetSel( 0, mUserNameEdit.GetWindowTextLength() );
			mUserIndex = m->dwData;
			break;
		}
	case MP_RM_PLAYER_GET_USER_NACK:
		{
			ASSERT( 0 && "There is no id for login" );
			break;
		}
	case MP_RM_PLAYER_GET_LICENSE_NACK:
		{
			mAgeEdit.EnableWindow( FALSE );
			mAreaCombo.EnableWindow( FALSE );
			mLoveScoreEdit.EnableWindow( FALSE );
			mGoodThingsTree.EnableWindow( FALSE );
			mBadThingsTree.EnableWindow( FALSE );
			mLicenseUpdateButton.EnableWindow( FALSE );
			break;
		}
	case MP_RM_PLAYER_SET_DATA_ACK:
	case MP_RM_PLAYER_SET_EXTENDED_DATA_ACK:
	case MP_RM_PLAYER_SET_LICENSE_ACK:
	// 080526 LUJ, 낚시 정보 변경 성공
	case MP_RM_PLAYER_SET_FISHING_ACK:
		{
			CString textUpdateWasSucceed;
			textUpdateWasSucceed.LoadString( IDS_STRING28 );

			MessageBox( textUpdateWasSucceed, _T( "" ), MB_OK );
			break;
		}
	case MP_RM_PLAYER_SET_LICENSE_NACK:
	case MP_RM_PLAYER_SET_DATA_NACK:
		{
			CString textUpdateWasFailed;
			textUpdateWasFailed.LoadString( IDS_STRING29 );

			MessageBox( textUpdateWasFailed, _T( "" ), MB_OK );

			Parse( mServerIndex, &mReceivedDataMessage );
			break;
		}
	case MP_RM_PLAYER_SET_EXTENDED_DATA_NACK:
		{
			CString textSameNameIsExisted;
			textSameNameIsExisted.LoadString( IDS_STRING180 );
			MessageBox( textSameNameIsExisted, _T( "" ), MB_OK );

			Parse( mServerIndex, &mReceivedExtendedMessage );
			break;
		}
	case MP_RM_PLAYER_NACK_BY_AUTH:
		{
			CString textYouHaveNoAuthority;
			textYouHaveNoAuthority.LoadString( IDS_STRING18 );

			MessageBox( textYouHaveNoAuthority, _T( "" ), MB_OK | MB_ICONERROR );
			break;
		}
	// 080328 LUJ, 플레이어의 패밀리 정보를 얻어온다
	case MP_RM_PLAYER_GET_FAMILY_ACK:
		{
			const MSG_NAME_DWORD* m = ( MSG_NAME_DWORD* )message;
			// 080731 LUJ, 패밀리 인덱스를 저장한다
			mFamilyIndex = m->dwData;

			const CString name( m->Name );
			mFamilyButton.SetWindowText( name );
			mFamilyButton.EnableWindow( ! name.IsEmpty() );
			break;
		}
	// 080526 LUJ, 낚시 정보를 표시한다
	case MP_RM_PLAYER_GET_FISHING_ACK:
		{
			const MSG_DWORD3* m = ( MSG_DWORD3* )message;

			mFishingDataGrid.ClearCells( CCellRange( 0, 1 ) );

			const DWORD style	= DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM		cell	= { 0 };
			cell.nFormat	= style;
			cell.col		= 1;
			cell.row		= 0;
			cell.mask		= GVIF_TEXT;

			cell.strText.Format( _T( "%0d" ), m->dwData1 );
			mFishingDataGrid.SetItem( &cell );
			mFishingDataGrid.SetItemState( cell.row++, cell.col, GVIS_MODIFIED );
			
			cell.strText.Format( _T( "%0d" ), m->dwData2 );
			mFishingDataGrid.SetItem( &cell );
			mFishingDataGrid.SetItemState( cell.row++, cell.col, GVIS_MODIFIED );
			
			cell.strText.Format( _T( "%0d" ), m->dwData3 );
			mFishingDataGrid.SetItem( &cell );
			mFishingDataGrid.SetItemState( cell.row++, cell.col, GVIS_MODIFIED );
			break;
		}
	case MP_RM_PLAYER_GET_COOK_ACK:
		{
			const MSG_DWORD4* receivedMessage = (MSG_DWORD4*)message;
			const DWORD cookLevel = receivedMessage->dwData1;
			const DWORD cookCount = receivedMessage->dwData2;
			const DWORD eatCount = receivedMessage->dwData3;
			const DWORD fireCount = receivedMessage->dwData4;

			mCookDataGrid.ClearCells(
				CCellRange(0, 1));

			const DWORD style = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM	cell = { 0 };
			cell.nFormat = style;
			cell.col = 1;
			cell.mask = GVIF_TEXT;

			// 칭호 / 요리 등급
			{
				switch(cookLevel)
				{
				case 1:
					{
						cell.strText.Format(
							_T("%s (%s)"),
							mApplication.GetInterfaceMessage(1032),
							mApplication.GetInterfaceMessage(1020));
						break;
					}
				case 2:
					{
						cell.strText.Format(
							_T("%s (%s)"),
							mApplication.GetInterfaceMessage(1033),
							mApplication.GetInterfaceMessage(1021));
						break;
					}
				case 3:
					{
						cell.strText.Format(
							_T("%s (%s)"),
							mApplication.GetInterfaceMessage(1034),
							mApplication.GetInterfaceMessage(1022));
						break;
					}
				case 4:
					{
						cell.strText.Format(
							_T("%s (%s)"),
							mApplication.GetInterfaceMessage(1073),
							mApplication.GetInterfaceMessage(1023));
						break;
					}
				default:
					{
						CString textEtc;
						textEtc.LoadString(
							IDS_STRING61);

						cell.strText.Format(
							_T("%s: %d"),
							textEtc,
							cookLevel);
						break;
					}
				}

				mCookDataGrid.SetItem(
					&cell);
				mCookDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_READONLY);
			}

			// 레벨
			{
				cell.strText.Format(
					_T("%d"),
					cookLevel);
				mCookDataGrid.SetItem(
					&cell);
				mCookDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 요리 숙련도
			{
				cell.strText.Format(
					_T("%d"),
					cookCount);
				mCookDataGrid.SetItem(
					&cell);
				mCookDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 요리 맛보기
			{
				cell.strText.Format(
					_T("%d"),
					eatCount);
				mCookDataGrid.SetItem(
					&cell);
				mCookDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 불 다루기
			{
				cell.strText.Format(
					_T("%d"),
					fireCount);
				mCookDataGrid.SetItem(
					&cell);
				mCookDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			break;
		}
	case MP_RM_PLAYER_GET_COOK_RECIPE_ACK:
		{
			const MSG_RM_COOK_RECIPE* const receivedMessage = (MSG_RM_COOK_RECIPE*)message;

			mApplication.UpdateRecipeListCtrl(
				*receivedMessage,
				mLearnedRecipeListCtrl,
				mForgottenRecipeListCtrl);
			break;
		}
	case MP_RM_PLAYER_GET_HOUSE_ACK:
		{
			mHouseDataGrid.ClearCells(
				CCellRange(0, 1));

			const MSG_RM_HOUSE* const receivedMessage = (MSG_RM_HOUSE*)message;

			const DWORD style = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
			GV_ITEM	cell = { 0 };
			cell.nFormat = style;
			cell.col = 1;
			cell.mask = GVIF_TEXT;

			// 이름
			{
				if(0 == strlen(receivedMessage->mHouseName))
				{
					CString textNothing;
					textNothing.LoadString(
						IDS_STRING203);

					cell.strText.Format(
						_T("%s"),
						textNothing = _T("*") + textNothing);

					mHouseDataGrid.EnableWindow(
						FALSE);
				}
				else
				{
					cell.strText.Format(
						_T("%s"),
						CString(receivedMessage->mHouseName));

					mHouseDataGrid.EnableWindow(
						TRUE);
				}
				
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 확장 레벨
			{
				cell.strText.Format(
					_T("%d"),
					receivedMessage->mExtendLevel);
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 총 방문 회수
			{
				cell.strText.Format(
					_T("%d"),
					receivedMessage->mTotalVisitCount);
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_READONLY);
			}

			// 일 방문 회수
			{
				cell.strText.Format(
					_T("%d"),
					receivedMessage->mDailyVisitCount);
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_READONLY);
			}

			// 꾸미기 포인트
			{
				cell.strText.Format(
					_T("%d"),
					receivedMessage->mDecorationPoint);
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			// 집 포인트
			{
				cell.strText.Format(
					_T("%d"),
					receivedMessage->mHousePoint);
				mHouseDataGrid.SetItem(
					&cell);
				mHouseDataGrid.SetItemState(
					cell.row++,
					cell.col,
					GVIS_MODIFIED);
			}

			break;
		}
	case MP_RM_PLAYER_ADD_COOK_RECIPE_ACK:
		{
			const MSG_DWORD* const receivedMessage = (MSG_DWORD*)message;
			const DWORD recipeIndex = receivedMessage->dwData;

			{
				TCHAR text[MAX_PATH] = {0};
				const int row = mLearnedRecipeListCtrl.GetItemCount();
				int step = -1;

				mLearnedRecipeListCtrl.InsertItem(
					row,
					_itot(mApplication.GetRecipeLevel(recipeIndex), text, 10),
					++step);
				mLearnedRecipeListCtrl.SetItemText(
					row,
					++step,
					_itot(recipeIndex, text, 10));
				mLearnedRecipeListCtrl.SetItemText(
					row,
					++step,
					mApplication.GetRecipeName(recipeIndex));
				mLearnedRecipeListCtrl.SetItemText(
					row,
					++step,
					_T("0"));
				mLearnedRecipeListCtrl.SetItemText(
					row,
					++step,
					_T("0"));
			}

			for(int row = 0; row < mForgottenRecipeListCtrl.GetItemCount(); ++row)
			{
				if(recipeIndex == _ttoi(mForgottenRecipeListCtrl.GetItemText(row, 1)))
				{
					mForgottenRecipeListCtrl.DeleteItem(
						row);
					break;
				}
			}

			break;
		}
	case MP_RM_PLAYER_REMOVE_COOK_RECIPE_ACK:
		{
			const MSG_DWORD* const receivedMessage = (MSG_DWORD*)message;
			const DWORD recipeIndex = receivedMessage->dwData;

			for(int row = 0; row < mLearnedRecipeListCtrl.GetItemCount(); ++row)
			{
				if(recipeIndex == _ttoi(mLearnedRecipeListCtrl.GetItemText(row, 1)))
				{
					mLearnedRecipeListCtrl.DeleteItem(
						row);
					break;
				}
			}

			{
				TCHAR text[MAX_PATH] = {0};
				const int row = mForgottenRecipeListCtrl.GetItemCount();
				int step = -1;

				mForgottenRecipeListCtrl.InsertItem(
					row,
					_itot(mApplication.GetRecipeLevel(recipeIndex), text, 10),
					++step);
				mForgottenRecipeListCtrl.SetItemText(
					row,
					++step,
					_itot(recipeIndex, text, 10));
				mForgottenRecipeListCtrl.SetItemText(
					row,
					++step,
					mApplication.GetRecipeName(recipeIndex));
				mForgottenRecipeListCtrl.SetItemText(
					row,
					++step,
					_T("0"));
				mForgottenRecipeListCtrl.SetItemText(
					row,
					++step,
					_T("0"));
			}

			break;
		}
	case MP_RM_PLAYER_SET_HOUSE_ACK:
	case MP_RM_PLAYER_SET_COOK_ACK:	
		{
			CString textSucceed;
			textSucceed.LoadString(
				IDS_STRING28);
			MessageBox(
				textSucceed,
				_T(""),
				MB_OK);
			break;
		}
	case MP_RM_PLAYER_GET_NOTE_LIST_ACK:
		{
			mMailBoxListCtrl.DeleteAllItems();

			CString textNothing;
			textNothing.LoadString(
				IDS_STRING203);
			textNothing = _T("*") + textNothing;

			const MSG_RM_NOTE_LIST* const receivedMessage = (MSG_RM_NOTE_LIST*)message;

			for(DWORD i = 0; i < receivedMessage->mSize; ++i)
			{
				const MSG_RM_NOTE_LIST::Note& note = receivedMessage->mNote[i];
				int step = -1;
				TCHAR text[MAX_PATH] = {0};

				mMailBoxListCtrl.InsertItem(
					i,
					_itot(note.mIndex, text, 10),
					++step);

				mMailBoxListCtrl.SetItemText(
					i,
					++step,
					CString(note.mDate));

				mMailBoxListCtrl.SetItemText(
					i,
					++step,
					0 == strlen(note.mTitle) ? textNothing : CString(note.mTitle));
			}

			break;
		}
	case MP_RM_PLAYER_GET_NOTE_ACK:
		{
			const MSG_RM_NOTE* const receivedMessage = (MSG_RM_NOTE*)message;
			const DWORD noteIndex = receivedMessage->mNoteIndex;

			NoteData& noteData = mNoteContainer[noteIndex];
			noteData.mMemo = receivedMessage->mNote;
			noteData.mSendedPlayer = receivedMessage->mSendedPlayer;
			noteData.mIsRead = receivedMessage->mIsRead;

			PutNote();
			break;
		}
	case MP_RM_PLAYER_GET_PARTY_ACK:
		{
			const MSG_RM_PARTY* const receivedMessage = (MSG_RM_PARTY*)message;
			mPartyCombo.ResetContent();

			CRect rect;
			mPartyCombo.GetWindowRect(
				rect);
			mPartyCombo.SetWindowPos(
				0,
				0,
				0,
				rect.Width(),
				200,
				SWP_NOMOVE | SWP_NOZORDER);
			mPartyCombo.EnableWindow(
				0 < receivedMessage->mSize);
			
			for(size_t i = 0; i < receivedMessage->mSize; ++i)
			{
				const MSG_RM_PARTY::Member& member = receivedMessage->mMember[i];

				TCHAR text[MAX_PATH] = {0};
				_sntprintf(
					text,
					_countof(text),
					_T("%s(%d) %s"),
					CString(member.mName),
					member.mPlayerIndex,
					1 < member.mRank ? _T("*") : _T(""));

				mPartyCombo.AddString(
					text);
			}

			mPartyCombo.SetCurSel(
				0);
			break;
		}
	default:
		{
			ASSERT( 0 && "It's not defined message" );
			break;
		}
	}
}
void CPlayerDialog::OnViewStatusBar()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	ShowWindow( SW_SHOW );
}


// 080526 LUJ, 버튼 하나로 모든 탭의 변경을 처리하도록 함
void CPlayerDialog::OnBnClickedPlayerUpdateButton()
{
	switch( mTabCtrl.GetCurSel() )
	{
	// 080525 LUJ, 플레이어 기본 정보
	case 0:
		{
			// 서버에 값 변경을 요청한다
			MSG_RM_PLAYER_DATA message;
			message = mReceivedDataMessage;

			// 셀의 데이터를 모두 읽는다. 주의: 순서를 잘 맞춰야한다...
			// 변경되어서는 안될 데이터는 셀에 read only 처리를 하자
			{
				MSG_RM_PLAYER_DATA::Player& player = message.mPlayer;

				player.mStrength				= _ttoi( mDataGrid.GetItemText( 0, 1 ) );
				player.mDexterity				= _ttoi( mDataGrid.GetItemText( 1, 1 ) );
				player.mVitality				= _ttoi( mDataGrid.GetItemText( 2, 1 ) );
				player.mIntelligence			= _ttoi( mDataGrid.GetItemText( 3, 1 ) );
				player.mWisdom					= _ttoi( mDataGrid.GetItemText( 4, 1 ) );
				player.mLife					= _ttoi( mDataGrid.GetItemText( 5, 1 ) );
				player.mMana					= _ttoi( mDataGrid.GetItemText( 6, 1 ) );
				player.mGrade					= _ttoi( mDataGrid.GetItemText( 7, 1 ) );
				player.mMaxGrade				= _ttoi( mDataGrid.GetItemText( 8, 1 ) );
				player.mExp						= _ttoi( mDataGrid.GetItemText( 9, 1 ) );
				player.mGradeUpPoint			= _ttoi( mDataGrid.GetItemText( 10, 1 ) );
				player.mMoney					= _tcstoul( mDataGrid.GetItemText( 11, 1 ), 0, 10 );
				// 081027 LUJ, 그리드 위치가 잘못되어 캐릭터 정보를 잘못 담는 문제 수정
				player.mSkillPoint				= _ttoi( mDataGrid.GetItemText( 12, 1 ) );
				player.mExtendedInventorySize	= _ttoi( mDataGrid.GetItemText( 13, 1 ) );
				player.mBadFame					= _ttoi( mDataGrid.GetItemText( 14, 1 ) );
			}

			{
				MSG_RM_PLAYER_DATA::Position& position = message.mPosition;

				// 081027 LUJ, 그리드 위치가 잘못되어 캐릭터 정보를 잘못 담는 문제 수정
				position.mMap	= _ttoi( mDataGrid.GetItemText( 15, 1 ) );
				position.mX		= _ttoi( mDataGrid.GetItemText( 16, 1 ) );
				position.mY		= _ttoi( mDataGrid.GetItemText( 17, 1 ) );
			}

			if( ! memcmp( &message, &mReceivedDataMessage, sizeof( message ) ) )
			{
				return;
			}

			message.Category	= MP_RM_PLAYER;
			message.Protocol	= MP_RM_PLAYER_SET_DATA_SYN;
			message.dwObjectID	= mPlayerIndex;

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	// 080525 LUJ, 플레이어 확장 정보
	case 1:
		{
			MSG_RM_PLAYER_EXTENDED_DATA message = mReceivedExtendedMessage;
			{
				message.dwObjectID	= mPlayerIndex;

				MSG_RM_PLAYER_EXTENDED_DATA::Player& data = message.mPlayer;

				const CString name		( mExtendedDataGrid.GetItemText( 0, 1 ) );
				const CString guildNick	( mExtendedDataGrid.GetItemText( 1, 1 ) );
				const CString familyNick( mExtendedDataGrid.GetItemText( 2, 1 ) );

				StringCopySafe( data.mName,			CW2AEX< MAX_PATH >( name ),			sizeof( data.mName ) );
				StringCopySafe( data.mGuildNick,	CW2AEX< MAX_PATH >( guildNick ),	sizeof( data.mGuildNick ) );
				StringCopySafe( data.mFamilyNick,	CW2AEX< MAX_PATH >( familyNick ),	sizeof( data.mFamilyNick ) );

				if( ! memcmp( &mReceivedExtendedMessage, &message, sizeof( message ) ) )
				{
					return;
				}
			}

			message.Protocol	= MP_RM_PLAYER_SET_EXTENDED_DATA_SYN;
			message.mPlayer.mIndex	= mPlayerIndex;

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
	// 080525 LUJ, 낚시 정보
	case 2:
		{
			MSG_DWORD3 message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_RM_PLAYER;
			message.Protocol	= MP_RM_PLAYER_SET_FISHING_SYN;
			message.dwObjectID	= mPlayerIndex;
			message.dwData1		= _ttoi( mFishingDataGrid.GetItemText( 0, 1 ) );
			message.dwData2		= _ttoi( mFishingDataGrid.GetItemText( 1, 1 ) );
			message.dwData3		= _ttoi( mFishingDataGrid.GetItemText( 2, 1 ) );

			mApplication.Send( mServerIndex, message, sizeof( message ) );
			break;
		}
		// 요리 정보
	case 3:
		{
			const LEVELTYPE cookLevel = _ttoi(mCookDataGrid.GetItemText(1, 1));
			const DWORD cookCount = _ttoi(mCookDataGrid.GetItemText(2, 1));
			const DWORD eatCount = _ttoi(mCookDataGrid.GetItemText(3, 1));
			const DWORD fireCount = _ttoi(mCookDataGrid.GetItemText(4, 1));

			if(mApplication.GetMaxCookLevel() < cookLevel)
			{
				CString textInformation;
				textInformation.LoadString(
					IDS_STRING69);
				CString textFormat;
				textFormat.LoadString(
					IDS_STRING498);
				CString textMessage;
				textMessage.Format(
					textFormat,
					mApplication.GetMaxCookLevel());

				MessageBox(
					textMessage,
					textInformation,
					MB_OK);
				break;
			}
			
			{
				
				CString textCookCount;
				textCookCount.LoadString(
					IDS_STRING488);
				CString textEatCount;
				textEatCount.LoadString(
					IDS_STRING489);
				CString textFireCount;
				textFireCount.LoadString(
					IDS_STRING490);
				CString textMax;
				textMax.LoadString(
					IDS_STRING499);

				CString textMessage1;
				textMessage1.Format(
					_T("%s: %d (%s: %d)\n"),
					textCookCount,
					cookCount,
					textMax,
					mApplication.GetMaxCookCount(cookLevel));

				CString textMessage2;
				textMessage2.Format(
					_T("%s: %d (%s: %d)\n"),
					textEatCount,
					eatCount,
					textMax,
					mApplication.GetMaxEatCount(cookLevel));

				CString textMessage3;
				textMessage3.Format(
					_T("%s: %d (%s: %d)\n"),
					textFireCount,
					fireCount,
					textMax,
					mApplication.GetMaxFireCount(cookLevel));

				CString textInformation;
				textInformation.LoadString(
					IDS_STRING69);
				CString textMessage4;
				textMessage4.LoadString(
					IDS_STRING500);
				
				if(IDNO == MessageBox(textMessage1 + textMessage2 + textMessage3 + textMessage4, textInformation, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
				{
					break;
				}
			}

			MSG_DWORD4 message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_RM_PLAYER;
			message.Protocol = MP_RM_PLAYER_SET_COOK_SYN;
			message.dwObjectID = mPlayerIndex;
			message.dwData1 = cookLevel;
			message.dwData2 = cookCount;
			message.dwData3 = eatCount;
			message.dwData4 = fireCount;

			mApplication.Send(
				mServerIndex,
				message,
				sizeof(message));
			break;
		}
		// 집 정보
	case 4:
		{
			CString textInformation;
			textInformation.LoadString(
				IDS_STRING69);
			CString textMessage;
			textMessage.LoadString(
				IDS_STRING500);

			if(IDNO == MessageBox(textMessage, textInformation, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
			{
				break;
			}

			MSG_RM_HOUSE message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_RM_PLAYER;
			message.Protocol = MP_RM_PLAYER_SET_HOUSE_SYN;
			message.dwObjectID = mUserIndex;
			StringCopySafe(
				message.mHouseName,
				CW2AEX< MAX_PATH >(mHouseDataGrid.GetItemText(0, 1)),
				_countof(message.mHouseName));
			message.mExtendLevel = _ttoi(mHouseDataGrid.GetItemText(1, 1));
			message.mDecorationPoint = _ttoi(mHouseDataGrid.GetItemText(4, 1));
			message.mHousePoint = _ttoi(mHouseDataGrid.GetItemText(5, 1));

			mApplication.Send(
				mServerIndex,
				message,
				sizeof(message));
			break;
		}
	}
}


void CPlayerDialog::Request( DWORD serverIndex, const CString& playerName, DWORD playerIndex )
{
	mServerIndex	= serverIndex;

	Request( mApplication.GetServerName( mServerIndex ), playerName, playerIndex );
}


void CPlayerDialog::Request( const CString& serverName, const CString& playerName, DWORD playerIndex )
{
	mPlayerIndex	= playerIndex;
	mPlayerName		= playerName;
	mServerName		= serverName;
	
	{
		CString	textTitleFormat;
		textTitleFormat.LoadString( IDS_STRING181 );

		CString text;
		text.Format( textTitleFormat, mServerName, playerName, playerIndex );

		SetWindowText( text );
	}

	// 데이터가 오기전까지 컨트롤을 잠근다
	{
		mTabCtrl.EnableWindow( FALSE );
	}

	// 080328 LUJ, 초기화
	{
		CString textNothing;
		textNothing.LoadString( IDS_STRING203 );

		mGuildButton.SetWindowText( textNothing );
		mGuildButton.EnableWindow( FALSE );

		mFamilyButton.SetWindowText( textNothing );
		mFamilyButton.EnableWindow( FALSE );

		mExtendedDataGrid.ClearCells( CCellRange( 0, 1 ) );
	}

	{
		MSG_RM_GET_PLAYER_SYN message;
		ZeroMemory( &message, sizeof( message ) );

		message.Category = MP_RM_PLAYER;
		message.Protocol = MP_RM_PLAYER_GET_DATA_SYN;
		message.mPlayerIndex = playerIndex;

		mApplication.Send( mServerIndex, message, sizeof( message ) );
	}
}


void CPlayerDialog::OnBnClickedPlayerLicenseUpdateButtno()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	MSG_RM_PLAYER_LOVE	message;
	{
		ASSERT( sizeof( message ) == sizeof( mReceivedLicenseMessage ) );

		memcpy( &message, &mReceivedLicenseMessage, sizeof( message ) );

		message.Category	= MP_RM_PLAYER;
		message.Protocol	= MP_RM_PLAYER_SET_LICENSE_SYN;
	}

	DATE_MATCHING_INFO&	data	= message.mDateMatching;

	{
		CString text;
		mAgeEdit.GetWindowText( text);
		data.dwAge		= _ttoi( text );

		mAreaCombo.GetLBText( mAreaCombo.GetCurSel(), text );
		int start = 0;
		data.dwRegion	= _ttoi( text.Tokenize( _T( " " ), start ) );

		mLoveScoreEdit.GetWindowText( text );
		data.dwGoodFeelingDegree	= _ttoi( text );
	}

	std::set< DWORD > favorite;

	// 선호 항목 트리 컨트롤의 값을 담는다
	{
		int			i		= 0;
		const DWORD size	= sizeof( data.pdwGoodFeeling ) / sizeof( *data.pdwGoodFeeling );

        for(	HTREEITEM item = mGoodThingsTree.GetFirstVisibleItem();
				item;
				item = mGoodThingsTree.GetNextVisibleItem( item ) )
		{
			if( ! mGoodThingsTree.GetCheck( item ) )
			{
				continue;
			}
			else if( i > size )
			{
				CString textSelectSomeFavoriteThing;
				textSelectSomeFavoriteThing.LoadString( IDS_STRING182 );

				CString text;
				text.Format( textSelectSomeFavoriteThing, size );

				MessageBox( text, _T( "" ), MB_ICONERROR | MB_OK );
				return;
			}

			CString text	= mGoodThingsTree.GetItemText( item );
			int		start	= 0;

			const DWORD index = _ttoi( text.Tokenize( _T( " " ), start ) );
			data.pdwGoodFeeling[ i ] = index;

			favorite.insert( index );

			++i;
		}

		if( i < size )
		{
			CString textSelectSomeFavoriteThing;
			textSelectSomeFavoriteThing.LoadString( IDS_STRING182 );

			CString text;
			text.Format( textSelectSomeFavoriteThing, size );

			MessageBox( text, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
	}

	// 비선호 항목 트리 컨트롤의 값을 담는다
	{
		int			i		= 0;
		const DWORD size	= sizeof( data.pdwBadFeeling ) / sizeof( *data.pdwBadFeeling );

		for(	HTREEITEM item = mBadThingsTree.GetFirstVisibleItem();
				item;
				item = mBadThingsTree.GetNextVisibleItem( item ) )
		{
			if( ! mBadThingsTree.GetCheck( item ) )
			{
				continue;
			}
			else if( i > size )
			{				
				CString textSelectSomeUnfavoriteThing;
				textSelectSomeUnfavoriteThing.LoadString( IDS_STRING183 );

				CString text;
				text.Format( textSelectSomeUnfavoriteThing, size );

				MessageBox( text, _T( "" ), MB_ICONERROR | MB_OK );
				return;
			}

			CString text			= mBadThingsTree.GetItemText( item );
			int		start			= 0;
			const DWORD index = _ttoi( text.Tokenize( _T( " " ), start ) );

			if( favorite.end() != favorite.find( index ) )
			{
				CString textDoNotSelectFavoriteAndUnfavoriteThingTogether;
				textDoNotSelectFavoriteAndUnfavoriteThingTogether.LoadString( IDS_STRING184 );

				MessageBox( textDoNotSelectFavoriteAndUnfavoriteThingTogether, _T( "" ), MB_ICONERROR | MB_OK );
				return;
			}

			data.pdwBadFeeling[ i ] = index;

			++i;
		}

		if( i < size )
		{
			CString textSelectSomeUnfavoriteThing;
			textSelectSomeUnfavoriteThing.LoadString( IDS_STRING183 );

			CString text;
			text.Format( textSelectSomeUnfavoriteThing, size );

			MessageBox( text, _T( "" ), MB_ICONERROR | MB_OK );
			return;
		}
	}

	ASSERT( sizeof( message ) == sizeof( mReceivedLicenseMessage ) );

	if( ! memcmp( &message, &mReceivedLicenseMessage, sizeof( message ) ) )
	{
		return;
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CPlayerDialog::OnBnClickedPlayerExpLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textExperience;
		textExperience.LoadString( IDS_STRING9 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textExperience,
			textLog,
			mPlayerName,
			mPlayerIndex );

		dialog->DoModal(
			mServerIndex,
			CExperienceLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


void CPlayerDialog::OnBnClickedPlayerStatLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textPlayer;
		textPlayer.LoadString( IDS_STRING54 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textPlayer,
			textLog,
			mPlayerName,
			mPlayerIndex );

		dialog->DoModal(
			mServerIndex,
			CStatLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


void CPlayerDialog::OnBnClickedPlayerGuildButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	const char* name = mReceivedDataMessage.mGuild.mName;

	if( ! *name )
	{
		CString textThereIsNoResult;
		textThereIsNoResult.LoadString( IDS_STRING1 );

		MessageBox( textThereIsNoResult, _T( "" ), MB_ICONERROR | MB_OK );
		return;
	}

	MSG_NAME_DWORD message;
	{
		ZeroMemory( &message, sizeof( message ) );

		message.Category	= MP_RM_GUILD;
		message.Protocol	= MP_RM_GUILD_GET_DATA_SYN;
		strncpy( message.Name, name, sizeof( message.Name ) );
	}

	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CPlayerDialog::OnBnClickedPlayerUserViewButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString userName;
	mUserNameEdit.GetWindowText( userName );

	CUserDialog* dialog = mApplication.GetUserDialog();
	ASSERT( dialog );

	dialog->Request( mServerIndex, userName, mUserIndex );
}


void CPlayerDialog::OnBnClickedPlayerJobButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textJob;
		textJob.LoadString( IDS_STRING126 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textJob,
			textLog,
			mPlayerName,
			mPlayerIndex );

		dialog->DoModal(
			mServerIndex,
			CJobLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


void CPlayerDialog::OnBnClickedPlayerItemLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CItemLogCommand::Configuration configuration = { 0 };
	configuration.mPlayerIndex	= mPlayerIndex;

	CLogDialog* logDialog = mApplication.GetLogDialog();

	if( logDialog )
	{
		CString textItem;
		textItem.LoadString( IDS_STRING189 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString textPlayerIndex;
		textPlayerIndex.Format( _T( "%d" ), mPlayerIndex );

		const CString title( textItem + _T( " " ) + textLog + _T( ": " ) + mPlayerName + _T( "(" ) + textPlayerIndex + _T( ")" ) );

		logDialog->DoModal(
			mServerIndex,
			CItemLogCommand( mApplication, title, configuration ) );
	}
}


void CPlayerDialog::OnBnClickedPlayerJobSubmitButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	MSG_RM_PLAYER_DATA message = mReceivedDataMessage;
	
	{
		// 이해를 위해서는 클래스 설정 참조할것
		MSG_RM_PLAYER_DATA::Job& job = message.mJob;

		//job.mIndex	= mJob1Combo.GetCurSel();	// 계열
		job.mData1	= mReceivedDataMessage.mJob.mData1;
		job.mData2	= mJob2Combo.GetCurSel();	// 2차 직업: 계열에 따라 인덱스로 정해짐
		job.mData3	= mJob3Combo.GetCurSel();	// 3차 직업
		job.mData4	= mJob4Combo.GetCurSel();	// 4차 직업
		job.mData5	= mJob5Combo.GetCurSel();	// 5차 직업
		job.mData6	= mJob6Combo.GetCurSel();	// 6차 직업

		// 계열 차수. 값이 있는 필드 위치를 알려준다.
		job.mIndex = 1 + ( job.mData2 ? 1 : 0 ) + ( job.mData3 ? 1 : 0 ) + ( job.mData4 ? 1 : 0 ) + ( job.mData5 ? 1 : 0 ) ;
		
		const DWORD maxJobSize	= 6;
		BOOL		isVaild		= FALSE;

		// 중간에 빈 값이 있는지 체크한다
		// 스위치 문을 goto처럼 사용하여 각 직업 차수에 값이 있는지 체크한다.
		switch( maxJobSize - job.mIndex )
		{
		case 0:
			if( ! job.mData6 )
			{
				break;
			}
		case 1:
			if( ! job.mData5 )
			{
				break;
			}
		case 2:
			if( ! job.mData4 )
			{
				break;
			}
		case 3:
			if( ! job.mData3 )
			{
				break;
			}
		case 4:
			if( ! job.mData2 )
			{
				break;
			}
		case 5:
			if( ! job.mData1 )
			{
				break;
			}

			isVaild = TRUE;
		}

		if( ! isVaild )
		{
			CString textFillAllData;
			textFillAllData.LoadString( IDS_STRING185 );

			MessageBox( textFillAllData, _T( "" ), MB_ICONERROR | MB_OK );

			Parse( mServerIndex, &mReceivedDataMessage );
			return;
		}
	}

	// 변경된 내역이 없으면 바꾸지 않는다
	if( ! memcmp( &message, &mReceivedDataMessage, sizeof( message ) ) )
	{
		return;
	}

	CString textWarningBeforeChangingJob;
	textWarningBeforeChangingJob.LoadString( IDS_STRING186 );
    
	if( IDNO == MessageBox( textWarningBeforeChangingJob, _T( "" ), MB_ICONINFORMATION | MB_OK ) )
	{
		return;
	}
	
	message.Protocol	= MP_RM_PLAYER_SET_DATA_SYN;
	message.dwObjectID	= mPlayerIndex;
	
	mApplication.Send( mServerIndex, message, sizeof( message ) );
}


void CPlayerDialog::OnCbnSelchangePlayerJob1Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 첫번째 직업은 무조건 선택되어야 한다
	if( ! mJob1Combo.GetCurSel() )
	{
		mJob1Combo.SetCurSel( 1 );
		return;
	}
}

void CPlayerDialog::OnCbnSelchangePlayerJob2Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 직업이 선택되어야 변경 가능
	if( ! mJob1Combo.GetCurSel() )
	{
		mJob2Combo.SetCurSel( 0 );
		return;
	}
	else if( ! mJob2Combo.GetCurSel() )
	{
		mJob3Combo.SetCurSel( 0 );
		mJob4Combo.SetCurSel( 0 );
		mJob5Combo.SetCurSel( 0 );
		mJob6Combo.SetCurSel( 0 );
	}
}

void CPlayerDialog::OnCbnSelchangePlayerJob3Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 직업이 선택되어야 변경 가능
	if( ! mJob2Combo.GetCurSel() )
	{
		mJob3Combo.SetCurSel( 0 );
		return;
	}
	else if( ! mJob3Combo.GetCurSel() )
	{
		mJob4Combo.SetCurSel( 0 );
		mJob5Combo.SetCurSel( 0 );
		mJob6Combo.SetCurSel( 0 );
	}
}

void CPlayerDialog::OnCbnSelchangePlayerJob4Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 직업이 선택되어야 변경 가능
	if( ! mJob3Combo.GetCurSel() )
	{
		mJob4Combo.SetCurSel( 0 );
		return;
	}
	else if( ! mJob4Combo.GetCurSel() )
	{
		mJob5Combo.SetCurSel( 0 );
		mJob6Combo.SetCurSel( 0 );
	}
}

void CPlayerDialog::OnCbnSelchangePlayerJob5Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 직업이 선택되어야 변경 가능
	if( ! mJob4Combo.GetCurSel() )
	{
		mJob5Combo.SetCurSel( 0 );
		return;
	}
	else if( ! mJob5Combo.GetCurSel() )
	{
		mJob6Combo.SetCurSel( 0 );
	}
}

void CPlayerDialog::OnCbnSelchangePlayerJob6Combo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// 이전 직업이 선택되어야 변경 가능
	if( ! mJob5Combo.GetCurSel() )
	{
		mJob6Combo.SetCurSel( 0 );
		return;
	}
}


void CPlayerDialog::OnTcnSelchangePlayerDataTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	mDataGrid.ShowWindow(
		SW_HIDE);
	mExtendedDataGrid.ShowWindow(
		SW_HIDE);
	mFishingDataGrid.ShowWindow(
		SW_HIDE);
	mCookDataGrid.ShowWindow(
		SW_HIDE);
	mHouseDataGrid.ShowWindow(
		SW_HIDE);

	mLearnedRecipeListCtrl.ShowWindow(
		SW_HIDE);
	mForgottenRecipeListCtrl.ShowWindow(
		SW_HIDE);

	mMailBoxListCtrl.ShowWindow(
		SW_HIDE);

	if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON))
	{
		window->ShowWindow(
			SW_HIDE);
	}

	if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON2))
	{
		window->ShowWindow(
			SW_HIDE);
	}

	if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_STATIC))
	{
		window->ShowWindow(
			SW_HIDE);
	}

	if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_ADD_BUTTON))
	{
		window->ShowWindow(
			SW_HIDE);
	}

	if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_REMOVE_BUTTON))
	{
		window->ShowWindow(
			SW_HIDE);
	}

	switch( mTabCtrl.GetCurSel() )
	{
	case 0:
		{
			mDataGrid.ShowWindow( SW_SHOW );
			break;
		}
	case 1:
		{
			mExtendedDataGrid.ShowWindow( SW_SHOW );
			break;
		}
	case 2:
		{
			mFishingDataGrid.ShowWindow( SW_SHOW );

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON))
			{
				CString textTitle;
				textTitle.LoadString(
					IDS_STRING379);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);

				window->SetWindowText(
					textTitle + _T(" ") + textLog + _T("(&L)"));
				window->ShowWindow(
					SW_SHOW);
			}

			break;
		}
	case 3:
		{
			mCookDataGrid.ShowWindow(
				SW_SHOW);

			mLearnedRecipeListCtrl.ShowWindow(
				SW_SHOW);
			mForgottenRecipeListCtrl.ShowWindow(
				SW_SHOW);

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON))
			{
				CString textTitle;
				textTitle.LoadString(
					IDS_STRING492);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);

				window->SetWindowText(
					textTitle + _T(" ") + textLog + _T("(&L)"));
				window->ShowWindow(
					SW_SHOW);
			}

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON2))
			{
				CString textTitle;
				textTitle.LoadString(
					IDS_STRING491);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);

				window->SetWindowText(
					textTitle + _T(" ") + textLog + _T("(&L)"));
				window->ShowWindow(
					SW_SHOW);
			}

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_STATIC))
			{
				window->ShowWindow(
					SW_SHOW);
			}

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_ADD_BUTTON))
			{
				window->ShowWindow(
					SW_SHOW);
			}

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_COOK_RECIPE_REMOVE_BUTTON))
			{
				window->ShowWindow(
					SW_SHOW);
			}

			break;
		}
	case 4:
		{
			mHouseDataGrid.ShowWindow(
				SW_SHOW);

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON))
			{
				CString textTitle;
				textTitle.LoadString(
					IDS_STRING476);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);

				window->SetWindowText(
					textTitle + _T(" ") + textLog + _T("(&L)"));
				window->ShowWindow(
					SW_SHOW);
			}

			break;
		}
	case 5:
		{
			mMailBoxListCtrl.ShowWindow(
				SW_SHOW);

			if(CWnd* const window = GetDlgItem(IDC_PLAYER_TAB_LOG_BUTTON))
			{
				CString textConsign;
				textConsign.LoadString(
					IDS_STRING522);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);

				window->SetWindowText(
					textConsign + _T(" ") + textLog + _T("(&L)"));
				window->ShowWindow(
					SW_SHOW);
			}

			break;
		}
	}
}


void CPlayerDialog::OnBnClickedPlayerNameLogButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textName;
		textName.LoadString( IDS_STRING20 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textName,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CNameLogCommand::Configuration findConfiguration;
		findConfiguration.mPlayerIndex	= mPlayerIndex;

		dialog->DoModal(
			mServerIndex,
			CNameLogCommand( mApplication, title, findConfiguration ) );
	}
}


// 080328 LUJ, 상세한 패밀리 정보를 요청한다
void CPlayerDialog::OnBnClickedPlayerFamilyButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFamilyDialog* dialog = mApplication.GetFamilyDialog();

	if( ! dialog )
	{
		return;
	}

	dialog->Request( mServerIndex, mPlayerName, mFamilyIndex );
}


// 080523 LUJ, 낚시 로그 창을 표시한다
void CPlayerDialog::OnBnClickedFishLogButton()
{
	CLogDialog* dialog = mApplication.GetLogDialog();
	
	if( dialog )
	{
		CString textFishing;
		textFishing.LoadString( IDS_STRING379 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textFishing,
			textLog,
			mPlayerName,
			mPlayerIndex );

		dialog->DoModal(
			mServerIndex,
			CFishLogCommand( mApplication, title, mPlayerIndex ) );
	}
}


// 080716 LUJ, 펫 로그 창 표시
void CPlayerDialog::OnBnClickedPlayerPetLogButton()
{
	CLogDialog* dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textPet;
		textPet.LoadString( IDS_STRING280 );

		CString textLog;
		textLog.LoadString( IDS_STRING10 );

		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textPet,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CPetLogCommand::Configuration configuration = { 0 };
		configuration.mUserIndex = mUserIndex;

		dialog->DoModal(
			mServerIndex,
			CPetLogCommand( mApplication, title, configuration ) );
	}
}

void CPlayerDialog::OnBnClickedPlayerFamilyLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textFamily;
		textFamily.LoadString( IDS_STRING165 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textFamily,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CFamilyLogCommand::Configuration configuration = { 0 };
		configuration.mPlayerIndex = mPlayerIndex;
		dialog->DoModal(
			mServerIndex,
			CFamilyLogCommand( mApplication, title, configuration ) );
	}
}

void CPlayerDialog::OnBnClickedPlayerLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textGuild;
		textGuild.LoadString( IDS_STRING164 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textGuild,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CGuildLogCommand::Configuration configuration = { 0 };
		configuration.mPlayerIndex = mPlayerIndex;
		dialog->DoModal(
			mServerIndex,
			CGuildLogCommand( mApplication, title, configuration ) );
	}
}

void CPlayerDialog::OnBnClickedPlayerMoneyLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textMoney;
		textMoney.LoadString( IDS_STRING58 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textMoney,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CMoneyLogCommand::Configuration configuration;
		configuration.mPlayerIndex = mPlayerIndex;
		dialog->DoModal(
			mServerIndex,
			CMoneyLogCommand( mApplication, title, configuration ) );
	}
}
void CPlayerDialog::OnBnClickedPlayerAutonoteLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textAutoNote;
		textAutoNote.LoadString( IDS_STRING440 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textAutoNote,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CAutoNoteLogCommand::Configuration configuration;
		configuration.mPlayerIndex = mPlayerIndex;
		dialog->DoModal(
			mServerIndex,
			CAutoNoteLogCommand( mApplication, title, configuration ) );
	}
}

void CPlayerDialog::OnBnClickedPlayerHouse()
{
    CLogDialog* const dialog = mApplication.GetLogDialog();

	if(0 == dialog)
	{
		return;
	}

	CString textHouse;
	textHouse.LoadString(
		IDS_STRING476);
	CString textLog;
	textLog.LoadString(
		IDS_STRING10);
	CString textUser;
	textUser.LoadString(
		IDS_STRING301);
	CString textTitle;
	textTitle.Format(
		_T("%s %s: %s(%s: %d)"),
		textHouse,
		textLog,
		mPlayerName,
		textUser,
		mUserIndex);

	CHouseLogCommand::Configuration configuration = {0};
	configuration.mUserIndex = mUserIndex;
	dialog->DoModal(
		mServerIndex,
		CHouseLogCommand(mApplication, textTitle, configuration));
}

void CPlayerDialog::OnBnClickedPlayerCookRecipeAddButton()
{
	POSITION position = mForgottenRecipeListCtrl.GetFirstSelectedItemPosition();
	const DWORD recipeIndex = _ttoi(mForgottenRecipeListCtrl.GetItemText(
		mForgottenRecipeListCtrl.GetNextSelectedItem(position),
		1));

	if(0 == _tcslen(mApplication.GetRecipeName(recipeIndex)))
	{
		return;
	}

	const DWORD slot = 0;
	const DWORD remainTime = 0;

	MSG_DWORD3 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_ADD_COOK_RECIPE_SYN;
	message.dwObjectID = mPlayerIndex;
	message.dwData1 = recipeIndex;
	message.dwData2 = slot;
	message.dwData3 = remainTime;

	mApplication.Send(
		mServerIndex,
		message,
		sizeof(message));
}

void CPlayerDialog::OnBnClickedPlayerCookRecipeRemoveButton()
{
	POSITION position = mLearnedRecipeListCtrl.GetFirstSelectedItemPosition();
	const DWORD recipeIndex = _ttoi(mLearnedRecipeListCtrl.GetItemText(
		mLearnedRecipeListCtrl.GetNextSelectedItem(position),
		1));

	if(0 == _tcslen(mApplication.GetRecipeName(recipeIndex)))
	{
		return;
	}

	MSG_DWORD message;
	message.Category = MP_RM_PLAYER;
	message.Protocol = MP_RM_PLAYER_REMOVE_COOK_RECIPE_SYN;
	message.dwObjectID = mPlayerIndex;
	message.dwData = recipeIndex;

	mApplication.Send(
		mServerIndex,
		message,
		sizeof(message));
}

void CPlayerDialog::OnBnClickedPlayerTabLogButton()
{
	switch(mTabCtrl.GetCurSel())
	{
	case 2:
		{
			if(CLogDialog* dialog = mApplication.GetLogDialog())
			{
				CString textFishing;
				textFishing.LoadString( IDS_STRING379 );

				CString textLog;
				textLog.LoadString( IDS_STRING10 );

				CString title;
				title.Format(
					_T( "%s %s: %s(%d)" ),
					textFishing,
					textLog,
					mPlayerName,
					mPlayerIndex );

				dialog->DoModal(
					mServerIndex,
					CFishLogCommand( mApplication, title, mPlayerIndex ) );
			}

			break;
		}
	case 3:
		{
			if(CLogDialog* const dialog = mApplication.GetLogDialog())
			{
				CString textCook;
				textCook.LoadString(
					IDS_STRING492);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);
				CString textTitle;
				textTitle.Format(
					_T("%s %s: %s(%d)"),
					textCook,
					textLog,
					mPlayerName,
					mPlayerIndex);

				CCookLogCommand::Configuration configuration = {0};
				configuration.mPlayerIndex = mPlayerIndex;
				dialog->DoModal(
					mServerIndex,
					CCookLogCommand(mApplication, textTitle, configuration));
			}

			break;
		}
	case 4:
		{
			if(CLogDialog* const dialog = mApplication.GetLogDialog())
			{
				CString textHouse;
				textHouse.LoadString(
					IDS_STRING476);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);
				CString textUser;
				textUser.LoadString(
					IDS_STRING301);
				CString textTitle;
				textTitle.Format(
					_T("%s %s: %s(%s: %d)"),
					textHouse,
					textLog,
					mPlayerName,
					textUser,
					mUserIndex);

				CHouseLogCommand::Configuration configuration = {0};
				configuration.mUserIndex = mUserIndex;
				dialog->DoModal(
					mServerIndex,
					CHouseLogCommand(mApplication, textTitle, configuration));
			}

			break;
		}
	case 5:
		{
			if(CLogDialog* const dialog = mApplication.GetLogDialog())
			{
				CString textConsign;
				textConsign.LoadString(
					IDS_STRING522);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);
				CString textTitle;
				textTitle.Format(
					_T("%s %s: %s(%d)"),
					textConsign,
					textLog,
					mPlayerName,
					mPlayerIndex);

				CConsignLogCommand::Configuration configuration = {0};
				configuration.mPlayerIndex = mPlayerIndex;
				configuration.mItemDBIndex = 0;
				configuration.mConsignIndex = 0;
				dialog->DoModal(
					mServerIndex,
					CConsignLogCommand(mApplication, textTitle, configuration));
			}

			break;
		}
	}
}

void CPlayerDialog::OnBnClickedPlayerTabLogButton2()
{
	switch(mTabCtrl.GetCurSel())
	{
	case 3:
		{
			if(CLogDialog* const dialog = mApplication.GetLogDialog())
			{
				CString textCook;
				textCook.LoadString(
					IDS_STRING491);
				CString textLog;
				textLog.LoadString(
					IDS_STRING10);
				CString textTitle;
				textTitle.Format(
					_T("%s %s: %s(%d)"),
					textCook,
					textLog,
					mPlayerName,
					mPlayerIndex);

				CRecipeLogCommand::Configuration configuration = {0};
				configuration.mPlayerIndex = mPlayerIndex;
				dialog->DoModal(
					mServerIndex,
					CRecipeLogCommand(mApplication, textTitle, configuration));
			}

			break;
		}
	}
}

void CPlayerDialog::OnLvnItemchangedPlayerMailBoxListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	POSITION position = mMailBoxListCtrl.GetFirstSelectedItemPosition();
	const DWORD noteIndex = _ttoi(mMailBoxListCtrl.GetItemText(
		mForgottenRecipeListCtrl.GetNextSelectedItem(position),
		0));

	if(0 == noteIndex)
	{
		return;
	}
	else if(mNoteContainer.end() == mNoteContainer.find(noteIndex))
	{
		MSGBASE message;
		message.Category = MP_RM_PLAYER;
		message.Protocol = MP_RM_PLAYER_GET_NOTE_SYN;
		message.dwObjectID = noteIndex;

		mApplication.Send(
			mServerIndex,
			message,
			sizeof(message));
		return;
	}

	PutNote();
}

void CPlayerDialog::PutNote() const
{
	POSITION position = mMailBoxListCtrl.GetFirstSelectedItemPosition();
	const DWORD noteIndex = _ttoi(mMailBoxListCtrl.GetItemText(
		mForgottenRecipeListCtrl.GetNextSelectedItem(position),
		0));
	const NoteContainer::const_iterator iterator = mNoteContainer.find(
		noteIndex);

	const NoteData& noteData = iterator->second;

	CString textSeal;
	textSeal.LoadString(
		IDS_STRING272);
	CString	textUnseal;
	textUnseal.LoadString(
		IDS_STRING273);

	CString textMessage;
	textMessage.Format(
		_T("%s(%s)\n\n%s"),
		noteData.mSendedPlayer,
		noteData.mIsRead ? textUnseal : textSeal,
		noteData.mMemo,
		mApplication.GetItemName(noteData.mPackageItemIndex));

	mApplication.GetMemoDialog()->SetText(
		textMessage);
	mApplication.GetMemoDialog()->ShowWindow(
		SW_SHOW);
}

void CPlayerDialog::OnBnClickedPlayerForbidChatLogButton()
{
	CLogDialog* const dialog = mApplication.GetLogDialog();

	if( dialog )
	{
		CString textForbidChat;
		textForbidChat.LoadString( IDS_STRING536 );
		CString textLog;
		textLog.LoadString( IDS_STRING10 );
		CString title;
		title.Format(
			_T( "%s %s: %s(%d)" ),
			textForbidChat,
			textLog,
			mPlayerName,
			mPlayerIndex );

		CForbidChatLogCommand::Configuration configuration;
		configuration.mPlayerIndex = mPlayerIndex;
		dialog->DoModal(
			mServerIndex,
			CForbidChatLogCommand( mApplication, title, configuration ) );
	}
}