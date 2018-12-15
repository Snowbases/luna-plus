#pragma once


#define MACROMGR USINGTON(CMacroManager)

class CKeyboard;
class CObject;

//---------------------------------------------------------------------------------------
// taiyo 
// 현재는 임시로 단축키를 셋팅했지만,
// 단축키 모듈을 만들어서 추가할 예정!
//---------------------------------------------------------------------------------------
//KES_030806
// 단축키 모듈
//---------------------------------------------------------------------------------------


////////////
//단축키목록
//#define START_CONFLICT_CHAT		ME_USE_QUICKITEM01	//채팅과 충돌되는 단축키.

enum eMacroEvent {	//다이얼로그와 순서를 맞추어야 한다.

	ME_USE_QUICKITEM_1_01,										// 메인 퀵슬롯 1번키 사용
	ME_USE_QUICKITEM_1_02, 										// 메인 퀵슬롯 2번키 사용
	ME_USE_QUICKITEM_1_03, 										// 메인 퀵슬롯 3번키 사용
	ME_USE_QUICKITEM_1_04, 										// 메인 퀵슬롯 4번키 사용
	ME_USE_QUICKITEM_1_05, 										// 메인 퀵슬롯 5번키 사용
	ME_USE_QUICKITEM_1_06, 										// 메인 퀵슬롯 6번키 사용
	ME_USE_QUICKITEM_1_07, 										// 메인 퀵슬롯 7번키 사용
	ME_USE_QUICKITEM_1_08, 										// 메인 퀵슬롯 8번키 사용
	ME_USE_QUICKITEM_1_09, 										// 메인 퀵슬롯 9번키 사용
	ME_USE_QUICKITEM_1_10, 										// 메인 퀵슬롯10번키 사용

	ME_USE_QUICKITEM_2_01,										// 서브 퀵슬롯 1번키 사용 
	ME_USE_QUICKITEM_2_02, 										// 서브 퀵슬롯 2번키 사용 
	ME_USE_QUICKITEM_2_03, 										// 서브 퀵슬롯 3번키 사용 
	ME_USE_QUICKITEM_2_04, 										// 서브 퀵슬롯 4번키 사용 
	ME_USE_QUICKITEM_2_05, 										// 서브 퀵슬롯 5번키 사용 
	ME_USE_QUICKITEM_2_06, 										// 서브 퀵슬롯 6번키 사용 
	ME_USE_QUICKITEM_2_07, 										// 서브 퀵슬롯 7번키 사용 
	ME_USE_QUICKITEM_2_08, 										// 서브 퀵슬롯 8번키 사용 
	ME_USE_QUICKITEM_2_09, 										// 서브 퀵슬롯 9번키 사용 
	ME_USE_QUICKITEM_2_10, 										// 서브 퀵슬롯10번키 사용 

	ME_CHANGE_QUICKSLOT_01,										// 메인 퀵슬롯을 1번으로 세팅함.
	ME_CHANGE_QUICKSLOT_02,										// 메인 퀵슬롯을 2번으로 세팅함.
	ME_CHANGE_QUICKSLOT_03,										// 메인 퀵슬롯을 3번으로 세팅함.
	ME_CHANGE_QUICKSLOT_04,										// 메인 퀵슬롯을 4번으로 세팅함.
	ME_CHANGE_QUICKSLOT_05,										// 메인 퀵슬롯을 5번으로 세팅함.
	ME_CHANGE_QUICKSLOT_06,										// 메인 퀵슬롯을 6번으로 세팅함.
	ME_CHANGE_QUICKSLOT_07,										// 메인 퀵슬롯을 7번으로 세팅함.
	ME_CHANGE_QUICKSLOT_08,										// 메인 퀵슬롯을 8번으로 세팅함.

	ME_INCREASE_MAIN_QUICKSLOT_NUM,								// 메인 퀵슬롯 번호를 증가 시킨다.
	ME_DECREASE_MAIN_QUICKSLOT_NUM,								// 메인 퀵슬롯 번호를 감소 시킨다.

	ME_INCREASE_SUB_QUICKSLOT_NUM,								// 서브 퀵슬롯 번호를 증가 시킨다.
	ME_DECREASE_SUB_QUICKSLOT_NUM,								// 서브 퀵슬롯 번호를 감소 시킨다.

	ME_SELECT_MYSELF,											// 내 자신을 선택한다.

	ME_SELECT_PARTYMEMBER_1,									// 첫 번째 파티 멤버를 선택한다.
	ME_SELECT_PARTYMEMBER_2,									// 두 번째 파티 멤버를 선택한다.
	ME_SELECT_PARTYMEMBER_3,									// 세 번째 파티 멤버를 선택한다.
	ME_SELECT_PARTYMEMBER_4,									// 네 번째 파티 멤버를 선택한다.
	ME_SELECT_PARTYMEMBER_5,									// 다섯 번째 파티 멤버를 선택한다.
	ME_SELECT_PARTYMEMBER_6,									// 여섯 번째 파티 멤버를 선택한다.

	ME_SELECT_NEAR_TARGET,										// 가장 가까이에 있는 타겟을 선택한다.

	ME_SCREENCAPTURE,											// 스크린을 캡쳐한다.

	ME_TOGGLE_EXITDLG,											// 종료 메시지 창을 띄운다.

	ME_TOGGLE_ALLINTERFACE,										// 모든 인터페이스를 토글한다.

	ME_TOGGLE_SKILLDLG,											// 스킬창을 토글한다.

	ME_TOGGLE_FAMILYDLG,										// 패밀리 창을 토글한다.

	ME_TOGGLE_INVENTORYDLG,										// 인벤토리를 토글한다.

	ME_TOGGLE_HELP_SHOW,										// 도움말을 토글한다.

	ME_TOGGLE_BIGMAP,											// 빅맵을 토글한다.

	ME_TOGGLE_WORLDMAP,											// 월드맵을 토글한다.

	ME_TOGGLE_OPTIONDLG,										// 옵션 창을 토글한다.

	ME_TOGGLE_GUILDDLG,											// 길드 창을 토글한다.

	ME_TOGGLE_QUESTDLG,											// 퀘스트 창을 토글한다.

	ME_TOGGLE_CHARINFODLG,										// 캐릭터 정보창을 토글한다.

	ME_WALK_RUN,												// 걷기/뛰기 모드를 전환한다.

	ME_TOGGLE_CAMERAVIEWMODE,									// 카메라 뷰 모드를 전환한다.

	ME_TOGGLE_MATCHINGDLG,										// 매칭 창을 토글한다.

	ME_TOGGLE_FRIENDDLG,										// 친구창

	// 071025 LYW --- MacroManager : Add macro for help dialog.
	ME_TOGGLE_HELPDLG,											// 도움말 창.
	
	ME_FISHING_PULLINGBTN,										// 낚시 낚기버튼
	ME_TOGGLE_GTRESULT_DLG,										// 길드토너먼트 결과창 토글

	// 080429 LYW --- MacroManager : Add macro for chatroom system.
	ME_TOGGLE_CHATROOM_LISTDLG,									// 채팅방 리스트 다이얼로그 토글.
	ME_TOGGLE_CHATROOM_ROOMDLG,									// 채팅방 토글.
	ME_SETFOCUS_CHATROOM_CHATTING,								// 채팅창과 채팅방 에디트 박스 사이에 포커스를 이동시켜주는 단축키.

	ME_PET_INFO,
	ME_VEHICLE_GETONOFF,										// 090422 ShinJS --- 탈것 탑승 단축키
	ME_TOGGLE_RESTMODE,											// 100105 ONS 앉기/일어서기를 토글한다.
	ME_COUNT,					//매크로 총 개수
};

enum eMacroPressEvent
{
	MPE_CAMERAMOVE_ZOOMIN,
	MPE_CAMERAMOVE_ZOOMOUT,
	
	MPE_CAMERAMOVE_UP,
	MPE_CAMERAMOVE_DOWN,
	MPE_CAMERAMOVE_RIGHT,
	MPE_CAMERAMOVE_LEFT,

	// 070123 LYW --- Add macro event.
	MPE_VIEW_FRONTYARD, 
	MPE_VIEW_BACKYARD,
	
	MPE_COUNT,
};

////////
//조합키
enum eSysKey {

	MSK_NONE	= 1,	//
	MSK_CTRL	= 2,
	MSK_ALT		= 4,
	MSK_SHIFT	= 8,
	MSK_ALL		= MSK_NONE | MSK_CTRL | MSK_ALT | MSK_SHIFT,
};

//////////////
//매크로구조체
struct sMACRO
{
	int		nSysKey;	//조합키
	WORD	wKey;		//실행키
	BOOL	bAllMode;	//채팅모드에 관계없이 되어야하는 것체크
	BOOL	bUp;		//up에 이벤트가 발생해야 할것
};


enum eMacroMode 
{
	MM_CHAT,					//채팅 우선모드
	MM_MACRO,					//단축키 우선모드

	MM_COUNT,
};




class CMacroManager
{

protected:

	sMACRO		m_DefaultKey[MM_COUNT][ME_COUNT];	//디폴트매크로키
	sMACRO		m_MacroKey[MM_COUNT][ME_COUNT];		//유저매크로키

	sMACRO		m_DefaultPressKey[MM_COUNT][MPE_COUNT];		//디폴트매크로프레스키
	sMACRO		m_MacroPressKey[MM_COUNT][MPE_COUNT];		//유저매크로프레스키

	int			m_nMacroMode;					//선택된 매크로 모드
//	int			m_nUserSetMode;					//유저세팅에서 선택된 모드
	BOOL		m_bPause;
	BOOL		m_bChatMode;
	BOOL		m_bInVisibleCursor;

	int			m_nCurTargetIdx ;
	DWORD		m_dwTargetID ;
	BOOL		mIsToggleInterface;

protected:
	BOOL LoadMacro( LPCTSTR strPath );
	BOOL SaveMacro( LPCTSTR strPath );
	void SetDefaultMacro( int nMacroMode );

public:
	CMacroManager();
	virtual ~CMacroManager();

//	//MAKESINGLETON(CMacroManager);
	BOOL IsVisibleCursor() { return !m_bInVisibleCursor; }

	void Init();
	void KeyboardInput( CKeyboard* keyInfo );

	void KeyboardInput_Normal( BOOL bDock, CKeyboard* keyInfo ) ;
	void KeyboardInput_GMTool( BOOL bDock, CKeyboard* keyInfo ) ;
	void KeyboardInput_CheatEnable( BOOL bDock, CKeyboard* keyInfo ) ;
	void KeyboardInput_Cheat_TestClient( BOOL bDock, CKeyboard* keyInfo ) ;
	void KeyboardInput_Cheat_NotTestClient( BOOL bDock, CKeyboard* keyInfo ) ;

	void KeyboardPressInput( CKeyboard* keyInfo );
	void LoadUserMacro();
	void SaveUserMacro();

	void GetMacro( int nMacroMode, sMACRO* pMacro );		//전체 매크로 얻어옴
	void SetMacro( int nMacroMode, sMACRO* pMacro );		//전테 매크로 세팅

	sMACRO* GetCurMacroKey( int nMacroEvent )	//특정 매크로키 하나 얻어옴
	{ 
		if( nMacroEvent >= ME_COUNT )
			return NULL;
		
		return &m_DefaultKey[m_nMacroMode][nMacroEvent];	//차후에 바꾸자.userset잊지마시오.
	}
	
	void GetDefaultMacro( int nMacroMode, sMACRO* pMacro );

	int GetMacroMode() { return m_nMacroMode; }
	void SetMacroMode( int nMacroMode );

//	int GetUserSetMode() { return m_nUserSetMode; }
//	void SetUserSetMode( int nUserSetMode ) { m_nUserSetMode = nUserSetMode; }

	void SetPause( BOOL bPause ) { m_bPause = bPause; }
	int IsUsableKey( CKeyboard* keyInfo, WORD* wKey, BOOL bUserMode );
	BOOL IsChatMode()	{ return m_bChatMode; }

	void CheckToggleState( CObject* );

	// 061203 LYW --- Add Function To Setting Value That Will Be TRUE.
	void SetChatMode( BOOL val ) { m_bChatMode = val ; }

	void PlayMacro( int nMacroEvent );
	void PM_Toggle_ExitDlg() ;
	void PM_ScreenCapture() ;
	void PM_UseMainQuickSlotItem( int nMacroEvent ) ;
	void PM_UseSubQuickSlotItem( int nMacroEvent ) ;
	void PM_Change_MainSlotNum( int nMacroEvent ) ;
	void PM_SetSlotPage( DWORD dwSlotID, int nMacroEvent ) ;
	void PM_SelectPartyMember( int nMacroEvent ) ;
	void PM_Toggle_AllInterface() ;
	void PM_Toggle_SkillDlg() ;
	void PM_Toggle_FamilyDlg() ;
	void PM_Toggle_InventoryDlg() ;
	void PM_Toggle_ShowKeyTip() ;
	void PM_Toggle_BigMap() ;
	void PM_Toggle_WorldMap() ;
	void PM_Toggle_OptionDlg() ;
	void PM_Toggle_GuildDlg() ;
	void PM_Toggle_QuestDlg() ;
	void PM_Toggle_CharInfoDlg() ;
	void PM_Toggle_MatchingDlg() ;
	void PM_SetAutoTarget() ;

	void PM_Toggle_FriendDlg() ;
	void PM_Toggle_HelpDlg() ;
	// 080429 LYW --- MacroManager : Add functions to toggle chatroom part.

	void PM_Toggle_GTResultDlg();

	void PM_Toggle_ChatRoomListDlg() ;
	void PM_Toggle_ChatRoomDlg() ;
	void PM_SetFocus_ChatRoom_Chat() ;

	void PM_Toggle_Pet_Info();
	// 090422 ShinJS --- 탈것 탑승/하차
	void PM_Toggle_Vehicle_GetOnOff();

	int GetCurTargetIdx() { return m_nCurTargetIdx ; }

	void PlayMacroPress( int nMacroEvent );

	void ForceSetFocus();
/////
};

EXTERNGLOBALTON(CMacroManager)