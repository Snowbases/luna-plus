#include "StdAfx.h"
#include "cWindowManager.h"
#include "cScriptManager.h"
#include "GameResourceManager.h"
#include "../GlobalEventFunc.h"
#include "../GameIn.h"
#include "../WindowIDEnum.h"
#include "../ObjectManager.h"
#include "../CharacterDialog.h"
#include "../ItemManager.h"
#include "../InventoryExDialog.h"
#include "../MoneyDlg.h"
#include "../MousePointer.h"
#include "../input/cIMEWnd.h"
#include "cAnimationManager.h"
#include "../MiniMapDlg.h"
#include "../BigMapDlg.h"
#include "../MHMap.h"
#include "../cMsgBox.h"
#include "../cDivideBox.h"
#include "../cmoneydividebox.h"
#include "cWindowSystemFunc.h"
#include "mhFile.h"
#include "ComposeDialog.h"
#include "GuildCreateDialog.h"
#include "GuildDialog.h"
#include "GuildInviteDialog.h"
#include "GuildMarkDialog.h"
#include "GuildLevelUpDialog.h"
#include "GuildNickNameDialog.h"
#include "GuildRankDialog.h"
#include "GuildWarehouseRankDialog.h"
#include "GuildWarehouseDialog.h"
#include "StorageDialog.h"
#include "OptionDialog.h"
#include "NoteDialog.h"
#include "FriendDialog.h"
#include "MiniFriendDialog.h"
#include "MiniNoteDialog.h"
#include "MiniNoteDialogRead.h"
#include "ReviveDialog.h"
#include "ExchangeDialog.h"
#include "StallKindSelectDlg.h"
#include "StreetBuyStall.h"
#include "BuyRegDialog.h"
#include "StreetStall.h"
#include "NpcScriptDialog.h"
#include "DealDialog.h"
#include "InventoryExDialog.h"
#include "EnchantDialog.h"
#include "MixDialog.h"
#include "DissolveDialog.h"
#include "ReinforceDlg.h"
#include "ApplyOptionDialog.h"
#include "ProgressDialog.h"
#include "QuestTotalDialog.h"
#include "QuestDialog.h"
#include "QuestQuickViewDialog.h"
#include "PartyCreateDlg.h"
#include "PartyInviteDlg.h"
#include "ItemShopDialog.h"
#include "Input/UserInput.h"
#include "petstatusicondialog.h"
#include "GuildFieldWarDialog.h"
#include "ShoutDialog.h"
#include "FishingDialog.h"
#include "FishingGaugeDialog.h"
#include "FishingPointDialog.h"
#include "GTStandingDialog.h"
#include "GTStandingDialog16.h"
#include "GTBattleListDialog.h"
#include "GTScoreInfoDialog.h"
#include "GuageDialog.h"
#include "ReinforceGuideDialog.h"
#include "KeySettingTipDlg.h"
#include "GuildNoteDlg.h"
#include "UnionNoteDlg.h"
#include "GuildNoticeDlg.h"
#include "guildjoindialog.h"
#include "ChattingDlg.h"
#include "MainSystemDlg.h"
#include "MonsterGuageDlg.h"
#include "QuickSlotDlg.h"
#include "FamilyCreateDialog.h"
#include "FamilyDialog.h"
#include "FamilyMarkDialog.h"
#include "FamilyNickNameDialog.h"

#ifdef _HACK_SHIELD_
#include "HackShieldManager.h"
#endif

#ifdef _GMTOOL_
#include "interface/cResourceManager.h"
#endif

#include "cSkillTreeDlg.h"
#include "NpcImageDlg.h"
#include "QuickDlg.h"
#include "DateMatchingDlg.h"
#include "cSkillTrainingDlg.h"
#include "QuestManager.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "TutorialDlg.h"
#include "ItemPopupDlg.h"
#include "HelperDlg.h"
#include "TutorialBtnDlg.h"
#include "cMapMoveDialog.h"
#include "cChangeNameDialog.h"
#include "./Interface/cNumberPadDialog.h"
#include "AutoNoteDlg.h"
#include "AutoAnswerDlg.h"
#include "StoreSearchDlg.h"
#include "BodyChangeDialog.h"
#include "ChatRoomMainDlg.h"
#include "ChatRoomCreateDlg.h"
#include "ChatRoomDlg.h"
#include "ChatRoomGuestListDlg.h"
#include "ChatRoomOptionDlg.h"
#include "ChatRoomJoinDlg.h"
#include "PetInfoDialog.h"
#include "PetStateDialog.h"
#include "PetWearedDialog.h"
#include "PetResurrectionDialog.h"
#include "GTResultDlg.h"
#include "GTEntryEditDlg.h"
#include "CookDlg.h"
#include "SiegeWarFlagDlg.h"
#include "FadeDlg.h"
#include "popupmenudlg.h"
#include "RidePopupMenuDlg.h"
#include "GradeClassDlg.h"
#include "cHousingWebDlg.h"				//090622 NYJ 하우징 웹브라우저
#include "HousingRevolDlg.h"			//090323 pdy 하우징 설치 UI추가
#include "cHousingWarehouseDlg.h"		//090330 pdy 하우징 창고 UI추가
#include "cHouseSearchDlg.h"			//090409 pdy 하우징 하우스 검색 UI추가
#include "cHouseNameDlg.h"				//090410 pdy 하우징 하우스 이름설정 UI추가
#include "cHousingDecoPointDlg.h"		//090414 pdy 하우징 꾸미기포인트 보상 UI추가
#include "cHousingActionPopupMenuDlg.h" //090507 pdy 하우징 액션 팝업매뉴 UI추가
#include "cHousingMainPointDlg.h"		//090525 pdy 하우징 매인포인트 UI추가
#include "cHousingWarehouseButtonDlg.h"	//090525 pdy 하우징 내집창고 버튼 UI추가
#include "cHousingDecoModeBtDlg.h"		//090708 pdy 하우징 꾸미기모드 버튼 UI추가
#include "cHousingMgr.h"
#include "InputNameDlg.h"
#include "NpcNoticeDlg.h"
#include "Trigger/TriggerTimerDlg.h"
#include "VideoCaptureDlg.h"
#include "AdditionalButtonDlg.h"
#include "GameRatingDialog.h"
#include "CharSelect.h"
#include "ChannelDialog.h"
#include "ChangeClassDlg.h"
#include "ConsignmentGuideDlg.h"
#include "ConsignmentDlg.h"
#include "WorldMapDlg.h"
#ifdef _TW_LOCAL_
#include "ItemShopDlg.h"
#endif
extern HWND _g_hWnd;

GLOBALTON(cWindowManager);


cWindowManager::cWindowManager()
{
	m_OldDragFlag = FALSE;
	cbDragnDropProcess = NULL;
	m_pFocusedEdit = NULL;
	m_id = -1;
	m_pDragDialog = NULL;
	m_bHideInterface = FALSE;
	m_bOpendAllWindows = TRUE;
}

cWindowManager::~cWindowManager()
{
//	Release();	//PJH 여기서 실행하면 두번 Release하게 된다
}
void cWindowManager::Release()
{
	DestroyWindowAll();
	CFONT_OBJ->Release();

	RemoveFontResourceEx( "./Data/Interface/Windows/2002_EYA.ttf", FR_PRIVATE, 0 );
}

void cWindowManager::Init()
{
	AddFontResourceEx( "./Data/Interface/Windows/2002_EYA.ttf", FR_PRIVATE, 0 );

	LOGFONT	font;

	extern HWND _g_hWnd;

	int nNumerator = GetDeviceCaps(GetDC( _g_hWnd ), LOGPIXELSY) ;	// (LOGPIXELSY : 인치당 픽셀수, 즉 해상도를 조사한다.) DC가 참조하는 장치의 여러가지 속성이나 능력을 조사하여 조사된 값을 리턴한다.

//	font.lfHeight = -MulDiv(10, nNumerator, 72);						// 첫번째 인수와 두번째 인수를 곱하고, 그 결과를 세번째 인수로 나눈 결과를 리턴해준다.(오버플로나, 나누기 값이 0인경우는 -1을리턴한다.)
	font.lfHeight = -11;
	font.lfWidth = -font.lfHeight / 2;
	font.lfEscapement = 0;
	font.lfOrientation = 0;
	font.lfWeight = FW_NORMAL;
//	font.lfWeight = FW_DONTCARE;
	font.lfItalic = 0;
	font.lfUnderline = 0;
	font.lfStrikeOut = 0;
	font.lfCharSet = DEFAULT_CHARSET;
//	font.lfCharSet = GB2312_CHARSET;
	font.lfOutPrecision = 0;
//	font.lfClipPrecision = 0;
//	font.lfQuality = CLEARTYPE_QUALITY;
	font.lfQuality = ANTIALIASED_QUALITY;//NONANTIALIASED_QUALITY;//PROOF_QUALITY;
	font.lfPitchAndFamily = 0;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT0);

	font.lfHeight = -18;
	font.lfWidth = -font.lfHeight / 2;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT8);

	font.lfHeight = -MulDiv(9, nNumerator, 72);
	font.lfWidth = -font.lfHeight / 2;
	font.lfUnderline = 1;
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT4);
	
	font.lfUnderline = 0;
	font.lfHeight = -14;
	font.lfWidth = -font.lfHeight / 2;	
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT1);

	font.lfUnderline = 0;
	font.lfHeight = -14;
	font.lfWidth = -font.lfHeight / 2;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT2);
	
	font.lfHeight = -MulDiv(35, GetDeviceCaps(GetDC( _g_hWnd ), LOGPIXELSY), 72);
	font.lfWidth = -font.lfHeight / 2;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT3);

	// 070329 LYW --- cWindowManager : Add font type.
	font.lfHeight = -MulDiv(14, GetDeviceCaps(GetDC( _g_hWnd ), LOGPIXELSY), 72);
	font.lfWidth = -font.lfHeight / 2;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT5);

    // 퀘스트 공지 출력용 폰트 추가.
	nNumerator = GetDeviceCaps(GetDC( _g_hWnd ), LOGPIXELSY) ;	// (LOGPIXELSY : 인치당 픽셀수, 즉 해상도를 조사한다.) DC가 참조하는 장치의 여러가지 속성이나 능력을 조사하여 조사된 값을 리턴한다.

	font.lfHeight = -MulDiv(11, nNumerator, 72);						// 첫번째 인수와 두번째 인수를 곱하고, 그 결과를 세번째 인수로 나눈 결과를 리턴해준다.(오버플로나, 나누기 값이 0인경우는 -1을리턴한다.)
	font.lfWidth = -font.lfHeight / 2;
	font.lfEscapement = 0;
	font.lfOrientation = 0;
	font.lfWeight = FW_BOLD;
	font.lfItalic = 0;
	font.lfUnderline = 0;
	font.lfStrikeOut = 0;
	font.lfCharSet = GB2312_CHARSET;
	font.lfOutPrecision = 0;
	font.lfPitchAndFamily = 0;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT6);

	// 071017 LYW --- cWindowManager : Add font style to render tutorial text.
	nNumerator = GetDeviceCaps(GetDC( _g_hWnd ), LOGPIXELSY) ;	// (LOGPIXELSY : 인치당 픽셀수, 즉 해상도를 조사한다.) DC가 참조하는 장치의 여러가지 속성이나 능력을 조사하여 조사된 값을 리턴한다.

	font.lfHeight = -MulDiv(17, nNumerator, 72);						// 첫번째 인수와 두번째 인수를 곱하고, 그 결과를 세번째 인수로 나눈 결과를 리턴해준다.(오버플로나, 나누기 값이 0인경우는 -1을리턴한다.)
	font.lfWidth = -font.lfHeight / 2;
	font.lfEscapement = 0;
	font.lfOrientation = 0;
	font.lfWeight = FW_NORMAL;
	font.lfItalic = 0;
	font.lfUnderline = 0;
	font.lfStrikeOut = 0;
	font.lfCharSet = GB2312_CHARSET;
	font.lfOutPrecision = 0;
	font.lfPitchAndFamily = 0;
	lstrcpy(font.lfFaceName,"2002_EYA");
	CFONT_OBJ->CreateFontObject(&font, cFont::FONT7);

//KES MESSAGEBOX 030827
	cMsgBox::InitMsgBox();
}

void cWindowManager::CreateGameIn()
{
	// 091228 LUJ, GM툴일 경우에만 재설정을 허용한다
#ifdef _GMTOOL_
	RESRCMGR->LoadInterfaceMessage();
#endif
	TUTORIALMGR->Reset_MemberDlg() ;
	CreateMonsterGuageDlg();

	CreateCharDlg();
	CreateSkillTreeDlg();
	CreateInventoryDlg();
	CreateRecvExchangeDlg();
	
	CreateHeroGuage();
	CreateDealDlg();
	CreateStallKindSelectDlg();
	CreateStreetBuyStallDlg();	// 080219 KTH
	CreateBuyRegDlg();

	CreateStreetStallDlg();
	CreateNpcScriptDlg();
//	CreateHelpDlg();

	CreateQuickDlg();
	CreatePartyDlg();
	CreatePartyCreateDlg();
	CreatePartyInviteDlg();
	CreateGuildCreateDlg();
	CreateGuildNickNameDlg();
	CreateGuildRankDlg();
	CreateGuildDlg();
	CreateGuildInviteDlg();
	CreateGuildMarkDlg();
	CreateGuildLevelupDlg();
	CreateGuildWarehouseDlg();

	// 070831 웅주, 길드전 중 사망시 부활 안내
	AddWindow(GetDlgInfoFromFile( "Data/Interface/Windows/GuildRevival.bin", "rb" ));

	CreateMiniMapDlg();
	CreateStorageDlg();
	CreateFriendDlg();
	CreateMiniFriendDlg();
	CreateNoteDlg();
	CreateMiniNoteDlgWrite();
	CreateMiniNoteDlgRead();
	CreateReviveDlg();
	CreateQuestTotalDlg();
	CreateQuestquickDlg() ;
	CreateOptionDlg();
	CreateChattingDlg() ;
	// 061219 LYW --- Add main system dialog.
	CreateMainSystemDlg() ;
	CreateExitDlg();
	CreateMonstermeterDlg();
	CreateFarmBuyDlg();
	CreateFarmUpgradeDlg();
	CreateFarmManageDlg();
	CreateDateZoneListDlg();
	CreateChallengeZoneListDlg();
	CreateChallengeZoneClearNo1Dlg();
	CreateExchangeDlg();
	CreateMixDlg();
	CreateEnchantDlg();
	CreateDissolveDlg();
	CreateReinforceDlg();
	CreateApplyOptionDlg();
	CreateReinforceGuideDlg();
	CreateProgressDlog();
	// 080916 LUJ, 합성 창 생성
	CreateComposeDlg();
	CreateItemShopDlg();
	CreateGuildFieldWarDlg();
	CreateGTStandingDlg();
	CreateGTStandingDlg16();
//	CreateGTBattleListDlg();
	CreateGTScoreInfoDlg();
	CreateKeySettingTipDlg();

	CreateGuildNoteDlg();
	CreateUnionNoteDlg();

	CreateGuildNoticeDlg();
	CreateGuildWarehouseRankDlg();

	//SW060713 문하생
	CreateGuildInvitationKindSelectionDlg();
	CreateDateMatchingDlg();
	// 070605 LYW --- cWindowManager : Add function to create identification.
	CreateIdentificationDlg() ;

	// 070605 LYW --- cWindowManager : Add function to create favor icon dialog.
	CreateFavorIconDlg() ;

	CreateNpcImageDlg();
	CreateSkillTrainingDlg();

	CreateFamilyCreateDlg();
	CreateFamilyNickNameDlg();
//	CreateFamilyRankDlg();
	CreateFamilyDlg();
	CreateFamilyMarkDlg();
	CreateLimitDungeonDlg();
	CreateTutorialDlg() ;
	CreateHelpDlg() ;
	CreateTutorialBtnDlg() ;
	CreateItemPopupDlg() ;
	CreateMapMoveDlg() ;
	CreateChangeNameDlg() ;
	CreateAutoNoteDlg();
	CreateAutoAnswerDlg();
	CreateStoreSearchDlg();
	CreateShoutDlg();
	CreateBodyChangeDlg();
	CreateFishingDlg();
	CreateFishingGaugeDlg();
	CreateFishingPointDlg();
	CreateChatRoomMainDlg() ;
	CreateChatRoomCreateDlg() ;
	CreateChatRoomDlg() ;
	CreateChatRoomGuestListDlg() ;
	CreateChatRoomOptionDlg() ;
	CreateChatRoomJoinDlg() ;
	CreateFarmAnimalDlg();
	CreatePetDialog();
	CreateGTResultDlg();
	CreateGTEntryEditDlg();
	CreateCookDlg();
	CreateSiegeWarFlagDlg() ;
	CreateFadeDlg() ;
	CreatePopupMenuDlg();
	// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
	CreateRidePopupMenuDlg();
	// 090413 ONS 전직 가이드 다이얼로그 추가.
	CreateGradeClassDlg();

	// 090902 pdy 하우징맵에서만 쓰는 UI는 하우징 맵에서만 로딩한다.
	if( HOUSINGMGR->IsHousingMap() )
	{
		//090323 pdy 하우징 설치 UI추가
		CreateHousingRevolDlg();

		//090330 pdy 하우징 창고 UI추가
		CreateHousingWereHouseDlg();

		//090414 pdy 하우징 꾸미기포인트 보상 UI추가
		CreateHousingDecoPointDlg();

		//090507 pdy 하우징 액션 팝업매뉴 UI추가
		CreateHousingActionPopupMenuDlg();

		//090525 pdy 하우징 매인포인트 UI추가
		CreateHousingMainPointDlg();

		//090525 pdy 하우징 내집창고 버튼 UI추가
		CreateHousingWarehouseButtonDlg();

		//090708 pdy 하우징 꾸미기모드 버튼 UI추가
		CreateHousingDecoModeBtDlg();
	}

	//090409 pdy 하우징 하우스 검색 UI추가
	CreateHouseSearchDlg();

	//090410 pdy 하우징 하우스 이름설정 UI추가
	CreateHouseNameDlg();

	//090622 NYJ 하우징 웹브라우저
	CreateHousingWebDlg();

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	CreateInputNameDlg();

	// 090925 ShinJS --- Npc Notice Dialog 추가
	CreateNpcNoticeDlg();
	// 091102 NYJ - 트리거 타이머
	CreateTriggerTimerDlg();

	// 091210 ShinJS --- Video Capture Dialog 추가
	CreateVideoCaptureDlg();

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	CreateAdditionalButtonDlg();

	// 100511 ONS  전직변경 다이얼로그 추가
	CreateChangeClassDlg();

#ifdef _TW_LOCAL_
	CreateItemShopBrowserDlg();
#endif

	AddWindow(
		GetDlgInfoFromFile("data\\interface\\windows\\CristalEmpireBonusDialog.bin", "rb"));
	AddWindow(
		GetDlgInfoFromFile("data\\interface\\windows\\RideOptionDlg.bin", "rb"));

#ifdef _KOR_LOCAL_
	GameRatingDlg();
#endif
	// 100507 ShinJS --- 채널변경 UI 추가
	CreateChannelDlg();
	CHARSELECT->GetChannelDialog()->SetState( CChannelDialog::eState_GameIn );

	CreateConsignmentGuideDlg();
	CreateConsignmentDlg();
}

void cWindowManager::CreateGuildFieldWarDlg()
{
	cWindow* window = NULL;
	window = GetDlgInfoFromFile( "Data/Interface/Windows/GFWarDeclare.bin", "rb" );
	if( window)
	{
		AddWindow( window );
		GAMEIN->SetGFWarDeclareDlg( (CGFWarDeclareDlg*)window );
		GAMEIN->GetGFWarDeclareDlg()->Linking();
	}

	window = GetDlgInfoFromFile( "Data/Interface/Windows/GFWarResult.bin", "rb" );
	if( window)
	{
		AddWindow( window );
		GAMEIN->SetGFWarResultDlg( (CGFWarResultDlg*)window );
		GAMEIN->GetGFWarResultDlg()->Linking();
	}

	window = GetDlgInfoFromFile( "Data/Interface/Windows/GFWarInfo.bin", "rb" );
	if( window)
	{
		AddWindow( window );
		GAMEIN->SetGFWarInfoDlg( (CGFWarInfoDlg*)window );
		GAMEIN->GetGFWarInfoDlg()->Linking();
	}
	
	window = GetDlgInfoFromFile( "Data/Interface/Windows/GuildWarInfo.bin", "rb" );
	if( window)
	{
		AddWindow( window );
		GAMEIN->SetGuildWarInfoDlg( (CGuildWarInfoDlg*)window );
		GAMEIN->GetGuildWarInfoDlg()->Linking();
	}
}


void cWindowManager::CreateDissolveDlg()
{
	cWindow * window = GetDlgInfoFromFile( "Data/Interface/Windows/DissolveDialog.bin", "rb" );
	AddWindow(window);

	CDissolveDialog* pDlg = (CDissolveDialog*)window;
	GAMEIN->SetDissolveDialog( pDlg );
}


void cWindowManager::CreateEnchantDlg()
{
	CEnchantDialog* window = ( CEnchantDialog* )GetDlgInfoFromFile("Data/Interface/Windows/EnchantDialog.bin", "rb");

	AddWindow(window);
	window->Linking();
}


void cWindowManager::CreateProgressDlog()
{
	CProgressDialog* window = ( CProgressDialog* )GetDlgInfoFromFile("Data/Interface/Windows/ProgressDialog.bin", "rb");

	AddWindow(window);
	window->Linking();
}


void cWindowManager::CreateReinforceDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/ReinforceDialog.bin", "rb");
	AddWindow(window);
	GAMEIN->SetReinforceDialog((CReinforceDlg * )window);
	GAMEIN->GetReinforceDialog()->Linking();
}


void cWindowManager::CreateApplyOptionDlg()
{
	CApplyOptionDialog* window = ( CApplyOptionDialog* )GetDlgInfoFromFile( "Data/Interface/Windows/ApplyOptionDialog.bin", "rb" );

	if( window )
	{
		AddWindow( window );
		window->Linking();
	}
}


void cWindowManager::CreateMixDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/MixDialog.bin", "rb");
	AddWindow(window);
	GAMEIN->SetMixDialog((CMixDialog * )window);
	GAMEIN->GetMixDialog()->Linking();
}

// 080916 LUJ, 합성 창 생성
void cWindowManager::CreateComposeDlg()
{
	CComposeDialog* dialog = ( CComposeDialog* )GetDlgInfoFromFile( "Data/Interface/Windows/ComposeDialog.bin", "rb" );
	
	if( ! dialog )
	{
		cprintf( "ComposeDialog.bin loading is failed\n" );
		return;
	}

	dialog->Linking();
	AddWindow( dialog );
}

void cWindowManager::CreateHeroGuage()
{
	// 070116 LYW --- Modified bin file name of CGuageDialog.
	//cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/char_main.bin", "rb");
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/CharGage.bin", "rb");
	AddWindow(window);
	GAMEIN->SetGuageDlg((CGuageDialog * )window);
	GAMEIN->GetGuageDlg()->Linking();
}

void cWindowManager::CreateQuickDlg()
{
	// 070117 LYW --- Modified bin file name.
	//cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/q_slot.bin", "rb");
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/QuickSlot.bin", "rb");
	ASSERT(window);
	AddWindow(window);

	cQuickDlg* pDlg = ( cQuickDlg* )window;
	pDlg->Linking();
	GAMEIN->SetQuickDlg( pDlg );
}

void cWindowManager::CreatePartyDlg() 
{
	cWindow * member1window = GetDlgInfoFromFile("Data/Interface/Windows/partymember1.bin", "rb");
	cWindow * member2window = GetDlgInfoFromFile("Data/Interface/Windows/partymember2.bin", "rb");
	cWindow * member3window = GetDlgInfoFromFile("Data/Interface/Windows/partymember3.bin", "rb");
	cWindow * member4window = GetDlgInfoFromFile("Data/Interface/Windows/partymember4.bin", "rb");
	cWindow * member5window = GetDlgInfoFromFile("Data/Interface/Windows/partymember5.bin", "rb");
	cWindow * member6window = GetDlgInfoFromFile("Data/Interface/Windows/partymember6.bin", "rb");
	cWindow * partybtnwindow = GetDlgInfoFromFile("Data/Interface/Windows/PartySet.bin", "rb");
	
	ASSERT(member1window);	ASSERT(member2window);	ASSERT(member3window);
	ASSERT(member4window);	ASSERT(member5window);	ASSERT(member6window);
	ASSERT(partybtnwindow);
	
	AddWindow(member1window);	AddWindow(member2window);	AddWindow(member3window);
	AddWindow(member4window);	AddWindow(member5window);	AddWindow(member6window);
	AddWindow(partybtnwindow);
	
	GAMEIN->GetPartyDialog()->RegistMemberDlg(0, (CPartyMemberDlg*)member1window);	
	GAMEIN->GetPartyDialog()->RegistMemberDlg(1, (CPartyMemberDlg*)member2window);
	GAMEIN->GetPartyDialog()->RegistMemberDlg(2, (CPartyMemberDlg*)member3window);
	GAMEIN->GetPartyDialog()->RegistMemberDlg(3, (CPartyMemberDlg*)member4window);
	GAMEIN->GetPartyDialog()->RegistMemberDlg(4, (CPartyMemberDlg*)member5window);
	GAMEIN->GetPartyDialog()->RegistMemberDlg(5, (CPartyMemberDlg*)member6window);

	GAMEIN->GetPartyDialog()->RegistBtnDlg((CPartyBtnDlg*)partybtnwindow);

} 

void cWindowManager::CreatePartyCreateDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/PartyCreate.bin", "rb");

	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetPartyCreateDialog((CPartyCreateDlg * )window);
	GAMEIN->GetPartyCreateDialog()->Linking();

}

void cWindowManager::CreatePartyInviteDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/PartyInvite.bin", "rb");

	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetPartyInviteDialog((CPartyInviteDlg * )window);
	GAMEIN->GetPartyInviteDialog()->Linking();

}

void cWindowManager::CreateGuildCreateDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildCreate.bin", "rb");
	if( window)		// 임시로..
	{
		ASSERT(window);
		AddWindow(window);
		
		GAMEIN->SetGuildCreateDlg((CGuildCreateDialog * )window);
		GAMEIN->GetGuildCreateDlg()->Linking();
		
		window = GetDlgInfoFromFile("Data/Interface/Windows/GuildUnionCreate.bin", "rb");
		
		ASSERT(window);
		AddWindow(window);
		
		GAMEIN->SetGuildUnionCreateDlg((CGuildUnionCreateDialog * )window);
		GAMEIN->GetGuildUnionCreateDlg()->Linking();
	}
}

void cWindowManager::CreateGuildDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Guild.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetGuildDlg((CGuildDialog * )window);
	GAMEIN->GetGuildDlg()->Linking();
}

void cWindowManager::CreateGuildInviteDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildInvite.bin", "rb");
	if( window)
	{
		ASSERT(window);
		AddWindow(window);
		
		GAMEIN->SetGuildInviteDlg((CGuildInviteDialog * )window);
		GAMEIN->GetGuildInviteDlg()->Linking();
	}
}

void cWindowManager::CreateGuildMarkDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildMark.bin", "rb");
	if( window)
	{
		ASSERT(window);
		AddWindow(window);
		
		GAMEIN->SetGuildMarkDlg((CGuildMarkDialog * )window);
		GAMEIN->GetGuildMarkDlg()->Linking();
	}
}

void cWindowManager::CreateGuildLevelupDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildLevelUp.bin", "rb");
	if( window)
	{
		ASSERT(window);
		AddWindow(window);
		
		GAMEIN->SetGuildLevelUpDlg((CGuildLevelUpDialog * )window);
		GAMEIN->GetGuildLevelUpDlg()->Linking();
	}
}

void cWindowManager::CreateGuildNickNameDlg()
{
	CGuildNickNameDialog* dialog = ( CGuildNickNameDialog* )GetDlgInfoFromFile("Data/Interface/Windows/GuildNickName.bin", "rb");
	ASSERT( dialog );

	AddWindow( dialog );

	GAMEIN->SetGuildNickNameDlg( dialog );
	dialog->Linking();
}


void cWindowManager::CreateGuildRankDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildRank.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetGuildRankDlg((CGuildRankDialog * )window);
	GAMEIN->GetGuildRankDlg()->Linking();
}

void cWindowManager::CreateGuildWarehouseDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GuildWarehouse.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
		
	GAMEIN->SetGuildWarehouseDlg((CGuildWarehouseDialog * )window);
	GAMEIN->GetGuildWarehouseDlg()->Linking();
}

void cWindowManager::CreateStorageDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Storage.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetStorageDialog((CStorageDialog * )window);
	GAMEIN->GetStorageDialog()->Linking();
}

void cWindowManager::CreateMiniMapDlg()
{
	// 070116 LYW --- Modified this line.
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/MiniMap.bin", "rb");
	AddWindow(window);

	GAMEIN->SetMiniMapDialog((CMiniMapDlg * )window);
	GAMEIN->GetMiniMapDialog()->Linking();
	GAMEIN->GetMiniMapDialog()->InitMiniMap(MAP->GetMapNum());
	
	cWindow * window2 = GetDlgInfoFromFile("Data/Interface/Windows/BigMap.bin", "rb");
	AddWindow(window2);

	GAMEIN->SetBigMapDialog((CBigMapDlg * )window2);
	if( GAMEIN->GetBigMapDialog() )
	{
		GAMEIN->GetBigMapDialog()->Linking();
		GAMEIN->GetBigMapDialog()->InitBigMap(MAP->GetMapNum());
	}

	CWorldMapDlg* pWolrdmap = (CWorldMapDlg*)GetDlgInfoFromFile("Data/Interface/Windows/WorldMapDlg.bin", "rb");
	if( pWolrdmap )
	{
		pWolrdmap->Linking();
		GAMEIN->SetWorldMapDlg( pWolrdmap );
		AddWindow( pWolrdmap ) ;
	}
}

// 061127 LYW --- Add Create Chatting Dialog Function For New Chatting System.
void cWindowManager::CreateChattingDlg()
{
	cWindow * window = GetDlgInfoFromFile( "Data/Interface/Windows/ChatWindow.bin", "rb" ) ;
	AddWindow( window ) ;
	GAMEIN->SetChattingDlg( ( CChattingDlg* )window ) ;
	GAMEIN->GetChattingDlg()->Linking() ;
}

// 061219 LYW --- Add main system dialog.
void cWindowManager::CreateMainSystemDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/Menu.bin", "rb" ) ;
	AddWindow( window ) ;
	GAMEIN->SetMainSystemDlg( ( CMainSystemDlg* )window ) ;

	GAMEIN->GetMainSystemDlg()->Linking() ;
}

void cWindowManager::CreateOptionDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Option.bin", "rb");
	AddWindow(window);
	GAMEIN->SetOptionDialog((COptionDialog * )window);
	GAMEIN->GetOptionDialog()->Linking();
}

void cWindowManager::CreateExitDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/MainMenu.bin", "rb");
	AddWindow(window);
	GAMEIN->SetExitDialog((CExitDialog * )window);
}

// desc_hseos_몬스터미터_01
// S 몬스터미터 추가 added by hseos 2007.04.09
void cWindowManager::CreateMonstermeterDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/MonsterKill.bin", "rb");
	AddWindow(window);
	GAMEIN->SetMonstermeterDlg((CSHMonstermeterDlg * )window);
	GAMEIN->GetMonstermeterDlg()->Linking();
}
// E 몬스터미터 추가 added by hseos 2007.04.09

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23
void cWindowManager::CreateFarmBuyDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Farm_Get.bin", "rb");
	AddWindow(window);
	GAMEIN->SetFarmBuyDlg((CSHFarmBuyDlg * )window);
	GAMEIN->GetFarmBuyDlg()->Linking();
}

void cWindowManager::CreateFarmUpgradeDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Farm_Upgrade.bin", "rb");
	AddWindow(window);
	GAMEIN->SetFarmUpgradeDlg((CSHFarmUpgradeDlg * )window);
	GAMEIN->GetFarmUpgradeDlg()->Linking();
}

void cWindowManager::CreateFarmManageDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/FarmManage.bin", "rb");
	AddWindow(window);
	GAMEIN->SetFarmManageDlg((CSHFarmManageDlg * )window);
	GAMEIN->GetFarmManageDlg()->Linking();
}
// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
void cWindowManager::CreateDateZoneListDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/DateZoneListDlg.bin", "rb");
	AddWindow(window);
	GAMEIN->SetDateZoneListDlg((CSHDateZoneListDlg * )window);
	GAMEIN->GetDateZoneListDlg()->Linking();
}

void cWindowManager::CreateChallengeZoneListDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/ChallengeZoneListDlg.bin", "rb");
	AddWindow(window);
	GAMEIN->SetChallengeZoneListDlg((CSHChallengeZoneListDlg * )window);
	GAMEIN->GetChallengeZoneListDlg()->Linking();
}

void cWindowManager::CreateChallengeZoneClearNo1Dlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/ChallengeZoneClearNo1Dlg.bin", "rb");
	AddWindow(window);
	GAMEIN->SetChallengeZoneClearNo1Dlg((CSHChallengeZoneClearNo1Dlg * )window);
	GAMEIN->GetChallengeZoneClearNo1Dlg()->Linking();
}

void cWindowManager::CreateNoteDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Note.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetNoteDialog((CNoteDialog * )window);
	GAMEIN->GetNoteDialog()->Linking();
}

void cWindowManager::CreateFriendDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Friend.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFriendDialog((CFriendDialog * )window);
	GAMEIN->GetFriendDialog()->Linking();
}


void cWindowManager::CreateMiniFriendDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/FriendInvite.bin","rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetMiniFriendDialog((CMiniFriendDialog * )window);
	GAMEIN->GetMiniFriendDialog()->Linking();
}

void cWindowManager::CreateMiniNoteDlgWrite()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/NoteWrite.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetMiniNoteDialogWrite((CMiniNoteDialogWrite * )window);
	GAMEIN->GetMiniNoteDialogWrite()->Linking();
}

void cWindowManager::CreateMiniNoteDlgRead()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/NoteRead.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);

	GAMEIN->SetMiniNoteDialogRead((CMiniNoteDialogRead * )window);
	GAMEIN->GetMiniNoteDialogRead()->Linking();
}

void cWindowManager::CreateReviveDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Revival.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetReviveDialog((CReviveDialog * )window);
	GAMEIN->GetReviveDialog()->Linking();
}

void cWindowManager::CreateCharDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/charInfo.bin", "rb");

	AddWindow(window);
	GAMEIN->SetCharacterDialog((CCharacterDialog * )window);
	GAMEIN->GetCharacterDialog()->Linking();
}
void cWindowManager::CreateSkillTreeDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Skill.bin", "rb");
	AddWindow(window);
	
	cSkillTreeDlg* pSkillTreeDlg = ( cSkillTreeDlg* )window;
	GAMEIN->SetSkillTreeDlg( pSkillTreeDlg );
	pSkillTreeDlg->Linking();
}

void cWindowManager::CreateInventoryDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Inven.bin", "rb");
	AddWindow(window);

	GAMEIN->SetInventoryDialog((CInventoryExDialog * )window);
	GAMEIN->GetInventoryDialog()->Linking();
}


//KES DIALOG 030918 //EXCHANGEDLG 
void cWindowManager::CreateExchangeDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/Trade.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetExchangeDialog( (CExchangeDialog*)window );
	GAMEIN->GetExchangeDialog()->Linking();
}


void cWindowManager::CreateRecvExchangeDlg()
{
	
}

// LYJ 051017 구입노점상 추가
void cWindowManager::CreateStallKindSelectDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/StallOption.bin", "rb");
	AddWindow(window);
	GAMEIN->SetStallKindSelectDialog((CStallKindSelectDlg * )window);
	GAMEIN->GetStallKindSelectDialog()->Linking();
}

void cWindowManager::CreateStreetBuyStallDlg()
{
	//cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/30in", "rb");
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/StallBuy.bin", "rb");
	AddWindow(window);
	GAMEIN->SetStreetBuyStallDialog((CStreetBuyStall * )window);
	GAMEIN->GetStreetBuyStallDialog()->Linking();
}
// 080219 KTH -- 
void cWindowManager::CreateBuyRegDlg()
{
	//cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/32.bin", "rb");
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/StallBuyList.bin", "rb");
	AddWindow(window);
	GAMEIN->SetBuyRegDialog((CBuyRegDialog * )window);
	GAMEIN->GetBuyRegDialog()->Linking();
}

//LBS 03.09.18
void cWindowManager::CreateStreetStallDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/StallSell.bin", "rb");
	AddWindow(window);
	GAMEIN->SetStreetStallDialog((CStreetStall * )window);
	GAMEIN->GetStreetStallDialog()->Linking();
}

void cWindowManager::CreateNpcScriptDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/NPCChat.bin", "rb");
	AddWindow(window);
	GAMEIN->SetNpcScriptDialog((cNpcScriptDialog * )window);
	GAMEIN->GetNpcScriptDialog()->Linking();
}

void cWindowManager::CreateDealDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/NPCShop.bin", "rb");
	AddWindow(window);
	GAMEIN->SetDealDialog((CDealDialog * )window);
	GAMEIN->GetDealDialog()->Linking();

	CMHFile fp;
	fp.Init("./System/Resource/DealItem.bin","rb");
	GAMEIN->GetDealDialog()->LoadDealerItem(&fp);
	fp.Release();

}

void cWindowManager::CreateQuestTotalDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Quest.bin", "rb");
	AddWindow(window);

	CQuestTotalDialog * pDlg = (CQuestTotalDialog * )window;
	GAMEIN->SetQuestTotalDialog(pDlg);

	CQuestDialog* pQuestDlg = (CQuestDialog*)pDlg->GetTabSheet(0);
	GAMEIN->SetQuestDialog(pQuestDlg);
	pQuestDlg->Linking();


	QUESTMGR->SetQuestDialog(pQuestDlg) ;
}

void cWindowManager::CreateQuestquickDlg()
{
	cWindow* window = GetDlgInfoFromFile("./Data/Interface/Windows/QuestQuickView.bin", "rb") ;

	ASSERT(window) ;

	AddWindow(window) ;

	CQuestQuickViewDialog* pDlg = (CQuestQuickViewDialog*)window ;
	GAMEIN->SetQuestQuickViewDialog(pDlg) ;
	CQuestQuickViewDialog* pQuickDlg = GAMEIN->GetQuestQuickViewDialog() ;

	pQuickDlg->Linking() ;

	QUESTMGR->SetQuickViewDialog(pQuickDlg) ;
}

void cWindowManager::CreateItemShopDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/ItemMallWarehouse.bin", "rb");
	AddWindow(window);

	CItemShopDialog* pDlg = (CItemShopDialog*)window;
	GAMEIN->SetItemShopDialog(pDlg);
	if( pDlg )
		pDlg->Linking();
}

void cWindowManager::CreateGTStandingDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GTBattleTable_8.bin", "rb");
	AddWindow(window);

	CGTStandingDialog* pDlg = (CGTStandingDialog*)window;
	GAMEIN->SetGTStandingDlg(pDlg);
	pDlg->Linking();
}

void cWindowManager::CreateGTStandingDlg16()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GTBattleTable_16.bin", "rb");
	AddWindow(window);

	CGTStandingDialog16* pDlg = (CGTStandingDialog16*)window;
	GAMEIN->SetGTStandingDlg16(pDlg);
	pDlg->Linking();
}


void cWindowManager::CreateGTBattleListDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GDViewTournament.bin", "rb");
	AddWindow(window);

	CGTBattleListDialog* pDlg = (CGTBattleListDialog*)window;
	GAMEIN->SetGTBattleListDlg(pDlg);
	pDlg->Linking();
}


void cWindowManager::CreateGTScoreInfoDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GTScoreTime.bin", "rb");
	AddWindow(window);

	CGTScoreInfoDialog* pDlg = (CGTScoreInfoDialog*)window;
	GAMEIN->SetGTScoreInfoDlg(pDlg);
	pDlg->Linking();
}

void cWindowManager::CreateReinforceGuideDlg()
{
	/*cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/ReinforceGuideDialog.bin", "rb");
	AddWindow(window);

	CReinforceGuideDialog* pDlg = (CReinforceGuideDialog*)window;
	pDlg->Linking();
	GAMEIN->SetReinforceGuideDlg( pDlg );*/

	CReinforceGuideDialog* dialog = ( CReinforceGuideDialog* )( GetDlgInfoFromFile( "Data/Interface/Windows/ReinforceGuideDialog.bin", "rb" ) );
	AddWindow( dialog );

	dialog->Linking();
	GAMEIN->SetReinforceGuideDlg( dialog );
}

void cWindowManager::CreateKeySettingTipDlg()
{
	cWindow* window = NULL;

	window = GetDlgInfoFromFile("Data/Interface/Windows/KeySetting.bin", "rb");
	AddWindow(window);

	CKeySettingTipDlg* pDlg = (CKeySettingTipDlg*)window;
	pDlg->Linking();
	GAMEIN->SetKeySettingTipDlg( pDlg );
}

void cWindowManager::CreateGuildNoteDlg()
{
	cWindow* window = NULL;

	window = GetDlgInfoFromFile("Data/Interface/Windows/GuildNote.bin", "rb");
	if( window)
	{
		AddWindow(window);
		CGuildNoteDlg* pDlg = (CGuildNoteDlg*)window;
		pDlg->Linking();
		GAMEIN->SetGuildNoteDlg( pDlg );
	}
}

void cWindowManager::CreateUnionNoteDlg()
{
	cWindow* window = NULL;

	window = GetDlgInfoFromFile("Data/Interface/Windows/AllyNote.bin", "rb");
	if( window)
	{
		AddWindow(window);

		CUnionNoteDlg* pDlg = (CUnionNoteDlg*)window;
		pDlg->Linking();
		GAMEIN->SetUnionNoteDlg( pDlg );
	}
}

// 06. 03. 문파공지 - 이영준
void cWindowManager::CreateGuildNoticeDlg()
{
	CGuildNoticeDlg* dialog = ( CGuildNoticeDlg* )GetDlgInfoFromFile("Data/Interface/Windows/GuildNotice.bin", "rb");
	ASSERT( dialog );

	AddWindow( dialog );

	dialog->Linking();
	GAMEIN->SetGuildNoticeDlg( dialog );
}


void cWindowManager::CreateGuildWarehouseRankDlg()
{
	CGuildWarehouseRankDialog* dialog = ( CGuildWarehouseRankDialog* )GetDlgInfoFromFile( "Data/Interface/Windows/GuildWHRank.bin", "rb" );
	ASSERT( dialog );

	AddWindow( dialog );

	dialog->Linking();
}

void cWindowManager::CreateNpcImageDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/NpcImage.bin", "rb");
	AddWindow(window);

	cNpcImageDlg* pDlg = (cNpcImageDlg*)window;
	GAMEIN->SetNpcImageDlg(pDlg);

	// 070620 LYW --- cWindowManager : Linking controls.
	// 해상도에 따른 npc 이미지의 처리를 위해 상하 이미지를 링킹하기로 한다.
	pDlg->Linking() ;
}

void cWindowManager::AfterInit()
{
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cDialog * win = (cDialog *)m_pWindowList.GetNext(pos);

		if(win->GetType() & WT_ICONDIALOG || win->GetType() & WT_ICONGRIDDIALOG)
			m_pIconDlgListRef.AddTail(win);
		else if(win->GetType() & WT_TABDIALOG)
		{
			cTabDialog * tabDlg = (cTabDialog *)win;
			
			for( int i = 0; i < tabDlg->GetTabNum(); ++i )
			{
				if(tabDlg->GetTabSheet(i)->GetType() & WT_ICONDIALOG || tabDlg->GetTabSheet(i)->GetType() & WT_ICONGRIDDIALOG)
					m_pIconDlgListRef.AddTail(tabDlg->GetTabSheet(i));
			}
		}
		PTRLISTPOS pos2 = win->m_pComponentList.GetHeadPosition();
		while(pos2)
		{
			cWindow * win2 = (cWindow *)win->m_pComponentList.GetNext(pos2);
			if(win2->GetType() == WT_EDITBOX)
			{
				m_pEditBoxListRef.AddTail((void *)win2);
			}
			else if(win2->GetType() & WT_ICONDIALOG || win2->GetType() & WT_ICONGRIDDIALOG)
			{
				m_pIconDlgListRef.AddTail(win2);
			}
		}
	}
}

////////////////////////////////////////////////////
//CUserInput에서 Key Input이 들어왔을 시에 호출해 줌
void cWindowManager::KeyboardInput( CKeyboard* keyInfo )
{
	PTRLISTPOS pos = m_pWindowList.GetTailPosition();

	while(pos)
	{
		cWindow* window = (cWindow *)m_pWindowList.GetAt(pos);

		if(window->IsActive() && !window->IsDepend() && ! window->IsHide() ) //090119 pdy hide window
		{
			DWORD Action = window->ActionKeyboardEvent( keyInfo );

			if( Action & WE_CLOSEWINDOW )
				window->SetActive(FALSE);

			if( Action != WE_NULL ) break;	//키인풋이 처리되면 중지
		}

		m_pWindowList.GetPrev(pos);
	}
}

void cWindowManager::SetNextEditFocus()
{
	if( m_pFocusedEdit )
	{
		PTRLISTPOS posCur = m_pEditBoxListRef.Find( m_pFocusedEdit );
		PTRLISTPOS pos = posCur;

		m_pEditBoxListRef.GetNext(pos);
		
		while( pos != posCur )
		{
			if( pos == NULL )
				pos = m_pEditBoxListRef.GetHeadPosition();

			cEditBox* win = (cEditBox *)m_pEditBoxListRef.GetNext(pos);
			
			if( win->IsActive() && win->GetParent() == m_pFocusedEdit->GetParent() )
			{
				m_pFocusedEdit->SetFocusEdit( FALSE ) ;
				win->SetFocusEdit( TRUE );
				break;
			}
		}
	}
}

void cWindowManager::BackDragWindow()
{
	cDialog * srcDlg = GetWindowForID(m_id);
	if(!srcDlg) return;
	srcDlg->SetAbsXY((LONG)m_posBeforeDrag.x, (LONG)m_posBeforeDrag.y);	// SetDragEnd()가 있는데 필요한가???체크필

//	if( !m_pDragDialog ) return;
//	m_pDragDialog->SetAbsXY((LONG)m_posBeforeDrag.x, (LONG)m_posBeforeDrag.y);	// SetDragEnd()가 있는데 필요한가???체크필
	DragWindowNull();
}

void cWindowManager::Preprocess()
{
	if( m_OldDragFlag )
	{
		BOOL LeftUp = MOUSE->LButtonUp();
		
		if( LeftUp )
		{
			LONG ClickUpX = MOUSE->GetMouseEventX();
			LONG ClickUpY = MOUSE->GetMouseEventY();

			if( cbDragnDropProcess )
				cbDragnDropProcess( ClickUpX, ClickUpY, m_id, (LONG)m_posBeforeDrag.x, (LONG)m_posBeforeDrag.y);

			SetDragEnd();
		}
		else if( m_pDragDialog )
		{
			LONG x = MOUSE->GetMouseX();
			LONG y = MOUSE->GetMouseY();

//			cDialog * dlg = GetWindowForID( m_id );
//			if( dlg == NULL )
//				return;

			m_pDragDialog->SetAbsXY( x-m_OldX, y-m_OldY );
			
			const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
			RECT rect = { 0, 0, dispInfo.dwWidth, dispInfo.dwHeight };

			VECTOR2 magPos;
			magPos.x = m_pDragDialog->GetAbsX();
			magPos.y = m_pDragDialog->GetAbsY();
			if( cAnimationManager::MagProcess( &rect, (cWindow*)m_pDragDialog, &magPos ) )
				m_pDragDialog->SetAbsXY( magPos.x, magPos.y );
		}
	}
}

void cWindowManager::AddListDestroyWindow(cWindow * window)
{
	if( window == NULL ) return;

	PTRLISTSEARCHSTART((m_pDestroyWindowRef),cWindow*,pWindow)
		if(pWindow == window)
			return;
	PTRLISTSEARCHEND

	m_pDestroyWindowRef.AddTail(window);
	window->SetDestroy();
}

void cWindowManager::DestroyWindowProcess()
{
	PTRLISTPOS pos = m_pDestroyWindowRef.GetHeadPosition();

	while(pos)
	{
		cWindow * window = (cWindow *)m_pDestroyWindowRef.GetNext(pos);
		DeleteWindow( window );
	}

	m_pDestroyWindowRef.RemoveAll();
}

void cWindowManager::Process()
{
	ANIMGR->FadeProcess();

	PTRLISTPOS pos = m_pPlayerChatTooltip.GetHeadPosition();

	while(pos)
	{
		cWindow * window = (cWindow *)m_pPlayerChatTooltip.GetAt(pos);
		window->ActionEvent( MOUSE );
		
		if( !window->IsActive() )
			m_pPlayerChatTooltip.RemoveAt(pos);

		m_pPlayerChatTooltip.GetNext(pos);
	}
}

void cWindowManager::MouseInput( CMouse* pMouse )
{
//KES INTERFACE 031008
	m_bMouseOverUsed	= FALSE;
	m_bMouseDownUsed	= FALSE;

	m_MouseInputProcessed = FALSE;
	DestroyWindowProcess();
	Preprocess();
	
	cWindow * TopWindow = NULL;
	PTRLISTPOS TopPos = NULL;
	PTRLISTPOS pos = m_pWindowList.GetTailPosition();
	
	while(pos)
	{
		cWindow* window = (cWindow *)m_pWindowList.GetAt(pos);

		LONG Action=0;
		if( window->IsActive() && ! window->IsHide() )	//090119 pdy hide window ...->IsHide() 추가
		{
			if( !window->IsDepend() ) //KES INTERFACE 031010 //depend인것은 iconGridDialog가 한다.
				Action = window->ActionEvent( pMouse );
			//if(cbProcess)
				//cbProcess();
			
		}
		if((Action & WE_CLOSEWINDOW))
			window->SetActive(FALSE);
		else if(Action & WE_TOPWINDOW)
		{
			TopPos = pos;
			TopWindow = window;
		}
		if( Action & WE_LBTNCLICK || Action & WE_RBTNCLICK || 
			Action & WE_LBTNDBLCLICK || Action & WE_RBTNDBLCLICK ||
			Action & WE_COMBOBOXSELECT)
		{
			m_MouseInputProcessed	= TRUE;
			m_bMouseDownUsed		= TRUE;
		}
		if( Action & WE_MOUSEOVER )
		{
			m_MouseInputProcessed	= TRUE;
			m_bMouseOverUsed		= TRUE;
		}

		m_pWindowList.GetPrev(pos);
	}

	if(TopWindow && TopPos)
	{
		if( TopWindow->GetType() != WT_CHATTING )
		{
			m_pWindowList.RemoveAt(TopPos);			//중간에서 빼고

			if( TopWindow->IsAlwaysTop() )
				m_pWindowList.AddTail(TopWindow);		//끝에 넣고
			else
			{
				pos = m_pWindowList.GetTailPosition();		//메세지박스를 젤 위에 두기 위해

				while( pos )
				{
					cWindow* pWindow = (cWindow*)m_pWindowList.GetAt(pos);
					if( pWindow )
						if( !pWindow->IsAlwaysTop() )	break;
					m_pWindowList.GetPrev( pos );
				}

				if( pos )
					m_pWindowList.InsertAfter( pos, TopWindow );
				else 
					m_pWindowList.AddHead( TopWindow );
			}
		}
	}

	if( m_OldDragFlag == TRUE && m_pDragDialog == NULL )
		m_OldDragFlag = FALSE;
}


void cWindowManager::Render()
{
	PTRLISTPOS pos = m_pPlayerChatTooltip.GetHeadPosition();

	while(pos)
	{
		cWindow* window = (cWindow*)m_pPlayerChatTooltip.GetNext(pos);

		if(! window->IsHide() )			
			window->Render();
	}

	SetToolTipWindow( NULL ); //NULL로 하고, 아래 윈도 랜더할때 윈도들이 세팅한다.

	pos = m_pWindowList.GetHeadPosition();

	STATUSICONDLG->Render();
	PETSTATUSICONDLG->Render();

	if( m_OldDragFlag )
	{
		// 2010. 5. 10 이영준
		// AlwaysTop 윈도우를 렌더링할때
		// AlwaysTop 윈도우가 리스트 최상단에 있을때
		// 드래그 중인 윈도우가 바로 렌더링되어
		// 다른 윈도우에 가리는 버그를 수정하기 위해
		// AlwaysTop 윈도우를 나중에 렌더링하는 방식으로 수정
		cPtrList m_pTopWindowList;

		while(pos)
		{
			cWindow* const window = (cWindow *)m_pWindowList.GetNext(pos);

			if( window->IsAlwaysTop() )
			if( window->IsActive() )
			if( !window->IsHide() )	
			{
				m_pTopWindowList.AddTail( window );
				continue;
			}

			if( !window->IsDepend() && window->GetID() != m_id )
			if( !window->IsHide() )		//090119 pdy hide window		
				window->Render();
		}

		if( m_pDragDialog )
		if( !m_pDragDialog->IsHide() )			//090119 pdy hide window		
			m_pDragDialog->Render();

		PTRLISTPOS top_pos = m_pTopWindowList.GetHeadPosition();

		while( top_pos )
		{
			cWindow* const window = (cWindow *)m_pTopWindowList.GetNext(top_pos);

			if( window )
			if( !window->IsDepend() && window->GetID() != m_id )
				window->Render();
		}
	}
	else
	{
		cPtrList m_pTopWindowList;

		while(pos)
		{
			cWindow* const window = (cWindow *)m_pWindowList.GetNext(pos);

			if( window->IsAlwaysTop() )
			if( window->IsActive() )
			if( !window->IsHide() )		//090119 pdy hide window		
			{
				m_pTopWindowList.AddTail( window );
				continue;
			}

			if( !window->IsDepend() )
			if( !window->IsHide() )			//090119 pdy hide window		
				window->Render();
		}

		PTRLISTPOS top_pos = m_pTopWindowList.GetHeadPosition();

		while( top_pos )
		{
			cWindow* const window = (cWindow *)m_pTopWindowList.GetNext(top_pos);

			if( window )
			if( !window->IsDepend() && window->GetID() != m_id )
				window->Render();
		}

		if(m_pToolTipWindow)
		{
			m_pToolTipWindow->Render();
		}
	}

#ifdef _TESTCLIENT_
	extern g_bDisplayFPS;
	if(g_bDisplayFPS)
	{
		RECT rect;
		SetRect(&rect,30,100,500,150);
		char temp[256];
		VECTOR3 pos;
		HERO->GetPosition(&pos);
		sprintf(temp,"x: %f z : %f", pos.x, pos.z);
		CFONT_OBJ->RenderFont(0,temp,strlen(temp),&rect,0xffffffff);
	}
#endif
}

void cWindowManager::DestroyWindowAll()
{
	DestroyWindowProcess();
	
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cWindow * win = (cWindow *)m_pWindowList.GetNext(pos);
		SAFE_DELETE(win);
	}

	m_pWindowList.RemoveAll();
	m_pEditBoxListRef.RemoveAll();
	m_pIconDlgListRef.RemoveAll();
	m_pPlayerChatTooltip.RemoveAll();

	m_pDragDialog = NULL;
}

void cWindowManager::DeleteWindowForID(LONG id)
{
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cWindow * win = (cWindow *)m_pWindowList.GetAt(pos);

		if( win->GetID() == id )
		{
			m_pWindowList.RemoveAt(pos);
			SAFE_DELETE(win);
			if( m_id == id ) SetDragEnd();	//KES추가
			return;
		}

		m_pWindowList.GetNext(pos);
	}
}


void cWindowManager::DeleteWindow( cWindow* pWindow )
{
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cWindow* win = (cWindow *)m_pWindowList.GetAt(pos);

		if( win == pWindow )
		{
			m_pWindowList.RemoveAt(pos);

			if( m_id == win->GetID() ) SetDragEnd();	//KES추가
			SAFE_DELETE(win);
			return;
		}

		m_pWindowList.GetNext(pos);
	}
}

cDialog * cWindowManager::GetWindowForID(LONG id)
{

	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cDialog * win = (cDialog *)m_pWindowList.GetNext(pos);

		if(win->GetID() == id)
			return win;
	}
	return NULL;
}


cWindow * cWindowManager::GetWindowForIDEx(LONG id)
{
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while(pos)
	{
		cDialog * win = (cDialog *)m_pWindowList.GetNext(pos);

		if(win->GetID() == id)
			return (cWindow *)win;
		else
		{
			PTRLISTPOS pos2 = win->m_pComponentList.GetHeadPosition();
			while(pos2)
			{
				cWindow * win2 = (cWindow *)win->m_pComponentList.GetNext(pos2);
				if(win2->GetID() == id)
				{
					return win2;
				}
			}
		}
	}
	return NULL;
}

cDialog * cWindowManager::GetWindowForXYExceptIcon(LONG x, LONG y)
{
	PTRLISTPOS pos = m_pWindowList.GetTailPosition();

	while(pos)
	{
		cDialog * win = (cDialog *)m_pWindowList.GetPrev(pos);

		if(win->IsActive() == FALSE || win->GetType() & WT_ICON)
			continue;
		if(win->PtInWindow(x,y))
			return win;
	}
	return NULL;
}

cMsgBox* cWindowManager::MsgBox(LONG lMBId, eMBType nMBType, LPCTSTR pStrMsg, ...)
{
	if( !cMsgBox::IsInit() ) return NULL;

	if( WINDOWMGR->GetWindowForID( lMBId ) != NULL )
		return NULL;
	
	cMsgBox* pMsgBox = new cMsgBox;

	char msg[256] = {0,};

	if( pStrMsg != NULL ) 
	{
		va_list argList;

		va_start(argList, pStrMsg);
		vsprintf(msg, pStrMsg, argList);
		va_end(argList);
	}

//	pMsgBox->MsgBox( lMBId, nMBType, pStrMsg, cbMBFunc );
	pMsgBox->MsgBox( lMBId, nMBType, msg, MessageBox_Func );
	pMsgBox->SetAlwaysTop( TRUE );	//메세지 박스는 항상위. 싫으면 밖으로 빼기.
	AddWindow( pMsgBox );

	LONG lX = (LONG)pMsgBox->GetAbsX();
	LONG lY = (LONG)pMsgBox->GetAbsY();
	PositioningDlg( lX, lY );
	pMsgBox->SetAbsXY( lX, lY );
	pMsgBox->SetActive( TRUE );

	return pMsgBox;
}

void cWindowManager::PositioningDlg( LONG& rX, LONG& rY )
{
	int nCount = 0;
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();

	while( pos )
	{
		cWindow* pWindow = (cWindow*)m_pWindowList.GetNext( pos );

		if( (LONG)pWindow->GetAbsX() == rX )
		if( (LONG)pWindow->GetAbsY() == rY )
		{
			rX += 20;
			rY += 20;
			if( ++nCount >= 10 )//한없이 내려가는것 방지
			{
				rX -= 200;
				rY -= 200;
				nCount = 0;
			}
		}
	}
}

cDivideBox * cWindowManager::DivideBox( LONG lId, LONG x, LONG y, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void* vData2, char * strTitle )
{
	if( WINDOWMGR->GetWindowForID( lId ) != NULL )
		return NULL;

	cDivideBox * window = (cDivideBox * )GetDlgInfoFromFile("Data/Interface/Windows/DivideBox.bin", "rb" );
	
	window->CreateDivideBox( x, y, lId, cbFc1, cbFc2, vData1, vData2, strTitle );
	window->SetActive( TRUE );
	window->SetAlwaysTop( TRUE );

	AddWindow( window );

	return window;
}

cMoneyDivideBox * cWindowManager::MoneyDivideBox( LONG lId, LONG x, LONG y, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void* vData2, char * strTitle )
{
	if( WINDOWMGR->GetWindowForID( lId ) != NULL )
		return NULL;

	cMoneyDivideBox * window = (cMoneyDivideBox * )GetDlgInfoFromFile("Data/Interface/Windows/MoneyDivideBox.bin", "rb" );
	
	window->CreateDivideBox( x, y, lId, cbFc1, cbFc2, vData1, vData2, strTitle );
	window->SetActive( TRUE );
	window->SetAlwaysTop( TRUE );

	AddWindow( window );

	return window;
}

void cWindowManager::SetWindowTop( cWindow* win )
{
	PTRLISTPOS pos = m_pWindowList.Find( win );

	if( pos == NULL ) return;

	m_pWindowList.RemoveAt( pos );
	
	if( win->IsAlwaysTop() )
		m_pWindowList.AddTail(win);						//끝에 넣고
	else
	{
		pos = m_pWindowList.GetTailPosition();		//메세지박스를 젤 위에 두기 위해
		
		while( pos )
		{
			cWindow* pWindow = (cWindow*)m_pWindowList.GetAt(pos);

			if( pWindow )
			if( !pWindow->IsAlwaysTop() )	break;

			m_pWindowList.GetPrev(pos);
		}
		
		if( pos )
			m_pWindowList.InsertAfter( pos, win );
		else 
			m_pWindowList.AddHead( win );
	}
}


///////////////////
//Tab키 처리를 위해	//focus를 줄때 안줄때
void cWindowManager::SetFocusedEdit( cWindow* pEdit, BOOL bFocus )
{
	if( bFocus )
	{
		if( m_pFocusedEdit == pEdit ) return;

#ifdef _HACK_SHIELD_
		HS_PauseService();
#endif

#ifdef TAIWAN_LOCAL
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#elif defined _TW_LOCAL_
		HIMC hIMC = ImmGetContext( _g_hWnd );
		ImmSetOpenStatus( hIMC, TRUE );
		ImmReleaseContext( _g_hWnd, hIMC );

		HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
		SendMessage( hWndIme, WM_IME_CONTROL, IMC_OPENSTATUSWINDOW, 0 );
#endif

		m_pFocusedEdit = pEdit;
	}
	else
	{
		if( m_pFocusedEdit == pEdit )
		{
			m_pFocusedEdit = NULL;

#ifdef TAIWAN_LOCAL
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );

			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
#ifdef _TW_LOCAL_
			HIMC hIMC = ImmGetContext( _g_hWnd );
			ImmSetOpenStatus( hIMC, FALSE );
			ImmNotifyIME( hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			ImmNotifyIME( hIMC, NI_CLOSECANDIDATE, 0, 0 );

			ImmReleaseContext( _g_hWnd, hIMC );

			HWND hWndIme = ImmGetDefaultIMEWnd( _g_hWnd );
			SendMessage( hWndIme, WM_IME_CONTROL, IMC_CLOSESTATUSWINDOW, 0 );
#endif
#ifdef _HACK_SHIELD_
		HS_ResumeService();
#endif
		}
	}
}


BOOL cWindowManager::CloseAllAutoCloseWindows()
{
	PTRLISTPOS pos = m_pWindowList.GetHeadPosition();
	BOOL bClose = FALSE;

	//delete된 아이콘들이 아직 리스트에서 안없어졌을때 뻑이난다(?)
	while(pos)
	{
		cWindow* pWin = (cWindow*)m_pWindowList.GetNext(pos);

		if( pWin->IsAutoClose() && pWin->IsActive() )
		{
			pWin->SetActive( FALSE );
			bClose = TRUE;
		}
	}

	// desc_hseos_ESC01
	// S ESC 추가 added by hseos 2007.06.19
	// ..ESC 누르면 타겟도 사라지게 한다.
	if (OBJECTMGR->GetSelectedObject())
	{
		OBJECTMGR->GetSelectedObject()->OnDeselected();
		OBJECTMGR->SetSelectedObject(NULL);
	}
	// E ESC 추가 added by hseos 2007.06.19

	return bClose;
}


cWindow* cWindowManager::GetDlgInfoFromFile (char * filePath, char* mode )
{
//	return m_pScriptManager->GetDlgInfoFromFile(filePath, mode);
	return SCRIPTMGR->GetDlgInfoFromFile(filePath, mode);	// SCRIPTMGR로 대체.
	
}


cMsgBox* cWindowManager::GetFirstMsgBox()
{
	PTRLISTPOS pos = m_pWindowList.GetTailPosition();

	while(pos)
	{
		cWindow* window = (cWindow *)m_pWindowList.GetPrev(pos);

		if( !window ) continue;
		
		if( window->GetType()  == WT_MSGBOX )
			return (cMsgBox*)window;
	}

	return NULL;
}

void cWindowManager::CloseAllMsgBox()
{
	PTRLISTSEARCHSTART( (m_pWindowList), cWindow*, pWindow )
		if( pWindow )
		if( pWindow->GetType() == WT_MSGBOX )
		{
			((cMsgBox*)pWindow)->ForceClose();
		}
	PTRLISTSEARCHEND	
}

//사용에 주의하시오! 심각한 오류를 불러 일으킬수 있음. 제작:강은성
void cWindowManager::CloseAllMsgBoxNoFunc()
{
	PTRLISTSEARCHSTART( (m_pWindowList), cWindow*, pWindow )
		if( pWindow )
		if( pWindow->GetType() == WT_MSGBOX )
		{
			AddListDestroyWindow( pWindow );			
		}
	PTRLISTSEARCHEND	
}

//090119 pdy Window Hide Mode
//새로 추가된 HideAllWindow()는 Active는 그대로 두고 (윈도우를 닫지 않고)
//cObject에 m_bHide를 추가하여 Wnd매니져 단에서 랜더링과 인풋처리만 차단한다.
BOOL cWindowManager::HideAllWindow()
{
	WINDOWMGR->m_pActivedWindowList.RemoveAll();

	//GAMEIN->GetGuageDlg()->SetActive(FALSE);
	BOOL rt = FALSE;
	PTRLISTSEARCHSTART((m_pWindowList),cWindow*,pWindow)
		if(pWindow->IsActive() == TRUE)
		{
			// 070124 LYW --- Call function to save id of actived window.
			SaveAllActivedWindow( pWindow ) ;
    
			if(  pWindow->GetID() == PROGRESS_DIALOG ) 
				continue ;

			rt = TRUE;
			//090119 pdy Window Hide Mode
			pWindow->SetHide(TRUE);		//SetActive가 아닌 SetHide를 호출한다.
			m_bHideInterface = TRUE;
		}
	PTRLISTSEARCHEND

	//090119 pdy Window Hide Mode	
	if( IsDragWindow() )
		SetDragEnd();						//드래그중인게 있으면 End시킨다.

	return rt;
}

//////////////////////////////////////////////////////////////////////////
// yh edit 
BOOL cWindowManager::CloseAllWindow()
{
	WINDOWMGR->m_pActivedWindowList.RemoveAll();

	//GAMEIN->GetGuageDlg()->SetActive(FALSE);
	BOOL rt = FALSE;
	PTRLISTSEARCHSTART((m_pWindowList),cWindow*,pWindow)
		if(pWindow->IsActive() == TRUE)
		{
			// 070124 LYW --- Call function to save id of actived window.
			SaveAllActivedWindow( pWindow ) ;

			rt = TRUE;
			pWindow->SetActive(FALSE);
			m_bHideInterface = TRUE;
		}
	PTRLISTSEARCHEND

	return rt;
}
void cWindowManager::ShowBaseWindow()
{
	GAMEIN->GetChattingDlg()->SetActive(TRUE);
	GAMEIN->GetGuageDlg()->SetActive(TRUE);
	GAMEIN->GetQuickDlg()->SetActive(TRUE);
	GAMEIN->GetMainSystemDlg()->SetActive(TRUE);
	GAMEIN->GetMiniMapDialog()->SetActive(TRUE);
	GAMEIN->GetMonstermeterDlg()->SetActive(TRUE);
	// E 몬스터미터 추가 added by hseos 2007.07.26

	// 090525 pdy  하우징 맵일경우 기본 UI들을 띄운다
  	if( HOUSINGMGR->IsHousingMap() )
	{
		GAMEIN->GetHousingMainPointDlg()->SetActive( TRUE ) ;
		GAMEIN->GetHousingWarehouseButtonDlg()->SetActive( TRUE ) ;
	}

	m_bHideInterface = FALSE;
}
void cWindowManager::ToggleShowInterface()
{
	if(CloseAllWindow() == FALSE)
		ShowBaseWindow();
}

void cWindowManager::CreateGuildInvitationKindSelectionDlg()
{
	CGuildJoinDialog* dialog = ( CGuildJoinDialog* )GetDlgInfoFromFile("Data/Interface/Windows/JoinOption.bin", "rb");
	ASSERT( dialog );

	AddWindow( dialog );

	dialog->Linking();
	GAMEIN->SetGuildJoinDialog( dialog );
}

// 070124 LYW --- Add functino to save windows that actived.
void cWindowManager::SaveAllActivedWindow( cWindow* win )
{
	// 080214 LYW --- cWindowManager : [ 변환 중 입니다. ] 이 창은, 일정 시간이 지나면 자동으로 사라져야 한다.
	// 따라서, 일정 시간 후 사라져야 하는 윈도우는, 활성화 중인 윈도우에 포함시키지 않는다.
	// Ctrl + z 나 npc 대화 모드로 전환 시, 이런(일정 시간 후 자동으로 비활성화 되는 윈도우) 윈도우는, 
	// 비 활성화 중이던 창들을 활성화 시킬 때, 살아있게 된다. 일정 시간이 지나면 사라져야 함.

	if( !win ) return ;

	DWORD dwWindowID = 0 ;
	dwWindowID = win->GetID() ;
    
	if( dwWindowID == PROGRESS_DIALOG ) return ;

	LPVOID key = LPVOID(dwWindowID);

	PTRLISTPOS pos = m_pActivedWindowList.Find(key);

	if(0 == pos)
	{
		m_pActivedWindowList.AddTail(key);
	}
}


//090119 pdy Window Hide Mode
void cWindowManager::UnHideAllActivedWindow()
{
	for( PTRLISTPOS position = m_pActivedWindowList.GetHeadPosition(); position; )
	{
		const LONG windowId = ( LONG )m_pActivedWindowList.GetNext( position );

		cDialog* dialog = GetWindowForID( windowId );

		if( dialog )
		{
			dialog->SetHide(FALSE);
		}
	}
}

void cWindowManager::ShowAllActivedWindow()
{
	for( PTRLISTPOS position = m_pActivedWindowList.GetHeadPosition(); position; )
	{
		const LONG windowId = ( LONG )m_pActivedWindowList.GetNext( position );

		cDialog* dialog = GetWindowForID( windowId );

		if( dialog )
		{
			dialog->SetActive( TRUE );
		}
	}
}
// 2007/03/15 데이트 매칭 다이얼로그 추가-이진영
void cWindowManager::CreateDateMatchingDlg()
{
	CDateMatchingDlg*	pDlg	=	(CDateMatchingDlg*)GetDlgInfoFromFile("Data/Interface/Windows/DateMatchingDlg.bin", "rb");

	if( pDlg )
	{
		AddWindow( pDlg ) ;

		pDlg->Linking() ;

		GAMEIN->SetDateMatchingDlg( pDlg ) ;
	}
}

// 070605 LYW --- cWindowManager : Add function to create identification.
void cWindowManager::CreateIdentificationDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/IdentificationDlg.bin", "rb") ;

	if( window )
	{
		AddWindow(window) ;

		CIdentification* pDlg = (CIdentification*)window ;

		pDlg->Linking() ;
		pDlg->LoadFavorItem() ;

		GAMEIN->SetIdentificationDlg(pDlg) ;
	}
}

// 070605 LYW --- cWindowManager : Add function to create favor icon dialog.
void cWindowManager::CreateFavorIconDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/FavorIconDlg.bin", "rb") ;

	if( window )
	{
		AddWindow(window) ;

		CFavorIcon* pDlg = (CFavorIcon*)window ;

		pDlg->Linking() ;

		pDlg->LoadFavorItem() ;
		pDlg->ShowFavorItem() ;

		GAMEIN->SetFavorIconDlg(pDlg) ;
	}
}

void cWindowManager::CreateSkillTrainingDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/SKILLTRN.bin", "rb");
	AddWindow(window);

	cSkillTrainingDlg* pDlg = (cSkillTrainingDlg*)window;
	pDlg->Linking();
	GAMEIN->SetSkillTrainingDlg( pDlg );
}


void cWindowManager::CreateFamilyCreateDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/FamilyCreate.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFamilyCreateDlg((CFamilyCreateDialog * )window);
	GAMEIN->GetFamilyCreateDlg()->Linking();
/*	
	window = GetDlgInfoFromFile("Data/Interface/Windows/FamilyUnionCreate.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFamilyUnionCreateDlg((CFamilyUnionCreateDialog * )window);
	GAMEIN->GetFamilyUnionCreateDlg()->Linking();
*/
}

void cWindowManager::CreateFamilyDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/Family.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFamilyDlg((CFamilyDialog * )window);
	GAMEIN->GetFamilyDlg()->Linking();
}

void cWindowManager::CreateFamilyMarkDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/FamilyMark.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFamilyMarkDlg((CFamilyMarkDialog * )window);
	GAMEIN->GetFamilyMarkDlg()->Linking();
}

void cWindowManager::CreateFamilyNickNameDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/FamilyNickName.bin", "rb");
	
	ASSERT(window);
	AddWindow(window);
	
	GAMEIN->SetFamilyNickNameDlg((CFamilyNickNameDialog * )window);
	GAMEIN->GetFamilyNickNameDlg()->Linking();
}

void cWindowManager::CreateLimitDungeonDlg()
{
	cDialog* const dialog = (cDialog*)GetDlgInfoFromFile(
		"data/interface/windows/LimitDungeonDlg.bin",
		"rb");

	if(0 == dialog)
	{
		return;
	}

	AddWindow(dialog);
	dialog->OnActionEvent(0, 0, 0);
}

// 071017 LYW --- cWindowManager : Add function to create tutorial dialog.
void cWindowManager::CreateTutorialDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/TutorialDlg.bin", "rb") ;

	ASSERT(window) ;
	AddWindow(window) ;

	GAMEIN->SetTutorialDlg((cTutorialDlg*)window) ;
	GAMEIN->GetTutorialDlg()->Linking() ;

	TUTORIALMGR->Set_TutorialDlg(GAMEIN->GetTutorialDlg()) ;
}





// 071017 LYW --- cWindowManager : Add function to create help dialog.
void cWindowManager::CreateHelpDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/HelpDlg.bin", "rb") ;

	ASSERT(window) ;
	AddWindow(window) ;

	GAMEIN->SetHelpDlg((cHelperDlg*)window) ;
	GAMEIN->GetHelpDlg()->Linking() ;

	TUTORIALMGR->Set_HelperDlg(GAMEIN->GetHelpDlg()) ;
}





// 070123 LYW --- cWindowManager : Add function to create notice button dialog of tutorial.
void cWindowManager::CreateTutorialBtnDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/TutorialBtnDlg.bin", "rb") ;

	ASSERT(window) ;
	AddWindow(window) ;

	GAMEIN->SetTutorialBtnDlg((cTutorialBtnDlg*)window) ;
	GAMEIN->GetTutorialBtnDlg()->Linking() ;

	TUTORIALMGR->Set_TutorialBtnDlg(GAMEIN->GetTutorialBtnDlg()) ;
}

void cWindowManager::CreateItemPopupDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/ItemPopupDlg.bin", "rb") ;

	ASSERT(window) ;
	AddWindow(window) ;

	GAMEIN->SetItemPopupDlg((cItemPopupDlg*)window) ;
	GAMEIN->GetItemPopupDlg()->Linking() ;
}


// 071130 LYW --- cWindowManager : Add function to create map move dialog.
void cWindowManager::CreateMapMoveDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/MapMoveScrollDlg.bin", "rb") ;

	ASSERT(window) ;
	AddWindow(window) ;

	GAMEIN->SetMapMoveDlg((cMapMoveDialog*)window) ;
	GAMEIN->GetMapMoveDlg()->Linking() ;
}





// 071227 LYW --- cWindowManager : 캐릭터 이름 변경 다이얼로그 추가.
//=========================================================================
//	NAME : CreateChangeNameDlg
//	DESC : The function to create dialog to change hero name.
//=========================================================================
void cWindowManager::CreateChangeNameDlg()
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChangeNameDlg.bin", "rb") ;

	ASSERT(pWindow) ;
	AddWindow(pWindow) ;

	GAMEIN->SetChangeNameDlg((cChangeNameDialog*)pWindow) ;

	cChangeNameDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetChangeNameDlg() ;

	if( !pDlg )
	{
		MessageBox( NULL, "Failed to receive dialog to change name", "create dialog to change name", MB_OK ) ;
		ASSERT(pDlg) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateAutoNoteDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/AutoNoteDlg.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetAutoNoteDlg( (CAutoNoteDlg*)window );
	GAMEIN->GetAutoNoteDlg()->Linking();
}

void cWindowManager::CreateAutoAnswerDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/AutoAnswerDlg.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetAutoAnswerDlg( (CAutoAnswerDlg*)window );
	GAMEIN->GetAutoAnswerDlg()->Linking();
}
//--------------

//---KES 상점검색
void cWindowManager::CreateStoreSearchDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/StoreSearchDlg.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetStoreSearchDlg( (CStoreSearchDlg*)window );
	GAMEIN->GetStoreSearchDlg()->Linking();
}

// 080313 NYJ --- 전체외치기 다이얼로그 추가
void cWindowManager::CreateShoutDlg()
{
	cWindow* window = GetDlgInfoFromFile("Data/Interface/Windows/ShoutDlg.bin", "rb" );
	AddWindow(window);

	GAMEIN->SetShoutDlg((CShoutDialog*)window) ;	

	CShoutDialog* pDlg = NULL ;
	pDlg = GAMEIN->GetShoutDlg();

	if( !pDlg )
	{
		MessageBox( NULL, "Failed to receive dialog to Shout", "create dialog to change name", MB_OK ) ;
		ASSERT(pDlg) ;
		return ;
	}

	pDlg->Linking() ;
}

// 080414 LUJ, 외양 변경 창 생성
void cWindowManager::CreateBodyChangeDlg()
{
	CBodyChangeDialog* dialog = ( CBodyChangeDialog* )GetDlgInfoFromFile("Data/Interface/Windows/BodyChangeDialog.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	dialog->Linking();	
}


// 080403 LYW --- cWindowManager : Add functions to create dialog for chatroom system.
void cWindowManager::CreateChatRoomMainDlg()
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomMainDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCRMD", MB_OK) ;
		return ;
	}

	AddWindow(pWindow) ;

	GAMEIN->SetChatRoomMainDlg((CChatRoomMainDlg*)pWindow) ;

	CChatRoomMainDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomMainDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCRMD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateChatRoomCreateDlg()
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomCreateDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCRCD", MB_OK) ;
		return ;
	}

	AddWindow(pWindow) ;

	GAMEIN->SetChatRoomCreateDlg((CChatRoomCreateDlg*)pWindow) ;

	CChatRoomCreateDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomCreateDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCRCD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateChatRoomDlg()
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCRD", MB_OK) ;
		return ;
	}

	AddWindow(pWindow) ;

	GAMEIN->SetChatRoomDlg((CChatRoomDlg*)pWindow) ;

	CChatRoomDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCRD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateChatRoomGuestListDlg() 
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomGuestListDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCRGLD", MB_OK) ;
		return ;
	}

	AddWindow(pWindow) ;

	GAMEIN->SetChatRoomGuestListDlg((CChatRoomGuestListDlg*)pWindow) ;

	CChatRoomGuestListDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomGuestListDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCRGLD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateChatRoomOptionDlg() 
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomOptionDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCROD", MB_OK) ;
		return ;
	}

	AddWindow(pWindow) ;

	GAMEIN->SetChatRoomOptionDlg((CChatRoomOptionDlg*)pWindow) ;

	CChatRoomOptionDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomOptionDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCROD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

void cWindowManager::CreateChatRoomJoinDlg() 
{
	cWindow* pWindow = NULL ;
	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomJoinDlg.bin", "rb") ;

	ASSERT(pWindow) ;

	if(!pWindow)
	{
		MessageBox(	NULL, "FRDFF", "CCRJD", MB_OK) ;
		return ;
	}
AddWindow(pWindow) ;

	GAMEIN->SetChatRoomJoinDlg((CChatRoomJoinDlg*)pWindow) ;

	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		MessageBox(	NULL, "FRDFG", "CCRJD", MB_OK) ;
		return ;
	}

	pDlg->Linking() ;
}

//void cWindowManager::CreateChatRoomLobbyDlg()
//{
//	cWindow* pWindow = NULL ;
//	pWindow = GetDlgInfoFromFile("Data/Interface/Windows/ChatRoomLobbyDlg.bin", "rb") ;
//
//	ASSERT(pWindow) ;
//
//	if(!pWindow)
//	{
//		MessageBox(	NULL, "FRDFF", "CCRLD", MB_OK) ;
//		return ;
//	}
//
//	AddWindow(pWindow) ;
//
//	GAMEIN->SetChatRoomLobbyDlg((CChatRoomLobbyDlg*)pWindow) ;
//
//	CChatRoomLobbyDlg* pDlg = NULL ;
//	pDlg = GAMEIN->GetChatRoomLobbyDlg() ;
//
//	ASSERT(pDlg) ;
//
//	if(!pDlg)
//	{
//		MessageBox(	NULL, "FRDFG", "CCRLD", MB_OK) ;
//		return ;
//	}
//
//	pDlg->Linking() ;
//}

// 080421 NYJ --- 낚시 다이얼로그 추가
void cWindowManager::CreateFishingDlg()
{
	CFishingDialog* dialog = ( CFishingDialog* )GetDlgInfoFromFile("Data/Interface/Windows/FishingDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetFishingDlg(dialog);
	GAMEIN->GetFishingDlg()->Linking();
}

void cWindowManager::CreateFishingGaugeDlg()
{
	CFishingGaugeDialog* dialog = ( CFishingGaugeDialog* )GetDlgInfoFromFile("Data/Interface/Windows/FishingProgressDialog.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetFishingGaugeDlg(dialog);
	GAMEIN->GetFishingGaugeDlg()->Linking();
}

void cWindowManager::CreateFishingPointDlg()
{
	CFishingPointDialog* dialog = ( CFishingPointDialog* )GetDlgInfoFromFile("Data/Interface/Windows/FishingPointDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetFishingPointDlg(dialog);
	GAMEIN->GetFishingPointDlg()->Linking();
}

void cWindowManager::CreateFarmAnimalDlg()
{
	CAnimalDialog* dialog = ( CAnimalDialog* )GetDlgInfoFromFile("Data/Interface/Windows/FarmAnimalCageDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetAnimalDialog(dialog);
	GAMEIN->GetAnimalDialog()->Linking();
}

void cWindowManager::CreatePetDialog()
{
	CPetInfoDialog* pInfoDlg = ( CPetInfoDialog* )GetDlgInfoFromFile("Data/Interface/Windows/PetInfo.bin", "rb" );

	if( ! pInfoDlg )
	{
		return;
	}

	AddWindow( pInfoDlg );

	pInfoDlg->Linking();
	GAMEIN->SetPetInfoDlg( pInfoDlg );

	cWindow* pDlg = GetDlgInfoFromFile("Data/Interface/Windows/PetUI.bin", "rb" );

	if( !pDlg )
	{
		return;
	}

	AddWindow( pDlg );
	GAMEIN->SetPetUIDlg( (cDialog*)pDlg );

	CPetResurrectionDialog* pResDlg = ( CPetResurrectionDialog* )GetDlgInfoFromFile("Data/Interface/Windows/PetResDialog.bin", "rb" );

	if( ! pResDlg )
	{
		return;
	}

	AddWindow( pResDlg );

	pResDlg->Linking();
	GAMEIN->SetPetResurrectionDlg( pResDlg );
}


void cWindowManager::CreateGTResultDlg()
{
	CGTResultDlg* dialog = ( CGTResultDlg* )GetDlgInfoFromFile("Data/Interface/Windows/GTResultDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetGTResultDlg(dialog);
	GAMEIN->GetGTResultDlg()->Linking();
}

void cWindowManager::CreateGTEntryEditDlg()
{
	CGTEntryEditDlg* dialog = ( CGTEntryEditDlg* )GetDlgInfoFromFile("Data/Interface/Windows/GTEntryEditDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetGTEntryEditDlg(dialog);
	GAMEIN->GetGTEntryEditDlg()->Linking();
}

void cWindowManager::CreateCookDlg()
{
	CCookDlg* dialog = ( CCookDlg* )GetDlgInfoFromFile("Data/Interface/Windows/CookDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetCookDlg(dialog);
	GAMEIN->GetCookDlg()->Linking();
}

// 080929 LYW --- cWindowManager : 공성 깃발 정보 다이얼로그 추가.
void cWindowManager::CreateSiegeWarFlagDlg()
{
	CSiegeWarFlagDlg* pDlg = ( CSiegeWarFlagDlg* )GetDlgInfoFromFile("Data/Interface/Windows/SiegeWarFlagDlg.bin", "rb") ;

	if( !pDlg ) return ;

	AddWindow( pDlg ) ;

	GAMEIN->SetSiegeWarFlagDlg( pDlg ) ;

	CSiegeWarFlagDlg* pFlagDlg = NULL ;
	pFlagDlg = GAMEIN->GetSiegeWarFlagDlg() ;

	if( !pFlagDlg ) return ;

	pFlagDlg->Linking() ;
}

// 080930 LYW --- cWindowManager : 페이드 인/아웃 다이얼로그 추가.
void cWindowManager::CreateFadeDlg()
{
	// 다이얼로그 정보를 받는다.
	CFadeDlg* pDlg = NULL ;
	pDlg = ( CFadeDlg* )GetDlgInfoFromFile("Data/Interface/Windows/FadeDlg.bin", "rb") ;

	if( !pDlg ) return ;

	// 윈도우 매니져에 다이얼로그를 등록한다.
	AddWindow( pDlg ) ;

	// GAMEIN에 다이얼로그를 등록한다.
	GAMEIN->SetFadeDlg( pDlg ) ;

	CFadeDlg* pFadeDlg = NULL ;
	pFadeDlg = GAMEIN->GetFadeDlg() ;

	if( !pFadeDlg ) return ;

	pFadeDlg->Initialize() ;
}

void cWindowManager::CreatePopupMenuDlg()
{
	CPopupMenuDlg* dialog = ( CPopupMenuDlg* )GetDlgInfoFromFile("Data/Interface/Windows/PopupMenuDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetPopupMenuDlg(dialog);
	GAMEIN->GetPopupMenuDlg()->Linking();
}

// 090413 ONS 전직 가이드 다이얼로그 추가.
void cWindowManager::CreateGradeClassDlg()
{
	cWindow * window = GetDlgInfoFromFile("Data/Interface/Windows/GRADECLASSDLG_TU.bin", "rb");
	AddWindow(window);

	CGradeClassDlg* pDlg = (CGradeClassDlg*)window;
	GAMEIN->SetGradeClassDlg(pDlg);
	pDlg->Linking();
}

//090323 pdy 하우징 설치 UI추가
void cWindowManager::CreateHousingRevolDlg()
{
	CHousingRevolDlg* dialog = ( CHousingRevolDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingRevolDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingRevolDlg(dialog);
	GAMEIN->GetHousingRevolDlg()->Linking();
}

//090330 pdy 하우징 창고 UI추가
void cWindowManager::CreateHousingWereHouseDlg()
{
	cHousingWarehouseDlg* dialog = ( cHousingWarehouseDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingWarehouseDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingWarehouseDlg(dialog);
	GAMEIN->GetHousingWarehouseDlg()->Linking();
}

//090409 pdy 하우징 하우스 검색 UI추가
void cWindowManager::CreateHouseSearchDlg()
{
	cHouseSearchDlg* dialog = ( cHouseSearchDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HouseSearchDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHouseSearchDlg(dialog);
	GAMEIN->GetHouseSearchDlg()->Linking();

}

//090410 pdy 하우징 하우스 이름설정 UI추가
void cWindowManager::CreateHouseNameDlg()
{
	cHouseNameDlg* dialog = ( cHouseNameDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HouseNameDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHouseNameDlg(dialog);
	GAMEIN->GetHouseNameDlg()->Linking();

}

//090414 pdy 하우징 꾸미기포인트 보상 UI추가
void cWindowManager::CreateHousingDecoPointDlg()
{
	cHousingDecoPointDlg* dialog = ( cHousingDecoPointDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingDecoPointDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingDecoPointDlg(dialog);
	GAMEIN->GetHousingDecoPointDlg()->Linking();

}

//090507 pdy 하우징 액션 팝업매뉴 UI추가
void cWindowManager::CreateHousingActionPopupMenuDlg()
{
	cHousingActionPopupMenuDlg* dialog = ( cHousingActionPopupMenuDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingActionPopupMenuDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingActionPopupMenuDlg(dialog);
	GAMEIN->GetHousingActionPopupMenuDlg()->Linking();

}

//090525 pdy 하우징 매인포인트 UI추가
void cWindowManager::CreateHousingMainPointDlg()
{
	cHousingMainPointDlg* dialog = ( cHousingMainPointDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingMainPointDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingMainPointDlg(dialog);
	GAMEIN->GetHousingMainPointDlg()->Linking();

}

//090525 pdy 하우징 내집창고 버튼 UI추가
void cWindowManager::CreateHousingWarehouseButtonDlg()
{
	cHousingWarehouseButtonDlg* dialog = ( cHousingWarehouseButtonDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingWarehouseButtonDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingWarehouseButtonDlg(dialog);
	GAMEIN->GetHousingWarehouseButtonDlg()->Linking();

}

//090622 NYJ 하우징 웹브라우저
void cWindowManager::CreateHousingWebDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/HousingWebDlg.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetHousingWebDlg( (cHousingWebDlg*)window );
	GAMEIN->GetHousingWebDlg()->Linking();
}

// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
void cWindowManager::CreateRidePopupMenuDlg()
{
	CRidePopupMenuDlg* dialog = ( CRidePopupMenuDlg* )GetDlgInfoFromFile("Data/Interface/Windows/RidePopupMenuDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetRidePopupMenuDlg(dialog);
	GAMEIN->GetRidePopupMenuDlg()->Linking();
}

// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
void cWindowManager::CreateInputNameDlg()
{
	CInputNameDlg* dialog = ( CInputNameDlg* )GetDlgInfoFromFile("Data/Interface/Windows/InputNameDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetInputNameDlg(dialog);
	dialog->Linking();
}

//090708 pdy 하우징 꾸미기모드 버튼 UI추가
void cWindowManager::CreateHousingDecoModeBtDlg()
{
	cHousingDecoModeBtDlg* dialog = ( cHousingDecoModeBtDlg* )GetDlgInfoFromFile("Data/Interface/Windows/HousingDecoModeBtDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetHousingDecoModeBtDlg(dialog);
	GAMEIN->GetHousingDecoModeBtDlg()->Linking();

}

void cWindowManager::CreateNpcNoticeDlg()
{
	cNpcNoticeDlg* dialog = ( cNpcNoticeDlg* )GetDlgInfoFromFile("Data/Interface/Windows/NpcNoticeDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	dialog->Linking();
}

void cWindowManager::CreateTriggerTimerDlg()
{
 	CTriggerTimerDlg* dialog = ( CTriggerTimerDlg* )GetDlgInfoFromFile("Data/Interface/Windows/TriggerTimerDlg.bin", "rb" );

	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetTriggerTimerDlg(dialog);
	GAMEIN->GetTriggerTimerDlg()->Linking();
}

void cWindowManager::CreateVideoCaptureDlg()
{
	CVideoCaptureDlg* dialog = ( CVideoCaptureDlg* )GetDlgInfoFromFile("Data/Interface/Windows/VideoCaptureDlg.bin", "rb" );
	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetVideoCaptureDlg( dialog );
	dialog->Linking();
}

void cWindowManager::CreateAdditionalButtonDlg()
{
	cAdditionalButtonDlg* dialog = ( cAdditionalButtonDlg* )GetDlgInfoFromFile("Data/Interface/Windows/AdditionalButtonDlg.bin", "rb" );
	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetAdditionalButtonDlg( dialog );
	dialog->Linking();
}

void cWindowManager::GameRatingDlg()
{
	CGameRatingDialog* dialog = ( CGameRatingDialog* )GetDlgInfoFromFile("Data/Interface/Windows/GameRating.bin", "rb" );
	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	dialog->SetAlwaysTop( TRUE );
}

// 100511 ONS 전직변경 다이얼로그 추가
void cWindowManager::CreateChangeClassDlg()
{
	CChangeClassDlg* dialog = ( CChangeClassDlg* )GetDlgInfoFromFile("Data/Interface/Windows/ChangeClass.bin", "rb");
	if( ! dialog )
	{
		return;
	}
	AddWindow( dialog );
	GAMEIN->SetChangeClassDlg( dialog );
	dialog->Linking();
}

void cWindowManager::CreateConsignmentGuideDlg()
{
	CConsignmentGuideDlg* dialog = ( CConsignmentGuideDlg* )GetDlgInfoFromFile("Data/Interface/Windows/Consignment_Guide.bin", "rb" );
	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	dialog->Linking();
}

void cWindowManager::CreateConsignmentDlg()
{
	CConsignmentDlg* dialog = ( CConsignmentDlg* )GetDlgInfoFromFile("Data/Interface/Windows/Consignment.bin", "rb" );
	if( ! dialog )
	{
		return;
	}

	AddWindow( dialog );
	GAMEIN->SetConsignmentDlg( dialog );
	dialog->Linking();
}

#ifdef _TW_LOCAL_
void cWindowManager::CreateItemShopBrowserDlg()
{
	cWindow* window = GetDlgInfoFromFile( "Data/Interface/Windows/ItemShopDlg.bin", "rb" );
	AddWindow(window);
	GAMEIN->SetItemShopDlg( (CItemShopDlg*)window );
	GAMEIN->GetItemShopDlg()->Linking();
}
#endif

void cWindowManager::AddWindow(cWindow* window)
{ 
	if(0 == window)
	{
		_tprintf(
			_T("error: Window is null\n"));
		return;
	}	

	cWindow* const duplicatedWindow = GetWindowForID(
		window->GetID());

	if(duplicatedWindow)
	{
		_tprintf(
			"error: Window %s is duplicated\n",
			NAMESEARCH(duplicatedWindow->GetID()));
		return;
	}

	m_pWindowList.AddTail(
		window); 
}
