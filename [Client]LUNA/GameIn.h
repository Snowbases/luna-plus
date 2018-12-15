#pragma once


#include "GameState.h"
#include "PartyDialog.h"

class CInventoryExDialog;
class CExchangeDialog;
class CMixDialog;
class CDealDialog;
class CCharacterDialog;
// 080916 LUJ, 합성 창 반환위해 선언
class CComposeDialog;
class CDebugDlg;

// LYJ 051017 구입노점상 추가
class CStallKindSelectDlg;
class CStreetBuyStall;
class CBuyRegDialog;

class CStreetStall;
class cNpcScriptDialog;

class CChatOptionDialog;
class COptionDialog;
class cDialog;
class CMoneyDlg;
class cQuickDlg;
class CExitDialog;

// desc_hseos_몬스터미터_01
// S 몬스터미터 추가 added by hseos 2007.04.09
class CSHMonstermeterDlg;
// E 몬스터미터 추가 added by hseos 2007.04.09

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23	
class CSHFarmBuyDlg;
class CSHFarmUpgradeDlg;
class CSHFarmManageDlg;
// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

// desc_hseos_데이트 존_01
// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
class CSHDateZoneListDlg;
class CSHChallengeZoneListDlg;
class CSHChallengeZoneClearNo1Dlg;
// E 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24

class CGuildCreateDialog;
class CGuildDialog;
class CGuildInviteDialog;
class CGuildMarkDialog;
class CGuildLevelUpDialog;
class CGuildNickNameDialog;
class CGuildRankDialog;
class CGuildWarehouseDialog;
class CGuildUnionCreateDialog;
class CMiniMapDlg;
class CStorageDialog;
class CNoteDialog;
class CFriendDialog;
class CMiniNoteDialogWrite;
class CMiniNoteDialogRead;
class CMiniFriendDialog;
class CReviveDialog;
class CMonsterGuageDlg;
class CReinforceDlg;
class CWantNpcDialog;
class CWantRegistDialog;
class CQuestTotalDialog;
class CQuestDialog;
class CQuestQuickViewDialog;
class CGameNotifyDlg ;
class CPartyCreateDlg;
class CPartyInviteDlg;
class CDissolveDialog;
class CItemShopDialog;
class CMHMap;
class CMHCamera;

class CGFWarDeclareDlg;
class CGFWarResultDlg;
class CGFWarInfoDlg;
class CGuildWarInfoDlg;
class CPartyWarDialog;

class CShoutDialog;
class CFishingDialog;
class CFishingGaugeDialog;
class CFishingPointDialog;
class CBigMapDlg;
class CGTStandingDialog;
class CGTBattleListDialog;
class CGTScoreInfoDialog;
class CGuageDialog;
//class CReinforceResetDlg;
//class CRareCreateDialog;

// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
class CReinforceGuideDialog;
class CKeySettingTipDlg;

class CGuildNoteDlg;
class CUnionNoteDlg;

class CGuildNoticeDlg;
class CGuildJoinDialog;

class CScreenShotDlg;

class cSkillTreeDlg;
// 061127 LYW --- Add New Chatting class.
class CChattingDlg ;
// 061130 LYW --- Add New Outside Chatting class.
class COSChattingDlg ;
// 061219 LYW --- Include main system dialog's class.
class CMainSystemDlg ;

class cNpcImageDlg;

class CDateMatchingDlg;			// 데이트 매칭 다이얼로그.	2007/03/26 - 이진영
// 070605 LYW --- GameIn : Include class of identification part.
class CIdentification ;
class CFavorIcon ;
class CDateMatchingInfoDlg ;
class CDateMatchingRecordDlg ;
class CFamilyCreateDialog;
class CFamilyDialog;
class CFamilyMarkDialog;
class CFamilyNickNameDialog;
class cSkillTrainingDlg;

// 071017 LYW --- GameIn : Add class tutorial dialog.
class cTutorialDlg ;

// 071017 LYW --- GameIn : Add class help dialog.
class cHelperDlg ;

// 071023 LYW --- GameIn : Add class tutorial button dialog.
class cTutorialBtnDlg ;
class cItemPopupDlg ;

// 071201 LYW --- GameIn : Add class mapmove dialog.
class cMapMoveDialog ;

// 071227 LYW --- GameIn : 캐릭터 이름 변경 추가.
class cChangeNameDialog ;

// 080109 LYW --- cWindowManager : 숫자 패드 다이얼로그 추가.
class cNumberPadDialog ;

//---KES AUTONOTE
class CAutoNoteDlg;
class CAutoAnswerDlg;
//--------------
//---KES 상점검색
class CStoreSearchDlg;
//---------------

// 080403 LYW --- GameIn : Declare interfaces for chatroom system.
class CChatRoomMainDlg ;
class CChatRoomCreateDlg ;
class CChatRoomDlg ;
class CChatRoomGuestListDlg ;
class CChatRoomOptionDlg ;
class CChatRoomJoinDlg ;
//class CChatRoomLobbyDlg ;

// 080509 KTH --
class CAnimalDialog;

class CPetInfoDialog;
class CPetStateDialog;
class CPetWearedDialog;
class CPetInvenDialog;
class CPetResurrectionDialog;

class CGTResultDlg;
class CGTStandingDialog16;
class CGTEntryEditDlg;

class CCookDlg;

// 080929 LYW --- GameIn : 공성 깃발 정보 다이얼로그 추가.
class CSiegeWarFlagDlg ;

// 080930 LYW --- GameIn : 페이드 인/아웃 다이얼로그 추가.
class CFadeDlg ;

class CPopupMenuDlg;
// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
class CRidePopupMenuDlg;
// 090413 ONS 전직 가이드 개선
class CGradeClassDlg;

//090323 pdy 하우징 설치 UI추가
class CHousingRevolDlg;

//090330 pdy 하우징 창고 UI추가
class cHousingWarehouseDlg;

//090409 pdy 하우징 하우스 검색 UI추가
class cHouseSearchDlg;

//090410 pdy 하우징 이름설정 UI추가
class cHouseNameDlg;

//090414 pdy 하우징 꾸미기포인트 보상 UI추가
class cHousingDecoPointDlg;

//090507 pdy 하우징 액션 팝업매뉴 UI추가
class cHousingActionPopupMenuDlg;

//090525 pdy 하우징 매인포인트 UI추가
class cHousingMainPointDlg;

//090525 pdy 하우징 내집창고 버튼 UI추가
class cHousingWarehouseButtonDlg;

//090622 NYJ 하우징 웹브라우저
class cHousingWebDlg;

// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
class CInputNameDlg;

//090708 pdy 하우징 꾸미기모드 버튼 UI추가
class cHousingDecoModeBtDlg;
// 091102 NYJ - 트리거 타이머
class CTriggerTimerDlg;

// 091210 ShinJS --- VideoCaptureDlg 추가
class CVideoCaptureDlg;

// 100111 ONS 부가적인 버튼 다이얼로그 추가
class cAdditionalButtonDlg;

// 100511 ONS 전직변경 다이얼로그 추가
class CChangeClassDlg;
class CConsignmentDlg;

class CWorldMapDlg;

class cBattleGuageDlg;
#ifdef _TW_LOCAL_
class CItemShopDlg;
#endif

#define GAMEIN USINGTON(CGameIn)

class CGameIn : public CGameState  
{
	///////////////////// Dialog Pointer //////////////////////////////////
	cSkillTreeDlg*		m_pSkillTreeDlg;

	CInventoryExDialog	* m_pInventoryDlg;
	CExchangeDialog		* m_pExchangeDlg;
//	CRecvExchangeDialog	* m_pRecvExchangeDlg;
	CMixDialog			* m_pMixDlg;
	CDealDialog			* m_pDealDlg;

	CCharacterDialog	* m_pCharDlg;
	CDebugDlg			* m_pDebugDlg;

// LYJ 051017 구입노점상 추가
	CStallKindSelectDlg * m_pStallKindSelectDlg;
	CStreetBuyStall		* m_pStreetBuyStallDlg;
	CBuyRegDialog		* m_pBuyRegDlg;
	
// LBS 03.10.16
	CStreetStall		* m_pStreetStallDlg;
	cNpcScriptDialog	* m_pNpcScriptDlg;

//KES
//	CMacroModeDialog	* m_pMacroModeDlg;
	CChatOptionDialog	* m_pChatOptionDlg;
	COptionDialog		* m_pOptionDlg;
	CExitDialog			* m_pExitDlg;

	// desc_hseos_몬스터미터_01
	// S 몬스터미터 추가 added by hseos 2007.04.09
	CSHMonstermeterDlg	*m_pMonstermeterDlg;
	// E 몬스터미터 추가 added by hseos 2007.04.09

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23
	CSHFarmBuyDlg		*m_pFarmBuyDlg;
	CSHFarmUpgradeDlg	*m_pFarmUpgradeDlg;
	CSHFarmManageDlg	*m_pFarmManageDlg;
	// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
	CSHDateZoneListDlg			*m_pDateZoneListDlg;
	CSHChallengeZoneListDlg		*m_pChallengeZoneListDlg;
	CSHChallengeZoneClearNo1Dlg	*m_pChallengeZoneClearNo1Dlg;
	CDissolveDialog	* m_pDissolveDig;
	CMoneyDlg			* m_MoneyDlg;
	cQuickDlg		* m_pQuickDlg;
	CGuageDialog		* m_pGuageDlg;

	CPartyDialog		m_PartyDlg;
	CPartyCreateDlg		* m_pPartyCreateDlg;
	CPartyInviteDlg		* m_pPartyInviteDlg;

	CGuildCreateDialog	* m_pGuildCreateDlg;
	CGuildDialog		* m_pGuildDlg;
	CGuildInviteDialog	* m_pGuildInviteDlg;
	CGuildMarkDialog	* m_pGuildMarkDlg;
	CGuildLevelUpDialog	* m_pGuildLevelUpDlg;
	CGuildNickNameDialog* m_pGuildNickNameDlg;
	CGuildRankDialog	* m_pGuildRankDlg;
	CGuildWarehouseDialog * m_pGuildWarehouseDlg;
	CGuildUnionCreateDialog * m_pGuildUnionCreateDlg;

	CMiniMapDlg			* m_pMiniMapDlg;
	CBigMapDlg			* m_pBigMapDlg;
	CWorldMapDlg		* m_pWorldMapDlg;

	CStorageDialog		* m_pStorageDialog;
	CNoteDialog			* m_pNoteDialog;
	CFriendDialog		* m_pFriendDialog;
	CMiniNoteDialogWrite		* m_pMiniNoteDialogWrite;
	CMiniNoteDialogRead			* m_pMiniNoteDialogRead;
	CMiniFriendDialog	* m_pMiniFriendDialog;
	CReviveDialog		* m_pReviveDialog;
	CWantNpcDialog		* m_pWantNpcDialog;
	CWantRegistDialog	* m_pWantRegistDialog;
	CMonsterGuageDlg	* m_pMonsterGuageDlg;
	CReinforceDlg		* m_pReinforceDlg;

	CQuestTotalDialog			* m_pQuestTotalDlg;
	CQuestDialog				* m_pQuestDlg;
	CQuestQuickViewDialog		* m_pQuestQuickViewDlg ;

	CGameNotifyDlg				* m_pGameNotifyDlg ;
	CItemShopDialog		* m_pItemShopDialog;
	// guildfieldwar
	CGFWarDeclareDlg*	m_pGFWarDeclareDlg;
	CGFWarResultDlg*	m_pGFWarResultDlg;
	CGFWarInfoDlg*		m_pGFWarInfoDlg;
	CGuildWarInfoDlg*	m_pGuildWarInfoDlg;
	CPartyWarDialog*	m_pPartyWarDlg;

	// 외치기
	CShoutDialog*		m_pShoutDlg;
	// 061204 LYW --- Delete Preexistence Chatting Dialog.
	/*
	CShoutchatDialog*	m_pShoutchatDlg;
	*/

	// 낚시
	CFishingDialog*		m_pFishingDlg;
	CFishingGaugeDialog* m_pFishingGaugeDlg;
	CFishingPointDialog* m_pFishingPointDlg;
	// Guild Tournament
	CGTStandingDialog*			m_pGTStandingDlg;
	CGTStandingDialog16*		m_pGTStandingDlg16;
	CGTBattleListDialog*		m_pGTBattleListDlg;
	CGTScoreInfoDialog*			m_pGTScoreInfoDlg;	
	// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
	CReinforceGuideDialog*		m_pReinforceGuideDlg;
	CKeySettingTipDlg*			m_pKeySettingTipDlg;

	CGuildNoteDlg*				m_pGuildNoteDlg;
	CUnionNoteDlg*				m_pUnionNoteDlg;

	CGuildNoticeDlg*			m_pGuildNoticeDlg;
	CGuildJoinDialog* m_pGuildInvitationKindSelectDlg;

	//스크린샷
	CScreenShotDlg*			m_pScreenShotDlg;
	// 061127 LYW --- Add New Chatting Class Pointer In GameIn class.
	CChattingDlg*			m_pChattingDlg ;
	// 061130 LYW --- Add New Outside Chatting Clas Pointer In GameIn class.
	COSChattingDlg*			m_pOSChattingDlg ;
	// 061219 LYW --- Add main system dialog.
	CMainSystemDlg*			m_pMainSystemDlg ;

	cNpcImageDlg*			m_pNpcImageDlg;

	CDateMatchingDlg*		m_pDateMatchingDlg;			// 데이트 매칭 다이얼로그  2007/03/26 - 이진영

	// 070605 LYW --- GameIn : Add identification dialog.
	CIdentification*		m_pIdentificationDlg ;
	// 070605 LYW --- GameIn : Add favor icon dialog.
	CFavorIcon*				m_pFavorIconDlg ;
	// 070607 LYW --- GameIn : Add my info dialog.
	CDateMatchingInfoDlg*	m_pMatchMyInfoDlg ;
	// 070607 LYW --- GameIn : Add partner info dialog.
	CDateMatchingInfoDlg*	m_pMatchPartnerInfoDlg ;
	// 070607 LYW --- GameIn : Add record dialog.
	CDateMatchingRecordDlg*	m_pMatchRecordDlg ;

	CFamilyCreateDialog	* m_pFamilyCreateDlg;
	CFamilyDialog		* m_pFamilyDlg;
	CFamilyMarkDialog	* m_pFamilyMarkDlg;
	CFamilyNickNameDialog* m_pFamilyNickNameDlg;

	cSkillTrainingDlg*		m_pSkillTrainingDlg;

	// 071017 LYW --- GameIn : Add memder to tutorial dialog.
	cTutorialDlg*			m_TutorialDlg ;

	// 071017 LYW --- GameIn : Add memder to help dialog.
	cHelperDlg*				m_HelpDlg ;

	// 071023 LYW --- GameIn : Add member to tutorial button dialog.
	cTutorialBtnDlg*		m_TutorialBtnDlg ;

	cItemPopupDlg*			m_ItemPopupDlg ;

	// 071130 LYW --- GameIn : Add member to map move dialog.
	cMapMoveDialog*			m_MapMoveDlg ;

	// 071227 LYW --- GameIn : 캐릭터 이름변경창 추가.
	cChangeNameDialog*		m_pChangeNameDlg ;

	// 080109 LYW --- GameIn : 숫자 패드 다이얼로그 추가.
	cNumberPadDialog*		m_pNumberPadDlg ;

	//---KES AUTONOTE
	CAutoNoteDlg*			m_pAutoNoteDlg;
	CAutoAnswerDlg*			m_pAutoAnswerDlg;
	//--------------
	//---KES 상점검색
	CStoreSearchDlg*		m_pStoreSearchDlg;
	//---------------

	// 080403 LYW --- GameIn : Add dialogs for chatroom system.
	CChatRoomMainDlg*		m_pChatRoomMainDlg ;
	CChatRoomCreateDlg*		m_pChatRoomCreateDlg ;
	CChatRoomDlg*			m_pChatRoomDlg ;
	CChatRoomGuestListDlg*	m_pChatRoomGuestListDlg ;
	CChatRoomOptionDlg*		m_pChatRoomOptionDlg ;
	CChatRoomJoinDlg*		m_pChatRoomJoinDlg ;
	//CChatRoomLobbyDlg*		m_pChatRoomLobbyDlg ;

	// 080509 KTH -- GameIn : Add dialog for Animal control system
	CAnimalDialog*			m_pAnimalDialog;

	CPetInfoDialog*			mPetInfoDialog;
	CPetStateDialog*		mPetStateDialog;
	CPetWearedDialog*		mPetWearedDialog;
	CPetInvenDialog*		mPetInvenDialog;
	cDialog*				mPetUIDialog;
	CPetResurrectionDialog*	mPetResurrectionDialog;

	CGTResultDlg*			m_pGTResultDlg;
	CGTEntryEditDlg*		m_pGTEntryEditDlg;

	// 080929 LYW --- GameIn : 공성 깃발 정보 다이얼로그 추가.
	CSiegeWarFlagDlg*		m_pSiegeFlagDlg ;

	// 080930 LYW --- GameIn : 페이드 인/아웃 다이얼로그 추가.
	CFadeDlg*				m_pFadeDlg ;

	CPopupMenuDlg*			m_pPopupMenuDlg;
	CCookDlg*				m_pCookDlg;
	// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 추가
	CRidePopupMenuDlg*		m_pRidePopupMenuDlg;
	// 090413 ONS 전직 가이드 개선
	CGradeClassDlg*			m_pGradeClassDlg;

	//090323 pdy 하우징 설치 UI추가
	CHousingRevolDlg*		m_pHousingRevolDlg;

	//090330 pdy 하우징 창고 UI추가
	cHousingWarehouseDlg*					m_pHousingWarehouseDlg;

	//090409 pdy 하우징 하우스 검색 UI추가
	cHouseSearchDlg*		m_pHouseSearchDlg;

	//090410 pdy 하우징 하우스 이름설정 UI추가
	cHouseNameDlg*			m_pHouseNameDlg;

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	cHousingDecoPointDlg*	m_pHousingDecoPointDlg;

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	cHousingActionPopupMenuDlg* m_pHousingActionPopupMenuDlg;

	//090525 pdy 하우징 매인포인트 UI추가
	cHousingMainPointDlg*	 m_pHousingMainPointDlg;

	//090525 pdy 하우징 내집창고 버튼 UI추가
	cHousingWarehouseButtonDlg* m_pHousingWarehouseButtonDlg;

	//090622 NYJ 하우징 웹브라우저
	cHousingWebDlg*			m_pHousingWebDlg;

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	CInputNameDlg*			m_pInputNameDlg;

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	cHousingDecoModeBtDlg* m_pHousingDecoModeBtDlg;
	// 091102 NYJ - 트리거 타이머
	CTriggerTimerDlg* m_pTriggerTimerDlg;

	// 091210 ShinJS --- VideoCaptureDlg 추가
	CVideoCaptureDlg*		m_pVideoCaptureDlg;

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	cAdditionalButtonDlg*		m_pAdditionalButtonDlg;

	// 100511 ONS 전직변경 다이얼로그 추가
	CChangeClassDlg*			m_pChangeClassDlg;
	CConsignmentDlg*		m_pConsignmentDlg;

	cBattleGuageDlg*		m_pBattleGuageDlg;
#ifdef _TW_LOCAL_
	CItemShopDlg*			m_pItemShopDlg;
#endif

	BOOL	m_bInitForGame;
	int		m_GameInInitKind;	
	DWORD	m_MovePoint;
	DWORD	m_MoveMap;
	DWORD	m_DestMoveMap;
	DWORD	m_GuildIdx;
	DWORD	m_BattleIdx;
	DWORD	m_SiegeWarMapNum;
	BOOL	m_bAfterRender;

public:	
	BOOL m_bGameInAcked;
	CGameIn();
	virtual ~CGameIn();
	BOOL Init(LPVOID parameter);
	void Release(CGameState*);
	BOOL InitForGame(int GameInInitKind);
	void ReleaseForGame();
	void Process();
	void BeforeRender();
	void AfterRender();
	void NetworkMsgParse(BYTE Category, BYTE Protocol, LPVOID, DWORD dwMsgSize);
	BOOL IsGameInAcked() const { return m_bGameInAcked;	}
	int GetGameInInitKind()	const { return m_GameInInitKind; }
	// 100120 ShinJS --- InterFace 출력 여부 설정(AfterRender 실행 여부 결정)
	void SetAfterRender( BOOL bRender ) { m_bAfterRender = bRender; }
	virtual void OnDisconnect();

	DWORD GetMoveMap()			{	 return m_MoveMap;	}
	void SetGuildIdx( DWORD GuildIdx )		{	m_GuildIdx = GuildIdx;	}
	void SetMoveMap( DWORD MapNum )			{	m_MoveMap = MapNum;		}
	void SetBattleIdx( DWORD BattleIdx )	{	m_BattleIdx = BattleIdx;	}
	void SetSiegeWarMapNum( DWORD MapNum )	{	m_SiegeWarMapNum = MapNum;	}

	void SetDestMoveMap(DWORD DestMapNum)	{	m_DestMoveMap = DestMapNum;	}
	
	///// custom
	CMonsterGuageDlg	*	GetMonsterGuageDlg()	{ return m_pMonsterGuageDlg;	}	
	void	SetMonsterGuageDlg(CMonsterGuageDlg	* dlg)	{ m_pMonsterGuageDlg = dlg;	}

	cBattleGuageDlg     *	GetBattleGuageDlg()		{ return m_pBattleGuageDlg; }
	void	SetBattleGuageDlg( cBattleGuageDlg* pBattleGuageDlg ) { m_pBattleGuageDlg = pBattleGuageDlg; }

	CCharacterDialog * GetCharacterDialog(){ return m_pCharDlg; }
	void SetCharacterDialog(CCharacterDialog * dlg){ m_pCharDlg=dlg; }
	void SetDebugDlg(CDebugDlg* Dlg){m_pDebugDlg = Dlg;}
	CDebugDlg* GetDebugDlg(){return m_pDebugDlg;}

	cSkillTreeDlg* GetSkillTreeDlg() { return m_pSkillTreeDlg; }
	void SetSkillTreeDlg( cSkillTreeDlg* dlg ) { m_pSkillTreeDlg = dlg; }

    CInventoryExDialog * GetInventoryDialog(){ return m_pInventoryDlg; }
	void SetInventoryDialog(CInventoryExDialog * dlg){ m_pInventoryDlg=dlg; }
	CExchangeDialog * GetExchangeDialog(){ return m_pExchangeDlg; }
	void SetExchangeDialog(CExchangeDialog * dlg){ m_pExchangeDlg=dlg; }
	// 080916 LUJ, 합성 창 반환
	CComposeDialog* GetComposeDialog();
	CMixDialog * GetMixDialog(){ return m_pMixDlg; }
	void SetMixDialog(CMixDialog * dlg){ m_pMixDlg=dlg; }
	CReinforceDlg * GetReinforceDialog(){ return m_pReinforceDlg; }
	void SetReinforceDialog(CReinforceDlg * dlg){ m_pReinforceDlg=dlg; }
		
	CDealDialog * GetDealDialog(){ return m_pDealDlg; }
	void SetDealDialog(CDealDialog * dlg){ m_pDealDlg=dlg; }

	// LYJ 051017 구입노점상 추가
	CStallKindSelectDlg * GetStallKindSelectDialog(){ return m_pStallKindSelectDlg; }
	void SetStallKindSelectDialog(CStallKindSelectDlg * dlg){ m_pStallKindSelectDlg=dlg; }	
	CStreetBuyStall * GetStreetBuyStallDialog(){ return m_pStreetBuyStallDlg; }
	void SetStreetBuyStallDialog(CStreetBuyStall * dlg){ m_pStreetBuyStallDlg=dlg; }
	CBuyRegDialog * GetBuyRegDialog(){ return m_pBuyRegDlg; }
	void SetBuyRegDialog(CBuyRegDialog * dlg){ m_pBuyRegDlg=dlg; }
	
	
// LBS
	CStreetStall * GetStreetStallDialog(){ return m_pStreetStallDlg; }
	void SetStreetStallDialog(CStreetStall * dlg){ m_pStreetStallDlg=dlg; }
	cNpcScriptDialog* GetNpcScriptDialog() { return m_pNpcScriptDlg; }
	void SetNpcScriptDialog(cNpcScriptDialog* dlg) { m_pNpcScriptDlg = dlg; }
////
	CMoneyDlg* GetMoneyDialog()	{	return m_MoneyDlg;	}
	void SetMoneyDialog(CMoneyDlg* dlg)	{	m_MoneyDlg = dlg;	}
	cQuickDlg* GetQuickDlg()	{	return m_pQuickDlg;	}
	void SetQuickDlg(cQuickDlg* dlg)	{	m_pQuickDlg = dlg;	}

//KES
//	CMacroModeDialog * GetMacroModeDialog(){ return m_pMacroModeDlg; }
//	void SetMacroModeDialog(CMacroModeDialog * dlg){ m_pMacroModeDlg=dlg; }
	CChatOptionDialog * GetChatOptionDialog() { return m_pChatOptionDlg; }
	void SetChatOptionDialog(CChatOptionDialog * dlg) { m_pChatOptionDlg = dlg; }	
	COptionDialog * GetOptionDialog() { return m_pOptionDlg; }
	void SetOptionDialog(COptionDialog * dlg) { m_pOptionDlg = dlg; }
	CExitDialog * GetExitDialog() { return m_pExitDlg; }
	void SetExitDialog(CExitDialog * dlg) { m_pExitDlg = dlg; }

	// desc_hseos_몬스터미터_01
	// S 몬스터미터 추가 added by hseos 2007.04.09
	CSHMonstermeterDlg *GetMonstermeterDlg() { return m_pMonstermeterDlg; }
	void SetMonstermeterDlg(CSHMonstermeterDlg * dlg) { m_pMonstermeterDlg = dlg; }
	// E 몬스터미터 추가 added by hseos 2007.04.09

	// desc_hseos_농장시스템_01
	// S 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23
	CSHFarmBuyDlg *GetFarmBuyDlg() { return m_pFarmBuyDlg; }
	void SetFarmBuyDlg(CSHFarmBuyDlg * dlg) { m_pFarmBuyDlg = dlg; }

	CSHFarmUpgradeDlg *GetFarmUpgradeDlg() { return m_pFarmUpgradeDlg; }
	void SetFarmUpgradeDlg(CSHFarmUpgradeDlg * dlg) { m_pFarmUpgradeDlg = dlg; }

	CSHFarmManageDlg *GetFarmManageDlg() { return m_pFarmManageDlg; }
	void SetFarmManageDlg(CSHFarmManageDlg * dlg) { m_pFarmManageDlg = dlg; }
	// E 농장시스템 추가 added by hseos 2007.08.24	2007.09.10	2007.10.23

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.14  2007.11.15	2008.01.24
	CSHDateZoneListDlg *GetDateZoneListDlg() { return m_pDateZoneListDlg; }
	void SetDateZoneListDlg(CSHDateZoneListDlg * dlg) { m_pDateZoneListDlg = dlg; }

	CSHChallengeZoneListDlg *GetChallengeZoneListDlg() { return m_pChallengeZoneListDlg; }
	void SetChallengeZoneListDlg(CSHChallengeZoneListDlg * dlg) { m_pChallengeZoneListDlg = dlg; }

	CSHChallengeZoneClearNo1Dlg *GetChallengeZoneClearNo1Dlg() { return m_pChallengeZoneClearNo1Dlg; }
	void SetChallengeZoneClearNo1Dlg(CSHChallengeZoneClearNo1Dlg * dlg) { m_pChallengeZoneClearNo1Dlg = dlg; }
	
	void SetDissolveDialog( CDissolveDialog* pDlg ) { m_pDissolveDig = pDlg; }
	CDissolveDialog* GetDissolveDialog() { return m_pDissolveDig; }

	CGuageDialog * GetGuageDlg(){ return m_pGuageDlg; }
	void SetGuageDlg(CGuageDialog * dlg){ m_pGuageDlg=dlg; }

	CPartyDialog* GetPartyDialog() { return &m_PartyDlg; }
	
	CPartyCreateDlg* GetPartyCreateDialog() { return m_pPartyCreateDlg;	}
	void SetPartyCreateDialog(CPartyCreateDlg* dlg)	{ m_pPartyCreateDlg = dlg;	}

	CPartyInviteDlg* GetPartyInviteDialog() { return m_pPartyInviteDlg;	}
	void SetPartyInviteDialog(CPartyInviteDlg* dlg)	{ m_pPartyInviteDlg = dlg;	}
	
	CGuildCreateDialog* GetGuildCreateDlg() { return m_pGuildCreateDlg; }
	void SetGuildCreateDlg(CGuildCreateDialog* dlg) { m_pGuildCreateDlg = dlg;	}

	CGuildUnionCreateDialog* GetGuildUnionCreateDlg() { return m_pGuildUnionCreateDlg; }
	void SetGuildUnionCreateDlg(CGuildUnionCreateDialog* dlg) { m_pGuildUnionCreateDlg = dlg;	}	
	
	CGuildDialog* GetGuildDlg() { return m_pGuildDlg; }
	void SetGuildDlg(CGuildDialog* dlg) { m_pGuildDlg = dlg;	}

	CGuildInviteDialog* GetGuildInviteDlg() { return m_pGuildInviteDlg; }
	void SetGuildInviteDlg(CGuildInviteDialog* dlg) { m_pGuildInviteDlg = dlg;	}
	
	CGuildMarkDialog* GetGuildMarkDlg() { return m_pGuildMarkDlg; }
	void SetGuildMarkDlg(CGuildMarkDialog* dlg) { m_pGuildMarkDlg = dlg;	}
	
	CGuildLevelUpDialog* GetGuildLevelUpDlg() { return m_pGuildLevelUpDlg; }
	void SetGuildLevelUpDlg(CGuildLevelUpDialog* dlg) { m_pGuildLevelUpDlg = dlg;	}

	CGuildNickNameDialog* GetGuildNickNameDlg() { return m_pGuildNickNameDlg; }
	void SetGuildNickNameDlg(CGuildNickNameDialog* dlg) { m_pGuildNickNameDlg = dlg;	}

	CGuildRankDialog* GetGuildRankDlg() { return m_pGuildRankDlg; }
	void SetGuildRankDlg(CGuildRankDialog* dlg) { m_pGuildRankDlg = dlg;	}
	
	CGuildWarehouseDialog* GetGuildWarehouseDlg() { return m_pGuildWarehouseDlg; }
	void SetGuildWarehouseDlg(CGuildWarehouseDialog* dlg) { m_pGuildWarehouseDlg = dlg;	}

	CMiniMapDlg* GetMiniMapDialog() { return m_pMiniMapDlg; 	}
	void SetMiniMapDialog(CMiniMapDlg* dlg) { m_pMiniMapDlg = dlg; 	}

	CBigMapDlg* GetBigMapDialog() { return m_pBigMapDlg; 	}
	void SetBigMapDialog(CBigMapDlg* dlg) { m_pBigMapDlg = dlg; 	}

	CWorldMapDlg* GetWorldMapDialog() { return m_pWorldMapDlg; }
	void SetWorldMapDlg(CWorldMapDlg* dlg ) { m_pWorldMapDlg = dlg; }

	CStorageDialog* GetStorageDialog() { return m_pStorageDialog;	}
	void SetStorageDialog(CStorageDialog* dlg) { m_pStorageDialog = dlg; 	}
	
	CFriendDialog* GetFriendDialog() { return m_pFriendDialog; }
	void SetFriendDialog(CFriendDialog* dlg) { m_pFriendDialog = dlg; }

	CNoteDialog* GetNoteDialog() { return m_pNoteDialog; }
	void SetNoteDialog(CNoteDialog* dlg) { m_pNoteDialog = dlg;	}
	
	CMiniNoteDialogWrite* GetMiniNoteDialogWrite() { return m_pMiniNoteDialogWrite;	}
	void SetMiniNoteDialogWrite(CMiniNoteDialogWrite* dlg) { m_pMiniNoteDialogWrite = dlg;	}

	CMiniNoteDialogRead* GetMiniNoteDialogRead() { return m_pMiniNoteDialogRead;	}
	void SetMiniNoteDialogRead(CMiniNoteDialogRead* dlg) { m_pMiniNoteDialogRead = dlg;	}
	
	CMiniFriendDialog * GetMiniFriendDialog() { return m_pMiniFriendDialog; 	}
	void SetMiniFriendDialog(CMiniFriendDialog* dlg) { m_pMiniFriendDialog = dlg;	}
	
	CReviveDialog * GetReviveDialog() { return m_pReviveDialog; 	}
	void SetReviveDialog(CReviveDialog* dlg) { m_pReviveDialog = dlg;	}

	CWantNpcDialog * GetWantNpcDialog() { return m_pWantNpcDialog; 	}
	void SetWantNpcDialog(CWantNpcDialog* dlg) { m_pWantNpcDialog = dlg; }
	
	CWantRegistDialog * GetWantRegistDialog() { return m_pWantRegistDialog; }
	void SetWantRegistDialog(CWantRegistDialog * dlg) { m_pWantRegistDialog = dlg;	}

	CQuestTotalDialog * GetQuestTotalDialog() { return m_pQuestTotalDlg;	}
	void SetQuestTotalDialog(CQuestTotalDialog * dlg) { m_pQuestTotalDlg = dlg;	}
	CQuestDialog* GetQuestDialog()			{ return m_pQuestDlg;	}
	void SetQuestDialog(CQuestDialog* dlg)	{ m_pQuestDlg = dlg;	}

	CQuestQuickViewDialog* GetQuestQuickViewDialog() { return m_pQuestQuickViewDlg ; }
	void SetQuestQuickViewDialog(CQuestQuickViewDialog* pDlg) { m_pQuestQuickViewDlg = pDlg ; }

	CGameNotifyDlg* GetGameNotifyDlg() { return m_pGameNotifyDlg ; }
	void SetGameNotifyDlg(CGameNotifyDlg* pDlg) { m_pGameNotifyDlg = pDlg ; }
	CItemShopDialog* GetItemShopDialog()			{	return m_pItemShopDialog;	}
	void SetItemShopDialog(CItemShopDialog* dlg)	{	m_pItemShopDialog = dlg; 	}
	// guildfieldwar
	CGFWarDeclareDlg* GetGFWarDeclareDlg()			{	return m_pGFWarDeclareDlg;	}
	void SetGFWarDeclareDlg(CGFWarDeclareDlg* dlg)	{	m_pGFWarDeclareDlg = dlg;	}
	CGFWarResultDlg* GetGFWarResultDlg()			{	return m_pGFWarResultDlg;	}
	void SetGFWarResultDlg(CGFWarResultDlg* dlg)	{	m_pGFWarResultDlg = dlg;	}
	CGFWarInfoDlg* GetGFWarInfoDlg()				{	return m_pGFWarInfoDlg;	}
	void SetGFWarInfoDlg(CGFWarInfoDlg* dlg)		{	m_pGFWarInfoDlg = dlg;	}
	CGuildWarInfoDlg* GetGuildWarInfoDlg()			{	return m_pGuildWarInfoDlg;	}
	void SetGuildWarInfoDlg(CGuildWarInfoDlg* dlg)	{	m_pGuildWarInfoDlg = dlg;	}
	// partywar
	CPartyWarDialog*	GetPartyWarDlg()				{	return m_pPartyWarDlg;	}
	void SetPartyWarDlg(CPartyWarDialog* dlg)			{	m_pPartyWarDlg = dlg;	}

	// 외치기
	CShoutDialog*	GetShoutDlg()						{	return m_pShoutDlg;		}
	void SetShoutDlg(CShoutDialog* dlg)					{	m_pShoutDlg = dlg;		}
	// 061204 LYW --- Delete Preexistence Chatting Dialog.
	/*
	CShoutchatDialog*	GetShoutchatDlg()				{	return m_pShoutchatDlg;		}
	void SetShoutchatDlg(CShoutchatDialog* dlg)			{	m_pShoutchatDlg = dlg;		}
	*/

	// 낚시
	CFishingDialog* GetFishingDlg()						{	return m_pFishingDlg;	}
	void SetFishingDlg(CFishingDialog* dlg)				{	m_pFishingDlg = dlg;	}

	CFishingGaugeDialog* GetFishingGaugeDlg()			{	return m_pFishingGaugeDlg;	}
	void SetFishingGaugeDlg(CFishingGaugeDialog* dlg)	{	m_pFishingGaugeDlg = dlg;	}

	CFishingPointDialog* GetFishingPointDlg()			{	return m_pFishingPointDlg;	}
	void SetFishingPointDlg(CFishingPointDialog* dlg)	{	m_pFishingPointDlg = dlg;	}
	//스크린샷
	CScreenShotDlg*	GetScreenShotDlg()				{	return m_pScreenShotDlg;	}
	void SetScreenShotDlg( CScreenShotDlg* dlg )	{	m_pScreenShotDlg = dlg;		}

	// 061127 LYW --- Add Return & Setting Function For New Chatting System.
	CChattingDlg* GetChattingDlg() { return m_pChattingDlg ; }
	void SetChattingDlg( CChattingDlg* dlg ) { m_pChattingDlg = dlg ; }

	// 061127 LYW --- Add Return & Setting Function For New Outside Chatting Dialog.
	COSChattingDlg* GetOSChattingDlg() { return m_pOSChattingDlg ; }
	void SetOSChattingDlg( COSChattingDlg* dlg ) { m_pOSChattingDlg = dlg ; }

	// 061219 LYW --- Add function to return and setting main system dialog.
	void SetMainSystemDlg( CMainSystemDlg* pDlg ) { m_pMainSystemDlg = pDlg ; }
	CMainSystemDlg* GetMainSystemDlg() { return m_pMainSystemDlg ; }
	//-----------

	cNpcImageDlg* GetNpcImageDlg() { return m_pNpcImageDlg; }
	void SetNpcImageDlg( cNpcImageDlg* dlg ) { m_pNpcImageDlg = dlg; }
	CGTStandingDialog*	GetGTStandingDlg()					{	return m_pGTStandingDlg;		}
	void SetGTStandingDlg(CGTStandingDialog* dlg)			{	m_pGTStandingDlg = dlg;			}
	CGTStandingDialog16*	GetGTStandingDlg16()			{	return m_pGTStandingDlg16;		}
	void SetGTStandingDlg16(CGTStandingDialog16* dlg)		{	m_pGTStandingDlg16 = dlg;		}
	CGTBattleListDialog*	GetGTBattleListDlg()			{	return m_pGTBattleListDlg;		}
	void SetGTBattleListDlg(CGTBattleListDialog* dlg)		{	m_pGTBattleListDlg = dlg;		}
	CGTScoreInfoDialog*	GetGTScoreInfoDlg()					{	return m_pGTScoreInfoDlg;		}
	void SetGTScoreInfoDlg(CGTScoreInfoDialog* dlg)			{	m_pGTScoreInfoDlg = dlg;		}
	// 06. 02. 강화 재료 안내 인터페이스 추가 - 이영준
	CReinforceGuideDialog* GetReinforceGuideDlg()	{ return m_pReinforceGuideDlg; }
	void SetReinforceGuideDlg(CReinforceGuideDialog* dlg) { m_pReinforceGuideDlg = dlg; }

	CKeySettingTipDlg* GetKeySettingTipDlg() { return m_pKeySettingTipDlg; }
	void SetKeySettingTipDlg(CKeySettingTipDlg* dlg) { m_pKeySettingTipDlg = dlg; }

	CGuildNoteDlg*	GetGuildNoteDlg() { return	m_pGuildNoteDlg; }
	void SetGuildNoteDlg(CGuildNoteDlg* dlg) { m_pGuildNoteDlg = dlg; }
	CUnionNoteDlg*	GetUnionNoteDlg() { return	m_pUnionNoteDlg; }
	void SetUnionNoteDlg(CUnionNoteDlg* dlg) { m_pUnionNoteDlg = dlg; }

	// 06. 03. 문파공지 - 이영준
	CGuildNoticeDlg* GetGuildNoticeDlg() { return m_pGuildNoticeDlg; }
	void SetGuildNoticeDlg(CGuildNoticeDlg *dlg) { m_pGuildNoticeDlg = dlg; }
	//SW060713 문하생
	CGuildJoinDialog* GetGuildJoinDialog() {	return m_pGuildInvitationKindSelectDlg;	}
	void SetGuildJoinDialog(CGuildJoinDialog* dlg)	{	m_pGuildInvitationKindSelectDlg = dlg;	}

	CDateMatchingDlg*	GetDateMatchingDlg(){ return m_pDateMatchingDlg;}					// 데이트 매칭 다이얼로그. 2007/03/26 - 이진영
	void	SetDateMatchingDlg( CDateMatchingDlg* pDlg){ m_pDateMatchingDlg = pDlg;}

	// 070605 LYW --- GameIn : Add function to setting and return identification dialog.
	void SetIdentificationDlg( CIdentification* pDlg )  { m_pIdentificationDlg = pDlg ; }
	CIdentification* GetIdentificationDlg()				{ return m_pIdentificationDlg ; }

	// 070605 LYW --- GameIn : Add function to setting and return favor icon dialog.
	void SetFavorIconDlg( CFavorIcon* pDlg )			{ m_pFavorIconDlg = pDlg ; }
	CFavorIcon* GetFavorIconDlg()						{ return m_pFavorIconDlg ; }

	// 070607 LYW --- GameIn : Add function to setting and return my info dialog.
	void SetMatchMyInfoDlg( CDateMatchingInfoDlg* pDlg ) { m_pMatchMyInfoDlg = pDlg ; }
	CDateMatchingInfoDlg* GetMatchMyInfoDlg()			 { return m_pMatchMyInfoDlg ; }

	// 070607 LYW --- GameIn : Add function to setting and return partner info dialog.
	void SetMatchPartnerInfoDlg( CDateMatchingInfoDlg* pDlg ) { m_pMatchPartnerInfoDlg = pDlg ; }
	CDateMatchingInfoDlg* GetMatchPartnerInfoDlg()			  { return m_pMatchPartnerInfoDlg ; }

	// 070607 LYW --- GameIn : Add function to setting and return record dialog.
	void SetMatchRecordDlg(CDateMatchingRecordDlg* pDlg )	{ m_pMatchRecordDlg = pDlg ; }
	CDateMatchingRecordDlg* GetMatchRecordDlg()				{ return m_pMatchRecordDlg ; }

	CFamilyCreateDialog* GetFamilyCreateDlg() { return m_pFamilyCreateDlg; }
	void SetFamilyCreateDlg(CFamilyCreateDialog* dlg) { m_pFamilyCreateDlg = dlg;	}
	CFamilyDialog* GetFamilyDlg() { return m_pFamilyDlg; }
	void SetFamilyDlg(CFamilyDialog* dlg) { m_pFamilyDlg = dlg;	}
	CFamilyMarkDialog* GetFamilyMarkDlg() { return m_pFamilyMarkDlg; }
	void SetFamilyMarkDlg(CFamilyMarkDialog* dlg) { m_pFamilyMarkDlg = dlg;	}
	CFamilyNickNameDialog* GetFamilyNickNameDlg() { return m_pFamilyNickNameDlg; }
	void SetFamilyNickNameDlg(CFamilyNickNameDialog* dlg) { m_pFamilyNickNameDlg = dlg;	}

	CPetInfoDialog*			GetPetInfoDlg()											{ return mPetInfoDialog; }
	void					SetPetInfoDlg( CPetInfoDialog* pDlg )					{ mPetInfoDialog = pDlg; }
	CPetStateDialog*		GetPetStateDlg()										{ return mPetStateDialog; }
	void					SetPetStateDlg( CPetStateDialog* pDlg )					{ mPetStateDialog = pDlg; }
	CPetWearedDialog*		GetPetWearedDlg()										{ return mPetWearedDialog; }
	void					SetPetWearedDlg( CPetWearedDialog* pDlg )				{ mPetWearedDialog = pDlg; }
	CPetInvenDialog*		GetPetInvenDlg()										{ return mPetInvenDialog; }
	void					SetPetInvenDlg( CPetInvenDialog* pDlg )					{ mPetInvenDialog = pDlg; }
	cDialog*				GetPetUIDlg()											{ return mPetUIDialog; }
	void					SetPetUIDlg( cDialog* pDlg )							{ mPetUIDialog = pDlg; }
	CPetResurrectionDialog*	GetPetResurrectionDlg()									{ return mPetResurrectionDialog; }
	void					SetPetResurrectionDlg( CPetResurrectionDialog* pDlg )	{ mPetResurrectionDialog = pDlg; }

	// 
	CGTResultDlg* GetGTResultDlg()						{	return m_pGTResultDlg;	}
	void SetGTResultDlg(CGTResultDlg* dlg)				{	m_pGTResultDlg = dlg;	}

	CGTEntryEditDlg* GetGTEntryEditDlg()				{	return m_pGTEntryEditDlg;}
	void SetGTEntryEditDlg(CGTEntryEditDlg* dlg)		{	m_pGTEntryEditDlg = dlg;}

	CCookDlg* GetCookDlg()								{	return m_pCookDlg;}
	void SetCookDlg(CCookDlg* dlg)						{	m_pCookDlg = dlg;}

	// 080509 KTH --
	CAnimalDialog*	GetAnimalDialog()						{	return m_pAnimalDialog;	}
	void			SetAnimalDialog(CAnimalDialog* dlg)		{	m_pAnimalDialog = dlg;	}

	cSkillTrainingDlg* GetSkillTrainingDlg() { return m_pSkillTrainingDlg; }
	void SetSkillTrainingDlg( cSkillTrainingDlg *dlg ) { m_pSkillTrainingDlg = dlg; }
	//캐릭터 로그인 시간
	SYSTEMTIME	GameinServerTime;			//서버에서 받아온 캐릭터 로그인 시간
	DWORD				m_Clientdate;		//클라이언트에서 받아온 캐릭터 로그인 날짜
	DWORD				m_Clienttime;		//클라이언트에서 받아온 캐릭터 로그인 시간

	void	SetLoginTime(SYSTEMTIME Time);
	SYSTEMTIME	GetLoginTime();

	void	GetClientLoginTime(DWORD& date, DWORD& time);
	void	SetClientLoginTime(DWORD date, DWORD time);

	// 070205 LYW --- Add functions to process network msg.
public :
	/// USER CONNECTION PART.
	void UserConn_NetworkMsgParse(BYTE Protocol,void* pMsg) ;
	void UserConn_Object_Remove(LPVOID);
	void UserConn_Monster_Die(LPVOID);
	void UserConn_Character_Die(LPVOID);
	void UserConn_Pet_Die(LPVOID);
	void UserConn_ApplyExp(LPVOID);
	void UserConn_DownExp(LPVOID);
	void UserConn_DownLevel(LPVOID);
	void UserConn_Ready_To_Revive();
	void UserConn_Character_Revive(LPVOID);
	void UserConn_Character_Revive_Nack(LPVOID);
	void UserConn_Npc_Add(LPVOID);
	void UserConn_GridInit();
	void UserConn_GameIn_Ack(LPVOID);
	void UserConn_Character_Add(LPVOID);
	void UserConn_Monster_Add(LPVOID);
	void UserConn_BossMonster_Add(LPVOID);
	void UserConn_ChangeMap_Ack(LPVOID);
	void UserConn_Pet_Add(LPVOID);
	void UserConn_HeroPet_Remove(LPVOID);
	void UserConn_Vehicle_Add(LPVOID);
	void UserConn_Vehicle_Remove(LPVOID);
	void UserConn_Revive_Flag(LPVOID);
	void UserConn_Npc_ChangeMap_Ack(LPVOID);
	void UserConn_Npc_ChangeMap_Nack();
	void UserConn_Cheat_ChangeMap_Ack(LPVOID);
	void UserConn_ChangeMap_Nack();
	void UserConn_Monster_Tame(DWORD ownerObjectIndex, DWORD tamedObjectIndex) const;
	// 090512 ONS 타종족의 신규종족 시작맵으로의 이동 제한
	void UserConn_DevilMarket_ChangeMap_Nack() ;
	void UserConn_CharacterList_Ack( void* pMsg ) ;
	void UserConn_BackToCharSel_Nack() ;
	void UserConn_MapDesc( void* pMsg ) ;
	void UserConn_SetVisible( void* pMsg ) ;
	void UserConn_Alert(int result, int remainedSecond);

	void NPC_NetworkMsgParse(BYTE Protocol,void* pMsg) ;
	void Npc_Speech_Ack( void* pMsg ) ;
	void Npc_Speech_Nack( void* pMsg ) ;
	void Npc_CloseBomul_Ack() ;
	void Npc_Die_Ack( void* pMsg ) ;
	void Npc_DoJob_Nack() ;

	/// SIGNAL PART.
	void SIGNAL_NetworkMsgParse(BYTE Protocol,void* pMsg) ;
	void Signal_CommonUser( void* pMsg ) ;
	// 080114 KTH -- 아이템 획득을 알림
	void Signal_UserAddItem( void* pMsg ) ;
	void Signal_ShowDown_Result( void* pMsg ) ;

	// 071017 LYW --- GameIn : Add function to setting and return tutorial dialog.
	void SetTutorialDlg(cTutorialDlg* pDlg) { m_TutorialDlg = pDlg ; }
	cTutorialDlg* GetTutorialDlg() { return m_TutorialDlg ; }

	// 071017 LYW --- GameIn : Add function to setting and return help dialog.
	void SetHelpDlg(cHelperDlg* pDlg) { m_HelpDlg = pDlg ; }
	cHelperDlg* GetHelpDlg() { return m_HelpDlg ; }

	// 071023 LYW --- GameIn : Add function to setting and return notice button dialog of tutorial.
	void SetTutorialBtnDlg(cTutorialBtnDlg* pDlg) { m_TutorialBtnDlg = pDlg ; }
	cTutorialBtnDlg* GetTutorialBtnDlg() { return m_TutorialBtnDlg ; }

	void SetItemPopupDlg(cItemPopupDlg* pDlg) { m_ItemPopupDlg = pDlg ; }
	cItemPopupDlg* GetItemPopupDlg() { return m_ItemPopupDlg ; }
	// 071130 LYW --- GameIn : Add function to setting and return map move dialog.
	void SetMapMoveDlg(cMapMoveDialog* pDlg) { m_MapMoveDlg = pDlg ; }
	cMapMoveDialog* GetMapMoveDlg() { return m_MapMoveDlg ; }

	// 071227 LYW --- GameIn : Add function to setting and return change name dialog.
	void SetChangeNameDlg( cChangeNameDialog* pDlg ) { m_pChangeNameDlg = pDlg ; }
	cChangeNameDialog* GetChangeNameDlg() { return m_pChangeNameDlg ; }

	// 080109 LYW --- cWindowManager : 숫자 패드 다이얼로그 추가.
	void SetNumberPadDlg( cNumberPadDialog* pDlg ) { m_pNumberPadDlg = pDlg ; }
	cNumberPadDialog* GetNumberPadDlg() { return m_pNumberPadDlg ; }

	//---KES AUTONOTE
	void SetAutoNoteDlg( CAutoNoteDlg* pDlg )		{ m_pAutoNoteDlg = pDlg; }
	CAutoNoteDlg* GetAutoNoteDlg()					{ return m_pAutoNoteDlg; }
	void SetAutoAnswerDlg( CAutoAnswerDlg* pDlg )	{ m_pAutoAnswerDlg = pDlg; }
	CAutoAnswerDlg* GetAutoAnswerDlg()				{ return m_pAutoAnswerDlg; }
	//--------------

	//---KES 상점검색
	void SetStoreSearchDlg( CStoreSearchDlg* pDlg )		{ m_pStoreSearchDlg = pDlg; }
	CStoreSearchDlg* GetStoreSearchDlg()				{ return m_pStoreSearchDlg; }
	//---------------

	// 080403 LYW --- GameIn : Add functions to setting and return dialogs for chatroom system.
	void SetChatRoomMainDlg( CChatRoomMainDlg* pDlg )			{ m_pChatRoomMainDlg = pDlg ; }
	CChatRoomMainDlg* GetChatRoomMainDlg()						{ return m_pChatRoomMainDlg ; }

	void SetChatRoomCreateDlg( CChatRoomCreateDlg* pDlg )		{ m_pChatRoomCreateDlg = pDlg ; }
	CChatRoomCreateDlg* GetChatRoomCreateDlg()					{ return m_pChatRoomCreateDlg ; }

	void SetChatRoomDlg( CChatRoomDlg* pDlg )					{ m_pChatRoomDlg = pDlg ; }
	CChatRoomDlg* GetChatRoomDlg()								{ return m_pChatRoomDlg ; }

	void SetChatRoomGuestListDlg( CChatRoomGuestListDlg* pDlg )	{ m_pChatRoomGuestListDlg = pDlg ; }
	CChatRoomGuestListDlg* GetChatRoomGuestListDlg()			{ return m_pChatRoomGuestListDlg ; }

	void SetChatRoomOptionDlg( CChatRoomOptionDlg* pDlg )		{ m_pChatRoomOptionDlg = pDlg ; }
	CChatRoomOptionDlg*	GetChatRoomOptionDlg()					{ return m_pChatRoomOptionDlg ; }

	void SetChatRoomJoinDlg( CChatRoomJoinDlg* pDlg )			{ m_pChatRoomJoinDlg = pDlg ; }
	CChatRoomJoinDlg*	GetChatRoomJoinDlg()					{ return m_pChatRoomJoinDlg ; }

	/*void SetChatRoomLobbyDlg( CChatRoomLobbyDlg* pDlg )			{ m_pChatRoomLobbyDlg = pDlg ; }
	CChatRoomLobbyDlg*	GetChatRoomLobbyDlg()					{ return m_pChatRoomLobbyDlg ; }*/

	// 080929 LYW --- GameIn : 공성 깃발 정보 다이얼로그 설정/반환 함수 추가.
	void SetSiegeWarFlagDlg(CSiegeWarFlagDlg* pDlg )			{ m_pSiegeFlagDlg = pDlg ; }
	CSiegeWarFlagDlg* GetSiegeWarFlagDlg()						{ return m_pSiegeFlagDlg ; }

	// 080930 LYW --- GameIn : 페이드 인/아웃 설정/반환 함수 추가.
	void SetFadeDlg( CFadeDlg* pDlg )							{ m_pFadeDlg = pDlg ; }
	CFadeDlg* GetFadeDlg()										{ return m_pFadeDlg ; }

	void SetPopupMenuDlg( CPopupMenuDlg* pDlg )					{ m_pPopupMenuDlg = pDlg ; }
	CPopupMenuDlg* GetPopupMenuDlg()							{ return m_pPopupMenuDlg ; }

	// 090422 ShinJS --- 탈것을 위한 팝업 다이얼로그 설정/반환 함수 추가
	void SetRidePopupMenuDlg( CRidePopupMenuDlg* pDlg )			{ m_pRidePopupMenuDlg = pDlg ; }
	CRidePopupMenuDlg* GetRidePopupMenuDlg()					{ return m_pRidePopupMenuDlg ; }
	// 090413 ONS 전직 가이드 개선
	void SetGradeClassDlg( CGradeClassDlg* pDlg )					{ m_pGradeClassDlg = pDlg ; }
	CGradeClassDlg* GetGradeClassDlg()							{ return m_pGradeClassDlg ; }

	//090323 pdy 하우징 설치 UI추가
	void SetHousingRevolDlg( CHousingRevolDlg* pDlg )				{ m_pHousingRevolDlg = pDlg ; }
	CHousingRevolDlg* GetHousingRevolDlg()							{ return m_pHousingRevolDlg ; }

	//090330 pdy 하우징 창고 UI추가
	void SetHousingWarehouseDlg( cHousingWarehouseDlg* pDlg )							{ m_pHousingWarehouseDlg = pDlg ; }
	cHousingWarehouseDlg* GetHousingWarehouseDlg()									{ return m_pHousingWarehouseDlg ; }

	//090409 pdy 하우징 하우스 검색 UI추가
	void SetHouseSearchDlg( cHouseSearchDlg* pDlg )							{ m_pHouseSearchDlg = pDlg ; }
	cHouseSearchDlg* GetHouseSearchDlg()									{ return m_pHouseSearchDlg ; }

	//090410 pdy 하우징 하우스 이름설정 UI추가 
	void SetHouseNameDlg( cHouseNameDlg* pDlg )					{ m_pHouseNameDlg = pDlg ; }
	cHouseNameDlg* GetHouseNameDlg()							{return m_pHouseNameDlg;}

	//090414 pdy 하우징 꾸미기포인트 보상 UI추가
	void SetHousingDecoPointDlg( cHousingDecoPointDlg* pDlg )					{ m_pHousingDecoPointDlg = pDlg ; }
	cHousingDecoPointDlg* GetHousingDecoPointDlg()							{return m_pHousingDecoPointDlg;}

	//090507 pdy 하우징 액션 팝업매뉴 UI추가
	void SetHousingActionPopupMenuDlg( cHousingActionPopupMenuDlg* pDlg )					{ m_pHousingActionPopupMenuDlg = pDlg ; }
	cHousingActionPopupMenuDlg* GetHousingActionPopupMenuDlg()							{return m_pHousingActionPopupMenuDlg;}

	//090525 pdy 하우징 매인포인트 UI추가
	void SetHousingMainPointDlg( cHousingMainPointDlg* pDlg )					{ m_pHousingMainPointDlg = pDlg ; }
	cHousingMainPointDlg* GetHousingMainPointDlg()								{return m_pHousingMainPointDlg;}

	//090525 pdy 하우징 내집창고 버튼 UI추가
	void SetHousingWarehouseButtonDlg( cHousingWarehouseButtonDlg* pDlg )					{ m_pHousingWarehouseButtonDlg = pDlg ; }
	cHousingWarehouseButtonDlg* GetHousingWarehouseButtonDlg()								{return m_pHousingWarehouseButtonDlg;}

	//090622 NYJ 하우징 웹브라우저
	void SetHousingWebDlg( cHousingWebDlg* pDlg )		{ m_pHousingWebDlg = pDlg; }
	cHousingWebDlg* GetHousingWebDlg()					{ return m_pHousingWebDlg; }

	// 090525 ShinJS --- Party 다른맵에 있는 유저에 대해 초대시 이름을 입력하는 Dialog 추가
	void SetInputNameDlg( CInputNameDlg* pDlg )					{ m_pInputNameDlg = pDlg ; }
	CInputNameDlg* GetInputNameDlg()							{ return m_pInputNameDlg ; }

	//090708 pdy 하우징 꾸미기모드 버튼 UI추가
	void SetHousingDecoModeBtDlg( cHousingDecoModeBtDlg* pDlg )					{ m_pHousingDecoModeBtDlg = pDlg ; }
	cHousingDecoModeBtDlg* GetHousingDecoModeBtDlg()							{return m_pHousingDecoModeBtDlg;}
	//091102 NYJ 트리거 타이머
	void SetTriggerTimerDlg( CTriggerTimerDlg* pDlg )	{ m_pTriggerTimerDlg = pDlg; }
	CTriggerTimerDlg* GetTriggerTimerDlg()				{return m_pTriggerTimerDlg;}

	void SetVideoCaptureDlg( CVideoCaptureDlg* pDlg )	{ m_pVideoCaptureDlg = pDlg; }
	CVideoCaptureDlg* GetVideoCaptureDlg()				{ return m_pVideoCaptureDlg; }

	// 100111 ONS 부가적인 버튼 다이얼로그 추가
	void SetAdditionalButtonDlg( cAdditionalButtonDlg* pDlg ) { m_pAdditionalButtonDlg = pDlg; }
	cAdditionalButtonDlg* GetAdditionalButtonDlg()			{ return m_pAdditionalButtonDlg; }

	// 100511 ONS 전직변경 다이얼로그 추가
	void SetChangeClassDlg( CChangeClassDlg* pDlg ) { m_pChangeClassDlg = pDlg; }
	CChangeClassDlg* GetChangeClassDlg() { return m_pChangeClassDlg; }

	void SetConsignmentDlg(CConsignmentDlg* pDlg)	{m_pConsignmentDlg = pDlg;}
	CConsignmentDlg* GetConsignmentDlg()		{return m_pConsignmentDlg;}

#ifdef _TW_LOCAL_
	void SetItemShopDlg( CItemShopDlg* pDlg )		{ m_pItemShopDlg = pDlg; }
	CItemShopDlg* GetItemShopDlg()				{ return m_pItemShopDlg; }
#endif


	std::queue<void*> m_pBackupMsgQueue;
	void AddBackupMsg(void* pMsg, DWORD dwMsgSize);
	void ProcessBackupMsg();
};


EXTERNGLOBALTON(CGameIn)
