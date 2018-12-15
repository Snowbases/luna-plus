#pragma once
//=================================================================================================
//	FILE		: CChatRoomMgr.h
//	DESC		: 채팅방 시스템과 관련 된 메시지들의 전송을 관리하는 매니져 클래스.
//	DATE		: MARCH 31, 2008 LYW
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Definition part.
//-------------------------------------------------------------------------------------------------
#define CHATROOMMGR	CChatRoomMgr::GetInstance()

//#define _USE_ERR_MSGBOX_		// 이 매크로를 사용하면, 메시지 박스로 에러를 출력하고, 사용하지 않으면 로그를 남김.

enum ENUM_PRINT_ERRORS
{
	err_IMP = 0,				// 함수의 파라메터가 잘못되었다.
	err_FCMTB,					// 기본 메시지로 변환에 실패하였다.
	err_FCMTO,					// 원본 메시지로 변환에 실패하였다.
	err_FRUI,					// 유저 정보를 받는데 실패하였다.
} ;





//-------------------------------------------------------------------------------------------------
//	The ChatRoom Manager.
//-------------------------------------------------------------------------------------------------
class CChatRoomMgr
{
public:
	GETINSTANCE(CChatRoomMgr) ;			// 채팅 매니져 포인터 반환.

	CChatRoomMgr(void) ;				// 생성자 함수.
	virtual ~CChatRoomMgr(void) ;		// 소멸자 함수.

	// 채팅방 시스템에 추가 된 유저인지 검사하는 함수.
	BYTE IsAddedUser(DWORD dwCharacterID) ;

	// 서버(Dist)에 유저 등록을 시키는 함수.
	void RegistPlayer_To_Lobby(USERINFO* pUserInfo, BYTE byProtocol) ;
	BYTE RegistPlayer_To_Lobby(ST_CR_USER* pUserInfo) ;
	// 강제로 서버(Dist)에 유저를 등록시키는 함수.
	void ForceRegistPlayer_To_Lobby(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	// 유저를 삭제시키는 함수.
	void DestroyPlayer_From_Lobby(USERINFO* pUserInfo) ;

	// 유저의 정보를 업데이트 시키는 함수.
	void UpdatePlayerInfo(USERINFO* pUserInfo) ;

	// 채팅방 내에서 메시지 처리를 하는 함수.
	void ChatMsg_Normal_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

public :
	// 클라이언트로 부터 오는 메시지를 처리하는 함수.
	void UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

	// 서버(Dist/Agent)로 부터 오는 메시지를 처리하는 함수.
	void ServerMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

	// 유저에게 메시지 바로 전송하는 함수.
	void SendMsg2User(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void BroadCastToUser(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	// 상황에 따라 다른 Agent에게 메시지를 전송하는 함수.
	void Join_Room_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Out_Room_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Out_Room_Change_Owner_Notcie(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Change_Option_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Change_Owner_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Kick_Guest_Ack(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	void Kick_Guest_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;
	//void ChatMsg_Normal_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

	void Request_Friend_Ack(DWORD dwIndex, char* pMsg, DWORD dwLength) ;

public :
	// 에러 처리를 하는 함수들.
	void Throw_Error(BYTE errType, char* szCaption) ;	// 에러타입으로 에러 처리를 한다.
	void Throw_Error(char* szErr, char* szCaption) ;	// 에러 메시지로 에러 처리를 한다.
	void WriteLog(char* pMsg) ;							// 로그를 남기는 함수.
};
