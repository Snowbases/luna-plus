#pragma once
//=================================================================================================
//	FILE		: CChatRoomMgr.h
//	DESC		: 채팅방 시스템을 관리하는 매니져 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: APRIL 3, 2008
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
#define CHATROOMMGR USINGTON(CChatRoomMgr) // 클래스 이름을 전역이름으로 만들고, 포인터 반환.

// 이 매크로를 사용하면, 메시지 박스로 에러를 출력한다.
#define _MB_CRMGR_

//#define SECRET_CODE_SIZE	4+1												// 비밀번호 사이즈 제한.

enum ENUM_PRINT_ERRORS														// 공용으로 사용되는 에러 메시지 정의.
{
	err_IMP = 0,															// 함수의 파라메터가 잘못되었다.
	err_ICCTLS,																// 하위 컨트롤의 정보가 바르지 않다.
	err_FCMTB,																// 기본 메시지로 변환에 실패하였다.
	err_FCMTO,																// 원본 메시지로 변환에 실패하였다.
} ;

enum ENUM_CHATROOM_MSGTYPE													// 채팅방에 출력 될 메시지 타입 정의.
{
	e_ChatMsg_StepIn = 0,													// 채팅방 입장.
	e_ChatMsg_StepOut,														// 채팅방 퇴장.
	e_ChatMsg_MyMsg,														// 나의 메시지.
	e_ChatMsg_OtherMsg,														// 다른 사람의 메시지.
	e_ChatMsg_System,														// 채팅방 내 시스템 메시지.
	e_ChatMsg_Whisper_Sener,												// 귓말 보낸사람.
	e_ChatMsg_Whisper_Receiver,												// 귓말 받는사람.
} ;

enum ENUM_CHATROOM_HERO_STATE												// HERO의 채팅 관련 상태 정의.
{
	e_State_Away_From_Chatroom = 0,											// 채팅을 할 준비가 되지 않은 상태.
	e_State_In_Lobby ,														// 서버에 HERO 정보 등록 된 상태.
	e_State_In_ChatRoom,													// 채팅방에 참여중인 상태.
} ;

enum ENUM_CR_GUEST_COUNT													// 채팅방 최대 참여수 정의.
{
	e_GC_20,																// 20명.
	e_GC_15,																// 15명.
	e_GC_10,																// 10명.
	e_GC_5,																	// 5명.

	e_GC_Max,
} ;

enum ENUM_CR_MAIN_TYPE
{
	e_RTM_AllLooK = 0,														// 전체보기.
	e_RTM_Party ,															// 파티모집.
	e_RTM_Deal,																// 물품거래.
	e_RTM_Normal_Chat,														// 일반채팅.
	e_RTM_Local_Chat,														// 지역채팅.
	e_RTM_Make_Friend,														// 친구찾기.
	e_RTM_Searched,															// 검색결과

	e_RTM_Max,
} ;

enum ENUM_CR_ROOM_MODE														// 채팅방 공개/비공개.
{	
	e_RM_Open = 0 ,															// 공개.
	e_RM_Close,																// 비공개.

	e_RM_Max,
} ;

enum ENUM_CR_GUEST_STATE													// 채팅방 참여자 상태.
{
	e_GS_Owner,																// 방장.
	e_GS_Guest,																// 손님.
	e_GS_Freezed,															// 차단.
} ;

enum ENUM_CR_ROOM_TYPE														// 채팅방 분류 정의.
{
	//e_RT_AllLooK = 0,														// 전채보기.
	e_RT_Party,																// 파티모집.
	e_RT_Deal,																// 물품거래.
	e_RT_Normal_Chat,														// 일반채팅.
	e_RT_Local_Chat,														// 지역채팅.
	e_RT_Make_Friend,														// 친구찾기.
	//e_RT_Make_Searched,														// 검색 된 페이지.

	e_RT_Max,
} ;





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
class CChatRoomCreateDlg ;
class CChatRoomDlg ;
class CChatRoomGuestListDlg ;
class CChatRoomJoinDlg ;
class CChatRoomMainDlg ;
class CChatRoomOptionDlg ;
//class cTextArea ;
class cEditBox ;






//-------------------------------------------------------------------------------------------------
//		The class CChatRoomMgr.
//-------------------------------------------------------------------------------------------------
class CChatRoomMgr
{
	BYTE						m_byChatting ;									// HERO가 채팅중인지 여부를 담을 변수.

	CChatRoomCreateDlg*			m_pCreateDlg ;									// 채팅방 생성창.
	CChatRoomDlg*				m_pChatRoomDlg ;								// 채팅창.
	CChatRoomGuestListDlg*		m_pGuestListDlg ;								// 채팅방 대화 상대 목록창.
	CChatRoomJoinDlg*			m_pJoinDlg ;									// 채팅방 참여창.
	CChatRoomMainDlg*			m_pMainDlg ;									// 채팅방 목록창.
	CChatRoomOptionDlg*			m_pOptionDlg ;									// 채팅방 옵션창.

	ST_CR_ROOM_SRV				m_MyRoom ;										// HERO의 채팅방 정보.
	cPtrList					m_BackupList ;									// 맵 이동시 채팅내용을 저장할 리스트.
	cPtrList					m_GuestList ;									// HERO의 채팅방 참여자를 담을 리스트.

	DWORD						m_dwLastMyChat ;								// 채팅 메시지를 전송한 시간을 담을 변수.
	DWORD						m_dwLastMyComTime ;								// 채팅방 기능을 사용한 시간을 담을 변수.

	DWORD						m_dwFreezedID[MAX_USER_PER_ROOM] ;				// 채팅방 메시지 차단 아이디 관리 배열.

	BYTE						m_byRequestFriend ;								// 친구요청을 한 상태인지 담는 변수.

	// 옵션 저장을 위한 변수들.
	BYTE					m_byOptSavedMode ;									// 공개/비공개 모드 변수.
	char					m_szOptSavedCode[SECRET_CODE_SIZE+1] ;				// 비밀번호를 담을 문자열.
	BYTE					m_byOptSavedGuestCount ;							// 최대 참여자 수 타입 변수.
	BYTE					m_byOptSavedRoomType ;								// 채팅방 분류 변수.
	char					m_szOptSavedTitleBuff[128+1] ;						// 제목을 담을 문자열.

public:
	CChatRoomMgr(void) ;														// 생성자 함수.
	virtual ~CChatRoomMgr(void)	;												// 소멸자 함수.

	// 채팅방 관련 인터페이스 포인터 설정 함수.
	void SetCreateDlg(CChatRoomCreateDlg* pDlg)			{ m_pCreateDlg = pDlg ; }
	void SetChatRoomDlg(CChatRoomDlg* pDlg)				{ m_pChatRoomDlg = pDlg ; }
	void SetGuestListDlg(CChatRoomGuestListDlg* pDlg)	{ m_pGuestListDlg = pDlg ; }
	void SetJoinDlg(CChatRoomJoinDlg* pDlg)				{ m_pJoinDlg = pDlg ; }
	void SetMainDlg(CChatRoomMainDlg* pDlg)				{ m_pMainDlg = pDlg ; }
	void SetOptionDlg(CChatRoomOptionDlg* pDlg)			{ m_pOptionDlg = pDlg ; }

	void ToggleChatRoomMainDlg() ;						// 채팅방 리스트 여/닫기 함수.

	void ReleaseGuestList() ;							// HERO의 채팅방 참여리스트를 비우는 함수.

	// 옵션 관련 변수들 세팅/반환 함수들.
	//void SetOptSavedMode(BYTE byMode) ;
	BYTE GetOptSavedMode() { return m_byOptSavedMode ; }
	//void SetOptSecretCode(char* pCode) ;
	char* GetOptSecretCode() { return m_szOptSavedCode ; }
	//void SetOptTotalGuest(BYTE byGuestCount) ;
	BYTE GetOptTotalGuest() { return m_byOptSavedGuestCount ; }
	//void SetOptRoomType(BYTE byRoomType) ;
	BYTE GetOptRoomType() { return m_byOptSavedRoomType ; }
	//void SetOptRoomTitle(char* pTitle) ;
	char* GetOptRoomTitle() { return m_szOptSavedTitleBuff ; }

	void SetOption(ST_CR_ROOM_SRV* pInfo) ;

public :
	void NetworkMsgParser(void* pMsg) ;					// 네트워크 메시지 파싱 함수.

	void Add_User_Ack(void* pMsg) ;						// 유저정보 등록 성공 처리 함수.

	void Change_Option_Nack(void* pMsg) ;				// 옵션변경 실패 처리 함수.
	void Change_Option_Notice(void* pMsg) ;				// 옵션변경 공지 함수.

	void Change_Owner_Nack(void* pMsg) ;				// 방장위임 실패 처리 함수.
	void Change_Owner_Notice(void* pMsg) ;				// 방장위임 공지 함수.

	void Chatmsg_Normal_Nack(void* pMsg) ;				// 채팅 메시지 전송 실패 함수.
	void Chatmsg_Normal_Notice(void* pMsg) ;			// 채팅 메시지 출력 함수.

	void Create_Room_Ack(void* pMsg) ;					// 채팅방 개설 성공 함수.
	void Create_Room_Nack(void* pMsg) ;					// 채팅방 개설 실패 함수.

	void Join_Room_Ack(void* pMsg) ;					// 채팅방 참여 성공 함수.
	void Join_Room_Nack(void* pMsg) ;					// 채팅방 참여 실패 함수.
	void Join_Room_Notice(void* pMsg) ;					// 채팅방 참여 공지 함수.

	void Kick_Guest_Ack(void* pMsg) ;					// 강제퇴장 당한 처리 함수.
	void Kick_Guest_Nack(void* pMsg) ;					// 강제퇴장 실패 함수.
	void Kick_Guest_Notice(void* pMsg) ;				// 강제퇴장 공지 함수.

	void Out_Room_Ack(void* pMsg) ;						// 채팅방 나오기 성공 함수.
	void Out_Room_Nack(void* pMsg) ;					// 채팅방 나오기 실패 함수.
	void Out_Room_EmptyRoom(void* pMsg) ;				// 채팅방을 나왔을 때, 채팅방이 하나도 없을 때, 처리하는 함수.
	void Out_Room_Last_Man(void* pMSg) ;				// 채팅방을 나올때, 마지막 사람이었을 경우 처리함수.
	void Out_Room_Notice(void* pMsg) ;					// 채팅방 나가기 공지 함수.
	void Out_Room_Change_Owner_Notice(void* pMsg) ;		// 방장이 비정상적인 경로 등으로 채팅방을 나왔을 때, 방장 위임을 공지하는 함수.

	void Request_Friend_Nack(void* pMsg) ;				// 친구요청 실패 함수.

	void RoomList_Ack(void* pMsg) ;						// 채팅방 리스트 받기 성공 함수.
	void RoomList_Nack(void* pMsg) ;					// 채팅방 리스트 받기 실패 함수.
	
	void SearchName_Ack(void* pMsg) ;					// 이름 검색 성공 함수.
	void SearchName_Nack(void* pMsg) ;					// 이름 검색 실패 함수.

	void SearchTitle_Ack(void* pMsg) ;					// 채팅방 제목 검색 성공 함수.
	void SearchTitle_Nack(void* pMsg) ;					// 채팅방 제목 검색 실패 함수.

	// 채팅방 리스트 정보 업데이트 함수들.
	void UpdateInfo_Created_Room(void* pMsg) ;			// 채팅방 추가 처리.
	void UpdateInfo_Deleted_Room(void* pMsg) ;			// 채팅방 삭제 처리.
	void UpdateInfo_SecretMode(void* pMsg) ;			// 공개/비공개 업데이트 처리.
	void UpdateInfo_RoomType(void* pMsg) ;				// 채팅방 분류 업데이트 처리.
	void UpdateInfo_Title(void* pMsg) ;					// 채팅방 제목 업데이트 처리.
	void UpdateInfo_Cur_GuestCount(void* pMsg) ;		// 현재 참여자 수 업데이트 처리.
	void UpdateInfo_Total_GuestCount(void* pMsg) ;		// 최대 참여자 수 업데이트 처리.

public :
	// HERO 채팅 여부 세팅/반환.
	void Set_ChatRoomState(BYTE byEnum) {m_byChatting = byEnum ; }
	BYTE Get_ChatRoomState() { return m_byChatting ; }

	// 채팅내용 저장/복원 함수들.
	void SaveChat() ;				// 채팅 저장.
	void RestoreChat() ;			// 채팅 복구.
	void ReleaseBackupList() ;		// 저장 리스트 해제.

	// HERO의 채팅방 정보/방장 정보 반환 함수.
	DWORD GetMyRoomOwnerIdx() { return m_MyRoom.dwOwnerIdx ; }
	ST_CR_ROOM_SRV* GetMyRoom() { return &m_MyRoom ; }

	// 채팅방 내, 메시지를 추가하는 함수.
	void AddChatRoomMsg(char* pMsg, BYTE byType) ;

	// 채팅방 내 채팅 메시지를 입력하는 에디트 박스 반환 함수.
	//cTextArea* GetChatRoomTextArea() ;
	cEditBox* GetChatRoomInputBox() ;

	// 메시지 프로시저 함수 - 키보드 입력 처리를 함.
	void MsgProc() ;

	//BOOL IsWhisperMsg() ;
	//void SendNormalMsg() ;
	//void SendWhisperMsg() ;

	// HERO의 마지막 채팅기능 사용 시간을 업데이트/사용가능한 상태인지 반환하는 함수.
	void UpdateMyLastComTime() ;
	BYTE IsEnableExecuteCom() ;

	// HERO의 마지막 채팅시간을 업데이트/반환하는 함수.
	void UpdateChatLastTime(DWORD dwTime) { m_dwLastMyChat = dwTime ; }
	DWORD GetChatListTime() { return m_dwLastMyChat ; }

	BOOL IsFreezed(DWORD dwPlayerID) ;					// 선택한 대상이 차단 된 대상인지 체크하는 함수.
	void FreezeUser(DWORD dwPlayerID) ;					// 선택한 대상을 차단하는 함수.
	void UnFreezeUser(DWORD dwPlayerID) ;				// 선택한 대상의 차단을 해제하는 함수.
	void ClearFreeze() ;								// 차단 대상을 모두 해제하는 함수.
	//void DeleteFreezeUser(DWORD dwPlayerID) ;

	// HERO를 친구요청을 한 상태로 세팅하는 함수.
	void SetRequestFriendState(BYTE byState) { m_byRequestFriend = byState ; }
	// HERO가 친구요청을 한 상태인지 반환하는 함수.
	BYTE IsRequestedFriend() { return m_byRequestFriend ; }
	// HERO의 채팅방의 참여자를 관리하는 리스트를 반환하는 함수.
	cPtrList* GetGuestList() { return &m_GuestList ; }

public :
	// 에러 처리를 하는 함수들.
	void Throw_Error(BYTE errType, char* szCaption) ;	// 에러타입으로 에러 처리를 한다.
	void Throw_Error(char* szErr, char* szCaption) ;	// 에러 메시지로 에러 처리를 한다.
};

EXTERNGLOBALTON(CChatRoomMgr)							// Extern class name to global class.
