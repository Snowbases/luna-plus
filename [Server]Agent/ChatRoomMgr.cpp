//=================================================================================================
//	FILE		: CChatRoomMgr.cpp
//	DESC		: Implementation part of chatroom manager class.
//	DATE		: MARCH 31, 2008 LYW
//=================================================================================================





//-------------------------------------------------------------------------------------------------
//		Include part.
//-------------------------------------------------------------------------------------------------
#include "stdafx.h"																						// An include file for standard system include files

#include "../[CC]ServerModule/Network.h"																// An include file for network.

#include "./ChatRoomMgr.h"																				// An include file for chatroom manager.

#include "./AgentDBMsgParser.h"
#include "./FilteringTable.h"





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomMgr
//	DESC : The function constructor.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::CChatRoomMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMgr
//	DESC : The function destructor.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::~CChatRoomMgr(void)
{
}





//-------------------------------------------------------------------------------------------------
//	NAME : RegistPlayer_To_Lobby
//	DESC : 유저 정보를 담아 Dist 서버로 보내는 함수.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::IsAddedUser(DWORD dwCharacterID)
{
	// 유저 정보 받기.
	USERINFO* pInfo = NULL ;
	pInfo = g_pUserTableForObjectID->FindUser(dwCharacterID) ;

	ASSERT(pInfo) ;

	if(!pInfo)
	{
		// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
		//char tempMsg[128] = {0, } ;

		char tempMsg[257] = {0, } ;
		sprintf(tempMsg, "%d : Failed to find user to user id", dwCharacterID) ;

		Throw_Error(tempMsg, __FUNCTION__) ;

		return FALSE ;
	}

	if(pInfo->byAddedChatSystem) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : RegistPlayer_To_Lobby
//	DESC : 유저 정보를 담아 Dist 서버로 보내는 함수.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::RegistPlayer_To_Lobby(USERINFO* pUserInfo, BYTE byProtocol) 
{
	// 함수 파라메터 체크.
	ASSERT(pUserInfo) ;

	if(!pUserInfo)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 임시 유저 데이터.
	int nNameLength ;

	ST_CR_USER guest ;
	memset(&guest, 0, sizeof(ST_CR_USER)) ;

	for(BYTE count = 0 ; count < MAX_CHARACTER_NUM ; ++count)
	{
		if(pUserInfo->SelectInfoArray[count].dwCharacterID != pUserInfo->dwCharacterID) continue ;

		// 캐릭터 정보를 받는다.
		guest.dwUserID			= pUserInfo->dwUserID ;

		guest.dwPlayerID	= pUserInfo->SelectInfoArray[count].dwCharacterID ; 
		//guest.wClassIdx		= pUserInfo->SelectInfoArray[count].wClassIndex ;
		guest.byLevel		= (BYTE)pUserInfo->SelectInfoArray[count].Level ;
		guest.byMapNum		= (BYTE)pUserInfo->SelectInfoArray[count].MapNum ;

		// 캐릭터의 이름을 체크한다.
		nNameLength = 0 ;
		nNameLength = strlen(pUserInfo->SelectInfoArray[count].name) ;
		ASSERT(nNameLength > 0) ;
		if(nNameLength <= 0)
		{
			Throw_Error("Failed to receive user name!!", __FUNCTION__) ;
		}

		strcpy(guest.name, pUserInfo->SelectInfoArray[count].name) ;

		// 캐릭터 정보를 담는다.
		MSG_CR_USER msg ;
		memset(&msg, 0, sizeof(MSG_CR_USER)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= byProtocol ;
		msg.dwObjectID	= guest.dwPlayerID ;

		memcpy(&msg.user, &guest, sizeof(ST_CR_USER)) ;

		// Dist로 등록 메시지 전송.
		g_Network.Send2DistributeServer((char*)&msg, sizeof(MSG_CR_USER)) ;

		// 캐릭터의 상태를 Lobby에 있는 상태로 세팅.
		pUserInfo->byAddedChatSystem = TRUE ;

		break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : RegistPlayer_To_Lobby
//	DESC : 유저 정보를 담아 Dist 서버로 보내는 함수.
//  DATE : MAY 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::RegistPlayer_To_Lobby(ST_CR_USER* pUserInfo)
{
	// 함수 파라메터 체크.
	ASSERT(pUserInfo) ;

	if(!pUserInfo)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return FALSE ;
	}



	// 이름 체크.
	int nNameLength = 0 ;
	nNameLength = strlen(pUserInfo->name) ;

	ASSERT(nNameLength > 0) ;
	if(nNameLength <= 0 || !pUserInfo->name)
	{
		Throw_Error("Failed to receive name!!", __FUNCTION__) ;
		return FALSE ;
	}



	// 캐릭터 정보를 담는다.
	MSG_CR_USER msg ;
	memset(&msg, 0, sizeof(MSG_CR_USER)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_ADD_USER_SYN ;
	msg.dwObjectID	= pUserInfo->dwPlayerID ;

	memcpy(&msg.user, pUserInfo, sizeof(ST_CR_USER)) ;

	// Dist로 등록 메시지 전송.
	g_Network.Send2DistributeServer((char*)&msg, sizeof(MSG_CR_USER)) ;

	return TRUE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ForceRegistPlayer_To_Lobby
//	DESC : 강제로 캐릭터 정보를 Dist에 등록 시키는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ForceRegistPlayer_To_Lobby(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 기본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 유저 정보 받기.
	USERINFO* pInfo = NULL ;
	pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID) ;

	if(!pInfo)
	{
		Throw_Error(err_FRUI, __FUNCTION__) ;

		MSGBASE msg ;
		memset(&msg, 0, sizeof(MSGBASE)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_FORCE_ADD_USER_NACK ;
		msg.dwObjectID	= pmsg->dwObjectID ;

		g_Network.Send2User(dwIndex, (char*)&msg, sizeof(MSGBASE)) ;

		return ;
	}

	// 유저 정보 등록.
	RegistPlayer_To_Lobby(pInfo, MP_CHATROOM_FORCE_ADD_USER_SYN) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ChatMsg_Normal_Syn
//	DESC : 채팅방 내에서 메시지 처리를 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ChatMsg_Normal_Syn(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_MSG_BROADCAST* pmsg = NULL ;
	pmsg = (MSG_CR_MSG_BROADCAST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 메시지 전송 처리.
	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		MSG_CR_MSG msg ;
		memset(&msg, 0, sizeof(MSG_CR_MSG)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_CHATMSG_NORMAL_NOTICE ;
		msg.dwObjectID	= pmsg->dwObjectID ;

		SafeStrCpy(msg.name, pmsg->name, MAX_NAME_LENGTH + 1) ;
		SafeStrCpy(msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH + 1) ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&msg, sizeof(MSG_CR_MSG)) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_CHATMSG_NORMAL_SYN)
	{
		MSG_CR_MSG_BROADCAST msg ;
		memset(&msg, 0, sizeof(MSG_CR_MSG_BROADCAST)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_CHATMSG_NORMAL_NOTICE_OTHER_AGENT ;
		msg.dwObjectID	= pmsg->dwObjectID ;

		SafeStrCpy(msg.name, pmsg->name, MAX_NAME_LENGTH + 1) ;
		SafeStrCpy(msg.Msg, pmsg->Msg, MAX_CHAT_LENGTH+1) ;

		for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
		{
			msg.dwUser[count] = pmsg->dwUser[count] ;

			++msg.byCount ;
		}

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&msg, sizeof(MSG_CR_MSG_BROADCAST)) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : DestroyPlayer_From_Lobby
//	DESC : 유저 정보를 삭제하는 함수.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::DestroyPlayer_From_Lobby(USERINFO* pUserInfo) 
{
	// 함수 파라메터 체크.
	ASSERT(pUserInfo) ;

	if(!pUserInfo)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	if(pUserInfo->dwCharacterID == 0 || pUserInfo->dwUserID == 0) return ;

	pUserInfo->byAddedChatSystem = FALSE ;
	
	// 유저 삭제 요청을 보낸다.
	MSGBASE msg ;
	memset(&msg, 0, sizeof(MSGBASE)) ;

	msg.Category	= MP_CHATROOM ;
	msg.Protocol	= MP_CHATROOM_DEL_USER_SYN ;
	msg.dwObjectID	= pUserInfo->dwCharacterID ;

	g_Network.Send2DistributeServer((char*)&msg, sizeof(MSGBASE)) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : UpdatePlayerInfo
//	DESC : 유저의 정보를 업데이트 시키는 함수.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdatePlayerInfo(USERINFO* pUserInfo)
{
	// 함수 파라메터 체크.
	ASSERT(pUserInfo) ;

	if(!pUserInfo)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 임시 유저 데이터.
	ST_CR_USER guest ;
	memset(&guest, 0, sizeof(ST_CR_USER)) ;

	for(BYTE count = 0 ; count < MAX_CHARACTER_NUM ; ++count)
	{
		if(pUserInfo->SelectInfoArray[count].dwCharacterID != pUserInfo->dwCharacterID) continue ;

		// 캐릭터 정보를 받는다.
		//guest.wClassIdx		= pUserInfo->SelectInfoArray[count].wClassIndex ;
		guest.byLevel		= (BYTE)pUserInfo->SelectInfoArray[count].Level ;
		guest.byMapNum		= (BYTE)pUserInfo->SelectInfoArray[count].MapNum ;

		strcpy(guest.name, pUserInfo->SelectInfoArray[count].name) ;

		MSG_CR_USER msg ;
		memset(&msg, 0, sizeof(MSG_CR_USER)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_UPDATE_USERINFO_SYN ;
		msg.dwObjectID	= pUserInfo->dwCharacterID ;

		msg.user.byLevel		= guest.byLevel ;
		msg.user.byMapNum	= guest.byMapNum ;
		//msg.user.wClassIdx	= guest.wClassIdx ;

		SafeStrCpy(msg.user.name, guest.name, MAX_NAME_LENGTH + 1) ;

		g_Network.Send2DistributeServer((char*)&msg, sizeof(MSG_CR_USER)) ;

		break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UserMsgParser
//	DESC : 유저로 부터 오는 네트워크 메시지 처리를 하는 함수.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UserMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 기본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTB, __FUNCTION__) ;
		return ;
	}

	// 프로토콜 체크.
	switch(pmsg->Protocol)
	{
	case MP_CHATROOM_FORCE_ADD_USER_SYN :	ForceRegistPlayer_To_Lobby(dwIndex, pMsg, dwLength) ;	break ;
	case MP_CHATROOM_CHATMSG_NORMAL_SYN :	ChatMsg_Normal_Syn(dwIndex, pMsg, dwLength) ;			break ;
	default : g_Network.Send2DistributeServer(pMsg, dwLength) ;										break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : ServerMsgParser
//	DESC : 서버로 부터 오는 네트워크 메시지 처리를 하는 합수.
//  DATE : MARCH 31, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ServerMsgParser(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 기본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTB, __FUNCTION__) ;
		return ;
	}
	
	// 프로토콜 확인.
	switch(pmsg->Protocol)
	{
	case MP_CHATROOM_ADD_USER_ACK :

	case MP_CHATROOM_ROOMLIST_ACK :
	case MP_CHATROOM_ROOMLIST_NACK :

	case MP_CHATROOM_CREATE_ROOM_ACK :	
	case MP_CHATROOM_CREATE_ROOM_NACK :

	case MP_CHATROOM_JOIN_ROOM_ACK :
	case MP_CHATROOM_JOIN_ROOM_NACK :

	case MP_CHATROOM_OUT_ROOM_ACK :
	case MP_CHATROOM_OUT_ROOM_NACK :
	case MP_CHATROOM_OUT_ROOM_EMPTYROOM :
	case MP_CHATROOM_OUT_ROOM_LAST_MAN :

	case MP_CHATROOM_CHANGE_OPTION_NACK :

	case MP_CHATROOM_CHANGE_OWNER_NACK :

	case MP_CHATROOM_KICK_GUEST_NACK :

	case MP_CHATROOM_REQUEST_FRIEND_NACK :

	case MP_CHATROOM_CHATMSG_NORMAL_NACK :	

	case MP_CHATROOM_SEARCH_FOR_NAME_ACK :
	case MP_CHATROOM_SEARCH_FOR_NAME_NACK :

	case MP_CHATROOM_SEARCH_FOR_TITLE_ACK :
	case MP_CHATROOM_SEARCH_FOR_TITLE_NACK :

	case MP_CHATROOM_UPDATEINFO_CREATED_ROOM :
	case MP_CHATROOM_UPDATEINFO_DELETED_ROOM :
	case MP_CHATROOM_UPDATEINFO_SECRETMODE :
	case MP_CHATROOM_UPDATEINFO_ROOMTYPE :
	case MP_CHATROOM_UPDATEINFO_TITLE :
	case MP_CHATROOM_UPDATEINFO_CUR_GUESTCOUNT :
	case MP_CHATROOM_UPDATEINFO_TOTAL_GUESTCOUNT :				SendMsg2User(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_JOIN_ROOM_NOTICE :					
	case MP_CHATROOM_JOIN_ROOM_OTHER_AGENT :					Join_Room_Notice(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_OUT_ROOM_NOTICE :
	case MP_CHATROOM_OUT_ROOM_NOTICE_OTHER_AGENT :				Out_Room_Notice(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE :
	case MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE_OTHER_AGENT : Out_Room_Change_Owner_Notcie(dwIndex, pMsg, dwLength) ; break ;		

	case MP_CHATROOM_CHANGE_OPTION_NOTICE :
	case MP_CHATROOM_CHANGE_OPTION_NOTICE_OTHER_AGENT :			Change_Option_Notice(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_CHANGE_OWNER_NOTICE :
	case MP_CHATROOM_CHANGE_OWNER_NOTICE_OTHER_AGENT :			Change_Owner_Notice(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_KICK_GUEST_ACK :
	case MP_CHATROOM_KICK_GUEST_ACK_OTHER_AGENT :				Kick_Guest_Ack(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_KICK_GUEST_NOTICE :
	case MP_CHATROOM_KICK_GUEST_NOTICE_OTHER_AGENT :			Kick_Guest_Notice(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_REQUEST_FRIEND_ACK :						Request_Friend_Ack(dwIndex, pMsg, dwLength) ; break ;

	case MP_CHATROOM_CHATMSG_NORMAL_NOTICE_OTHER_AGENT :		ChatMsg_Normal_Syn(dwIndex, pMsg, dwLength) ; break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SendMsg2User
//	DESC : 서버(Dist)로 부터 전달 된 메시지를 클라이언트로 전송.
//  DATE : APRIL 2, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SendMsg2User(DWORD dwIndex, char* pMsg, DWORD dwLength) 
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSGBASE* pmsg = NULL ;
	pmsg = (MSGBASE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}


	// 유저 찾기.
	USERINFO* pInfo = NULL ;
	pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwObjectID) ;

	if(!pInfo) return ;

	// 메시지 전달.
	g_Network.Send2User(pInfo->dwConnectionIndex, pMsg, dwLength) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Ack
//		 : 
//	DESC : 채팅방 참여 성공 처리를 하는 함수.
//		 : 
//		 : 1. 전송용 메시지 선언.
//		 : 2. 유저 검색.
//		 : 3. 참가한 유저일경우는 성공 메시지 전송.
//		 : 4. 참가한 유저가 아닐경우에는 참여 공지 전송.
//		 : 5. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_JOIN_NOTICE* pmsg = NULL ;
	pmsg = (MSG_CR_JOIN_NOTICE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_JOIN_NOTICE) ;

	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_JOIN_NOTICE sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_JOIN_ROOM_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_JOIN_ROOM_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_JOIN_NOTICE sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_JOIN_ROOM_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Notice
//		 : 
//	DESC : 채팅방 나가기 성공 처리를 하는 함수.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 나가는 유저일경우는 성공 메시지 전송.
//		 : 3. 나가는 유저가 아닐경우에는 참여 공지 전송.
//		 : 4. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 7, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_IDNAME* pmsg = NULL ;
	pmsg = (MSG_CR_IDNAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_IDNAME) ;

	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_OUT_ROOM_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_OUT_ROOM_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_OUT_ROOM_NOTICE_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Change_Owner_Notcie
//		 : 
//	DESC : 채팅방을 나간 사람 처리를 하는데, 방장이 정상적이지 않은 경로로 나갔을 때 처리하는 함수.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 메시지 전송.
//		 : 3. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Change_Owner_Notcie(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_IDNAME* pmsg = NULL ;
	pmsg = (MSG_CR_IDNAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_IDNAME) ;

	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Change_Option_Notice
//		 : 
//	DESC : 채팅방 옵션이 변경되었다는 처리를 하는 함수.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 메시지 전송.
//		 : 3. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Option_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOM_NOTICE* pmsg = NULL ;
	pmsg = (MSG_CR_ROOM_NOTICE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_ROOM_NOTICE) ;

	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_ROOM_NOTICE sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_CHANGE_OPTION_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_CHANGE_OPTION_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_ROOM_NOTICE sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_CHANGE_OPTION_NOTICE_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Change_Owner_Notice
//		 :
//	DESC : 방장 변경 성공 처리를 하는 함수.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 메시지 전송.
//		 : 3. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Owner_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_IDNAME* pmsg = NULL ;
	pmsg = (MSG_CR_IDNAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_IDNAME) ;

	// 유저 수 만큼 루프.
	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_CHANGE_OWNER_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_CHANGE_OWNER_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_CHANGE_OWNER_NOTICE_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Guest_Ack
//		 : 
//	DESC : 강제 퇴장 당한 사람에게 전하는 처리.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 메시지 전송.
//		 : 3. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Guest_Ack(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_KICK_ACK* pmsg = NULL ;
	pmsg = (MSG_CR_KICK_ACK*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_KICK_ACK) ;

	// 유저를 검색.
	USERINFO* pInfo = NULL ;
	pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwKickPlayer) ;

	if(pInfo)
	{
		// 메시지를 복사.
		MSG_CR_KICK_ACK sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_KICK_GUEST_ACK ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}
	else
	{
		// 다른 에이전트에도 전송.
		if(pmsg->Protocol == MP_CHATROOM_KICK_GUEST_ACK)
		{
			// 메시지를 복사.
			MSG_CR_KICK_ACK sendMsg ;
			memset(&sendMsg, 0, nSize) ;
			memcpy(&sendMsg, pmsg, nSize) ;

			// 다른 에이전트에게 전송 할 프로토콜 세팅.
			sendMsg.Protocol = MP_CHATROOM_KICK_GUEST_ACK_OTHER_AGENT ;

			// 다른 에이전트에 메시지를 전송.
			g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
		}
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Guest_Notice
//		 : 
//	DESC : 강제 퇴장 성공 처리를 하는 함수.
//		 : 
//		 : 1. 유저 검색.
//		 : 2. 메시지 전송.
//		 : 3. 다른 에이전트에도 같은 방식으로 전송.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Guest_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_IDNAME* pmsg = NULL ;
	pmsg = (MSG_CR_IDNAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 많이 사용되는 연산 - 사이즈 받기.
	int nSize = 0 ;
	nSize = sizeof(MSG_CR_IDNAME) ;

	// 유저 수 만큼 루프.
	USERINFO* pInfo ;
	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
	{
		// 유저를 검색.
		pInfo = NULL ;
		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;

		if(!pInfo) continue ;

		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		sendMsg.Protocol = MP_CHATROOM_KICK_GUEST_NOTICE ;

		// 메시지를 유저에게 전송.
		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
	}

	// 다른 에이전트에도 전송.
	if(pmsg->Protocol == MP_CHATROOM_KICK_GUEST_NOTICE)
	{
		// 메시지를 복사.
		MSG_CR_IDNAME sendMsg ;
		memset(&sendMsg, 0, nSize) ;
		memcpy(&sendMsg, pmsg, nSize) ;

		// 다른 에이전트에게 전송 할 프로토콜 세팅.
		sendMsg.Protocol = MP_CHATROOM_KICK_GUEST_NOTICE_OTHER_AGENT ;

		// 다른 에이전트에 메시지를 전송.
		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
	}
}





////-------------------------------------------------------------------------------------------------
////	NAME : ChatMsg_Normal_Notice
////		 : 
////	DESC : 채팅방 내 메시지 전송 처리를 하는 함수.
////		 : 
////	DESC : 강제 퇴장 성공 처리를 하는 함수.
////		 : 
////		 : 1. 유저 검색.
////		 : 2. 메시지 전송.
////		 : 3. 다른 에이전트에도 같은 방식으로 전송.
////		 : 
////  DATE : APRIL 14, 2008 LYW
////-------------------------------------------------------------------------------------------------
//void CChatRoomMgr::ChatMsg_Normal_Notice(DWORD dwIndex, char* pMsg, DWORD dwLength)
//{
//	// 함수 파라메터 체크.
//	ASSERT(pMsg) ;
//
//	if(!pMsg)
//	{
//		Throw_Error(err_IMP, __FUNCTION__) ;
//		return ;
//	}
//
//	// 원본 메시지 변환.
//	MSG_CR_MSG_RESULT* pmsg = NULL ;
//	pmsg = (MSG_CR_MSG_RESULT*)pMsg ;
//
//	ASSERT(pmsg) ;
//
//	if(!pmsg)
//	{
//		Throw_Error(err_FCMTO, __FUNCTION__) ;
//		return ;
//	}
//
//	// 많이 사용되는 연산 - 사이즈 받기.
//	int nSize = 0 ;
//	nSize = sizeof(MSG_CR_MSG_RESULT) ;
//
//	// 유저 수 만큼 루프.
//	USERINFO* pInfo ;
//	for(BYTE count = 0 ; count < pmsg->byCount ; ++count)
//	{
//		// 유저를 검색.
//		pInfo = NULL ;
//		pInfo = g_pUserTableForObjectID->FindUser(pmsg->dwUser[count]) ;
//
//		if(!pInfo) continue ;
//
//		// 메시지를 복사.
//		MSG_CR_MSG_RESULT sendMsg ;
//		memset(&sendMsg, 0, nSize) ;
//		memcpy(&sendMsg, pmsg, nSize) ;
//
//		// 메시지를 유저에게 전송.
//		g_Network.Send2User(pInfo->dwConnectionIndex, (char*)&sendMsg, nSize) ;
//	}
//
//	// 다른 에이전트에도 전송.
//	if(pmsg->Protocol == MP_CHATROOM_CHATMSG_NORMAL_NOTICE)
//	{
//		// 메시지를 복사.
//		MSG_CR_MSG_RESULT sendMsg ;
//		memset(&sendMsg, 0, nSize) ;
//		memcpy(&sendMsg, pmsg, nSize) ;
//
//		// 다른 에이전트에게 전송 할 프로토콜 세팅.
//		sendMsg.Protocol = MP_CHATROOM_CHATMSG_NORMAL_NOTICE_OTHER_AGENT ;
//
//		// 다른 에이전트에 메시지를 전송.
//		g_Network.Send2AgentExceptThis((char*)&sendMsg, nSize) ;
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME : Request_Friend_Ack
//		 : 
//	DESC : 친구 요청 체크 성공 처리를 하는 함수.
//		 : 
//		 : 1. 유저 찾기.
//		 : 2. 친구 추가 처리.
//		 : 
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Request_Friend_Ack(DWORD dwIndex, char* pMsg, DWORD dwLength)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_NAME* pmsg = NULL ;
	pmsg = (MSG_NAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 유저 찾기.
	USERINFO * userinfo = NULL ;
	userinfo = (USERINFO *)g_pUserTableForObjectID->FindUser(pmsg->dwObjectID) ;

	if(!userinfo) return ;
	
	// 친구 추가 처리.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char buf[MAX_NAME_LENGTH+1] = {0, } ;
	//SafeStrCpy( buf, pmsg->Name, MAX_NAME_LENGTH+1 ) ;

	char buf[257] = {0, } ;

	SafeStrCpy( buf, pmsg->Name, 256 ) ;

	//금지문자 체크 "'"등...
	if( FILTERTABLE->IsInvalidCharInclude( (unsigned char*) buf ) ) return ;
	
	// 100305 ONS 허락여부를 묻지않고 친구를 추가하도록 수정.
	FriendAddFriendByName(pmsg->dwObjectID, buf) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Error
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error(BYTE errType, char* szCaption)
{
	// 함수 파라메터 체크.
	ASSERT(szCaption) ;

	if(!szCaption || strlen(szCaption) <= 1)
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK) ;
#else
		// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
		//char tempStr[128] = {0, } ;
		//SafeStrCpy(tempStr, __FUNCTION__, 128) ;

		char tempStr[257] = {0, } ;

		SafeStrCpy(tempStr, __FUNCTION__, 256) ;
		strcat(tempStr, " - ") ;
		strcat(tempStr, "Invalid Caption!!") ;
		WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	// 에러 타입 확인.
	//char tempErr[128] = {0, } ;
	//switch(errType)
	//{
	//case err_IMP :		SafeStrCpy(tempErr, "Invalid a message parameter!!", 128) ;					break ;
	//case err_FCMTB :	SafeStrCpy(tempErr, "Failed to convert a message to base!!", 128) ;			break ;
	//case err_FCMTO :	SafeStrCpy(tempErr, "Failed to convert a message to original!!", 128) ;		break ;
	//case err_FRUI : 	SafeStrCpy(tempErr, "Failed to receive a user info!!", 128) ;				break ;
	//default : break ;
	//}

	char tempErr[257] = {0, } ;

	switch(errType)
	{
	case err_IMP :		SafeStrCpy(tempErr, "Invalid a message parameter!!", 256) ;					break ;
	case err_FCMTB :	SafeStrCpy(tempErr, "Failed to convert a message to base!!", 256) ;			break ;
	case err_FCMTO :	SafeStrCpy(tempErr, "Failed to convert a message to original!!", 256) ;		break ;
	case err_FRUI : 	SafeStrCpy(tempErr, "Failed to receive a user info!!", 256) ;				break ;
	default : break ;
	}

	// 에러 출력/로그.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, tempErr, szCaption, MB_OK) ;
#else
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempStr[128] = {0, } ;
	//SafeStrCpy(tempStr, szCaption, 128) ;

	char tempStr[257] = {0, } ;

	SafeStrCpy(tempStr, szCaption, 256) ;
	strcat(tempStr, " - ") ;
	strcat(tempStr, tempErr) ;
	WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error(char* szErr, char* szCaption)
{
	// 함수 파라메터 체크.
	ASSERT(szCaption) ;

	if(!szErr || strlen(szErr) <= 1)
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid err message!!", "Throw_Error", MB_OK) ;
#else
		// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
		//char tempStr[128] = {0, } ;
		//SafeStrCpy(tempStr, __FUNCTION__, 128) ;

		char tempStr[257] = {0, } ;

		SafeStrCpy(tempStr, __FUNCTION__, 256) ;
		strcat(tempStr, " - ") ;
		strcat(tempStr, "Invalid err message!!") ;
		WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	if(!szCaption || strlen(szCaption) <= 1)
	{
#ifdef _USE_ERR_MSGBOX_
		MessageBox( NULL, "Invalid Caption!!", "Throw_Error", MB_OK) ;
#else
		// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
		//char tempStr[128] = {0, } ;
		//SafeStrCpy(tempStr, __FUNCTION__, 128) ;

		char tempStr[257] = {0, } ;

		SafeStrCpy(tempStr, __FUNCTION__, 256) ;
		strcat(tempStr, " - ") ;
		strcat(tempStr, "Invalid Caption!!") ;
		WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
		return ;
	}

	// 에러 출력/로그.
#ifdef _USE_ERR_MSGBOX_
	MessageBox( NULL, szErr, szCaption, MB_OK) ;
#else
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempStr[128] = {0, } ;
	//SafeStrCpy(tempStr, szCaption, 128) ;

	char tempStr[257] = {0, } ;

	SafeStrCpy(tempStr, szCaption, 256) ;
	strcat(tempStr, " - ") ;
	strcat(tempStr, szErr) ;
	WriteLog(tempStr) ;
#endif //_USE_ERR_MSGBOX_
}





//-------------------------------------------------------------------------------------------------
//	NAME : WriteLog
//	DESC : 채팅방 내, 에러 메시지 관련 로그를 남기는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::WriteLog(char* pMsg)
{
	SYSTEMTIME time ;
	GetLocalTime(&time) ;

	TCHAR szTime[_MAX_PATH] = {0, } ;
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond) ;

	FILE *fp = fopen("Log/Agent-ChatRoomErr.log", "a+");
	if (fp)
	{
		fprintf(fp, "%s [%s]\n", pMsg,  szTime);
		fclose(fp);
	}
}




