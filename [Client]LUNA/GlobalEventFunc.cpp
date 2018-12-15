#include "stdafx.h"
#include "GlobalEventFunc.h"
#include "MainGame.h"
#include "CharMake.h"
#include "CharSelect.h"
#include "GameIn.h"
#include "MainTitle.h"
#include "cComboBoxEx.h"
#include "ObjectManager.h"
#include "MoveManager.h"
#include "AppearanceManager.h"
#include "WindowIdEnum.h"
#include "interface/cWindowManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "StorageManager.h"
#include "FriendManager.h"
#include "NoteManager.h"
#include "ChatManager.h"
#include "ItemManager.h"
#include "ExchangeManager.h"
#include "StreetStallManager.h"
#include "ShowdownManager.h"
#include "FilteringTable.h"
#include "CharMakeManager.h"
#include "ChannelDialog.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "MixDialog.h"
#include "ProgressDialog.h"
#include "StallKindSelectDlg.h"
#include "StreetBuyStall.h"
#include "BuyRegDialog.h"
#include "StreetStall.h"
#include "BuyItem.h"
#include "cListDialogEx.h"
#include "NpcScriptDialog.h"
#include "MoneyDlg.h"
#include "DealDialog.h"
#include "MiniFriendDialog.h"
#include "FriendDialog.h"
#include "MiniNoteDialog.h"
#include "MiniNoteDialogRead.h"
#include "NoteDialog.h"
#include "OptionDialog.h"
#include "ExchangeDialog.h"
#include "PKLootingDialog.h"
#include "StorageDialog.h"
#include "MiniMapDlg.h"
#include "ExitDialog.h"
#include "ReinforceDlg.h"
#include "QuestDialog.h"
#include "QuestQuickViewDialog.h"
#include "PartyCreateDlg.h"
#include "PartyInviteDlg.h"
#include "QuestManager.h"
#include "GuildDialog.h"
#include "FamilyDialog.h"
#include "FamilyRankDialog.h"
#include "FamilyMarkDialog.h"
#include "FamilyNickNameDialog.h"
#include "MHMap.h"
#include "MHCamera.h"
#include "cMsgBox.h"
#include "cDivideBox.h"
#include "cmoneydividebox.h"
#include "./input/UserInput.h"
#include "./Audio/MHAudioManager.h"
#include "cImeEx.h"
#include "PKManager.h"
#include "ObjectStateManager.h"
#include "ReviveDialog.h"
#include "UserInfoManager.h"
#include "ExitManager.h"
#include "InventoryExDialog.h"
#include "PartyIconManager.h"
#include "ExchangeItem.h"
#include "ItemShopDialog.h"
#include "GuildFieldWarDialog.h"
#include "GuildFieldWar.h"
#include "ShoutDialog.h"
#include "PartyBtnDlg.h"
#include "MonsterGuageDlg.h"
#include "GTStandingDialog.h"
#include "GTBattleListDialog.h"
#include "GTScoreInfoDialog.h"
#include "GuildUnion.h"
#include "GuildUnionMarkMgr.h"
#include "GuageDialog.h"
#include "ReinforceGuideDialog.h"
#include "ServerListDialog.h"
#include "../Interface/cResourceManager.h"
#include "cSkillTreeDlg.h"
#include "ChattingDlg.h"
#include "CharMakeNewDlg.h"
#include "CertificateDlg.h"
#include "MainSystemDlg.h"
#include "NpcScriptManager.h"
#include "ActionTarget.h"
#include "NpcImageDlg.h"
#include "cJobSkillDlg.h"
#include "QuickDlg.h"
#include "cSkillTrainingDlg.h"
#include "DateMatchingDlg.h"
#include "../KeySettingTipDlg.h"
#include "../hseos/ResidentRegist/SHResidentRegistManager.h"
#include "../hseos/family/shfamilymanager.h"
#include "QuestTotalDialog.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "../hseos/Farm/SHFarmBuyDlg.h"
#include "../hseos/Farm/SHFarmUpgradeDlg.h"
#include "../hseos/Farm/SHFarmManageDlg.h"
#include "TutorialManager.h"
#include "TutorialBtnDlg.h"
#include "cMapMoveDialog.h"
#include "../hseos/Date/SHDateZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneListDlg.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"
#include "./Interface/cNumberPadDialog.h"
#include "AutoNoteDlg.h"
#include "AutoAnswerDlg.h"
#include "StoreSearchDlg.h"
#include "ChatRoomMgr.h"
#include "ChatRoomMainDlg.h"
#include "ChatRoomCreateDlg.h"
#include "ChatRoomJoinDlg.h"
#include "ChatRoomDlg.h"
#include "ChatRoomGuestListDlg.h"
#include "ChatRoomOptionDlg.h"
#include "FishingManager.h"
#include "FishingDialog.h"
#include "FishingPointDialog.h"
#include "petstatedialog.h"
#include "PetResurrectionDialog.h"
#include "../hseos/Farm/FarmAnimalDlg.h"
#include "petweareddialog.h"
#include "../[CC]SiegeDungeon/SiegeDungeonMgr.h"
#include "./NpcRecallMgr.h"
#include "MHMap.h"
#include "GuildTournamentMgr.h"
#include "popupmenudlg.h"
#include "CookDlg.h"
#include "RidePopupMenuDlg.h"
#include "VehicleManager.h"
#include "GradeClassDlg.h"
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
#include "cHousingStoredIcon.h"
#include "InputNameDlg.h"
#include "cchangenamedialog.h"
#include "BigMapDlg.h"
#include "VideoCaptureManager.h"
#include "AdditionalButtonDlg.h"
#include "ConsignmentDlg.h"
#include "WorldMapDlg.h"
#include "MonsterTargetDlg.h"

extern HWND _g_hWnd;

extern int	g_nServerSetNum;
extern ScriptCheckValue g_Check;

FUNC g_mt_func[] =
{
	{MT_LogInOkBtnFunc,"MT_LogInOkBtnFunc"},
	{MT_EditReturnFunc, "MT_EditReturnFunc"},
	{MT_ExitBtnFunc,"MT_ExitBtnFunc"},
	{MT_DynamicBtnFunc,"MT_DynamicBtnFunc"},
	{MT_DynamicEditReturnFunc, "MT_DynamicEditReturnFunc"},

	{CS_BtnFuncCreateChar,"CS_BtnFuncCreateChar"}, 
	{CS_BtnFuncDeleteChar,"CS_BtnFuncDeleteChar"},	 
	{CS_BtnFuncFirstChar,"CS_BtnFuncFirstChar"},	 
	{CS_BtnFuncSecondChar, "CS_BtnFuncSecondChar"},
	{CS_BtnFuncThirdChar,"CS_BtnFuncThirdChar"},
	{CS_BtnFuncFourthchar,"CS_BtnFuncFourthchar"},
	{CS_BtnFuncFifthchar,"CS_BtnFuncFifthchar"},
	{CS_BtnFuncEnter,"CS_BtnFuncEnter"},//¡E￠c¡§￠®OAO￠®¡×oAAU ¡§Ioo￠®¡×¡E￠c¡E￠c¡§￠® //10
	{CS_FUNC_OkISee,"CS_FUNC_OkISee"},
	{CS_BtnFuncLogOut,"CS_BtnFuncLogOut"},
	// 061218 LYW --- Add callback function to process events from change serverlist button.
	{CS_BtnFuncChangeServer, "CS_BtnFuncChangeServer"},
	// 061219 LYW --- Add callback function to process events from certificate dialog.
	{CTF_CBFunc, "CTF_CBFunc"},

	{MI_CharBtnFunc, "MI_CharBtnFunc"},
	{MI_ExchangeBtnFunc, "MI_ExchangeBtnFunc"},
	{MI_InventoryBtnFunc, "MI_InventoryBtnFunc"},
	{MI_SkillBtnFunc, "MI_SkillBtnFunc"},
	{IN_DlgFunc, "IN_DlgFunc"}, 
	{STD_SkillTreeDlgFunc, "STD_SkillTreeDlgFunc"},
	{CI_DlgFunc, "CI_DlgFunc"},
	{CI_AddExpPoint, "CI_AddExpPoint"},
	// 070111 LYW --- Add callback function to process events.
	{ CI_ActionEventFun, "CI_ActionEventFun" },
	{SO_DlgFunc, "SO_DlgFunc"},

	{BRS_DlgFunc, "BRS_DlgFunc"},
	{BRS_CloseFunc, "BRS_CloseFunc"},
	{BRS_DeleteFunc, "BRS_DeleteFunc"},
	{BRS_TypeListFunc, "BRS_TypeListFunc"},
	{BRS_ItemListFunc, "BRS_ItemListFunc"},
	{BRS_ClassListFunc, "BRS_ClassListFunc"},
	{BRS_REGBtnFunc, "BRS_REGBtnFunc"},

	{BS_DlgFunc, "BS_DlgFunc"},
	{BS_TitleEditBoxFunc, "BS_TitleEditBoxFunc"},
	{BS_SellBtnFunc, "BS_SellBtnFunc"}, 
	{SSI_DlgFunc, "SSI_DlgFunc"},
	{SSI_TitleEditBoxFunc, "SSI_TitleEditBoxFunc"},
	{SSI_BuyBtnFunc, "SSI_BuyBtnFunc"}, 
	{SSI_EditBtnFunc, "SSI_EditBtnFunc"}, 
	{NSI_HyperLinkFunc, "NSI_HyperLinkFunc"},

	{CMI_MoneyOkFunc, "CMI_MoneyOkFunc"},
	{DE_BuyItemFunc, "DE_BuyItemFunc"},//40
	{DE_SellItemFunc, "DE_SellItemFunc"},
	// 070503 LYW --- GlobalEventFunc : Add global event function .
	{ DE_CancelFunc, "DE_CancelFunc" },
	{DE_DlgFunc, "DE_DlgFunc"},
	{DE_CloseHideBtn, "DE_CloseHideBtn"},

	{PA_BtnFunc, "PA_BtnFunc"},

	// 080318 LUJ, 파티 멤버 창 이벤트 처리
	{PartyMemberFunc, "PartyMemberFunc" },

	{QI_QuickDlgFunc, "QI_QuickDlgFunc"},
	{CM_CharMakeBtnFunc, "CM_CharMakeBtnFunc"},
	{CM_CharCancelBtnFunc, "CM_CharCancelBtnFunc"}, 
	{PYO_DlgBtnFunc, "PYO_DlgBtnFunc"},
	{Note_DlgBtnFunc, "Note_DlgBtnFunc"},
	{Friend_DlgFunc, "Friend_DlgFunc"},			//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
	{CR_DlgBtnFunc, "CR_DlgBtnFunc"},

	{MNM_DlgFunc,"MNM_DlgFunc"},
	{ITD_DlgFunc,"ITD_DlgFunc"},
	{ITMD_DlgFunc,"ITMD_DlgFunc"},  //60
	{ITR_DlgFunc, "ITR_DlgFunc"}, //

	// 080215 LUJ, 옵션 적용 창을 위한 함수
	{ApplyOptionDialogFunc, "ApplyOptionDialogFunc"},
	{CHA_DlgBtnFunc, "CHA_DlgBtnFunc"},
	{OTI_DlgFunc, "OTI_DlgFunc"},	//¡E￠c¡§￠®OAO￠®E?E￠®¡×uCA¡E￠cE
	{EXT_DlgFunc, "EXT_DlgFunc"},	//A￠®¡×u¡E￠c¡E￠￥aA¡E￠cE	//
	{XCI_DlgFunc, "XCI_DlgFunc"},	//¡E￠c¡§u¡§I¨I¨￡E?A¡E￠cE
	{PLI_DlgFunc, "PLI_DlgFunc"},
	{CNA_BtnOkFunc, "CNA_BtnOkFunc"},
	{DIVIDEBOX_Func, "DIVIDEBOX_Func"},	//Divide Box
	{MONEYDIVIDEBOX_Func, "MONEYDIVIDEBOX_Func"},	//Divide Box
	{SL_DlgBtnFunc, "SL_DlgBtnFunc" },
	{QUE_QuestDlgFunc, "QUE_QuestDlgFunc"},
	{GD_WarehouseFunc,	"GD_WarehouseFunc"	},
	{GD_DlgFunc,		"GD_DlgFunc"		},
	{GDREVIVAL_DlgFunc,	"GDREVIVAL_DlgFunc"	},
	{GDLEVEL_DlgFunc,	"GDLEVEL_DlgFunc"	},
	{GDRANK_DlgFunc,	"GDRANK_DlgFunc"	},
	{GDMARK_DlgFunc,	"GDMARK_DlgFunc"	},
	{GDCREATE_DlgFunc,	"GDCREATE_DlgFunc"	},
	{GDINVITE_DlgFunc,	"GDINVITE_DlgFunc"	},
	{GDWHRANK_DlgFunc,	"GDWHRANK_DlgFunc"	},
	{GDUnion_DlgFunc,	"GDUnion_DlgFunc"	},
	{GDNICK_DlgFunc,	"GDNICK_DlgFunc"	},

	{PROGRESS_DlgFunc, "PROGRESS_DlgFunc" },

	{FD_DlgFunc, "FD_DlgFunc"},

	// guildfieldwar
	{GFW_DlgFunc, "GFW_DlgFunc" },
	// Guild Tournament
	{GDT_DlgFunc, "GDT_DlgFunc"},
	{CG_DlgFunc, "CG_DlgFunc"},
	// 080916 LUJ, 합성 창 실행 함수
	{ComposeDialogFunc, "ComposeDialogFunc" },
	// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
	{RFGUIDE_DLGFunc, "RFGUIDE_DLGFunc"},
	{GN_DlgBtnFunc, "GN_DlgBtnFunc"},
	{AN_DlgBtnFunc, "AN_DlgBtnFunc"},
	// 06. 03. 문파공지 - 이영준
	{GNotice_DlgBtnFunc, "GNotice_DlgBtnFunc"},

	//060802 GuildPlusTime - wonju
	{GuildPlusTime_DlgFunc, "GuildPlusTime_DlgFunc"},
	{ CM_CMNCbFunc, "CM_CMNCbFunc" },
	{ MDS_CBFunc, "MDS_CBFunc" },
	{ CharMain_BtnFunc, "CharMain_BtnFunc" },
	{JO_DlgFunc, "JO_DlgFunc"},
	{DMD_DateMatchingDlgFunc, "DMD_DateMatchingDlgFunc"},			// 매칭 다이얼로그 테스트. 2007/03/15 이진영
	{DMD_PartnerDlgFunc, "DMD_PartnerDlgFunc"}, 
	{DMD_IDT_Func, "DMD_IDT_Func"}, 
	{DMD_FID_Func, "DMD_FID_Func"},
	{DMD_MID_Func, "DMD_MID_Func"}, 
	{DMD_RID_Func, "DMD_RID_Func"}, 
	{KS_Func, "KS_Func"}, 

	{SkillTrainDlgFunc, "SkillTrainDlgFunc"},
	{FD_GuildDlg,			"FD_GuildDlg"},
	{FARM_BUY_DlgFunc,		"FARM_BUY_DlgFunc"},
	{FARM_UPGRADE_DlgFunc,	"FARM_UPGRADE_DlgFunc"},
	{FARM_MANAGE_DlgFunc ,	"FARM_MANAGE_DlgFunc"},
	{TTR_DlgFunc, "TTR_DlgFunc"}, 
	{TTR_BTNDlgFunc, "TTR_BTNDlgFunc"}, 
	{HD_DlgFunc, "HD_DlgFunc"}, 
	{MapMove_DlgFunc, "MapMove_DlgFunc"}, 
	{DATE_ZONE_LIST_DlgFunc ,		"DATE_ZONE_LIST_DlgFunc"},
	{CHALLENGE_ZONE_LIST_DlgFunc ,	"CHALLENGE_ZONE_LIST_DlgFunc"},
	{CHALLENGE_ZONE_LIST_DlgFunc ,	"CHALLENGE_ZONE_CLEARNO1_DlgFunc"},
	{Change_Name_Func, "Change_Name_Func"}, 
	{NumberPad_DlgFunc, "NumberPad_DlgFunc"}, 
	{AutoNoteDlg_Func, "AutoNoteDlg_Func"},
	{AutoAnswerDlg_Func, "AutoAnswerDlg_Func"},
	{StoreSearchDlg_Func, "StoreSearchDlg_Func"},
	{Shout_DlgBtnFunc, "Shout_DlgBtnFunc"},
	{BodyChangeFunc, "BodyChangeFunc"},
	{ChatRoomMainDlg_Func,		"ChatRoomMainDlg_Func"},
	{ChatRoomCreateDlg_Func,	"ChatRoomCreateDlg_Func"},
	{ChatRoomDlg_Func,			"ChatRoomDlg_Func"}, 
	{ChatRoomGuestListDlg_Func, "ChatRoomGuestListDlg_Func"}, 
	{ChatRoomJoinDlg_Func,		"ChatRoomJoinDlg_Func"}, 
	{ChatRoomOptionDlg_Func,	"ChatRoomOptionDlg_Func"},
	{ChatRoomMainDlg_Func,		"ChatRoomMainDlg_Func"},
	{Fishing_DlgBtnFunc, "Fishing_DlgBtnFunc"},
	{Animal_DlgBtnFunc,	"Animal_DlgBtnFunc"},

	{PET_STATUS_DLG_FUNC, "PET_STATUS_DLG_FUNC"},
	{PET_RES_DLG_FUNC, "PET_RES_DLG_FUNC"},
	
	{PopupMenuDlgFunc, "PopupMenuDlgFunc" },

	{COOK_DLG_FUNC, "COOK_DLG_FUNC"},
	// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가작업
	{RidePopupMenuDlgFunc, "RidePopupMenuDlgFunc" },

	//090323 pdy 하우징 설치 UI추가
	{HousingRevolDlgFunc, "HousingRevolDlgFunc"},

	//090330 pdy 하우징 창고 UI추가
	{Housing_WarehouseDlg_Func, "Housing_WarehouseDlg_Func" },

	//090409 pdy 하우징 하우스 검색 UI추가
	{HouseSearchDlgFunc,"HouseSearchDlgFunc"},

	//090410 pdy 하우징 하우스 이름설정 UI추가
	{HouseNameDlgFunc,"HouseNameDlgFunc"},

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	{HousingDecoPointDlgFunc,"HousingDecoPointDlgFunc"},
	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	{HousingActionPopupMenuDlgFunc,"HousingActionPopupMenuDlgFunc"},

	//090525 pdy 하우징 내집창고 버튼 UI추가
	{HousingWarehouseButtonDlgFunc,"HousingWarehouseButtonDlgFunc"},

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	{InputNameDlgFunc, "InputNameDlgFunc"},

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	{HousingDecoModeBtDlgFunc,"HousingDecoModeBtDlgFunc"},

	//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경 
	{HousingMainPointDlgFunc,"HousingMainPointDlgFunc"},

	//091223 NYJ 하우징 다른집방문
	{HousingSearchDlgFunc,"HousingSearchDlgFunc"},

	// 091210 ShinJS --- 녹화 Dialog 추가
	{VideoCaptureDlgFunc, "VideoCaptureDlgFunc"},

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	{AdditionalButtonDlgFunc, "AdditionalButtonDlgFunc"},

	// 100511 ONS 전직변경 다이얼로그 추가
	{ChangeClassDlgFunc, "ChangeClassDlgFunc"},
	// 100503 NYJ 판매대행
	{ConsignmentDlgFunc, "ConsignmentDlgFunc"},
	// 100629 몬스터타겟 다이알로그
	{MonsterTargetDlgFunc,"MonsterTargetDlgFunc"},
	// 100709 ONS 퀘스트알리미 삭제버튼 처리 추가
	{QuickView_DlgFunc, "QuickView_DlgFunc"},
	// 100628 ShinJS 월드맵 다이얼로그 추가
	{WorldMapDlgFunc, "WorldMapDlgFunc"},
	{BigMapDlgFunc, "BigMapDlgFunc"},
	{NULL, ""},	//마지막줄에 이거 꼭 놔두세요.
};

int FUNCSEARCH(char * funcName)
{
	for(int i = 0 ;; i++)
	{
		if(g_mt_func[i]._func == NULL)
			break;
		if(strcmp(funcName, g_mt_func[i]._funcName)==0)
			return i;
	}

	return -1; 
}

void CS_BtnFuncChangeServer( LONG lId, void* p, DWORD we )
{
	CHARSELECT->BackToMainTitle();
	TITLE->SetServerList();
	CAMERA->ResetInterPolation();
}

void CS_BtnFuncLogOut(LONG lId, void* p, DWORD we)
{
	PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
}
void CS_BtnFuncCreateChar(LONG lId, void* p, DWORD we)
{
	if(CHARSELECT->IsFull() == FALSE)
	{
		MAINGAME->SetGameState(eGAMESTATE_CHARMAKE);
		CAMERA->MoveStart(eCP_CHARMAKE);
	}
	else
	{
		CHARSELECT->DisplayNotice(17);
	}
}

void		CS_BtnFuncFirstChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(0);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncSecondChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(1);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncThirdChar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(2);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncFourthchar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(3);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void		CS_BtnFuncFifthchar(LONG lId, void* p, DWORD we)
{
	if( we & WE_PUSHDOWN )
		CHARSELECT->SelectPlayer(4);
	else if( we & WE_PUSHUP )
		((cPushupButton*)((cDialog*)p)->GetWindowForID(lId))->SetPush( TRUE );
}
void CS_BtnFuncEnter(LONG lId, void* p, DWORD we)
{
	if(	CHARSELECT->GetCurSelectedPlayer() == NULL)
		CHARSELECT->DisplayNotice(18);
	else
	{
		CHARSELECT->SendMsgGetChannelInfo();
		CHARSELECT->SetDisablePick(TRUE);
	} 
}

void CS_FUNC_OkISee(LONG lId, void* p, DWORD we)	 
{
	cStatic * CharacterSelectMessageDlg = (cStatic *)WINDOWMGR->GetWindowForIDEx(CSID_MessageBox);
	CharacterSelectMessageDlg->SetActive(FALSE);
}

void CS_BtnFuncDeleteChar(LONG lId, void* p, DWORD we)
{
	int nChar = CHARSELECT->GetCurSelectedPlayerNum();
	int MaxChar = MAX_CHARACTER_NUM-1;

	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
	LONG Xpos = (LONG)(dispInfo.dwWidth/2 - 100) ;	// 100 is half of msgbox's width.
	LONG Ypos = (LONG)(dispInfo.dwHeight/2 - 57) ;	// 57 is half of msgbox's height.

	// 070205 LYW --- Add a conditional sentence.
	if( nChar >= 0 && nChar <= MaxChar )
	{
		cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_DELETECHAR, MBT_YESNO, RESRCMGR->GetMsg( 267 ) );

		if( pMsgBox )
			pMsgBox->SetAbsXY( Xpos, Ypos );
	}
	else
	{
		CHARSELECT->DisplayNotice( 20 );
	}

	CHARSELECT->SetDisablePick( TRUE );
}

// 061219 LYW --- Add function to process events from certificate dialog.
void CTF_CBFunc( LONG lId, void* p, DWORD we )
{
	CHARSELECT->GetCertificateDlg()->OnActionEvent( lId, p, we ) ;
}

extern void	CM_CharMakeBtnFunc(LONG lId, void* p, DWORD we)
{
	cEditBox * editboxName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(CM_IDEDITBOX);
	char* pName = editboxName->GetEditText(); 
	int nLen	= strlen(pName);

	if( nLen == 0 )
	{
		// 070125 LYW --- CharMake : Modified message number.
		//CHARMAKE->DisplayNotice( 11 );
		CHARMAKE->DisplayNotice( 11 );
		return;
	}
	else if( nLen < 4 )
	{
		CHARMAKE->DisplayNotice( 19 );
		return;
	}

	if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)pName) )
	{
		CHARMAKE->DisplayNotice( 14 );
		return;
	}

	if( !FILTERTABLE->IsUsableName(pName) )
	{
		CHARMAKE->DisplayNotice( 14 );
		// 061219 LYW --- Setting to null editbox.
		cEditBox* pEdit = ( cEditBox* )CHARMAKE->GetCharMakeNewDlg()->GetWindowForID( CM_IDEDITBOX ) ;
		pEdit->SetEditText("") ;
		return;
	}

	CHARACTERMAKEINFO msg;
	ZeroMemory(&msg, sizeof(CHARACTERMAKEINFO)) ;
	CHARMAKEMGR->GetCharMakeNewDlg()->ApplyOption( &msg ) ;

	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHARACTER_MAKE_SYN;
	msg.StandingArrayNum = BYTE(-1);
	strcpy(msg.Name, editboxName->GetEditText());

	NETWORK->Send(&msg,sizeof(msg));

	CHARMAKE->SetDisableDlg( TRUE );
	//	CHARMAKE->GetCharMakeDialog()->SetDisable( TRUE );
}

extern void	CM_CharCancelBtnFunc(LONG lId, void* p, DWORD we)
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_DIRECTCHARACTERLIST_SYN;
	NETWORK->Send(&msg,sizeof(msg));	

	//
	CAMERA->MoveStart(eCP_CHARSEL);
}

extern char g_szHeroIDName[];
extern char g_CLIENTVERSION[];

//=========================================================================
//	NAME : MT_LogInOkBtnFunc
//	DESC : The function to management action event of all controls in main title. 080109 LYW
//=========================================================================
void MT_LogInOkBtnFunc(LONG lId, void* p, DWORD we)
{
	ScriptCheckValue check;
	SetScriptCheckValue( check );

	if( check.mValue != g_Check.mValue )
	{
#ifndef _GMTOOL_
		// 090120 LUJ, 패치 버전을 최초로 돌린다
		{
			const char* const	versionFile = "LunaVerInfo.ver";
			FILE* const			file		= fopen( versionFile, "r+" );
			if( file )
			{
				// 090109 LUJ, 4글자로 구성된 버전 헤더를 파일에서 읽어와 문자열 변수에 담는다
				const size_t headerSize = 4;
				char header[ headerSize + 1 ] = { 0 };
				fread(
					header,
					headerSize,
					sizeof( *header ),
					file );

				// 090109 LUJ, 전체 패치를 수행할 버전 번호를 만든다
				char versionText[ MAX_PATH ] = { 0 };
				sprintf(
					versionText,
					"%s00000000",
					header );
				// 090109 LUJ, 패치 버전 문자열을 파일에 쓴다
				fseek(
					file,
					0,
					SEEK_SET );
				fwrite(
					versionText,
					sizeof( *versionText ),
					strlen( versionText ),
					file );
				fclose( file );
			}
		}

		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		return ;
#endif
	}

	cDialog * dlg = (cDialog*)p;

	if( !dlg ) return;

	cNumberPadDialog* pPad = ((cNumberPadDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG ));

	if( !pPad ) return ;														// 숫자 패드의 무결성을 체크하자.

	cEditBox * editboxId = NULL ;												// 아이디 에디트 박스를 받는다.
	editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX) ;					

	if( !editboxId ) return ;													// 아이디 에디트 박스의 무결성을 체크하자.

	cEditBox * editboxPwd = NULL ;												// 패스워드 에디트 박스를 받는다.
	editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX) ;				

	if( !editboxPwd ) return ;													// 패스워드 에디트 박스의 무결성을 체크하자.

	cDialog* pIDDlg = NULL ;
	pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;							// 로그인 다이얼로그를 받는다.

	if( !pIDDlg ) return ;														// 로그인 다이얼로그의 무결성을 체크한다.

	cMsgBox* pMsgBox = NULL ;													

	//--------------------------------------------------------------------------

	char * userID = editboxId->GetEditText() ;									// 아이디를 받는다.
	char * userPWD = editboxPwd->GetEditText() ;								// 패스워드를 받는다.

	//------------------------------------------------------------------------- // 아이디와 비밀 번호를 제대로 입력 했다면,
#ifdef _KOR_WEBLAUNCHER_
	if( strlen( pPad->GetNumberStr() ) > 0 )
#else
	if((strcmp(userID, "") != 0) && (strcmp(userPWD, "") != 0))					
#endif
	{
		MSG_LOGIN_SYN msg ;														// 로그인 정보를 담을 메시지 구조체를 선언한다.
		memset(&msg, 0, sizeof(MSG_LOGIN_SYN)) ;								// 메시지 구조체를 초기화 한다.

		msg.Category = MP_USERCONN ;											// 카테고리와 프로토콜을 세팅한다.
		msg.Protocol = MP_USERCONN_LOGIN_SYN ;

		SafeStrCpy(msg.id, userID, MAX_NAME_LENGTH + 1 ) ;						// 아이디 세팅.
		SafeStrCpy(msg.pw, userPWD, MAX_NAME_LENGTH + 1 ) ;						// 비밀번호 세팅.

		// 보안 비밀번호 추가 처리.
		strcpy(msg.strSecurityPW, pPad->GetNumberStr()) ;

		SafeStrCpy(msg.Version,g_CLIENTVERSION, 16 ) ;							// 클라이언트 버전 세팅.

		msg.AuthKey = TITLE->GetDistAuthKey() ;									// 인증키를 담는다.
		
		msg.Check.mValue = check.mValue;

		SafeStrCpy(
			msg.mLoginKey,
			TITLE->GetLoginKey().c_str(),
			_countof(msg.mLoginKey));

		NETWORK->Send(&msg,sizeof(msg)) ;										// 메시지를 전송한다.

//---KES 로그인 실패후 재 로그인이 잘 안되던 것 수정
		NETWORK->m_bDisconnecting = FALSE;
//---------------------------------------------

		SafeStrCpy(g_szHeroIDName, userID, MAX_NAME_LENGTH + 1 ) ;				// 아이디를 받는다.

		pMsgBox = WINDOWMGR->MsgBox( MBI_WAIT_LOGINCHECK,						// 로그인 중이라는 메시지 박스를 받는다.
								 MBT_NOBTN, 
								 RESRCMGR->GetMsg( 265 ) ) ;

		if( pMsgBox )
		{
			float fXpos = pIDDlg->GetAbsX() ;									// X좌표를 받는다.
			float fYpos = pIDDlg->GetAbsY() + pIDDlg->GetHeight() ;				// Y좌표를 받는다.

			pMsgBox->SetAbsXY( fXpos, fYpos ) ;									// 메시지 박스의 위치를 세팅한다.
		}

		TITLE->StartWaitConnectToAgent( TRUE ) ;								// 에이전트 연결을 기다린다.

		USERINFOMGR->SetUserID( userID ) ;										// 유저 아이디를 설정한다.
	}
	else	
	{
		cMsgBox* pMsgBox = WINDOWMGR->MsgBox(
			MBI_LOGINEDITCHECK, 
			MBT_OK, 
#ifdef _KOR_WEBLAUNCHER_
			CHATMGR->GetChatMsg(1478)); 	// "비밀번호를 올바르게 입력해 주세요."
#else
			CHATMGR->GetChatMsg(208));
#endif


		if( pMsgBox )
		{
			float fXpos = pIDDlg->GetAbsX() ;									// X좌표를 받는다.
			float fYpos = pIDDlg->GetAbsY() + pIDDlg->GetHeight() ;				// Y좌표를 받는다.

			pMsgBox->SetAbsXY( fXpos, fYpos ) ;									// 메시지 박스의 위치를 세팅한다.
		}
	}

	editboxId->SetFocusEdit(FALSE) ;											// 아이디, 비번 에디트 박스의 포커스를 해제 한다.
	editboxPwd->SetFocusEdit(FALSE) ;
	dlg->SetDisable( TRUE ) ;													// 아이디, 비번 창을 비활성화 한다.
}

void MT_DynamicBtnFunc(LONG lId, void * p, DWORD we)
{
	cDialog* dlg = (cDialog*)p;

	if( lId == MT_DYNAMICOKBTN )
	{
		cEditBox* editboxPwd = (cEditBox*)dlg->GetWindowForID(MT_DYNAMICPWDEDITBOX);
		char* userPWD = editboxPwd->GetEditText();
		if( strcmp(userPWD, "") != 0 )
		{
			MSG_LOGIN_DYNAMIC_SYN msg;
			msg.Category = MP_USERCONN;
			msg.Protocol = MP_USERCONN_LOGIN_DYNAMIC_SYN;
			msg.AuthKey = TITLE->GetDistAuthKey();
			SafeStrCpy( msg.id, g_szHeroIDName, MAX_NAME_LENGTH + 1 );
			SafeStrCpy( msg.pw, userPWD, MAX_NAME_LENGTH + 1 );
			NETWORK->Send( &msg, sizeof(msg) );

			cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
			if( pMsgBox )
			{
				pMsgBox->SetDisable( FALSE );
				pMsgBox->SetActive( TRUE );
				cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( pIDDlg )
				{
					pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
				}
			}

			TITLE->StartWaitConnectToAgent( TRUE );
		}
		else
		{
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox(MBI_CANTUSEDYNAMIC, MBT_OK, CHATMGR->GetChatMsg(208) ); 
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );		
			}
		}

		editboxPwd->SetFocusEdit(FALSE);
		dlg->SetDisable( TRUE );
	}
	else if( lId == MT_DYNAMICEXITBTN )
	{
		((cEditBox*)dlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetEditText( "" );

		dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
		((cEditBox*)dlg->GetWindowForID(MT_IDEDITBOX))->SetEditText( "" );
		((cEditBox*)dlg->GetWindowForID(MT_PWDEDITBOX))->SetEditText( "" );

		TITLE->NoDisconMsg();
		TITLE->ShowServerList();
	}
}

void MT_DynamicEditReturnFunc( LONG lId, void* p, DWORD we )
{
	if(we == WE_RETURN)
	{
		if( lId == MT_DYNAMICPWDEDITBOX )
		{
			MT_DynamicBtnFunc( MT_DYNAMICOKBTN, p, we );
		}
	}
}

void MT_EditReturnFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		cDialog* dlg = (cDialog*)p;
		cEditBox* pEdit;
		if( lId == MT_IDEDITBOX )
		{
			pEdit = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
			if( *pEdit->GetEditText() != 0 )
			{
				WINDOWMGR->SetNextEditFocus();
			}
		}
		else if( lId == MT_PWDEDITBOX )
		{
			pEdit = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
			if( *pEdit->GetEditText() != 0 )
			{
				MT_LogInOkBtnFunc( MT_OKBTN, p, WE_BTNCLICK );
			}
		}
	}
}

void MT_ExitBtnFunc(LONG lId, void * p, DWORD we)
{
	cDialog* dlg = (cDialog *)p;
	cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
	cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);

	editboxId->SetEditText("");
	editboxPwd->SetEditText("");
	editboxId->SetFocusEdit(TRUE);
	editboxPwd->SetFocusEdit(FALSE);

	TITLE->NoDisconMsg();
	TITLE->ShowServerList();
}

////////////////////////////////////// Main Interface //////////////////////////////////////////
void MI_CharBtnFunc(LONG lId, void* p, DWORD we)
{
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetCharacterDialog()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetCharacterDialog()->UpdateData();
		GAMEIN->GetCharacterDialog()->SetActive(TRUE);
	}
}
void MI_ExchangeBtnFunc(LONG lId, void* p, DWORD we)
{
	static BOOL run = 0;
	if(run)
	{
		MSGBASE msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_WALKMODE;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		run = 0;
		MOVEMGR->SetWalkMode(HERO);
	}
	else
	{
		MSGBASE msg;
		msg.Category = MP_MOVE;
		msg.Protocol = MP_MOVE_RUNMODE;
		msg.dwObjectID = HEROID;
		NETWORK->Send(&msg,sizeof(msg));
		run = 1;
		MOVEMGR->SetRunMode(HERO);
	}
}

void MI_InventoryBtnFunc(LONG lId, void* p, DWORD we)
{
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetInventoryDialog()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetInventoryDialog()->SetActive(TRUE);
	}
}

void MI_SkillBtnFunc(LONG lId, void* p, DWORD we)
{
	//MessageBox(NULL,"¡§Io¡E￠ci¡E￠c¡§￠®¡§I¡§¡IA¡E￠cE Aa¡E￠c¡E￠￥A",NULL,NULL);
	if(we == WE_PUSHUP)
	{
		GAMEIN->GetSkillTreeDlg()->SetActive(FALSE);
	}
	else if(we == WE_PUSHDOWN)
	{
		GAMEIN->GetSkillTreeDlg()->SetActive(TRUE);
	}

}

void IN_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_LBTNCLICK ||/* we == WE_RBTNCLICK || */we == WE_RBTNDBLCLICK || we == WE_LBTNDBLCLICK || we == WE_RBTNDBLCLICK || WE_RBTNCLICK)
	{
		GAMEIN->GetInventoryDialog()->OnActionEvnet(lId, p, we);
	}
}

void STD_SkillTreeDlgFunc(LONG lId, void * p, DWORD we)
{
	if( lId == STD_SKILLTRAININGBTN && we & WE_BTNCLICK )
	{
		GAMEIN->GetSkillTrainingDlg()->OpenDialog();
	}
	else if( lId == CMI_CLOSEBTN )
	{
		GAMEIN->GetSkillTreeDlg()->SetActive( FALSE );
	}
	else if( p )
	{
		( ( cJobSkillDlg* )p )->OnActionEvnet(lId, p, we);
	}
}

void CI_DlgFunc(LONG lId, void * p, DWORD we)
{
	if( lId == CI_CLOSEBTN )
	{
		GAMEIN->GetCharacterDialog()->SetActive( !( GAMEIN->GetCharacterDialog()->IsActive() ) );
		// 070115 LYW --- Close tree dialog.
		GAMEIN->GetCharacterDialog()->SetViewTreeDlg( FALSE ) ;
	}
}
void CI_AddExpPoint(LONG lId, void * p, DWORD we)
{
	BYTE whats=STR_POINT;
	switch(lId)
	{
	case CI_STATUS_BTN_0:
		whats=STR_POINT;
		break;
	case CI_STATUS_BTN_1:
		whats=DEX_POINT;
		break;
	case CI_STATUS_BTN_2:
		whats=VIT_POINT;
		break;
	case CI_STATUS_BTN_3:
		whats=INT_POINT ;
		break;
	case CI_STATUS_BTN_4 :
		whats=WIS_POINT;
		break ;
	}

	if( lId==CI_STATUS_BTN_0 || lId==CI_STATUS_BTN_1 || lId==CI_STATUS_BTN_2 || lId==CI_STATUS_BTN_3 || lId==CI_STATUS_BTN_4 )
		GAMEIN->GetCharacterDialog()->OnAddPoint(whats);
}

// 070111 LYW --- Add function to process events.
void CI_ActionEventFun( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetCharacterDialog()->OnActionEvent( lId, p, we ) ;
}

void SSI_TitleEditBoxFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		// A|￠®E￠®ⓒ­n￠®E?￠®E¡Ec ￠®E￠®IeCN ￠®E?¡§Io￠®E?UA¡§I¨I¨￡￠®E￠®ⓒ­￠®Ec CO¡E￠c¡§￠®I..
		STREETSTALLMGR->EditTitle();
	}
}

void SSI_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStreetStallDialog()->OnActionEvnet(lId, p, we);
}

void SSI_BuyBtnFunc(LONG lId, void * p, DWORD we)
{
	CStreetStall* pStallDlg = GAMEIN->GetStreetStallDialog();

	if( pStallDlg->GetCurSelectedItemNum() == -1 ) return;

	if( we == WE_BTNCLICK )
	{

		if( !pStallDlg->SelectedItemCheck() ) return;

		// FOR FIND_SS_BUG
		if( ITEMMGR->IsDupItem( pStallDlg->GetCurSelectedItemIdx() ) ) // ￠®E?￠®Ii￠®¡×uaAI¡§I¨I¨￡￠®¡×￠®I ￠®¡×¡E￠c?¡E￠cie¡§Io¡E￠c¡§￠® ￠®¡×u￠®¡×¡E￠cAIAUAI ¡E￠c¡§￠®¡§I¡E￠c￠®E?i
		{
			if( pStallDlg->GetCurSelectedItemDur() == 1 )
				STREETSTALLMGR->BuyItem();
			else
				GAMEIN->GetStreetStallDialog()->ShowDivideBox();
		}
		else
		{
			STREETSTALLMGR->BuyItem();
		}
	}
}

void SSI_EditBtnFunc(LONG lId, void * p, DWORD we)
{
	if( GAMEIN->GetStreetStallDialog()->GetCurSelectedItemNum() == -1 ) return;

	CStreetStall* pStall = (CStreetStall*)p;
	if(we == WE_BTNCLICK)
	{

		CExchangeItem* pExItem = pStall->FindItem( pStall->GetCheckDBIdx() );
		if( pExItem == NULL ) return;

		//		if( !GAMEIN->GetStreetStallDialog()->SelectedItemCheck() ) return;
		//락이 걸려있나 안있나만 보자..
		if( pExItem->IsLocked() == TRUE ) return;

		STREETSTALLMGR->ItemStatus( pExItem, FALSE );
	}
}

// LYJ 051017 구입노점상 추가
void SO_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStallKindSelectDialog()->OnActionEvnet(lId, p, we);
}

void BS_TitleEditBoxFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_RETURN)
	{
		STREETSTALLMGR->EditTitle();
	}
}

void BS_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetStreetBuyStallDialog()->OnActionEvnet(lId, p, we);
}

void BS_SellBtnFunc(LONG lId, void * p, DWORD we)
{
	CStreetBuyStall* pStallBuyDlg = GAMEIN->GetStreetBuyStallDialog();

	if( pStallBuyDlg->GetCurSelectedItemNum() == -1 ) return;

	if( we == WE_BTNCLICK )
	{

		if( !pStallBuyDlg->SelectedItemCheck() ) return;

		STREETSTALLMGR->BuyItem();
	}
}

void BRS_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->OnActionEvnet(lId, p, we);
}

void BRS_CloseFunc(LONG lId, void * p, DWORD we)
{
	MSGBASE msg;
	msg.Category = MP_STREETSTALL;
	msg.dwObjectID = HEROID;
	msg.Protocol = MP_STREETSTALL_UPDATEEND_SYN;

	NETWORK->Send( &msg, sizeof(MSGBASE) );
	GAMEIN->GetBuyRegDialog()->Close();
}

void BRS_DeleteFunc(LONG lId, void * p, DWORD we)
{
	CBuyItem* pItem = (CBuyItem*)GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItem();
	POSTYPE pos = GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItemNum();

	//선택된 곳에 아이템이 있었다면 삭제한다
	if( pos != -1 && pItem )
		GAMEIN->GetStreetBuyStallDialog()->FakeDeleteItem(pos);

	GAMEIN->GetBuyRegDialog()->Close();
}


void BRS_TypeListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateType();
}

void BRS_ItemListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateItem();
}

void BRS_ClassListFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetBuyRegDialog()->UpdateClass();
}

void BRS_REGBtnFunc(LONG lId, void * p, DWORD we)
{
	ITEM_INFO* pItem = GAMEIN->GetBuyRegDialog()->GetSelectItemInfo();

	if(!pItem)
		return;

	BUY_REG_INFO RegInfo;

	if( !GAMEIN->GetBuyRegDialog()->GetBuyRegInfo(RegInfo) )
		return;

	GAMEIN->GetStreetBuyStallDialog()->FakeRegistItem(RegInfo, pItem);
	GAMEIN->GetBuyRegDialog()->Close();
}

void NSI_HyperLinkFunc(LONG lId, void * p, DWORD we)
{
	cListDialogEx* pListDlg = (cListDialogEx*)((cDialog*)p)->GetWindowForID( lId );
	int nIdx = pListDlg->GetCurSelectedRowIdx();
	if( nIdx == -1 ) return;

	cNpcScriptDialog* pDlg = GAMEIN->GetNpcScriptDialog() ;

	if( pDlg )
	{
		if( we == WE_BTNCLICK )
		{
			pDlg->OnActionEvent( lId, p, we ) ;
		}

		if( lId == NSI_TOPBTN )
		{
			return;
		}
		else if(ITEM* const item = pListDlg->GetItem(pListDlg->GetCurSelectedRowIdx()))
		{
			pDlg->HyperLinkParser(
				item->string,
				item->dwData);
		}

		WINDOWMGR->SetMouseInputProcessed();	
	}
}

void CMI_MoneyOkFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetMoneyDialog()->OkPushed();
}

/*¡E￠cioA￠®E¡EcA¡E￠cE*/
void DE_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(we == WE_CLOSEWINDOW)
	{
		CDealDialog* pDlg = GAMEIN->GetDealDialog();

		if( pDlg && pDlg->IsShow() )
		{
			pDlg->SetActive(FALSE);
		}
	}

	if(we == WE_LBTNCLICK)
	{
		GAMEIN->GetDealDialog()->OnSelectedItem();
	}
	else if(/*we == WE_LBTNCLICK || we == WE_RBTNCLICK || we == WE_RBTNDBLCLICK ||*/ we == WE_LBTNDBLCLICK)
	{
		GAMEIN->GetDealDialog()->OnActionEvnet(lId, p, we);
	}
}
void DE_BuyItemFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->OnBuyPushed();
}
void DE_SellItemFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->OnSellPushed();
}
// 070503 LYW --- GlobalEventFunc : Add function to cancel sell dialog.
void DE_CancelFunc(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetDealDialog()->SetActive(FALSE) ;
}
void DE_CloseHideBtn(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetDealDialog()->HideDealer();
}

//////////////////////////////////////////////////////////////////////////
//*￠®¡×¡E￠cA￠®¡×¡E￠c￠®¡×uA¡E￠cE*/
void PA_BtnFunc(LONG lId, void * p, DWORD we) 
{
	switch(lId) 
	{		
	case PA_SECEDEBTN: //A¡E￠ciA¡§I￠®I 
		{
			if( HEROID != PARTYMGR->GetMasterID() )
				WINDOWMGR->MsgBox( MBI_PARTY_SECEDE, MBT_YESNO, CHATMGR->GetChatMsg(400));	
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1042)) ;
			}
		}	
		break;

	case PA_FORCEDSECEDEBTN: //¡E￠c¡§￠®¡E￠c¡§IA|A¡E￠ciA¡§I￠®I
		{
			PARTYMGR->BanPartyMemberSyn(
				GAMEIN->GetPartyDialog()->GetClickedMemberID());
		}
		break;

	case PA_TRANSFERBTN: //￠®E￠®ⓒ­￠®Ec￠®E￠®Io¡E￠c¡§uC AI￠®¡×uc
		{
			if(gHeroID != PARTYMGR->GetMasterID())
			{
				break;
			}

			CPartyDialog* const pPartyDlg = GAMEIN->GetPartyDialog();

			if(0 == pPartyDlg)
			{
				break;
			}
			else if(pPartyDlg->GetClickedMemberID() == gHeroID)
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(1300));
				break;
			}
            
			PARTYMGR->ChangeMasterPartySyn(
				gHeroID,
				pPartyDlg->GetClickedMemberID());
		}
		break;

	case PA_BREAKUPBTN: //C￠®¡×¡§¡I¡E￠cie
		{
			if( HEROID != PARTYMGR->GetMasterID() )	return;

			if(PARTYMGR->IsProcessing() == TRUE)
				return;
			WINDOWMGR->MsgBox(MBI_PARTYBREAKUP, MBT_YESNO, CHATMGR->GetChatMsg(651));
		}
		break;

	case PA_ADDPARTYMEMBERBTN: //￠®E￠®ⓒ­a¡§Ioo A¡§I￠®ⓒ­¡E￠c¡§￠®￠®E¡Ec
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();
			if(!pObject)
			{
				// 090528 ShinJS --- 선택된 오브젝트가 없는 경우 파티초대 이름입력창을 띄운다
				GAMEIN->GetInputNameDlg()->SetActive( TRUE );
				return;
			}
			else 
			{
				if( pObject->GetID() == HEROID )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1269 ) );
					return ;
				}

				if( PARTYMGR->IsPartyMember(pObject->GetID()) )
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1038)) ;
					return ;
				}

				if(pObject->GetObjectKind() == eObjectKind_Player)
					PARTYMGR->AddPartyMemberSyn(OBJECTMGR->GetSelectedObject()->GetID());
				else
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(18));
			}
		}
		break;
	case PA_CREATEOK:
		{
			GAMEIN->GetPartyCreateDialog()->SetActive(
				FALSE);

			PARTY_OPTION partyOption = ePartyOpt_Damage;

			if(cComboBox* pDistirbute = (cComboBox*)WINDOWMGR->GetWindowForIDEx(PA_CREATECOMBOX))
			{
				if(0 == _tcsicmp(RESRCMGR->GetMsg(434), pDistirbute->GetComboText()))
				{
					partyOption = ePartyOpt_Damage;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(432), pDistirbute->GetComboText()))
				{
					partyOption = ePartyOpt_Sequence;
				}
			}

			BYTE distributionOption = 0;
			
			if(cComboBox* pDiceGrade = (cComboBox*)WINDOWMGR->GetWindowForIDEx(PA_DICEGRADECOMBOX))
			{
				if(0 == _tcsicmp(RESRCMGR->GetMsg(1194), pDiceGrade->GetComboText()))
				{
					distributionOption = 0;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1182), pDiceGrade->GetComboText()))
				{
					distributionOption = 1;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1183), pDiceGrade->GetComboText()))
				{
					distributionOption = 2;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1184), pDiceGrade->GetComboText()))
				{
					distributionOption = 3;
				}
				else if(0 == _tcsicmp(RESRCMGR->GetMsg(1185), pDiceGrade->GetComboText()))
				{
					distributionOption = 4;
				}
			}

			MSG_BYTE2 message;
			ZeroMemory(
				&message,
				sizeof(message));
			message.Category = MP_PARTY;
			message.Protocol = MP_PARTY_CREATE_SYN;
			message.bData1 = partyOption;
			message.bData2 = distributionOption;
			NETWORK->Send(
				&message,
				sizeof(message));

			PARTYMGR->SetIsProcessing(
				TRUE);
		}
		break;
	case PA_CREATECANCEL:
		{
			GAMEIN->GetPartyCreateDialog()->SetActive(FALSE);
		}
		break;
	case PA_INVITEOK:
		{
			GAMEIN->GetPartyInviteDialog()->SetActive(FALSE);
			PARTYMGR->PartyInviteAccept();
		}
		break;
	case PA_INVITECANCEL:
		{
			PARTYMGR->PartyInviteDeny();
			GAMEIN->GetPartyInviteDialog()->SetActive(FALSE);
		}
		break;
	case PA_WARSUGGESTBTN:
		{
			CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(528));
			return;
		}
		break;
	case PA_DISTRIBUTE:
		{
			if(CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG ))
			{
				cComboBox* pDistribute = pPartySet->GetDistribute() ;
				if( !pDistribute ) return ;

				if( we==WE_PUSHDOWN && HEROID!=PARTYMGR->GetMasterID())
				{
					pDistribute->ForceListFold();
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1116)) ;
					return ;
				}
				else if( we != WE_COMBOBOXSELECT )
				{
					return;
				}

				PARTYMGR->SetPickupItem(pDistribute->GetCurSelectedIdx()) ;
			}
		}
		break ;

	case PA_DICEGRADE:
		{
			if(CPartyBtnDlg* pPartySet = (CPartyBtnDlg*)WINDOWMGR->GetWindowForID( PA_BTNDLG ))
			{
				cComboBox* pDiceGrade = pPartySet->GetDiceGrade() ;
				if( !pDiceGrade ) return ;

				if( we==WE_PUSHDOWN && HEROID!=PARTYMGR->GetMasterID())
				{
					pDiceGrade->ForceListFold();
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1116)) ;
					return ;
				}
				else if( we != WE_COMBOBOXSELECT )
				{
					return;
				}

				PARTYMGR->SetDiceGrade(pDiceGrade->GetCurSelectedIdx());
			}
		}
		break ;
	// 080318 LUJ, 표시된 파티원의 버프 스킬 아이콘을 토글한다
	case PA_SKILL_ICON_BUTTON:
		{
			if(CPartyDialog* dialog = GAMEIN->GetPartyDialog())
			{
				dialog->OnActionEvent( lId, p, we );
			}

			break;
		}
	}
}

// 080318 LUJ, 파티 멤버 창 이벤트 처리
void PartyMemberFunc( LONG lId, void* p, DWORD we )
{
	if(CPartyDialog* dialog = GAMEIN->GetPartyDialog())
	{
		dialog->OnActionEvent( lId, p, we );
	}
}


// ￠®E￠®IUAaA¡E￠cE ¡E￠c¡§￠®u¡E￠c¡E￠￥A
void QI_QuickDlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetQuickDlg()->OnActionEvnet(lId, p, we);
}

void PYO_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
		// 070326 LYW --- GlobalEventFunc : add close button.
	case PYO_STORAGE_CLOSE :
		{
			GAMEIN->GetStorageDialog()->ShowStorageDlg(FALSE) ;
		}
		break ;
	case PYO_STORAGELISTBTN:
		{
			GAMEIN->GetStorageDialog()->SetStorageListInfo();
			GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageListInfo);
			GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageListInfo);
			GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;

			// 길드창을 끄자
			{
				cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
				ASSERT( dialog );

				dialog->SetActive( FALSE );
			}
		}
		break;
	case PYO_BUYSTORAGEBTN:
		{
			if(HERO->GetStorageNum() >= MAX_BASESTORAGELIST_NUM)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(535) );
				return;
			}
			// 070324 LYW --- GlobalEventFunc : Modified message of messagebox.
			WINDOWMGR->MsgBox( MBI_STORAGEBUY, MBT_YESNO, CHATMGR->GetChatMsg(531), 
				HERO->GetStorageNum()+1, AddComma( GAMEIN->GetStorageDialog()->GetPrice(HERO->GetStorageNum() ) ) );
		}
		break;

	case PYO_STORAGE1BTN: case PYO_STORAGE2BTN: case PYO_STORAGE3BTN:
	case PYO_STORAGE4BTN: case PYO_STORAGE5BTN:
		{
			int num = lId-PYO_STORAGE1BTN;
			CStorageDialog* pDlg = (CStorageDialog*)GAMEIN->GetStorageDialog();
			if( !pDlg )
			{
				ASSERT(0);
				return;
			}

			cPushupButton* pstoragebtn[TAB_STORAGE_NUM];

			if( HERO->GetStorageNum() == 0 )
			{
				for(int i=0; i<TAB_STORAGE_NUM; ++i)
				{
					pstoragebtn[i] = (cPushupButton*)pDlg->GetWindowForID(PYO_STORAGE1BTN+i);
					pstoragebtn[i]->SetPush(FALSE);
				}

				return;
			}
			for(int i=0; i<TAB_STORAGE_NUM; ++i)
			{
				pstoragebtn[i] = (cPushupButton*)pDlg->GetWindowForID(PYO_STORAGE1BTN+i);
				if(i==num)
				{
					pstoragebtn[i]->SetPush(TRUE);
				}
				else
				{
					pstoragebtn[i]->SetPush(FALSE);
				}
			}
			// ¡§Ioo￠®¡×¡E￠c¡E￠c¡§￠®￠®E?￠®E¡Ec ￠®Iiu￠®EOo ￠®¡×￠®ⓒ­￠®E￠®ⓒ­￠®E?￠®¡×IAo￠®E￠®IA ￠®E￠®ⓒ­¡§I￠®I￠®¡×oA￠®E￠®ⓒ­￠®E￠®ⓒ­ ￠®¡×￠®ⓒ­?E¡E￠c¡§ICN￠®E￠®IU.
			GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1+num);
			GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1+num);
		}
		break;

	case PYO_PUTINMONEYBTN:
		{
			STORAGEMGR->PutInOutMoney(1);
		}
		break;
	case PYO_PUTOUTMONEYBTN:
		{
			STORAGEMGR->PutInOutMoney(0);
		}
		break;
	case PYO_NOBUYBTN: case PYO_NOBUYSTORAGEBTN:
		{
			GAMEIN->GetStorageDialog()->ShowStorageDlg(FALSE) ;
		}
		break;
	}
}


void Friend_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
	case FRI_PAGE1BTN: case FRI_PAGE2BTN: case FRI_PAGE3BTN: 
		{
			GAMEIN->GetFriendDialog()->SetFriendList(lId - FRI_PAGE1BTN + 1);
		}
		break;
	//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
	case FRI_PCROOM_PAGE1BTN: case FRI_PCROOM_PAGE2BTN: case FRI_PCROOM_PAGE3BTN: 
		{
			GAMEIN->GetFriendDialog()->SetPCRoomMemberList(lId - FRI_PCROOM_PAGE1BTN + 1);
		}
		break;

	case FRI_ADDFRIENDBTN:
		{
			if(GAMEIN->GetMiniFriendDialog()->IsActive())
			{
				GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
			}
			else
			{
				GAMEIN->GetMiniFriendDialog()->SetActive(TRUE);
				char Name[MAX_NAME_LENGTH+1] = {0};
				CObject* pTarget = OBJECTMGR->GetSelectedObject();
				if(pTarget && (pTarget->GetObjectKind() == eObjectKind_Player))
				{
					//strcpy(Name, OBJECTMGR->GetSelectedObject()->GetObjectName());
					SafeStrCpy(Name, OBJECTMGR->GetSelectedObject()->GetObjectName(), MAX_NAME_LENGTH+1);
					GAMEIN->GetMiniFriendDialog()->SetName(Name);
				}
			}
		}
		break;
	case FRI_DELFRIENDBTN:
		{
			GAMEIN->GetFriendDialog()->CheckedDelFriend();
		}
		break;
	case FRI_ADDOKBTN:
		{		
			cEditBox * pName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(FRI_NAMEEDIT);
			char code[MAX_NAME_LENGTH+1] = {0,};			
			sscanf(pName->GetEditText(),"%s",code);
			strupr(code);
			if(strcmp("",code) == 0)
				return;
			char heroname[MAX_NAME_LENGTH+1] = {0,};
			sscanf(HERO->GetObjectName(), "%s", heroname);
			strupr(heroname);
			if(strcmp(heroname, code) == 0)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1488) );
				return;
			}				
			FRIENDMGR->AddDelFriendSyn(pName->GetEditText(), MP_FRIEND_ADD_SYN);
			GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
		}
		break;
	case FRI_ADDCANCELBTN:
		{
			GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);
		}
		break;
	case FRI_SENDNOTEBTN:
		{
			CMiniNoteDialogWrite* dialog = GAMEIN->GetMiniNoteDialogWrite();
			ASSERT( dialog );

			dialog->SetActive( TRUE );

			if( ! strlen( GAMEIN->GetFriendDialog()->GetChkedName() ) )
			{
				dialog->SetMiniNote( "", "", 0 );
			}
			else
			{
				dialog->SetMiniNote(GAMEIN->GetFriendDialog()->GetChkedName(), "", 0);
			}
		}
		break;
	case FRI_SENDWHISPERBTN:
		{
			char temp[MAX_NAME_LENGTH+5];
			if(strcmp(GAMEIN->GetFriendDialog()->GetChkedName(), "") == 0)
				return;
			sprintf(temp,"/%s ",GAMEIN->GetFriendDialog()->GetChkedName());
			//061212 LYW --- Change chatting dialog.
			//cEditBox* pChatBox = GAMEIN->GetChattingDlg()->GetInputBox();
			//070102 LYW --- Delete this code.
			
			cEditBox* pChatBox = GAMEIN->GetChattingDlg()->GetInputBox();
			pChatBox->SetFocusEdit(TRUE);
			pChatBox->SetEditText(temp);


		}
		break;
	//090623 pdy 하우징 기능추가 (친구목록 링크 하우스입장) 
	case FRI_FRIENDLIHOUSE1: 
	case FRI_FRIENDLIHOUSE2: 
	case FRI_FRIENDLIHOUSE3:
	case FRI_FRIENDLIHOUSE4:
	case FRI_FRIENDLIHOUSE5:
	case FRI_FRIENDLIHOUSE6:
	case FRI_FRIENDLIHOUSE7:
	case FRI_FRIENDLIHOUSE8:
	case FRI_FRIENDLIHOUSE9:
	case FRI_FRIENDLIHOUSE10:
		{
			if( HERO->GetState() != eObjectState_None || HERO->IsPKMode() )
			{
				WINDOWMGR->MsgBox( MBI_CANTUSE_MAPMOVESCROLL, MBT_OK, CHATMGR->GetChatMsg(789) ) ;
				return ;
			}

			DWORD BtnNum = lId - FRI_FRIENDLIHOUSE1;

			CFriendDialog* Dlg = GAMEIN->GetFriendDialog() ;
		
			cListCtrl* pFriendListLCtl = (cListCtrl*)((cTabDialog*)Dlg->GetTabSheet(0))->GetWindowForID(FRI_FRIENDLISTLCTL);
			cRITEMEx * rItem = (cRITEMEx *)pFriendListLCtl->GetRItem( BtnNum );

			if( ! rItem )
				break;

			// 091019 pdy 하우징맵에서 로딩중에 각종 하우징 기능 사용금지 추가
			if( HOUSINGMGR->IsHousingMap() &&	
				HOUSINGMGR->IsHouseInfoLoadOk() == FALSE )
			{
				return;
			}

			//090625 pdy 하우징 팝업창 [친구목록 링크 하우스입장]
			char MsgBuf[128] = {0,};
			sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), rItem->pString[0]);
			cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_FRIENDHOME, MBT_YESNO, MsgBuf ) ;	//1890	"%s의 집으로 이동 하시겠습니까?"

			//친구의 이름의 주소값을 Param으로 넣자  
			if( pBox )
				pBox->SetParam( (DWORD)rItem->pString[0] );

			OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
		}
		break;
	}
}

void Note_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId) 
	{
	case NOTE_WRITENOTEBTN:
		{
			if(GAMEIN->GetMiniNoteDialogWrite()->IsActive())
			{
				GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
			}
			else
			{
				GAMEIN->GetMiniNoteDialogWrite()->SetActive(TRUE);
			}
		}
		break;
	case NOTE_DELNOTEBTN:
		{
			GAMEIN->GetNoteDialog()->CheckedNoteID();
		}
		break;
	case NOTE_SAVENOTEBTN:
		{
			GAMEIN->GetNoteDialog()->SaveNoteID();
		}
		break;

	case NOTE_PAGE1BTN:
	case NOTE_PAGE2BTN:
	case NOTE_PAGE3BTN:
	case NOTE_PAGE4BTN:
	case NOTE_PAGE5BTN:
	case NOTE_PAGE6BTN:
	case NOTE_PAGE7BTN:
	case NOTE_PAGE8BTN:
		{
			NOTEMGR->NoteListSyn( lId - NOTE_PAGE1BTN +1, GAMEIN->GetNoteDialog()->GetMode() );
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		//////MINI NOTE
	case NOTE_MSENDOKBTN:
		{
			cEditBox * pReceiver = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NOTE_MRECEIVEREDIT);
			char pname[MAX_NAME_LENGTH+1] = {0};
			SafeStrCpy(pname, pReceiver->GetEditText(), MAX_NAME_LENGTH+1);
			if(strlen(pname) == 0)
				return;

			cEditBox * pTitle = (cEditBox *)WINDOWMGR->GetWindowForIDEx(NOTE_MWTITLE);
			char ptitle[MAX_NOTE_TITLE+1] = {0,};
			SafeStrCpy(ptitle, pTitle->GetEditText(), MAX_NOTE_TITLE+1);
			int len = strlen(ptitle);
			if(len <= 2)
				return;

			if(len >= MAX_NOTE_TITLE)
			{
				len = MAX_NOTE_TITLE - 1;
				ptitle[len] = 0;
			}

			cTextArea * pNote = (cTextArea *)WINDOWMGR->GetWindowForIDEx(NOTE_MWNOTETEXTREA);
			char FullText[512];
			pNote->GetScriptText(FullText);

			char Note[MAX_NOTE_LENGTH];
			SafeStrCpy(Note, FullText, MAX_NOTE_LENGTH - 1);
			Note[MAX_NOTE_LENGTH - 1] = 0;
			NOTEMGR->WriteNote(Note, ptitle, pname);
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);			
		}
		break;
	case NOTE_MSENDCANCELBTN:
		{
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(FALSE);
		}
		break;
	case NOTE_MREPLYBTN:
		{
			char ToName[MAX_NAME_LENGTH+1];

			if( 0 == strcmp(CHATMGR->GetChatMsg(2236), GAMEIN->GetMiniNoteDialogRead()->GetSenderName()) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2237) );
				return;
			}

			strcpy(ToName, GAMEIN->GetMiniNoteDialogRead()->GetSenderName());
			if(strlen(ToName) == 0 )
				return;

			GAMEIN->GetMiniNoteDialogWrite()->SetMiniNote(ToName, "", 0);
			GAMEIN->GetMiniNoteDialogWrite()->SetActive(TRUE);
			GAMEIN->GetMiniNoteDialogRead()->SetActive(FALSE);

		}
		break;
	case NOTE_MGETPACKAGEBTN:
		{
			if( GAMEIN->GetMiniNoteDialogRead()->IsActive() )
				GAMEIN->GetMiniNoteDialogRead()->SendGetPackageMsg();
		}
		break;
	case NOTE_MDELETEBTN:
		{
			NOTEMGR->DelNoteSyn(GAMEIN->GetMiniNoteDialogRead()->GetNoteID(), TRUE);
			GAMEIN->GetMiniNoteDialogRead()->SetActive(FALSE);
		}
		break;
	case NOTE_TABBTN1:
		{
			GAMEIN->GetNoteDialog()->SetMode( eMode_NormalNote );
			NOTEMGR->NoteListSyn( 1, eMode_NormalNote );
		}
		break;
	case NOTE_TABBTN2:
		{
			GAMEIN->GetNoteDialog()->SetMode( eMode_StoredNote );
			NOTEMGR->NoteListSyn( 1, eMode_StoredNote );
		}
		break;
	case NOTE_NOTELISTCHK12:
		{
			GAMEIN->GetNoteDialog()->SetChkAll();
		}
		break;
	}
}

// 080313 NYJ --- 전체외치기 추가
void Shout_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(SHOUT_SHOUTDLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

//080410 NYJ --- 낚시시스템
void Fishing_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case FISHING_BTN_CLOSE:
		{
			GAMEIN->GetFishingDlg()->SetActive(FALSE);
		}
		break;
	case FISHING_BTN_START:
		{
			// 이전버튼 상태는 현재상태의 반대.
			BOOL bLastPushed = !GAMEIN->GetFishingDlg()->IsPushedStartBtn();
			
			if(bLastPushed)
				GAMEIN->GetFishingDlg()->Fishing_Cancel();
			else
				GAMEIN->GetFishingDlg()->Fishing_Start();
				

			GAMEIN->GetFishingDlg()->SetLockStartBtn(TRUE);
		}
		break;

	case FISHING_BTN_CLOSEPOINT:
	case FISHING_BTN_CLOSEPOINTX:
		{
			// 포인트 창닫기 X버튼
			GAMEIN->GetFishingPointDlg()->SetActive(FALSE);
		}
		break;

	case FISHING_BTN_CHANGEPOINT:
		{
			// 포인트변환
			GAMEIN->GetFishingPointDlg()->SendFishPoint();
		}
		break;

	case FISHING_BTN_CLEARSLOT:
		{
			// 슬롯초기화
			GAMEIN->GetFishingPointDlg()->ClearFishIcon();
		}
		break;
	}
}

void CR_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	if( EXITMGR->IsExitWaiting() )	//종료중엔 부활 안됨
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(643) );
		GAMEIN->GetReviveDialog()->SetActive( FALSE );
		return;
	}

	LEVELTYPE heroLevel = HERO->GetLevel() ;

	if( heroLevel >= 10 )
	{
		switch(lId) 
		{
		case CR_PRESENTSPOT:
			{
				if( SIEGEDUNGEONMGR->IsSiegeDungeon(MAP->GetMapNum()) )
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_PRESENTSPOT, MBT_YESNO, CHATMGR->GetChatMsg( 1687 ) );//"1~5%의 경험치가 하락합니다, 제자리에서 부활 하시겠습니까?") ;

  				}
  				else
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_PRESENTSPOT, MBT_YESNO, CHATMGR->GetChatMsg(850)) ;
  				}
			}
			break;

		case CR_TOWNSPOT:
			{
				if( SIEGEDUNGEONMGR->IsSiegeDungeon(MAP->GetMapNum()) )
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_TOWNSOPT, MBT_YESNO, CHATMGR->GetChatMsg( 1688 )) ;

  				}
  				else
  				{
  					WINDOWMGR->MsgBox(MBI_REVIVE_TOWNSOPT, MBT_YESNO, CHATMGR->GetChatMsg(851)) ;
  				}
			}
			break;
		}
	}
	else
	{
		switch(lId)
		{
		case CR_PRESENTSPOT :
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_PRESENTSPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			break ;

		case CR_TOWNSPOT :
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_VILLAGESPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			break ;
		}
	}

	GAMEIN->GetReviveDialog()->SetDisable(TRUE);
}

void OTI_DlgFunc(LONG lId, void * p, DWORD we)
{
	GAMEIN->GetOptionDialog()->OnActionEvent(lId, p, we);
}

void EXT_DlgFunc(LONG lId, void * p, DWORD we)
{
	//A¡E￠cAA¡E￠cE¡E￠c¡§u￠®E￠®ⓒ­Co
	if( we == WE_BTNCLICK )
	{
		switch( lId )
		{
		case EXT_BTN_EXITGAME:
			{
				GAMEIN->GetExitDialog()->SetActive( FALSE ) ;
				WINDOWMGR->MsgBox( MBI_EXIT, MBT_YESNO, RESRCMGR->GetMsg( 263 ) ) ;
			}
			break;
			
		case EXT_BTN_SELECTCHAR:
			{
				WINDOWMGR->MsgBox( MBI_MOVE_TO_CHARSELECT, MBT_YESNO, CHATMGR->GetChatMsg( 1329 ) );
			}
			break;

		case EXT_BTN_OPTION:
			{
				COptionDialog* pDlg = GAMEIN->GetOptionDialog() ;					

				if( pDlg )															
				{
					pDlg->SetActive( TRUE ) ;										
				}
			}
			break;

		case EXT_BTN_EMERGENCY:
			{
				if(MAP->GetMapNum() == GTMAPNUM)
				{
					WINDOWMGR->MsgBox( MBI_GTEMERGENCY_FAIL, MBT_OK, CHATMGR->GetChatMsg( 1656 ) );
					return;
				}

				MSGBASE msg;
				msg.Category	= MP_CHAR;
				msg.Protocol	= MP_CHAR_EMERGENCY_SYN;
				msg.dwObjectID	= HEROID;	
				NETWORK->Send( &msg, sizeof(msg) );
				EXITMGR->SendExitMsg( eEK_CHARSEL );
			}
			break;
		}
	}
}

void XCI_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetExchangeDialog()->OnActionEvent(lId, p, we);
}

void PLI_DlgFunc( LONG lId, void* p, DWORD we )
{
	if( p )
	{
		if( ((cDialog*)p)->GetType() == WT_PKLOOTINGDLG )
		{
			((CPKLootingDialog*)p)->OnActionEvent( lId, p, we );
		}
	}
}

void CNA_BtnOkFunc( LONG lId, void* p, DWORD we )
{
	if( p )
	{
		((cDialog*)p)->SetActive( FALSE );
	}
	TITLE->SetServerList();
}

void DIVIDEBOX_Func(LONG lId, void * p, DWORD we)
{
	if( we == WE_RETURN )
	{
		((cDivideBox*)p)->ExcuteDBFunc( WE_RETURN );		
	}
}

void MONEYDIVIDEBOX_Func(LONG lId, void * p, DWORD we)
{
	if( we == WE_RETURN )
	{
		((cMoneyDivideBox*)p)->ExcuteDBFunc( WE_RETURN );		
	}
}

void MessageBox_Func( LONG lId, void * p, DWORD we )
{
	switch( lId )
	{
	case MBI_EXIT:
		{
			if( we == MBI_YES )
			{
				if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN )
				{
					//					MAINGAME->SetGameState( eGAMESTATE_END );
					PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
					break;
				}

				int nErrorCode;
				if( ( nErrorCode = EXITMGR->CanExit() ) == eEXITCODE_OK )
				{
					EXITMGR->SendExitMsg( eEK_GAMEEXIT );
				}
				else
				{
					switch( nErrorCode )
					{
					case eEXITCODE_PKMODE:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1261) );
						break;
					case eEXITCODE_LOOTING:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1262) );
						break;
					default:
						// 070622 LYW --- GlobalEventFunc : 메시지 번호 수정.
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );
						break;
					}
				}
			}
		}
		break;

	// 080121 LYW --- GlobalEventFunc : 캐릭터 선택 화면으로 이동확인 메시지 처리.
	case MBI_MOVE_TO_CHARSELECT :
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetExitDialog()->SetActive( FALSE );
				int nErrorCode;
				if( ( nErrorCode = EXITMGR->CanExit() ) == eEXITCODE_OK )
				{
					EXITMGR->SendExitMsg( eEK_CHARSEL );
				}
				else
				{
					switch( nErrorCode )
					{
					case eEXITCODE_PKMODE:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1261) );
						break;
					case eEXITCODE_LOOTING:
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1262) );
						break;
					default:
						// 070622 LYW --- GlobalEventFunc : 메시지 번호 수정.
						CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(219) );
						break;
					}
				}
			}
		}
		break ;

	case MBI_PASSERROR:
		{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
			cEditBox* pEditId = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_IDEDITBOX );
			cEditBox* pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_PWDEDITBOX );
			if( pEditId )
				pEditId->SetFocusEdit( FALSE );
			if( pEditPwd )
			{
				pEditPwd->SetEditText( "" );
				pEditPwd->SetFocusEdit( TRUE );
			}
			cDialog* pDlg1 = (cDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			pDlg1->SetDisable( FALSE );

			cDialog* pDlg2 = (cDialog*)WINDOWMGR->GetWindowForID( MT_DYNAMICDLG );

			if( pDlg2 )
				if( pDlg2->IsActive() )				
				{
					pDlg1->SetActive( TRUE );
					pDlg2->SetDisable( FALSE );
					pDlg2->SetActive( FALSE	);
					pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_DYNAMICPWDEDITBOX );
					pEditPwd->SetEditText( "" );
					//				pEditPwd->SetFocusEdit( TRUE );
				}
		}
		break;
	case MBI_DISCONNECTED_OVERLAPPEDLOGIN:
		{
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case MBI_PARTYBREAKUP:
		{
			if(we == MBI_YES)
			{
				PARTYMGR->BreakUpSyn();
			}
		}
		break;
	case MBI_STORAGEBUY:
		{
			if(we == MBI_YES)
			{
				STORAGEMGR->BuyStorageSyn();
			}
		}
		break;
	case MBI_SKILLDELETE:
		{
			if( we == MBI_YES )
			{
				AUDIOMGR->Play(
					67,
					gHeroID);
			}
		}
		break;
	case MBI_MAKESTREETSTALL:
		{
			if(we == MBI_YES)
			{
				STREETSTALLMGR->OpenStreetStall();
			}
			else if( we == MBI_NO )
			{
				STREETSTALLMGR->SetOpenMsgBox(TRUE);
			}
		}
		break;

	case MBI_DELETECHAR:
		{
			if( we == MBI_YES )
			{
				CHARSELECT->GetCertificateDlg()->MakeRandomNumber() ;
				CHARSELECT->GetCertificateDlg()->SetActive(TRUE) ;
			}
			else
			{
				CHARSELECT->SetDisablePick( FALSE );
			}
		}
		break;
	case MBI_CANCEL :
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break ;

	case MBI_EXCHANGEAPPLY:
		{
			if(we == MBI_YES)
			{
				EXCHANGEMGR->AcceptExchange( TRUE );
			}
			else if( we == MBI_NO )
			{
				EXCHANGEMGR->AcceptExchange( FALSE );
			}
		}
		break;

	case MBI_EXCHANGEWAITING:
		{
			if(we == MBI_CANCEL)
			{
				EXCHANGEMGR->CancelApply();				
			}
		}
		break;

	case MBI_SELECTLOGINPOINT:
		{
			if(we == MBI_YES)
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_SELECTLOGINPOINT );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );
				if( pNpc )
				{
					g_UserInput.GetHeroMove()->SelectLoginPoint( pNpc->GetNpcUniqueIdx() );
				}
				else
				{
					//error ¡E￠c¡E￠￥I¡E￠c¡§u¡E￠c¡E?AI￠®¡×¡E￠c¡E￠cAAI￠®¡×¡E￠c￠®Ec￠®E￠®ⓒ­| AuAaCO ￠®¡×uo ￠®¡×u¡§I¡§¡I￠®¡×oA￠®E￠®II￠®E￠®IU.
				}

				if( HERO->GetState() == eObjectState_Deal )
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
			}
			else if( we == MBI_NO )
			{
				if( HERO->GetState() == eObjectState_Deal )
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
			}
		}
		break;

	case MBI_LOGINEDITCHECK:
		{

			if( we == MBI_OK )
			{
				cEditBox * pIdEdit =(cEditBox *) WINDOWMGR->GetWindowForIDEx(MT_IDEDITBOX); // pjs ID￠®E?￠®E¡Ec ￠®¡×¡E￠c¡E￠cAA￠®E?￠®¡×o￠®¡×￠®ⓒ­ [5/20/2003]
				cEditBox * pPwEdit =(cEditBox *) WINDOWMGR->GetWindowForIDEx(MT_PWDEDITBOX); // pjs ID￠®E?￠®E¡Ec ￠®¡×¡E￠c¡E￠cAA￠®E?￠®¡×o￠®¡×￠®ⓒ­ [5/20/2003]

				if( *pIdEdit->GetEditText() == 0 )
					pIdEdit->SetFocusEdit(TRUE);
				else
					pPwEdit->SetFocusEdit(TRUE);
			}
			cDialog* pDlg= (cDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			pDlg->SetDisable( FALSE );
		}
		break;
	case MBI_DELETEICONCHECK:
		{
			switch( we )
			{
			case MBI_YES:
				{
					ITEMMGR->SendDeleteItem();
					AUDIOMGR->Play(
						69,
						gHeroID);
					break;
				}
			case MBI_NO:
				{
					GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
					GAMEIN->GetStorageDialog()->SetDisable( FALSE );

					// 길드창을 사용 가능하게 한다.
					{
						cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
						ASSERT( dialog );

						dialog->SetDisable( FALSE );
					}

					break;
				}
			}
		}
		break;
	case MBI_DELETEPETICONCHECK:
		{
			switch( we )
			{
			case MBI_YES :
				{
					GAMEIN->GetPetWearedDlg()->SendDelete() ;
					AUDIOMGR->Play(
						69,
						gHeroID);
				}
				break ;

			default : break ;
			}

			GAMEIN->GetPetWearedDlg()->SetDisable( FALSE ) ;
		}
		break;

	case MBI_FRIEND_ADD:
		{
			if(we == MBI_YES)
			{
				FRIENDMGR->FriendInviteAccept();

			}
			else if( we == MBI_NO )
			{
				FRIENDMGR->FriendInviteDeny();
			}
		}
		break;
	case MBI_SHOWDOWNAPPLY:
		{
			if(we == MBI_YES)
			{
				SHOWDOWNMGR->AcceptShowdown( TRUE );
			}
			else if( we == MBI_NO )
			{
				SHOWDOWNMGR->AcceptShowdown( FALSE );
			}
		}
		break;
	case MBI_SHOWDOWNWAITING:
		{
			if(we == MBI_CANCEL)
			{
				SHOWDOWNMGR->CancelApply();
			}
		}
		break;
	case MBI_CHARSELERROR:
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break;
	case MBI_CHARMAKEERROR:
		{
			CHARMAKE->SetDisableDlg( FALSE );

			// 070619 LYW --- GlobalEventFunc : Setting To Disable Gender button.
			// 성별을 선택할 수 없도록 하기 위해 Disable로 세팅해 준다.
			CHARMAKE->GetCharMakeNewDlg()->GetPushUpButton(e_PB_MAN)->SetDisable(TRUE) ;
			CHARMAKE->GetCharMakeNewDlg()->GetPushUpButton(e_PB_WOMAN)->SetDisable(TRUE) ;
		}
		break;
	case MBI_PARTY_SECEDE:
		{
			if(we == MBI_YES)
			{
				if( GAMEIN->GetGuageDlg()->GetHeroState() == e_CAPTAIN )
				{
					GAMEIN->GetGuageDlg()->SecessionPartyCaptain() ;
				}

				PARTYMGR->DelPartyMemberSyn();
			}
		}
		break;
	case MBI_OVERLAPPEDLOGININOTHERSERVER:
		{
			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				dlg->SetDisable( FALSE );
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}
		}
		break;
	case MBI_OVERLAPPEDLOGIN:
		{			
			if(we == MBI_YES)
			{
				MSGBASE msg;
				msg.Category = MP_USERCONN;
				msg.Protocol = MP_USERCONN_FORCE_DISCONNECT_OVERLAPLOGIN;
				NETWORK->Send(&msg,sizeof(msg));

				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(4) );
				cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( pIDDlg && pMsgBox )
				{
					pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
				}
			}
			else if( we == MBI_NO )
			{
				cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( dlg )
				{
					dlg->SetDisable( FALSE );
					cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
					cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
					if( editboxId )
					{
						editboxId->SetEditText("");
						editboxId->SetFocusEdit(TRUE);
					}
					if( editboxPwd )
					{
						editboxPwd->SetEditText("");
						editboxPwd->SetFocusEdit(FALSE);
					}
				}				
			}
		}
		break;
	case MBI_SEND_DISCONNECT_OVERLAPPEDLOGIN:
		{

		}
		break;
	case MBI_LOGINERRORCLOSE:
		{
			//			MAINGAME->SetGameState(eGAMESTATE_END);
			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				dlg->SetDisable( FALSE );
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}

			TITLE->NoDisconMsg();		//
			TITLE->ShowServerList();
		}
		break;
	case MBI_CANTUSEDYNAMIC:
		{
			cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_DYNAMICDLG );
			if( pDlg )
			{
				pDlg->SetDisable( FALSE );
				((cEditBox*)pDlg->GetWindowForID( MT_DYNAMICPWDEDITBOX ))->SetEditText( "" );
				((cEditBox*)pDlg->GetWindowForID( MT_DYNAMICPWDEDITBOX ))->SetFocusEdit( TRUE );
			}			
		}
		break;
	case MBI_SERVERDISCONNECT:
		{
			//			MAINGAME->SetGameState(eGAMESTATE_END);
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;
	case MBI_DISTCONNECT_ERROR:
		{
			WINDOWMGR->CloseAllMsgBoxNoFunc();
			TITLE->ShowServerList();
		}
		break;
	case MBI_MAPSERVER_CLOSE:
		{
			CHARSELECT->SetDisablePick( FALSE );
		}
		break;
	case MBI_SELLITEM:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetDealDialog()->SendSellItemMsg();
			}
			else
			{
				CDealDialog::OnCancelSellItem(0,NULL,0,GAMEIN->GetDealDialog(),NULL);
				//GAMEIN->GetDealDialog()->CancelSellItem();
			}
		}
		break;
	case MBI_BUYITEM:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetDealDialog()->SendBuyItemMsg();
			}
			else
			{
				GAMEIN->GetDealDialog()->CancelBuyItem();
			}
		}
		break;
	case MBI_PKMODE:
		{
			if( we == MBI_YES )
			{
				PKMGR->SendMsgPKOn();
			}
			else
			{

			}
		}
		break;
	case MBI_NOT_GM:
		{
			//			MAINGAME->SetGameState( eGAMESTATE_END );
			PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		}
		break;

	case MBI_WAIT_LOGINCHECK:
		{
			//￠®E?￠®¡×I¡E￠c¡§ua￠®¡×u¡E￠c¡§I ￠®E￠®ⓒ­￠®¡×￠®¨I￠®¡×u￠®¡×uAo￠®E￠®ⓒ­| ￠®¡×￠®ⓒ­￠®E￠®ⓒ­¡§I¨I¨￡¡E￠ci¡E￠c¡§￠®i ¡§Ii¨Io￠®Ei¡§o￠®¡×u￠®¡×¡E￠c￠®¡×u¡E￠c¡§I A¡§I¨I¨￡￠®E￠®ⓒ­￠®Ec? ¡E￠c¡E￠￥¡E￠cEAI￠®EO¡E￠c￠®￠?￠®E￠®IA AI¡E￠c¡§￠®I￠®Ii￠®Ii A¡§I¨I¨￡￠®E￠®ⓒ­￠®Ec ￠®¡×uE￠®IiE¡E￠c¡§￠®I ￠®¡×u￠®¡×¡E￠c￠®E￠®IN¡E￠c¡§￠®￠®E¡Ec?

			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}

			TITLE->NoDisconMsg();		//￠®¡×u￠®¡×¡E￠c¡E￠c¡E￠￥￠®E¡Ec showserverlist￠®E?￠®E¡Ec￠®¡×u¡E￠c¡§I A¡E￠cE￠®¡×uO ¡§I￠®"￠cAA¡E￠ci￠®EO¡E￠c￠®￠? ¡§I￠®Ao¡E￠c¡§ue ￠®E￠®ⓒ­￠®¡×￠®¨I￠®¡×u￠®¡×uAo ￠®¡×u¡§I¡§¡I￠®Ii￠®Ii¡E￠c¡E￠￥I.
			TITLE->ShowServerList();
			TITLE->StartWaitConnectToAgent( FALSE );
		}
		break;
	case MBI_TIMEOVER_TOCONNECT:
		{
			TITLE->GetServerListDialog()->SetActive( TRUE );
		}
		break;


	case MBI_STREETBUYMSG:
		{
			if( we == MBI_YES )
			{
				STREETSTALLMGR->SendBuyItemMsg();
			}

			// 081202 LUJ, 잠금을 해제한다
			if(CStreetStall* stallDialog = GAMEIN->GetStreetStallDialog())
			{
				stallDialog->SetDisable( FALSE );
			}
		}
		break;

	case MBI_REGEDITMSG:
		{
			if( we == MBI_YES )
			{
				if(!GAMEIN->GetStreetBuyStallDialog()->IsActive())
					return;

				POSTYPE pos = GAMEIN->GetStreetBuyStallDialog()->GetCurSelectedItemNum();
				BUY_REG_INFO RegInfo = GAMEIN->GetStreetBuyStallDialog()->GetBuyRegInfo(pos);
				GAMEIN->GetBuyRegDialog()->SetRegInfo(RegInfo);
				GAMEIN->GetBuyRegDialog()->Show();	

			}
			else
			{
				MSGBASE msg;
				msg.Category = MP_STREETSTALL;
				msg.dwObjectID = HEROID;
				msg.Protocol = MP_STREETSTALL_UPDATEEND_SYN;

				NETWORK->Send( &msg, sizeof(MSGBASE) );
			}
		}
		break;
	case MBI_STREETSELLMSG:
		{
			CStreetBuyStall* pStall = GAMEIN->GetStreetBuyStallDialog();

			if( we == MBI_YES )
				if(pStall && pStall->IsActive() && STREETSTALLMGR->SendSellItemMsg())
					break;
				else
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1218) );

			if(pStall) GAMEIN->GetStreetBuyStallDialog()->SetData(NULL);

			CItem* pItem = pStall->GetSellItem();

			if(pItem)	pItem->SetLock(FALSE);

			pStall->DelSellItem();
			// 081202 LUJ, 잠금을 해제한다
			pStall->SetDisable( FALSE );
		}
		break;

	case MBI_QUESTGIVEUP:
		{
			if( we == MBI_YES )
				QUESTMGR->QuestAbandon();
		}
		break;
	case MBI_QUESTSTART:
		{
			if( we == MBI_YES )
				GAMEIN->GetInventoryDialog()->ItemUseForQuestStart();

		}
		break;
	case MBI_GUILD_SECEDE:
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_GUILD;
				msg.Protocol = MP_GUILD_SECEDE_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
				GAMEIN->GetGuildDlg()->SetActive(FALSE);
			}
		}
		break;
	case MBI_GUILD_BREAKUP:
		{
			if( we == MBI_YES )
			{
				if( GUILDWARMGR->IsValid( 3 ) )
				{
					//GUILDMGR->BreakUpGuildSyn();

					if(HERO->GetGuildIdx() == 0)
					{
						char buf[128];
						sprintf(buf, "HOW CAN U SHOW DLG? chridx : %u", HEROID);
						ASSERTMSG(0, buf);
						return;
					}
					if(HERO->GetGuildMemberRank() != GUILD_MASTER)
					{
						char buf[128];
						sprintf(buf, "HOW CAN U BREAKUP? chridx : %u, rank : %d", HEROID, HERO->GetGuildMemberRank());
						ASSERTMSG(0, buf);
						return;
					}
					if( HERO->GetGuildUnionIdx() != 0 )
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(591));
						return;
					}
				
					MSGBASE msg;
					msg.Category = MP_GUILD;
					msg.Protocol = MP_GUILD_BREAKUP_SYN;
					msg.dwObjectID = HEROID;
				
					NETWORK->Send(&msg, sizeof(msg));	
				}
			}
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}
		break;
	case MBI_GUILD_BAN:
		{
			CGuildDialog* dialog = GAMEIN->GetGuildDlg();

			if( ! dialog )
			{
				break;
			}

			// 080417 LUJ, 메시지 창 표시 전에 잠갔으므로 반드시 다시 풀어주자
			dialog->SetDisable( FALSE );

			if(we == MBI_YES)
			{
				MSG_DWORD msg;
				msg.Category	= MP_GUILD;
				msg.Protocol	= MP_GUILD_DELETEMEMBER_SYN;
				msg.dwObjectID	= HEROID;
				msg.dwData		= dialog->GetKickMemberIndex();
				NETWORK->Send(&msg, sizeof(msg));
			}
		}
		break;
	case MBI_SHOPITEM_LOCK:
		{
			CInventoryExDialog* pInven = GAMEIN->GetInventoryDialog() ;
			if( !pInven )
			{
				return ;
			}
			else
			{
				if( we == MBI_YES ) pInven->ItemUnSealingSync() ;
			}


			// 인벤토리 Disable을 해제해 준다.
			pInven->SetDisable(FALSE) ;
		}
		break;
	case MBI_REVIVE_PRESENTSPOT :
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_PRESENTSPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			else
			{
				GAMEIN->GetReviveDialog()->SetDisable(FALSE);
			}
		}
		break ;

	case MBI_REVIVE_TOWNSOPT :
		{
			if( we == MBI_YES )
			{
				MSGBASE msg;
				msg.Category = MP_CHAR_REVIVE;
				msg.Protocol = MP_CHAR_REVIVE_VILLAGESPOT_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send(&msg, sizeof(msg));
			}
			else
			{
				GAMEIN->GetReviveDialog()->SetDisable(FALSE);
			}
		}
		break ;

		// guildfieldwar -----------------------------------------------------------------
	case MBI_GUILDFIELDWAR_DECLARE_ACCEPT:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendDeclareAcceptDeny( MP_GUILD_WAR_DECLARE_ACCEPT );
		}
		break;
	case MBI_GUILDFIELDWAR_DECLARE_DENY:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendDeclareAcceptDeny( MP_GUILD_WAR_DECLARE_DENY );
		}
		break;	
	case MBI_GUILDFIELDWAR_SUGGEST:
		{
			GAMEIN->GetGFWarInfoDlg()->SetDisable( FALSE );
			//			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendSuggest();
		}
		break;
	case MBI_GUILDFIELDWAR_SURREND:
		{
			GAMEIN->GetGFWarInfoDlg()->SetDisable( FALSE );
			//			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
			if( we == MBI_YES )
				GUILDWARMGR->SendSurrender();
		}
		break;
	case MBI_GUILDFIELDWAR_SUGGEST_ACCEPTDENY:
		{
			if( we == MBI_YES )
			{
				GUILDWARMGR->SendSuggestAcceptDeny( MP_GUILD_WAR_SUGGESTEND_ACCEPT );
			}
			else if( we == MBI_NO )
			{
				GUILDWARMGR->SendSuggestAcceptDeny( MP_GUILD_WAR_SUGGESTEND_DENY );
			}
		}
		break;
	case MBI_GUILDFIELDWAR_REVIVE:
		{
			if( MBI_YES == we )
			{
				// 부활 선택 창 끄고
				{
					cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
					ASSERT( dialog );

					dialog->SetActive( FALSE );
				}

				// 부활 요청
				{
					MSGBASE message;

					message.Category	= MP_CHAR_REVIVE;
					message.Protocol	= MP_CHAR_REVIVE_LOGINSPOT_SYN;
					message.dwObjectID	= HEROID;

					NETWORK->Send( &message, sizeof( message ) );
				}
			}
		}
		break;
		//--------------------------------------------------------------------------------
		// guildunion
	case MBI_UNION_INVITE:
		{
			if( we == MBI_YES )
				GUILDUNION->SendInviteAcceptDeny( MP_GUILD_UNION_INVITE_ACCEPT );
			else if( we == MBI_NO )
				GUILDUNION->SendInviteAcceptDeny( MP_GUILD_UNION_INVITE_DENY );
		}
		break;
	case MBI_UNION_REMOVE:
		{
			if( we == MBI_YES )
				GUILDUNION->SendRemoveGuildUnion();			
		}
		break;
	case MBI_UNION_DESTROY:
		{
			if( we == MBI_YES )
				GUILDUNION->SendDestroyGuildUnion();
			OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Deal );
		}
		break;
	case MBI_UNION_LEAVE:
		{
			if( we == MBI_YES )
			{
				MSGBASE message;
				message.Category	= MP_GUILD_UNION;
				message.Protocol	= MP_GUILD_UNION_SECEDE_SYN;
				message.dwObjectID	= HEROID;
				
				NETWORK->Send( &message, sizeof( message ) );
			}
		}
		break;
		//----------------------------------------------------------------------------------
	case MBI_PRESENT_NOTIFY:
		{
		}
		break;
		// 070607 LYW --- GlobalEventFunc : Add identification part.
	case MBI_IDENTIFICATION_REGIST_INFO :
		{
			CIdentification* pDlg = GAMEIN->GetIdentificationDlg() ;

			if( we == MBI_YES )
			{
				if( pDlg )
				{
					// check all data is ok.
					CSHResidentRegistManager::stRegistInfo* pRegistInfo = pDlg->GetRegistInfo() ;

					// setting name.
					strcpy(pRegistInfo->szNickName, HERO->GetObjectName()) ;

					// setting gender.
					if( strcmp(pDlg->GetGender()->GetStaticText(), RESRCMGR->GetMsg(508)) == 0 )
					{
						pRegistInfo->nSex = 0 ;
					}
					else
					{
						pRegistInfo->nSex = 1 ;
					}

					// setting age.
					pRegistInfo->nAge = (WORD)atoi(pDlg->GetAge()->GetEditText()) ;
					pRegistInfo->nLocation = pDlg->GetRegion()->GetClickedRowIdx() + 1;
					pRegistInfo->nFavor = 0 ;

					g_csResidentRegistManager.CLI_RequestRegist(pRegistInfo) ;
				}
			}
			else
			{
				pDlg->CancelRegist() ;
			}
		}
		break ;

	case MBI_IDENTIFICATION_ALREADY_HAVE :
		{
			// Deactive npc script dialog.
			if( HERO->GetState() != eObjectState_Die )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
		}
		break ;

	case MBI_MATCHAT_INVITE :
		{
			// check.
			if( we == MBI_YES )
			{
				// start chatting.
				g_csResidentRegistManager.CLI_StartDateMatchingChat() ;
			}
			else
			{
				// send message.
				g_csResidentRegistManager.CLI_RefuseDateMatchingChat() ;
			}
		}
		break ;

	case MBI_MATCHAT_CANCEL :
		{
			if( we == MBI_CANCEL )
			{
				// cancel invite.
				g_csResidentRegistManager.CLI_CancelDateMatchingChat();
			}
		}
		break ;

	case MBI_END_CHATTING_PARTNERLIST :
		{
			if( we == MBI_YES )
			{
				// end chatting.
				g_csResidentRegistManager.CLI_EndDateMatchingChat() ;

				// return to partner list.
				GAMEIN->GetDateMatchingDlg()->SetUpperDlg(e_PartnerListDlg) ;
			}
			else if( we == MBI_NO )
			{
				GAMEIN->GetDateMatchingDlg()->GetPartnerTabBtn()->SetPush(FALSE) ;
			}
		}
		break ;

	case MBI_END_CHATTING_CLOSE :
		{
			if( we == MBI_YES )
			{
				// end chatting.
				g_csResidentRegistManager.CLI_EndDateMatchingChat() ;

				// close matching dialog.
				GAMEIN->GetDateMatchingDlg()->SetActive(FALSE) ;
			}
		}
		break ;

	case MBI_FAMILY_INVITE_YOU :
		{
			/*enum FAMILY_INVITE_RESULT
			{
				FIR_ACCEPT_MASTER = 0,
					FIR_ACCEPT_PLAYER,
					FIR_DENY_MASTER,
					FIR_DENY_PLAYER,
			};*/
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_ACCEPT_PLAYER) ;
			}
			else
			{
				g_csFamilyManager.CLI_RequestAnswerToInvite(CSHFamilyManager::FIR_DENY_PLAYER) ;
			}
		}
		break ;

		// 길드 권한 이양 확인 메시지 창
	case MBI_GUILD_RETIRE_NOTIFY:
		{
			CGuildDialog* dialog = ( CGuildDialog* )WINDOWMGR->GetWindowForID( GD_GUILDDLG );
			ASSERT( dialog );

			dialog->SetDisable( FALSE );

			const GUILDMEMBERINFO* member = dialog->GetSelectedMember();
			ASSERT( member );

			if( we == MBI_YES )
			{
				MSG_DWORD message;
				message.Category	= MP_GUILD;
				message.Protocol	= MP_GUILD_RETIRE_SYN;
				message.dwObjectID	= HEROID;
				message.dwData		= member->MemberIdx;

				NETWORK->Send( &message, sizeof( message ) );
			}
		}
		break;
	case MBI_GUILD_RETIRE_REQUEST:
		{
			// 길드 창에서 길드 마스터 인덱스를 가져온다.
			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			MSG_DWORD message;
			message.Category	= MP_GUILD;
			message.dwObjectID	= gHeroID;
			message.dwData		= gHeroID;
			message.Protocol	= ( MBI_YES == we ? MP_GUILD_RETIRE_ACK : MP_GUILD_RETIRE_NACK );

			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
		// 아이템 조합을 복수로 할때 실행 여부에 대한 확인 창을 표시한다.
	case MBI_ITEM_MIX_CONFIRM:
		{
			// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
			if( we == MBI_YES )
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionMix );
			}
			else
			{
				CMixDialog* dialog = ( CMixDialog* )WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
				ASSERT( dialog );

				dialog->Restore();
			}
		}
		break;
		// 아이템 강화 옵션이 있는 경우 옵션이 낮아질 수 있음에 대한 경고를 표시한다
	case MBI_ITEM_REINFORCE_CONFIRM:
		{
			// 진행 표시가 끝난 후 서버에 작업을 요청할 것이다
			if( we == MBI_YES )
			{
				CProgressDialog* dialog = ( CProgressDialog* )WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
				ASSERT( dialog );

				dialog->Wait( CProgressDialog::eActionReinforce );
			}
			else
			{
				CReinforceDlg* dialog = ( CReinforceDlg* )WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
				ASSERT( dialog );

				dialog->Restore();
			}
		}
		break;
	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.10.16
	// ..탈퇴
	case MBI_FAMILY_LEAVE:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestLeave();
			}
		}
		break;
	// ..추방
	case MBI_FAMILY_EXPEL:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestExpel(HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID);
			}
		}
		break;
	// ..해체
	case MBI_FAMILY_BREAKUP:
		{
			if( we == MBI_YES )
			{
				g_csFamilyManager.CLI_RequestBreakUp();
			}
		}
		break;

	case MBI_SKILLTRAIN:
		{
			if( we == MBI_YES )
			{
				GAMEIN->GetSkillTrainingDlg()->Training();
			}
			else
			{
				GAMEIN->GetSkillTrainingDlg()->SetDisable( FALSE );
			}
		}
		break;
	// E 패밀리 추가 added by hseos 2007.10.16

	// 071201 LYW --- GlovalEventFunc : 
	case MBI_MAPMOVE_AREYOUSURE :
		{
			cMapMoveDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetMapMoveDlg() ;

			if( !pDlg ) return ;

			if( we == MBI_YES )
			{
				pDlg->MoveMapSyn() ;
			}
		}
		break ;

	// 071203 LYW --- GlovalEventFunc : 현재 마나 감소 효과를 취소하고 다시 적용할 것인지 물어본다.
	case MBI_FORCE_USEBUFFITEM_AREYOUSURE :
	case MBI_USEBUFFITEM_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			// 080728 LUJ, 인벤토리 잠금을 해제한다
			pDlg->SetDisable( FALSE );

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				// 인벤에 아이템 등록 하고 사용하면, 현재 선택 된 아이템이 없을 때 처리를 못하기 때문에
				// GetCurSelectedItem은 사용하지 않는다.
				//pItem = pDlg->GetCurSelectedItem(INV) ;	

				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;
			}
		}
		break ;

	// 071204 LYW --- GlovalEventFunc : 스탯 초기화 확인 메시지 처리.
	case MBI_RESETSTATS_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetStats(pItem) ;

				pItem->SetLock(FALSE) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;

	// 071204 LYW --- GlovalEventFunc : 스킬 초기화 확인 메시지 처리.
	case MBI_RESETSKILL_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetSkill(pItem) ;

				pItem->SetLock(FALSE) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;
	// 080218 KTH -- ResetRegidentRegist
	case MBI_RESET_REGIDENTREGIST :
		{
			CInventoryExDialog* pDlg = NULL;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return;

			CItem* pItem = NULL;
			pItem = pDlg->Get_QuickSelectedItem();

			if( !pItem ) return;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ResetRegistResident(pItem);
				pItem->SetLock(FALSE);
			}
			
			pItem->SetLock(FALSE);
			pDlg->Set_QuickSelectedItem(NULL);
		}
		break;

	// 071205 LYW --- GlovalEventFunc : 경험치 증가 확인 메시지 처리.
	case MBI_INCEXPRATE_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;

				pDlg->Set_QuickSelectedItem(NULL) ;
			}
		}
		break ;

	// 071205 LYW --- GlovalEventFunc : 드랍률 증가 확인 메시지 처리.
	case MBI_INCDROPRATE_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			if( we == MBI_YES )
			{
				CItem* pItem = NULL ;
				pItem = pDlg->Get_QuickSelectedItem() ;

				if( !pItem ) return ;

				ITEMMGR->UseItem_ItemMallItem(pItem, HERO) ;

				pDlg->Set_QuickSelectedItem(NULL) ;
			}
		}
		break ;
	// 071210 LYW --- GlobalEventFunc : 인벤토리 확장 사용 메시지 처리.
	case MBI_EXTENDINVEN_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				ITEMMGR->UseItem_ExtendInventory(pItem) ;
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;
	// S 농장시스템 추가 added by hseos 2008.01.17
	case MBI_FARM_TAX_PAY_REQUEST:
		{
			if( we == MBI_YES )
			{
				// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 버튼 추가
				// 농장맵에서만 농장정보가 관리되고 있기 때문에 농장맵에서의 납부와 
				// 타맵에서의 납부를 구분하여 처리한다.
				if( CSHFarmManager::ePayTaxMode_FarmNpc == g_csFarmManager.GetPayTaxMode() )
				{
					g_csFarmManager.CLI_RequestPayTax(1);
				}
				else
				{
					g_csFarmManager.CLI_RequestPayTax(3);
				}
			}
		}
		break;
	// E 농장시스템 추가 added by hseos 2008.01.17
		// 080414 LUJ, 외양 변경 아이템 사용
	case MBI_USEBODYCHANGEITEM:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );

			// 080930 LUJ, 메시지 박스 처리도 해당 창에서 하도록 한다
			if( ! dialog )
			{	
				break;
			}

			dialog->OnActionEvent( MBI_USEBODYCHANGEITEM, 0, we );
		}
		break;
	// 080507 KTH -- 가축아이템 사용
	case MBI_FARM_ANIMAL_INSTALL :
		{
			CInventoryExDialog* pDlg = NULL ;
			pDlg = GAMEIN->GetInventoryDialog() ;

			if( !pDlg ) return ;

			CItem* pItem = NULL ;
			pItem = pDlg->Get_QuickSelectedItem() ;

			if( !pItem ) return ;

			if( we == MBI_YES )
			{
				g_csFarmManager.CLI_RequestInstallAnimalInAnimalCage(pItem->GetPosition(), pItem->GetItemIdx());
			}

			pItem->SetLock(FALSE) ;
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break ;

	case MBI_CR_SECRETCODE_ERR :
 		{
 			if( we != MBI_OK ) return ;
 
 			CChatRoomCreateDlg* pDlg = NULL ;
 			pDlg = GAMEIN->GetChatRoomCreateDlg() ;
 
 			if( !pDlg ) return ;
 
 			cEditBox* pCodeBox = NULL ;
 			pCodeBox = pDlg->GetCodeBox() ;
 
 			if( !pCodeBox ) return ;
 
 			pCodeBox->SetFocus(TRUE) ;
 			// 혹여 채팅창에 포커스가 있으면 ㅡㅡ; 해재 하자.
 		}
 		break ;

	case MBI_CR_ENDCHAT :														// 채팅방 종료 버튼 처리.
		{
			if( we == MBI_YES )
			{
				CChatRoomDlg* pDlg = NULL ;										// 채팅방 정보를 받는다.
				pDlg = GAMEIN->GetChatRoomDlg() ;

				ASSERT(pDlg) ;

				if(!pDlg)
				{
					MessageBox( NULL, "ERR-FRCRD", "GEF-CR_EC", MB_OK) ;
					return ;
				}

				MSG_DWORD msg ;													// 종료 요청 메시지를 보낸다.
				memset(&msg, 0, sizeof(MSG_DWORD)) ;

				msg.Category	= MP_CHATROOM ;
				msg.Protocol	= MP_CHATROOM_OUT_ROOM_SYN ;
				msg.dwObjectID	= HEROID ;

				msg.dwData		= CHATROOMMGR->GetMyRoomOwnerIdx() ;

				NETWORK->Send( &msg, sizeof(MSG_DWORD) ) ;
			}
		}
		break;
	case MBI_PETGRADEUP:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			if( we == MBI_YES )
			{
				pDlg->PetGradeUp();
			}
			else
			{
				pDlg->SetDisable( FALSE );
			}
		}
		break;
	case MBI_GTENTER_SYN:
		{
			if( we == MBI_YES)
			{
				MSG_DWORD msgMapSyn;
				msgMapSyn.Category = MP_GTOURNAMENT;
				msgMapSyn.Protocol = MP_GTOURNAMENT_MOVETOBATTLEMAP_SYN;
				msgMapSyn.dwObjectID = HEROID;
				msgMapSyn.dwData = 0;
				NETWORK->Send( &msgMapSyn, sizeof(msgMapSyn) );
			}
		}
		break;

		// 080821 LYW --- GlobalEventFunc : 물어보는 맵 이동 포탈 처리 추가.
  	case MBI_RECALLPORTAL_CHANGEMAP :
  		{
  			if( we == MBI_YES ) 
  			{
  				CObject* pObject = NULL ;
  				pObject = OBJECTMGR->GetSelectedObject() ;
  
  				if( !pObject )
  				{
  					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1689 ) );//"대상을 선택해 주세요." ) ;

  					return ;
  				}
  
  				if( pObject->GetObjectKind() != eObjectKind_Npc ||
  					((CNpc*)pObject)->GetNpcJob() != PORTAL_ROLE )
  				{
  					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1690 ) ); //"올바른 대상이 아닙니다." ) ; 
  					return ;
  				}
  
  				MSG_DWORD msg ;
  
  				msg.Category	= MP_RECALLNPC ;
  				msg.Protocol	= MP_RECALLNPC_CTOAM_CHANGEMAP_SYN ;
  				msg.dwObjectID	= HEROID ; 
  				msg.dwData		= NPCRECALLMGR->Get_CurSelectNpcId() ;
  				
  				NETWORK->Send(&msg, sizeof(msg));
  			}
  		}
  		break ;
  
  		// 080922 LYW --- GlobalEventFunc : 길드 아지트로 이동하겠냐는 처리 추가.
  	case MBI_SIEGEWARFLAG : 
  		{
  			if( we == MBI_YES )
  			{
  				MSGBASE msg ;
  
  				msg.Category	= MP_SIEGEWARFARE ;
  				msg.Protocol	= MP_SIEGEWARFARE_WARPTOAGIT_SYN ;
  
  				msg.dwObjectID	= HEROID ;
  
  				NETWORK->Send( &msg, sizeof(MSGBASE) ) ;
  			}
  		}
  		break;
		// 091127 LUJ, 소환 스킬 사용
	case MBI_RECALL_REQUEST:
		{
			if( MBI_NO == we )
			{
				break;
			}

			const CMoveManager::TemporaryRecallData& data = MOVEMGR->GetTemporaryRecallData();

			MSG_NAME_DWORD3 message;
			ZeroMemory(&message, sizeof(message));
			message.Category = MP_SKILL;
			message.Protocol = MP_SKILL_RECALL_SYN;
			message.dwObjectID = gHeroID;
			message.dwData1	= data.mTargetPlayerIndex;
			message.dwData2	= data.mSkillIndex;
			message.dwData3 = data.mMapType;
			SafeStrCpy(
				message.Name,
				data.mTargetPlayerName,
				sizeof(message.Name) / sizeof(*message.Name));
			NETWORK->Send( &message, sizeof( message ) );
		}
		break;

		// 081031 LUJ, 소환 승낙 여부를 처리한다
	case MBI_RECALL_ASK:
		{
			const CMoveManager::TemporaryRecallData& data = MOVEMGR->GetTemporaryRecallData();

			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_MOVE;
			message.Protocol	= ( MBI_YES == we ? MP_MOVE_RECALL_ASK_ACK : MP_MOVE_RECALL_ASK_NACK );
			message.dwObjectID	= HEROID;
			message.dwData		= data.mKey;
			NETWORK->Send( &message, sizeof( message ) );
		}
		break;
		// 090422 ShinJS --- 탈것 탑승 요청시 수락/거부 
	case MBI_VEHICLE_ALLOW_GETON:
		{
			// 자신의 탈것 정보를 얻어온다
			CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( HERO->GetVehicleID() );
			if( !pVehicle || pVehicle->GetObjectKind() != eObjectKind_Vehicle )		break;

			cMsgBox* const messageBox = (cMsgBox*)WINDOWMGR->GetWindowForID(
				MBI_VEHICLE_ALLOW_GETON);

			if(0 == messageBox)
			{
				break;
			}

			// 탑승 수락
			if( we == MBI_YES )
			{
				MSG_DWORD4 msg;
				ZeroMemory( &msg, sizeof( msg ) );
				msg.Category = MP_VEHICLE;
				msg.Protocol = MP_VEHICLE_MOUNT_ALLOW_SYN;
				msg.dwData1 = pVehicle->GetRequestGetOnPlayerID();
				msg.dwData2 = pVehicle->GetID();
				msg.dwData3 = pVehicle->GetEmptySeatPos(pVehicle->GetRequestGetOnPlayerID());
				msg.dwData4 = messageBox->GetParam();
				NETWORK->Send(
					&msg,
					sizeof(msg));
			}
			// 탑승 거부
			else
			{
				MSG_DWORD msg;
				ZeroMemory( &msg, sizeof( msg ) );
				msg.Category = MP_VEHICLE;
				msg.Protocol = MP_VEHICLE_MOUNT_REJECT_SYN;
				msg.dwObjectID = HEROID;
				msg.dwData = pVehicle->GetRequestGetOnPlayerID();
				NETWORK->Send( &msg, sizeof( msg ) );
			}
		}
		break;

	// 090422 ShinJS --- 탈것 탑승시 요금이 필요한 경우 탑승 시도 판단
	case MBI_VEHICLE_GETON_ASK_PAY_TOLL:
		{
			if( we != MBI_YES )
			{
				break;
			}

			CVehicle* const pVehicle = (CVehicle*)OBJECTMGR->GetObject( VEHICLEMGR->GetRidingVehicle() );

			if(0 == pVehicle)
			{
				break;
			}
			else if(eObjectKind_Vehicle != pVehicle->GetObjectKind())
			{
				break;
			}

			if(gHeroID != pVehicle->GetOwnerIndex())
			{
				CHATMGR->AddMsg(
					CTC_SYSMSG,
					CHATMGR->GetChatMsg(2034),
					pVehicle->GetMasterName());
			}

			cMsgBox* const messageBox = (cMsgBox*)WINDOWMGR->GetWindowForID(
				MBI_VEHICLE_GETON_ASK_PAY_TOLL);

			if(0 == messageBox)
			{
				break;
			}
			else if(WT_MSGBOX != messageBox->GetType())
			{
				break;
			}

			MSG_DWORD2 msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_VEHICLE;
			msg.Protocol = MP_VEHICLE_MOUNT_ASK_SYN;
			msg.dwObjectID = gHeroID;
			msg.dwData1 = pVehicle->GetID();
			msg.dwData2 = messageBox->GetParam();
			NETWORK->Send(
				&msg,
				sizeof(msg));
		}
		break;
	// 090415 하우징 이름짓기 실행 여부를 처리한다 
	case MBI_HOUSENAME_AREYOUSURE:
		{
			//패킷을 보네자. 
			cHouseNameDlg* pNameDlg = GAMEIN->GetHouseNameDlg();
			if( MBI_NO == we )
			{
				pNameDlg->SetDisable(FALSE);
				break;
			}
			//090420 하우스생성 패킷을 보네자 MP_HOUSE_CREATE_SYN

			MSG_HOUSE_CREATE Message;
			ZeroMemory( &Message, sizeof( Message ) );
			Message.Category = MP_HOUSE ; 
			Message.Protocol = MP_HOUSE_CREATE_SYN ; 
			Message.dwPlayerID = HEROID;
			Message.dwPlayerID = HEROID;
			SafeStrCpy(Message.szHouseName, pNameDlg->GetName() , MAX_HOUSING_NAME_LENGTH+1);

			NETWORK->Send( &Message, sizeof( Message ) );

		}
		break;
	// 090415 하우징 집찾기 실행 여부를 처리한다 . 
	case MBI_HOUSESEACH_AREYOUSURE:
		{
			//패킷을 보네자.
			cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
			stVisitInfo* pVisitInfo = pSearchDlg->GetVisitInfo();
			if( MBI_NO == we || ! pVisitInfo )
			{
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				pSearchDlg->SetDisableState(FALSE);
				break;
			}

			pSearchDlg->AddVisitList();

			MSG_HOUSE_VISIT msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
			msg.dwObjectID = HEROID;
			msg.cKind	= pVisitInfo->m_dwVisitKind;
			msg.dwValue1 = pVisitInfo->m_dwData1;
			msg.dwValue2 = pVisitInfo->m_dwData2;

			SafeStrCpy(msg.Name, pSearchDlg->GetName() , MAX_NAME_LENGTH+1);
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
	//090618 pdy 하우징 기능추가 랜덤입장 
	case MBI_VISIT_RANDOM_AREYOUSURE:
		{
			//패킷을 보네자.
			cHouseSearchDlg* pSearchDlg = GAMEIN->GetHouseSearchDlg();
			stVisitInfo* pVisitInfo = pSearchDlg->GetVisitInfo();
			if( MBI_NO == we || ! pVisitInfo )
			{
				GAMEIN->GetInventoryDialog()->SetDisable( FALSE );
				pSearchDlg->SetDisableState(FALSE);
				break;
			}

			MSG_HOUSE_VISIT msg;
			ZeroMemory( &msg, sizeof( msg ) );
			msg.Category = MP_HOUSE;
			msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
			msg.dwObjectID = HEROID;
			msg.cKind	=  eHouseVisitByItemRandom;
			msg.dwValue1 = pVisitInfo->m_dwData1;
			msg.dwValue2 = pVisitInfo->m_dwData2;

			SafeStrCpy(msg.Name, pSearchDlg->GetName() , MAX_NAME_LENGTH+1);
			NETWORK->Send(&msg,sizeof(msg));
		}
		break;
		// 090415 하우징 꾸미기 보너스 사용 여부를 처리한다 . 
	case MBI_USEDECOBONUS_AREYOUSURE:
		{
			//처리하자 처리하자 처리하자 
			cHousingDecoPointDlg* pDecoPointDlg = GAMEIN->GetHousingDecoPointDlg();
			pDecoPointDlg->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			stHouseBonusInfo* pBonusInfo = pDecoPointDlg->GetCurSelectBonusInfo();

			if(! pBonusInfo )
				break;

			HOUSINGMGR->RequestUseDecoBonus(pBonusInfo->dwBonusIndex);
		}
		break;
	case MBI_DECOINSTALL_AREYOUSURE:
		{
			GAMEIN->GetHousingRevolDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			//최초시에만 ㅇㅇ 
			if( HOUSINGMGR->RequestInstallCurDeco() )
				GAMEIN->GetHousingRevolDlg()->SetActive(FALSE);
		}
		break;
	case MBI_HOUSE_EXIT_AREYOUSURE:
		{
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				//090618 pdy 하우징 나가기 액션 버그 수정 
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				break;
			}
			HOUSINGMGR->RequestUseCurAction();
		}
		break;
	case MBI_HOUSE_VOTEHOUSE_AREYOUSURE:
		{
			GAMEIN->GetHousingActionPopupMenuDlg()->SetDisable(FALSE);
			if( MBI_NO == we )
			{
				break;
			}

			HOUSINGMGR->RequestUseCurAction();
		}
		break;
	case MBI_HOUSE_CHANGEMATERIAL_AREYOUSURE:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			ITEM_INFO* pInfo = ITEMMGR->GetItemInfo( pItem->GetItemIdx() );
			if( ! pInfo ) return ;

			if( MBI_NO == we )
			{
				pItem->SetLock(FALSE) ;
				HOUSINGMGR->ChangeMaterialFurniture(pInfo->SupplyType , pInfo->SupplyValue, 1 );
			}
			else
			{
				MSG_ITEM_USE_SYN msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_USE_SYN;
				msg.dwObjectID = HEROID;
				msg.wItemIdx = pItem->GetItemIdx();
				msg.TargetPos = pItem->GetPosition();

				NETWORK->Send(&msg,sizeof(msg));
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_HOUSE_VISIT_MYHOME_AREYOUSURE:
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			if( MBI_YES == we )
			{
				MSG_HOUSE_VISIT msg;
				msg.Category = MP_HOUSE;
				msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
				msg.dwObjectID = HEROID;
				msg.cKind	=  eHouseVisitByItem ;
				msg.dwValue1 = pItem->GetItemIdx() ;
				msg.dwValue2 = pItem->GetPosition() ;
				SafeStrCpy(msg.Name, HERO->GetObjectName() , MAX_NAME_LENGTH+1);

				NETWORK->Send(&msg,sizeof(msg));
			}
			else
			{
				pItem->SetLock(FALSE);
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_VISIT_LINK_PLAYER_RCLICK:						//090615 pdy 하우징 캐릭터 우클릭 링크 하우스입장 기능 추가
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_VISIT_LINK_PLAYER_RCLICK );				
				CObject* pDestObj = OBJECTMGR->GetObject( pMsgBox->GetParam() );

				if( pDestObj )
				{
					BASEOBJECT_INFO DestObjInfo;	
					pDestObj->GetBaseObjectInfo(&DestObjInfo);

					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;			
					msg.dwValue1 = 0;				
					msg.dwValue2 = DestObjInfo.dwUserID;
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE:			//090604 pdy static Npc를 통해 내집가기 추가 
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );

				if( pNpc )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;			//Npc를 통해 들어가는것은 eHouseVisitByLink
					msg.dwValue1 = pNpc->GetID();				
					msg.dwValue2 = TITLE->GetUserIdx();			//Npc를 통해 내집을 들어갈때는 유저인덱스를 보네야한다.
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_AREYOUSURE:
		{
			if( MBI_YES == we )
			{
				if(HERO->GetVehicleID())
				{
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1839) ); //1839	"탑승 도구를 이용 시에는 해당 기능을 사용하실 수 없습니다."
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
					return;
				}

				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_AREYOUSURE );				
				CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( pMsgBox->GetParam() );
				if( pNpc )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByLink ;
					msg.dwValue1 = pNpc->GetID();
					msg.dwValue2 = pNpc->GetNpcTotalInfo()->dwSummonerUserIndex;
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else 
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}
		}
		break;
	case MBI_HOUSE_VISIT_LINK_FRIENDHOME :
		{
			if( MBI_YES == we )
			{
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_HOUSE_VISIT_LINK_FRIENDHOME );		
				char* pFriendName = (char*)(pMsgBox->GetParam());
				if( pFriendName )
				{
					MSG_HOUSE_VISIT msg;
					msg.Category = MP_HOUSE;
					msg.Protocol = MP_HOUSE_ENTRANCE_SYN;
					msg.dwObjectID = HEROID;
					msg.cKind	=  eHouseVisitByName ;
					msg.dwValue1 = 0;
					msg.dwValue2 = 0;
					SafeStrCpy(msg.Name, pFriendName , MAX_NAME_LENGTH+1);
					NETWORK->Send(&msg,sizeof(msg));
				}
				else
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
				}
			}
			else
			{
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
			}

		}
		break;
	case MBI_HOUSE_EXTEND_AREYOUSURE :
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			CItem* pItem = pDlg->Get_QuickSelectedItem() ;
			if( !pItem ) return ;

			if( MBI_YES == we )
			{
				MSG_ITEM_USE_SYN msg;
				msg.Category = MP_ITEM;
				msg.Protocol = MP_ITEM_USE_SYN;
				msg.dwObjectID = HEROID;
				msg.wItemIdx = pItem->GetItemIdx();
				msg.TargetPos = pItem->GetPosition();

				NETWORK->Send(&msg,sizeof(msg));
			}
			else
			{
				pItem->SetLock(FALSE);
			}

			pDlg->SetDisable(FALSE);
			pDlg->Set_QuickSelectedItem(NULL) ;
		}
		break;
	case MBI_HOUSE_DESTROY_FUTNITURE_AREYOUSURE:
		{
			cHousingWarehouseDlg* pDlg = GAMEIN->GetHousingWarehouseDlg();
			if(! pDlg ) return;

			cHousingStoredIcon* pStoredIcon = pDlg->Get_QuickSelectedIcon();
			if( ! pStoredIcon ) return;

			if( MBI_YES == we )
			{
				HOUSINGMGR->RequestDestroyFuniture(pStoredIcon->GetLinkFurniture());
			}
		}
		break;
	// 091125 ONS 캐릭터명 변경시 팝업 메세지 출력
	case MBI_CHANGENAME_ACK:
		{
			cChangeNameDialog* pDlg = GAMEIN->GetChangeNameDlg();
			if(! pDlg ) return;

			if( MBI_YES == we )
			{
				ITEMMGR->UseItem_ChangeName( pDlg->GetChangeName() ) ;
			}
		}
		break;
	// 091126 ONS 패밀리 다이얼로그에 패밀리장 이양 기능 추가
	case MBI_FAMILY_TRANSFER:
		{	
			if( MBI_YES == we )
			{
				g_csFamilyManager.CLI_RequestTransfer( HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID );
			}
		}
		break;
	// 100105 ShinJS --- PC방 아이템 지급 요청 확인
	case MBI_PCROOM_PROVIDE_ITEM:
		{
			if( MBI_YES == we )
			{
				// 아이템 지급 요청
				MSGBASE msg;
				ZeroMemory( &msg, sizeof(msg) );
				msg.Category = MP_PCROOM;
				msg.Protocol = MP_PCROOM_PROVIDE_ITEM_SYN;
				msg.dwObjectID = HEROID;
				NETWORK->Send( &msg, sizeof(msg) );
			}
		}
		break;
	// 100211 ONS 부활계열 스킬 사용시 대상자에게 부활의사를 묻는 처리 추가
	case MBI_RESURRECT_ASK:
		{
			MSGBASE msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_SKILL;
			if( MBI_YES == we )
			{
				msg.Protocol = MP_SKILL_RESURRECT_ACK;
			}
			else
			{
				msg.Protocol = MP_SKILL_RESURRECT_NACK;

				// 부활을 거부하면 기존의 부활지역 선택 다이얼로그를 활성화한다.
				CReviveDialog* pDlg = GAMEIN->GetReviveDialog();
				if( pDlg && !pDlg->IsActive() )
				{
					pDlg->SetActive( TRUE );
				}
			}
			msg.dwObjectID = HEROID;
			NETWORK->Send( &msg, sizeof(msg) );		
		}
		break;

	case MBI_CONSIGNMENT_CANCEL_CONFIRM:
		{
			if(MBI_YES == we)
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if(pDlg && pDlg->GetMode()==CConsignmentDlg::eConsignment_Mode_Regist)
				{
					MSG_CONSIGNMENT_UPDATE msg;
					msg.Category = MP_CONSIGNMENT;
					msg.Protocol = MP_CONSIGNMENT_UPDATE_SYN;
					msg.dwObjectID = HEROID;
					msg.dwConsignmentIndex = pDlg->GetConsignmentIndex();
					msg.wUpdateKind = eConsignmentUPDATEKIND_CancelByUser;
					msg.dwBuyItemIndex = 0;
					msg.dwBuyTotalPrice = 0;
					msg.wBuyDurability = 0;
					
					NETWORK->Send( &msg, sizeof(msg) );
				}
			}
		}
		break;

	case MBI_CONSIGNMENT_BUY_CONFIRM:
		{
			if(MBI_YES == we)
			{
				CConsignmentDlg* pDlg = GAMEIN->GetConsignmentDlg();
				if(pDlg && pDlg->GetMode()==CConsignmentDlg::eConsignment_Mode_Buy)
				{
					DWORD dwConsignmentIndex = pDlg->GetConsignmentIndex();
					DWORD dwBuyItemIndex = pDlg->GetBuyItemIndex(dwConsignmentIndex);
					WORD wBuyDurbility = pDlg->GetBuyDurability(dwConsignmentIndex);
					DWORD dwBuyTotalPrice = pDlg->GetBuyPrice(dwConsignmentIndex) * wBuyDurbility;

					if(!dwConsignmentIndex || !dwBuyItemIndex || !wBuyDurbility || !dwBuyTotalPrice)
						return;

					MSG_CONSIGNMENT_UPDATE msg;
					msg.Category = MP_CONSIGNMENT;
					msg.Protocol = MP_CONSIGNMENT_UPDATE_SYN;
					msg.dwObjectID = HEROID;
					msg.dwConsignmentIndex = dwConsignmentIndex;
					msg.wUpdateKind = eConsignmentUPDATEKIND_Buy;
					msg.dwBuyItemIndex = dwBuyItemIndex;
					msg.wBuyDurability = wBuyDurbility;
					msg.dwBuyTotalPrice = dwBuyTotalPrice;
					NETWORK->Send( &msg, sizeof(msg) );
				}
			}
		}	
		break;
	}
}

void MNM_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case MNM_PARTYDISPLAY_ONOFF:
		break;
	case MNM_WORLDMAPBTN:
		{
			cDialog* pDlg = GAMEIN->GetWorldMapDialog();
			if( pDlg )
			{
				pDlg->SetActive( !pDlg->IsActive() );
			}
		}
		break;
	case MNM_BIGMAPBTN:
		{
			cDialog* pDlg = GAMEIN->GetBigMapDialog();
			if( pDlg )
			{
				pDlg->SetActive( !pDlg->IsActive() );
			}
		}
		break;

	// 091209 ShinJS --- 길찾기용 버튼 추가
	case MNM_PATHFIND_BTN:
		{
			// 길찾기 실행
			CMiniMapDlg* pMiniMapDlg = (CMiniMapDlg*)p;
			if( pMiniMapDlg && we == WE_BTNCLICK )
				pMiniMapDlg->Move_UsePath();
		}
		break;

		// 091209 ShinJS --- 아이템몰 버튼
	case MNM_ITEMMALL_BTN:
		{
			if(GAMEIN->GetItemShopDialog()->IsActive())
			{
				GAMEIN->GetItemShopDialog()->SetActive( FALSE );
				// 100223 ONS 인벤토리도 함께 닫는다.
				GAMEIN->GetInventoryDialog()->SetActive( FALSE );
				return;
			}

			// 091105 pdy 조합/인챈트/강화/분해 중인경우 아이템몰 열기 불가능 
			BOOL isOpen = FALSE;
			{
				cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
				cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
				cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
				cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

				ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

				isOpen =	mixDialog->IsActive()		||
					enchantDialog->IsActive()	||
					reinforceDialog->IsActive()	||
					dissoloveDialog->IsActive();
			}

			if( isOpen == TRUE )
			{
				CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) ) ;
				return;
			}

			GAMEIN->GetItemShopDialog()->SetActive( TRUE );
			GAMEIN->GetInventoryDialog()->SetActive( TRUE );

			MSGBASE msg;																

			msg.Category = MP_ITEM;														
			msg.Protocol = MP_ITEM_SHOPITEM_INFO_SYN;									
			msg.dwObjectID = gHeroID;
			NETWORK->Send(&msg, sizeof(msg));
		}
		break;

		// 091209 ShinJS --- 채팅방 버튼
	case MNM_CHATROOM_BTN:
		{
			CHATROOMMGR->ToggleChatRoomMainDlg();
		}
		break;

		// 091209 ShinJS --- 녹화 시작(녹화 Dialog 띄우기) 버튼
	case MNM_VIDEO_REC_START_BTN:
		{
			// 녹화 Dialog 를 띄운다
			cDialog* pVideoCaptureDlg = WINDOWMGR->GetWindowForID( VIDEOCAPTURE_DLG );
			if( !pVideoCaptureDlg )
				break;

			pVideoCaptureDlg->SetActive( !pVideoCaptureDlg->IsActive() );
		}
		break;

		// 091209 ShinJS --- 녹화 중지 버튼
	case MNM_VIDEO_REC_STOP_BTN:
		{
			CMiniMapDlg* pMiniMapDlg = (CMiniMapDlg*)p;
			if( !pMiniMapDlg )
				break;

			pMiniMapDlg->ShowVideoCaptureStartBtn( TRUE );

			VIDEOCAPTUREMGR->CaptureStop();
		}
		break;

		// 091209 ShinJS --- PC방 상정
	case MNM_PCROOM_MALL_BTN:
		{
			if( GAMEIN->GetDealDialog()->IsActive() )
			{
				// 이미 PC방 상점이 열린 경우
				if( GAMEIN->GetDealDialog()->IsActivePointInfo() )
				{
					// 100223 ONS 상점이 열려있는경우 닫는다.
					GAMEIN->GetDealDialog()->ShowDealDialog( FALSE );
					break;
				}
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1077 ) );		// 상점을 먼저 닫으세요
				break;
			}

			if( HERO->GetState() == eObjectState_Die)
				break;
			
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

			const BOOL isOpen = mixDialog->IsActive()		||
								enchantDialog->IsActive()	||
								reinforceDialog->IsActive()	||
								dissoloveDialog->IsActive();

			if( isOpen )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1104 ) );
				return;
			}

			MSGBASE msg;
			ZeroMemory( &msg, sizeof(msg) );
			msg.Category = MP_PCROOM;
			msg.Protocol = MP_PCROOM_OPEN_MALL_SYN;
			msg.dwObjectID = HEROID;
			NETWORK->Send( &msg, sizeof(msg) );
		}
		break;
	
		// 100105 ShinJS --- PC방 아이템 지급 요청
	case MNM_PCROOM_PROVIDE_ITEM_BTN:
		{
			// 아이템 지급 여부를 묻는 MsgBox 실행
			WINDOWMGR->MsgBox( MBI_PCROOM_PROVIDE_ITEM, MBT_YESNO, CHATMGR->GetChatMsg( 2016 ) );
		}
		break;
	// 100611 레벨업버튼 처리
	case MNM_LEVELUP_BTN:
		{
			const HERO_TOTALINFO*	heroInfo	=	HERO->GetHeroTotalInfo();

			cPushupButton* pLevelUpButton	=	(cPushupButton*)GAMEIN->GetMiniMapDialog()->GetWindowForID( MNM_LEVELUP_BTN );

			if( pLevelUpButton )
			{
				if( heroInfo->LevelUpPoint >= MINIMUM_LEVELUP_POINT )
					GAMEIN->GetCharacterDialog()->SetActive( TRUE );
				
				if( heroInfo->SkillPoint >= MINIMUM_SKILL_POINT )
					GAMEIN->GetSkillTreeDlg()->SetActive( TRUE );		

				pLevelUpButton->SetPush( TRUE );
				pLevelUpButton->SetDisable( TRUE );
			}		
		}
		break;
	case MNM_CHANNEL_COMBOBOX:
		{
			if( we == WE_PUSHDOWN )
				GAMEIN->GetMiniMapDialog()->RequestChannelInfo();

			if( we != WE_COMBOBOXSELECT )
				break;

			GAMEIN->GetMiniMapDialog()->ChangeChannel();
		}
		break;

	}
}

void ITD_DlgFunc(LONG id, void * p, DWORD we)
{
	// send event o CEnchantDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	 );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void ITMD_DlgFunc(LONG lId, void * p, DWORD we)
{
	// send event to CMixDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );

}


void ITR_DlgFunc(LONG lId, void * p, DWORD we)
{
	// send event to CReinforceDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}


void ApplyOptionDialogFunc( LONG lId, void * p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( APPLY_OPTION_DIALOG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}

// 080916 LUJ, 합성 창 이벤트 함수
void ComposeDialogFunc( LONG lId, void * p, DWORD we )
{
	cDialog* dialog = ( cDialog* )GAMEIN->GetComposeDialog();
	
	if( ! dialog )
	{
		return;
	}

	dialog->OnActionEvent( lId, p, we );
}

void CHA_DlgBtnFunc(LONG lId, void * p, DWORD we)
{
	switch(lId)
	{
	case CHA_CONNNECTOK:
		{
			CHARSELECT->GetChannelDialog()->OnConnect();	
		}
		break;
	case CHA_CONNNECTCANCEL:
		{
			CHARSELECT->GetChannelDialog()->SetActive(FALSE);
			
			if( CHARSELECT->GetChannelDialog()->GetState() == CChannelDialog::eState_CharSelect )
			{
				CHARSELECT->SetDisablePick(FALSE);
			}
		}
		break;
	}
}

void SL_DlgBtnFunc( LONG lId, void* p, DWORD we )
{
	CServerListDialog* pDlg = (CServerListDialog*)p;
	switch( lId )
	{
	case SL_BTN_CONNECT:
		TITLE->ConnectToServer( pDlg->GetSelectedIndex() );
		break;

	case SL_BTN_EXIT:
		//		MAINGAME->SetGameState( eGAMESTATE_END );
		PostMessage( _g_hWnd, WM_CLOSE, NULL, NULL );
		break;
	}
}

void QUE_QuestDlgFunc( LONG lId, void* p, DWORD we )
{
	CQuestDialog* pDlg = (CQuestDialog*)p;

	switch(lId)
	{
		//case QUE_JOURNALBTN:
		//	JOURNALMGR->GetListSyn();
		//	break;
	case QUE_ITEM_GRID:
		//		pDlg->GetPositionForXYRef();
		break;
	case QUE_PAGE1BTN:
		pDlg->RefreshQuestItem(0);
		break;
	case QUE_PAGE2BTN:
		pDlg->RefreshQuestItem(1);
		break;
	case QUE_PAGE3BTN:
		pDlg->RefreshQuestItem(2);
		break;
	case QUE_PAGE4BTN:
		pDlg->RefreshQuestItem(3);
		break;
	case QUE_PAGE5BTN:
		pDlg->RefreshQuestItem(4);
		break;

		// 070125 LYW --- Quest : Modified this line.
	case QUE_PAGE6BTN :
		{
			pDlg->RefreshQuestItem(5);
		}
		break ;

	case QUE_NOTICEBTN :
		{
			//QUE_RegistQuestNotice(TRUE);	// 080305 NYJ --- 퀘스트자동알리미 사용을 위해 아래코드를 함수로 처리함. (현재 사용되지 않음)
			/**/
			DWORD QuestIdx = 0 ;																			// 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.

			CQuestDialog* pDlg = NULL ;																		// 퀘스트 다이얼로그 정보를 받을 포인터를 선언하고 null처리를 한다.
			pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// 퀘스트 다이얼로그 정보를 받는다.

			if( pDlg )																						// 퀘스트 다이얼로그 정보가 유효하다면,
			{
				QuestIdx = pDlg->GetSelectedQuestID() ;														// 선택 된 퀘스트 인덱스를 받는다.
			}

			if( QuestIdx < 1 )																				// 퀘스트 인덱스가 1보다 작으면,
			{
				WINDOWMGR->MsgBox( MBI_QUEST_DELETE, MBT_OK, CHATMGR->GetChatMsg( 1301 ) );

				return ;																					// 리턴 처리를 한다.
			}
			else
			{
				CQuestQuickViewDialog* pQuickDlg = GAMEIN->GetQuestQuickViewDialog() ;

				if( pQuickDlg )
				{
					pQuickDlg->RegistQuest(QuestIdx) ;
					// 100601 ONS 퀘스트 알리미 등록창을 업데이트한다.
					pQuickDlg->ResetQuickViewDialog();

					CQuestDialog* pQuestDlg = GAMEIN->GetQuestDialog() ;

					if(pQuestDlg)
					{
						pQuestDlg->RefreshQuestList() ;	
					}
				}
			}
			/**/
		}
		break ;

	case QUE_GIVEUPBTN :
		{
			DWORD QuestIdx = 0 ;																			// 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.

			CQuestDialog* pDlg = NULL ;																		// 퀘스트 다이얼로그 정보를 받을 포인터를 선언하고 null처리를 한다.
			pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// 퀘스트 다이얼로그 정보를 받는다.

			if( pDlg )																						// 퀘스트 다이얼로그 정보가 유효하다면,
			{
				QuestIdx = pDlg->GetSelectedQuestID() ;														// 선택 된 퀘스트 인덱스를 받는다.
			}

			if( QuestIdx < 1 )																				// 퀘스트 인덱스가 1보다 작으면,
			{
				WINDOWMGR->MsgBox( MBI_QUEST_DELETE, MBT_OK, CHATMGR->GetChatMsg( 1302 ) );

				return ;																					// 리턴 처리를 한다.
			}
			else
			{
				WINDOWMGR->MsgBox( MBI_QUESTGIVEUP, MBT_YESNO, CHATMGR->GetChatMsg( 233 ) );
			}
		}
		break ;
		// 100414 ONS 일반보상/선택보상 버튼처리추가
	case QUE_BTN_REQUITAL_FIX:
		{
			if( !pDlg ) return;
			pDlg->SetRequitalType( (BYTE)eQuestExecute_TakeItem );
			pDlg->SetActiveSelectRequital( FALSE );
		}
		break;
	case QUE_BTN_REQUITAL_SELECT:
		{
			if( !pDlg ) return;
			pDlg->SetRequitalType( (BYTE)eQuestExecute_TakeSelectItem );
			pDlg->SetActiveSelectRequital( TRUE );
		}
		break;
	}
}

void GDRANK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildRankDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDR_RANKDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDLEVEL_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildLevelUpDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_LEVELUPDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDMARK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildMarkDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDM_MARKREGISTDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GD_WarehouseFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildWarehouseDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDW_WAREHOUSEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDCREATE_DlgFunc(LONG id, void* p, DWORD we )
{
	// send event to CGuildCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_CREATEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDINVITE_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildInviteDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_INVITEDlG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDWHRANK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildWarehouseRankDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDWHR_RANKDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDREVIVAL_DlgFunc( LONG id, void* p, DWORD we )
{
	// send evenCGuildRevivalDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_REVIVALDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDUnion_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildUnionCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GDU_CREATEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GDNICK_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to CGuildUnionCreateDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_NICKNAMEDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void GD_DlgFunc( LONG id, void* p, DWORD we )
{
	// send event to cGuildDialog
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


void FD_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FDM_REGISTOKBTN:
		{
			cEditBox* pMarkName = (cEditBox*)WINDOWMGR->GetWindowForIDEx(FDM_NAMEEDIT);

			if(strcmp(pMarkName->GetEditText(), "") == 0)
				break;

			if (g_csFamilyManager.CLI_RequestRegistEmblem(pMarkName->GetEditText()))
			{
				GAMEIN->GetFamilyMarkDlg()->SetActive(FALSE);			
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(549));
			}
		}
		break;
	case FD_CCREATEOKBTN:
		{
			if(HERO->GetFamilyIdx())
			{
				// 100309 ONS 패밀리 중복신청시 메세지 수정
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(697));
				return;
			}
			cEditBox * pFamilyName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(FD_CNAME);
			if(strcmp(pFamilyName->GetEditText(), "") == 0)
				return;

			g_csFamilyManager.CLI_RequestCreateFamily(pFamilyName->GetEditText());
		}
		break;
	case FD_GIVEMEMBERNICK:
		{
			GAMEIN->GetFamilyNickNameDlg()->SetActive(TRUE);
		}
		break;
	case FD_NREGISTOKBTN:
		{
			CFamilyDialog* const pDlg = GAMEIN->GetFamilyDlg();

			if(0 == pDlg)
			{
				break;
			}
			else if(pDlg->GetSelectedMemberIdx() < 0)
			{
				break;
			}

			CFamilyNickNameDialog* const pChangeDlg = GAMEIN->GetFamilyNickNameDlg();

			if(0 == pChangeDlg)
			{
				break;
			}

			CSHFamilyMember* const pMember = HERO->GetFamily()->GetMember(
				pDlg->GetSelectedMemberIdx());

			if(0 == pMember)
			{
				break;
			}
			else if(cEditBox* pEditBox = pChangeDlg->GetEditBox())
			{
				g_csFamilyManager.CLI_RequestChangeNickname(
					pMember->Get()->nID,
					pEditBox->GetEditText());
			}

			pChangeDlg->SetActive(
				FALSE);
		}
		break;
	}
}

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23
void FARM_BUY_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FARM_BUY_OKBTN:
		{
			g_csFarmManager.CLI_RequestBuyFarmToSrv();
		}
		break;
	case FARM_BUY_CANCELBTN:
		{
			GAMEIN->GetFarmBuyDlg()->SetActive(FALSE);
		}
		break;
	}
}

void FARM_UPGRADE_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch(lId)
	{
	case FARM_UPGRADE_OKBTN:
		{
			g_csFarmManager.CLI_RequestUpgradeFarm((CSHFarmZone::FARM_ZONE)GAMEIN->GetFarmUpgradeDlg()->GetFarmZone(),
													GAMEIN->GetFarmUpgradeDlg()->GetFarmID(),
													GAMEIN->GetFarmUpgradeDlg()->GetFarmUpgradeKind());
		}
		break;
	case FARM_UPGRADE_CANCELBTN:
		{
			GAMEIN->GetFarmUpgradeDlg()->SetActive(FALSE);
		}
		break;
	}
}

void FARM_MANAGE_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetFarmManageDlg()->OnActionEvent(lId, p, we);
}
// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
void DATE_ZONE_LIST_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetDateZoneListDlg()->OnActionEvent(lId, p, we);
}

void CHALLENGE_ZONE_LIST_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetChallengeZoneListDlg()->OnActionEvent(lId, p, we);
}

void CHALLENGE_ZONE_CLEARNO1_DlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetChallengeZoneClearNo1Dlg()->OnActionEvent(lId, p, we);
}
// E 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24

// guildfieldwar
void GFW_DlgFunc( LONG lId, void* p, DWORD we )
{
	switch( lId )
	{
		// declare
	case GFW_DECLARE_OKBTN:
		{
			cEditBox* pName = (cEditBox*)WINDOWMGR->GetWindowForIDEx( GFW_DECLARE_EDITBOX );
			cEditBox* pMoney = (cEditBox*)WINDOWMGR->GetWindowForIDEx( GFW_MONEY_EDITBOX );
			GUILDWARMGR->SendDeclare( pName->GetEditText(), RemoveComma(pMoney->GetEditText()) );
		}
		break;
	case GFW_DECLARE_CANCELBTN:
		{
			GAMEIN->GetGFWarDeclareDlg()->SetActive( FALSE );
		}
		break;
	case GFW_MONEY_CHK:
		{
			GAMEIN->GetGFWarDeclareDlg()->ShowMoneyEdit();
		}
		break;

		// result
	case GFW_RESULT_OKBTN:
		{
			if( GUILDWARMGR->IsValid( 4 ) )
			{
				GAMEIN->GetGFWarResultDlg()->SetDisable( TRUE );
				WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_DECLARE_ACCEPT, MBT_YESNO, CHATMGR->GetChatMsg( 957 ) );
			}
		}
		break;
	case GFW_RESULT_CANCELBTN:
		{
			GAMEIN->GetGFWarResultDlg()->SetDisable( TRUE );
			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_DECLARE_DENY, MBT_YESNO, CHATMGR->GetChatMsg( 958 ) );
		}
		break;
	case GFW_RESULT_CONFIRMBTN:
		{
			GAMEIN->GetGFWarResultDlg()->SetActive( FALSE );
		}
		break;
	// 종전 제안
	case GFW_INFO_SUGGEST_OKBTN:
		{
			CGFWarInfoDlg* dialog = GAMEIN->GetGFWarInfoDlg();
			ASSERT( dialog );

			const GUILDINFO* pInfo = GUILDWARMGR->GetEnemyFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );

			if( ! pInfo )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 895 ) );
				break;
			}
			else if( 	pInfo->UnionIdx &&
					!	pInfo->mIsUnionMaster )
			{
				const MSG_GUILD_LIST::Data* data = GUILDWARMGR->GetUnionMasterEnemy( pInfo->UnionIdx );

				if( data )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1270 ), data->GuildName );
				}

				break;
			}

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_SUGGEST, MBT_YESNO, CHATMGR->GetChatMsg( 983 ), pInfo->GuildName );
			dialog->SetDisable( TRUE );
		}
		break;
	// 항복 제안
	case GFW_INFO_SURREND_OKBTN:
		{
			CGFWarInfoDlg* dialog = GAMEIN->GetGFWarInfoDlg();
			ASSERT( dialog );

			const GUILDINFO* pInfo = GUILDWARMGR->GetEnemyFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );

			if( ! pInfo )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 894 ) );
				break;
			}
			else if( 	pInfo->UnionIdx &&
					!	pInfo->mIsUnionMaster )
			{
				const MSG_GUILD_LIST::Data* data = GUILDWARMGR->GetUnionMasterEnemy( pInfo->UnionIdx );

				if( data )
				{
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1270 ), data->GuildName );
				}
				
				break;
			}

			WINDOWMGR->MsgBox( MBI_GUILDFIELDWAR_SURREND, MBT_YESNO, CHATMGR->GetChatMsg( 984 ), pInfo->GuildName );
			dialog->SetDisable( TRUE );
		}
		break;
	case GFW_GDU_REMOVE_OKBTN:	// guildunion remove
		{
			const char* pName = GUILDUNION->GetNameFromListIndex( GAMEIN->GetGFWarInfoDlg()->GetSelectedListIdx() );
			if( pName )
			{
				WINDOWMGR->MsgBox( MBI_UNION_REMOVE, MBT_YESNO, CHATMGR->GetChatMsg( 985 ), pName );
			}
			// 080128 LYW --- GlobalEventFunc : [MP회복] 출력 삭제.
			/*else
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 847 ) );
			}*/
		}
		break;
	case GFW_INFO_CANCELBTN:
		{
			GAMEIN->GetGFWarInfoDlg()->SetActive( FALSE );
		}
		break;

		// warinfo
	case GW_INFOCLOSEBTN:
		{
			GAMEIN->GetGuildWarInfoDlg()->SetActive( FALSE );
		}
		break;
	}
}

void GDT_DlgFunc(LONG lId, void * p, DWORD we)
{
	switch( lId )
	{
	case GTENTRYEDIT_SYN:
	case GTENTRYCHANGE:
	// 090824 ONS 엔트리 등록다이얼로그에 X버튼 기능 추가.
	case GTENTRYEDIT_CLOSE:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GTENTRYEDITDLG );
			ASSERT( dialog );

			dialog->OnActionEvent( lId, p, we );
		}
		break;

	case GT_RESULTOK:
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( GTRESULTDLG );
			ASSERT( dialog );

			dialog->SetActive(FALSE);
		}
		break;
	}
}

void CG_DlgFunc(LONG lId, void *p, DWORD we)
{
	GAMEIN->GetGuageDlg()->OnActionEvent(lId, p, we);
}

// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
void RFGUIDE_DLGFunc(LONG lId, void *p, DWORD we)
{
	GAMEIN->GetReinforceGuideDlg()->OnActionEvent(lId, p, we);
}

void GN_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetGuildNoteDlg()->OnActionEvnet( id, p, we);

	// send event to CGuildNoteDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( GUILDNOTE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void AN_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetUnionNoteDlg()->OnActionEvnet(id, p, we);

	// send event to CUnionNoteDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( ALLYNOTE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

// 06. 03. 문파공지 - 이영준
void GNotice_DlgBtnFunc(LONG id, void *p, DWORD we)
{
	//GAMEIN->GetGuildNoticeDlg()->OnActionEvnet(id, p, we);

	// send to event CGuildNoticeDlg
	cDialog* dialog = WINDOWMGR->GetWindowForID( GUILDNOTICE_DLG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

void GuildPlusTime_DlgFunc(LONG id, void* p, DWORD we)
{
	// GAMEIN->GetGuildPlusTimeDlg()->OnActionEvnet(id,p,we);

	cDialog* dialog = WINDOWMGR->GetWindowForID( GP_TIME );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}

// 061215 LYW --- Add function to call callback function.
void CM_CMNCbFunc( LONG lId, void* p, DWORD we)
{
	CHARMAKEMGR->GetCharMakeNewDlg()->OnActionEvent( lId, p, we ) ;
}

// 061219 LYW --- Add callback function to processing event from controls of main system dialog.
void MDS_CBFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetMainSystemDlg()->OnActionEvent( lId, p, we ) ;
}

void CharMain_BtnFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetGuageDlg()->OnActionEvent( lId, p, we ) ;
}

void JO_DlgFunc(LONG lId, void* p, DWORD we)
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( JOIN_OPTIONDLG );
	ASSERT( dialog );

	dialog->OnActionEvent( lId, p, we );
}


// Date Matching Dialog Procedure
// 2007/03/15	이진영

void DMD_DateMatchingDlgFunc(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetDateMatchingDlg()->OnActionEvent(lId, p, we);
	_asm nop;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of partner idalog.
void DMD_PartnerDlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->OnActionEvent(lId, p, we) ;
}

// 070606 LYW --- GlovalEventFunc : Add function to call callback function of identification dialog.
void DMD_IDT_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetIdentificationDlg()->OnActionEvent(lId, p, we) ;
	_asm nop ;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of favor icon dialog.
void DMD_FID_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetFavorIconDlg()->OnActionEvent(lId, p, we) ;
	_asm nop ;
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of my info dialog.
void DMD_MID_Func(LONG lId, void* p, DWORD we)
{
	CDateMatchingInfoDlg* pDlg = GAMEIN->GetMatchMyInfoDlg() ;

	if( !pDlg ) return ;

	switch(lId)
	{
	case DMD_CHANGE_MY_INTRODUCTION_BTN :
		{
			cTextArea* pTextArea = pDlg->GetIntroduce() ;
			if( pTextArea )
			{
				pTextArea->SetReadOnly(FALSE);
				pTextArea->SetFocusEdit(TRUE);
			}
		}
		break;
	case DMD_SAVE_MY_INTRODUCTION_BTN :
		{
			cTextArea* pTextArea = pDlg->GetIntroduce() ;

			if( pTextArea )
			{
				// check text.
				char tempBuf[MAX_INTRODUCE_LEN+1] = {0, } ;
				pTextArea->GetScriptText(tempBuf) ;

				g_csResidentRegistManager.CLI_RequestUpdateIntroduction(tempBuf) ;
			}
		}
		break;
	}
}

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of record info dialog.
void DMD_RID_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetMatchRecordDlg()->OnActionEvent(lId, p, we) ;
}

// 070618 LYW --- GlovalEventFunc : Add function to call callback function of key setting dialog.
void KS_Func(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetKeySettingTipDlg()->OnActionEvent(lId, p, we) ;
}

void SkillTrainDlgFunc( LONG lId, void* p, DWORD we )
{
	GAMEIN->GetSkillTrainingDlg()->OnActionEvent(lId, p, we);
}

// desc_hseos_패밀리01
// S 패밀리 추가 added by hseos 2007.07.02
void FD_GuildDlg(LONG lId, void* p, DWORD we)
{
	GAMEIN->GetFamilyDlg()->OnActionEvent(lId, p, we);
}
// E 패밀리 추가 added by hseos 2007.07.02


void PROGRESS_DlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( PROGRESS_DIALOG );
	ASSERT( dialog );

	dialog->OnActionEvent( id, p, we );
}


// 071018 LYW --- GlovalEventFunc : Add event function for tutorial dialog.
void TTR_DlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( TUTORIAL_DLG );
	ASSERT( dialog );

	if( we != WE_BTNCLICK ) return ;

	dialog->OnActionEvent( id, p, we );
}


// 071023 LYW --- GlovalEventFunc : Add event function for tutorial button dialog.
void TTR_BTNDlgFunc( LONG id, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( TUTORIALBTN_DLG ) ;
	ASSERT( dialog ) ;

	if( we != WE_BTNCLICK ) return ;

	dialog->OnActionEvent( id, p, we ) ;
}

// 071024 LYW --- GlovalEventFunc : Add event function for helper dialog.
void HD_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( HELP_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_CHECKED )													// 체크 박스 체크 이벤트가 발생했다면,
	{
		if( lId != HD_SHOWTUTORIAL ) return ;								// 체크 박스 아이디와 일치하지 않으면, return 처리를 한다.

		TUTORIALMGR->Set_ActiveTutorial(TRUE) ;								// 튜토리얼을 활성화 상태로 세팅한다.
		TUTORIALMGR->Get_TutorialBtnDlg()->SetActive(TRUE) ;				// 튜토리얼 버튼을 활성화 한다.

		OPTIONMGR->Set_ShowTutorial(TRUE) ;									// 튜토리얼 보이기를 true로 세팅한다.
		OPTIONMGR->SaveGameOption();
	}
	else if( we & WE_NOTCHECKED )											// 체크 박스 체크 풀기 이벤트가 발생했다면,
	{
		if( lId != HD_SHOWTUTORIAL ) return ;								// 체크 박스 아이디와 일치하지 않으면, return 처리를 한다.

		TUTORIALMGR->Set_ActiveTutorial(FALSE) ;							// 튜토리얼을 비활성화 상태로 세팅한다.
		TUTORIALMGR->Get_TutorialBtnDlg()->SetActive(FALSE) ;				// 튜토리얼 버튼을 비활성화 한다.

		OPTIONMGR->Set_ShowTutorial(FALSE) ;								// 튜토리얼 보이기를 false로 세팅한다.
		OPTIONMGR->SaveGameOption();
	}

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}





// 071130 LYW --- GlovalEventFunc : Add event function for map move dialog.
void MapMove_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( MAPMOVE_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}





//=========================================================================
//	NAME : Change_Name_Func
//	DESC : Add event function for change name dialog. 071228
//=========================================================================
void Change_Name_Func(LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( CHANGENAME_DLG ) ;
	ASSERT( dialog ) ;

	if( we & WE_BTNCLICK ) 
	{
		dialog->OnActionEvent( lId, p, we ) ;
	}
}


//=========================================================================
//	NAME : NumberPad_DlgFunc
//	DESC : Add event function for number pad dialog. 080109 LYW
//=========================================================================
void NumberPad_DlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG ) ;

	ASSERT( pDlg ) ;

	if( we & WE_BTNCLICK )
	{
		pDlg->OnActionEvent( lId, p, we ) ;
	}
}


//---KES AUTONOTE
void AutoNoteDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetAutoNoteDlg() )
		GAMEIN->GetAutoNoteDlg()->OnActionEvent(lId, p, we);
}

void AutoAnswerDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetAutoAnswerDlg() )
		GAMEIN->GetAutoAnswerDlg()->OnActionEvent(lId, p, we);
}
//---------------

//---KES 상점검색
void StoreSearchDlg_Func( LONG lId, void* p, DWORD we )
{
	if( GAMEIN->GetStoreSearchDlg() )
		GAMEIN->GetStoreSearchDlg()->OnActionEvent(lId, p, we);
}
//---------------

// 080414 LUJ, 외양 변경 창
void BodyChangeFunc( LONG lId, void* p, DWORD we )
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( BODY_CHANGE_DIALOG );

	if( dialog )
	{
		dialog->OnActionEvent( lId, p, we );
	}
}


// 080403 LYW --- GlovalEventFunc : Add event functions for chatroom system.
void ChatRoomMainDlg_Func(LONG lId, void* p, DWORD we )
{
    ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRMDF", MB_OK) ;
		return ;
	}

	CChatRoomMainDlg* pDlg = NULL ;
 	pDlg = GAMEIN->GetChatRoomMainDlg() ;

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRMDF", MB_OK) ;
 		return ;
 	}
 
 	pDlg->OnActionEvent(lId, p, we) ;
 }

//=========================================================================
//	NAME : ChatRoomCreateDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to create dialog.
//=========================================================================
void ChatRoomCreateDlg_Func(LONG lId, void* p, DWORD we )
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRCDF", MB_OK) ;
		return ;
	}

	CChatRoomCreateDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomCreateDlg() ;											// Receive create dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRCDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to create dialog.
}





//=========================================================================
//	NAME : ChatRoomDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to chatting dialog.
//=========================================================================
void ChatRoomDlg_Func(LONG lId, void* p, DWORD we )
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRDF", MB_OK) ;
		return ;
	}

	CChatRoomDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomDlg() ;												// Receive chatring dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to chatring dialog.
}





//=========================================================================
//	NAME : ChatRoomGuestListDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to guest list dialog.
//=========================================================================
void ChatRoomGuestListDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRGLDF", MB_OK) ;
		return ;
	}

	CChatRoomGuestListDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomGuestListDlg() ;											// Receive guestList dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRGLDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to guestList dialog.
}





//=========================================================================
//	NAME : ChatRoomJoinDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to join dialog.
//=========================================================================
void ChatRoomJoinDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRJDF", MB_OK) ;
		return ;
	}

	CChatRoomJoinDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomJoinDlg() ;											// Receive join dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRJDF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to join dialog.
}





//=========================================================================
//	NAME : ChatRoomOptionDlg_Func - 080312 : LYW
//	DESC : The functino to pass event to option dialog.
//=========================================================================
void ChatRoomOptionDlg_Func(LONG lId, void* p, DWORD we ) 
{
	ASSERT(p) ;

	if(!p)
	{
		MessageBox( NULL, "ERR-IMP", "CRODF", MB_OK) ;
		return ;
	}

	CChatRoomOptionDlg* pDlg = NULL ;
	pDlg = GAMEIN->GetChatRoomOptionDlg() ;											// Receive option dialog.

	ASSERT(pDlg) ;

 	if(!pDlg)
 	{
 		MessageBox(	NULL, "ERR-FRD", "CRODF", MB_OK) ;
 		return ;
 	}

	pDlg->OnActionEvent(lId, p, we) ;												// Pass event to option dialog.
}

void Animal_DlgBtnFunc(LONG lId, void* p, DWORD we )
{
	ASSERT(p);

	if(!p)
	{
		MessageBox(NULL, "ERR-IMP", "CRLDF", MB_OK);
		return;
	}
	CAnimalDialog* pDlg = NULL;
	pDlg = GAMEIN->GetAnimalDialog();

	ASSERT(pDlg);

	if(!pDlg)
	{
		MessageBox( NULL, "ERR-FRD", "CRLDF", MB_OK );
		return;
	}

	GAMEIN->GetAnimalDialog()->OnActionEvent( lId, p, we );
	//pDlg->OnActionEvent( lId, p, we);
}

void PET_STATUS_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPetStateDlg()->OnActionEvent(lId, p, we);
}

void PET_RES_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPetResurrectionDlg()->OnActionEvent(lId, p, we);
}

void PopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetPopupMenuDlg()->OnActionEvent(lId, p, we);
}

void COOK_DLG_FUNC(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetCookDlg()->OnActionEvent(lId, p, we);
}

// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가작업
void RidePopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetRidePopupMenuDlg()->OnActionEvent(lId, p, we);
}

//090323 pdy 하우징 설치 UI추가
void HousingRevolDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingRevolDlg()->OnActionEvent(lId, p, we);
}

//090330 pdy 하우징 창고 UI추가
void Housing_WarehouseDlg_Func(LONG lId, void* p, DWORD we ) 
{
	GAMEIN->GetHousingWarehouseDlg()->OnActionEvent(lId, p, we);
}

//090409 pdy 하우징 하우스 검색 UI추가
void HouseSearchDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHouseSearchDlg()->OnActionEvent(lId, p, we);
}

//090410 pdy 하우징 하우스 이름설정 UI추가
void HouseNameDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHouseNameDlg()->OnActionEvent(lId, p, we);
}

//090414 pdy 하우징 꾸미기포인트 보상 UI추가
void HousingDecoPointDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingDecoPointDlg()->OnActionEvent(lId, p, we);
}

//090507 pdy 하우징 액션 팝업매뉴 UI추가
void HousingActionPopupMenuDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingActionPopupMenuDlg()->OnActionEvent(lId, p, we);
}

//090525 pdy 하우징 내집창고 버튼 UI추가
void HousingWarehouseButtonDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingWarehouseButtonDlg()->OnActionEvent(lId, p, we);
}

// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
void InputNameDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetInputNameDlg()->OnActionEvent(lId, p, we);
}

//090708 pdy 하우징 꾸미기모드 버튼 UI추가
void HousingDecoModeBtDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingDecoModeBtDlg()->OnActionEvent(lId, p, we);
}

//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경
void HousingMainPointDlgFunc(LONG lId, void* p, DWORD we )
{
	GAMEIN->GetHousingMainPointDlg()->OnActionEvent(lId, p, we);
}

//091223 NYJ 하우징 다른집방문
void HousingSearchDlgFunc(LONG lId, void* p, DWORD we ) 
{
	GAMEIN->GetHouseSearchDlg()->OnActionEvent(lId, p, we);
}

// 091210 ShinJS --- 녹화 Dialog 추가
void VideoCaptureDlgFunc(LONG lId, void* p, DWORD we )
{
	cDialog* pVideoCaptureDlg = (cDialog*)p;
	if( !pVideoCaptureDlg )
		return;

	switch( lId )
	{
		// 녹화 시작
	case VIDEOCAPTURE_START_BTN:
		{
			pVideoCaptureDlg->SetActive( FALSE );

			CMiniMapDlg* pMiniMapDlg = GAMEIN->GetMiniMapDialog();
			if( !pMiniMapDlg )
				break;

			pMiniMapDlg->ShowVideoCaptureStartBtn( FALSE );

			VIDEOCAPTUREMGR->CaptureStart();

			// 100111 ONS 해당맵에 미니맵이 없을경우 화면 좌상단에 [녹화중]이미지를 출력한다.
			if( !pMiniMapDlg->HaveMinimap() )
			{
				cAdditionalButtonDlg* pAdditionBtnDlg = GAMEIN->GetAdditionalButtonDlg();
				if( !pAdditionBtnDlg )
					break;

				pAdditionBtnDlg->SetActive(TRUE);
				pAdditionBtnDlg->ShowRecordStartBtn( FALSE );
			}
		}
		break;
		// 녹화 취소
	case VIDEOCAPTURE_CANCEL_BTN:
		{
			pVideoCaptureDlg->SetActive( FALSE );
		}
		break;
	}
}

// 100111 ONS 부가적인 버튼 다이얼로그 추가
void AdditionalButtonDlgFunc( LONG lId, void* p, DWORD we )
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(ADDITIONAL_BUTTON_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

// 100511 ONS 전직변경 다이얼로그 추가
void ChangeClassDlgFunc( LONG lId, void* p, DWORD we )
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CHANGE_CLASS_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

void ConsignmentDlgFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(CONSIGNMENT_DLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

// 100629 몬스터게이지 다이알로그
void MonsterTargetDlgFunc( LONG lId, void * p, DWORD we)
{
	if(CMonsterGuageDlg* const pMonsterGuageDlg = (CMonsterGuageDlg*)WINDOWMGR->GetWindowForID( CG_PLAYERGUAGEDLG ) )
	{
		if( cDialog*  pMonsterTargetDialog = pMonsterGuageDlg->GetCurTarget() )
		{
			pMonsterTargetDialog->OnActionEvent( 
				lId,
				p,
				we);
		}
	}
}

// 100709 ONS 퀘스트알리미 삭제버튼 처리 추가
void QuickView_DlgFunc(LONG lId, void * p, DWORD we)
{
	if(cDialog* const dialog = WINDOWMGR->GetWindowForID(QUE_QUICKVIEWDLG))
	{
		dialog->OnActionEvent(
			lId,
			p,
			we);
	}
}

void WorldMapDlgFunc(LONG lId, void * p, DWORD we)
{
	if( cDialog* const dialog = WINDOWMGR->GetWindowForID( WORLDMAP_DLG ) )
	{
		dialog->OnActionEvent( lId, p, we );
	}
}

void BigMapDlgFunc(LONG lId, void * p, DWORD we)
{
	if( p == NULL )
		return;

	((cDialog*)p)->OnActionEvent( lId, p, we );
}