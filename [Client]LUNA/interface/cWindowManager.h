#pragma once


#include "cWindowDef.h"

class cWindow;
class cDialog;
class cMultiLineText;
class cMsgBox;
class cDivideBox;
class cMoneyDivideBox;
class CKeyboard;
class CMouse;
class cNumberPadDialog;
enum eMBType;

#define WINDOWMGR	USINGTON(cWindowManager)
typedef void (*cbDROPPROCESSFUNC)(LONG curDragX, LONG curDragY, LONG id, LONG beforeDragX, LONG beforeDragY);

class cWindowManager  
{
public:
	cWindowManager();
	virtual ~cWindowManager();

	void Init();
	void AfterInit();
	void Preprocess();
	void DestroyWindowProcess();
	void AddListDestroyWindow(cWindow * window); 
	void Process();
	void Render();
	cbDROPPROCESSFUNC cbDragnDropProcess;
	void SetcbDropProcess(cbDROPPROCESSFUNC func) { cbDragnDropProcess = func; }
	cDialog * GetWindowForID(LONG id);
	cWindow * GetWindowForIDEx(LONG id);
	cDialog * GetWindowForXYExceptIcon(LONG x, LONG y);
	void DestroyWindowAll();
	void DeleteWindowForID(LONG id);
	void DeleteWindow(cWindow*);
	void Release();

	cbWindowProcess cbProcess;

	cWindow * GetDlgInfoFromFile(char * filePath, char* mode = "rt");
	void CreateGameIn();
	void CreateCharDlg();
	void CreateSkillTreeDlg();
	void CreateInventoryDlg();
	void CreateExchangeDlg();
	void CreateRecvExchangeDlg();
	void CreateMixDlg();
	void CreateProgressDlog();
	void CreateDealDlg();
	void CreateChattingDlg();
	void CreateMainSystemDlg();
	void CreateComposeDlg();
	void CreateHeroGuage();
	void CreateQuickDlg();
	void CreatePartyDlg();
	void CreatePartyCreateDlg();
	void CreatePartyInviteDlg();
	void CreateGuildCreateDlg();
	void CreateGuildDlg();
	void CreateGuildInviteDlg();
	void CreateGuildMarkDlg();
	void CreateGuildLevelupDlg();
	void CreateGuildNickNameDlg();
	void CreateGuildRankDlg();
	void CreateGuildWarehouseDlg();
	void CreateStorageDlg();
	void CreateNoteDlg();
	void CreateFriendDlg();
	void CreateMiniFriendDlg();
	void CreateMiniMapDlg();
	void CreateMiniNoteDlgWrite();
	void CreateMiniNoteDlgRead();
	void CreateReviveDlg();
	void CreateQuestTotalDlg();
	void CreateQuestquickDlg() ;
	void CreateStallKindSelectDlg();
	void CreateStreetBuyStallDlg();
	void CreateBuyRegDlg();
	void CreateStreetStallDlg();
	void CreateNpcScriptDlg();
	void CreateMacroDlg();
	void CreateOptionDlg();
	void CreateExitDlg();
	void CreateEnchantDlg();
	void CreateReinforceDlg();
	void CreateDissolveDlg();

	// 080218 LUJ, 옵션 부여 창
	void CreateApplyOptionDlg();
	void CreateItemShopDlg();
	// guildfieldwar
	void CreateGuildFieldWarDlg();
	void CreateGTRegistDlg();
	void CreateGTRegistcancelDlg();
	void CreateGTStandingDlg();
	void CreateGTStandingDlg16();
	void CreateGTBattleListDlg();
	void CreateGTScoreInfoDlg();
	void CreateReinforceGuideDlg();
	void CreateKeySettingTipDlg();

	void CreateGuildNoteDlg();
	void CreateUnionNoteDlg();
	// 06. 03. 문파공지 - 이영준
	void CreateGuildNoticeDlg();

	// 070816 웅주, 길드 창고 이용권한 부여
	void CreateGuildWarehouseRankDlg();

	void CreateGuildInvitationKindSelectionDlg();
	void CreateNpcImageDlg();
	void CreateDateMatchingDlg();
	void CreateIdentificationDlg();
	void CreateFavorIconDlg();
	void CreateMonstermeterDlg();
	void CreateFarmBuyDlg();
	void CreateFarmUpgradeDlg();
	void CreateFarmManageDlg();
	void CreateDateZoneListDlg();
	void CreateChallengeZoneListDlg();
	void CreateChallengeZoneClearNo1Dlg();
	void CreateSkillTrainingDlg();
	void CreateFamilyCreateDlg();
	void CreateFamilyDlg();
	void CreateFamilyMarkDlg();
	void CreateFamilyNickNameDlg();
	void CreateLimitDungeonDlg();
	void CreateTutorialDlg();
	void CreateHelpDlg();
	void CreateTutorialBtnDlg();
	void CreateItemPopupDlg();
	void CreateMapMoveDlg();
	void CreateChangeNameDlg();
	void CreateAutoNoteDlg();
	void CreateAutoAnswerDlg();
	void CreateStoreSearchDlg();
	void CreatePetStorageDlg();
	void CreateShoutDlg();
	void CreateChatRoomMainDlg() ;
	void CreateChatRoomCreateDlg() ;
	void CreateChatRoomDlg() ;
	void CreateChatRoomGuestListDlg() ;
	void CreateChatRoomOptionDlg() ;
	void CreateChatRoomJoinDlg();
	void CreateFishingDlg();// 080421 NYJ --- 낚시 다이얼로그 추가
	void CreateFishingGaugeDlg(); // 080422 NYJ --- 낚시게이지 다이얼로그 추가
	void CreateFishingPointDlg(); // 080508 NYJ --- 낚시포인트 다이얼로그 추가

	// 080414 LUJ, 외양 변경 창
	void CreateBodyChangeDlg();

	// 080509 KTH -- 가축 관리 다이얼 로그
	void CreateFarmAnimalDlg();

	void CreatePetDialog();


	void CreateGTResultDlg();
	void CreateGTEntryEditDlg();

	void CreateCookDlg();

	// 080929 LYW --- cWindowManager : 공성 깃발 정보 다이얼로그 추가.
	void CreateSiegeWarFlagDlg() ;

	// 080930 LYW --- cWindowManager : 페이드 인/아웃 다이얼로그 추가.
	void CreateFadeDlg() ;
	void CreatePopupMenuDlg();
	// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
	void CreateRidePopupMenuDlg();
	// 090413 ONS 전직 가이드 다이얼로그 추가.
	void CreateGradeClassDlg();

	
	//090323 pdy 하우징 설치 UI추가
	void CreateHousingRevolDlg();

	//090330 pdy 하우징 창고 UI추가
	void CreateHousingWereHouseDlg();

	//090409 pdy 하우징 하우스 검색 UI추가
	void CreateHouseSearchDlg();

	//090410 pdy 하우징 하우스 이름설정 UI추가
	void CreateHouseNameDlg();

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	void CreateHousingDecoPointDlg();

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	void CreateHousingMainPointDlg();

	//090525 pdy 하우징 매인포인트 UI추가
	void CreateHousingActionPopupMenuDlg();

	//090525 pdy 하우징 내집창고 버튼 UI추가
	void CreateHousingWarehouseButtonDlg();

	//090622 NYJ 하우징 웹브라우저
	void CreateHousingWebDlg();

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	void CreateInputNameDlg();

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	void CreateHousingDecoModeBtDlg();

	// 090925 ShinJS --- Npc Notice Dialog 추가
	void CreateNpcNoticeDlg();
	// 091102 NYJ - 트리거 타이머
	void CreateTriggerTimerDlg();

	// 091210 ShinJS --- Video Capture Dialog 추가
	void CreateVideoCaptureDlg();

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	void CreateAdditionalButtonDlg();

	void GameRatingDlg();
	void CreateConsignmentGuideDlg();
	void CreateConsignmentDlg();

	// 100511 ONS 전직변경 다이얼로그 추가
	void CreateChangeClassDlg();

#ifdef _TW_LOCAL_
	void CreateItemShopBrowserDlg();
#endif

	cMsgBox* MsgBox(LONG lMBId, eMBType, LPCTSTR, ...);	
	cDivideBox* DivideBox( LONG lId, LONG x, LONG y, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void * vData2, char* strTitle );
	cMoneyDivideBox* MoneyDivideBox( LONG lId, LONG x, LONG y, cbDivideFUNC cbFc1, cbDivideFUNC cbFc2, void * vData1, void * vData2, char* strTitle );
	void CreatMousePoint();
	void AddWindow(cWindow*);
	void AddChatTooltip(cWindow* window) { m_pPlayerChatTooltip.AddTail(window); }
	void RemoveChatTooltipWindow(cWindow* window) { m_pPlayerChatTooltip.Remove(window); }

	cPtrList m_pDestroyWindowRef;
	cPtrList m_pWindowList;
	cPtrList m_pIconDlgListRef;	// reference
	cPtrList m_pEditBoxListRef;	// for tab and shift+tab key	// reference
	cPtrList m_pPlayerChatTooltip;
	cPtrList m_pActivedWindowList;

	void SetDragStart(LONG sX, LONG sY, LONG x, LONG y, LONG id)
	{
		m_id = id;
		m_pDragDialog = GetWindowForID( id );
		m_OldX = x; m_OldY = y;
		m_posBeforeDrag.x=(float)sX;
		m_posBeforeDrag.y=(float)sY;
		m_OldDragFlag = TRUE;
	}
	void SetDragEnd()
	{
		m_id=-1;
		m_pDragDialog = NULL;
		m_OldX = -1; m_OldY = -1;
	}

	void BackDragWindow();
	void DragWindowNull()
	{
		m_pDragDialog = NULL;
	}

	BOOL IsDragWindow() { return m_OldDragFlag;	}
	
	//090119 pdy Window Hide Mode
	BOOL HideAllWindow();				//Active를 건드리지 않고 숨김처리 
	void UnHideAllActivedWindow() ;		//숨김해제 								

	BOOL CloseAllWindow();
	void ShowBaseWindow();

	// 070124 LYW --- Add functino to save windows that actived.
	void SaveAllActivedWindow( cWindow* win ) ;
	void ShowAllActivedWindow() ;

	void SetOpendAllWindows( BOOL val ) { m_bOpendAllWindows = val ; }
	BOOL IsOpendAllWindows() { return m_bOpendAllWindows ; }


public:
	void ToggleShowInterface();
	
	BOOL m_OldDragFlag;
	void SetFocusedEdit( cWindow* pEdit, BOOL bFocus );
	void SetNextEditFocus();
	void SetWindowTop( cWindow* win );
	void SetToolTipWindow( cMultiLineText* toolTip ) { m_pToolTipWindow = toolTip; }

//KES INTERFACE 031008
	BOOL IsMouseInputProcessed()	{ return m_MouseInputProcessed; }

	BOOL IsMouseOverUsed()			{ return m_bMouseOverUsed; }
	BOOL IsMouseDownUsed()			{ return m_bMouseDownUsed; }
	void SetMouseInputProcessed()	{ m_MouseInputProcessed = TRUE; }
	void SetMouseOverUsed()			{ m_bMouseOverUsed = TRUE; }
	void SetMouseDownUsed()			{ m_bMouseDownUsed = TRUE; }
	void KeyboardInput( CKeyboard* keyInfo );
	void MouseInput( CMouse* pMouse );
	
	cMsgBox* GetFirstMsgBox();
	void CloseAllMsgBox();
	void CloseAllMsgBoxNoFunc();
	void PositioningDlg( LONG& rX, LONG& rY );
	BOOL CloseAllAutoCloseWindows();
	cDialog* GetDragDlg() { return m_pDragDialog; }
	BOOL IsInterfaceHided() const { return m_bHideInterface; }

protected:

	BOOL			m_MouseInputProcessed;
	cWindow*		m_pFocusedEdit;
	cMultiLineText* m_pToolTipWindow;
	BOOL			m_bMouseOverUsed;
	BOOL			m_bMouseDownUsed;
	LONG		m_id;
	cDialog*	m_pDragDialog;
	LONG		m_OldX;
	LONG		m_OldY;
	VECTOR2		m_posBeforeDrag;
	BOOL		m_bHideInterface;
	BOOL		m_bOpendAllWindows;
};

EXTERNGLOBALTON(cWindowManager);
