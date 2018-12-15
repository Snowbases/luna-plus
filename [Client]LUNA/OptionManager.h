#pragma once

#define OPTIONMGR USINGTON(COptionManager)
#define OPTIONVERSION	0x00070716

class COptionDialog;
class cGuageBar;

struct sGAMEOPTION
{
	BOOL bNoDeal;															// 거래 거부.
	BOOL bNoParty;															// 파티 거부.
	BOOL bNoFriend;															// 친구등록 거부.
	BOOL bNoShowdown;														// 결투 거부.

	BOOL bNameMunpa;														// 길드원 이름 표시.
	BOOL bNameParty;														// 파티원 이름 표시.
	BOOL bNameOthers;														// 다른 캐릭터 이름 표시.
	BOOL bNoMemberDamage;													// 파티원 데미지 보기.
	BOOL bNoGameTip;														// 게임 팁 보기.

	// 090116 ShinJS --- 캐릭터/펫 보이기 옵션 추가
	BOOL bHideOtherPlayer;													// 캐릭터 보이기
	BOOL bHideOtherPet;														// 펫 보이기
	BOOL bHideStreetStallTitle;												// 노점 타이틀 안보이기

	int nMacroMode;															// 채팅우선모드/단축키우선모드.

	BOOL bNoWhisper;														// 귓속말 거부.
	BOOL bNoChatting;														// 일반채팅 끄기.
	BOOL bNoBalloon;														// 말풍선 끄기.
	//BOOL bAutoHide;															// 메시지 표시창 자동 숨김.
	BOOL bNoPartyChat ;														// 파티 채팅 끄기.
	BOOL bNoFamilyChat ;													// 패밀리 채팅 끄기.
	BOOL bNoShoutChat;														// 외치기 끄기.
	BOOL bNoGuildChat;														// 길드 채팅 끄기.
	BOOL bNoAllianceChat;													// 동맹채팅 끄기.
	BOOL bNoSystemMsg;														// 시스템 메시지 끄기.
	BOOL bNoExpMsg;															// 경험치/수련치 획득메시지 끄기.
	BOOL bNoItemMsg;														// 돈/아이템 획득메시지 끄기.

	int nSightDistance;														// 시야 거리 값.

	BOOL bGraphicCursor;													// 그래픽 커서 여부?

	BOOL bShadowHero;														// 그림자 표시 - 자신.
	BOOL bShadowMonster;													// 그림자 표시 - 몬스터/NPC.
	BOOL bShadowOthers;														// 그림자 표시 - 다른캐릭터.

	BOOL bAutoCtrl;															// 게임 화면 자동조절.
	int nLODMode;															// 그래픽 상태 값.
	int nEffectMode;														// 이펙트 표시값.
	int nEffectSnow;														// 눈 이펙트 끄기.

	BOOL bSoundBGM;															// 배경음악 ON/OFF.
	BOOL bSoundEnvironment;													// 효과음 ON/OFF.

	int	nVolumnBGM;															// 배경음악 볼륨.
	int nVolumnEnvironment;													// 효과음 볼륨.

	BOOL bAmbientMax;														//SW060904 캐릭환경반사값 설정 옵션 추가

	// desc_hseos_옵션01
	// S 옵션 추가 added by hseos 2007.06.19
	int	nChatLineNum;
	int nCurChatMode;
	// E 옵션 추가 added by hseos 2007.06.19

	// 071025 LYW === OptionManager : Add tutorial option.
	BOOL bShowTutorial ;													// 튜토리얼 보이기 여부를 담는 변수.

	// 080916 KTH -- OptionManager : Add Shout Dialog Option.
	BOOL bShowGeneralShoutDlg;
	DWORD dwHideParts;
	struct Vehicle
	{
		BOOL mIsNeedAllow;
		MONEYTYPE mChargedMoney;
	}
	mVehicle;

	// 100322 ONS 퀘스트알리미 자동등록 여부
	BOOL bAutoQuestNotice;	

	// 100629 ONS 데이트매칭 거부기능 추가
	BOOL bNoDateMatching;

	// 100726 전투 게이지 (HERO / MONSTER ) 출력여부
	BOOL bIsOnBattleGuage;
};

// 100112 ONS 사운드 변경후, 취소버튼 눌렀을 경우 기존의 설정으로 되돌리기 위한 정보
struct sSoundInfo
{
	int		m_nVolumnBgm;
	int		m_nVolumnEnvironment;
};

class COptionManager  
{
protected:

	sGAMEOPTION		m_GameOption;											// 게임 옵션을 담고 있는 구조체.

	sSoundInfo		m_SoundInfoBackup;										// 100112 ONS 옵션 적용 취소시 사운드정보를 되돌리기는 변수.

	BOOL			m_bBGMSoundChanged;										// 배경 음악이 바뀌었는지 여부를 담는 변수.
	WORD			m_wSendOption;											// 옵션 상태를 보낼 때 사용하는 플래그 변수.

	BOOL			m_bShowMemberDamage ;

	COptionDialog*	m_pOptionDlg;											// 옵션 다이얼로그 포인터.

	cGuageBar*		m_pCammaGB;		//현재 적용은 안됨						// 감마 게이지바.
	cGuageBar*		m_pSightGB;												// 시야거리 게이지바.

	LONG			m_SigntGBMinValue;										// 시야거리 최소 값.
	LONG			m_SigntGBMaxValue;										// 시야거리 최대 값.

	DWORD			m_dwShdowOption;										// 그림자 옵션값을 담을 변수.

	enum {LOWEST=1,LOW,MID,HIGH,STANDARD_FRAME=15,NICE_FRAME=20};			// 옵션 상태 구분을 위한 이넘.

	DWORD			m_dwLastCheckTime;										// 마지막 체크 시간.
	DWORD			m_dwCheck;												// 체크 값.
	DWORD			m_dwFrameAverage;										// 평균 프레임 값.
	DWORD			m_dwCountFrame;											// 프레임 카운트.
	int				m_nCurOptionState;										// 현재 옵션 상태.
	
protected:
	BOOL LoadGameOption();

public:
	
	COptionManager();														// 생성자 함수.
	virtual ~COptionManager();												// 소멸자 함수.
	void Init();
	void SaveGameOption();
	void SetDefaultOption();
	void ApplySettings();
	void ApplySound();
	void ApplyGraphics();
	void ApplyChatting();
	void ApplyEnvironmemt();
	void CancelSettings();
	void SendOptionMsg();

	sGAMEOPTION* GetGameOption() { return &m_GameOption; }
	void SetGameOption( sGAMEOPTION* pOption, BOOL bSaveToFile = TRUE );
	void SetMemberDemageActive(BOOL bDo) { m_bShowMemberDamage = bDo; }
	BOOL IsShowMemberDamage() const { return m_bShowMemberDamage; }
	void ApplyGraphicSetting();
	void InitForGameIn();
	void Set_ShowTutorial(BOOL val) { m_GameOption.bShowTutorial = val; }
	BOOL Is_ShowTutorial() const { return m_GameOption.bShowTutorial; }
	void UpdateHideFlagFromDB(DWORD hideFlag) { m_GameOption.dwHideParts = hideFlag; }
	void SoundInfoBackup();
};

EXTERNGLOBALTON(COptionManager);