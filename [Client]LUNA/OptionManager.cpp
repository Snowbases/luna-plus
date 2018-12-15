// OptionManager.cpp: implementation of the COptionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OptionManager.h"
#include "MacroManager.h"
#include "MouseCursor.h"
#include "MousePointer.h"
#include "./Input/UserInput.h"
#include "ChatManager.h"
#include ".\Effect\EffectManager.h"
#include "cChatTipManager.h"
#include "ObjectManager.h"
#include "MHAudioManager.h"
#include "MHCamera.h"
#include "GameIn.h"
#include "./interface/cWindowManager.h"
#include "OptionDialog.h"
#include "./interface/cGuageBar.h"
#include "WindowIDEnum.h"
#include "MHMap.h"
#include "ChattingDlg.h"
#include "TutorialManager.h"
#include "AppearanceManager.h"

GLOBALTON(COptionManager);

COptionManager::COptionManager()
{
	ZeroMemory(
		&m_GameOption,
		sizeof(m_GameOption));
	ZeroMemory(
		&m_SoundInfoBackup,
		sizeof(m_SoundInfoBackup));
	SetDefaultOption();
}

COptionManager::~COptionManager()
{
}

void COptionManager::Init()
{
	LoadGameOption();

	m_bBGMSoundChanged	= FALSE;																			// 배경음악이 바뀌었는지 여부를 FALSE로 세팅한다.
	
	AUDIOMGR->SetBGMSoundOn( m_GameOption.bSoundBGM );														// 옵션 값에 따라 배경 음악의 ON/OFF 를 설정한다.
	
	// 100112 ONS 사운드옵션정보를 백업한다. 사운드는 게이지바를 통해 직접적으로 값을 변경하여
	// 적용시키기 때문에 최소버튼의 처리에 대비하여 백업해둘 필요가 있다.
	SoundInfoBackup();

	m_wSendOption		= 0;																				// 멤버 옵션 플래그 값을 0으로 세팅한다.
	
	m_bShowMemberDamage = TRUE;																				// 파티원 데미지 보기를 TRUE로 세팅한다.

	// 071026 LYW --- OptionManager : Setting option of tutorial.
	if( m_GameOption.bShowTutorial )
	{
		TUTORIALMGR->Set_ActiveTutorial(TRUE) ;							// 튜토리얼을 비활성화 상태로 세팅한다.
	}
	else
	{
		TUTORIALMGR->Set_ActiveTutorial(FALSE) ;							// 튜토리얼을 비활성화 상태로 세팅한다.
	}

	// 080916 KTH -- OptionManager : Setting Option Of ShoutDialog
	if( CHATMGR->GetChattingDlg() )
	{
		CHATMGR->GetChattingDlg()->SetRaceActive(e_CHATTINGDLG_GENERAL_SHOUT, !m_GameOption.bShowGeneralShoutDlg);
	}
}

void COptionManager::SendOptionMsg()
{
	MSG_DWORD6 message;
	ZeroMemory(
		&message,
		sizeof(message));
	message.Category = MP_USERCONN;
	message.Protocol = MP_USERCONN_OPTION_SYN;
	message.dwObjectID = gHeroID;
	message.dwData1 = MAKELPARAM(
		m_GameOption.bNoDeal,
		m_GameOption.bNoParty);
	message.dwData2 = MAKELPARAM(
		m_GameOption.bNoShowdown,
		m_GameOption.mVehicle.mIsNeedAllow);
	message.dwData3 = m_GameOption.mVehicle.mChargedMoney;
	// 이 정보는 에이전트 서버에서 획득하여 사용하므로 변경할 때 주의하자.
	message.dwData4 = MAKELPARAM(
		m_GameOption.bNoFriend,
		m_GameOption.bNoWhisper);
	message.dwData5 = m_GameOption.bNoDateMatching;
	message.dwData6 = m_GameOption.dwHideParts;
	NETWORK->Send(
		&message,
		sizeof(message));
}

void COptionManager::SetGameOption( sGAMEOPTION* pOption, BOOL bSaveToFile )
{
	if( m_GameOption.bSoundBGM != pOption->bSoundBGM )														// 입력 받은 배경음악 값과, 멤버의 값이 같지 않으면,
	{
		m_bBGMSoundChanged = TRUE;																			// 배경음악이 바뀌었는지 여부를 TRUE로 세팅한다.
	}

	m_GameOption = *pOption;																				// 멤버의 옵션 구조체를 입력받은 구조체로 세팅한다.

	// 100112 ONS 사운드 게이지바 조절때마다 저장하면 비효율적이므로 변경.
	if( bSaveToFile )
	{
		SaveGameOption();																						// 옵션을 저장한다.
	}
}

BOOL COptionManager::LoadGameOption()
{
	HANDLE	hFile = CreateFile( "./system/GameOption.opt", GENERIC_READ, 0, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )																		// 핸들 받기에 실패하면,
		return FALSE;																						// 리턴 처리를 한다.

	DWORD dwRead = 0;
	DWORD dwVersion = 0;

	if( !ReadFile( hFile, &dwVersion, sizeof(DWORD), &dwRead, NULL ) )										// 받은 핸들을 참조해 데이터를 읽는다. 실패하면,
	{
		CloseHandle( hFile );																				// 받은 핸들을 닫는다.
		return FALSE;																						// 리턴 처리를 한다.
	}
	if( dwVersion != OPTIONVERSION )																		// 버전이 주어진 수와 같지 않으면,
	{
		CloseHandle( hFile );																				// 받은 핸들을 닫는다.
		return FALSE;																						// 리턴 처리를 한다.
	}

	ReadFile(
		hFile,
		&m_GameOption,
		sizeof(m_GameOption),
		&dwRead,
		0);
	CloseHandle(
		hFile);
	return TRUE;
}

void COptionManager::SaveGameOption()
{
	HANDLE	hFile;																							// 파일 핸들을 선언한다.

	hFile=CreateFile( "./system/GameOption.opt", GENERIC_WRITE, 0, NULL,									// 지정한 파일로 핸들을 받는다.(쓰기모드)
					  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )																		// 핸들 받기에 실패하면,
		return;																								// 리턴 처리를 한다.
	
	DWORD dwWritten;																						// 실제로 기록한 바이트 수를 리턴받기 위한 변수.
	DWORD dwVersion = OPTIONVERSION ;																		// 버전을 세팅한다.
	WriteFile( hFile, &dwVersion, sizeof(DWORD), &dwWritten, NULL );										// 버전을 기록한다.
	WriteFile( hFile, &m_GameOption, sizeof(m_GameOption), &dwWritten, NULL );								// 옵션구조체 만큼 기록한다.
	
	CloseHandle( hFile );																					// 핸들을 닫는다.
}

void COptionManager::ApplySettings()
{
	ApplySound() ;																							// 사운드 파트를 적용한다.
	// 100112 ONS 옵션적용시 사운드정보를 백업한다.
	SoundInfoBackup();
	
	ApplyGraphics() ;																						// 그래픽 파트를 적용한다.

	ApplyChatting() ;																						// 채팅 파트를 적용한다.

	ApplyEnvironmemt() ;																					// 환경 파트를 적용한다.
}

void COptionManager::ApplySound()
{
	if( m_bBGMSoundChanged )																				// 배경음악의 변경여부가 TRUE 이면,
	{
		AUDIOMGR->SetBGMSoundOn( m_GameOption.bSoundBGM );													// 옵션에 담긴 값으로 배경음악을 ON/OFF 세팅한다.

		if( m_GameOption.bSoundBGM )																		// 배경음악 옵션값이 ON이면, 
		{
			AUDIOMGR->PlayLastBGM();																		// 오디오 매니져의 마지막 배경음악을 플레이 시킨다.
		}
		else																								// 배경음악 옶션값이 OFF 이면, 
		{
			AUDIOMGR->StopBGM();																			// 오디오 매니져의 배경음악을 스톱 시킨다.
		}

		m_bBGMSoundChanged = FALSE;																			// 배경음악의 변경여부를 FALSE로 세팅한다.
	}

	if( m_GameOption.bSoundBGM )	//bgm이 play중이 아닐때 볼륨조절하면 뻑난다.							// 배경음악이 플레이 중이면,
	{
		AUDIOMGR->SetBGMVolume( (float)m_GameOption.nVolumnBGM / 100.0f );									// 옵션의 BGM Volume을 백분율로 해서 배경음악 볼륨을 세팅한다.
	}
	
	AUDIOMGR->SetEffectSoundOn( m_GameOption.bSoundEnvironment );											// 옵션의 사운드 값에 따라 이펙트 사운드를 ON/OFF 한다.

	if( m_GameOption.bSoundEnvironment )																	// 옵션의 사운드 값이 TRUE 이면,
	{
		AUDIOMGR->SetSoundVolume( (float)m_GameOption.nVolumnEnvironment / 100.0f ) ;						// 옵션의 볼륨값을 백분율로 해서 사운드 볼륨을 세팅한다.
	}
	else																									// 옵션의 사운드값이 FALSE 이면,
	{
		AUDIOMGR->StopAll();																				// 모두 스톱시킨다.
	}
}

void COptionManager::ApplyGraphics()
{
	CURSOR->SetActive( !m_GameOption.bGraphicCursor );														// 옵션의 커서 값에 따라 커서를 ON/OFF 한다.

	CAMERA->SetSightDistanceRate( (float)m_GameOption.nSightDistance );										// 시야 거리율을 세팅한다.

	DWORD dwShadowOption = eOO_SHADOW_NONE;																	// 그림자 옵션을 담을 변수를 선언 및 초기화 한다.

	if( m_GameOption.bShadowHero )		dwShadowOption |= eOO_SHADOW_ONLYHERO;								// 자신이 체크되어 있으면 추가한다.
	if( m_GameOption.bShadowMonster )	dwShadowOption |= eOO_SHADOW_MONSTER;								// 몬스터나 NPC가 체크되어 있으면 추가한다.
	if( m_GameOption.bShadowOthers )	dwShadowOption |= eOO_SHADOW_OTHERCHARACTER;						// 다른캐릭터가 체크되어 있으면 추가한다.

	OBJECTMGR->SetShadowOption( dwShadowOption );															// 오브젝트 매니져에 옵션을 적용한다.

	//SW 기획, 그래픽 전달은 0레벨이 최소 버텍스지만 여기서는 2 레벨이 최소이다.
	// @@@ Iros LOD Option 변경 처리
	OBJECTMGR->SetLodMode(m_GameOption.nLODMode);
	g_pExecutive->SetModelLODUsingMode( m_GameOption.nLODMode, 2 ) ;										// LOD 사용모드를 설정한다.
	//Effect
	OBJECTMGR->SetEffectOption( m_GameOption.nEffectMode ) ;												// 이펙트 모드를 설정한다.
}

void COptionManager::ApplyChatting()
{	
	sChatOption ChatOption;

	ChatOption.bOption[CTO_NOWHISPERMSG]	= m_GameOption.bNoWhisper ;										// 귓속말 거부를 세팅한다.
	ChatOption.bOption[CTO_NOCHATMSG]		= m_GameOption.bNoChatting ;									// 일반채팅 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOSHOUTMSG]		= m_GameOption.bNoShoutChat ;									// 외치기 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOPARTYMSG]		= m_GameOption.bNoPartyChat ;									// 파티 채팅 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOFAMILYMSG]		= m_GameOption.bNoFamilyChat ;									// 패밀리 채팅 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOGUILDMSG]		= m_GameOption.bNoGuildChat ;									// 길드 채팅 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOALLIANCEMSG]	= m_GameOption.bNoAllianceChat ;								// 동맹 채팅 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOSYSMSG]		= m_GameOption.bNoSystemMsg ;									// 시스템 메시지 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOEXPMSG]		= m_GameOption.bNoExpMsg ;										// 경험치 획득 메시지 끄기를 세팅한다.
	ChatOption.bOption[CTO_NOITEMMSG]		= m_GameOption.bNoItemMsg ;										// 아이템 획득 메시지 끄기를 세팅한다.

	// 080916 KTH -- OptionManager ShoutListDialog Setting
	if( CHATMGR->GetChattingDlg() )
	{
		CHATMGR->GetChattingDlg()->SetRaceActive(e_CHATTINGDLG_GENERAL_SHOUT, !m_GameOption.bShowGeneralShoutDlg);
	}

	CHATMGR->SetOption(
		ChatOption);
}

void COptionManager::ApplyEnvironmemt()
{
	DWORD dwOverInfoOption = eOO_NAME_CLICKEDONLY ;	//---default											// 오버 정보 옵션을 담을 변수 선언.

	if( m_GameOption.bNameOthers )	dwOverInfoOption |= eOO_NAME_WHOLE ;									// 다른 캐릭터 이름표시를 추가한다.
	if( m_GameOption.bNoMemberDamage )	dwOverInfoOption |= eOO_DAMAGE_MEMBERS ;							// 파티원 데미지 보기를 추가한다.
	if( m_GameOption.bNameParty )	dwOverInfoOption |= eOO_NAME_PARTY ;									// 파티원 이름 표시를 추가한다.
	if( m_GameOption.bNameMunpa )	dwOverInfoOption |= eOO_NAME_MUNPA ;									// 길드원 이름 표시를 추가한다.
	if( m_GameOption.bNoBalloon )	dwOverInfoOption |= eOO_CHAT_NOBALLOON ;								// 말풍선이 TRUE면 추가한다.

	// 090116 ShinJS --- 옵션 적용사항에 캐릭터/펫/노점의 보이기/안보이기 적용
	if( m_GameOption.bHideOtherPlayer )			dwOverInfoOption |= eOO_HIDE_PLAYER;
	if( m_GameOption.bHideOtherPet ) 			dwOverInfoOption |= eOO_HIDE_PET;
	if( m_GameOption.bHideStreetStallTitle )	dwOverInfoOption |= eOO_HIDE_STREETSTALLTITLE;

	OBJECTMGR->SetOverInfoOption( dwOverInfoOption ) ;														// 오브젝트 매니져에 오버 정보 옵션을 적용한다.

	CHATTIPMGR->SetActive( m_GameOption.bNoGameTip ) ;														// 게임 팁 보기를 설정한다.

	OPTIONMGR->SetMemberDemageActive( m_GameOption.bNoMemberDamage ) ;										// 파티원 데미지 보기를 설정한다.

	MACROMGR->SetMacroMode( m_GameOption.nMacroMode ) ;														// 채팅우선, 단축키 우선모드를 설정한다.
}

void COptionManager::CancelSettings()
{
	//직접 영향 주었던 것들 제대로 돌려놓기
	// 100112 ONS 옵션 적용 취소시에 사운드(효과음, 배경음악)정보를 되돌린다.
	m_GameOption.nVolumnBGM			=	m_SoundInfoBackup.m_nVolumnBgm;
	m_GameOption.nVolumnEnvironment	=	m_SoundInfoBackup.m_nVolumnEnvironment;
	ApplySound();
}

void COptionManager::SetDefaultOption()
{
	m_GameOption.bNoDeal 			= FALSE;																// 거래 거부.
	m_GameOption.bNoParty 			= FALSE;																// 파티 거부.
	m_GameOption.bNoFriend 			= FALSE;																// 친구등록 거부.
	m_GameOption.bNoShowdown 		= FALSE;																// 결투 거부.
	m_GameOption.bNoDateMatching	= FALSE;
	m_GameOption.bIsOnBattleGuage	= TRUE;																	// 전투게이지 출력여부	

	m_GameOption.bNameMunpa 		= TRUE;																	// 길드원 이름 표시.
	m_GameOption.bNameParty 		= TRUE;																	// 파티원 이름 표시.
	m_GameOption.bNameOthers 		= TRUE;																	// 다른 캐릭터 이름 표시.
	m_GameOption.bNoMemberDamage 	= TRUE;																	// 파티원 데미지 보기.
	m_GameOption.bNoGameTip 		= TRUE;																	// 게임 팁 보기.

	// 090116 ShinJS --- 캐릭터/펫 보이기 초기화, 노점 안보이기 초기화
	m_GameOption.bHideOtherPlayer 	= FALSE;																// 캐릭터 보이기
	m_GameOption.bHideOtherPet		= FALSE;																// 펫 보이기
	m_GameOption.bHideStreetStallTitle	= FALSE;															// 노점타이틀 안보이기
	// 100322 ONS 퀘스트알리미 자동등록 여부 초기화
	m_GameOption.bAutoQuestNotice		= TRUE;

	m_GameOption.nMacroMode 		= 1 ;																	// 채팅우선모드/단축키우선모드.

	m_GameOption.bNoWhisper			= FALSE;																// 귓속말 거부.
	m_GameOption.bNoChatting		= FALSE;																// 일반채팅 끄기.
	m_GameOption.bNoBalloon			= FALSE;																// 말풍선 끄기.
	m_GameOption.bNoPartyChat		= FALSE ;																// 파티 채팅 끄기.
	m_GameOption.bNoFamilyChat		= FALSE ;																// 패밀리 채팅 끄기.
	m_GameOption.bNoShoutChat		= FALSE;																// 외치기 끄기.
	m_GameOption.bNoGuildChat		= FALSE;																// 길드채팅 끄기.
	m_GameOption.bNoAllianceChat	= FALSE;																// 동맹채팅 끄기.

	m_GameOption.bNoSystemMsg		= FALSE;																// 시스템메시지 끄기.
	m_GameOption.bNoExpMsg			= FALSE;																// 경험치.수련치 획득메시지 끄기.
	m_GameOption.bNoItemMsg			= FALSE;																// 돈/아이템 획득메시지 끄기.

	m_GameOption.nSightDistance		= 155;//m_SigntGBMaxValue-50;													// 시야거리 조절값.
	m_GameOption.bGraphicCursor		= FALSE;																// 커서 온오프?
	m_GameOption.bShadowHero		= TRUE;																	// 그림자 표시 자신.
	m_GameOption.bShadowMonster		= TRUE;																	// 그림자 표시 몬스터/NPC.
	m_GameOption.bShadowOthers		= TRUE;																	// 그림자 표시 다른캐릭터.

	m_GameOption.bAutoCtrl			= FALSE;																// 게임 화면 자동 조절.
	m_GameOption.bAmbientMax		= TRUE;																	// 캐릭터 밝기?
	m_GameOption.nLODMode			= 0;																	// 그래픽 상태.
	m_GameOption.nEffectMode		= 0;																	// 이펙트 모드.
	m_GameOption.nEffectSnow        = 0;																	// 눈 이펙트 끄기.

	// 090429 ShinJS --- 배경음악 변경되는 경우
	if( !m_GameOption.bSoundBGM )
		m_bBGMSoundChanged = TRUE;

	m_GameOption.bSoundBGM = TRUE;
	m_GameOption.bSoundEnvironment = TRUE;
	m_GameOption.nVolumnBGM	= 100;
	m_GameOption.nVolumnEnvironment	= 100;
	m_GameOption.bShowTutorial = TRUE;
	m_GameOption.dwHideParts = 0;
	m_GameOption.bShowGeneralShoutDlg = FALSE ;
	m_GameOption.mVehicle.mIsNeedAllow = TRUE;
}

void COptionManager::ApplyGraphicSetting()
{
	CAMERA->SetSightDistanceRate( (float)m_GameOption.nSightDistance );										// 카메라의 시야거리를 세팅한다.
	g_pExecutive->SetModelLODUsingMode( m_GameOption.nLODMode, 2 );											// LOD사용모드를 설정한다.
	OBJECTMGR->SetShadowOption( m_dwShdowOption );															// 그림자 옵션을 적용한다.
	OBJECTMGR->SetEffectOption( m_GameOption.nEffectMode );													// 이펙트 옵션을 적용한다.
}

void COptionManager::InitForGameIn()
{
	m_pOptionDlg = (COptionDialog*)WINDOWMGR->GetWindowForID( OTI_TABDLG );									// 옵션 다이얼로그 정보를 받는다.
	cDialog* pDlg = (cDialog*)m_pOptionDlg->GetTabSheet(2);													// 세번째 시트를 받는다.
	m_pCammaGB = (cGuageBar*)(pDlg->GetWindowForID( OTI_GB_GAMMA ));										// 밝기 조절 게이지바 정보를 받는다.
	m_pSightGB = (cGuageBar*)(pDlg->GetWindowForID( OTI_GB_SIGHT ));										// 시야거리 조절 게이지바 정보를 받는다.
	m_SigntGBMinValue = m_pSightGB->GetMinValue();															// 시야거리 최소값을 받는다.
	m_SigntGBMaxValue = m_pSightGB->GetMaxValue();															// 시야거리 최대값을 받는다.
	m_dwShdowOption = eOO_SHADOW_NONE;																		// 그림자 옵션을 아무것도 없는것으로 세팅한다.
	m_dwLastCheckTime = m_dwCheck = m_dwFrameAverage = m_dwCountFrame = 0;									// 시간관련 변수들을 0으로 세팅한다.
	m_nCurOptionState = HIGH;																				// 현재 옵션 상태를 HIGH로 설정한다.
}

void COptionManager::SoundInfoBackup()
{
	m_SoundInfoBackup.m_nVolumnBgm			= m_GameOption.nVolumnBGM;
	m_SoundInfoBackup.m_nVolumnEnvironment	= m_GameOption.nVolumnEnvironment;
}