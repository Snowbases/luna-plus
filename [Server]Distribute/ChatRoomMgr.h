#pragma once
//=================================================================================================
//	FILE		: CChatRoomMgr.h
//	DESC		: 채팅방 시스템을 관리하는 매니져 클래스.
//	DATE		: APRIL 2, 2008 LYW
//				:
//	DESC		: FEBRUARY 25, 2009 - 기존 유저관리 등을 STL MAP으로 관리 하던 것을 CYHHashTable
//				: 관리 방식으로 수정 함.
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Include Part.
//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
#define CHATROOMMGR		CChatRoomMgr::GetInstance()

// 이 매크로를 사용하면, 메시지 박스로 에러를 출력하고, 사용하지 않으면 로그를 남김.
//#define _USE_ERR_MSGBOX_

enum ENUM_PRINT_ERRORS														// 공통 에러 타입 정의.
{
	err_IMP = 0,															// 함수의 파라메터가 잘못되었다.
	err_FCMTB,																// 기본 메시지로 변환에 실패하였다.
	err_FCMTO,																// 원본 메시지로 변환에 실패하였다.
} ;

enum ENUM_CR_GUEST_COUNT													// 채팅방 최대 참여수 정의.
{
	e_GC_20,																// 20명.
	e_GC_15,																// 15명.
	e_GC_10,																// 10명.
	e_GC_5,																	// 5명.

	e_GC_Max,
} ;

enum ENUM_CR_ROOM_TYPE														// 채팅방 분류 정의.
{
	e_RT_Party = 0,															// 파티모집.
	e_RT_Deal,																// 물품거래.
	e_RT_Normal_Chat,														// 일반채팅.
	e_RT_Local_Chat,														// 지역채팅.
	e_RT_Make_Friend,														// 친구찾기.

	e_RT_Max,
} ;

enum ENUM_CR_ROOM_MODE														// 채팅방 공개/비공개.
{	
	e_RM_Open = 0 ,															// 공개.
	e_RM_Close,																// 비공개.

	e_RM_Max,
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

struct ST_SEARCH_NAME														// 이름 검색용 구조체.
{
	BYTE byRoomIdx ;														// 방인덱스.
	char name[ MAX_NAME_LENGTH+1 ] ;										// 이름.
} ;





//-------------------------------------------------------------------------------------------------
//	The ChatRoom Manager.
//-------------------------------------------------------------------------------------------------
class CChatRoomMgr
{
	CYHHashTable< ST_CR_USER >		m_htUser ; 								// 유저를 관리 할 해쉬 테이블.
	CYHHashTable< ST_SEARCH_NAME >	m_htSearchName ;						// 검색용 유저 이름을 관리 할 해쉬 테이블.
	
	cPtrList					m_pRoomList ;								// 채팅방을 관리하는 리스트.
	cPtrList					m_pTitleList ;								// 검색용 제목을 관리하는 리스트.

	BYTE						m_byRoomIdx[ MAX_ROOM_COUNT+1 ] ;			// 채팅방 인덱스 관리 배열.


public:
	CChatRoomMgr( void ) ;													// 생성자 함수.
	virtual ~CChatRoomMgr( void ) ;											// 소멸자 함수.

	static CChatRoomMgr* GetInstance() ;

	void NetworkMsgParser( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;	// 네트웤 메시지 파서 함수.

private :
	// 리스트 제어 함수들.
	void ReleaseRoom() ;													// 채팅방 리소스 해제 함수.

	BYTE IsInRoomList( BYTE byRoomIdx ) ;									// 방 리스트에, 방 정보가 있는지 체크하는 함수.

	BYTE AddRoom( ST_CR_ROOM_SRV* pInfo ) ;									// 채팅방 정보를 추가하는 함수.
	BYTE DeleteRoom( BYTE byRoomIdx ) ;										// 채팅방 정보를 삭제하는 함수.

	ST_CR_ROOM_SRV* GetRoomInfo( BYTE byRoomIdx ) ;							// 채팅방 정보를 반환하는 함수.

	BYTE GetRoomCountByType( BYTE byType ) ;								// 타입별 방 개수 반환 함수.

	BYTE GetEmptyRoomIdx() ;												// 비어있는 방 인덱스 반환 함수.

	DWORD GetHashCode( const char* pName ) ;								// 인자로 주어진 스트링을 해쉬맵에 사용할 코드 값으로 변환하는 함수.

	void InsertUserName( char* pName ) ;									// 검색용 해쉬맵에 유저 이름을 추가하는 함수.
	void EraseUserName( char* pName ) ;										// 검색용 해쉬맵의 유저 이름을 삭제하는 함수.

	void UpdateNameRoomIdx( char* pName, BYTE byRoomIdx ) ;					// 검색용 해쉬맵의 유저가 참여한 방 번호를 업데이트 하는 함수.

	void UpdateSearchRoomInfo( ST_CR_ROOM_SRV* pInfo ) ;					// 검색용 방의 정보를 업데이트 하는 함수.

private :
	// 에러 메시지 전송 함수.
	void SendErrors( DWORD dwIndex, DWORD dwPlayerID, BYTE byProtocol, BYTE byErr ) ;

	// 처리 함수들.
	void Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// 유저 추가 함수.
	void Force_Add_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;	// 강제로 유저를 추가하는 함수.
	void Del_User_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// 유저 삭제 함수.
	void Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// 채팅방 리스트를 보내주는 함수.
	void Create_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// 채팅방을 생성하는 함수.
	void Join_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// 유저를 채팅방에 참여시키는 함수.
	void Out_Room_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;		// 유저를 채팅방에서 내보내는 함수.
	void Option_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// 옵션을 변경하는 함수.
	void Owner_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// 방장을 변경하는 함수.
	void Kick_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// 유저를 강제 퇴장시키는 함수.
	void Friend_Syn( DWORD dwIndex, char* pMsg, DWORD dwLength ) ;			// 친구요청을 할 때, 유저 정보를 체크하는 함수.
	void UpdateUserInfo( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// 유저 정보를 업데이트 하는 함수.
	void SearchName_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// 이름으로 채팅방 정보를 검색하는 함수.
	void SearchTitle_Syn( DWORD dwIndex, char*pMsg, DWORD dwLength ) ;		// 제목으로 채팅방 정보를 검색하는 함수.

private :
	void UpdateInfo_Created_Room( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Deleted_Room( ST_CR_ROOM_SRV* pInfo ) ;

	void UpdateInfo_Changed_SecretMode( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_RoomType( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Title( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Current_GuestCount( ST_CR_ROOM_SRV* pInfo ) ;
	void UpdateInfo_Changed_Total_GuestCount( ST_CR_ROOM_SRV* pInfo ) ;

private :
	// 에러 처리를 하는 함수들.
	void Throw_Error( BYTE errType, char* szCaption ) ;						// 에러타입으로 에러 처리를 한다.
	void Throw_Error( char* szErr, char* szCaption ) ;						// 에러 메시지로 에러 처리를 한다.
	void Throw_Error( char* szCommonErr, char* szErr, char* szCaption ) ;	// 에러 메시지로 에러 처리를 한다.
	void WriteLog( char* pMsg ) ;											// 로그를 남기는 함수.	
};
