#pragma once

/* 메인 타이틀 */
struct FUNC
{
	void (*_func)(LONG lId, void * p, DWORD we);
	const char * _funcName;
};
FUNC g_mt_func[];
int FUNCSEARCH(char * funcName);

// 로그인
void MT_LogInOkBtnFunc(LONG lId, void* p, DWORD we);
void MT_ExitBtnFunc(LONG lId, void* p, DWORD we);
void MT_EditReturnFunc(LONG lId, void* p, DWORD we);
void MT_DynamicBtnFunc(LONG lId, void* p, DWORD we);
void MT_DynamicEditReturnFunc(LONG lId, void* p, DWORD we);

// 플레이어 생성
void CS_BtnFuncCreateChar(LONG lId, void* p, DWORD we);
void CS_BtnFuncDeleteChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFirstChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncSecondChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncThirdChar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFourthchar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncFifthchar(LONG lId, void* p, DWORD we);	 
void CS_BtnFuncEnter(LONG lId, void* p, DWORD we);
void CS_FUNC_OkISee(LONG lId, void* p, DWORD we);
void CS_BtnFuncLogOut(LONG lId, void* p, DWORD we);
void CS_BtnFuncChangeServer( LONG lId, void* p, DWORD we );
void CTF_CBFunc( LONG lId, void* p, DWORD we ) ;
void CM_CharMakeBtnFunc(LONG lId, void* p, DWORD we);
void CM_CharCancelBtnFunc(LONG lId, void* p, DWORD we);
void MI_CharBtnFunc(LONG lId, void* p, DWORD we);
void MI_ExchangeBtnFunc(LONG lId, void* p, DWORD we);
void MI_InventoryBtnFunc(LONG lId, void* p, DWORD we);
void MI_SkillBtnFunc(LONG lId, void* p, DWORD we);
/*Inventory interface*/
void IN_DlgFunc(LONG lId, void * p, DWORD we);
/*Skill Dialog Interface*/
void MGI_DlgFunc(LONG lId, void * p, DWORD we);
void MGI_SkillDlgFunc(LONG lId, void * p, DWORD we);
void STD_SkillTreeDlgFunc(LONG lId, void * p, DWORD we);
void CI_DlgFunc(LONG lId, void * p, DWORD we);
void CI_AddExpPoint(LONG lId, void * p, DWORD we);

// 070111 LYW --- Add function to process events.
void CI_ActionEventFun( LONG lId, void* p, DWORD we ) ;
/*Quick Dialog Interface */
void QI_QuickDlgFunc(LONG lId, void * p, DWORD we);

// LYJ 051017 구입노점상 추가
void SO_DlgFunc(LONG lId, void * p, DWORD we);

void BRS_DlgFunc(LONG lId, void * p, DWORD we);
void BRS_CloseFunc(LONG lId, void * p, DWORD we);
void BRS_DeleteFunc(LONG lId, void * p, DWORD we);
void BRS_TypeListFunc(LONG lId, void * p, DWORD we);
void BRS_ItemListFunc(LONG lId, void * p, DWORD we);
void BRS_ClassListFunc(LONG lId, void * p, DWORD we);
void BRS_REGBtnFunc(LONG lId, void * p, DWORD we);

void BS_DlgFunc(LONG lId, void * p, DWORD we);
void BS_TitleEditBoxFunc(LONG lId, void * p, DWORD we);
void BS_SellBtnFunc(LONG lId, void * p, DWORD we);
/*StreetStall interface*/
void SSI_DlgFunc(LONG lId, void * p, DWORD we);
void SSI_TitleEditBoxFunc(LONG lId, void * p, DWORD we);
void SSI_BuyBtnFunc(LONG lId, void * p, DWORD we);
void SSI_EditBtnFunc(LONG lId, void * p, DWORD we);	//6->34

/*NPC 대사창*/
void NSI_HyperLinkFunc(LONG lId, void * p, DWORD we);

/*공통창*/
void CMI_MoneyOkFunc(LONG lId, void * p, DWORD we);
/*상점창*/
void DE_DlgFunc(LONG lId, void * p, DWORD we);
void DE_BuyItemFunc(LONG lId, void * p, DWORD we);
// 070503 LYW --- GlobalEventFunc : Add function to cancel sell dialog.
void DE_CancelFunc(LONG lId, void* p, DWORD we) ;
void DE_SellItemFunc(LONG lId, void * p, DWORD we);
void DE_CloseHideBtn(LONG lId, void * p, DWORD we); //7->41
/*파티*/
void PA_BtnFunc(LONG lId, void * p, DWORD we);

//080318 LUJ, 파티원 멤버 스킬 아이콘 표시 관리
void PartyMemberFunc( LONG lId, void * p, DWORD we );

/*길드*/
void GD_WarehouseFunc( LONG id, void* p, DWORD we );
void GD_DlgFunc( LONG id, void* p, DWORD we );
void GDMARK_DlgFunc( LONG id, void* p, DWORD we );
void GDLEVEL_DlgFunc( LONG id, void* p, DWORD we );
void GDRANK_DlgFunc( LONG id, void*, DWORD we );
void GDCREATE_DlgFunc( LONG id, void*, DWORD we );
void GDINVITE_DlgFunc( LONG id, void*, DWORD we );
void GDWHRANK_DlgFunc( LONG id, void*, DWORD we );
void GDREVIVAL_DlgFunc( LONG id, void*, DWORD we );
void GDUnion_DlgFunc( LONG id, void*, DWORD we );
void GDNICK_DlgFunc( LONG id, void*, DWORD we );

/*조합/분해/강화/인챈트를 위한 상태 진행 창*/
void PROGRESS_DlgFunc( LONG id, void*, DWORD we );

/*패밀리*/
void FD_DlgFunc( LONG lId, void* p, DWORD we );

/*표국*/
void PYO_DlgBtnFunc(LONG lId, void * p, DWORD we);
void Note_DlgBtnFunc(LONG lId, void * p, DWORD we);
//100118 ONS 친구관리 다이얼로그에 PC방정보관련 기능추가
void Friend_DlgFunc(LONG lId, void * p, DWORD we);

/*전체외치기*/
void Shout_DlgBtnFunc(LONG lId, void * p, DWORD we);

/*낚시시스템*/
void Fishing_DlgBtnFunc(LONG lId, void * p, DWORD we);

/*캐릭터 재생*/
void CR_DlgBtnFunc(LONG lId, void * p, DWORD we);
/*퀘스트*/
//void QUE_RegistQuestNotice(BOOL bManual);	// 080304 ggomgrak --- 퀘스트자동알리미 처리를 위해 추가 (현재 사용되지 않음)
void QUE_QuestDlgFunc( LONG lId, void* p, DWORD we );
void GAMEIN_ChatFunc(LONG lId, void * p, DWORD we);
/*옵션창*/
void OTI_DlgFunc(LONG lId, void * p, DWORD we);
/*종료창*/
void EXT_DlgFunc(LONG lId, void * p, DWORD we);
// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23
void FARM_BUY_DlgFunc(LONG lId, void * p, DWORD we);
void FARM_UPGRADE_DlgFunc(LONG lId, void * p, DWORD we);
void FARM_MANAGE_DlgFunc(LONG lId, void * p, DWORD we);
// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
void DATE_ZONE_LIST_DlgFunc(LONG lId, void * p, DWORD we);
void CHALLENGE_ZONE_LIST_DlgFunc(LONG lId, void * p, DWORD we);
void CHALLENGE_ZONE_CLEARNO1_DlgFunc(LONG lId, void * p, DWORD we);
// E 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24

/*교환창*/
void XCI_DlgFunc(LONG lId, void * p, DWORD we);
/*PKLOOTING*/
void PLI_DlgFunc( LONG lId, void* p, DWORD we );
/*CNA_BtnOKFunc*/
void CNA_BtnOkFunc( LONG lId, void* p, DWORD we );
void MessageBox_Func(LONG lId, void * p, DWORD we);
void DIVIDEBOX_Func(LONG lId, void * p, DWORD we);
void MONEYDIVIDEBOX_Func(LONG lId, void * p, DWORD we);
/*미니맵*/
void MNM_DlgFunc(LONG lId, void * p, DWORD we);

/*등급업 창*/
void ITD_DlgFunc(LONG lId, void * p, DWORD we);
/*조합 창*/
void ITMD_DlgFunc(LONG lId, void * p, DWORD we);
/* 강화 창*/
void ITR_DlgFunc(LONG lId, void * p, DWORD we);

// 080215 LUJ, 옵션 적용 창
void ApplyOptionDialogFunc(LONG lId, void * p, DWORD we);
// 080916 LUJ, 합성 창
void ComposeDialogFunc(LONG lId, void * p, DWORD we);
/*채널 창*/
void CHA_DlgBtnFunc(LONG lId, void * p, DWORD we);
// ServerList
void SL_DlgBtnFunc( LONG lId, void* p, DWORD we );

// guildfieldwar
void GFW_DlgFunc( LONG lId, void* p, DWORD we );
// Guild Tournament
void GDT_DlgFunc(LONG lId, void * p, DWORD we);
void CG_DlgFunc(LONG lId, void *p, DWORD we);
// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
void RFGUIDE_DLGFunc(LONG lId, void *p, DWORD we);
void GN_DlgBtnFunc(LONG lId, void *p, DWORD we);
void AN_DlgBtnFunc(LONG lId, void *p, DWORD we);

// 06. 03. 문파공지 - 이영준
void GNotice_DlgBtnFunc(LONG lId, void *p, DWORD we);

void GuildPlusTime_DlgFunc(LONG IId, void* p, DWORD we);//Add 060803 by wonju
// 061217 LYW --- Add function to call callback function.
void CM_CMNCbFunc( LONG lId, void* p, DWORD we) ;
void MDS_CBFunc( LONG lId, void* p, DWORD we ) ;
void CharMain_BtnFunc( LONG lId, void* p, DWORD we ) ;
void JO_DlgFunc(LONG lId, void* p, DWORD we);
void DMD_LogInOkBtnFunc( LONG lId, void* p, DWORD we ) ;		// 매칭 다이얼로그 테스트 2007/03/15 이진영
void DMD_DateMatchingDlgFunc( LONG lId, void* p, DWORD we ) ;		// 매칭 다이얼로그 테스트 2007/03/16 이진영
void DMD_PartnerDlgFunc( LONG lId, void* p, DWORD we ) ;

// 070606 LYW --- GlovalEventFunc : Add function to call callback function of identification dialog.
void DMD_IDT_Func(LONG lId, void* p, DWORD we) ;
// 070607 LYW --- GlovalEventFunc : Add function to call callback function of favor icon dialog.
void DMD_FID_Func(LONG lId, void* p, DWORD we) ;

// 070607 LYW --- GlovalEventFunc : Add function to call callback function of my info dialog.
void DMD_MID_Func(LONG lId, void* p, DWORD we) ;
// 070607 LYW --- GlovalEventFunc : Add function to call callback function of record info dialog.
void DMD_RID_Func(LONG lId, void* p, DWORD we) ;

// 070618 LYW --- GlovalEventFunc : Add function to call callback function of key setting dialog.
void KS_Func(LONG lId, void* p, DWORD we) ;

void SkillTrainDlgFunc( LONG lId, void* p, DWORD we );

// desc_hseos_패밀리01
// S 패밀리 추가 added by hseos 2007.07.02
void FD_GuildDlg(LONG IId, void* p, DWORD we);
// E 패밀리 추가 added by hseos 2007.07.02

// 071018 LYW --- GlovalEventFunc : Add event function for tutorial dialog.
void TTR_DlgFunc( LONG id, void* p, DWORD we ) ;

// 071023 LYW --- GlovalEventFunc : Add event function for tutorial button dialog.
void TTR_BTNDlgFunc( LONG id, void* p, DWORD we ) ;

// 071024 LYW --- GlovalEventFunc : Add event function for helper dialog.
void HD_DlgFunc(LONG lId, void* p, DWORD we ) ;

// 071130 LYW --- GlovalEventFunc : Add event function for map move dialog.
void MapMove_DlgFunc(LONG lId, void* p, DWORD we ) ;

// 071228 LYW --- GlovalEventFunc : Add event function for change name dialog.
void Change_Name_Func(LONG lId, void* p, DWORD we ) ;

// 080109 LYW --- GlovalEventFunc : 숫자 패드 다이얼로그 함수 추가.
void NumberPad_DlgFunc(LONG lId, void* p, DWORD we ) ;

//---KES AUTONOTE
void AutoNoteDlg_Func( LONG lId, void* p, DWORD we );
void AutoAnswerDlg_Func( LONG lId, void* p, DWORD we );
//---------------

//---KES 상점검색
void StoreSearchDlg_Func( LONG lId, void* p, DWORD we );
//---------------

// 080414 LUJ, 외양 변경 창
void BodyChangeFunc( LONG lId, void* p, DWORD we );


// 080403 LYW --- GlovalEventFunc : Add event functions for chatroom system.
void ChatRoomMainDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomCreateDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomGuestListDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomJoinDlg_Func(LONG lId, void* p, DWORD we ) ;
void ChatRoomOptionDlg_Func(LONG lId, void* p, DWORD we ) ;
//void ChatRoomLobbyDlg_Func(LONG lId, void* p, DWORD we ) ;

// 080414 KTH -- 가축 다이얼로그 추가
void Animal_DlgBtnFunc(LONG lId, void * p, DWORD we);

void PET_STATUS_DLG_FUNC(LONG lId, void* p, DWORD we ) ;
void PET_RES_DLG_FUNC(LONG lId, void* p, DWORD we ) ;

void PopupMenuDlgFunc(LONG lId, void* p, DWORD we ) ;

void COOK_DLG_FUNC(LONG lId, void* p, DWORD we ) ;
// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
void RidePopupMenuDlgFunc(LONG lId, void* p, DWORD we );
// 090415 ONS 전직 가이드 개선
void GradeClassDlgFunc(LONG lId, void* p, DWORD we ) ;

//090323 pdy 하우징 설치 UI추가
void HousingRevolDlgFunc(LONG lId, void* p, DWORD we );

//090330 pdy 하우징 창고 UI추가
void Housing_WarehouseDlg_Func(LONG lId, void* p, DWORD we ) ;

//090409 pdy 하우징 하우스 검색 UI추가
void HouseSearchDlgFunc(LONG lId, void* p, DWORD we ) ;

//090410 pdy 하우징 하우스 이름설정 UI추가
void HouseNameDlgFunc(LONG lId, void* p, DWORD we );

//090414 pdy 하우징 꾸미기포인트 보상 UI추가
void HousingDecoPointDlgFunc(LONG lId, void* p, DWORD we );

//090507 pdy 하우징 액션 팝업매뉴 UI추가
void HousingActionPopupMenuDlgFunc(LONG lId, void* p, DWORD we );

//090525 pdy 하우징 내집창고 버튼 UI추가
void HousingWarehouseButtonDlgFunc(LONG lId, void* p, DWORD we );

// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
void InputNameDlgFunc(LONG lId, void* p, DWORD we );

//090708 pdy 하우징 꾸미기모드 버튼 UI추가
void HousingDecoModeBtDlgFunc(LONG lId, void* p, DWORD we );

//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경
void HousingMainPointDlgFunc(LONG lId, void* p, DWORD we ) ;

//091223 NYJ 하우징 다른집방문
void HousingSearchDlgFunc(LONG lId, void* p, DWORD we ) ;

// 091210 ShinJS --- 녹화 Dialog 추가
void VideoCaptureDlgFunc(LONG lId, void* p, DWORD we );

// 100111 ONS 부가적인 버튼 다이얼로그 추가
void AdditionalButtonDlgFunc( LONG lId, void* p, DWORD we );

// 100511 ONS 전직변경 다이얼로그 추가
void ChangeClassDlgFunc( LONG lId, void* p, DWORD we );
// 100503 NYJ 판매대행
void ConsignmentDlgFunc(LONG lId, void * p, DWORD we);

// 몬스터타겟 다이알로그
void MonsterTargetDlgFunc(LONG lId, void * p, DWORD we);

// 100709 ONS 퀘스트알리미 삭제버튼 처리 추가
void QuickView_DlgFunc(LONG lId, void * p, DWORD we);

// 100628 ShinJS 월드맵 다이얼로그 추가
void WorldMapDlgFunc(LONG lId, void * p, DWORD we);

// 100714 ShinJS 빅맵 다이얼로그 추가
void BigMapDlgFunc(LONG lId, void * p, DWORD we);
