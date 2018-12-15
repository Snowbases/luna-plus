#include "stdafx.h"
#include "cScriptManager.h"
#include "../GlobalEventFunc.h"
#include "../WindowIDEnum.h"
#include "cResourceManager.h"
#include "../CharacterDialog.h"
#include "../InventoryExDialog.h"
#include "../MoneyDlg.h"
#include "../DealDialog.h"
#include "../PartyMemberDlg.h"
#include "../PartyBtnDlg.h"
#include "../PartyCreateDlg.h"
#include "../PartyInviteDlg.h"
#include "../MousePointer.h"
#include "../MiniMapDlg.h"
#include "../BigMapDlg.h"
#include "../StorageDialog.h"
#include "../FriendDialog.h"
#include "../NoteDialog.h"
#include "../MiniNoteDialog.h"
#include "../MiniNoteDialogRead.h"
#include "../MiniFriendDialog.h"
#include "../ReviveDialog.h"
#include "../OptionDialog.h"
#include "../ExitDialog.h"
#include "../ExchangeDialog.h"
#include "../PKLootingDialog.h"
#include "../StreetStall.h"
#include "../StallKindSelectDlg.h"		// LYJ 051017 구입노점상 추가
#include "../StreetBuyStall.h"
#include "../BuyRegDialog.h"
#include "../NpcScriptDialog.h"
#include "../ComposeDialog.h"
#include "../ObjectGuagen.h"
#include "../cComboBoxEx.h"
#include "../MonsterGuageDlg.h"
#include "../PrivateWarehouseDialog.h"
#include "../cDivideBox.h"
#include "../cmoneydividebox.h"
#include "../LoadingDlg.h"
#include "../EnchantDialog.h"
#include "../MixDialog.h"
#include "../ReinforceDlg.h"
#include "../ApplyOptionDialog.h"
#include "../ChannelDialog.h"
#include "../ServerListDialog.h"
#include "../QuestTotalDialog.h"
#include "../QuestDialog.h"
#include "../QuestQuickViewDialog.h"
#include "../ChinaAdviceDlg.h"
#include "../DissolveDialog.h"
#include "../GuildCreateDialog.h"
#include "../GuildDialog.h"
#include "../GuildInviteDialog.h"
#include "../GuildMarkDialog.h"
#include "../GuildLevelupDialog.h"
#include "../GuildNickNameDialog.h"
#include "../GuildRankDialog.h"
#include "../GuildWarehouseRankDialog.h"
#include "../GuildWarehouseDialog.h"
#include "../GuildRevivalDialog.h"
#include "../ItemShopDialog.h"
#include "../ItemShopGridDialog.h"
#include "../ShoutDialog.h"
#include "../GTStandingDialog.h"
#include "../GTStandingDialog16.h"
#include "../GTBattleListDialog.h"
#include "../GTScoreInfoDialog.h"
#include "../ItemManager.h"
#include "../GuildFieldWarDialog.h"
#include "../GuageDialog.h"
#include "../ReinforceGuideDialog.h"		// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
#include "../KeySettingTipDlg.h"
#include "../ProgressDialog.h"
#include "../LimitDungeonDialog.h"
#include "../GuildNoteDlg.h"
#include "../UnionNoteDlg.h"
#include "../GuildNoticeDlg.h"
#include "../GuildJoinDialog.h"
#include "../ChattingDlg.h"
#include "../CristalEmpireBonusDialog.h"
#include "../ProfileDlg.h"
#include "../CharMakeNewDlg.h"
#include "../CertificateDlg.h"
#include "../MainSystemDlg.h"
#include "../cSkillTreeDlg.h"
#include "../cJobSkillDlg.h"
#include "../NpcImageDlg.h"
#include "../QuickDlg.h"
#include "../QuickSlotDlg.h"
#include "../DateMatchingDlg.h"
#include "../cSkillTrainingDlg.h"
#include "../FamilyCreateDialog.h"
#include "../FamilyDialog.h"
#include "../FamilyMarkDialog.h"
#include "../FamilyNickNameDialog.h"
#include "../RideOptionDlg.h"
#include "../hseos/Monstermeter/SHMonstermeterDlg.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "TutorialDlg.h"
#include "HelperDlg.h"
#include "TutorialBtnDlg.h"
#include "ItemPopupDlg.h"											
#include "cMapMoveDialog.h"
#include "cChangeNameDialog.h"
#include "./Interface/cNumberPadDialog.h"
#include "AutoNoteDlg.h"
#include "AutoAnswerDlg.h"
#include "StoreSearchDlg.h"
#include "BodyChangeDialog.h"
#include "FishingDialog.h"
#include "FishingGaugeDialog.h"
#include "FishingPointDialog.h"
#include "./ChatRoomMainDlg.h"
#include "./ChatRoomCreateDlg.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomOptionDlg.h"
#include "./ChatRoomJoinDlg.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "PetInfoDialog.h"
#include "PetStateDialog.h"
#include "PetWearedDialog.h"
#include "PetInvenDialog.h"
#include "PetResurrectionDialog.h"
#include "GTResultDlg.h"
#include "GTEntryEditDlg.h"
#include "CookDlg.h"
#include "SiegeWarFlagDlg.h"
#include "FadeDlg.h"
#include "popupmenudlg.h"
#include "RidePopupMenuDlg.h"
#include "GradeClassDlg.h"
#include "HousingRevolDlg.h"			//090323 pdy 하우징 설치 UI추가
#include "cHousingWarehouseDlg.h"		//090330 pdy 하우징 창고 UI추가
#include "cScrollIconGridDialog.h"		//090330 pdy ScrollIconGridDialog 추가
#include "cHouseSearchDlg.h"			//090409 pdy 하우징 하우스 검색 UI추가
#include "cHouseNameDlg.h"				//090410 pdy 하우징 하우스 이름설정 UI추가
#include "cHousingDecoPointDlg.h"		//090414 pdy 하우징 꾸미기포인트 보상 UI추가
#include "cHousingActionPopupMenuDlg.h" //090507 pdy 하우징 액션 팝업매뉴 UI추가
#include "cHousingMainPointDlg.h"		//090525 pdy 하우징 매인포인트 UI추가
#include "cHousingWarehouseButtonDlg.h"	//090525 pdy 하우징 내집창고 버튼 UI추가
#include "cHousingDecoModeBtDlg.h"		//090708 pdy 하우징 꾸미기모드 버튼 UI추가
#include "cBrowser.h"
#include "cHousingWebDlg.h"				//090622 NYJ 하우징 웹브라우저
#include "InputNameDlg.h"
#include "NpcNoticeDlg.h"
#include "Trigger/TriggerTimerDlg.h"
#include "UiSettingManager.h"			// 0911116 ONS UI위치저장 
#include "VideoCaptureDlg.h"
#include "AdditionalButtonDlg.h"
#include "GameRatingDialog.h"
#include "ChangeClassDlg.h"
#include "ConsignmentGuideDlg.h"
#include "ConsignmentDlg.h"
#include "WorldMapDlg.h"
#include "WearedExDialog.h"
#ifdef _TW_LOCAL_
#include "ItemShopDlg.h"
#endif

#include "MonsterTargetDlg.h"
#include "BossTargetDlg.h"
#include "BattleGuageDlg.h"
#include "CharacterTargetDlg.h"
#include "ChannelComboBox.h"

// 080112 LYW --- cScriptManager : 다이얼로그 테이블 enum 코드 추가 처리.
enum ENUM_DLGINFO_TABLES
{
	eDLG,								
	eCHARGAGE,						
	eLISTDLG,							
	eLISTDLGEX,						
	eCHARINFODLG,						
	eINVENTORYDLG,					
	eSTALLOPTIONDLG,					
	eBUYSTALLDLG,
	eBUYREGDLG,
	eSTALLSELLDLG,
	eNPCCHATDLG,
	eHELPDLG,
	eCHARMAKEDLG,
	eHELPERSPEECHDLG,
	eMONEYDLG,
	eNPCSHOPDLG,
	eQUICKSLOTDLG,
	ePA_MEMBER1DLG,
	ePA_MEMBER2DLG,
	ePA_MEMBER3DLG,
	ePA_MEMBER4DLG,
	ePA_MEMBER5DLG,
	ePA_MEMBER6DLG,
	ePARTYSETDLG,
	ePA_CREATEDLG,
	ePA_INVITEDLG,
	eANI,
	eMOUSEPOINTER,
	eSTORAGEDLG,
	eFRIENDDLG,
	eNOTEDLG,							
	eMINNOTEDLG,
	eFRIENDINVITEDLG,
	eREVIVALDLG,
	eCHANNELDLG,
	eWANTNPCDLG,
	eWANTREGISTDLG,
	eMAINMENUDLG,
	eMONSTERMETERDLG,
	eOPTIONDLG,
	eTRADEDLG,
	eCHARSTATEDLG,
	eMENUSLOTDIALOG,
	ePKLOOTINGDLG,
	eCNADVICEDLG,
	eDISSOLUTIONDLG,
	eMNPLAYROOMDLG,
	eMNCHANNELDLG,
	eMINIMAPDLG,
	eBIGMAPDLG,
	eTARGETDLG,						
	eDIVIDEBOX,
	eMONEYDIVIDEBOX,
	eLOADINGDLG,
	eUPGRADEDLG,
	eMIXDLG,
	eREINFORCEDLG,
	eSERVERSELECTDLG,
	eQUESTDLG,
	eBAILDLG,
	eGMOPENTALKDLG,
	eITEM_MALLDLG,
	eGUILDCREATEDLG,
	eGUILDUNIONCREATEDLG,
	eGUILDDLG,
	eGUILDINVITEDlG,
	eGUILDMARKDLG,
	eGUILDLEVELUPDLG,
	eGUILDNICKNAMEDLG,
	eGUILDRANKDLG,
	eGUILDWAREHOUSEDLG,
	eITEMMALLWAREHOUSE,						
	eSAVEMOVEDLG,
	eCHANGSAVEDLG,
	eGFWARDECLAREDLG,
	eGFWARRESULTDLG,
	eGFWARINFODLG,
	eGUILDWARINFODLG,
	eSKILLPOINTAGAINDLG,
	eSKILLPOINTOPDLG,
	ePARTYWARDLG,
	eCHALLWORLDCREATEDLG,
	eSEECHASEDLG,
	eCHASEDLG,
	eCHNAMECHANGEDLG,
	eOPCHNAMECHANGEDLG,
	eGDTENTRYDLG,
	eGDTENTRYCANCELDLG,
	eGTBATTLETABLE_8,
	eGDTVIEWDLG,
	eGTSCORETIMEDLG,
	eSEIGEWARTIMEREGDLG,				
	eSEIGEWARPROTECTREGDLG,
	eSEIGEWARINFODLG,
	eSWSTARTTIMEDLG,
	eITEMLOCKDLG,
	eSWPROFITDLG,
	eENGRAVEDLG,
	eSWCOUNTDLG,
	eCHANGEJOBDLG,
	eRFDefaultDlg,
	eRareCreateDlg,	
	eRFDATAGUIDEDLG,
	eTIPBROWSERDLG,
	eKSDLG,
	eGUILDNOTEDLG,					
	eALLYNOTEDLG,
	eGUILDNOTICEDLG,
	eGUILDPLUSTIMEDLG,
	eSCREENSHOTDLG,
	eCHATWINDOWDLG,
	eCHARMAKEPROFILEDLG,
	eCHARDEL,
	eMENUDLG,
	eJOINOPTIONDLG,
	eSKILLDLG,
	eNIDLG,
	eDATEMATCHINGDLG,
	eIDENTIFICATIONDLG, 
	eFAVORICONDLG,
	eCHATTING,
	eTRAININGDLG,
	eFAMILYDLG,
	eFAMILYCREATEDLG,
	eFAMILYINVITEDlG,
	eFAMILYMARKDLG,					
	eFAMILYNICKNAMEDLG,
	eGUILDWAREHOUSERANKDLG,
	eQUESTQUICKVIEWDLG,
	eGUILDREVIVALDLG,
	eFARM_BUY_DLG,
	eFARM_UPGRADE_DLG,
	ePROGRESSDLG,
	eFARM_MANAGE_DLG,
	eTUTORIALDLG,
	eHELPERDLG,
	eTUTORIALBTNDLG,
	eITEMMALLBTNDLG,
	eITEMPOPUPDLG,
	eMAPMOVEDLG,
	eDATEZONELISTDLG,
	eCHALLENGEZONELISTDLG,
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2008.01.24
	eCHALLENGEZONECLEARNO1DLG,
	// E 데이트 존 추가 added by hseos 2008.01.24
	eCHANGENAMEDLG, 
	eNUMBERPAD_DLG,

	//---KES AUTONOTE
	eAUTONOTEDLG,
	eAUTOANSWERDLG,
	//---------------

	// 080218 LUJ, 옵션 부여 창
	eAPPLYOPTIONDLG,

	eSHOUTDLG,	// 080313 NYJ --- 전체외치기 다이얼로그

	//---KES 상점검색
	eSTORESEARCHDLG,
	//---------------

	// 080414 LUJ, 외양 변경 창
	eBODYCHANGEDLG,
	
	eFISHINGDLG,		// 080421 NYJ --- 낚시 다이얼로그
	eFISHINGGAUGEDLG,	// 080422 NYJ --- 낚시게이지 다이얼로그
	eFISHINGPOINTDLG,	// 080505 NYJ --- 낚시포인트 다이얼로그

	eWEATHERDLG,		// 080505 NYJ --- 날씨 다이얼로그


// 080401 LYW --- cScriptManager : Add dialogs part for chatroom system.
	eCHATROOMMAINDLG,
	eCHATROOMCREATEDLG,
	eCHATROOMDLG,
	eCHATROOMGUESTLISTDLG, 
	eCHATROOMOPTIONDLG, 
	eCHATROOMJOINDLG,
	//eCHATROOMLOBBYDLG,
	eFARMANIMALDLG,

	ePET_INFO_DLG,
	ePET_UI_DLG,
	ePET_RES_DLG,
	
	eGTRESULTDLG,
	eGTBATTLETABLE_16,
	eGTENTRYEDITDLG,
	// 080916 LUJ, 아이템 합성 창
	eCOMPOSEDLG,
	eCOOKDLG,

	// 080929 LYW --- cScriptManager : 공성 깃발 정보 다이얼로그 추가.
	eSIEGEWARFALGDLG,

	// 080930 LYW --- cScriptManager : 페이드 인/아웃 다이얼로그 추가.
	eFADEDLG,
	ePOPUPMENUDLG,
	// 090422 ShinJS --- 탈것을 위한 팝업메뉴 다이얼로그 추가
	eRIDEPOPUPMENUDLG,
	// 090413 ONS 전직 가이드 다이얼로그 추가.
	eGRADECLASS_TU_DLG,

	//090323 pdy 하우징 설치 UI추가
	eHOUSING_REVOLDLG,

	//090330 pdy 하우징 창고 UI추가
	eHOUSING_WAREHOUSE_DLG,

	//090409 pdy 하우징 하우스 검색 UI추가
	eHOUSE_SEARCH_DLG,

	//090410 pdy 하우징 하우스 이름설정 UI추가
	eHOUSE_NAME_DLG,

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	eHOUSING_DECOPOINTDLG,

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	eHOUSING_ACTIONPOPUPMENUDLG,

	//090525 pdy 하우징 매인포인트 UI추가
	eHOUSING_MAINPOINT_DLG,

	//090525 pdy 하우징 내집창고 버튼 UI추가
	eHOUSING_WAREHOUSEBT_DLG,

	//090622 NYJ 하우징 웹브라우저
	eHOUSINGWEBDLG,

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	eInputNameDlg,

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	eHOUSING_DECOMODE_BT_DLG,

	// 090925 ShinJS --- Npc Notice Dialog 추가
	eNPC_NOTICE_DLG,
	//091102 NYJ - 트리거 타이머
	eTRIGGER_TIMER_DLG,
	// 091106 LUJ, 리미트 던젼
  	eLIMIT_DUNGEON_DLG,

	// 091210 ShinJS --- Video Capture Dialog 추가
	eVIDEO_CAPTURE_DLG,

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	eADDITIONAL_BUTTON_DLG,
	eCristalEmpireBonusDialog,
	eRideOptionDialog,
	eGameRatingDialog,
	eChangeClassDialog,

	// 100420 NYJ 우편읽기창 추가
	eMINNOTEDLGREAD,
	// 100503 NYJ 판매대행
	eCONSIGNMENTGUIDEDLG,
	eCONSIGNMENTDLG,
	// 100628 ShinJS 월드맵
	eWORLDMAPDLG,

	// 몬스터 TargetDialog
	eMONSTERTARGETDLG,
	// 보스 몬스터 TargetDialog
	eBOSSTARGETDLG,
	// HERO 전투상태일때 뜨는 정보
	eBATTLEGUAGEDLG,
	// Character TargetDialog
	eCHARACTERGUAGEDLG,

#ifdef	_TW_LOCAL_
	eITEMSHOPDLG,
#endif
} ;

// GetDlgInfoFromFile()용.
char	g_szDlgInfoTable[][50]	=	
{
	"$DLG",								// 0
	"$CHARGAGE",						// 1
	"$LISTDLG",							// 2
	"$LISTDLGEX",						// 3
	"$CHARINFODLG",						// 4
	"$INVENTORYDLG",					// 5
	"$STALLOPTIONDLG",					// 6
	"$BUYSTALLDLG",						// 7
	"$BUYREGDLG",						// 8
	"$STALLSELLDLG",					// 9
	"$NPCCHATDLG",						// 10
	"$HELPDLG",							// 11
	"$CHARMAKEDLG",						// 12
	"$HELPERSPEECHDLG",					// 13
	"$MONEYDLG",						// 14
	"$NPCSHOPDLG",						// 15
	"$QUICKSLOTDLG",					// 16
	"$PA_MEMBER1DLG",					// 17
	"$PA_MEMBER2DLG",					// 18
	"$PA_MEMBER3DLG",					// 19
	"$PA_MEMBER4DLG",					// 20
	"$PA_MEMBER5DLG",					// 21
	"$PA_MEMBER6DLG",					// 22
	"$PARTYSETDLG",						// 23
	"$PA_CREATEDLG",					// 24
	"$PA_INVITEDLG",					// 25
	"$ANI",								// 26
	"$MOUSEPOINTER",					// 27
	"$STORAGEDLG",						// 28
	"$FRIENDDLG",						// 29
	"$NOTEDLG",							// 30
	"$MINNOTEDLG",						// 31
	"$FRIENDINVITEDLG",					// 32
	"$REVIVALDLG",						// 33
	"$CHANNELDLG",						// 34
	"$WANTNPCDLG",						// 35
	"$WANTREGISTDLG",					// 36
	"$MAINMENUDLG",						// 37
	"$MONSTERMETERDLG",					// 38
	"$OPTIONDLG",						// 39
	"$TRADEDLG",						// 40
	"$CHARSTATEDLG",					// 41
	"$MENUSLOTDIALOG",					// 42
	"$PKLOOTINGDLG",					// 43
	"$CNADVICEDLG",						// 44
	"$DISSOLUTIONDLG",					// 45
	"$MNPLAYROOMDLG",					// 46
	"$MNCHANNELDLG",					// 47
	"$MINIMAPDLG",						// 48
	"$BIGMAPDLG",						// 49
	"$TARGETDLG",						// 50
	"$DIVIDEBOX",						// 51
	"$MONEYDIVIDEBOX",					// 52
	"$LOADINGDLG",						// 53
	"$UPGRADEDLG",						// 54
	"$MIXDLG",							// 55
	"$REINFORCEDLG",					// 56
	"$SERVERSELECTDLG",					// 57
	"$QUESTDLG",						// 58
	"$BAILDLG",							// 59
	"$GMOPENTALKDLG",					// 60
	"$ITEM_MALLDLG",					// 61
	"$GUILDCREATEDLG",					// 62
	"$GUILDUNIONCREATEDLG",				// 63
	"$GUILDDLG",						// 64
	"$GUILDINVITEDlG",					// 65
	"$GUILDMARKDLG",					// 66
	"$GUILDLEVELUPDLG",					// 67
	"$GUILDNICKNAMEDLG",				// 68
	"$GUILDRANKDLG",					// 69
	"$GUILDWAREHOUSEDLG",				// 70
	"$ITEMMALLWAREHOUSE",				// 71
	"$SAVEMOVEDLG",						// 72
	"$CHANGSAVEDLG",					// 73
	"$GFWARDECLAREDLG",					// 74
	"$GFWARRESULTDLG",					// 75
	"$GFWARINFODLG",					// 76
	"$GUILDWARINFODLG",					// 77
	"$SKILLPOINTAGAINDLG",				// 78
	"$SKILLPOINTOPDLG",					// 79
	"$PARTYWARDLG",						// 80
	"$CHALLWORLDCREATEDLG",				// 81
	"$SEECHASEDLG",						// 82
	"$CHASEDLG",						// 83
	"$CHNAMECHANGEDLG",					// 84
	"$OPCHNAMECHANGEDLG",				// 85
	"$GDTENTRYDLG",						// 86 길드토너먼트 DLG
	"$GDTENTRYCANCELDLG",				// 87 길드토너먼트 DLG
	"$GTBATTLETABLE_8",					// 88 길드토너먼트 DLG
	"$GDTVIEWDLG",						// 89 길드토너먼트 DLG (배틀리스트)
	"$GTSCORETIMEDLG",					// 90 길드토너먼트 DLG
	"$SEIGEWARTIMEREGDLG",				// 91
	"$SEIGEWARPROTECTREGDLG",			// 92
	"$SEIGEWARINFODLG",					// 93
	"$SWSTARTTIMEDLG",					// 94
	"$ITEMLOCKDLG",						// 95
	"$SWPROFITDLG",						// 96
	"$ENGRAVEDLG",						// 97
	"$SWCOUNTDLG",						// 98
	"$CHANGEJOBDLG",					// 99
	"$RFDefaultDlg",					// 100
	"$RareCreateDlg",					// 101 Not used
	"$RFDATAGUIDEDLG",					// 102
	"$TIPBROWSERDLG",					// 103
	"$KSDLG",							// 104
	"$GUILDNOTEDLG",					// 105
	"$ALLYNOTEDLG",						// 106
	"$GUILDNOTICEDLG",					// 107
	"$GUILDPLUSTIMEDLG",				// 108
	"$SCREENSHOTDLG",					// 109
	"$CHATWINDOWDLG",					// 110
	"$CHARMAKEPROFILEDLG",				// 111
	"$CHARDEL",							// 112
	"$MENUDLG",							// 113
	"$JOINOPTIONDLG",					// 114
	"$SKILLDLG",						// 115
	"$NIDLG",							// 116
	"$DATEMATCHINGDLG",					// 117
	"$IDENTIFICATIONDLG",				// 118
	"$FAVORICONDLG",					// 119
	"$CHATTING",						// 120
	"$TRAININGDLG",						// 121
	"$FAMILYDLG",						// 122
	"$FAMILYCREATEDLG",					// 123
	"$FAMILYINVITEDlG",					// 124
	"$FAMILYMARKDLG",					// 125
	"$FAMILYNICKNAMEDLG",				// 126
	"$GUILDWAREHOUSERANKDLG",			// 127
	"$QUESTQUICKVIEWDLG",				// 128
	"$GUILDREVIVALDLG",					// 129
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10
	"$FARM_BUY_DLG",					// 130
	"$FARM_UPGRADE_DLG",				// 131
	// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10
	"$PROGRESSDLG",						// 132

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.23
	"$FARM_MANAGE_DLG",					// 133
	// E 농장시스템 추가 added by hseos 2007.10.23

	// 071016 LYW --- cScriptManager : Add Tutorial dialog.
	"$TUTORIALDLG",						// 134
	// 071016 LYW --- cScriptManager : Add Helper dialog.
	"$HELPERDLG",						// 135
	// 071023 LYW --- cScriptManager : Add Notice button dialog of tutorial.
	"$TUTORIALBTNDLG",					// 136
	"$ITEMMALLBTNDLG",					// 137
	"$ITEMPOPUPDLG",					// 138
	// 071130 LYW --- cScriptManager : Add map move dialog.
	"$MAPMOVEDLG",						// 139
	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
	"$DATEZONELISTDLG",					// 140
	"$CHALLENGEZONELISTDLG",			// 141
	"$CHALLENGEZONECLEARNO1DLG",		// 142
	// E 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24

	// 071227 LYW --- cScriptManager : 캐릭터 이름 변경 다이얼로그 추가.
	"$CHANGENAMEDLG",					// 143

	// 080109 LYW --- cScriptManager : 숫자 패드 다이얼로그 추가.
	"$NUMBERPAD_DLG",					// 144

//---KES AUTONOTE
	"$AUTONOTEDLG",						// 145
	"$AUTOANSWERDLG",					// 146
//---------------

	// 080218 LUJ
	"$APPLYOPTIONDLG",					// 147
	"$SHOUTDLG",						// 148

//---KES 상점검색
	"$STORESEARCHDLG",					// 149
//---------------

	// 080414 LUJ, 외양 변경 창
	"$BODYCHANGEDIALOG",				// 150

	// 080421 NYJ --- 낚시
	"$FISHINGDLG",						// 151
	"$FISHINGFROGRESSDIALOG",			// 152
	"$FISHINGPOINTDLG",					// 153

	"$WEATHERDLG",						// 154

	// 080403 LYW --- cScriptManager : Add dialogs part for chatroom system.
	"$CHATROOMMAINDLG",					// 155
	"$CHATROOMCREATEDLG",				// 156
	"$CHATROOMDLG",						// 157
	"$CHATROOMGUESTLISTDLG",			// 158
	"$CHATROOMOPTIONDLG",				// 159
	"$CHATROOMJOINDLG",					// 160
	//"$CHATROOMLOBBYDLG",
	"$FARMANIMALDLG",					// 161

	"$PETINFODLG",						// 162
	"$PETUIDLG",						// 163
	"$PETRESDLG",						// 164
	
	"$GTRESULTDLG",						// 165
	"$GTBATTLETABLE_16",				// 166
	"$GTENTRYEDITDLG",					// 167
	"$COMPOSEDLG",						// 168
	"$COOKDLG",							// 169
	// 080929 LYW --- cScriptManager : 공성 깃발 정보 다이얼로그 추가.
	"$SIEGEWARFALGDLG",					// 169

	// 080930 LYW --- cScriptManager : 페이드 인/아웃 다이얼로그 추가.
	"$FADEDLG",		

	"$PopupMenuDlg",
	// 090422 ShinJS --- 탈것을 위한 팝업메뉴 다이얼로그 추가
	"$RidePopupMenuDlg",
	// 090413 ONS 전직 가이드 다이얼로그 추가.
	"$GRADECLASS_TU_DLG",

	//090323 pdy 하우징 설치 UI추가
	"$HousingRevolDlg",

	//090330 pdy 하우징 창고 UI추가 
	"$HOUSING_WAREHOUSE_DLG",

	//090409 pdy 하우징 하우스 검색 UI추가
	"$HOUSE_SEARCHDLG",

	//090410 pdy 하우징 하우스 이름설정 UI추가 
	"$HOUSE_NAMEDLG",

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	"$HOUSING_DECOPOINTDLG",

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	"$HousingActionPopupMenuDlg",

	//090525 pdy 하우징 매인포인트 UI추가
	"$HousingMainPointDlg",

	//090525 pdy 하우징 내집창고 버튼 UI추가
	"$HousingWareHousetButtonDlg",

	//090622 NYJ 하우징 웹브라우저
	"$HousingWebDlg",

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	"$InputNameDlg",

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	"$HOUSING_DECOMODE_BT_DLG",

	// 090925 ShinJS --- NPC Notice Dialog
	"$NPCNOTICEDLG",
	// 091102 NYJ - 트리거 타이머
	"$TRIGGERTIMERDLG",
	"$LIMITDUNGEONDLG",

	// 091210 ShinJS --- Video Capture Dialog 추가
	"$VIDEO_CAPTURE_DLG",
	"$ADDITIONAL_BUTTON_DLG",
	"$CristalEmpireBonusDialog",
	"$RideOptionDialog",
	"$GAMERATINGDLG",
	"$CHANGECLASSDLG",
	"$MINNOTEDLGREAD",
	"$CONSIGNMENTGUIDEDLG",
	"$CONSIGNMENTDLG",
	"$WORLDMAPDLG",
	"$MONSTERTARGETDLG",
	"$BOSSTARGETDLG",
	"$BATTLEGUAGEDLG",
	"$CHARACTERGUAGEDLG",

#ifdef _TW_LOCAL_
	"$ITEMSHOPDLG",	//---BROWSR
#endif
};


// GetCtrlInfo()용.
char	g_szCtrlInfoTable[][50]	=	
{
	"$BTN",
	"$EDITBOX",
	"$STATIC",
	"$PUSHUPBTN",
	"$CHECKBOX",
	"$COMBOBOX",
	"$COMBOBOXEX",
	"$SPIN",
	"$LISTCTRL",
	"$LISTCTRLEX",
	"$ICONDLG",
	"$ICONGRIDDLG",
	"$LISTDLGEX",
	"$LISTDLG",
	"$WEAREDDLG",
	"$LIST",
	"$GUAGEBAR",
	"$ANI",
	"$GUAGE",
	"$TEXTAREA",
	"$GUAGEN",
	"$GUAGENE",
	"$DLG",
	"$CHARGAGE",
	"$PRIVATEWAREHOUSEDLG",
	"$QUESTDLG",
	"$WANTEDDLG",
	"$JOURNALDLG",
	"$ITEMSHOPGRIDDLG",
	"$SHOPITEMINVENGRID",
	"$JOBSKILLDLG",
	"$QUICKSLOTDIALOG",
	"$CHATTING",
	// 070605 LYW --- cScriptManager : Add dialog for matching part.
	"$MATCHPARTNERLISTDLG", 
	"$MATCHCHATDLG", 
	"$MATCHINFODLG", 
	"$MATCHRECORDDLG", 
	"$IDENTIFICATIONDLG", 
	"$FAVORICONDLG",
	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.10.23
	"$FARM_MANAGE_GARDEN_DLG",
	// E 농장시스템 추가 added by hseos 2007.10.23

	"$PETSTATEDLG",
	"$PETWEAREDDLG",
	"$PETINVENDLG",

	//pdy 090330 하우징 창고 UI추가 
	"$SCROLLICONGRIDDLG",

	// 090622 NYJ 하우징 웹브라우저
	"$BROWSER",	//---BROWSER
	"$HOUSINGWEBDLG",
	"$CHANNELCOMBOBOX",
};

// GetInfoFromFile()용.
char	g_szPropertyInfoTable[][50]	=	
{
	"#POSITION",
	"#SETWH",
	"#POINT",
	"#FUNC",
	"#ID",
	"#FONTIDX",
	"#AUTOCLOSE",
	"#TOOLTIPIMAGE",
	"#TOOLTIPMSG",
	"#TOOLTIPCOL",
	"#TEXTXY",
	"#MOVEABLE",
	"#ACTIVE",
	"#BASICIMAGE",
	"#OVERIMAGE",
	"#LISTOVERIMAGE",
	"#SELECTIMAGE",
	"#PRESSIMAGE",
	"#FOCUSIMAGE",
	"#BTNTEXT",
	"#BTNTEXTANI",
	"#IMAGESRCRECT",
	"#ALPHA",
	"#EDITSIZE",
	"#SPINSIZE",
	"#SECRET",
	"#TEXTCOLOR",
	"#COORD",
	"#FGCOLOR",
	"#CARETCOLOR",
	"#EXTENDCOLOR",
	"#TEXT",
	"#ITEMTOOLTIP",
	"#SHADOW",
	"#SHADOWCOLOR",
	"#SHADOWTEXTXY",
	"#MESSAGE",
	"#TEXTRECT",
	"#PUSHUP",
	"#VALIDXY",
	"#CAPTIONRECT",
	"#ICONCELLNUM",
	"#ICONCELL",
	"#SELECTOPTION",
	"#CELLRECT",
	"#LIMITLINENUM",
	"#LIMITBYTES",
	"#TEXTALIGN",
	"#AUTOSCROLL",
	"#PASSIVE",
	"#TABNUM",
	"#ICONCELLBGIMAGE",
	"#DRAGOVERBGIMAGE",
	"#COLS",
	"#ROWS",
	"#MIDDLENUM",
	"#TOPIMAGE",
	"#TOPHEIGHT",
	"#DOWNHEIGHT",
	"#MIDDELIMAGE",
	"#MIDDLEHEIGHT",
	"#DOWNIMAGE",
	"#COMBOTEXTCOLOR",
	"#SELECTCOMBOINDEX",
	"#READONLY",
	"#ADDSTRING",
	"#INITCOMBOLIST",
	"#CREATE",
	"#HEADIMAGE",
	"#BODYIMAGE",
	"#HEADHEIGHT",
	"#BODYHEIGHT",
	"#INSERTCOLUMN",
	"#SETITEMTEXT",
	"#SPINMINMAX",
	"#SPINUNIT",
	"#LINEUPWARD",
	"#LISTBARINTERVAL",
	"#LISTBARPOINT",
	"#LISTBARIMAGE",
	"#LISTMAXLINE",
	"#LINEHEIGHT",
	"#BTNCLICKSOUND",
	"#MINMAXMIDDLENUM",
	"#INITGRID",
	"#INITGUAGEBAR",
	"#MAXSPRITE",
	"#SPRITELOOP",
	"#TRAVELINGTYPE",
	"#ADDSPRITE",
	"#GUAGEPIECEIMAGE",
	"#GUAGEMINVALUE",
	"#GUAGEMAXVALUE",
	"#GUAGECURVALUE",
	"#GUAGENEVALUE",
	"#GUAGEPOS",
	"#CHECKBOXIMAGE",
	"#CHECKIMAGE",
	"#SETCHECK",
	"#SCALE",
	"#LISTSCALE",
	"#GUAGEWIDTH",
	"#GUAGEPIECEWIDTH",
	"#GUAGEEFFECTPIECEIMAGE",
	"#GUAGEEFFECTPIECEWIDTH",
	"#GUAGEPIECEHEIGHTSCALE",
	"#GUAGEEFFECTPIECEHEIGHTSCALE",
	"#BALLOONIMAGETOP",
	"#BALLOONIMAGEMIDDLE",
	"#BALLOONIMAGEBOTTOM",
	"#BALLOONIMAGETAIL",
	"#BALLOONIMAGEBORDER",
	"#TEXTALPHA",
	"#COMMANDNUM",
	"#TEXTOFFSET",
	"#SHOWSCROLL",
	"#DELTAPOS",
	"#COLUMNCOLOR",
	"#SELECTCOLUMCOLOR",
	"#TOOLTIPLINESIZE",
	"#ROWS_MSGID",
	"#TAB_STYLE",

	//090330 pdy ScrollIconGridDialog 추가
	"#VIEW_COLS",
	"#VIEW_ROWS",

	// 090820 ShinJS List Dialog 정렬 추가
	"#SORT_LIST",

	// 091125 ShinJS --- List Dialog, 클릭Item에 대하여 마우스 OverImage Render 여부 추가
	"#REMAIN_OVERIMAGE",

	// 091116 ONS UI위치 저장 속성 추가
	"#SAVE_POSITION",
	// 091207 ONS UI현재 활성화상태 속성 추가
	"#SAVE_ACTIVITY",

	// 091209 ShinJS --- List Dialog, TopListItem 변경시 Func 실행 여부 추가
	"#USE_TOPROW_CHANGE",
	"#OPEN_SOUND",
	"#CLOSE_SOUND",

	"#ADD_BTN_ANIIMG",
	"#ADD_BTN_OVER_ANIIMG",
	"#ADD_BTN_PRESS_ANIIMG",
	"#ACTIVE_BTN_ANIIMG",
	"#BTN_ANIIMG_FRAMETIME",
	"#BTN_OVER_ANIIMG_FRAMETIME",
	"#BTN_PRESS_ANIIMG_FRAMETIME",
	"#BTN_ANIIMG_FIRST_DRAW",

	"#COMPONENT_ID",

	"#BTN_MOUSE_EVT_USE_IMG_ALPHA",
	"#VALIDXY_BTN_ANIIMG",
	"#VALIDXY_BTN_OVER_ANIIMG",
	"#VALIDXY_BTN_PRESS_ANIIMG",

	"#IMAGE_RGBA"
};


GLOBALTON(cScriptManager);
cScriptManager::cScriptManager()
{
	InitTokenHash();
}

cScriptManager::~cScriptManager()
{
	ReleaseTokenHash();
}
char * cScriptManager::GetMsg( int idx )
{
	return RESRCMGR->GetMsg(idx);
}
void cScriptManager::GetImage( CMHFile * fp, cImage * pImage )
{
	ASSERT(pImage);
	char buff[255];
	int	idx = 0;
	cImageRect rect = {0,0,0,0};

	fp->GetString(buff);
	if(buff[0] != '(')		return ;
	idx						= fp->GetInt();
	rect.left				= fp->GetLong();
	rect.top				= fp->GetLong();
	rect.right				= fp->GetLong();
	rect.bottom				= fp->GetLong();
	fp->GetString(buff);
	if(buff[0] != ')')		return ;

	GetImage( idx, pImage, &rect );	
}

void cScriptManager::GetImage( int idx, cImage * pImage, cImageRect * rect )
{
	ASSERT(pImage);

	pImage->SetSpriteObject(RESRCMGR->GetImageInfo(idx));
	IMAGE_NODE* pNode = RESRCMGR->GetInfo(idx);
	if(pNode)
		pImage->SetImageSrcSize(&pNode->size);
	pImage->SetImageSrcRect(rect);
}

void cScriptManager::GetImage( int hard_idx, cImage * pImage , ePATH_FILE_TYPE type )
{
	CMHFile fp;

	char msg[255];
	switch( type )
	{
	case PFT_HARDPATH:
		fp.Init( FILE_IMAGE_HARD_PATH, "rb" );
		break;
	case PFT_ITEMPATH:
		fp.Init( FILE_IMAGE_ITEM_PATH, "rb" );
		break;
	case PFT_SKILLPATH:
		fp.Init( FILE_IMAGE_SKILL_PATH, "rb" );
		break;
	case PFT_BUFFPATH:
		fp.Init( FILE_IMAGE_BUFF_PATH, "rb");
		break;
	case PFT_MINIMAPPATH:
		fp.Init( FILE_IMAGE_MINIMAP_PATH, "rb");
		break;
	case PFT_CONDUCTPATH :
		fp.Init( FILE_IMAGE_CONDUCT_PACH, "rb" ) ;
		break ;
	case PFT_DATEMATCHINGPATH:
		fp.Init( FILE_IMAGE_DATEMATCHING_PATH, "rb");
		break;

		// 070607 LYW --- cScriptManager : Add case part for favor icon.
	case PFT_FAVOR_ICON_PATH :
		fp.Init( FILE_IMAGE_FAVOR_ICON_PATH, "rb" ) ;
		break ;

		// 070714 LYW --- cScriptManager : Add case part for class icon.
	case PFT_CLASS_ICON_PATH :
		fp.Init( FILE_IMAGE_CLASS_ICON_PATH, "rb" ) ;
		break ;

	}
	for( int i = 0 ; i < hard_idx ; ++i )
	{
		fp.GetLine(msg, 255);
	}
	fp.GetInt();
	
	int	idx = 0;
	cImageRect rect = {0,0,0,0};

	if( type == PFT_CLASS_ICON_PATH )
	{
		fp.GetLong();
	}

	idx						= fp.GetInt();
	rect.left				= fp.GetLong();
	rect.top				= fp.GetLong();
	rect.right				= fp.GetLong();
	rect.bottom				= fp.GetLong();
	
	if( rect.right - rect.left == 0 || rect.top - rect.bottom == 0 )
		return;

	GetImage( idx, pImage, &rect );
}


cWindow * cScriptManager::GetDlgInfoFromFile(char * filePath, char* mode)
{
	CMHFile fp;
	if(!fp.Init(filePath, mode))
		return NULL;
	char buff[256]={0,};
	cWindow * dlg = NULL;
	while(1)
	{
		fp.GetString(buff);
		if(fp.IsEOF())
			break;
		if(buff[0] == '@')
		{
			fp.GetLineX(buff, 256);
			continue;
		}

		size_t nDlgNum = UINT_MAX;

		for(size_t i = sizeof(g_szDlgInfoTable) / sizeof(*g_szDlgInfoTable);
			0 < i--;)
		{
			if(0 == _tcsicmp(g_szDlgInfoTable[i], buff))
			{
				nDlgNum = i;
				break;
			}
		}

		switch( nDlgNum)
		{
		case eDLG :					// $DLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cDialog, &fp);
			else
				__asm int 3;
			break;
		case eCHARGAGE :			// $CHARGAGE
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CGuageDialog, &fp);
			else
				__asm int 3;
			break;
		case eLISTDLG :				// $LISTDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cListDialog, &fp);
			else
				__asm int 3;
			break;
		case eLISTDLGEX :			// $LISTDLGEX
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cListDialogEx, &fp);
			else
				__asm int 3;
			break;
		case eCHARINFODLG :			// $CHARINFODLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CCharacterDialog, &fp);
			else
				__asm int 3;
			break;
		case eINVENTORYDLG :		// $INVENTORYDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CInventoryExDialog, &fp);
			else
				__asm int 3;
			break;
		case eSTALLOPTIONDLG :		// $STALLOPTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CStallKindSelectDlg, &fp);
			else
				__asm int 3;
			break;
		case eBUYSTALLDLG :			// $BUYSTALLDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CStreetBuyStall, &fp);
			else
				__asm int 3;
			break;
		case eBUYREGDLG :			// $BUYREGDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CBuyRegDialog, &fp);
			else
				__asm int 3;
			break;
		case eSTALLSELLDLG :		// $STALLSELLDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CStreetStall, &fp);
			else
				__asm int 3;
			break;
		case eNPCCHATDLG :			// $NPCCHATDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cNpcScriptDialog, &fp);
			else
				__asm int 3;
			break;
		case eCHARMAKEDLG :			// $CHARMAKEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CCharMakeNewDlg *)GetInfoFromFile(new CCharMakeNewDlg, &fp);
			else
				__asm int 3;
			break;
		case eMONEYDLG :			// $MONEYDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CMoneyDlg, &fp);
			else
				__asm int 3;
			break;
		case eNPCSHOPDLG :			// $NPCSHOPDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CDealDialog, &fp);
			else
				__asm int 3;	
			break;
		case eQUICKSLOTDLG :		// $QUICKSLOTDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cQuickDlg, &fp);
			else
				__asm int 3;	
			break;
		case ePA_MEMBER1DLG :		// $PA_MEMBER1DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePA_MEMBER2DLG :		// $PA_MEMBER2DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePA_MEMBER3DLG :		// $PA_MEMBER3DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePA_MEMBER4DLG :		// $PA_MEMBER4DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePA_MEMBER5DLG :		// $PA_MEMBER5DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePA_MEMBER6DLG :		// $PA_MEMBER6DLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyMemberDlg *)GetInfoFromFile(new CPartyMemberDlg, &fp);
			else
				__asm int 3;
			break;
		case ePARTYSETDLG :			// $PARTYSETDLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyBtnDlg *)GetInfoFromFile(new CPartyBtnDlg, &fp);
			else
				__asm int 3;			
			break;
		case ePA_CREATEDLG :		// $PA_CREATEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyCreateDlg *)GetInfoFromFile(new CPartyCreateDlg, &fp);
			else
				__asm int 3;			
			break;
		case ePA_INVITEDLG :		// $PA_INVITEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CPartyInviteDlg *)GetInfoFromFile(new CPartyInviteDlg, &fp);
			else
				__asm int 3;
			break;
		case eANI :					// $ANI
			if((fp.GetString())[0] == '{')
				dlg = (cAni *)GetInfoFromFile(new cAni, &fp);
			else
				__asm int 3;	
			break;
		case eMOUSEPOINTER :		// $MOUSEPOINTER
			if((fp.GetString())[0] == '{')
				dlg = (CMousePointer *)GetInfoFromFile(new CMousePointer, &fp);
			else
				__asm int 3;
			break;
		case eSTORAGEDLG :			// $STORAGEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CStorageDialog *)GetInfoFromFile(new CStorageDialog, &fp);
			else
				__asm int 3;
			break;
		case eFRIENDDLG :			// $FRIENDDLG
			if((fp.GetString())[0] == '{')
				dlg = (CFriendDialog *)GetInfoFromFile(new CFriendDialog, &fp);
			else
				__asm int 3;
			break;
		case eNOTEDLG :				// $NOTEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CNoteDialog *)GetInfoFromFile(new CNoteDialog, &fp);
			else
				__asm int 3;
			break;
		case eMINNOTEDLG :			// $MINNOTEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CMiniNoteDialogWrite *)GetInfoFromFile(new CMiniNoteDialogWrite, &fp);
			else
				__asm int 3;
			break;
		case eFRIENDINVITEDLG :		// $FRIENDINVITEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CMiniFriendDialog *)GetInfoFromFile(new CMiniFriendDialog, &fp);
			else
				__asm int 3;
			break;
		case eREVIVALDLG :			// $REVIVALDLG
			if((fp.GetString())[0] == '{')
				dlg = (CReviveDialog*)GetInfoFromFile(new CReviveDialog, &fp);
			else
				__asm int 3;			
			break;
		case eCHANNELDLG :			// $CHANNELDLG
			if((fp.GetString())[0] == '{')
				dlg = (CChannelDialog*)GetInfoFromFile(new CChannelDialog, &fp);
			else
				__asm int 3;			
			break;
		case eMAINMENUDLG :			// $MAINMENUDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CExitDialog, &fp);
			else
				__asm int 3;
			break;
		case eMONSTERMETERDLG :		// $MONSTERMETERDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHMonstermeterDlg, &fp);
			else
				__asm int 3;
			break;
		case eOPTIONDLG :			// $OPTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new COptionDialog, &fp);
			else
				__asm int 3;
			break;
		case eTRADEDLG :			// $TRADEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CExchangeDialog, &fp);
			else
				__asm int 3;
			break;
		case ePKLOOTINGDLG :		// $PKLOOTINGDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CPKLootingDialog, &fp);
			else
				__asm int 3;
			break;
		case eCNADVICEDLG :			// $CNADVICEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChinaAdviceDlg, &fp);
			else
				__asm int 3;
			break;
		case eDISSOLUTIONDLG :		// $DISSOLUTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CDissolveDialog, &fp);
			else
				__asm int 3;
			break;
		case eMINIMAPDLG :			// $MINIMAPDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CMiniMapDlg, &fp);
			else
				__asm int 3;
			break;
		case eBIGMAPDLG :			// $BIGMAPDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CBigMapDlg, &fp);
			else
				__asm int 3;
			break;
		case eTARGETDLG :			// $TARGETDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CMonsterGuageDlg, &fp);
			else
				__asm int 3;
			break;
		case eDIVIDEBOX :			// $DIVIDEBOX
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cDivideBox, &fp);
			else
				__asm int 3;
			break;
		case eMONEYDIVIDEBOX :			// $MONEYDIVIDEBOX
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cMoneyDivideBox, &fp);
			else
				__asm int 3;
			break;
		case eLOADINGDLG :			// $LOADINGDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CLoadingDlg, &fp);
			else
				__asm int 3;
			break;
		case eUPGRADEDLG :			// $UPGRADEDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CEnchantDialog, &fp);
			else
				__asm int 3;
			break;
		case eMIXDLG :				// $MIXDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CMixDialog, &fp);
			else
				__asm int 3;
			break;
			// 080916 LUJ, 아이템 합성 창, $COMPOSEDLG
		case eCOMPOSEDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CComposeDialog, &fp);
			else
				__asm int 3;
			break;
		case eREINFORCEDLG :		// $REINFORCEDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CReinforceDlg, &fp);
			else
				__asm int 3;
			break;
		// 080218 LUJ, 옵션 부여 창
		case eAPPLYOPTIONDLG :		// $APPLYOPTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CApplyOptionDialog, &fp);
			else
				__asm int 3;
			break;		
		case eSERVERSELECTDLG :		// $SERVERSELECTDLG
			if( (fp.GetString())[0] == '{' )
				dlg = (cDialog*)GetInfoFromFile( new CServerListDialog, &fp );
			else
				__asm int 3;
			break;

		case eQUESTDLG :			// $QUESTDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CQuestTotalDialog, &fp);
			else
				__asm int 3;
			break;
		case eGUILDCREATEDLG :		// $GUILDCREATEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildCreateDialog, &fp);
				else
					__asm int 3;	
				break;
		case eGUILDUNIONCREATEDLG :	// $GUILDUNIONCREATEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog*)GetInfoFromFile(new CGuildUnionCreateDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDDLG :			// $GUILDDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDINVITEDlG :		// $GUILDINVITEDlG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildInviteDialog, &fp);
				else
					__asm int 3;
			break;
		case eGUILDMARKDLG :		// $GUILDMARKDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildMarkDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDLEVELUPDLG :		// $GUILDLEVELUPDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildLevelUpDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDNICKNAMEDLG :	// $GUILDNICKNAMEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildNickNameDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDRANKDLG :		// $GUILDRANKDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildRankDialog, &fp);
				else
					__asm int 3;
				break;
		case eGUILDWAREHOUSEDLG :	// $GUILDWAREHOUSEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildWarehouseDialog, &fp);
				else
					__asm int 3;
				break;
		case eITEMMALLWAREHOUSE :	// $ITEMMALLWAREHOUSE
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CItemShopDialog, &fp);
				else
					__asm int 3;
				break;
		case eGFWARDECLAREDLG:		// $GFWARDECLAREDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGFWarDeclareDlg, &fp);
				else
					__asm int 3;
				break;
		case eGFWARRESULTDLG :		// $GFWARRESULTDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGFWarResultDlg, &fp);
				else
					__asm int 3;
				break;
		case eGFWARINFODLG :		// $GFWARINFODLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGFWarInfoDlg, &fp);
				else
					__asm int 3;
				break;
		case eGUILDWARINFODLG :		// $GUILDWARINFODLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGuildWarInfoDlg, &fp);
				else
					__asm int 3;
				break;
		case eCHALLWORLDCREATEDLG :	// $CHALLWORLDCREATEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CShoutDialog, &fp);
				else
					__asm int 3;
				break;
		case eGTBATTLETABLE_8:
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGTStandingDialog, &fp);
				else
					__asm int 3;
				break;
		case eGDTVIEWDLG :			// $GDTVIEWDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGTBattleListDialog, &fp);
				else
					__asm int 3;
				break;
		case eGTSCORETIMEDLG :	// $GTSCORETIMEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CGTScoreInfoDialog, &fp);
				else
					__asm int 3;			
				break;
		case eRFDATAGUIDEDLG :		// $RFDATAGUIDEDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CReinforceGuideDialog, &fp);
				else
					__asm int 3;
				break;
		case eKSDLG :				// $KSDLG
				if((fp.GetString())[0] == '{')
					dlg = (cDialog *)GetInfoFromFile(new CKeySettingTipDlg, &fp);
				else
					__asm int 3;
				break;
		case eGUILDNOTEDLG :		// $GUILDNOTEDLG
				if((fp.GetString())[0] == '{')
					dlg = (CGuildNoteDlg *)GetInfoFromFile(new CGuildNoteDlg, &fp);
				else
					__asm int 3;
				break;
		case eALLYNOTEDLG :			// $ALLYNOTEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CUnionNoteDlg *)GetInfoFromFile(new CUnionNoteDlg, &fp);
			else
				__asm int 3;
			break;
		case eGUILDNOTICEDLG :		// $GUILDNOTICEDLG
			if((fp.GetString())[0] == '{')
				dlg = (CGuildNoticeDlg *)GetInfoFromFile(new CGuildNoticeDlg, &fp);
			else
				__asm int 3;
			break;
		case eCHATWINDOWDLG:		// $CHATWINDOWDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CChattingDlg, &fp);
			else
				__asm int 3;	
			break;
		case eCHARMAKEPROFILEDLG :	//$CHARMAKEPROFILEDLG
				if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CProfileDlg, &fp);
			else
				__asm int 3;
				break;
		case eCHARDEL :				// $CHARDEL
				if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CCertificateDlg, &fp);
			else
				__asm int 3;
				break;
		case eMENUDLG :				// $MENUDLG
				if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CMainSystemDlg, &fp);
			else
				__asm int 3;
				break;
		case eJOINOPTIONDLG :		// $JOINOPTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog*)GetInfoFromFile(new CGuildJoinDialog, &fp);
			else
				__asm int 3;
			break;
		case eSKILLDLG :			// $SKILLDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog*)GetInfoFromFile(new cSkillTreeDlg, &fp);
			else
				__asm int 3;
			break;

		case eNIDLG :				// $NIDLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog*)GetInfoFromFile(new cNpcImageDlg, &fp);
			else
				__asm int 3;
			break;
		case eDATEMATCHINGDLG :		// $DATEMATCHINGDLG
			if((fp.GetString())[0] == '{')
			{
				CDateMatchingDlg*	pDateMatching	=	new CDateMatchingDlg;
				dlg = (cDialog*)GetInfoFromFile(pDateMatching, &fp);
			}
			else
				_asm int 3;
			break;
		case eIDENTIFICATIONDLG :	// $IDENTIFICATIONDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CIdentification, &fp) ;
			}
			else
				_asm int 3;
			break;
		case eFAVORICONDLG :		// $FAVORICONDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CFavorIcon, &fp) ;					
			}
			else
				_asm int 3;
			break;
		case eCHATTING :			// $CHATTING
			if((fp.GetString())[0] == '{')
			{					
			}
			else
				_asm int 3;
			break;
		case eTRAININGDLG :			// $TRAININGDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new cSkillTrainingDlg, &fp);
			}
			else
				_asm int 3;
			break;
		case eFAMILYDLG :			// $FAMILYDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CFamilyDialog, &fp);
			}
			else
				_asm int 3;
			break;
		case eFAMILYCREATEDLG :		// $FAMILYCREATEDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CFamilyCreateDialog, &fp);
			}
			else
				_asm int 3;
			break;
		case eFAMILYMARKDLG :		// $FAMILYMARKDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CFamilyMarkDialog, &fp);
			}
			else
				_asm int 3;
			break;
		case eFAMILYNICKNAMEDLG :	// $FAMILYNICKNAMEDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile(new CFamilyNickNameDialog, &fp);
			}
			else
				_asm int 3;
			break;
		case eGUILDWAREHOUSERANKDLG :	// $GUILDWAREHOUSERANKDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile( new CGuildWarehouseRankDialog, &fp);
			}
			else
				_asm int 3;
			break;

		case eQUESTQUICKVIEWDLG :	// "$QUESTQUICKVIEWDLG											// 퀘스트 알림이 다이얼로그를 위한 파트.
			if((fp.GetString())[0] == '{')															// 읽어들인 스트링이 { 라면,
			{
				dlg = (cDialog*)GetInfoFromFile((cWindow*)(new CQuestQuickViewDialog), &fp) ;		// 퀘스트 알림이 다이얼로그를 생성하여 윈도우를 초기화 한다.
			}
			else
				_asm int 3 ;
			break ;
		case eGUILDREVIVALDLG :		// "$GUILDREVIVALDLG
			if((fp.GetString())[0] == '{')
			{
				dlg = (cDialog*)GetInfoFromFile((cWindow*)(new CGuildRevivalDialog), &fp);
			}
			else
				_asm int 3 ;
			break ;
		case eFARM_BUY_DLG :		// $FARM_BUY_DLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHFarmBuyDlg, &fp);
			else
				__asm int 3;
			break;

		case eFARM_UPGRADE_DLG :	// $FARM_UPGRADE_DLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHFarmUpgradeDlg, &fp);
			else
				__asm int 3;
			break;
		case ePROGRESSDLG :			// $PROGRESSDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CProgressDialog, &fp);
			else
				__asm int 3;
			break;
		case eFARM_MANAGE_DLG :		// $FARM_MANAGE_DLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHFarmManageDlg, &fp);
			else
				__asm int 3;
			break;
		case eTUTORIALDLG :			// $TUTORIALDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cTutorialDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eHELPERDLG :			// $HELPERDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cHelperDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eTUTORIALBTNDLG :		//$TUTORIALBTNDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cTutorialBtnDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eITEMPOPUPDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cItemPopupDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eMAPMOVEDLG :			//$MAPMOVEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cMapMoveDialog, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eDATEZONELISTDLG :		//$DATEZONELISTDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHDateZoneListDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eCHALLENGEZONELISTDLG ://$CHALLENGEZONELISTDLG   2007.11.15
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHChallengeZoneListDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		// desc_hseos_데이트 존_01
		// S 데이트 존 추가 added by hseos 2008.01.24
		case eCHALLENGEZONECLEARNO1DLG ://$CHALLENGEZONECLEARNO1DLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSHChallengeZoneClearNo1Dlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		// E 데이트 존 추가 added by hseos 2008.01.24
		case eCHANGENAMEDLG :		// $CHANGENAMEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cChangeNameDialog, &fp) ;
			else
				__asm int 3 ;
			break ;
		case eNUMBERPAD_DLG :		// "$NUMBERPAD_DLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cNumberPadDialog, &fp) ;
			else
				__asm int 3 ;
			break ;

		//---KES AUTONOTE
		case eAUTONOTEDLG:		// "$AUTONOTEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CAutoNoteDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eAUTOANSWERDLG:		// "$AUTOANSWERDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CAutoAnswerDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		//-----------------


		case eSHOUTDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CShoutDialog, &fp) ;
			else
				__asm int 3 ;
			break ;
		//---KES 상점점검색
		case eSTORESEARCHDLG:		// "$STORESEARCHDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CStoreSearchDlg, &fp) ;
			else
				__asm int 3 ;
			break ;
		// 080414 LUJ, 외양 변경 창
		case eBODYCHANGEDLG:		// "$SBODYCHANGEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CBodyChangeDialog, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eFISHINGDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CFishingDialog, &fp) ;
			else
				__asm int 3 ;
			break;

		case eFISHINGGAUGEDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CFishingGaugeDialog, &fp) ;
			else
				__asm int 3 ;
			break;

		case eFISHINGPOINTDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CFishingPointDialog, &fp) ;
			else
				__asm int 3 ;
			break;

		// 080403 LYW --- cScriptManager : Add dialogs for chatroom system.
		case eCHATROOMMAINDLG :		// "$CHATROOMMAINDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomMainDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eCHATROOMCREATEDLG :	// "$CHATROOMCREATEDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomCreateDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eCHATROOMDLG :				// "$CHATROOMDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eCHATROOMGUESTLISTDLG :	// "$CHATROOMGUESTLISTDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomGuestListDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eCHATROOMOPTIONDLG :		// "$CHATROOMOPTIONDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomOptionDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		case eCHATROOMJOINDLG :			// "$CHATROOMJOINDLG
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CChatRoomJoinDlg, &fp) ;
			else
				__asm int 3 ;
			break ;

		//case eCHATROOMLOBBYDLG :			// "$CHATROOMLOBBYDLG
		//	if((fp.GetString())[0] == '{')
		//		dlg = GetInfoFromFile(new CChatRoomLobbyDlg, &fp) ;
		//	else
		//		__asm int 3 ;
		//	break ;	
		case eFARMANIMALDLG :
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CAnimalDialog, &fp);
			else
				__asm int 3;
			break;

		case ePET_INFO_DLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CPetInfoDialog, &fp);
			else
				__asm int 3;
			break;

		case ePET_UI_DLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new cDialog, &fp);
			else
				__asm int 3;
			break;

		case ePET_RES_DLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CPetResurrectionDialog, &fp);
			else
				__asm int 3;
			break;

		case eGTRESULTDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CGTResultDlg, &fp);
			else
				__asm int 3;
			break;

		case eGTBATTLETABLE_16:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CGTStandingDialog16, &fp);
			else
				__asm int 3;
			break;

		case eGTENTRYEDITDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CGTEntryEditDlg, &fp);
			else
				__asm int 3;
			break;

		case eCOOKDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CCookDlg, &fp);
			// 080929 LYW --- cScriptManager : 공성 깃발 정보 다이얼로그 추가.
			else
				__asm int 3;
			break;

		case eSIEGEWARFALGDLG :
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CSiegeWarFlagDlg, &fp);
			else
				__asm int 3;
			break;

			// 080930 LYW --- cScriptManager : 페이드 인/아웃 다이얼로그 추가.
		case eFADEDLG :
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CFadeDlg, &fp);
			else
				__asm int 3;
			break;
		case ePOPUPMENUDLG :
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CPopupMenuDlg, &fp);
			else
				__asm int 3;
			break;
		// 090422 ShinJS --- 탈것을 위한 팝업메뉴 다이얼로그 추가
		case eRIDEPOPUPMENUDLG :
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CRidePopupMenuDlg, &fp);
			else
				__asm int 3;
			break;
		// 090413 ONS 전직 가이드 다이얼로그 추가.
		case eGRADECLASS_TU_DLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CGradeClassDlg, &fp);
			else
				__asm int 3;
			break;

		//090323 pdy 하우징 설치 UI추가
		case eHOUSING_REVOLDLG:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CHousingRevolDlg, &fp);
			else
				__asm int 3;
			break;

		//090330 pdy 하우징 창고 UI추가
		case eHOUSING_WAREHOUSE_DLG :		// $HOUSING_WAREHOUSE_DLG
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingWarehouseDlg, &fp);
			else
				__asm int 3;
			break;

		//090409 pdy 하우징 하우스 검색 UI추가
		case eHOUSE_SEARCH_DLG :
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHouseSearchDlg, &fp);
			else
				__asm int 3;
			break;

		//090410 pdy 하우징 하우스 이름설정 UI추가
		case eHOUSE_NAME_DLG :
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHouseNameDlg, &fp);
			else
				__asm int 3;
			break;

		//090414 pdy 하우징 꾸미기포인트 보상 UI추가
		case eHOUSING_DECOPOINTDLG :
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingDecoPointDlg, &fp);
			else
				__asm int 3;
			break;

		//090507 pdy 하우징 액션 팝업매뉴 UI추가
		case eHOUSING_ACTIONPOPUPMENUDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingActionPopupMenuDlg, &fp);
			else
				__asm int 3;
			break;

		case eHOUSING_MAINPOINT_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingMainPointDlg, &fp);
			else
				__asm int 3;
			break;

		
		case eHOUSING_WAREHOUSEBT_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingWarehouseButtonDlg, &fp);
			else
				__asm int 3;
			break;

		case eHOUSINGWEBDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingWebDlg, &fp);
			else
				__asm int 3;
			break;

		// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
		case eInputNameDlg:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(new CInputNameDlg, &fp);
			else
				__asm int 3;
			break;

		case eHOUSING_DECOMODE_BT_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cHousingDecoModeBtDlg, &fp);
			else
				__asm int 3;
			break;
		
		case eNPC_NOTICE_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cNpcNoticeDlg, &fp);
			else
				__asm int 3;
			break;

		case eTRIGGER_TIMER_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CTriggerTimerDlg, &fp);
			else
				__asm int 3;
			break;

			// 091106 LUJ, 리미트 던젼 창 추가
		case eLIMIT_DUNGEON_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CLimitDungeonDialog, &fp);
			else
				__asm int 3;
			break;

			// 091210 ShinJS --- Video Capture Dialog 추가
		case eVIDEO_CAPTURE_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CVideoCaptureDlg, &fp);
			else
				__asm int 3;
			break;
			// 100111 ONS 부가적인 버튼 다이얼로그 추가
		case eADDITIONAL_BUTTON_DLG:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new cAdditionalButtonDlg, &fp);
			else
				__asm int 3;
			break;
		case eCristalEmpireBonusDialog:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(
					new CCristalEmpireBonusDialog,
					&fp);
			break;
		case eRideOptionDialog:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(
				new CRideOptionDlg,
				&fp);
			break;
		case eGameRatingDialog:
			if((fp.GetString())[0] == '{')
				dlg = GetInfoFromFile(
				new CGameRatingDialog,
				&fp);
			break;
		// 100511 ONS 전직 초기화 다이얼로그 추가
		case eChangeClassDialog:
			if((fp.GetString())[0] == '{')
				dlg = (cDialog *)GetInfoFromFile(new CChangeClassDlg, &fp);
			break;
		case eMINNOTEDLGREAD :
			if((fp.GetString())[0] == '{')
				dlg = (CMiniNoteDialogRead *)GetInfoFromFile(new CMiniNoteDialogRead, &fp);
			else
				__asm int 3;
			break;
		case eCONSIGNMENTGUIDEDLG:
			if((fp.GetString())[0] == '{')
				dlg = (CConsignmentDlg *)GetInfoFromFile(new CConsignmentGuideDlg, &fp);
			else
				__asm int 3;
			break;
		case eCONSIGNMENTDLG:
			if((fp.GetString())[0] == '{')
				dlg = (CConsignmentDlg *)GetInfoFromFile(new CConsignmentDlg, &fp);
			else
				__asm int 3;
			break;
		case eWORLDMAPDLG:
			if((fp.GetString())[0] == '{')
				dlg = (CWorldMapDlg *)GetInfoFromFile(new CWorldMapDlg, &fp);
			else
				__asm int 3;
			break;
		//Monster TargetDlg 추가
		case eMONSTERTARGETDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cMonsterTargetDlg*)GetInfoFromFile(new cMonsterTargetDlg, &fp);
			else
				__asm int 3;
			break;
		case eBOSSTARGETDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cMonsterTargetDlg*)GetInfoFromFile(new cBossTargetDlg, &fp);
			else
				__asm int 3;
			break;
		case eBATTLEGUAGEDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cBattleGuageDlg*)GetInfoFromFile(new cBattleGuageDlg, &fp);
			else
				__asm int 3;
			break;
		case eCHARACTERGUAGEDLG:
			if((fp.GetString())[0] == '{')
				dlg = (cCharacterTargetDlg*)GetInfoFromFile(new cCharacterTargetDlg, &fp);
			else
				__asm int 3;
			break;

		}
	}
	
	return dlg;
}

cWindow * cScriptManager::GetCtrlInfo(cWindow * wnd, char * buff, CMHFile * fp)
{
	if( 0 == strcmp( "$CHATTING", buff))
		_asm nop;
	ASSERT(wnd);
	cWindow*	window		=	NULL;
		DWORD	dwItem;
		DWORD dwSelect	=	VBHSelect( m_hCtrlInfoHash, &dwItem, 1, buff, strlen(buff));
		if( dwSelect)
		{
			switch( dwItem)
			{
			case 0:				// $BTN")
				window = GetInfoFromFile((cWindow*)(new cButton), fp);
				wnd->Add(window);
				break;
			case 1:				// $EDITBOX")
				window = GetInfoFromFile((cWindow*)(new cEditBox), fp);
				wnd->Add(window);
				break;
			case 2:				// $STATIC")
				window = GetInfoFromFile((cWindow*)(new cStatic), fp);
				wnd->Add(window);
				break;
			case 3:				// $PUSHUPBTN")
				window = GetInfoFromFile((cWindow*)(new cPushupButton), fp);
				wnd->Add(window);
				break;
			case 4:				// $CHECKBOX")
				window = GetInfoFromFile((cWindow*)(new cCheckBox), fp);
				wnd->Add(window);
				break;
			case 5:				// $COMBOBOX")
				window = GetInfoFromFile((cWindow*)(new cComboBox), fp);
				wnd->Add(window);
				break;
			case 6:				// $COMBOBOXEX")
				window = GetInfoFromFile((cWindow*)(new cComboBoxEx), fp);
				wnd->Add(window);
				break;
			case 7:				// $SPIN")
				window = GetInfoFromFile((cWindow*)(new cSpin), fp);
				wnd->Add(window);
				break;
			case 8:				// $LISTCTRL")
				window = GetInfoFromFile((cWindow*)(new cListCtrl), fp);
				wnd->Add(window);
				break;
			case 9:				// $LISTCTRLEX")
				window = GetInfoFromFile((cWindow*)(new cListCtrlEx), fp);
				wnd->Add(window);
				break;
			case 10:			// $ICONDLG")
				window = GetInfoFromFile((cWindow*)(new cIconDialog), fp);
				wnd->Add(window);
				break;
			case 11:			// $ICONGRIDDLG")
				window = GetInfoFromFile((cWindow*)(new cIconGridDialog), fp);
				wnd->Add(window);
				break;
			case 12:			// $LISTDLGEX")
				window = GetInfoFromFile((cWindow*)(new cListDialogEx), fp);
				wnd->Add(window);
				break;
			case 13:			// $LISTDLG")
				window = GetInfoFromFile((cWindow*)(new cListDialog), fp);
				wnd->Add(window);
				break;
			case 14:			// $WEAREDDLG")
				window = GetInfoFromFile((cWindow*)(new CWearedExDialog), fp);
				ASSERT(wnd);
				wnd->Add(window);
				break;
			case 15:			// $LIST")
				window = GetInfoFromFile((cWindow*)(new cList), fp);
				wnd->Add(window);
				break;
			case 16:			// $GUAGEBAR")
				window = GetInfoFromFile((cWindow*)(new cGuageBar), fp);
				wnd->Add(window);
				break;
			case 17:			// $ANI")
				window = GetInfoFromFile((cWindow*)(new cAni), fp);
				wnd->Add(window);
				break;
			case 18:			// $GUAGE")
				window = GetInfoFromFile((cWindow*)(new cGuage), fp);
				wnd->Add(window);
				break;
			case 19:			// $TEXTAREA")
				window = GetInfoFromFile((cWindow*)(new cTextArea), fp);
				wnd->Add(window);
				break;
			case 20:			// $GUAGEN")
				window = GetInfoFromFile((cWindow*)(new cGuagen), fp);
				wnd->Add(window);
				break;
			case 21:			// $GUAGENE")
				window = GetInfoFromFile((cWindow*)(new CObjectGuagen), fp);
				wnd->Add(window);
				break;
//KES 030821 추가
			case 22:			// $DLG")
				window = GetInfoFromFile((cWindow*)(new cDialog), fp);				// 293라인이랑 같은 코드 겹치는듯.? 잘하면 이 함수랑 윗 함수랑 합칠 수도 있을듯 한데....
				wnd->Add(window);
				break;
	//SW051112 무쌍모드
	// 070116 LYW --- Modified dialog's name in chargage.bin file.
	//CMD_CS("$CHARGUAGEDLG")
			case 23:			// $CHARGAGE")
				window = GetInfoFromFile((cWindow*)(new CGuageDialog), fp);
				wnd->Add(window);
				break;
//HS추가
			case 24:			// $PRIVATEWAREHOUSEDLG")
				window = GetInfoFromFile((cWindow*)(new CPrivateWarehouseDialog), fp);
				wnd->Add(window);
				break;
			case 25:			// $QUESTDLG")
				window = GetInfoFromFile((cWindow*)(new CQuestDialog), fp);
				wnd->Add(window);
				break;
			case 28:			// $ITEMSHOPGRIDDLG")
				window = GetInfoFromFile((cWindow*)(new CItemShopGridDialog), fp);
				wnd->Add(window); 
				break;
			case 30:			// $JOBSKILLDLG")
				window = GetInfoFromFile((cWindow*)(new cJobSkillDlg), fp);
				wnd->Add(window);
				break;
			case 31:			// $QUICKSLOTDIALOG")
				window = GetInfoFromFile((cWindow*)(new cQuickSlotDlg), fp);
				wnd->Add(window);
				break;
			// 070402 LYW --- cScriptManager : Add chatting window.
			case 32:			// $CHATTING")
			//	window = GetInfoFromFile((cWindow*)(new CChatting), fp);
			//	wnd->Add(window);
				break;

			// 070605 LYW --- cScriptManager : Add matching part.
			case 33 :			// $MATCHPARTNERLISTDLG
				window = GetInfoFromFile((cWindow*)(new CDateMatchingPartnerList), fp) ;
				wnd->Add(window) ;
				break ;

			case 34 :			// $MATCHCHATDLG
				window = GetInfoFromFile((cWindow*)(new CDateMatchingChatDlg), fp) ;
				wnd->Add(window) ;
				break ;

			case 35 :			// $MATCHINFODLG
				window = GetInfoFromFile((cWindow*)(new CDateMatchingInfoDlg), fp) ;
				wnd->Add(window) ;
				break ;

			case 36 :			// $MATCHRECORDDLG
				window = GetInfoFromFile((cWindow*)(new CDateMatchingRecordDlg), fp) ;
				wnd->Add(window) ;
				break ;

			/*case 37 :
				window = GetInfoFromFile((cWindow*)(new CIdentification), fp) ;
				wnd->Add(window) ;
				break ;

			case 38 :
				window = GetInfoFromFile((cWindow*)(new CFavorIcon), fp) ;
				wnd->Add(window) ;
				break ;*/

			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.10.23
			case 39:
				window = GetInfoFromFile((cWindow*)(new CSHFarmManageGardenDlg), fp) ;
				wnd->Add(window) ;
				break;
			// E 농장시스템 추가 added by hseos 2007.10.23

			case 40:
				window = GetInfoFromFile((cWindow*)(new CPetStateDialog), fp) ;
				wnd->Add(window) ;
				break;

			case 41:
				window = GetInfoFromFile((cWindow*)(new CPetWearedDialog), fp) ;
				wnd->Add(window) ;
				break;

			case 42:
				window = GetInfoFromFile((cWindow*)(new CPetInvenDialog), fp) ;
				wnd->Add(window) ;
				break;

			//090330 pdy ScrollIconGridDialog 추가
			case 43:
				window = GetInfoFromFile((cWindow*)(new cScrollIconGridDialog), fp) ;
				wnd->Add(window) ;
				break;

			case 44:	//$BROWSER
				window = GetInfoFromFile((cWindow*)(new cBrowser), fp) ;
				wnd->Add(window) ;
				break;

			case 45:		//$HOUSINGWEBDLG
				window = GetInfoFromFile((cWindow*)(new cHousingWebDlg), fp) ;
				wnd->Add(window) ;
				break;

			case 46:		//$CHANNELCOMBOBOX
				window = GetInfoFromFile((cWindow*)(new cChannelComboBox), fp) ;
				wnd->Add(window);
				break;

			default:
				_asm int 3;
				break;
				}
		}
		else 
			_asm int 3;			// 모르는 키워드.


	return wnd;
}

// 파서를 위한 해시 초기화.
void cScriptManager::InitTokenHash()
{
	// 080114 LYW --- cScriptManager : Dialog 해쉬 테이블을 사용하지 않도록 처리 함.
	//// GetDlgInfoFromFile()용 해시.
	//m_hDlgInfoHash	=	VBHCreate();
	//if( m_hDlgInfoHash == 0)
	//	_asm int 3;

	////GLOBAL_FUNC_DLL	BOOL			__stdcall	VBHInitialize(VBHASH_HANDLE pHash,DWORD dwMaxBucketNum,DWORD dwMaxSize,DWORD dwMaxItemNum);
	//DWORD	dwDlgInfoTableSize	=	sizeof( g_szDlgInfoTable);
	//DWORD	dwDlgInfoTableNum	=	dwDlgInfoTableSize / 50;				// 50은 char[50].
	//BOOL	bInit	=	VBHInitialize( m_hDlgInfoHash, dwDlgInfoTableNum, 50, dwDlgInfoTableNum + 4 );
	//if( bInit == FALSE)
	//	_asm int 3;
	//for( DWORD i = 0; i < dwDlgInfoTableNum; i++)
	//{
	//	void* pTemp	=	VBHInsert( m_hDlgInfoHash, i, g_szDlgInfoTable[i], strlen( g_szDlgInfoTable[i]) );
	//	if( pTemp == 0)
	//		_asm int 3;
	//}

	BOOL	bInit = FALSE ;

	// GetCtrlInfo()용 해시.
	m_hCtrlInfoHash	=	VBHCreate();
	if( m_hCtrlInfoHash == 0)
		_asm int 3;
	DWORD	dwCtrlInfoTableSize	=	sizeof( g_szCtrlInfoTable);
	DWORD	dwCtrlInfoTableNum	=	dwCtrlInfoTableSize / 50;				// 50은 char[50].
	bInit	=	VBHInitialize( m_hCtrlInfoHash, dwCtrlInfoTableNum+4, 50, dwCtrlInfoTableNum+4);
	if( bInit == FALSE)
		_asm int 3;
	for( DWORD i = 0; i < dwCtrlInfoTableNum; i++)
	{
		int	iLen	=	strlen( g_szCtrlInfoTable[i]);
		void* pTemp	=	VBHInsert( m_hCtrlInfoHash, i, g_szCtrlInfoTable[i], iLen);
		if( pTemp == 0)
			_asm int 3;
	}

	// GetInfoFromFile()용 해시.
	m_hPropertyInfoHash		=	VBHCreate();
	if( m_hPropertyInfoHash == 0)
		_asm int 3;
	DWORD	dwPropertyInfoTableSize	=	sizeof( g_szPropertyInfoTable);
	DWORD	dwPropertyInfoTableNum	=	dwPropertyInfoTableSize / 50;				// 50은 char[50].
	bInit	=	VBHInitialize( m_hPropertyInfoHash, dwPropertyInfoTableNum+4, 50, dwPropertyInfoTableNum+4);
	if( bInit == FALSE)
		_asm int 3;
	for( DWORD i = 0; i < dwPropertyInfoTableNum; i++)
	{
		int	iLen	=	strlen( g_szPropertyInfoTable[i]);
		void* pTemp	=	VBHInsert( m_hPropertyInfoHash, i, g_szPropertyInfoTable[i], iLen);
		if( pTemp == 0)
			_asm int 3;
	}

	_asm nop;
}

// 파서를 위한 해시 말기화(?).
void cScriptManager::ReleaseTokenHash()
{
	if( m_hPropertyInfoHash)
	{
		VBHDeleteAll( m_hPropertyInfoHash);
		VBHRelease( m_hPropertyInfoHash);
	}
	else
		_asm int 3;

	if( m_hCtrlInfoHash)
	{
		VBHDeleteAll( m_hCtrlInfoHash);
		VBHRelease( m_hCtrlInfoHash);
	}
	else
		_asm int 3;

	// 080114 LYW --- cScriptManager : Dialog 해쉬 테이블을 사용하지 않도록 처리 함.
	/*if( m_hDlgInfoHash)
	{
		VBHDeleteAll( m_hDlgInfoHash);
		VBHRelease( m_hDlgInfoHash);
	}
	else
		_asm int 3;*/

	_asm nop;
}

cWindow * cScriptManager::GetInfoFromFile(cWindow * wnd, CMHFile * fp)
{
	cPoint point={0,0,0,0};
	// 070202 LYW --- Add for position of dialog.
	VECTOR2 point1={0,0};
	VECTOR2 point2={0,0};
	VECTOR2 point3={0,0};

	cPoint listBarPoint={0,0,0,0};
	char toolTipMsg[256]={0,};
	char buff[256]={0,};
	char funcName[256]={0,};
	char IDName[256]={0,};
	char staticText[512]={0,};
	RGBCOLOR color={0,0,0};
	BOOL fMovable = TRUE;
	BOOL fActive = TRUE;
	BOOL fPush = FALSE;
	cImage BasicImage;
	cImage OverImage;
	cImage ListOverImage;
	cImage PressImage	;
	cImage ToolTipImage	;
	cImage TopImage		;
	cImage MiddleImage	;
	cImage DownImage	;
	cImage HeadImage	;
	cImage BodyImage	;
	cImage ListBarImage	;
	cImage FocusImage	;
	cImage SelectedImage;

	cImage AniSpriteImage	;
	cImage GuagePieceImage	;
	cImage CheckImage		;
	cImage CheckBoxImage	;
	cImage selectedBGImage	;
	cImage dragoverBGImage	;
	cImage guageEffectPieceImage	;

	cImage BalloonImage[10];
	FLOAT  BalloonBorder = 8;

	// 070328 LYW --- cScriptManager : Add text alpha.
	BOOL   bTextAlpha = FALSE ;
	// 070403 LYW --- cScriptManager : Add command numbers for chatting.
	int	   nCommandNum1	 = 0 ;
	int	   nCommandNum2	 = 0 ;
	int	   nCommandNum3	 = 0 ;
	int	   toolTipLineSize	= 0;

	BOOL	bChecked = FALSE;
	WORD	HeadHei = 0;
	WORD	BodyHei = 0;
	int		nLeftMargin = 0;
	int		nTopMargin = 0;

	WORD EditBoxSize = 0;
	WORD EditBoxEableTypingSize = 0;
	BOOL fSecret = FALSE;
	RGBCOLOR activeTextColor={0,0,0};
	RGBCOLOR nonactiveTextColor={0,0,0};
	RGBCOLOR fgColor={0,0,0};
	RGBCOLOR comboTextColor={0,0,0};
	char szBtnText[256]={0,};
	RGBCOLOR btnTextBasicCol={0,0,0};
	RGBCOLOR btnTextOverCol={0,0,0};
	RGBCOLOR btnTextPressCol={0,0,0};
	BYTE alphaDepth = 255;
	cCoord coord;
	RECT captionRect={0,0,0,0};
	RECT textRect={0,0,0,0};
	RECT imgSrcRect={0,0,0,0};
	BYTE cols=0;
	BYTE rows=0;
	BYTE tabNum=0;
	BYTE middleNum=0;
	WORD topHei = 0;
	WORD middleHei = 0;
	WORD downHei = 0;
	LONG minValue = 0;
	LONG maxValue = 100;
	LONG spinUnit = 10;
	BOOL lineUpward = TRUE;
	int	 interval = 0;
	WORD maxLine = 0;
	LONG listMaxHeight = 15;
	BYTE minMiddleNum=0;
	BYTE maxMiddleNum=0;
	WORD gridBorderX = 0;
	WORD gridBorderY = 0;
	cPoint gridPos ={0,0,0,0};
	VECTOR2 validXY={0,0};
	BOOL vertical = FALSE;
	BOOL fSpriteloop = TRUE;
	WORD travelingType = 0;

	float guageMinVal=0;
	float guageMaxVal=0;
	float guageCurVal=0;
	cCoord guagePos = {0,0};
	WORD selectComboIdx = 0;

	BOOL bAutoClose = FALSE;
	cCoord textXY = {5,6};
	WORD	fontIdx = 0;
	BOOL bReadOnly = FALSE;
	LONG limitLineNum = 20;
	int	soundIdx = -1;
	WORD	wOption = 0;
//KES 030916
	int	nLimitBytes = 256;
	int nTextAlign = TXT_LEFT | TXT_TOP;
	BYTE btnTextAniX = 1;
	BYTE btnTextAniY = 1;

	BOOL bAutoScroll = TRUE;
	BOOL bPassive = FALSE;

	int tt_idx = -1;
//
	cImageScale scale={1,1};
	cImageScale ListScale={1,1};

	float guageWidth = 0.f;
	float guagePieceWidth = 0.f;
	float guageEffectPieceWidth = 0.f;
	float guagePieceHeightScaleY = 1.f;
	float guageEffectPieceHeightScaleY = 1.f;
	DWORD	guageEstTime = 0;

	LONG		shadowTextX=0;
	LONG		shadowTextY=0;
	RGBCOLOR	dwShadowColor={10,10,10};
	BOOL		bShadow = FALSE;

	// 061227 LYW --- Add variable for color of caret.
	RGBCOLOR caretColor={0,0,0};	

	// 070412 LYW --- Add ExtendColor to cStatic.
	BOOL	bHasExtendColor = FALSE ;
	BYTE	byExtendColorCount = 0 ;
	DWORD	dwExtendColor[MAX_EXTENDCOLOR] = { 0, 0, 0, 0, 0 } ;


	// 070409 LYW --- Add variables for text offset of edit box.
	LONG lLeft = 0 ;
	LONG lRight = 0 ;
	LONG lTop = 0 ;

	// 070410 LYW --- Set show scroll or not.
	BOOL bShowScroll = FALSE ;

	// 070620 LYW --- cScriptManager : Add columncolor for lsit dialog.
	RGBCOLOR List_ColumnColor={255, 255,0};
	RGBCOLOR List_SelectColumnColor={0,255, 255};

	//090330 pdy ScrollIconGridDialog 추가
	BYTE view_cols=0;
	BYTE view_rows=0;

	// 090820 ShinJS --- List Dialog 정렬 추가
	BYTE bySortType = 0;

	// 091125 ShinJS --- List Dialog, 클릭Item에 대하여 마우스 OverImage Render 여부 추가
	BOOL bRemainOverImage = FALSE;

	// 091116 ONS 위치저장 속성 추가
	BOOL bSavePosition = FALSE;
	// 091207 ONS UI현재활성화 속성 추가
	BOOL bSaveActivity = FALSE;

	// 091209 ShinJS --- List Dialog, TopListItem 변경시 Func 실행 여부 추가
	BOOL bUseTopRowChangeEvent = FALSE;

	std::vector< std::pair< DWORD, cImageRect > > btnAniImgList;
	std::vector< std::pair< DWORD, cImageRect > > btnOverAniImgList;
	std::vector< std::pair< DWORD, cImageRect > > btnPressAniImgList;
	BOOL bActiveBtnAniImg = FALSE;
	BOOL bFirstDrawBtnAnimateImage = FALSE;
	DWORD dwBtnAniImgFrameTime = 0;
	DWORD dwBtnOverAniImgFrameTime = 0;
	DWORD dwBtnPressAniImgFrameTime = 0;
	cButton::eBtn_Mouse_Evt btnMouseEvt = cButton::eBtn_Mouse_Evt_None;
	VECTOR2 validXYBtnAniImg={0,0};
	VECTOR2 validXYBtnOverAniImg={0,0};
	VECTOR2 validXYBtnPressAniImg={0,0};

	// ID 설정기능.
	LONG uiID = 0;
	LONG uiComponentID = 0;
	
	while(1)
	{
		fp->GetString(buff);
		if(buff[0] == '}'|| fp->IsEOF())
			break;
		if(buff[0] == '{')
			continue;
		if(buff[0] == '$')	
		{
			GetCtrlInfo(wnd, buff, fp);
			continue;
		}
		if(buff[0] == '@')
		{
			fp->GetLineX(buff, 256);
			continue;
		}
		// 토큰 문자열로 인덱스를 받아온다.
		DWORD	dwItem;
		DWORD dwSelect	=	VBHSelect( m_hPropertyInfoHash, &dwItem, 1, buff, strlen(buff));
		if( dwSelect)
		{
			switch( dwItem)
			{
				case 0:		// "#POSITION")
				{
					point1.x = fp->GetFloat();
					point1.y = fp->GetFloat();

					point2.x = fp->GetFloat();
					point2.y = fp->GetFloat();

					point3.x = fp->GetFloat();
					point3.y = fp->GetFloat();
					wnd->SaveDispPoint( point1, point2, point3 ) ;
					break;
				}
				case 1:		// "#SETWH")
				{
					WORD width  = fp->GetWord() ;
					WORD height = fp->GetWord() ;
					wnd->SetWH( width, height ) ;
					//wnd->SetWH(fp->GetDword(), fp->GetDword()) ;
					break;
				}
				case 2:		// "#POINT")
				{
					point.x = fp->GetDword();
					point.y = fp->GetDword();
					point.w = fp->GetDword();
					point.h = fp->GetDword();
					wnd->SetAbsXY(point.x, point.y);
					wnd->SetRelXY(point.x, point.y);
					break;
				}
				case 3:		// "#FUNC")
					fp->GetString(funcName);
					break;
				case 4:		// "#ID")
					fp->GetString(IDName);
					break;
				case 5:		// "#FONTIDX")
					fontIdx = fp->GetWord();
					break;
				case 6:		// "#AUTOCLOSE")
					bAutoClose = fp->GetBool();
					break;
				case 7:		// "#TOOLTIPIMAGE")
					GetImage(fp, &ToolTipImage);
					break;
				case 8:		// "#TOOLTIPMSG")
					strcpy( toolTipMsg, GetMsg(fp->GetInt()));
					break;
				case 9:		// "#TOOLTIPCOL")
					color.r = fp->GetByte();
					color.g = fp->GetByte();
					color.b = fp->GetByte();
					break;
				case 10:	// "#TEXTXY")
					textXY.x = fp->GetDword();
					textXY.y = fp->GetDword();
					break;
				case 11:	// "#MOVEABLE")
					fMovable = fp->GetBool();
					break;
				case 12:	// "#ACTIVE")
					fActive = fp->GetBool();
					break;
				case 13:	// "#BASICIMAGE")
					GetImage(fp, &BasicImage);
					break;
				case 14:	// "#OVERIMAGE")
					GetImage(fp, &OverImage);
					break;
				case 15:	// "#LISTOVERIMAGE")
					GetImage(fp, &ListOverImage);
					break;
				case 16:	// "#SELECTIMAGE")
					GetImage(fp, &SelectedImage);
					break;
				case 17:	// "#PRESSIMAGE")
					GetImage(fp, &PressImage);
					break;
				case 18:	// "#FOCUSIMAGE")
					GetImage(fp, &FocusImage);
					break;
				case 19:	// "#BTNTEXT")
					strcpy( szBtnText, GetMsg(fp->GetInt()) );
					btnTextBasicCol.r = fp->GetByte();
					btnTextBasicCol.g = fp->GetByte();
					btnTextBasicCol.b = fp->GetByte();

					btnTextOverCol.r = fp->GetByte();
					btnTextOverCol.g = fp->GetByte();
					btnTextOverCol.b = fp->GetByte();

					btnTextPressCol.r = fp->GetByte();
					btnTextPressCol.g = fp->GetByte();
					btnTextPressCol.b = fp->GetByte();
					break;
				case 20:	// "#BTNTEXTANI")
					btnTextAniX	= fp->GetByte();
					btnTextAniY	= fp->GetByte();
					break;
				case 21:	// "#IMAGESRCRECT")
					imgSrcRect.left =  fp->GetLong();
					imgSrcRect.top = fp->GetLong();
					imgSrcRect.right = fp->GetLong();
					imgSrcRect.bottom = fp->GetLong();
					break;
				case 22:	// "#ALPHA")
					alphaDepth = fp->GetByte();
					// desc_hseos_몬스터미터01
					// S 몬스터미터 추가 added by hseos 2007.05.30
					wnd->SetUseOwnAlpha(TRUE);
					// E 몬스터미터 추가 added by hseos 2007.05.30
					break;
				case 23:	// "#EDITSIZE");
					EditBoxSize = fp->GetWord();
					EditBoxEableTypingSize = fp->GetWord();
					break;
				case 24:	// "#SPINSIZE");
					EditBoxSize = fp->GetWord();
					EditBoxEableTypingSize = fp->GetWord();
					break;
				case 25:	// "#SECRET")
					fSecret = fp->GetBool();
					break;
				case 26:	// "#TEXTCOLOR")
					activeTextColor.r = fp->GetByte();
					activeTextColor.g = fp->GetByte();
					activeTextColor.b = fp->GetByte();
					nonactiveTextColor.r = fp->GetByte();
					nonactiveTextColor.g = fp->GetByte();
					nonactiveTextColor.b = fp->GetByte();
					break;
				case 27:	// "#COORD")
					coord.x = fp->GetDword();
					coord.y = fp->GetDword();
					break;
				case 28:	// "#FGCOLOR")
					fgColor.r = fp->GetByte();
					fgColor.g = fp->GetByte();
					fgColor.b = fp->GetByte();
					break;
				// 061227 LYW --- Add color of caret part.
				case 29:	// "#CARETCOLOR")
					caretColor.r = fp->GetByte() ;
					caretColor.g = fp->GetByte() ;
					caretColor.b = fp->GetByte() ;
					break;
			// 070412 LYW --- cScriptManager : Add ExtendColor to cStatic.
				case 30:	// "#EXTENDCOLOR")
					byExtendColorCount = fp->GetByte() ;

					if( byExtendColorCount > MAX_EXTENDCOLOR )
					{
						char buff[256];
						sprintf(buff, "#EXTENDCOLOR : ColorCount is over 5!!");
						LOGEX(buff, PT_MESSAGEBOX);
					}
					else
					{
						bHasExtendColor = TRUE ;

						for( BYTE count = 0 ; count < byExtendColorCount ; ++count )
						{
							int nRed   = fp->GetInt() ;
							int nGreen = fp->GetInt() ;
							int nBlue  = fp->GetInt() ;

							dwExtendColor[count] = RGB_HALF( nRed, nGreen, nBlue ) ;
						}
					}
					break;
				case 31:	// "#TEXT")
					{
						SafeStrCpy(
							staticText,
							GetMsg(fp->GetInt()),
							sizeof(staticText) / sizeof(*staticText));
						break;
					}
				case 32:	// "#ITEMTOOLTIP")
					tt_idx = fp->GetInt();
					break;
				case 33:	// "#SHADOW")
					bShadow = fp->GetBool();
					break;
				case 34:	// "#SHADOWCOLOR")
					dwShadowColor.r = fp->GetByte();
					dwShadowColor.g = fp->GetByte();
					dwShadowColor.b = fp->GetByte();
					break;
				case 35:	// "#SHADOWTEXTXY")
					shadowTextX = fp->GetLong();
					shadowTextY = fp->GetLong();
					break;
				case 36:	// "#MESSAGE")
					{
						int msg_idx =  fp->GetInt();
						strcpy( staticText, GetMsg(msg_idx) );
						fgColor.r = fp->GetByte();
						fgColor.g = fp->GetByte();
						fgColor.b = fp->GetByte();
						break;
					}
				case 37:	// "#TEXTRECT")
					textRect.left =  fp->GetLong();
					textRect.top = fp->GetLong();
					textRect.right = fp->GetLong();
					textRect.bottom = fp->GetLong();
					break;
				case 38:	// "#PUSHUP")
					fPush = fp->GetBool();
					break;
				case 39:	// "#VALIDXY")
					validXY.x = fp->GetFloat();
					validXY.y = fp->GetFloat();
					break;
				case 40:	// "#CAPTIONRECT")
					captionRect.left = fp->GetLong();
					captionRect.top = fp->GetLong();
					captionRect.right = fp->GetLong();
					captionRect.bottom = fp->GetLong();
					break;
				case 41:	// "#ICONCELLNUM")
					{
						wnd->SetCellNum(fp->GetWord());
						break;
					}
				case 42:	// "#ICONCELL")
					{
						RECT iconCellRect={0,0,0,0};
						iconCellRect.left = fp->GetLong();
						iconCellRect.top = fp->GetLong();
						iconCellRect.right = fp->GetLong();
						iconCellRect.bottom = fp->GetLong();
						wnd->AddIconCell(&iconCellRect);
						break;
					}
				case 43:	// "#SELECTOPTION")
					wOption = fp->GetWord();
					break;
				case 44:	// "#CELLRECT")
					{
						RECT CellRect={0,0,0,0};
						CellRect.left = fp->GetLong();
						CellRect.top = fp->GetLong();
						CellRect.right = fp->GetLong();
						CellRect.bottom = fp->GetLong();
						wnd->SetCellRect(&CellRect);
						break;
					}
				case 45:	// "#LIMITLINENUM")
					limitLineNum = fp->GetLong();
					break;
	//KES 030916
				case 46:	// "#LIMITBYTES")
					nLimitBytes = fp->GetInt();
					break;
				case 47:	// "#TEXTALIGN")
					nTextAlign = fp->GetInt();
					if( nTextAlign == 0 )	nTextAlign = TXT_LEFT;
					else if( nTextAlign == 1 )	nTextAlign = TXT_RIGHT;
					else if( nTextAlign == 2 )	nTextAlign = TXT_CENTER;
					break;
				case 48:	// "#AUTOSCROLL")
					bAutoScroll = fp->GetByte();
					break;
				case 49:	// "#PASSIVE")
					bPassive = fp->GetByte();
					break;
				case 50:	// "#TABNUM")
					{
						tabNum = fp->GetByte();
						wnd->InitTab(tabNum);
						break;
					}
				case 51:	// "#ICONCELLBGIMAGE")
					GetImage(fp, &selectedBGImage);
					break;
				case 52:	// "#DRAGOVERBGIMAGE")
					GetImage(fp, &dragoverBGImage);
					break;
				case 53:	// "#COLS")
					cols = fp->GetByte();
					break;
				case 54:	// "#ROWS")
					rows = fp->GetByte();
					break;
				case 55:	// "#MIDDLENUM")
					middleNum = fp->GetByte();
					break;
				case 56:	// "#TOPIMAGE")
					GetImage(fp, &TopImage);
					topHei = fp->GetWord();
					break;
				case 57:	// "#TOPHEIGHT")
					topHei = fp->GetWord();
					break;
				case 58:	// "#DOWNHEIGHT")
					downHei = fp->GetWord();
					break;
				case 59:	// "#MIDDELIMAGE")		// 이거 철자 바꿔야 되염 ^^;;		
					GetImage(fp, &MiddleImage);
					middleHei = fp->GetWord();
					break;
				case 60:	// "#MIDDLEHEIGHT")
					middleHei = fp->GetWord();
					break;
				case 61:	// "#DOWNIMAGE")
					GetImage(fp, &DownImage);
					downHei = fp->GetWord();
					break;
				case 62:	// "#COMBOTEXTCOLOR")
					comboTextColor.r = fp->GetByte();
					comboTextColor.g = fp->GetByte();
					comboTextColor.b = fp->GetByte();
					break;
				case 63:	// "#SELECTCOMBOINDEX")
					selectComboIdx = fp->GetWord();
					break;
				case 64:	// "#READONLY")
					bReadOnly = fp->GetBool();
					break;
				case 65:	// "#ADDSTRING")
					{
						int msg_idx =  fp->GetInt();
						char msg2[255]={0,};
						strcpy( msg2, GetMsg(msg_idx) );
						RGBCOLOR comboListColor={0,0,0};
						comboListColor.r = fp->GetByte();
						comboListColor.g = fp->GetByte();
						comboListColor.b = fp->GetByte();

						switch(wnd->GetType())
						{
						case WT_LIST:
							{
								cList * win = (cList *)wnd;
								ITEM* pItem = new ITEM;
								strcpy(pItem->string, msg2);
								pItem->rgb =  RGB_HALF(comboListColor.r, comboListColor.g, comboListColor.b);
								win->AddItem(pItem);
							}
							break;
						case WT_LISTDIALOG:
							{
								cListDialog * win = (cListDialog *)wnd;
								win->AddItem(msg2, RGB_HALF(comboListColor.r, comboListColor.g, comboListColor.b));
							}
							break;
						case WT_COMBOBOX:
						case WT_CHANNELCOMBOBOX:
							{
								cComboBox * win = (cComboBox *)wnd;
								ITEM* pItem = new ITEM;
								strcpy(pItem->string, msg2);
								pItem->rgb =  RGB_HALF(comboListColor.r, comboListColor.g, comboListColor.b);
								win->AddItem(pItem);
							}
							break;
						}
						break;
					}
				case 66:	// "#INITCOMBOLIST")
					{
						if(WT_COMBOBOX != wnd->GetType() && WT_CHANNELCOMBOBOX != wnd->GetType() )
						{
							break;
						}

						cComboBox * win = (cComboBox *)wnd;
						WORD listWidth = fp->GetWord();				// 넓이
						win->SetMaxLine(fp->GetWord());				// 라인 갯수
						GetImage(fp, &TopImage);					// 상단 백그라운드 이미지
						topHei = fp->GetWord();						// 상단 이미지 높이값.
						GetImage(fp, &MiddleImage);					// 중간 백그라운드 이미지.
						middleHei = fp->GetWord();					// 중간 높이값(?)
						GetImage(fp, &DownImage);					// 하단
						downHei = fp->GetWord();					// 하단 높이.
						GetImage(fp, &ListOverImage);				// 커서 올렸을때 색 변하는 이미지
						win->InitComboList(listWidth, &TopImage, topHei, &MiddleImage, middleHei, &DownImage, downHei, &ListOverImage);
						break;
					}
				// LISTCTRL에서 사용하는 속성
				case 67:	// "#CREATE")								// 최대 컬럼 갯수(?)
					{
						if(WT_LISTCTRL != wnd->GetType())
						{
							break;
						}

						cListCtrl * win = (cListCtrl *)wnd;
						win->Create(fp->GetInt());
						break;
					}
				case 68:	// "#HEADIMAGE")
					GetImage(fp, &HeadImage);
					break;
				case 69:	// "#BODYIMAGE")
					GetImage(fp, &BodyImage);
					break;
				case 70:	// "#HEADHEIGHT")
					HeadHei = fp->GetWord();
					break;
				case 71:	// "#BODYHEIGHT")
					BodyHei = fp->GetWord();
					break;
				case 72:	// "#INSERTCOLUMN")
					{
						if(WT_LISTCTRL != wnd->GetType())
						{
							break;
						}

						cListCtrl * win = (cListCtrl *)wnd;
						int col_idx =  fp->GetInt();
						int msg_idx =  fp->GetInt();
						char msg2[255]={0,};
						strcpy( msg2, GetMsg(msg_idx) );
						int colSize = fp->GetInt();
						win->InsertColoumn(col_idx, msg2, colSize);
						break;
					}
				case 73:	// "#SETITEMTEXT")
					{
						if(WT_LISTCTRL != wnd->GetType())
						{
							break;
						}

						cListCtrl * win = (cListCtrl *)wnd;
						int col_idx =  fp->GetInt();
						int row_idx =  fp->GetInt();
						int msg_idx =  fp->GetInt();
						char msg2[255]={0,};
						strcpy( msg2, GetMsg(msg_idx) );
						win->SetItemText(col_idx, row_idx, msg2);
						break;
					}
				case 74:	// "#SPINMINMAX")
					minValue = fp->GetLong();
					maxValue = fp->GetLong();
					break;
				case 75:	// "#SPINUNIT")
					spinUnit = fp->GetLong();
					break;
				case 76:	// "#LINEUPWARD")
					lineUpward = fp->GetBool();
					break;
				case 77:	// "#LISTBARINTERVAL")
					interval = fp->GetInt();
					break;
				case 78:	// "#LISTBARPOINT")
					listBarPoint.x = fp->GetDword();
					listBarPoint.y = fp->GetDword();
					listBarPoint.w = fp->GetDword();
					listBarPoint.h = fp->GetDword();
					break;
				case 79:	// "#LISTBARIMAGE")
					GetImage(fp, &ListBarImage);
					break;
					//listBarPoint = fp->
				case 80:	// "#LISTMAXLINE")
					maxLine = fp->GetWord();
					break;
				case 81:	// "#LINEHEIGHT")
					listMaxHeight = fp->GetLong();
					break;
				case 82:	// "#BTNCLICKSOUND")
					soundIdx = fp->GetInt();
					break;
				case 83:	// "#MINMAXMIDDLENUM")
					minMiddleNum = fp->GetByte();
					maxMiddleNum = fp->GetByte();
					break;
				case 84:	// "#INITGRID")
					gridPos.x = fp->GetDword();
					gridPos.y = fp->GetDword();
					gridPos.w = fp->GetDword();
					gridPos.h = fp->GetDword();
					gridBorderX = fp->GetWord();
					gridBorderY = fp->GetWord();
					break;
				case 85:	// "#INITGUAGEBAR")
					{
						if(WT_GUAGEBAR != wnd->GetType())
						{
							break;
						}

						cGuageBar * win = (cGuageBar *)wnd;
						interval = fp->GetLong();
						vertical = fp->GetBool();
						win->InitGuageBar(interval, vertical);
						break;
					}
				case 86:	// "#MAXSPRITE")
					{
						if(WT_ANI != wnd->GetType())
						{
							break;
						}

						cAni * win = (cAni *)wnd;
						win->SetMaxSprite(fp->GetWord());
						break;
					}
				case 87:	// "#SPRITELOOP")
					fSpriteloop = fp->GetBool();
					break;
				case 88:	// "#TRAVELINGTYPE")
					travelingType = fp->GetWord();
					break;
				case 89:	// "#ADDSPRITE")
					{
						if(WT_ANI != wnd->GetType())
						{
							break;
						}

						cAni * win = (cAni *)wnd;
						GetImage(fp, &AniSpriteImage);
						WORD delayTime = fp->GetWord();
						win->AddSprite(&AniSpriteImage, delayTime);
						break;
					}
				case 90:	// "#GUAGEPIECEIMAGE")
		//			cGuage * win = (cGuage *)wnd;
					GetImage(fp, &GuagePieceImage);
		//			win->SetPieceImage(&GuagePieceImage);
					break;
				case 91:	// "#GUAGEMINVALUE")
					guageMinVal = fp->GetFloat();
					break;
				case 92:	// "#GUAGEMAXVALUE")
					guageMaxVal = fp->GetFloat();
					break;
				case 93:	// "#GUAGECURVALUE")
					guageCurVal = fp->GetFloat();
					break;
				case 94:	// "#GUAGENEVALUE")
					guageCurVal		= fp->GetFloat();
					guageEstTime	= fp->GetDword();
					break;
				case 95:	// "#GUAGEPOS")
					guagePos.x = fp->GetLong();
					guagePos.y = fp->GetLong();
					break;
				case 96:	// "#CHECKBOXIMAGE")
					GetImage(fp, &CheckBoxImage);
					break;
				case 97:	// "#CHECKIMAGE")
					GetImage(fp, &CheckImage);
					break;
				case 98:	// "#SETCHECK")
					bChecked = fp->GetBool();
					break;
				case 99:	// "#SCALE")
					scale.x = fp->GetFloat();
					scale.y = fp->GetFloat();
					break;
				case 100:	// "#LISTSCALE")
					ListScale.x = fp->GetFloat();
					ListScale.y = fp->GetFloat();
					break;
				case 101:	// "#GUAGEWIDTH")
					guageWidth = fp->GetFloat();
					break;
				case 102:	// "#GUAGEPIECEWIDTH")
					guagePieceWidth = fp->GetFloat();
					break;
				case 103:	// "#GUAGEEFFECTPIECEIMAGE")
					GetImage(fp, &guageEffectPieceImage);
					break;
				case 104:	// "#GUAGEEFFECTPIECEWIDTH")
					guageEffectPieceWidth = fp->GetFloat();
					break;
				case 105:	// "#GUAGEPIECEHEIGHTSCALE")
					guagePieceHeightScaleY = fp->GetFloat();
					break;
				case 106:	// "#GUAGEEFFECTPIECEHEIGHTSCALE")
					guageEffectPieceHeightScaleY = fp->GetFloat();
					break;
				case 107:	// "#BALLOONIMAGETOP")
					GetImage(fp, &BalloonImage[0]);
					GetImage(fp, &BalloonImage[1]);
					GetImage(fp, &BalloonImage[2]);
					break;
				case 108:	// "#BALLOONIMAGEMIDDLE")
					GetImage(fp, &BalloonImage[3]);
					GetImage(fp, &BalloonImage[4]);
					GetImage(fp, &BalloonImage[5]);
					break;
				case 109:	// "#BALLOONIMAGEBOTTOM")
					GetImage(fp, &BalloonImage[6]);
					GetImage(fp, &BalloonImage[7]);
					GetImage(fp, &BalloonImage[8]);
					break;
				case 110:	// "#BALLOONIMAGETAIL")
					GetImage(fp, &BalloonImage[9]);
					break;
				case 111:	// "#BALLOONIMAGEBORDER")
					BalloonBorder = fp->GetFloat();
					break;
				// 070328 LYW --- cScriptManager : Add text alpha.
				case 112:	// "#TEXTALPHA")
					bTextAlpha = fp->GetBool() ;
					break;
				// 070403 LYW --- cScriptManager : Add command numbers for chatting.
				case 113:	// "#COMMANDNUM")
					nCommandNum1 = fp->GetInt() ;
					nCommandNum2 = fp->GetInt() ;
					nCommandNum3 = fp->GetInt() ;
					break;
				// 070409 LYW --- cScriptManager : Add text offset for edit box.
				case 114:	// "#TEXTOFFSET")
					lLeft	= fp->GetLong() ;
					lRight	= fp->GetLong() ;
					lTop	= fp->GetLong() ;
					break;
				// 070410 LYW --- cScriptManager : Set show scroll or not.
				case 115:	// "#SHOWSCROLL")
					bShowScroll = fp->GetBool() ;
					break;
				case 116:	// "#DELTAPOS"
				// E 해상도 추가 added by hseos 2007.06.12
					{
						VECTOR2 deltaPosition800 = {0};
						deltaPosition800.x = fp->GetFloat();
						deltaPosition800.y = fp->GetFloat();
						VECTOR2 deltaPosition1024 = {0};
						deltaPosition1024.x = fp->GetFloat();
						deltaPosition1024.y = fp->GetFloat();

						SHORT_RECT rect = {0};
						WORD value = 0;
						g_pExecutive->GetRenderer()->GetClientRect(
							&rect,
							&value,
							&value);

						wnd->SetDeltaPos(
							800 == rect.right ? deltaPosition800 : deltaPosition1024);
					}
					break;
					// 070620 LYW --- cScriptManager : Setting Color of List dialog.
				case 117 :	// "#COLUMNCOLOR"
					{
						List_ColumnColor.r = fp->GetByte() ;
						List_ColumnColor.g = fp->GetByte() ;
						List_ColumnColor.b = fp->GetByte() ;
					}
					break ;

				case 118 :	// "#SELECTCOLUMCOLOR"
					{
						List_SelectColumnColor.r = fp->GetByte() ;
						List_SelectColumnColor.g = fp->GetByte() ;
						List_SelectColumnColor.b = fp->GetByte() ;
					}
					break ;
				case 119:	// #TOOLTIPLINESIZE
					toolTipLineSize = fp->GetDword();
					break;
				// desc_hseos_데이트 존_01
				// S 데이트 존 추가 added by hseos 2007.11.14
				case 120:	// #ROWS_MSGID
					for(int i=0; i<10; i++)
					{
						const DWORD messageIndex = fp->GetDword();

						if(WT_LISTCTRL != wnd->GetType())
						{
							continue;
						}

						((cListCtrl *)wnd)->SetRowsMsgID(i, messageIndex);
					}
					break;
				// E 데이트 존 추가 added by hseos 2007.11.14

				// 071210 LYW --- cScriptManager : 탭 다이얼로그의 스타일을 정의하는 구문 추가.
				case 121 :	//"#TAB_STYLE"
					{
						wnd->SetTabStyle(fp->GetByte());
						wnd->SetVisibleTabCount(fp->GetByte());
					}
					break ;

				//090330 pdy ScrollIconGridDialog 추가
				case 122 :	//"#VIEW_COLS"
					{
						view_cols = fp->GetByte() ;
					}
					break ;

				case 123 :	//"#VIEW_ROWS"
					{
						view_rows = fp->GetByte() ;
					}
					break ;

				case 124 : //"#SORT_LIST"
					{
						bySortType = fp->GetByte();
					}
					break;
				case 125 : //"#REMAIN_OVERIMAGE"
					{
						bRemainOverImage = fp->GetBool();
					}
					break;

				// 091116 ONS 위치저장 속성 추가
				case 126 : // "#SAVE_POSITION"
					{
						bSavePosition = fp->GetBool();
					}
					break;

				// 091207 ONS 현재 활성화상태 속성 추가
				case 127 : // "#SAVE_ACTIVITY"
					{
						bSaveActivity = fp->GetBool();
					}
					break;

				case 128 : // "#USE_TOPROW_CHANGE"
					{
						bUseTopRowChangeEvent = fp->GetBool();
					}
					break;
				case 129:
					{
						wnd->SetOpenSound(
							fp->GetDword());

					}
					break;
				case 130:
					{
						wnd->SetCloseSound(
							fp->GetDword());
					}
					break;
					
				case 131:	// "#ADD_BTN_ANIIMG"
					{
						char buff[MAX_PATH]={0};
						fp->GetString( buff );
						if( buff[0] != '(' )	break;

						const DWORD dwImgIdx = fp->GetDword();
						cImageRect rect;

						rect.left = fp->GetLong();
						rect.top = fp->GetLong();
						rect.right = fp->GetLong();
						rect.bottom = fp->GetLong();

						fp->GetString( buff );
						if( buff[0] != ')' )		break;

						btnAniImgList.push_back( std::make_pair( dwImgIdx, rect ) );
					}
					break;

				case 132:	// "#ADD_BTN_OVER_ANIIMG"
					{
						char buff[MAX_PATH]={0};
						fp->GetString( buff );
						if( buff[0] != '(' )	break;

						const DWORD dwImgIdx = fp->GetDword();
						cImageRect rect;

						rect.left = fp->GetLong();
						rect.top = fp->GetLong();
						rect.right = fp->GetLong();
						rect.bottom = fp->GetLong();

						fp->GetString( buff );
						if( buff[0] != ')' )		break;

						btnOverAniImgList.push_back( std::make_pair( dwImgIdx, rect ) );
					}
					break;

				case 133:	// "#ADD_BTN_PRESS_ANIIMG"
					{
						char buff[MAX_PATH]={0};
						fp->GetString( buff );
						if( buff[0] != '(' )	break;

						const DWORD dwImgIdx = fp->GetDword();
						cImageRect rect;

						rect.left = fp->GetLong();
						rect.top = fp->GetLong();
						rect.right = fp->GetLong();
						rect.bottom = fp->GetLong();

						fp->GetString( buff );
						if( buff[0] != ')' )		break;

						btnPressAniImgList.push_back( std::make_pair( dwImgIdx, rect ) );
					}
					break;
				case 134:	// "#ACTIVE_BTN_ANIIMG"
					{
						bActiveBtnAniImg = fp->GetBool();
					}
					break;
				case 135:	// "#BTN_ANIIMG_FRAMETIME"
					{
						dwBtnAniImgFrameTime = fp->GetDword();
					}
					break;
				case 136:	// "#BTN_OVER_ANIIMG_FRAMETIME"
					{
						dwBtnOverAniImgFrameTime = fp->GetDword();
					}
					break;
				case 137:	// "#BTN_PRESS_ANIIMG_FRAMETIME"
					{
						dwBtnPressAniImgFrameTime = fp->GetDword();
					}
					break;
				case 138:	// "#BTN_ANIIMG_FIRST_DRAW"
					{
						bFirstDrawBtnAnimateImage = fp->GetDword();
					}
					break;
				case 139:	// "#COMPONENT_ID"
					{
						uiComponentID = fp->GetLong();
					}
					break;
				case 140:	// "#BTN_MOUSE_EVT_USE_IMG_ALPHA"
					{
						btnMouseEvt = cButton::eBtn_Mouse_Evt( fp->GetInt() );
					}
					break;
				case 141:	// "#VALIDXY_BTN_ANIIMG"
					{
						validXYBtnAniImg.x = fp->GetFloat();
						validXYBtnAniImg.y = fp->GetFloat();
					}
					break;
				case 142:	// "#VALIDXY_BTN_OVER_ANIIMG"
					{
						validXYBtnOverAniImg.x = fp->GetFloat();
						validXYBtnOverAniImg.y = fp->GetFloat();
					}
					break;
				case 143:	// "#VALIDXY_BTN_PRESS_ANIIMG"
					{
						validXYBtnPressAniImg.x = fp->GetFloat();
						validXYBtnPressAniImg.y = fp->GetFloat();
					}
					break;
				case 144:	// "#IMAGE_RGBA"
					{
						const BYTE red = fp->GetByte();
						const BYTE green = fp->GetByte();
						const BYTE blue = fp->GetByte();
						const BYTE alpha = fp->GetByte();
						const DWORD rgba = RGBA_MAKE( red, green, blue, alpha );
						wnd->SetImageRGB( rgba );
					}
					break;

				default:
					_asm int 3;
			}
		}
	}
	// 모든 윈도우에 등록
	wnd->SetAutoClose(bAutoClose);
	wnd->SetFontIdx(fontIdx);
	wnd->SetImageSrcRect(&imgSrcRect);
	wnd->SetScale(&scale);
	wnd->OnCreate(fActive);
	wnd->SetAlpha(alphaDepth);
	wnd->SetValidXY( validXY.x, validXY.y );

	// UI ID 설정
	if( uiComponentID == 0 )
		uiID = IDSEARCH(IDName);
	else
		uiID = uiComponentID;

	// 091116 ONS 위치 및 활성화 속성 저장 
	wnd->SetSavePosition(bSavePosition);
	if( bSavePosition || bSaveActivity )
	{
		UISETTINGMGR->SetSettingList( uiID, bSavePosition, bSaveActivity );
	}

	SWINDOWTYPE(wnd->GetType())
	CASE(WT_GUAGE)
		cGuage * win = (cGuage *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->InitGuage(guagePos.x,guagePos.y);
		win->SetMinValue(guageMinVal);
		win->SetMaxValue(guageMaxVal);
		win->SetCurValue(guageCurVal);
		win->SetPieceImage(&GuagePieceImage);
		
	BREAK
	CASE(WT_GUAGEN)
		cGuagen * win = (cGuagen *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetGuageImagePos(guagePos.x,guagePos.y);
		win->SetValue(guageCurVal);
		win->SetPieceImage(&GuagePieceImage);
		win->SetGuageWidth(guageWidth);
		win->SetGuagePieceWidth(guagePieceWidth);
	BREAK
	CASE(WT_GUAGENE)
		CObjectGuagen * win = (CObjectGuagen *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetGuageImagePos(guagePos.x,guagePos.y);
		win->SetValue(guageCurVal, guageEstTime);
		win->SetPieceImage(&GuagePieceImage);
		win->SetGuageWidth(guageWidth);
		win->SetGuagePieceWidth(guagePieceWidth);
		win->SetGuagePieceHeightScale(guagePieceHeightScaleY);


		win->SetGuageEffectPieceWidth(guageEffectPieceWidth);
		win->SetEffectPieceImage(&guageEffectPieceImage);
		win->SetGuageEffectPieceHeightScale(guageEffectPieceHeightScaleY);
	BREAK
	CASE(WT_ANI)
		cAni * win = (cAni *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetLoop(fSpriteloop);
		win->SetTravelingType(travelingType);
	BREAK
	CASE(WT_SPIN)
		cSpin * win = (cSpin *)wnd;
		if(FUNCSEARCH(funcName) == -1)
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, NULL, uiID);
		else
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, g_mt_func[FUNCSEARCH(funcName)]._func, uiID);
		win->InitSpin(EditBoxSize, EditBoxEableTypingSize);
		win->SetActiveTextColor(RGB_HALF(activeTextColor.r,activeTextColor.g,activeTextColor.b));
		win->SetNonactiveTextColor(RGB_HALF(nonactiveTextColor.r,nonactiveTextColor.g,nonactiveTextColor.b));
		win->SetMinMax(minValue, maxValue);
		win->SetUnit(spinUnit);

		// 090819 ShinJS --- cSpin 에 Caret Color 적용 추가
		win->SetCaretColor( RGB_HALF(caretColor.r, caretColor.g, caretColor.b) ) ;
	BREAK
	CASE(WT_STATIC)
		cStatic * win = (cStatic *)wnd;
		if( bHasExtendColor )
		{
			win->SetHasExtendColor( TRUE ) ;

			win->SetExtendColorCount(byExtendColorCount) ;

			win->InitializeExtendColor() ;

			for( BYTE count = 0 ; count < byExtendColorCount ; ++count )
			{
				win->SetExtendColor( count, dwExtendColor[count] ) ;
			}
		}

		win->SetTextAlpha(bTextAlpha) ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetTextXY(textXY.x, textXY.y);
		win->SetStaticText(staticText);
		win->SetAlign( nTextAlign );
		win->SetFGColor(RGB_HALF(fgColor.r, fgColor.g, fgColor.b));

		win->SetShadowTextXY(shadowTextX, shadowTextY);
		win->SetShadowColor(RGB_HALF(dwShadowColor.r, dwShadowColor.g, dwShadowColor.b));
		win->SetShadow(bShadow);

		if( toolTipLineSize )
		{
			win->SetAutoLineToolTip( toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage, toolTipLineSize );
		}
		else
		{
			win->SetToolTip(toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage);
		}
		
		if( tt_idx >= 0 )
		{
			cImage imgToolTip;
			SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
			win->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &imgToolTip, TTCLR_DEFAULT );

			ITEMMGR->AddToolTip( win, tt_idx );
		}

	BREAK
	CASE(WT_PUSHUPBUTTON)
		cPushupButton * win = (cPushupButton *)wnd;
		if(FUNCSEARCH(funcName) == -1)
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &OverImage, &PressImage, NULL, uiID);
		else	
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &OverImage, &PressImage, g_mt_func[FUNCSEARCH(funcName)]._func, uiID);
		
		if( toolTipLineSize )
		{
			win->SetAutoLineToolTip( toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage, toolTipLineSize );
		}
		else
		{
			win->SetToolTip(toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage);
		}

		win->SetPush(fPush);
		win->SetText(szBtnText, RGB_HALF(btnTextBasicCol.r,btnTextBasicCol.g,btnTextBasicCol.b), RGB_HALF(btnTextOverCol.r,btnTextOverCol.g,btnTextOverCol.b), RGB_HALF(btnTextPressCol.r,btnTextPressCol.g,btnTextPressCol.b)); 
		win->SetTextXY(textXY.x,textXY.y);
		win->SetTextAni(btnTextAniX, btnTextAniY );
		win->SetAlign( nTextAlign );
		win->SetShadowTextXY(shadowTextX, shadowTextY);
		win->SetShadowColor(RGB_HALF(dwShadowColor.r, dwShadowColor.g, dwShadowColor.b));
		win->SetShadow(bShadow);
		win->SetPassive(bPassive);
	BREAK
	CASE(WT_COMBOBOX)
		cComboBox * win = (cComboBox *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, NULL, uiID);
		win->SetComboTextColor(RGB_HALF(comboTextColor.r, comboTextColor.g, comboTextColor.b));
		win->SelectComboText(selectComboIdx);

		win->SetOverImageScale(ListScale.x, ListScale.y); // over image 추가
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
		//win->Init()
	BREAK
	CASE(WT_COMBOBOXEX)
		cComboBoxEx * win = (cComboBoxEx *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, NULL, uiID);
		win->SetComboTextColor(RGB_HALF(comboTextColor.r, comboTextColor.g, comboTextColor.b));
		win->SelectComboText(selectComboIdx);
		win->SetOverImageScale(ListScale.x, ListScale.y); // over image 추가
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_BUTTON)
		cButton * win = (cButton *)wnd;
		if(FUNCSEARCH(funcName) == -1)
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &OverImage, &PressImage, NULL, uiID);
		else 
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &OverImage, &PressImage, g_mt_func[FUNCSEARCH(funcName)]._func, uiID);

		if( toolTipLineSize )
		{
			win->SetAutoLineToolTip( toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage, toolTipLineSize );
		}
		else
		{
			win->SetToolTip(toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage);
		}

		if(*szBtnText != 0)
		win->SetText(szBtnText, RGB_HALF(btnTextBasicCol.r,btnTextBasicCol.g,btnTextBasicCol.b), RGB_HALF(btnTextOverCol.r,btnTextOverCol.g,btnTextOverCol.b), RGB_HALF(btnTextPressCol.r,btnTextPressCol.g,btnTextPressCol.b)); 
		win->SetTextXY(textXY.x,textXY.y);
		win->SetTextAni(btnTextAniX, btnTextAniY );
		win->SetAlign( nTextAlign );

		win->SetShadowTextXY(shadowTextX, shadowTextY);
		win->SetShadowColor(RGB_HALF(dwShadowColor.r, dwShadowColor.g, dwShadowColor.b));
		win->SetShadow(bShadow);

		if( soundIdx != -1 )
			win->SetClickSound( soundIdx );

		win->AddAnimationImage( cButton::eBtn_State_None, btnAniImgList, dwBtnAniImgFrameTime );
		win->AddAnimationImage( cButton::eBtn_State_Over, btnOverAniImgList, dwBtnOverAniImgFrameTime );
		win->AddAnimationImage( cButton::eBtn_State_Press, btnPressAniImgList, dwBtnPressAniImgFrameTime );
		win->SetActiveAnimationImage( bActiveBtnAniImg );
		win->SetFirstDrawAnimateImage( bFirstDrawBtnAnimateImage );
		win->SetBtnMouseEvent( btnMouseEvt );
		win->SetAnimationImageValidXY( validXYBtnAniImg, validXYBtnOverAniImg, validXYBtnPressAniImg );
	BREAK
	CASE(WT_CHECKBOX)	
		cCheckBox * win = (cCheckBox *)wnd;
		if(FUNCSEARCH(funcName) == -1)
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &CheckBoxImage, &CheckImage, NULL, uiID);
		else
			win->Init(point.x, point.y, point.w, point.h, &BasicImage, &CheckBoxImage, &CheckImage, g_mt_func[FUNCSEARCH(funcName)]._func, uiID);
		win->SetChecked(bChecked);
		win->SetCheckBoxMsg(staticText, RGB_HALF(fgColor.r,fgColor.g,fgColor.b));
		// 061204 LYW --- Add tooltip message.
		if( toolTipLineSize )
		{
			win->SetAutoLineToolTip( toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage, toolTipLineSize );
		}
		else
		{
			win->SetToolTip(toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage);
		}

	BREAK
	CASE(WT_EDITBOX)
		cEditBox * win = (cEditBox *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, &FocusImage, uiID);
		win->InitEditbox(EditBoxSize, EditBoxEableTypingSize);
		win->SetAlign( nTextAlign );
		
		if( toolTipLineSize )
		{
			win->SetAutoLineToolTip( toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage, toolTipLineSize );
		}
		else
		{
			win->SetToolTip(toolTipMsg, RGBA_MAKE(color.r,color.g,color.b, 255), &ToolTipImage);
		}

		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
		win->SetSecret(fSecret);
		win->SetActiveTextColor(RGB_HALF(activeTextColor.r,activeTextColor.g,activeTextColor.b));
		win->SetNonactiveTextColor(RGB_HALF(nonactiveTextColor.r,nonactiveTextColor.g,nonactiveTextColor.b));
		win->SetReadOnly(bReadOnly);
		// 061227 LYW --- Add function to setting color of caret.
		win->SetCaretColor( RGB_HALF(caretColor.r, caretColor.g, caretColor.b) ) ;

		// 070409 LYW --- Add function to setting text offset.
		win->SetTextOffset( lLeft, lRight, lTop ) ;
	BREAK
	CASE(WT_GUAGEBAR)
		cGuageBar * win = (cGuageBar *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->InitValue(guageMinVal, guageMaxVal, guageCurVal);
	BREAK
	CASE(WT_TEXTAREA)
		cTextArea * win = (cTextArea *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->InitTextArea(&textRect, nLimitBytes, &TopImage, topHei, &MiddleImage, middleHei, &DownImage, downHei );
 		win->SetLimitLine( limitLineNum );	//inittextarea보다 아래에 있어야한다.
		win->SetReadOnly(bReadOnly);
		win->SetMiddleScale( ListScale );
		win->SetScriptText(staticText); 
		// 061227 LYW --- Add font color.
		win->SetFontColor(RGB_HALF(fgColor.r,fgColor.g,fgColor.b)) ;
	BREAK
	CASE(WT_LIST)
		cList * win = (cList *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &TopImage, uiID);
		win->InitList(maxLine, &textRect);
		win->SetLineUpward(lineUpward);
		win->SetListBarImage(&ListBarImage);
		win->SetListBar(listBarPoint.x, listBarPoint.y, listBarPoint.w, listBarPoint.h, interval);
		win->SetMaxLine(maxLine);
	BREAK
	CASE(WT_LISTCTRL)
		cListCtrl * win = (cListCtrl *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->InitListCtrl(cols, rows);
		win->InitListCtrlImage(&HeadImage, HeadHei, &BodyImage, BodyHei, &ListOverImage );
		win->SetOverImageScale(ListScale.x, ListScale.y); // over image 추가
		win->SetSelectOption( wOption );
	BREAK
	CASE(WT_LISTCTRLEX)
		cListCtrlEx * win = (cListCtrlEx *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->InitListCtrlEx( cols, middleNum, middleHei, nLeftMargin, nTopMargin );
		win->InitListCtrlExImage( &HeadImage, &BodyImage, &DownImage, &SelectedImage, &ListOverImage );
	BREAK
	CASE(WT_LISTDIALOG)
		cListDialog * win = (cListDialog *)wnd;

	// 070410 LYW --- cScriptManager : Set show scroll or not.
		win->SetShowScroll(bShowScroll) ;
		win->SetTextRect( textRect );

		win->SetMinMaxMiddleNum(minMiddleNum, maxMiddleNum);
		win->Init(point.x, point.y, point.w, &TopImage, topHei, &MiddleImage, middleHei, &DownImage, downHei, &ListOverImage, middleNum, uiID);
		win->SetLineHeight(listMaxHeight);
		win->InitList(maxLine, &textRect);
		win->SetMaxLine(maxLine);
		win->SetOverImageScale(ListScale.x, ListScale.y); // over image 추가
		win->SetAutoScroll( bAutoScroll );

		// 070620 LYW --- cScriptManager : Add columncolor for list dialog.
		win->SetColumnColor(RGB_HALF(List_ColumnColor.r, List_ColumnColor.g, List_ColumnColor.b)) ;
		win->SetSelectColumnColor(RGB_HALF(List_SelectColumnColor.r, List_SelectColumnColor.g, List_SelectColumnColor.b)) ;

		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);

		// 090820 ShinJS --- 정렬 추가
		win->SetSortType( bySortType );

		// 091125 ShinJS --- List Dialog, 클릭Item에 대하여 마우스 OverImage Render 여부 추가
		win->SetRemainOverImage( bRemainOverImage );

		// 091209 ShinJS --- List Dialog, TopListItem 변경시 Func 실행 여부 추가
		win->SetUseTopRowChangeEvent( bUseTopRowChangeEvent );
	BREAK
	CASE(WT_CHARMAKEDIALOG)
		// 061217 LYW --- Change this code.
		CCharMakeNewDlg * win = (CCharMakeNewDlg *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_DIALOG)
		cDialog * win = (cDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_TABDIALOG)
		cTabDialog * win = (cTabDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_ICONDIALOG)
		cIconDialog * win = (cIconDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetCaptionRect(&captionRect);
		win->SetIconCellBGImage(&selectedBGImage);
		win->SetDragOverBGImage(&dragoverBGImage);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_WEAREDDIALOG)
		CWearedExDialog * win = (CWearedExDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetCaptionRect(&captionRect);
		win->SetIconCellBGImage(&selectedBGImage);
		win->SetDragOverBGImage(&dragoverBGImage);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_ICONGRIDDIALOG)
		cIconGridDialog * win = (cIconGridDialog *)wnd;
		win->InitDialog(point.x, point.y, point.w, point.h, &BasicImage, cols, rows, uiID);
		win->SetCaptionRect(&captionRect);
		win->InitGrid(gridPos.x,gridPos.y,gridPos.w,gridPos.h,gridBorderX,gridBorderY);
		win->SetIconCellBGImage(&selectedBGImage);
		win->SetDragOverBGImage(&dragoverBGImage);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_MONSTERGUAGEDLG)
		cDialog * win = (cDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_QUICKDIALOG)
		cQuickDlg * win = (cQuickDlg *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_CHARINFODIALOG)
		CCharacterDialog * win = (CCharacterDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_INVENTORYDIALOG)
		CInventoryExDialog * win = (CInventoryExDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_STREETSTALLDIALOG)
		CStreetStall * win = (CStreetStall *)wnd;	
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_STREETBUYSTALLDIALOG)
		CStreetBuyStall * win = (CStreetBuyStall *)wnd;	
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_NPCSCRIPDIALOG)
		cNpcScriptDialog * win = (cNpcScriptDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_MONEYDIALOG)
		CMoneyDlg * win = (CMoneyDlg *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_DEALDIALOG)
		CDealDialog * win = (CDealDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK		
	CASE(WT_STORAGEDLG)
		CStorageDialog * win = (CStorageDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK		
	CASE(WT_NOTEDLG)
		CNoteDialog * win = (CNoteDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK		
	CASE(WT_MININOTEDLG)
		CMiniNoteDialogWrite * win = (CMiniNoteDialogWrite*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_MININOTEDLGREAD)
		CMiniNoteDialogRead * win = (CMiniNoteDialogRead*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_FRIENDDLG)
		CFriendDialog * win = (CFriendDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK		
	CASE(WT_MINIFRIENDDLG)
		CMiniFriendDialog * win = (CMiniFriendDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_QUESTTOTALDIALOG)
		CQuestTotalDialog * win = (CQuestTotalDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_QUESTDIALOG)
		CQuestDialog * win = (CQuestDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetIconCellBGImage(&selectedBGImage);
		win->SetDragOverBGImage(&dragoverBGImage);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_PKLOOTINGDLG)
		CPKLootingDialog* win = (CPKLootingDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, -1);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_PARTYCREATEDLG)
		CPartyCreateDlg * win = (CPartyCreateDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_PARTYINVITEDLG)
		CPartyInviteDlg * win = (CPartyInviteDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDCREATEDLG)
		CGuildCreateDialog * win = (CGuildCreateDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDDLG)
		CGuildDialog * win = (CGuildDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDINVITEDLG)
		CGuildInviteDialog * win = (CGuildInviteDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDMARKDLG)
		CGuildMarkDialog * win = (CGuildMarkDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDLEVELUPDLG)
		CGuildLevelUpDialog * win = (CGuildLevelUpDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDNICKNAMEDLG)
		CGuildNickNameDialog * win = (CGuildNickNameDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDRANKDLG)
		CGuildRankDialog * win = (CGuildRankDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);		
	BREAK
	CASE(WT_GUILDWAREHOUSEDLG)
		CGuildWarehouseDialog * win = (CGuildWarehouseDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDUNIONCREATEDLG)
		CGuildUnionCreateDialog * win = (CGuildUnionCreateDialog *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_ITEMSHOPDIALOG)
		CItemShopDialog * win = (CItemShopDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDFIELDWAR_DECLARE_DLG)
		CGFWarDeclareDlg* win = (CGFWarDeclareDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDFIELDWAR_RESULT_DLG)
		CGFWarResultDlg* win = (CGFWarResultDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDFIELDWAR_INFO_DLG)
		CGFWarInfoDlg* win = (CGFWarInfoDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GUILDWAR_INFO_DLG)
		CGuildWarInfoDlg* win = (CGuildWarInfoDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_SHOUT_DLG)
		CShoutDialog* win = (CShoutDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GTSTANDING_DLG)
		CGTStandingDialog* win = (CGTStandingDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GTBATTLELIST_DLG)
		CGTBattleListDialog* win = (CGTBattleListDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_GTSCOREINFO_DLG)
		CGTScoreInfoDialog* win = (CGTScoreInfoDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_SKILLTREE_DLG)
		cSkillTreeDlg* win = (cSkillTreeDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
		// 061219 LYW --- Add main system dialog part in script.
	CASE(WT_MAINSYSTEM_DLG)
		CMainSystemDlg* win = ( CMainSystemDlg*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;
	CASE(WT_JOBSKILL_DLG)
		cJobSkillDlg* win = (cJobSkillDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_CHATTING)
		CChattingDlg* win = (CChattingDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	// 070605 LYW --- cScriptManager : Add matching part.
	CASE(WT_MATCHPARTNERLISTDLG)
		CDateMatchingPartnerList* win = (CDateMatchingPartnerList*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_MATCHCHATDLG)
		CDateMatchingChatDlg* win = (CDateMatchingChatDlg*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_MATCHINFODLG)
		CDateMatchingInfoDlg* win = (CDateMatchingInfoDlg*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_MATCHRECORDDLG)
		CDateMatchingRecordDlg* win = (CDateMatchingRecordDlg*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_MATCH_IDENTIFICATIONDLG)
		CIdentification* win = (CIdentification*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_MATCH_FAVOR_ICONDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK ;

	CASE(WT_FAMILYDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK;

	CASE(WT_FAMILYCREATEDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK;
	CASE(WT_FAMILYMARKDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK;

	CASE(WT_FAMILYNICKNAMEDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK;

	CASE(WT_GUILDWAREHOUSERANKDLG)
		CFavorIcon* win = (CFavorIcon*)wnd ;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK;

	CASE(WT_QUESTQUICKVIEWDIALOG)
		CQuestQuickViewDialog * win = (CQuestQuickViewDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK 

	CASE(WT_GUILDREVIVALDLG)
		CGuildRevivalDialog * win = (CGuildRevivalDialog*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK 

	//090323 pdy 하우징 설치 UI추가
	CASE(WT_HOUSING_REVOL_DLG)
		CHousingRevolDlg * win = (CHousingRevolDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090323 pdy SCROLLICONGRIDDLG 추가 
	CASE(WT_SCROLLICONGRIDDLG)
		cScrollIconGridDialog * win = (cScrollIconGridDialog *)wnd;
		win->InitDialog(point.x, point.y, point.w, point.h, &BasicImage, cols, rows, uiID);
		win->SetCaptionRect(&captionRect);
		win->InitGrid(gridPos.x,gridPos.y,gridPos.w,gridPos.h,gridBorderX,gridBorderY);
		win->SetIconCellBGImage(&selectedBGImage);
		win->SetDragOverBGImage(&dragoverBGImage);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);

		win->SetViewRowCol(view_rows,view_cols);
	BREAK

	//090330 pdy 하우징 창고 UI추가
	CASE(WT_HOUSING_WAREHOUSE_DLG)
		cHousingWarehouseDlg * win = (cHousingWarehouseDlg *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);

	BREAK

	//090409 pdy 하우징 하우스 검색 UI추가
	CASE(WT_HOUSE_SEARCH_DLG)
		cHouseSearchDlg * win = (cHouseSearchDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090410 pdy 하우징 하우스 이름설정 UI추가
	CASE(WT_HOUSE_NAME_DLG)
		cHouseNameDlg * win = (cHouseNameDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	CASE(WT_HOUSING_DECOPOINT_DLG)
		cHousingDecoPointDlg * win = (cHousingDecoPointDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	CASE(WT_HOUSING_ACTIONPOPUPMENUDLG)
		cHousingActionPopupMenuDlg * win = (cHousingActionPopupMenuDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090525 pdy 하우징 매인포인트 UI추가
	CASE(WT_HOUSING_MAINPOINTDLG)
		cHousingMainPointDlg * win = (cHousingMainPointDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090525 pdy 하우징 내집창고 버튼 UI추가
	CASE(WT_HOUSING_WAREHOUSE_BUTTON_DLG)
		cHousingWarehouseButtonDlg * win = (cHousingWarehouseButtonDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	//090622 NYJ 하우징 웹브라우저
	CASE(WT_BROWSER)
		cBrowser * win = (cBrowser*)wnd;
		win->Init(point.x, point.y, point.w, point.h, NULL, uiID);
		win->SetMovable(fMovable);
		win->SetActive(fActive);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK 

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	CASE(WT_HOUSING_DECOMODE_BT_DLG)
		cHousingDecoModeBtDlg * win = (cHousingDecoModeBtDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK
	CASE(WT_CONSIGNMENTDLG)
		CConsignmentDlg * win = (CConsignmentDlg*)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, uiID);		
		win->SetMovable(fMovable);
		win->SetCaptionRect(&captionRect);
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);
	BREAK

	CASE(WT_CHANNELCOMBOBOX)
		cChannelComboBox * win = (cChannelComboBox *)wnd;
		win->Init(point.x, point.y, point.w, point.h, &BasicImage, NULL, uiID);
		win->SetComboTextColor(RGB_HALF(comboTextColor.r, comboTextColor.g, comboTextColor.b));
		win->SelectComboText(selectComboIdx);

		win->SetOverImageScale(ListScale.x, ListScale.y); // over image 추가
		if(-1 != FUNCSEARCH(funcName))
			win->SetcbFunc(g_mt_func[FUNCSEARCH(funcName)]._func);

	BREAK


	EWINDOWTYPE	

	return wnd;
}
