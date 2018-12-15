#include "stdafx.h"
#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

#include "WindowIdEnum.h"
#include "interface/cWindowManager.h"
#include "../GlobalEventFunc.h"
#include "MacroManager.h"
#include "GameIn.h"
#include "QuickManager.h"
#include "MoveManager.h"
#include "ObjectManager.h"
#include "MainGame.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "cChatTipManager.h"
#include "input/cIMEWnd.h"
#include "input/UserInput.h"
#include "ShowdownManager.h"
#include "StreetStallManager.h"
#include "ExchangeManager.h"
#include "MouseCursor.h"
#include "FriendManager.h"
#include "QuickDlg.h"
#include "QuickSlotDlg.h"
#include "CharacterDialog.h"
#include "InventoryExDialog.h"
#include "OptionDialog.h"
#include "ReviveDialog.h"
#include "BattleSystem_Client.h"
#include "PKManager.h"
#include "QuestTotaldialog.h"
#include "QuestManager.h"
#include "MiniMapDlg.h"
#include "BigmapDlg.h"
#include "cMsgBox.h"
#include "PartyIconManager.h"
#include "FreeImage/FreeImage.h"
#include "ItemShopDialog.h"
#include "StatusIconDlg.h"
#include "ItemManager.h"
#include "ObjectStateManager.h"
#include "DissolveDialog.h"
#include "Interface/cEditBox.h"
#include "AppearanceManager.h"
#include "ShoutDialog.h"
#include "GTResultDlg.h"
#include "GTScoreInfoDialog.h"
#include "KeySettingTipDlg.h"
#include "cSkillTreeDlg.h"
#include "ExitDialog.h"
#include "cResourceManager.h"
#include "QuestDialog.h"
#include "../FamilyDialog.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "../[cc]skill/client/info/activeskillinfo.h"
#include "../DateMatchingDlg.h"
#include "Party.h"
#include "PartyManager.h"
#include "../mhMap.h"
#include "../hseos/Date/SHDateManager.h"
#include "./ChatRoomMgr.h"
#include "FishingManager.h"
#include "FishingDialog.h"
#include "PetInfoDialog.h"
#include "../hseos/Date/SHChallengeZoneClearNo1Dlg.h"			// 090317 ShinJS --- 챌린지 맵의 스코어창을 닫기 위해 호출
#include "cHousingMgr.h"
#include "ConductManager.h"										// 100105 ONS 앉기/일어서기를 토글한다.
#include "PCRoomManager.h"
#include "ChattingDlg.h"
#include "WorldMapDlg.h"
#include "..\hseos\Family\SHFamily.h"

#ifdef _TESTCLIENT_																	// 테스트 클라이언트라면,

#include "TSToolManager.h"
#include "MHMap.h"																	// 맵 헤더를 불러온다.
#include "ObjectManager.h"															// 오브젝트 매니져 헤더를 불러온다.
#include "EFFECT/EffectManager.h"													// 이펙트 매니져 해더를 불러온다.
#include "GameResourceManager.h"													// 게임 리소스 매니져 해더를 불러온다.
#include "interface/cFont.h"														// 폰트 해더를 불러온다.

void InitTestHero();																// 테스트용 히어로를 초기화 한다.
void InitTestMonster();																// 테스트용 몬스터를 초기화 한다.
void InitTestCharacters(void);														// 테스트용 캐릭터 파트를 초기화 한다.

#endif																				// 테스트 클라이언트 처리 종료.


extern BOOL g_bActiveApp;
extern BOOL g_bDisplayFPS;															// 프레임을 출력할지 여부를 담은 변수.
extern int g_nServerSetNum;															// 서버셋 번호를 담은 변수.

GLOBALTON(CMacroManager)

CMacroManager::CMacroManager()														// 매크로 매니져 생성자 함수.
{
	ZeroMemory( m_MacroKey, sizeof( m_MacroKey ) );									// 매크로키 구조체를 제로 메모리 한다.
	ZeroMemory( m_DefaultKey, sizeof( m_DefaultKey ) );								// 디폴트키 구조체를 제로 메모리 한다.

	ZeroMemory( m_MacroPressKey, sizeof( m_MacroPressKey ) );						// 매크로 프레스키를 제로 메모리 한다.
	ZeroMemory( m_DefaultPressKey, sizeof( m_DefaultPressKey ) );					// 디폴트 프레스키를 제로 메모리 한다.

	m_nMacroMode	= MM_CHAT;														// 매크로 모드를 채팅으로 한다.
	m_bChatMode		= TRUE;															// 채팅모드를 TRUE로 세팅한다.
	m_bPause		= FALSE;														// 포즈를 FALSE로 세팅한다.
	m_bInVisibleCursor = FALSE;														// 커서의 출력여부를 FALSE로 세팅한다.

	m_nCurTargetIdx = 0 ;															// 자동 타겟의 인덱스를 0으로 세팅한다.
	m_dwTargetID = 0 ;																// 자동 타겟의 아이디를 0으로 세팅한다.
}

CMacroManager::~CMacroManager()														// 매크로 매니져 소멸자 함수.
{
}


void CMacroManager::KeyboardInput_Normal( BOOL bDock, CKeyboard* keyInfo )
{
	extern HWND _g_hWnd;
	if(_g_hWnd != GetFocus())
	{
		return;
	}

	if( !g_bActiveApp || m_bPause )													// 프로그램이 활성화 되지 않았거나, 포즈상태면,
	{
		return;																		// 리턴한다.							
	}
	
	if(HEROID == 0)																	// 히어로 아이디가 0과 같으면,
	{
		return;																		// 리턴한다.
	}

	if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN )							// 메인 게임의 상태가 게임 인 상태가 아니면,
	{
		return;																		// 리턴한다.
	}

	//if (keyInfo->GetKeyPressed(88) == 1)											//f12를 누를경우 시계 다이얼로그를 보인다.
	//{
	//	PlayMacro(ME_SCREENCAPTURE) ;												//키클릭후 랜더링이 작동하므로 키다운시 렌더링하고 키업시 스크린샷 저장.
	//}

	bDock = FALSE;																	// 도킹여부를 담을 변수를 FALSE로 세팅한다.

	if( CIMEWND->IsDocking() )														// IMEWND가 도킹중이라면,
	{
		if( m_nMacroMode == MM_MACRO )												// 매크로 모드가 단축키 우선모드라면,
		{
			bDock = TRUE;															// 도킹 변수를 TRUE로 세팅한다.
		}
	}
	
	int nSysKey = MSK_NONE;															// 시스템키를 MSK_NONE으로 세팅한다.

	if( keyInfo->GetKeyPressed( KEY_MENU ) )										// 메뉴키가 눌렸다면,
	{
		nSysKey = 0;																// 시스템키를 0으로 세팅하고,
		nSysKey |= MSK_ALT;															// ALT키를 추가한다.
	}
	if( keyInfo->GetKeyPressed( KEY_CONTROL ) )										// 컨트롤 키가 눌렸다면,
	{
		nSysKey &= ~MSK_NONE;														// 시스템 키를 MSK_NONE의 역으로 세팅한다.
		nSysKey |= MSK_CTRL;														// 시스템 키에 CTRL키를 추가한다.
	}
	if( keyInfo->GetKeyPressed( KEY_SHIFT ) )										// 시프트키가 눌렸다면,
	{
		nSysKey &= ~MSK_NONE;														// 시스템 키를 MSK_NONE의 역으로 세팅한다.
		nSysKey |= MSK_SHIFT;														// 시스템 키에 SHIFT를 추가한다.
	}
	
	for( int i = 0 ; i < ME_COUNT ; ++i )
	{
		const sMACRO& macro = m_MacroKey[m_nMacroMode][i];

		if( bDock && !macro.bAllMode )
		{
			continue;
		}
		else if(FALSE == (macro.nSysKey & nSysKey))
		{
			continue;
		}
		else if( macro.bUp )
		{
			if( keyInfo->GetKeyUp( macro.wKey ) )
			{
				PlayMacro( i );
			}
		}
		else if( keyInfo->GetKeyDown( macro.wKey ) )
		{
 			PlayMacro( i );
		}
	}
}

void CMacroManager::KeyboardInput_GMTool( BOOL bDock, CKeyboard* keyInfo )
{
#if defined(_GMTOOL_)
	if( (keyInfo->GetKeyDown(KEY_PADENTER) || (keyInfo->GetKeyDown(KEY_RETURN)))  // 엔터키와 ALT키가 눌렸거나,
		&& keyInfo->GetKeyPressed(KEY_MENU) )										// 숫자패드의 엔터키와 ALT키가 눌렸으면,
	{
		if( GMTOOLMGR->IsShowing() )												// GM툴이 보이고 있는상태라면,
		{
			GMTOOLMGR->ShowGMDialog( FALSE, TRUE );									// GM툴을 숨긴다.
		}
		else																		// GM툴이 숨겨진 상태라면,
		{
			GMTOOLMGR->ShowGMDialog( TRUE, TRUE );									// GM툴을 꺼낸다.
		}
	}
	else if( keyInfo->GetKeyDown(KEY_SUBTRACT) && keyInfo->GetKeyPressed(KEY_MENU) )// ALT키와 숫자패드의 -키가 눌렸다면,
	{
		MSGBASE msg;																// 기본 메시지 구조체를 선언한다.

		msg.Category	= MP_CHEAT;													// 카테고리를 치트로 세팅한다.
		msg.Protocol	= MP_CHEAT_AGENTCHECK_SYN;									// 프로토콜을 에이전트 체크 싱크로  세팅한다.
		msg.dwObjectID	= HEROID;													// 오브젝트 아이디에 히어로 아이디를 세팅한다.

		NETWORK->Send( &msg, sizeof(msg) );											// 메시지를 전송한다.
	}
#endif
}

void CMacroManager::KeyboardInput_CheatEnable( BOOL bDock, CKeyboard* keyInfo )
{	
#ifdef _TESTCLIENT_	
	KeyboardInput_Cheat_TestClient( bDock, keyInfo ) ;								// 테스트 클라이언트 일 때 처리를 한다.
#else
	KeyboardInput_Cheat_NotTestClient( bDock, keyInfo ) ;							// 테스트 클라이언트가 아닐 때 처리를 한다.
#endif // _TESTCLIENT_
}

void CMacroManager::KeyboardInput_Cheat_TestClient( BOOL bDock, CKeyboard* keyInfo )
{
#ifdef _TESTCLIENT_	

	if( (keyInfo->GetKeyDown(KEY_PADENTER) || (keyInfo->GetKeyDown(KEY_RETURN)))  // 엔터키와 ALT키가 눌렸거나,
		&& keyInfo->GetKeyPressed(KEY_MENU) )										// 숫자패드의 엔터키와 ALT키가 눌렸으면,
	{
		if( TSTOOLMGR->IsShowing() )												// GM툴이 보이고 있는상태라면,
		{
			TSTOOLMGR->ShowTSDialog( FALSE, TRUE );									// GM툴을 숨긴다.
		}
		else																		// GM툴이 숨겨진 상태라면,
		{
			TSTOOLMGR->ShowTSDialog( TRUE, TRUE );									// GM툴을 꺼낸다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_Z))
		HERO->GetEngineObject()->ChangeMotion(eMotion_Society_Happy, FALSE);

	if(keyInfo->GetKeyDown(KEY_U) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 U키가 눌렸다면,
	{
		if( GAMEIN->GetChattingDlg()->IsActive() )
		{
			GAMEIN->GetChattingDlg()->SetActive(FALSE) ;
			//GAMEIN->GetOSChattingDlg()->SetActive( FALSE ) ;
		}
		else
		{
			GAMEIN->GetChattingDlg()->SetActive(TRUE) ;
			//GAMEIN->GetOSChattingDlg()->SetActive( TRUE ) ;
		}
	}

	if(keyInfo->GetKeyDown(KEY_1))
	{
		if (keyInfo->GetKeyPressed(KEY_CONTROL)) // CTRL키와 1번키가 눌렸다면,
		{
			SKILLMGR->Release();													// 스킬매니져를 해제한다.
			SKILLMGR->Init();														// 스킬 매니져를 초기화 한다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_2) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 2번키가 눌렸다면,
	{
		EFFECTMGR->RemoveAllEffect();												// 모든 이펙트를 해제 한다.
		EFFECTMGR->Release();														// 이펙트 매니져를 해제한다.
		EFFECTMGR->Init();															// 이펙트 매니져를 초기화 한다.
	}

	if(keyInfo->GetKeyDown(KEY_3) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 3번키가 눌렸다면,
	{
		EFFECTMGR->RemoveAllEffect();												// 모든 이펙트를 해제 한다.

		GAMERESRCMNGR->LoadTestClientInfo();										// 테스트 클라이언트 정보를 로딩한다.

		OBJECTMGR->RemoveAllObject();												// 모든 오브젝트를 해제한다.

		GAMERESRCMNGR->ResetMonsterList();											// 몬스터 리스트를 다시 세팅한다.
		GAMERESRCMNGR->LoadMonsterList();											// 몬스터 리스트를 로딩한다.

		InitTestHero();																// 테스트 히어로를 초기화 한다.
		InitTestMonster();															// 테스트 몬스터를 초기화 한다.
		InitTestCharacters();														// 테스트 캐릭터를 초기화 한다.
	}

	if(keyInfo->GetKeyDown(KEY_4) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 4번키가 눌렸다면,
	{
		MAP->LoadMapDesc(MAP->GetMapNum());											// 맵 정보를 로딩한다.
		MAP->ApplyMapDesc();														// 맵 정보를 적용한다.
	}
	if(keyInfo->GetKeyDown(KEY_ESCAPE))												// ESC키가 눌렸다면,
	{
		EFFECTMGR->SetEndFlagAllEffect();											// 모든 이펙트의 종료 플레그를 세팅한다.
		OBJECTMGR->SetSelectedObject(NULL);											// 선택된 오브젝트를 해제 한다.
	}

	static BOOL bWireFrame = FALSE;													// 와이어 프레임 여부를 FALSE로 세팅한다.

	if(keyInfo->GetKeyDown(KEY_F12) && keyInfo->GetKeyPressed(KEY_MENU))			// ALT키와 F12가 눌렸다면,
	{
		if( !bWireFrame )															// 와이어 프레임 여부가 FALSE라면,
		{			
			g_pExecutive->GetRenderer()->SetRenderMode(2);							// 렌더모드를 와이어 프레임으로 한다.

			bWireFrame = TRUE;														// 와이어 프레임 여부를 TRUE로 세팅한다.
		}
		else																		// 와이어 프레임 여부가 TRUE라면,
		{
			g_pExecutive->GetRenderer()->SetRenderMode(0);							// 렌더모드를 솔리드로 한다.

			bWireFrame = FALSE;														// 와이어 프레임 여부를 FALSE로 세팅한다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_R) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT와 R키가 눌렸다면,
	{
		EFFECTMGR->StartEffectProcess(1034,HERO,NULL,0,0);							// 1034 이펙트를 시작한다.
	}

	if(keyInfo->GetKeyDown(KEY_4) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT키와 4번 키가 눌렸다면,
	{
		EFFECTMGR->StartHeroEffectProcess(FindEffectNum("Mall_eff_heal.beff"));		// 힐 이펙트를 시작한다.
	}

	if(keyInfo->GetKeyDown(KEY_5) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT키와 5번 키가 눌렸다면,
	{
		EFFECTMGR->StartHeroEffectProcess(FindEffectNum("Enchant_daggers01_A1.beff"));			// 이펙트를 시작한다.
	}

	if(keyInfo->GetKeyDown(KEY_7) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT키와 7번 키가 눌렸다면,
	{
		EFFECTMGR->StartHeroEffectProcess(eEffect_ShopItem_Revive);					// 샵아이템 부활 이펙트를 시작한다.
	}

	if(keyInfo->GetKeyDown(KEY_W))													// W키가 눌렸다면,
	{
		CActionTarget Target;														// 액션 타겟을 선언한다.

		cSkillInfo* pInfo = NULL ;													// 스킬 인포를 받을 포인터를 선언한다.
		
		pInfo = SKILLMGR->GetSkillInfo(GAMERESRCMNGR->m_TestClientInfo.SkillIdx);	// 테스트 클라이언트 스킬인덱스로 스킬 정보를 받는다.

		if( !pInfo )																// 스킬 정보가 유효하지 않다면,
		{
			return;																	// 리턴한다.
		}

		cActiveSkillInfo* pSkillInfo = ( cActiveSkillInfo* )pInfo;					// 액티브 스킬 정보를 받는다.

		if(pSkillInfo->GetSkillInfo()->Target == 1)									// 스킬의 타겟이 1과 같으면,
		{
			CObject * targetObj = OBJECTMGR->GetSelectedObject();					// 선택한 타겟의 오브젝트 정보를 받는다.

			if( targetObj )															// 오브젝트 정보가 유효한지 체크한다.
			{
				Target.InitActionTarget(targetObj,NULL);							// 타겟을 대상으로 액션을 초기화 한다.
			}
		}
		else if(pSkillInfo->GetSkillInfo()->Target == 0)							// 스킬의 타겟이 0과 같으면,
		{
			Target.InitActionTarget(HERO,NULL);										// 히어로를 대상으로 액션을 초기화 한다.
		}
		
		SKILLMGR->OnSkillCommand( HERO, &Target, TRUE );							// 스킬 매니져 명령을 실행한다.
	}

	if(keyInfo->GetKeyDown(KEY_E))													// E키가 눌렸다면,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// 선택한 타겟의 오브젝트 정보를 받는다.

		if( targetObj )																// 오브젝터 정보가 유효한지 체크한다.
		{
			TARGETSET Target;														// 타겟 셋 구조체를 선언한다.

			Target.pTarget = targetObj;												// 타겟을 설정한다.
			memset(&Target.ResultInfo,0,sizeof(RESULTINFO));						// 결과 정보를 메모리 셋한다.
			Target.ResultInfo.bCritical = rand()%2 ? TRUE : FALSE;					// 크리티컬 여부를 설정한다.
			Target.ResultInfo.RealDamage = rand() % 50;								// 리얼 데미지를 설정한다.
			Target.ResultInfo.CounterDamage = 0;									// 카운터 데미지를 설정한다.

			// 이펙트를 시작한다.
			EFFECTMGR->StartEffectProcess(GAMERESRCMNGR->m_TestClientInfo.Effect,HERO,&Target,1,targetObj->GetID(),EFFECT_FLAG_HEROATTACK);
		}
	}

	if(keyInfo->GetKeyDown(KEY_4))													// 4번키가 눌렸다면,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// 타겟 오브젝트 정보를 받아온다.

		if( targetObj )																// 오브젝트 정보가 유효한지 체크한다.
		{
			VECTOR3 pos;															// 벡터를 선언한다.

			targetObj->GetPosition(&pos);											// 타겟의 위치를 받아온다.
			pos.z -= 3000;															// Z값을 설정한다.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_WALKAROUND);					// 몬스터의 이동 타입을 eMA_WALKAROUND로 설정한다.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// 이동을 시작한다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_5))													// 5번키가 눌련다면,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// 타겟 오브젝트 정보를 받아온다.

		if( targetObj )																// 오브젝트 정보가 유효한지 체크한다.
		{
			VECTOR3 pos;															// 벡터를 선언한다.

			targetObj->GetPosition(&pos);											// 타겟의 위치를 받아온다.
			pos.z -= 3000;															// Z값을 설정한다.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_PERSUIT);						// 몬스터의 이동 타입을 eMA_PERSUIT로 설정한다.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// 이동을 시작한다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_6))													// 6번키가 눌렸다면,
	{
		CObject * targetObj = OBJECTMGR->GetSelectedObject();						// 선택한 타겟 오브젝트 정보를 받아온다.

		if( targetObj )																// 오브젝트 정보가 유효한지 체크한다.
		{
			VECTOR3 pos;															// 벡터를 선언한다.

			targetObj->GetPosition(&pos);											// 타겟의 위치를 받아온다.
			pos.z -= 3000;															// Z값을 설정한다.

			MOVEMGR->SetMonsterMoveType(targetObj,eMA_RUNAWAY);						// 몬스터의 이동 타입을 eMA_RUNAWAY로 설정한다.
			MOVEMGR->StartMoveEx(targetObj,gCurTime,&pos);							// 이동을 시작한다.
		}
	}

	if(keyInfo->GetKeyDown(KEY_F1) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 F1키가 눌렸다면,
	{
		g_bDisplayFPS = !g_bDisplayFPS;												// 프레임을 출력한다.
	}

	if( keyInfo->GetKeyDown(KEY_C) && keyInfo->GetKeyPressed(KEY_CONTROL))	// ALT키와 C키가 눌리면,
	{
		if( CAMERA->GetCameraMode() == eCM_EyeView )								// 카메라 모드가 EyeView와 같으면,
		{
			CAMERA->SetCameraMode( eCM_Free );										// 카메라 모드를 Free모드로 세팅한다.

			if( !WINDOWMGR->IsOpendAllWindows() )									// 모든 창들이 열려 있지 않으면,
			{
				WINDOWMGR->ShowAllActivedWindow() ;									// 활성화 된 모든 창을 열어준다.

				WINDOWMGR->SetOpendAllWindows( TRUE ) ;								// 모든 윈도우가 열린 것으로 세팅한다.

				WINDOWMGR->ShowBaseWindow() ;										// 기본 윈도우들을 보여준다.
			}

		}
		else																		// 카메라 모드가 EyeView와 같지 않으면,
		{
			CAMERA->SetCameraMode( eCM_EyeView );									// 카메라 모드를 EyeView로 세팅한다.

			if( WINDOWMGR->IsOpendAllWindows() )									// 모든 윈도우가 열려 있으면,
			{
				//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;						// 활성화 된 창들을 담고 있는 리스트를 모두 비운다.

				WINDOWMGR->CloseAllWindow() ;										// 모든 윈도우를 닫는다.

				WINDOWMGR->SetOpendAllWindows( FALSE ) ;							// 모든 윈도우가 닫힌 것으로 세팅한다.
			}
		}
	}
#endif // _TESTCLIENT_
}

void CMacroManager::KeyboardInput_Cheat_NotTestClient( BOOL bDock, CKeyboard* keyInfo )
{
#if defined(_GMTOOL_)
	if(keyInfo->GetKeyDown(KEY_F1) && keyInfo->GetKeyPressed(KEY_CONTROL))			// CTRL키와 F1키가 눌렸다면,
	{
		g_bDisplayFPS = !g_bDisplayFPS;												// 프레임을 출력한다.
	}
	
	if(keyInfo->GetKeyDown(KEY_BACK) && keyInfo->GetKeyPressed(KEY_MENU))			// ALT키와 BACK 키가 눌리면,
	{
		WINDOWMGR->ToggleShowInterface();											// 인터페이스를 토글 시킨다.
	}

	if(keyInfo->GetKeyDown(KEY_P) && keyInfo->GetKeyPressed(KEY_MENU))				// ALT키와 P키가 눌리면,
	{
		m_bInVisibleCursor ^= TRUE;													// 커서 출력여부를 세팅한다.
		CURSOR->SetCheatMode( m_bInVisibleCursor );									// 커서를 치트모드로 세팅한다.
	}

	if( keyInfo->GetKeyDown(KEY_C) && keyInfo->GetKeyPressed(KEY_CONTROL))	// ALT키와 C키가 눌리면,
	{
		if( CAMERA->GetCameraMode() == eCM_EyeView )								// 카메라 모드가 EyeView와 같으면,
		{
			CAMERA->SetCameraMode( eCM_Free );										// 카메라 모드를 Free모드로 세팅한다.

			if( !WINDOWMGR->IsOpendAllWindows() )									// 모든 창들이 열려 있지 않으면,
			{
				WINDOWMGR->ShowAllActivedWindow() ;									// 활성화 된 모든 창을 열어준다.

				WINDOWMGR->SetOpendAllWindows( TRUE ) ;								// 모든 윈도우가 열린 것으로 세팅한다.

				WINDOWMGR->ShowBaseWindow() ;										// 기본 윈도우들을 보여준다.
			}

		}
		else																		// 카메라 모드가 EyeView와 같지 않으면,
		{
			CAMERA->SetCameraMode( eCM_EyeView );									// 카메라 모드를 EyeView로 세팅한다.

			if( WINDOWMGR->IsOpendAllWindows() )									// 모든 윈도우가 열려 있으면,
			{
				//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;						// 활성화 된 창들을 담고 있는 리스트를 모두 비운다.

				WINDOWMGR->CloseAllWindow() ;										// 모든 윈도우를 닫는다.

				WINDOWMGR->SetOpendAllWindows( FALSE ) ;							// 모든 윈도우가 닫힌 것으로 세팅한다.
			}
		}
	}
#endif
}

void CMacroManager::KeyboardInput( CKeyboard* keyInfo )
{
	BOOL bDock = FALSE;																// 도킹여부를 담을 변수를 FALSE로 세팅한다.

	KeyboardInput_Normal( bDock, keyInfo ) ;										// 일반적인 키보드 입력 처리를 한다.

#ifdef _GMTOOL_																		// GM툴 처리를 시작한다.

	KeyboardInput_GMTool( bDock, keyInfo ) ;										// GM툴일 때 키보드 입력 처리를 한다.

#endif																				// GM툴 처리 종료.


#ifdef _CHEATENABLE_																// 치트모드일 때 처리를 시작한다.

	KeyboardInput_CheatEnable( bDock, keyInfo ) ;									// 치트모드일 때 키보드 입력 처리를 한다.

#endif	//_CHEATENABLE_																// 치트모드일 때 처리를 종료 한다.

}

void CMacroManager::KeyboardPressInput( CKeyboard* keyInfo )
{
	if( !g_bActiveApp || m_bPause )	return;											// 프로그램이 활성화 중이지 않거나, 포즈상태이면 리턴한다.

	if( MAINGAME->GetCurStateNum() != eGAMESTATE_GAMEIN ) return;					// 게임인 상태가 아이면 리턴한다.

	if( CIMEWND->IsDocking() )														// 도킹상태이면, 
	{
		return;																		// 리턴한다.
	}

	int nSysKey = MSK_NONE;															// 시스템 키 값을 기본으로 세팅한다.

	if( keyInfo->GetKeyPressed( KEY_CONTROL ) )		nSysKey |= MSK_CTRL;			// 컨트롤 키가 눌렸으면, 컨트롤 키를 추가한다.
	if( keyInfo->GetKeyPressed( KEY_MENU ) )		nSysKey |= MSK_ALT;				// 알트 키가 눌렸으면, 알트키를 추가한다.
	if( keyInfo->GetKeyPressed( KEY_SHIFT ) )		nSysKey |= MSK_SHIFT;			// 시프트 키가 눌렸으면, 시프트키를 추가한다.

	//090116 pdy 클라이언트 최적화 작업 ( Show Name On Press Key )
	//SHIFT키눌렸을시 이름보이기 추가 
	OBJECTMGR->SetPress_ShowNameKey( nSysKey & MSK_SHIFT );								// 090108 쉬프트키가 눌렸냐 때졋냐를 오브젝트 메니져에 셋팅


	for( int i = 0 ; i < MPE_COUNT ; ++i )											// 매크로 프레스 이벤트 수만큼 포문을 돌린다.
	{
		if( m_MacroPressKey[m_nMacroMode][i].nSysKey & nSysKey						// 현재 매크로 키와 시스템 키를 확인하고, 매크로 키가 눌렸으면,
			&& keyInfo->GetKeyPressed( m_MacroPressKey[m_nMacroMode][i].wKey ) )
		{
			PlayMacroPress( i );													// 매크로를 실행한다.
		}
	}		
}

void CMacroManager::GetMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( pMacro, m_MacroKey[nMacroMode], sizeof(sMACRO) * ME_COUNT );			// 들어온 매크로 모드로 들어온 매크로 구조체에 매크로 정보를 복사한다.
}

void CMacroManager::SetMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( m_MacroKey[nMacroMode], pMacro, sizeof(sMACRO) * ME_COUNT );			// 들어온 매크로 모드로 들어온 매크로 구조체를 멤버 매크로 구조체에 복사한다.
}

void CMacroManager::GetDefaultMacro( int nMacroMode, sMACRO* pMacro )
{
	memcpy( pMacro, m_DefaultKey[nMacroMode], sizeof(sMACRO) * ME_COUNT );			// 디폴트 매크로를 들어온 매크로 구조체에 복사한다.
}

void CMacroManager::SetMacroMode( int nMacroMode )
{
	m_nMacroMode = nMacroMode;														// 인자로 들어온 매크로 모드를 멤버 매크로 모드 변수에 복사한다.

	if( m_nMacroMode == MM_CHAT )													// 매크로모드가 채팅모드와 같다면,
	{
		m_bChatMode = TRUE;															// 채팅모드 변수를 TRUE로 세팅한다.
	}
	else																			// 매크로모드가 채팅모드와 같지 않다면,
	{
		m_bChatMode = FALSE;														// 채팅모드 변수를 FALSE로 세팅한다.
	}
}

void CMacroManager::LoadUserMacro()
{
	SetDefaultMacro( MM_MACRO );													// 디폴트 매크로를 단축키 우선 모드로 세팅한다.
	SetDefaultMacro( MM_CHAT );														// 디폴트 매크로를 채팅 우선 모드로 세팅한다.
	SetMacroMode( MM_MACRO );														// 매크로 모드를 단축키 우선 모드로 세팅한다.
}


BOOL CMacroManager::LoadMacro( LPCTSTR strPath )
{
	HANDLE	hFile;																	// 파일 핸들을 선언한다.

	hFile = CreateFile( strPath, GENERIC_READ, 0, NULL,								// 읽기모드로 들어온 패스의 파일을 읽어 핸들을 넘겨 받는다.
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )												// 핸들을 받는데 실패했다면,
		return FALSE;																// 리턴처리를 한다.

	DWORD dwRead;																	// 실제로 읽은 바이트 수를 리턴받기 위한 출력용 인수.
	
	if( !ReadFile( hFile, m_MacroKey,												// 매크로키 구조체에, 매크로 키 사이즈 만큼 읽어들인다.
		sizeof( m_MacroKey ), &dwRead, NULL ) )										// 읽어들이는게 실패하면,
	{
		CloseHandle( hFile );														// 핸들을 닫는다.
		return FALSE;																// FALSE 리턴 처리를 한다.
	}

	if( !ReadFile( hFile, m_MacroPressKey,											// m_MacroPressKey구조체에 m_MacroPressKey 사이즈 만큼 읽어들인다.
		sizeof( m_MacroPressKey ), &dwRead, NULL ) )								// 읽어들이는게 실패하면,
	{
		CloseHandle( hFile );														// 핸들을 닫는다.
		return FALSE;																// FALSE 리턴 처리를 한다.
	}
	
	CloseHandle( hFile );															// 핸들을 닫는다.

	return TRUE;																	// TRUE 리턴 처리를 한다.
}

void CMacroManager::SaveUserMacro()
{
	SaveMacro( "./INI/MacroUserSet.mcr" );											// 매크로를 저장한다.
}

BOOL CMacroManager::SaveMacro( LPCTSTR strPath )
{
	HANDLE	hFile;																	// 파일 핸들을 선언한다.

	hFile=CreateFile( strPath, GENERIC_WRITE, 0, NULL,								// 정해진 패스로 쓰기모드로 파일 핸들을 받아온다.
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )												// 파일 핸들을 받는데 실패하면,
		return FALSE;																// FALSE 리턴 처리를 한다.
	
	DWORD dwWritten;																// 실제로 쓴 바이트 수를 리턴받기 위한 출력용 인수.
	
	if( !WriteFile( hFile, m_MacroKey,												// m_MacroKey 구조체에 담긴 정보를 m_MacroKey 구조체 사이즈 만큼 쓴다.
		sizeof( m_MacroKey ), &dwWritten, NULL ) )									// 쓰기에 실패한다면,
	{
		CloseHandle( hFile );														// 핸들을 닫는다.
		return FALSE;																// FALSE 리턴 처리를 한다.
	}

	if( !WriteFile( hFile, m_MacroPressKey,											// m_MacroPressKey 구조체에 담긴 정보를 m_MacroPressKey 구조체 사이즈 만큼 쓴다.
		sizeof( m_MacroPressKey ), &dwWritten, NULL ) )								// 쓰기에 실패한다면,
	{
		CloseHandle( hFile );														// 핸들을 닫는다.
		return FALSE;																// FALSE 리턴처리를 한다.
	}

	CloseHandle( hFile );															// 핸들을 닫는다.
	return TRUE;																	// TRUE 리턴 처리를 한다.
}

void CMacroManager::PlayMacro( int nMacroEvent )
{
	if( CAMERA->GetCameraMode() == eCM_EyeView )									// 카메라 모드가 eCM_EyeView와 같다면,
	{
		return;																		// 리턴 처리를 한다.
	}

	switch( nMacroEvent )															// 매크로 이벤트를 확인한다.
	{
		case ME_USE_QUICKITEM_1_01: 												// 메인 퀵슬롯 1번키 사용
		case ME_USE_QUICKITEM_1_02: 												// 메인 퀵슬롯 2번키 사용
		case ME_USE_QUICKITEM_1_03: 												// 메인 퀵슬롯 3번키 사용
		case ME_USE_QUICKITEM_1_04: 												// 메인 퀵슬롯 4번키 사용
		case ME_USE_QUICKITEM_1_05: 												// 메인 퀵슬롯 5번키 사용
		case ME_USE_QUICKITEM_1_06: 												// 메인 퀵슬롯 6번키 사용
		case ME_USE_QUICKITEM_1_07: 												// 메인 퀵슬롯 7번키 사용
		case ME_USE_QUICKITEM_1_08: 												// 메인 퀵슬롯 8번키 사용
		case ME_USE_QUICKITEM_1_09: 												// 메인 퀵슬롯 9번키 사용
		case ME_USE_QUICKITEM_1_10:	PM_UseMainQuickSlotItem(nMacroEvent) ;	break ;	// 메인 퀵슬롯10번키 사용

		case ME_USE_QUICKITEM_2_01: 												// 서브 퀵슬롯 1번키 사용 
		case ME_USE_QUICKITEM_2_02: 			 									// 서브 퀵슬롯 2번키 사용 
		case ME_USE_QUICKITEM_2_03: 			 									// 서브 퀵슬롯 3번키 사용 
		case ME_USE_QUICKITEM_2_04: 			 									// 서브 퀵슬롯 4번키 사용 
		case ME_USE_QUICKITEM_2_05: 			 									// 서브 퀵슬롯 5번키 사용 
		case ME_USE_QUICKITEM_2_06: 			 									// 서브 퀵슬롯 6번키 사용 
		case ME_USE_QUICKITEM_2_07: 			 									// 서브 퀵슬롯 7번키 사용 
		case ME_USE_QUICKITEM_2_08: 			 									// 서브 퀵슬롯 8번키 사용 
		case ME_USE_QUICKITEM_2_09: 			 									// 서브 퀵슬롯 9번키 사용 
		case ME_USE_QUICKITEM_2_10:	PM_UseSubQuickSlotItem(nMacroEvent) ;	break ;	// 서브 퀵슬롯10번키 사용 

		case ME_CHANGE_QUICKSLOT_01: 												// 메인 퀵슬롯을 1번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_02: 												// 메인 퀵슬롯을 2번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_03: 												// 메인 퀵슬롯을 3번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_04: 												// 메인 퀵슬롯을 4번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_05: 												// 메인 퀵슬롯을 5번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_06: 												// 메인 퀵슬롯을 6번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_07: 												// 메인 퀵슬롯을 7번으로 세팅함.
		case ME_CHANGE_QUICKSLOT_08:	PM_Change_MainSlotNum(nMacroEvent) ;break ;	// 메인 퀵슬롯을 8번으로 세팅함.

		case ME_INCREASE_MAIN_QUICKSLOT_NUM:														// 메인 퀵슬롯 번호를 증가 시킨다.
		case ME_DECREASE_MAIN_QUICKSLOT_NUM: PM_SetSlotPage(QI1_QUICKSLOTDLG, nMacroEvent) ; break ;// 메인 퀵슬롯 번호를 감소 시킨다.

		// 080707 LYW --- MacroManager : 확장 슬롯 단축키는 막는다.
		//case ME_INCREASE_SUB_QUICKSLOT_NUM: 														// 서브 퀵슬롯 번호를 증가 시킨다.
		//case ME_DECREASE_SUB_QUICKSLOT_NUM:  PM_SetSlotPage(QI2_QUICKSLOTDLG, nMacroEvent) ; break ;// 서브 퀵슬롯 번호를 감소 시킨다.

		case ME_SELECT_MYSELF: HERO->ShowObjectName( TRUE, NAMECOLOR_SELECTED ); OBJECTMGR->SetSelectedObject( HERO ); break ;// 내 자신을 선택한다.

		case ME_SELECT_PARTYMEMBER_1: 												// 첫 번째 파티 멤버를 선택한다.
		case ME_SELECT_PARTYMEMBER_2: 												// 두 번째 파티 멤버를 선택한다.
		case ME_SELECT_PARTYMEMBER_3: 												// 세 번째 파티 멤버를 선택한다.
		case ME_SELECT_PARTYMEMBER_4: 												// 네 번째 파티 멤버를 선택한다.
		case ME_SELECT_PARTYMEMBER_5: 												// 다섯 번째 파티 멤버를 선택한다.
		case ME_SELECT_PARTYMEMBER_6:	PM_SelectPartyMember(nMacroEvent) ;break ;	// 여섯 번째 파티 멤버를 선택한다.

		case ME_SELECT_NEAR_TARGET:		PM_SetAutoTarget() ;				break ;	// 가장 가까이에 있는 타겟을 선택한다.

		case ME_SCREENCAPTURE:					PM_ScreenCapture() ;		break ;	// 스크린을 캡쳐한다.

		case ME_TOGGLE_EXITDLG:					PM_Toggle_ExitDlg() ;		break ;	// 종료 메시지 창을 띄운다.

		case ME_TOGGLE_ALLINTERFACE:			PM_Toggle_AllInterface() ;	break ;	// 모든 인터페이스를 토글한다.
		
		case ME_TOGGLE_SKILLDLG:				PM_Toggle_SkillDlg() ;		break ;	// 스킬창을 토글한다.

		case ME_TOGGLE_FAMILYDLG:				PM_Toggle_FamilyDlg() ;		break ;	// 패밀리 창을 토글한다.

		case ME_TOGGLE_INVENTORYDLG:			PM_Toggle_InventoryDlg() ;  break ;	// 인벤토리를 토글한다.

		case ME_TOGGLE_HELP_SHOW:				PM_Toggle_ShowKeyTip() ;	break ;	// 도움말을 토글한다.

		case ME_TOGGLE_BIGMAP:					PM_Toggle_BigMap() ;		break ;	// 빅맵을 토글한다.

		case ME_TOGGLE_WORLDMAP:				PM_Toggle_WorldMap() ;		break ;	// 월드맵을 토글한다.

		case ME_TOGGLE_OPTIONDLG:				PM_Toggle_OptionDlg() ;		break ;	// 옵션 창을 토글한다.

		case ME_TOGGLE_GUILDDLG:				PM_Toggle_GuildDlg() ;		break ;	// 길드 창을 토글한다.

		case ME_TOGGLE_QUESTDLG:				PM_Toggle_QuestDlg() ;		break ;	// 퀘스트 창을 토글한다.

		case ME_TOGGLE_CHARINFODLG:				PM_Toggle_CharInfoDlg() ;	break ;	// 캐릭터 정보창을 토글한다.

		case ME_WALK_RUN:						MOVEMGR->ToggleHeroMoveMode() ;	break ;	// 걷기/뛰기 모드를 전환한다.

		case ME_TOGGLE_CAMERAVIEWMODE:			CAMERA->ToggleCameraViewMode(); break ;	// 카메라 뷰 모드를 전환한다.

		case ME_TOGGLE_MATCHINGDLG:				PM_Toggle_MatchingDlg() ;		break ;	// 매칭 창을 토글한다.

		case ME_TOGGLE_FRIENDDLG:				PM_Toggle_FriendDlg() ;		break ;		// 친구 목록을 토글한다.

		// 071025 LYW --- MacroManager : Add macro for help dialog.
		case ME_TOGGLE_HELPDLG :				PM_Toggle_HelpDlg() ;		break ;		// 도움말 창을 토글한다.

		case ME_FISHING_PULLINGBTN:				GAMEIN->GetFishingDlg()->Fishing_Pullling();	break;
		case ME_TOGGLE_GTRESULT_DLG:			PM_Toggle_GTResultDlg();	break;

		// 080429 LYW --- MacroManager : Add macro for chatroom system.
		case ME_TOGGLE_CHATROOM_LISTDLG :		PM_Toggle_ChatRoomListDlg() ; break ;	// 채팅방 리스트창을 토글한다.
		case ME_TOGGLE_CHATROOM_ROOMDLG :		PM_Toggle_ChatRoomDlg() ;	  break ;	// 채팅방을 토클한다.
		case ME_SETFOCUS_CHATROOM_CHATTING :	PM_SetFocus_ChatRoom_Chat() ; break ;	// 채팅창과 채팅방 사이로 에디트 박스 포커스를 이동시킨다.


		case ME_PET_INFO : 	PM_Toggle_Pet_Info();	break;

		// 090422 ShinJS --- 탈것 탑승/하차 실행
		case ME_VEHICLE_GETONOFF :				PM_Toggle_Vehicle_GetOnOff();	break;			

		// 100105 ONS 앉기/일어서기를 토글한다.
		case ME_TOGGLE_RESTMODE:				CONDUCTMGR->OnConductCommand( 1 ) ;	break;

		default : break ;
	}
}

void CMacroManager::PM_Toggle_ExitDlg()
{
	if( HERO->GetCurrentSkill() )
	{
		SKILLMGR->SkillCancel(
			SKILLMGR->GetSkillObject(HERO->GetCurrentSkill()));
	}
	else if(GAMEIN->GetFishingDlg()->IsPushedStartBtn())		// 낚시중이면 낚시만 중단한다.
	{
		GAMEIN->GetFishingDlg()->Fishing_Cancel();
	}
	else if(CVehicle* vehicleObject = (CVehicle*)OBJECTMGR->GetObject(VEHICLE_INSTALL_ID))
	{
		if(eObjectKind_Vehicle == vehicleObject->GetObjectKind())
		{
			if(CItem* const item = ITEMMGR->GetItem(vehicleObject->GetUsedItem().dwDBIdx))
			{
				item->SetLock(
					FALSE);
			}

			OBJECTMGR->RemoveVehicle(
				vehicleObject->GetID());
		}

        GAMEIN->GetInventoryDialog()->SetDisable(
			FALSE);
	}
	// 090611 pdy 하우징 가구 설치중에 esc눌렸을때 설치 취소를 해주자 
	else if( HOUSINGMGR->IsHousingMap()	&& HOUSINGMGR->IsDoDecoration() )
	{
		HOUSINGMGR->CancelDecoration();
	}
	else if( !WINDOWMGR->CloseAllAutoCloseWindows() )									// 자동으로 닫히는 윈도우들을 닫는 것이 실패하면,?
	{
		// 080117 LYW --- MacroManager : 챌린지 맵에서는 ESC키 기능을 막는다.
		if (!g_csDateManager.IsChallengeZone(MAP->GetMapNum()))
		{
			CExitDialog * pWindow = GAMEIN->GetExitDialog();							// 종료 메시지 윈도우 정보를 받아온다.

			if(pWindow)																	// 종료 메시지 윈도우 정보가 유효하면,
			{
				pWindow->SetActive(!pWindow->IsActive()) ;								// 종료 메시지 윈도우를 활성, 비활성화 한다.
			}
		}
	}

	// 090317 ShinJS --- 챌린지 맵에서 스코어는 지우고 LimitTime 활성화를 위한 처리
	if( g_csDateManager.IsChallengeZone(MAP->GetMapNum()) )
	{		
		CSHChallengeZoneClearNo1Dlg* pDlg = GAMEIN->GetChallengeZoneClearNo1Dlg();
		if( pDlg )
			pDlg->SetOnlyRenderLimitTime( !pDlg->GetOnlyRenderLimitTime() );
	}

	if(CObject* const object = OBJECTMGR->GetObject(m_dwTargetID))															// 오브젝트 정보가 유효하다면,
	{
		object->ShowObjectName(
			FALSE,
			NAMECOLOR_DEFAULT);
		OBJECTMGR->SetSelectedObject(
			NULL);
	}
}

void CMacroManager::PM_ScreenCapture()
{
	DIRECTORYMGR->SetLoadMode(eLM_Root);											// 로드 모드를 eLM_Root로 세팅한다.

	CreateDirectory( "ScreenShot", NULL );											// 스크린 샷 디렉토리를 생성한다.
	
	WORD wSrv = 0, wY = 0, wM = 0, wD = 0, wN = 0;									// 연도를 담을 변수들을 선언한다.
	WORD wHH = 0, wMM = 0, wSS = 0;													// 시간대를 담을 변수들을 선언한다.

	char fname[256];																// 파일명을 담을 임시 버퍼를 선언한다.

	SYSTEMTIME ti;																	// 시간 정보를 담을 구조체를 선언한다.
	GetLocalTime( &ti );															// 로컬시간을 받아온다.

	HANDLE hFile = CreateFile( "screenshot/ScreenShot.cnt", GENERIC_READ, 0, NULL,	// 읽기모드로 ScreenShot.cnt를 읽어 파일핸들을 넘겨 받는다.
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	DWORD dwSize = 0;																// 사이즈를 담을 변수를 선언한다.

	if( hFile != INVALID_HANDLE_VALUE )												// 핸들을 넘겨받는데 성공했다면,
	{
		ReadFile( hFile, &wSrv, sizeof(wSrv), &dwSize, NULL );						// 서버셋 번호를 받는다.
		ReadFile( hFile, &wY, sizeof(wY), &dwSize, NULL );							// 년도를 받는다.
		ReadFile( hFile, &wM, sizeof(wM), &dwSize, NULL );							// 월을 받는다.
		ReadFile( hFile, &wD, sizeof(wD), &dwSize, NULL );							// 요일을 받는다.
		ReadFile( hFile, &wHH, sizeof(wHH), &dwSize, NULL );						// 시간을 받는다.
		ReadFile( hFile, &wMM, sizeof(wMM), &dwSize, NULL );						// 분을 받는다.
		ReadFile( hFile, &wSS, sizeof(wSS), &dwSize, NULL );						// 초를 받는다.
		ReadFile( hFile, &wN, sizeof(wN), &dwSize, NULL );							// 밀리 세컨드 값을 받는다.
		
		CloseHandle( hFile );														// 파일 핸들을 닫는다.

		if( !( wSrv == (WORD)g_nServerSetNum && ti.wYear == wY &&					// 읽어들인 정보들이
			ti.wMonth == wM && ti.wDay == wD && ti.wHour == wHH &&					// 로컬 시간대와 같지 않으면,
			ti.wMinute == wMM && ti.wSecond == wSS) )
		{
			wSrv = WORD(g_nServerSetNum);
			wY = ti.wYear;
			wM = ti.wMonth;					// GetLocalTime()으로 받은 정보로 변수들을 세팅한다.
			wD = ti.wDay;
			wN = 1; 
			wHH = ti.wHour;
			wMM = ti.wMinute;
			wSS = ti.wSecond;
		}					
	}
	else // 핸들을 받는데 실패했다면,
	{
		wSrv = WORD(g_nServerSetNum);
		wY = ti.wYear;
		wM = ti.wMonth;						// GetLocalTime()으로 받은 정보로 변수들을 세팅한다.
		wD = ti.wDay;
		wN = 1; 
		wHH = ti.wHour;
		wMM = ti.wMinute;
		wSS = ti.wSecond;
	}


	sprintf(fname,"ScreenShot/Luna_%02d_%02d%02d%02d_%02d%02d%02d_%03d.jpg",		// 파일명을 위에서 설정한 정보들을 합하여 설정한다.
		wSrv, wY%100, wM, wD, wHH, wMM, wSS, wN );

	BOOL rt = g_pExecutive->GetRenderer()->CaptureScreen(fname);					// 파일명으로 스크린을 캡쳐한다.

	if( rt )																		// 캡쳐한 결과가 성공이면,
	{
		hFile = CreateFile( "screenshot/ScreenShot.cnt", GENERIC_WRITE, 0, NULL,	// 쓰기모드로 ScreenShot.cnt 파일을 쓰고 
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );				// 파일 핸들을 넘겨 받는다.

		if( hFile != INVALID_HANDLE_VALUE )											// 파일 쓰기에 성공했다면,
		{
			++wN;																	// 밀리 세컨드 값을 증가시킨다.

			WriteFile( hFile, &wSrv, sizeof(wSrv), &dwSize, NULL );					// 서버셋 번호를 쓴다.
			WriteFile( hFile, &wY, sizeof(wY), &dwSize, NULL );						// 연도를 기록한다.
			WriteFile( hFile, &wM, sizeof(wM), &dwSize, NULL );						// 달을 기록한다.
			WriteFile( hFile, &wD, sizeof(wD), &dwSize, NULL );						// 요일을 기록한다.
			WriteFile( hFile, &wHH, sizeof(wHH), &dwSize, NULL );					// 시간을 기록한다.
			WriteFile( hFile, &wMM, sizeof(wMM), &dwSize, NULL );					// 분을 기록한다.
			WriteFile( hFile, &wSS, sizeof(wSS), &dwSize, NULL );					// 초를 기록한다.
			WriteFile( hFile, &wN, sizeof(wN), &dwSize, NULL );						// 밀리 초를 기록한다.

			CloseHandle( hFile );													// 파일 핸들을 닫는다.
		}

		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(387), fname );				// 스크린을 캡쳐 했다는 메시지를 출력한다.
	}
	else																			// 파일 쓰기에 실패했다면,
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(386) );					// 스크린 캡쳐 실패 메시지를 출력한다.
	}

	FIBITMAP* pBitmap = FreeImage_Load(FIF_TARGA, fname, TARGA_DEFAULT);			// 프리 이미지 정보를 로드한다.
	
	if(pBitmap)																		// 이미지 정보가 유효하면,
	{
		DeleteFile(fname);															// 파일을 삭제한다.

		int len = strlen(fname);													// 파일명의 길이를 구한다.

		fname[len-3] = 'j';															// 파일명에 jpg를 추가한다.
		fname[len-2] = 'p';
		fname[len-1] = 'g';

		FreeImage_SaveJPEG(pBitmap,fname,JPEG_QUALITYSUPERB);						// jpg 이미지로 저장을 한다.

		FreeImage_Unload(pBitmap);													// 받아온 이미지 정보를 반환한다.
	}
}

void CMacroManager::PM_UseMainQuickSlotItem( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;

	switch( nMacroEvent )															// 매크로 이벤트를 확인한다.
	{
	case ME_USE_QUICKITEM_1_01:		slotNum = 0 ;	break ;							// 슬롯 번호를 0으로 세팅한다.
	case ME_USE_QUICKITEM_1_02:		slotNum = 1 ;	break ;							// 슬롯 번호를 1으로 세팅한다.
	case ME_USE_QUICKITEM_1_03:		slotNum = 2 ;	break ;							// 슬롯 번호를 2으로 세팅한다.
	case ME_USE_QUICKITEM_1_04:		slotNum = 3 ;	break ;							// 슬롯 번호를 3으로 세팅한다.
	case ME_USE_QUICKITEM_1_05:		slotNum = 4 ;	break ;							// 슬롯 번호를 4으로 세팅한다.
	case ME_USE_QUICKITEM_1_06:		slotNum = 5 ;	break ;							// 슬롯 번호를 5으로 세팅한다.
	case ME_USE_QUICKITEM_1_07:		slotNum = 6 ;	break ;							// 슬롯 번호를 6으로 세팅한다.
	case ME_USE_QUICKITEM_1_08:		slotNum = 7 ;	break ;							// 슬롯 번호를 7으로 세팅한다.
	case ME_USE_QUICKITEM_1_09:		slotNum = 8 ;	break ;							// 슬롯 번호를 8으로 세팅한다.
	case ME_USE_QUICKITEM_1_10:		slotNum = 9 ;	break ;							// 슬롯 번호를 9으로 세팅한다.
	}

	if(USHRT_MAX != slotNum)
	{
		QUICKMGR->UseQuickItem( GAMEIN->GetQuickDlg()->GetSlotDlg( 0 )->GetCurSheet(), slotNum );
	}
}

void CMacroManager::PM_UseSubQuickSlotItem( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;
																					
	switch( nMacroEvent )															// 매크로 이벤트를 확인한다.
	{																				
	case ME_USE_QUICKITEM_2_01:		slotNum = 0 ;	break ;							// 슬롯 번호를 0으로 세팅한다.
	case ME_USE_QUICKITEM_2_02:		slotNum = 1 ;	break ;							// 슬롯 번호를 1으로 세팅한다.
	case ME_USE_QUICKITEM_2_03:		slotNum = 2 ;	break ;							// 슬롯 번호를 2으로 세팅한다.
	case ME_USE_QUICKITEM_2_04:		slotNum = 3 ;	break ;							// 슬롯 번호를 3으로 세팅한다.
	case ME_USE_QUICKITEM_2_05:		slotNum = 4 ;	break ;							// 슬롯 번호를 4으로 세팅한다.
	case ME_USE_QUICKITEM_2_06:		slotNum = 5 ;	break ;							// 슬롯 번호를 5으로 세팅한다.
	case ME_USE_QUICKITEM_2_07:		slotNum = 6 ;	break ;							// 슬롯 번호를 6으로 세팅한다.
	case ME_USE_QUICKITEM_2_08:		slotNum = 7 ;	break ;							// 슬롯 번호를 7으로 세팅한다.
	case ME_USE_QUICKITEM_2_09:		slotNum = 8 ;	break ;							// 슬롯 번호를 8으로 세팅한다.
	case ME_USE_QUICKITEM_2_10:		slotNum = 9 ;	break ;							// 슬롯 번호를 9으로 세팅한다.
	}																				
																					
	if(USHRT_MAX != slotNum)
	{		
		QUICKMGR->UseQuickItem(
			GAMEIN->GetQuickDlg()->GetSlotDlg(1)->GetCurSheet() + MAX_SLOTPAGE,
			slotNum);
	}
}

void CMacroManager::PM_Change_MainSlotNum( int nMacroEvent )
{
	WORD slotNum = USHRT_MAX;
																					
	switch( nMacroEvent )															// 매크로 이벤트를 확인한다.
	{																				
	case ME_CHANGE_QUICKSLOT_01:		slotNum = 0 ;	break ;						// 슬롯 번호를 0으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_02:		slotNum = 1 ;	break ;						// 슬롯 번호를 1으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_03:		slotNum = 2 ;	break ;						// 슬롯 번호를 2으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_04:		slotNum = 3 ;	break ;						// 슬롯 번호를 3으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_05:		slotNum = 4 ;	break ;						// 슬롯 번호를 4으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_06:		slotNum = 5 ;	break ;						// 슬롯 번호를 5으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_07:		slotNum = 6 ;	break ;						// 슬롯 번호를 6으로 세팅한다.
	case ME_CHANGE_QUICKSLOT_08:		slotNum = 7 ;	break ;						// 슬롯 번호를 7으로 세팅한다.
	}																				
																					
	if(USHRT_MAX != slotNum)
	{
		if(cQuickSlotDlg* pSlot = (cQuickSlotDlg*)WINDOWMGR->GetWindowForID(QI1_QUICKSLOTDLG))
		{
			pSlot->SelectPage(slotNum);
		}
	}
}

void CMacroManager::PM_SetSlotPage( DWORD dwSlotID, int nMacroEvent )
{
	cQuickSlotDlg* pSlot = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( dwSlotID ) ;// 메인 슬롯을 얻어온다.

	if(pSlot)																		// 슬롯 정보가 유효한지 체크한다.
	{
		WORD curPage = pSlot->GetCurSheet() ;										// 현재 페이지를 받아온다.

		switch( nMacroEvent )														// 매크로 이벤트를 확인한다.
		{
		case ME_INCREASE_MAIN_QUICKSLOT_NUM :										// 페이지를 증가시키는 이벤트라면,
		case ME_INCREASE_SUB_QUICKSLOT_NUM :
			{
				if( curPage >= MAX_SLOTPAGE-1 )										// 페이지가 마지막 페이지라면,
				{
					curPage = 0 ;													// 마지막 페이지로 세팅한다.
				}
				else																// 그렇지 않을경우,
				{
					curPage = curPage+1 ;											// 페이지를 하나 증가시킨다.
				}
			}
			break ;

		case ME_DECREASE_MAIN_QUICKSLOT_NUM :										// 페이지를 감소시키는 이벤트라면,
		case ME_DECREASE_SUB_QUICKSLOT_NUM :
			{
				if( curPage <= 0 )													// 페이지가 최소 페이지라면,
				{
					curPage = MAX_SLOTPAGE-1 ;										// 첫 페이지로 세팅한다.
				}
				else																// 그렇지 않을경우,
				{
					curPage = curPage-1 ;											// 페이지를 하나 감소시킨다.
				}
			}
			break ;
		}

		pSlot->SelectPage( curPage );												// 페이지를 세팅한다.
	}
}

void CMacroManager::PM_SelectPartyMember( int nMacroEvent )
{
	DWORD dwMemberID = 0;

	if(PARTYMGR->GetParty().GetMasterIdx() == HEROID)
	{
		switch(nMacroEvent)
		{
		case ME_SELECT_PARTYMEMBER_1: dwMemberID = PARTYMGR->GetParty().GetMemberID(1);	break;
		case ME_SELECT_PARTYMEMBER_2: dwMemberID = PARTYMGR->GetParty().GetMemberID(2);	break;
		case ME_SELECT_PARTYMEMBER_3: dwMemberID = PARTYMGR->GetParty().GetMemberID(3);	break;
		case ME_SELECT_PARTYMEMBER_4: dwMemberID = PARTYMGR->GetParty().GetMemberID(4);	break;
		case ME_SELECT_PARTYMEMBER_5: dwMemberID = PARTYMGR->GetParty().GetMemberID(5);	break;
		case ME_SELECT_PARTYMEMBER_6: dwMemberID = PARTYMGR->GetParty().GetMemberID(6);	break;
		}
	}
	else
	{
		DWORD dwMemberOrder[MAX_PARTY_LISTNUM] = {0};
		int orderCount = 0;

		for(int count = 0; count < _countof(dwMemberOrder); ++count)
		{
			DWORD dwCurMemberID = PARTYMGR->GetParty().GetMemberID(count);

			if(dwCurMemberID == gHeroID)
			{
				continue;
			}

			dwMemberOrder[orderCount] = dwCurMemberID;
			++orderCount;
		}

		switch(nMacroEvent)
		{
		case ME_SELECT_PARTYMEMBER_1: dwMemberID = dwMemberOrder[0]; break;
		case ME_SELECT_PARTYMEMBER_2: dwMemberID = dwMemberOrder[1]; break;
		case ME_SELECT_PARTYMEMBER_3: dwMemberID = dwMemberOrder[2]; break;
		case ME_SELECT_PARTYMEMBER_4: dwMemberID = dwMemberOrder[3]; break;
		case ME_SELECT_PARTYMEMBER_5: dwMemberID = dwMemberOrder[4]; break;
		case ME_SELECT_PARTYMEMBER_6: dwMemberID = dwMemberOrder[5]; break;
		}
	}

	if(dwMemberID)
	{
		GAMEIN->GetPartyDialog()->SetClickedMemberID(dwMemberID);
	}
}
void CMacroManager::PM_Toggle_AllInterface()
{
	// 071205 LUJ	NPC 이미지 창에서 자체적으로 모든 창을 토글한다. 따라서 해당 창이 열려있을 때
	//				토글 동작을 수행하면 단독으로 표시되어야할 NPC 창 위에 다른 창들이 겹쳐서 
	//				표시되는 문제가 발생한다
	{
		cDialog* dialog = WINDOWMGR->GetWindowForID( NSI_SCRIPTDLG );
		ASSERT( dialog );

		if( dialog->IsActive() )
		{
			return;
		}
	}

	if( WINDOWMGR->IsOpendAllWindows() )											// 윈도우들이 열려있는 상태라면,
	{
		//WINDOWMGR->m_pActivedWindowList->RemoveAll() ;								// 열려있는 윈도우를 담고있는 리스트를 모두 비운다.
		//WINDOWMGR->CloseAllWindow() ;												// 모든 윈도우를 닫는다.
		
		//090119 pdy Window Hide Mode
		WINDOWMGR->HideAllWindow() ;												//모든 윈도우를 숨긴다.											

		WINDOWMGR->SetOpendAllWindows( FALSE ) ;									// 모든 윈도우가 닫혀있는 상태로 세팅한다.
	}
	else																			// 윈도우들이 닫혀있는 상태라면,
	{
		//WINDOWMGR->ShowAllActivedWindow() ;											// 활성화 된 모든 윈도우들을 보여준다.

		//090119 pdy Window Hide Mode
		WINDOWMGR->UnHideAllActivedWindow();										// 백업되있는 숨겨진 윈도우들을 UnHide한다.

		WINDOWMGR->SetOpendAllWindows( TRUE ) ;										// 모든 윈도우가 열려있는 상태로 세팅한다.

		WINDOWMGR->ShowBaseWindow() ;												// 기본적으로 보이는 윈도우들을 보여준다.
	}
}

void CMacroManager::PM_Toggle_SkillDlg()
{
	cSkillTreeDlg* pDlg = GAMEIN->GetSkillTreeDlg() ;								// 스킬창 정보를 받아온다.

	if( pDlg )																		// 스킬창 정보가 유효한지 체크한다.
	{
		pDlg->SetActive( !pDlg->IsActive() );										// 스킬창을 활성, 비활성화 한다.
	}
}

void CMacroManager::PM_Toggle_FamilyDlg()
{
	CSHFamily* pFamily = HERO->GetFamily() ;	

	if( pFamily && pFamily->Get()->nMasterID != 0 )
	{
		CFamilyDialog* pDlg = GAMEIN->GetFamilyDlg() ;

		if( pDlg )
		{
			if( pDlg->IsActive() )
			{
				pDlg->SetActive(FALSE) ;
			}
			else
			{
				pDlg->UpdateAllInfo() ;
				pDlg->SetActive(TRUE) ;
			}
		}
	}
	else
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 369 ));
	}
}

void CMacroManager::PM_Toggle_InventoryDlg()
{
	CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog() ;						// 인벤토리 다이얼로그 정보를 받아온다.

	if( !pDlg ) return ;															// 다이얼로그 정보가 유효한지 체크한다.
	
	pDlg->SetActive( !pDlg->IsActive() ) ;											// 다이얼로그를 활성, 비활성화 한다.
}

void CMacroManager::PM_Toggle_ShowKeyTip()
{
	if(CKeySettingTipDlg* pDlg = GAMEIN->GetKeySettingTipDlg())
	{
		pDlg->SetMode(
			WORD(m_nMacroMode));
		pDlg->SetActive(
			! pDlg->IsActive());
	}
}

void CMacroManager::PM_Toggle_BigMap()
{
	CBigMapDlg* pDlg = GAMEIN->GetBigMapDialog();
	if( pDlg )
	{
		pDlg->SetActive( !pDlg->IsActive() );
	}
}

void CMacroManager::PM_Toggle_WorldMap()
{
	CWorldMapDlg* pDlg = GAMEIN->GetWorldMapDialog();
	if( pDlg )
	{
		pDlg->SetActive( !pDlg->IsActive() );
	}
}

void CMacroManager::PM_Toggle_OptionDlg()
{
	COptionDialog* pDlg = GAMEIN->GetOptionDialog() ;								// 옵션 다이얼로그 정보를 받아온다.

	if( pDlg )																		// 다이얼로그 정보가 유효한지 체크한다.
	{
		pDlg->SetActive( !pDlg->IsActive() ) ;										// 다이얼로그를 활성, 비활성화 한다.
	}
}

void CMacroManager::PM_Toggle_GuildDlg() 
{
	// CGuildDialog;
	cDialog* dialog = WINDOWMGR->GetWindowForID( GD_GUILDDLG );
	ASSERT( dialog );

	dialog->SetActive( ! dialog->IsActive() );
}

void CMacroManager::PM_Toggle_QuestDlg()
{
	CQuestTotalDialog* pDlg = GAMEIN->GetQuestTotalDialog() ;						// 퀘스트 다이얼로그 정보를 받아온다.

	if( pDlg )																		// 다이얼로그 정보가 유효한지 체크한다.
	{
		pDlg->SetActive( !pDlg->IsActive() ) ;										// 다이얼로그를 활성, 비활성화 한다.

		CQuestDialog* pQuestDialog = GAMEIN->GetQuestDialog() ;

		if( pQuestDialog )
		{
			if( pQuestDialog->GetQuestCount() > 0 )
			{
				DWORD dwQuestIdx = 0 ;
				dwQuestIdx = pQuestDialog->GetSelectedQuestID() ;

				if( dwQuestIdx < 1 ) 
				{
					DWORD questIndex = pQuestDialog->GetSelectedQuestIDFromTree( 0 );
					pQuestDialog->SetSelectedQuestIdx( questIndex );
				}
				pQuestDialog->RefreshQuestList();
			}
		}
	}
}

void CMacroManager::PM_Toggle_CharInfoDlg()
{
	CCharacterDialog* pDlg = GAMEIN->GetCharacterDialog() ;

	if( pDlg )																		// 다이얼로그 정보가 유효한지 체크한다.
	{
		pDlg->RefreshFamilyInfo() ;													// 패밀리 정보를 새로고침 한다.
		pDlg->SetActive( !pDlg->IsActive() ) ;										// 다이얼로그를 활성, 비활성화 한다.
	}
}

void CMacroManager::PM_Toggle_MatchingDlg()
{
	const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;				// 매칭 정보를 받아온다.

	if(!dateInfo.bIsValid)															// 매칭 정보가 유효하지 않으면,
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1284 ) );
	}
	else																			// 매칭 정보가 유효하다면,
	{
		CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;						// 매칭 다이얼로그 정보를 받아온다.

		if( pDlg )																	// 다이얼로그 정보가 유효한지 체크한다.
		{
			if( pDlg->IsActive() )													// 다이얼로그가 열려 있다면,
			{
				CDateMatchingChatDlg* pChatDlg = NULL ;								// 매칭 채팅 다이얼로그 정보를 받을 포인터 변수를 선언한다.

				pChatDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;			// 매칭 다이얼로그 정보를 받아온다.

				if( pChatDlg )														// 다이얼로그 정보가 유효한지 체크한다.
				{
					if( pChatDlg->IsOnChatMode() )									// 채팅 중이라면,
					{
						// notice end chatting.
						WINDOWMGR->MsgBox( MBI_END_CHATTING_CLOSE, MBT_YESNO, CHATMGR->GetChatMsg( 1304 ) );
					}
					else															// 채팅 중이 아니라면,
					{
						pDlg->SetActive(FALSE) ;									// 다이얼로그를 비활성화 한다.
					}
				}
			}
			else																	// 채팅 다이얼로그다 비활성화 중이라면,
			{
				pDlg->GetPartnerListDlg()->SettingControls() ;						// 파트너 리스트의 컨드롤들을 세팅한다.
				//pDlg->GetPartnerListDlg()->UpdateInfo() ;
				pDlg->SetActive(TRUE) ;												// 채팅 다이얼로그를 활성화 한다.
			}
		}
	}
}

void CMacroManager::PM_SetAutoTarget()
{
	cDialog* const dialog = WINDOWMGR->GetWindowForID(
		NSI_SCRIPTDLG);

	if(0 == dialog)
	{
		return;
	}
	else if(dialog->IsActive())
	{
		return;
	}

	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	RECT rect = {0};
	rect.left = screenRect.left;
	rect.top = screenRect.top;
	rect.right = screenRect.right;
	rect.bottom = screenRect.bottom;

	const size_t maxFindSize = 200;
	PICK_GXOBJECT_DESC descriptionArray[maxFindSize] = {0};
	const DWORD pickedSize = g_pExecutive->GXOGetMultipleObjectWithScreenCoordRect(
		descriptionArray,
		_countof(descriptionArray),
		&rect,
		0,
		PICK_TYPE_PER_COLLISION_MESH);

	typedef DWORD ObjectIndex;
	typedef std::list< ObjectIndex > ObjectIndexContainer;
	static ObjectIndexContainer objectIndexContainer;
	static PICK_GXOBJECT_DESC previousDescriptionArray[maxFindSize] = {0};
	static BOOL isFirstTarget = TRUE;

	if(0 == memcmp(descriptionArray, previousDescriptionArray, sizeof(descriptionArray)))
	{
		isFirstTarget = ! isFirstTarget;
	}
	else
	{
		isFirstTarget = TRUE;

		memcpy(
			previousDescriptionArray,
			descriptionArray,
			sizeof(descriptionArray));

		VECTOR3 heroPosition = {0};
		CHero* const hero = OBJECTMGR->GetHero();
		hero->GetPosition(
			&heroPosition);

		typedef float Distance;
		typedef std::map< Distance, ObjectIndex > DistanceContainer;
		DistanceContainer distanceContainer;

		for(DWORD i = pickedSize; i-- > 0;)
		{
			const PICK_GXOBJECT_DESC& description = descriptionArray[i];

			CEngineObject* const engineObject = (CEngineObject*)g_pExecutive->GetData(
				description.gxo);

			if(0 == engineObject)
			{
				continue;
			}

			CObject* const object = (CObject*)engineObject->GetBaseObject();

			if(0 == object)
			{
				continue;
			}
			else if(gHeroID == object->GetID())
			{
				continue;
			}
			else if(eObjectKind_Monster == object->GetObjectKind())
			{
				CMonster* const monsterObject = (CMonster*)object;

				if(const BOOL isPuppy = (2 == monsterObject->GetSInfo()->SpecialType))
				{
					continue;
				}
			}

			if(eObjectBattleState_Battle == hero->GetObjectBattleState())
			{
				if(FALSE == (eObjectKind_Monster & object->GetObjectKind()))
				{
					continue;
				}
				else if(eObjectKind_Vehicle == object->GetObjectKind())
				{
					continue;
				}
			}

			VECTOR3 objectPosition = {0};
			object->GetPosition(
				&objectPosition);

			Distance distance = CalcDistance(
				&heroPosition,
				&objectPosition);

			while(distanceContainer.end() != distanceContainer.find(distance))
			{
				distance += 0.001f;
			}

			distanceContainer[distance] = object->GetID();
		}

		objectIndexContainer.clear();

		CObject* pTargetObject	=	NULL;

		for(DistanceContainer::const_iterator iterator = distanceContainer.begin();
			distanceContainer.end() != iterator;
			++iterator)
		{
			const ObjectIndex objectIndex = iterator->second;

			// 100803 플래이어, 탈것, 팻 제외
			pTargetObject	=	OBJECTMGR->GetObject( objectIndex );
			if( pTargetObject )
			{
				if( pTargetObject->GetObjectKind() != eObjectKind_Player &&
					pTargetObject->GetObjectKind() != eObjectKind_Vehicle &&
					pTargetObject->GetObjectKind() != eObjectKind_Pet )
				{
					objectIndexContainer.push_back(objectIndex);
				}
			}

			if(1 < objectIndexContainer.size())
			{
				break;
			}
		}
	}

	CObject* const heroObject = OBJECTMGR->GetObject(
		gHeroID);

	if(0 == heroObject)
	{
		return;
	}

	ObjectIndex targetObjectIndex = 0;

	if(objectIndexContainer.empty())
	{
		OBJECTMGR->SetSelectedObject(
			0);
		targetObjectIndex = 0;
	}
	else if(1 == objectIndexContainer.size() ||
		isFirstTarget)
	{
		targetObjectIndex = (*objectIndexContainer.begin());
	}
	else
	{
		targetObjectIndex = *(++objectIndexContainer.begin());
	}

	CObject* const targetObject = OBJECTMGR->GetObject(
		targetObjectIndex);

	if(0 == targetObject)
	{
		return;
	}

	OBJECTMGR->SetSelectedObject(
		targetObject);

	targetObject->ShowObjectName(
		TRUE,
		RGB_HALF(255, 141, 39));

	// 타겟을 쳐다보는 코드. 죽을 때나 이동 중에 쓰면 당연히 어색하다
	if(FALSE == heroObject->IsDied() &&
		FALSE == heroObject->GetBaseMoveInfo()->bMoving)
	{
		VECTOR3 targetObjectPosition = {0};
		targetObject->GetPosition(
			&targetObjectPosition);

		VECTOR3 heroPosition = {0};
		heroObject->GetPosition(
			&heroPosition);

		VECTOR3 position = targetObjectPosition - heroPosition;
		heroObject->GetEngineObject()->SetDirection(
			&position);
	}
}

void CMacroManager::PM_Toggle_FriendDlg()
{
	cDialog* dialog = WINDOWMGR->GetWindowForID( FRI_FRIENDDLG );
	ASSERT( dialog );

	if( dialog->IsActive() )
	{
		dialog->SetActive( FALSE );
	}
	else
	{
		FRIENDMGR->FriendListSyn( 1 );
	}
}


void CMacroManager::PM_Toggle_HelpDlg()
{
	cDialog* pDlg = WINDOWMGR->GetWindowForID(HELP_DLG) ;
	ASSERT( pDlg ) ;

	pDlg->SetActive( !pDlg->IsActive() ) ;
}



void CMacroManager::PM_Toggle_GTResultDlg()
{
#ifdef _GMTOOL_
	CGTScoreInfoDialog* pScoreDlg = (CGTScoreInfoDialog*)WINDOWMGR->GetWindowForID(GTSCORETIMEDLG) ;
	if(!pScoreDlg)		return;

	CGTResultDlg* pResultDlg = (CGTResultDlg*)WINDOWMGR->GetWindowForID(GTRESULTDLG) ;
	if(!pResultDlg)		return;

	if(pScoreDlg->IsBattle())
	{
		if(pResultDlg->IsActive())
			pResultDlg->SetActive(FALSE);
		else
			pResultDlg->SetResult();
	}
#endif
}


// 080429 LYW --- MacroManager : Add functions to toggle chatroom part.
void CMacroManager::PM_Toggle_ChatRoomListDlg()
{
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRMD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crmd!!", __FUNCTION__) ;
		return ;
	}

	//pDlg->SetActive(!pDlg->IsActive()) ;
	if(pDlg->IsActive())
	{
		pDlg->SetActive(FALSE) ;
	}
	else
	{
		CHATROOMMGR->ToggleChatRoomMainDlg() ;
	}
}





void CMacroManager::PM_Toggle_ChatRoomDlg()
{
	if(!CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1498)) ;
		return ;
	}

	// 채팅방 다이얼로그.
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	pDlg->SetActive(!pDlg->IsActive()) ;

	// 참여자 창.
	cDialog* pGuestDlg = NULL ;
	pGuestDlg = WINDOWMGR->GetWindowForID(CRGLD_DLG) ;

	ASSERT(pGuestDlg) ;

	if(!pGuestDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	if(pGuestDlg->IsActive()) pGuestDlg->SetActive(FALSE) ;
	//pGuestDlg->SetActive(!pDlg->IsActive()) ;

	// 옵션 창.
	cDialog* pOptionDlg = NULL ;
	pOptionDlg = WINDOWMGR->GetWindowForID(CROD_DLG) ;

	ASSERT(pOptionDlg) ;

	if(!pOptionDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	if(pOptionDlg->IsActive()) pOptionDlg->SetActive(FALSE) ;
	//pOptionDlg->SetActive(!pDlg->IsActive()) ;
}





void CMacroManager::PM_SetFocus_ChatRoom_Chat()
{
	// 채팅방 참여중인지 체크.
	if(!CHATROOMMGR->GetMyRoomOwnerIdx())
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1498)) ;
		return ;
	}



	// 채팅방 다이얼로그.
	cDialog* pDlg = NULL ;
	pDlg = WINDOWMGR->GetWindowForID(CRD_DLG) ;

	ASSERT(pDlg) ;

	if(!pDlg)
	{
		CHATROOMMGR->Throw_Error("Failed to receive crd!!", __FUNCTION__) ;
		return ;
	}

	// 채팅방이 비활성화 되어있다면, return 처리를 한다.
	if(!pDlg->IsActive()) return ;



	// 채팅방의 에디트박스 받기.
	cEditBox* roomBox = NULL ;
	roomBox = CHATROOMMGR->GetChatRoomInputBox() ;

	ASSERT(roomBox) ;

	if(!roomBox)
	{
		CHATROOMMGR->Throw_Error("Failed to receive creb!!", __FUNCTION__) ;
		return ;
	}

	cEditBox* chatBox = GAMEIN->GetChattingDlg()->GetInputBox();

	if(roomBox->IsFocus())
	{
		roomBox->SetFocus(FALSE) ;
		roomBox->SetFocusEdit(FALSE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), FALSE) ;
		chatBox->SetFocus(TRUE) ;
		chatBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(chatBox->GetIME(), TRUE) ;
	}
	else if(chatBox->IsFocus())
	{
		chatBox->SetFocus(FALSE) ;
		chatBox->SetFocusEdit(FALSE) ;
		CIMEWND->SetDockingEx(chatBox->GetIME(), FALSE) ;
		roomBox->SetFocus(TRUE) ;
		roomBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), TRUE) ;
	}
	else
	{
		roomBox->SetFocus(TRUE) ;
		roomBox->SetFocusEdit(TRUE) ;
		CIMEWND->SetDockingEx(roomBox->GetIME(), TRUE) ;
	}
}


void CMacroManager::PM_Toggle_Pet_Info()
{
	if( !HEROPET )
		return;

	CPetInfoDialog* pDlg = GAMEIN->GetPetInfoDlg();

	if( !pDlg )
	{
		return;
	}

	if( pDlg->IsActive() )
	{
		pDlg->SetActive( FALSE );
	}
	else
	{
		pDlg->SetActive( TRUE );
	}
}

// 090422 ShinJS --- 탈것 탑승/하차
void CMacroManager::PM_Toggle_Vehicle_GetOnOff()
{
	// 091214 ShinJS --- 탑승 여부 구분
	// 탑승 상태
	if( HERO->IsGetOnVehicle() )
	{
		// 하차
		// 090316 LUJ, 서버에 요청한다
		MSGBASE message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_DISMOUNT_SYN;
		message.dwObjectID = HEROID;
		NETWORK->Send( &message, sizeof( message ) );
	}
	// 미탑승 상태
	else
	{
		// 탑승
		// 탈것 소유 여부 확인
		const DWORD dwVehicleID = HERO->GetVehicleID();
		CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( dwVehicleID );
		if( !pVehicle || 
			pVehicle->GetObjectKind() != eObjectKind_Vehicle ||
			pVehicle->GetOwnerIndex() != gHeroID)
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1829 ) );
			return;
		}

		DWORD dwEmptySeatPos = pVehicle->CanGetOn( HERO );

		// 탑승 불가능한 경우
		if( dwEmptySeatPos == 0 )
		{
			return;
		}

		MSG_DWORD message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category = MP_VEHICLE;
		message.Protocol = MP_VEHICLE_MOUNT_ASK_SYN;
		message.dwObjectID = HEROID;
		message.dwData = pVehicle->GetID();
		NETWORK->Send( &message, sizeof( message ) );
	}
}

void CMacroManager::PlayMacroPress( int nMacroEvent )
{
	float Rate = 1.f;																// 회전 값.

	if( g_UserInput.GetKeyboard()->GetKeyPressed(KEY_RSHIFT) )						// 오른쪽 시프트가 눌렸다면,
	{
		Rate = 0.3f;																// 회전 값을 0.3f로 세팅한다.
	}

	switch( nMacroEvent )															// 인자로 들어온 매크로 이벤트를 확인한다.
	{
		case MPE_CAMERAMOVE_ZOOMIN:													// 줌인 매크로라면,
			{

#ifdef _GMTOOL_																		// GM툴일 때 처리를 시작한다.
				if( CAMERA->GetCameraMode() == eCM_EyeView )						// 카메라 모드가 eCM_EyeView와 같다면,
				{
					CAMERA->Fly( 0.8f * gTickTime*Rate*5 );							// 케미라의 fly 함수를 실행한다.				
				}
				else
#endif
					CAMERA->ZoomDistance( 0, -0.8f * gTickTime*Rate, 0 );
			}
			break;

		case MPE_CAMERAMOVE_ZOOMOUT:												// 줌 아웃 매크로라면,
			{

#ifdef _GMTOOL_																		// GM툴 처리를 시작한다.

				if( CAMERA->GetCameraMode() == eCM_EyeView )						// 카메라 모드가 eCM_EyeView와 같다면,
				{
					CAMERA->Fly( -0.8f * gTickTime*Rate*5 );							// 주어진 값으로 카메라의 Fly 함수를 실행한다.
				}
				else
#endif
					CAMERA->ZoomDistance( 0, 0.8f * gTickTime*Rate, 0 );				// 주어진 식으로 카메라 줌 거리를 조정한다.
			}
			break;

		case MPE_CAMERAMOVE_UP:														// 카메라 업 매크로라면,
			CAMERA->MouseRotate(
				0,
				int(0.3f * gTickTime * Rate));
			break;

		case MPE_CAMERAMOVE_DOWN:													// 카메라 다운 매크로라면,
			CAMERA->MouseRotate(
				0,
				int(-0.3f * gTickTime*Rate));
			break;

		case MPE_CAMERAMOVE_RIGHT:													// 카메라 라이트 매크로라면,
			CAMERA->MouseRotate(
				int(-0.3f * gTickTime*Rate),
				0);
			break;

		case MPE_CAMERAMOVE_LEFT:													// 카메라 레프트 매크로라면,
			CAMERA->MouseRotate(
				int(0.3f * gTickTime*Rate),
				0);
			break;

	}
}

void CMacroManager::ForceSetFocus()
{
	// 웹브라우저 등으로 _g_hWnd가 포커스를 되돌려 받지 못하는 경우
	// 다시 세팅해준다.
	extern HWND _g_hWnd;
	SetFocus(_g_hWnd);
}

void CMacroManager::Init()
{

	sMACRO DefaultMacro[MM_COUNT][ME_COUNT] = {
		{
			{ MSK_NONE,		KEY_F1,	FALSE, FALSE },									// 메인 퀵슬롯 1번키 사용
			{ MSK_NONE,		KEY_F2,	FALSE, FALSE },									// 메인 퀵슬롯 2번키 사용
			{ MSK_NONE,		KEY_F3,	FALSE, FALSE },									// 메인 퀵슬롯 3번키 사용
			{ MSK_NONE,		KEY_F4,	FALSE, FALSE },									// 메인 퀵슬롯 4번키 사용
			{ MSK_NONE,		KEY_F5,	FALSE, FALSE },									// 메인 퀵슬롯 5번키 사용
			{ MSK_NONE,		KEY_F6,	FALSE, FALSE },									// 메인 퀵슬롯 6번키 사용
			{ MSK_NONE,		KEY_F7,	FALSE, FALSE },									// 메인 퀵슬롯 7번키 사용
			{ MSK_NONE,		KEY_F8,	FALSE, FALSE },									// 메인 퀵슬롯 8번키 사용
			{ MSK_NONE,		KEY_F9,	FALSE, FALSE },									// 메인 퀵슬롯 9번키 사용
			{ MSK_NONE,		KEY_F10,FALSE, FALSE },									// 메인 퀵슬롯10번키 사용

			{ MSK_CTRL,		KEY_F1,	FALSE, FALSE },									// 서브 퀵슬롯 1번키 사용 
			{ MSK_CTRL,		KEY_F2,	FALSE, FALSE },									// 서브 퀵슬롯 2번키 사용 
			{ MSK_CTRL,		KEY_F3,	FALSE, FALSE },									// 서브 퀵슬롯 3번키 사용 
			{ MSK_CTRL,		KEY_F4,	FALSE, FALSE },									// 서브 퀵슬롯 4번키 사용 
			{ MSK_CTRL,		KEY_F5,	FALSE, FALSE },									// 서브 퀵슬롯 5번키 사용 
			{ MSK_CTRL,		KEY_F6,	FALSE, FALSE },									// 서브 퀵슬롯 6번키 사용 
			{ MSK_CTRL,		KEY_F7,	FALSE, FALSE },									// 서브 퀵슬롯 7번키 사용 
			{ MSK_CTRL,		KEY_F8,	FALSE, FALSE },									// 서브 퀵슬롯 8번키 사용 
			{ MSK_CTRL,		KEY_F9,	FALSE, FALSE },									// 서브 퀵슬롯 9번키 사용 
			{ MSK_CTRL,		KEY_F10,FALSE, FALSE },									// 서브 퀵슬롯10번키 사용 

			{ MSK_ALT,		KEY_1,	FALSE, FALSE },									// 메인 퀵슬롯을 1번으로 세팅함.
			{ MSK_ALT,		KEY_2,	FALSE, FALSE },									// 메인 퀵슬롯을 2번으로 세팅함.
			{ MSK_ALT,		KEY_3,	FALSE, FALSE },									// 메인 퀵슬롯을 3번으로 세팅함.
			{ MSK_ALT,		KEY_4,	FALSE, FALSE },									// 메인 퀵슬롯을 4번으로 세팅함.
			{ MSK_ALT,		KEY_5,	FALSE, FALSE },									// 메인 퀵슬롯을 5번으로 세팅함.
			{ MSK_ALT,		KEY_6,	FALSE, FALSE },									// 메인 퀵슬롯을 6번으로 세팅함.
			{ MSK_ALT,		KEY_7,	FALSE, FALSE },									// 메인 퀵슬롯을 7번으로 세팅함.
			{ MSK_ALT,		KEY_8,	FALSE, FALSE },									// 메인 퀵슬롯을 8번으로 세팅함.

			{ MSK_ALT,		KEY_OEM_3, FALSE, FALSE },								// 메인 퀵슬롯 번호를 증가 시킨다.
			{ MSK_CTRL,		KEY_OEM_3, FALSE, FALSE },								// 메인 퀵슬롯 번호를 감소 시킨다.

			{ MSK_ALT | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// 서브 퀵슬롯 번호를 증가 시킨다.
			{ MSK_CTRL | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// 서브 퀵슬롯 번호를 감소 시킨다.

			{ MSK_ALT,		KEY_F1,	FALSE, FALSE },									// 내 자신을 선택한다.

			{ MSK_ALT,		KEY_F2,	FALSE, FALSE },									// 첫 번째 파티 멤버를 선택한다.
			{ MSK_ALT,		KEY_F3,	FALSE, FALSE },									// 두 번째 파티 멤버를 선택한다.
			{ MSK_ALT,		KEY_F4,	FALSE, FALSE },									// 세 번째 파티 멤버를 선택한다.
			{ MSK_ALT,		KEY_F5,	FALSE, FALSE },									// 네 번째 파티 멤버를 선택한다.
			{ MSK_ALT,		KEY_F6,	FALSE, FALSE },									// 다섯 번째 파티 멤버를 선택한다.
			{ MSK_ALT,		KEY_F7,	FALSE, FALSE },									// 여섯 번째 파티 멤버를 선택한다.

			{ MSK_NONE,		KEY_TAB,	FALSE, FALSE },								// 가장 가까이에 있는 타겟을 선택한다.

			{ MSK_ALT,		KEY_F12,	FALSE, FALSE },								// 스크린을 캡쳐한다.

			{ MSK_NONE,		KEY_ESCAPE,	FALSE, FALSE },								// 종료 메시지 창을 띄운다.

			{ MSK_CTRL,		KEY_Z,	FALSE, FALSE },									// 모든 인터페이스를 토글한다.

			{ MSK_ALT,		KEY_S,	FALSE, FALSE },									// 스킬창을 토글한다.

			{ MSK_ALT,		KEY_F,	FALSE, FALSE },									// 패밀리 창을 토글한다.

			{ MSK_ALT,		KEY_I,	FALSE, FALSE },									// 인벤토리를 토글한다.

			{ MSK_ALT,		KEY_H,	FALSE, FALSE },									// 도움말을 토글한다.

			{ MSK_ALT,		KEY_M,	FALSE, FALSE },									// 미니맵을 토글한다.

			{ MSK_ALT,		KEY_W,	FALSE, FALSE },									// 빅맵을 토글한다.

			{ MSK_ALT,		KEY_O,	FALSE, FALSE },									// 옵션 창을 토글한다.

			{ MSK_ALT,		KEY_G,	FALSE, FALSE },									// 길드 창을 토글한다.

			{ MSK_ALT,		KEY_Q,	FALSE, FALSE },									// 퀘스트 창을 토글한다.

			{ MSK_ALT,		KEY_C,	FALSE, FALSE },									// 캐릭터 정보창을 토글한다.

			{ MSK_ALT,		KEY_R,	FALSE, FALSE },									// 걷기/뛰기 모드를 전환한다.

			{ MSK_ALT,		KEY_INSERT,	FALSE, FALSE },								// 카메라 뷰 모드를 전환한다.

			{ MSK_ALT,		KEY_D,	FALSE, FALSE },									// 매칭 창을 토글한다.
			{ MSK_ALT,		KEY_B,	FALSE, FALSE },									// 친구 창을 토글한다.

			// 071025 LYW --- MacroManager : Add toggle key for help dialog.
			{ MSK_ALT,		KEY_T,	FALSE, FALSE },									// 도움말 창을 토글한다.
			{ MSK_CTRL,		KEY_CONTROL,	FALSE, FALSE },							// 낚시 낚기버튼
			{ MSK_NONE,		KEY_SPACE,		FALSE, FALSE },							// 길드토너먼트 결과창 토글

			// 080429 LYW --- MacroManager : Add toggle key for chatroom system.
			{ MSK_NONE,		KEY_HOME, FALSE, FALSE },								// 채팅방 리스트창을 토글한다.
			{ MSK_CTRL,		KEY_HOME, FALSE, FALSE },								// 채팅방을 토글한다.
			{ MSK_CTRL,		KEY_END,  TRUE, FALSE },								// 채팅방과 채팅창 사이의 포커스를 이동시킨다.

			{ MSK_ALT,		KEY_P,	FALSE, FALSE },									// 펫정보창
			{ MSK_ALT,		KEY_U, FALSE, FALSE },									// 090422 ShinJS --- 탈것 탑승 단축키
			{ MSK_NONE,		KEY_Z, FALSE, FALSE },									// 100105 ONS 앉기/일어서기를 토글한다.
		},
		{			
			{ MSK_NONE,		KEY_1,	FALSE, FALSE },									// 메인 퀵슬롯 1번키 사용
			{ MSK_NONE,		KEY_2,	FALSE, FALSE },									// 메인 퀵슬롯 2번키 사용
			{ MSK_NONE,		KEY_3,	FALSE, FALSE },									// 메인 퀵슬롯 3번키 사용
			{ MSK_NONE,		KEY_4,	FALSE, FALSE },									// 메인 퀵슬롯 4번키 사용
			{ MSK_NONE,		KEY_5,	FALSE, FALSE },									// 메인 퀵슬롯 5번키 사용
			{ MSK_NONE,		KEY_6,	FALSE, FALSE },									// 메인 퀵슬롯 6번키 사용
			{ MSK_NONE,		KEY_7,	FALSE, FALSE },									// 메인 퀵슬롯 7번키 사용
			{ MSK_NONE,		KEY_8,	FALSE, FALSE },									// 메인 퀵슬롯 8번키 사용
			{ MSK_NONE,		KEY_9,	FALSE, FALSE },									// 메인 퀵슬롯 9번키 사용
			{ MSK_NONE,		KEY_0, FALSE, FALSE },									// 메인 퀵슬롯10번키 사용

			{ MSK_CTRL,		KEY_1,	FALSE, FALSE },									// 서브 퀵슬롯 1번키 사용 
			{ MSK_CTRL,		KEY_2,	FALSE, FALSE },									// 서브 퀵슬롯 2번키 사용 
			{ MSK_CTRL,		KEY_3,	FALSE, FALSE },									// 서브 퀵슬롯 3번키 사용 
			{ MSK_CTRL,		KEY_4,	FALSE, FALSE },									// 서브 퀵슬롯 4번키 사용 
			{ MSK_CTRL,		KEY_5,	FALSE, FALSE },									// 서브 퀵슬롯 5번키 사용 
			{ MSK_CTRL,		KEY_6,	FALSE, FALSE },									// 서브 퀵슬롯 6번키 사용 
			{ MSK_CTRL,		KEY_7,	FALSE, FALSE },									// 서브 퀵슬롯 7번키 사용 
			{ MSK_CTRL,		KEY_8,	FALSE, FALSE },									// 서브 퀵슬롯 8번키 사용 
			{ MSK_CTRL,		KEY_9,	FALSE, FALSE },									// 서브 퀵슬롯 9번키 사용 
			{ MSK_CTRL,		KEY_0, FALSE, FALSE },									// 서브 퀵슬롯10번키 사용 

			{ MSK_ALT,		KEY_1,	FALSE, FALSE },									// 메인 퀵슬롯을 1번으로 세팅함.
			{ MSK_ALT,		KEY_2,	FALSE, FALSE },									// 메인 퀵슬롯을 2번으로 세팅함.
			{ MSK_ALT,		KEY_3,	FALSE, FALSE },									// 메인 퀵슬롯을 3번으로 세팅함.
			{ MSK_ALT,		KEY_4,	FALSE, FALSE },									// 메인 퀵슬롯을 4번으로 세팅함.
			{ MSK_ALT,		KEY_5,	FALSE, FALSE },									// 메인 퀵슬롯을 5번으로 세팅함.
			{ MSK_ALT,		KEY_6,	FALSE, FALSE },									// 메인 퀵슬롯을 6번으로 세팅함.
			{ MSK_ALT,		KEY_7,	FALSE, FALSE },									// 메인 퀵슬롯을 7번으로 세팅함.
			{ MSK_ALT,		KEY_8,	FALSE, FALSE },									// 메인 퀵슬롯을 8번으로 세팅함.

			{ MSK_NONE,		KEY_OEM_3, FALSE, FALSE },								// 메인 퀵슬롯 번호를 증가 시킨다.
			{ MSK_CTRL,		KEY_OEM_3, FALSE, FALSE },								// 메인 퀵슬롯 번호를 감소 시킨다.

			{ MSK_NONE | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// 서브 퀵슬롯 번호를 증가 시킨다.
			{ MSK_CTRL | MSK_SHIFT,	KEY_OEM_3, FALSE, FALSE },						// 서브 퀵슬롯 번호를 감소 시킨다.

			{ MSK_NONE,		KEY_F1,	FALSE, FALSE },									// 내 자신을 선택한다.

			{ MSK_NONE,		KEY_F2,	FALSE, FALSE },									// 첫 번째 파티 멤버를 선택한다.
			{ MSK_NONE,		KEY_F3,	FALSE, FALSE },									// 두 번째 파티 멤버를 선택한다.
			{ MSK_NONE,		KEY_F4,	FALSE, FALSE },									// 세 번째 파티 멤버를 선택한다.
			{ MSK_NONE,		KEY_F5,	FALSE, FALSE },									// 네 번째 파티 멤버를 선택한다.
			{ MSK_NONE,		KEY_F6,	FALSE, FALSE },									// 다섯 번째 파티 멤버를 선택한다.
			{ MSK_NONE,		KEY_F7,	FALSE, FALSE },									// 여섯 번째 파티 멤버를 선택한다.

			{ MSK_NONE,		KEY_TAB,	FALSE, FALSE },								// 가장 가까이에 있는 타겟을 선택한다.

			{ MSK_NONE,		KEY_F12,	FALSE, FALSE },								// 스크린을 캡쳐한다.

			{ MSK_NONE,		KEY_ESCAPE,	FALSE, FALSE },								// 종료 메시지 창을 띄운다.

			{ MSK_CTRL,		KEY_Z,	FALSE, FALSE },									// 모든 인터페이스를 토글한다.

			{ MSK_NONE,		KEY_S,	FALSE, FALSE },									// 스킬창을 토글한다.

			{ MSK_NONE,		KEY_F,	FALSE, FALSE },									// 패밀리 창을 토글한다.

			{ MSK_NONE,		KEY_I,	FALSE, FALSE },									// 인벤토리를 토글한다.

			{ MSK_NONE,		KEY_H,	FALSE, FALSE },									// 도움말을 토글한다.

			{ MSK_NONE,		KEY_M,	FALSE, FALSE },									// 미니맵을 토글한다.

			{ MSK_NONE,		KEY_W,	FALSE, FALSE },									// 빅맵을 토글한다.

			{ MSK_NONE,		KEY_O,	FALSE, FALSE },									// 옵션 창을 토글한다.

			{ MSK_NONE,		KEY_G,	FALSE, FALSE },									// 길드 창을 토글한다.

			{ MSK_NONE,		KEY_Q,	FALSE, FALSE },									// 퀘스트 창을 토글한다.

			{ MSK_NONE,		KEY_C,	FALSE, FALSE },									// 캐릭터 정보창을 토글한다.

			// 100216 ONS 걷기/뛰기 Alt+R로 변경
			{ MSK_ALT,		KEY_R,	FALSE, FALSE },									// 걷기/뛰기 모드를 전환한다.

			{ MSK_NONE,		KEY_INSERT,	FALSE, FALSE },								// 카메라 뷰 모드를 전환한다.

			{ MSK_NONE,		KEY_D,	FALSE, FALSE },									// 매칭 창을 토글한다.													
			{ MSK_NONE,		KEY_B,	FALSE, FALSE },									// 친구 창을 토글한다.

			// 071025 LYW --- MacroManager : Add toggle key for help dialog.
			{ MSK_NONE,		KEY_T,	FALSE, FALSE },									// 도움말 창을 토글한다.

			{ MSK_CTRL,		KEY_CONTROL,	FALSE, FALSE },							// 낚시 낚기버튼
			{ MSK_NONE,		KEY_SPACE,		FALSE, FALSE },							// 길드토너먼트 결과창 토글
			

			// 080429 LYW --- MacroManager : Add toggle key for chatroom system.
			{ MSK_NONE,		KEY_HOME, FALSE, FALSE },								// 채팅방 리스트창을 토글한다.
			{ MSK_CTRL,		KEY_HOME, FALSE, FALSE },								// 채팅방을 토글한다.
			{ MSK_CTRL,		KEY_END,  TRUE, FALSE },								// 채팅방과 채팅창 사이의 포커스를 이동시킨다.
			
			{ MSK_NONE,		KEY_P,	FALSE, FALSE },									// 펫정보창
			{ MSK_NONE,		KEY_U, FALSE, FALSE },									// 090422 ShinJS --- 탈것 탑승 단축키
			{ MSK_NONE,		KEY_Z, FALSE, FALSE },									// 100105 ONS 앉기/일어서기를 토글한다.
		}
	};

	sMACRO DefaultPressMacro[MM_COUNT][MPE_COUNT] = {
		{
			{ MSK_NONE,		KEY_PGUP,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMIN
			{ MSK_NONE,		KEY_PGDN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMOUT

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_CAMERAMOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_LEFT
		},
		{
			{ MSK_NONE,		KEY_PGUP,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMIN
			{ MSK_NONE,		KEY_PGDN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_ZOOMOUT

			{ MSK_NONE,		KEY_UP,		FALSE, FALSE },								//	MPE_CAMERAMOVE_UP
			{ MSK_NONE,		KEY_DOWN,	FALSE, FALSE },								//	MPE_CAMERAMOVE_DOWN
			{ MSK_NONE,		KEY_RIGHT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_RIGHT
			{ MSK_NONE,		KEY_LEFT,	FALSE, FALSE },								//	MPE_CAMERAMOVE_LEFT			
		}
	};

	memcpy( m_DefaultKey, DefaultMacro, sizeof( DefaultMacro ) );
	memcpy( m_DefaultPressKey, DefaultPressMacro, sizeof( DefaultPressMacro ) );

	LoadUserMacro();
}


void CMacroManager::SetDefaultMacro( int nMacroMode )
{
	memcpy( m_MacroKey[nMacroMode], m_DefaultKey[nMacroMode], sizeof( sMACRO ) * ME_COUNT );

	memcpy( m_MacroPressKey[nMacroMode], m_DefaultPressKey[nMacroMode], sizeof( sMACRO ) * MPE_COUNT );
}


int CMacroManager::IsUsableKey( CKeyboard* keyInfo, WORD* wKey, BOOL bUserMode )
{
	static WORD wUsableSysKey[] =
	{
		KEY_CONTROL,
		KEY_MENU,
		KEY_SHIFT
	};

	static WORD wUsableCombineKey[] =
	{
		KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
		KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
		KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
		KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

		KEY_EQUALS, KEY_SEMICOLON, KEY_OEM_3, KEY_SLASH, KEY_LBRACKET, KEY_RBRACKET, KEY_BACKSLASH,
		KEY_APOSTROPHE, KEY_MINUS, KEY_EQUALS, /*KEY_OEM_102,*/ KEY_COMMA, KEY_PERIOD, 

		KEY_SPACE, KEY_BACK, KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN,
		KEY_TAB, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,

		KEY_PAD0, KEY_PAD1, KEY_PAD2, KEY_PAD3, KEY_PAD4, KEY_PAD5, KEY_PAD6, KEY_PAD7, KEY_PAD8, KEY_PAD9,
		KEY_MULTIPLY, KEY_ADD, KEY_SUBTRACT, KEY_DECIMAL, KEY_DIVIDE
	};
	
	static WORD wUsableKey[] =
	{
		KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
		KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

		KEY_SPACE, KEY_BACK, KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PGUP, KEY_PGDN,
		KEY_TAB, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	};

	static BOOL bSysKeyDown = FALSE;
	int i;

	if( ( keyInfo->GetKeyDown( KEY_TAB ) || keyInfo->GetKeyDown( KEY_F4 ) )
		  && keyInfo->GetKeyPressed( KEY_MENU ) )
	{
		return 0;
	}


	if( bSysKeyDown || bUserMode == 1 )
	{
		for( i = 0 ; i < sizeof( wUsableCombineKey ) / sizeof( WORD ) ; ++i )
		{
			if( keyInfo->GetKeyDown( wUsableCombineKey[i] ) )
			{
				*wKey = wUsableCombineKey[i];
				return 1;
			}
		}
	}
	else
	{
		for( i = 0 ; i < sizeof( wUsableKey ) / sizeof( WORD ) ; ++i )
		{
			if( keyInfo->GetKeyDown( wUsableKey[i] ) )
			{
				*wKey = wUsableKey[i];
				return 1;
			}
		}
	}

	for( i = 0 ; i < sizeof( wUsableSysKey ) / sizeof( WORD ) ; ++i )
	{
		if( keyInfo->GetKeyPressed( wUsableSysKey[i] ) )
		{
			*wKey = wUsableSysKey[i];
			bSysKeyDown = TRUE;
			return 2;
		}
	}

	*wKey = 0x0000;
	bSysKeyDown = FALSE;
	return 0;
}

void CMacroManager::CheckToggleState( CObject* player )
{
	if( !	player			||
			player != HERO	||
			WINDOWMGR->IsOpendAllWindows() )
	{
		return;
	}

	// 071204 LUJ	스크린샷 촬영위해 인터페이스가 토글 중인데 이동을 시도할 경우
	//				정지 상태에서만 이용 가능한 인터페이스를 닫아야 한다

	// 거래창
	struct
	{
		void operator() ( cDialog* dialog )
		{
			cPtrList& list = WINDOWMGR->m_pActivedWindowList;

			if( ! dialog)
			{
				return;
			}

			void* key = ( void* )( dialog->GetID() );

			PTRLISTPOS	position = list.Find( key );

			list.RemoveAt( position );
		}
	}
	RemoveSavedWindow;

	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStorageDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetGuildWarehouseDlg() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetExchangeDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetNpcScriptDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStreetBuyStallDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetStreetStallDialog() );
	RemoveSavedWindow( ( cDialog* )GAMEIN->GetDealDialog() );
}
