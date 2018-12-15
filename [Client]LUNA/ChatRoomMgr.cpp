#include "stdafx.h"
#include "../WindowIDEnum.h"

#include "./Interface/cWindowManager.h"																	// Include header file of window manager.

#include "./Interface/cListDialog.h"
//#include "./Interface/cTextArea.h"
#include "./Interface/cEditBox.h"

#include "./cMsgBox.h"																					// Include header file of message box.
#include "./GameIn.h"

#include "./ObjectManager.h"
#include "./UserInfoManager.h"
#include "./FilteringTable.h"

#include "./ChatManager.h"
#include "./ChatRoomMgr.h"

#include "./ChatRoomCreateDlg.h"
#include "./ChatRoomDlg.h"
#include "./ChatRoomGuestListDlg.h"
#include "./ChatRoomJoinDlg.h"
#include "./ChatRoomMainDlg.h"
#include "./ChatRoomOptionDlg.h"





//-------------------------------------------------------------------------------------------------
//		Define some macroinstruction.
//-------------------------------------------------------------------------------------------------
GLOBALTON(CChatRoomMgr)





//-------------------------------------------------------------------------------------------------
//	NAME : CChatRoomMgr
//	DESC : 생성자 함수.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::CChatRoomMgr(void)
{
	// 초기화 처리.
	m_byChatting	= e_State_Away_From_Chatroom ;

	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	m_pCreateDlg	= NULL ;
	m_pChatRoomDlg	= NULL ;
	m_pGuestListDlg = NULL ;
	m_pJoinDlg		= NULL ;
	m_pMainDlg		= NULL ;
	m_pOptionDlg	= NULL ;

	m_BackupList.RemoveAll() ;

	m_dwLastMyChat = 0 ;

	m_dwLastMyComTime = 0 ;

	// 유저 상태 초기화.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_dwFreezedID[count] = 0 ;
	}

	m_byRequestFriend = FALSE ;

	// 옵션 저장을 위한 변수들 초기화.
	m_byOptSavedMode			= e_RM_Open ;										
	memset(m_szOptSavedCode, 0, sizeof(SECRET_CODE_SIZE+1)) ;

	m_byOptSavedGuestCount	= e_GC_20 ;
	m_byOptSavedRoomType		= e_RT_Party ;

	memset(m_szOptSavedTitleBuff, 0, 128) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ~CChatRoomMgr
//	DESC : 소멸자 함수.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
CChatRoomMgr::~CChatRoomMgr(void)
{
	ReleaseGuestList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : ToggleChatRoomMainDlg
//		 : 
//	DESC : 채팅방 목록창 여/닫기 함수.
//		 : 
//		 : 1. 채팅방 목록을여는 상황이면, HERO의 상태체크.
//		 : 2. 로비에 등록 된 상태가 아니면, 로비 등록을 요청.
//		 : 3. 등록 된 상태면, 채팅방 리스트 요청.
//		 : 
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ToggleChatRoomMainDlg()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// 채팅방 목록 여/닫기.
	if(m_pMainDlg->IsActive())
	{
		m_pMainDlg->SetActive(FALSE) ;
		// 100223 ONS 채팅창을 닫을때 경고메세지도 같이 닫는다.
		cDialog* pDlg = WINDOWMGR->GetWindowForID( MBI_CHATROOM_NOTICE );
		if( pDlg )
			WINDOWMGR->AddListDestroyWindow( pDlg );
	}
	else
	{
		if(CHATROOMMGR->Get_ChatRoomState() == e_State_Away_From_Chatroom)
		{
			// 유저의 클라이언트가 다운 되었을 경우, 
			// 서버 쪽에 캐릭터의 등록을 강제로 다시 하는 요청을 한다.
			MSGBASE msg ;
			memset(&msg, 0, sizeof(MSGBASE)) ;

			msg.Category	= MP_CHATROOM ;
			msg.Protocol	= MP_CHATROOM_FORCE_ADD_USER_SYN ;
			msg.dwObjectID	= HEROID ;

            NETWORK->Send( &msg, sizeof(MSGBASE) ) ;
		}
		else
		{
			// 재사용 시간 체크.
			if(!IsEnableExecuteCom())
			{
				WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ;
				return ;
			}
			// 열때는, 채팅방 목록 받아오기.
			//m_pMainDlg->SetActive(TRUE) ;
			
			MSG_BYTE2 msg ;
			memset(&msg, 0, sizeof(MSG_BYTE2)) ;

			msg.Category		= MP_CHATROOM ;
			msg.Protocol		= MP_CHATROOM_ROOM_SYN ;
			msg.dwObjectID		= HEROID ;

			msg.bData1			= e_RTM_AllLooK ;
			msg.bData2			= 0 ;

			NETWORK->Send( &msg, sizeof(MSG_BYTE2) ) ;

			// 시간 업데이트.
			CHATROOMMGR->UpdateMyLastComTime() ;
		}

		m_pMainDlg->SetActive(TRUE) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : ReleaseGuestList
//	DESC : Player가 참여한 방의 참여자 정보를 담은 리스트를 해제 한다.
//  DATE : APRIL 23, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ReleaseGuestList()
{
	ST_CR_USER* pInfo ;
	PTRLISTPOS pos = NULL ;
	pos = m_GuestList.GetHeadPosition() ;

	while(pos)
	{
		pInfo = NULL ;
		pInfo = (ST_CR_USER*)m_GuestList.GetNext(pos) ;

		if(!pInfo) continue ;
		delete pInfo ;
	}

	m_GuestList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SetOption
//	DESC : 변경 된 옵션을 저장하는 함수.
//  DATE : APRIL 30, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SetOption(ST_CR_ROOM_SRV* pInfo)
{
	// 함수 파라메터 체크.
	ASSERT(pInfo) ;

	if(!pInfo)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}



	// 하위 컨트롤 체크.
	ASSERT(m_pOptionDlg) ;

	if(!m_pOptionDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 옵션 적용.
	m_byOptSavedMode		= pInfo->bySecretMode ;

	SafeStrCpy(m_szOptSavedCode, pInfo->code, SECRET_CODE_SIZE) ;

	m_byOptSavedGuestCount	= pInfo->byTotalGuestCount ;
	m_byOptSavedRoomType	= pInfo->byRoomType ;

	SafeStrCpy(m_szOptSavedTitleBuff, pInfo->title, 128) ;

	// 적용된 옵션 세팅.
	m_pOptionDlg->SetStoredDlg() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : NetworkMsgParser
//	DESC : 네트워크 메시지 분류 함수.
//  DATE : APRIL 3, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::NetworkMsgParser(void* pMsg)
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
	case MP_CHATROOM_ADD_USER_ACK :					Add_User_Ack(pMsg) ;					break ;

	case MP_CHATROOM_CHANGE_OPTION_NACK :			Change_Option_Nack(pMsg) ;				break ;
	case MP_CHATROOM_CHANGE_OPTION_NOTICE :			Change_Option_Notice(pMsg) ;			break ;

	case MP_CHATROOM_CHANGE_OWNER_NACK :			Change_Owner_Nack(pMsg) ;				break ;
	case MP_CHATROOM_CHANGE_OWNER_NOTICE :			Change_Owner_Notice(pMsg) ;				break ;

	case MP_CHATROOM_CHATMSG_NORMAL_NACK :			Chatmsg_Normal_Nack(pMsg) ;				break ;
	case MP_CHATROOM_CHATMSG_NORMAL_NOTICE :		Chatmsg_Normal_Notice(pMsg) ;			break ;

	case MP_CHATROOM_CREATE_ROOM_ACK :				Create_Room_Ack(pMsg) ;					break ;
	case MP_CHATROOM_CREATE_ROOM_NACK :				Create_Room_Nack(pMsg) ;				break ;

	case MP_CHATROOM_JOIN_ROOM_ACK :				Join_Room_Ack(pMsg) ;					break ;
	case MP_CHATROOM_JOIN_ROOM_NACK :				Join_Room_Nack(pMsg) ;					break ;
	case MP_CHATROOM_JOIN_ROOM_NOTICE :				Join_Room_Notice(pMsg) ;				break ;

	case MP_CHATROOM_KICK_GUEST_ACK :				Kick_Guest_Ack(pMsg) ;					break ;
	case MP_CHATROOM_KICK_GUEST_NACK :				Kick_Guest_Nack(pMsg) ;					break ;
	case MP_CHATROOM_KICK_GUEST_NOTICE :			Kick_Guest_Notice(pMsg) ;				break ;

	case MP_CHATROOM_OUT_ROOM_ACK :					Out_Room_Ack(pMsg) ;					break ;
	case MP_CHATROOM_OUT_ROOM_NACK :				Out_Room_Nack(pMsg) ;					break ;
	case MP_CHATROOM_OUT_ROOM_EMPTYROOM :			Out_Room_EmptyRoom(pMsg) ;				break ;
	case MP_CHATROOM_OUT_ROOM_LAST_MAN :			Out_Room_Last_Man(pMsg) ;				break ;
	case MP_CHATROOM_OUT_ROOM_NOTICE :				Out_Room_Notice(pMsg) ;					break ;
	case MP_CHATROOM_OUT_ROOM_CHANGE_OWNER_NOTICE :	Out_Room_Change_Owner_Notice(pMsg) ;	break ;

	case MP_CHATROOM_REQUEST_FRIEND_NACK :			Request_Friend_Nack(pMsg) ;				break ;

	case MP_CHATROOM_ROOMLIST_ACK :					RoomList_Ack(pMsg) ;					break ;
	case MP_CHATROOM_ROOMLIST_NACK :				RoomList_Nack(pMsg) ;					break ;

	case MP_CHATROOM_SEARCH_FOR_NAME_ACK :			SearchName_Ack(pMsg) ;					break ;
	case MP_CHATROOM_SEARCH_FOR_NAME_NACK :			SearchName_Nack(pMsg) ;					break ;

	case MP_CHATROOM_SEARCH_FOR_TITLE_ACK :			SearchTitle_Ack(pMsg) ;					break ;
	case MP_CHATROOM_SEARCH_FOR_TITLE_NACK :		SearchTitle_Nack(pMsg) ;				break ;

	case MP_CHATROOM_UPDATEINFO_CREATED_ROOM :		UpdateInfo_Created_Room(pMsg) ;			break ;
	case MP_CHATROOM_UPDATEINFO_DELETED_ROOM :		UpdateInfo_Deleted_Room(pMsg) ;			break ;
	case MP_CHATROOM_UPDATEINFO_SECRETMODE :		UpdateInfo_SecretMode(pMsg) ;			break ;
	case MP_CHATROOM_UPDATEINFO_ROOMTYPE :			UpdateInfo_RoomType(pMsg) ;				break ;
	case MP_CHATROOM_UPDATEINFO_TITLE :				UpdateInfo_Title(pMsg) ;				break ;
	case MP_CHATROOM_UPDATEINFO_CUR_GUESTCOUNT :	UpdateInfo_Cur_GuestCount(pMsg) ;		break ;
	case MP_CHATROOM_UPDATEINFO_TOTAL_GUESTCOUNT :	UpdateInfo_Total_GuestCount(pMsg) ;		break ;

	default : break ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : Add_User_Ack
//	DESC : HERO의 상태를 로비에 있는 상태로 세팅.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Add_User_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}
	
	// HERO 상태 변경.
	Set_ChatRoomState(e_State_In_Lobby) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Change_Option_Nack
//	DESC : 옵션 변경 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Option_Nack(void* pMsg) 
{
	// 에러 메시지 정의.
	// 0 = err_You_Are_Not_In_ChatRoom,
	// 1 = err_CanNot_Found_Room,
	// 2 = err_You_Are_Not_Owner,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1498 ; break ;
	case 1 : wErrMsgNum = 1507 ; break ;
	case 2 : wErrMsgNum = 1487 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Change_Option_Notice
//		 : 
//	DESC : 옵션 변경이 성공했을 때 처리하는 함수.
//		 : 
//		 : 1. CHATMGR의 HERO 채팅방 정보 업데이트.
//		 : 2. 옵션창에 바뀐 옵션 적용.
//		 : 3. 채팅방 리스트 업데이트.
//		 : 4. 채팅방에 옵션 변경 공지 적용.
//		 : 
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Option_Notice(void* pMsg)
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

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;
	ASSERT(m_pChatRoomDlg) ;
	ASSERT(m_pOptionDlg) ;

	if(!m_pMainDlg || !m_pChatRoomDlg || !m_pOptionDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 정보 받기.
	m_MyRoom = pmsg->room ;

	// 채팅방 리스트 업데이트.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	// 옵션창에 바뀐 옵션 적용.
	//if(!m_pOptionDlg->SetOption(&pmsg->room))
	//{
	//	Throw_Error("Failed to apply option!!", __FUNCTION__) ;
	//	return ;
	//}
	SetOption(&pmsg->room) ;

	// 옵션 변경 공지.
	m_pChatRoomDlg->SetRoomTitle(&myRoom) ;

	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[128] = {0, } ;
	char tempMsg[256] = {0, } ;
	strcat(tempMsg, "[") ;
	strcat(tempMsg, CHATMGR->GetChatMsg(1494)) ;
	strcat(tempMsg, "]") ;
	m_pChatRoomDlg->AddMsg(tempMsg, e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Change_Owner_Nack
//	DESC : 방장 변경 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Owner_Nack(void* pMsg)
{
	// 에러 메시지 정의.
	// 0 = err_Invalid_Current_Owner,
	// 1 = err_Invalid_Next_Owner,
	// 2 = err_CanNot_Found_RoomInfo,
	// 3 = err_You_Are_Not_Owner,
	// 4 = err_You_Alone,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1501 ; break ;
	case 1 : wErrMsgNum = 1502 ; break ;
	case 2 : wErrMsgNum = 1503 ; break ;
	case 3 : wErrMsgNum = 1487 ; break ;
	case 4 : wErrMsgNum = 1489 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Change_Owner_Notice
//	DESC : 방장 변경 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Change_Owner_Notice(void* pMsg) 
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

	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;
	ASSERT(m_pGuestListDlg) ;

	if(!m_pChatRoomDlg || !m_pGuestListDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 방장 인덱스 업데이트.
	m_MyRoom.dwOwnerIdx = pmsg->dwID ;

	// 참여자 인덱스 업데이트.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_MyRoom.dwUser[count] = pmsg->dwUser[count] ;
	}

	// 참여자 수 업데이트.
	m_MyRoom.byCurGuestCount = pmsg->byCount ;



	// 방장 위임 공지 출력.
	// 0807054 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char NoticeMsg[128] = {0, } ;
	char NoticeMsg[256] = {0, } ;
	strcat(NoticeMsg, "[") ;
	strcat(NoticeMsg, "<") ;
	strcat(NoticeMsg, pmsg->name) ;
	strcat(NoticeMsg, ">") ;
	strcat(NoticeMsg, CHATMGR->GetChatMsg(1495)) ;
	strcat(NoticeMsg, "]") ;
	
	m_pChatRoomDlg->AddMsg(NoticeMsg, e_ChatMsg_System) ;

	m_pGuestListDlg->SetClassMark() ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Chatmsg_Normal_Nack
//	DESC : 채팅방 내 채팅 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Chatmsg_Normal_Nack(void* pMsg) 
{
	// 에러 메시지 정의.
	// 0 = err_Invalid_Room_Info,
	// 1 = err_Invalid_Sender_Info,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1503 ; break ;
	case 1 : wErrMsgNum = 1514 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Chatmsg_Normal_Notice
//	DESC : 채팅방 내, 메시지 전송 성공 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Chatmsg_Normal_Notice(void* pMsg) 
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_MSG* pmsg = NULL ;
	pmsg = (MSG_CR_MSG*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pGuestListDlg) ;
	ASSERT(m_pChatRoomDlg) ;

	if(!m_pGuestListDlg || !m_pChatRoomDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// 차단 된 대상이면 return 처리.
	if(IsFreezed(pmsg->dwObjectID)) return ;

	// 메시지 타입을 결정.
	BYTE byMsgType = e_ChatMsg_OtherMsg ;

	if(pmsg->dwObjectID == HEROID)
	{
		byMsgType = e_ChatMsg_MyMsg ;
	}

	// 메시지 세팅 후, 채팅방에 출력.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[(MAX_CHAT_LENGTH+1) + (MAX_NAME_LENGTH+1)] = {0,} ;
	char tempMsg[(256+1) + (256+1)] = {0,} ;

	strcat(tempMsg, "[") ;
	strcat(tempMsg, pmsg->name) ;
	strcat(tempMsg, "]") ;
	strcat(tempMsg, ": ") ;
	strcat(tempMsg, pmsg->Msg) ;

	m_pChatRoomDlg->AddMsg(tempMsg, byMsgType) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Create_Room_Ack
//	DESC : 채팅방 생성 성공을 처리하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Create_Room_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMINFO* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMINFO*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;
	ASSERT(m_pGuestListDlg) ;
	ASSERT(m_pMainDlg) ;
	ASSERT(m_pOptionDlg) ;

	if(!m_pChatRoomDlg || !m_pGuestListDlg || !m_pMainDlg || !m_pOptionDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 방정보 복사.
	m_MyRoom = pmsg->room ;

	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->room.byIdx ;
	myRoom.bySecretMode			= pmsg->room.bySecretMode ;
	myRoom.byRoomType			= pmsg->room.byRoomType ;
	myRoom.byCurGuestCount		= pmsg->room.byCurGuestCount ;
	myRoom.byTotalGuestCount	= pmsg->room.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pmsg->room.title, TITLE_SIZE) ;

	// 채팅방 리스트에 방정보 추가.
	// 채팅방 리스트가 열려있다면, 방 분류도 맞으면 추가.
	/*if(m_pMainDlg->IsActive())
	{
		BYTE byType = 0 ;
		byType = m_pMainDlg->GetCurRoomType() ;

		if(byType == pmsg->room.byRoomType || byType == e_RTM_AllLooK)
		{
			m_pMainDlg->Add_Room(&myRoom) ;
		}
	}*/

	m_pMainDlg->Add_Room(&myRoom) ;

	// 채팅방 옵션 적용.
	//if(!m_pOptionDlg->SetOption(&m_MyRoom))
	//{
	//	Throw_Error("Failed to setting room option!!", __FUNCTION__) ;
	//	return ;
	//}
	SetOption(&m_MyRoom) ;

	// 참여자 리스트 적용.
	m_pGuestListDlg->InitGuestList(pmsg->user, 1) ;

	// 채팅창 활성화.
	m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->SetRoomTitle(&myRoom) ;
	m_pChatRoomDlg->AddCommonMsg(e_ChatMsg_StepIn, HERO->GetObjectName()) ;
	m_pChatRoomDlg->SetActive(TRUE) ;

	// HERO 채팅 상태로 세팅.
	Set_ChatRoomState(e_State_In_ChatRoom) ;

	// 차단 명단 초기화.
	ClearFreeze() ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Create_Room_Nack
//	DESC : 채팅방 생성 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Create_Room_Nack(void* pMsg) 
{
	// 에러 메시지 정의.
	// 0 = err_Already_Chatting,
	// 1 = err_RoomCount_Over,
	// 2 = err_User_Is_Not_In_Lobby,
	// 3 = err_Registed_But_Not_Found_Room,
	// 4 = err_Registed_But_Not_Found_User,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1505 ; break ;
	case 1 : wErrMsgNum = 1506 ; break ;
	case 2 : wErrMsgNum = 1514 ; break ;
	case 3 : wErrMsgNum = 1507 ; break ;
	case 4 : wErrMsgNum = 1508 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(wErrMsgNum)) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Ack
//	DESC : 채팅방 참여 성공 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Ack(void* pMsg) 
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMINFO* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMINFO*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;
	ASSERT(m_pGuestListDlg) ;
	ASSERT(m_pMainDlg) ;
	ASSERT(m_pOptionDlg) ;

	if(!m_pChatRoomDlg || !m_pGuestListDlg || !m_pMainDlg || !m_pOptionDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 방정보 복사.
	m_MyRoom = pmsg->room ;

	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->room.byIdx ;
	myRoom.bySecretMode			= pmsg->room.bySecretMode ;
	myRoom.byRoomType			= pmsg->room.byRoomType ;
	myRoom.byCurGuestCount		= pmsg->room.byCurGuestCount ;
	myRoom.byTotalGuestCount	= pmsg->room.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pmsg->room.title, TITLE_SIZE) ;

	// 채팅방 리스트에 방정보 추가.
	// 채팅방 리스트가 열려있다면, 방 분류도 맞으면 추가.
	/*if(m_pMainDlg->IsActive())
	{
		BYTE byType = 0 ;
		byType = m_pMainDlg->GetCurRoomType() ;

		if(byType == pmsg->room.byRoomType || byType == e_RTM_AllLooK)
		{
			m_pMainDlg->Update_RoomInfo(&myRoom) ;
		}
	}*/
	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	// 채팅방 옵션 적용.
	//if(!m_pOptionDlg->SetOption(&m_MyRoom))
	//{
	//	Throw_Error("Failed to setting room option!!", __FUNCTION__) ;
	//	return ;
	//}
	SetOption(&m_MyRoom) ;

	// 참여자 리스트 적용.
	m_pGuestListDlg->InitGuestList(pmsg->user, pmsg->room.byCurGuestCount) ;

	// 채팅창 활성화.
	m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->SetRoomTitle(&myRoom) ;
	m_pChatRoomDlg->AddCommonMsg(e_ChatMsg_StepIn, HERO->GetObjectName()) ;
	m_pChatRoomDlg->SetActive(TRUE) ;

	// HERO 채팅 상태로 세팅.
	Set_ChatRoomState(e_State_In_ChatRoom) ;

	// 차단 명단 초기화.
	ClearFreeze() ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Nack
//	DESC : 채팅방 참여 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Nack(void* pMsg) 
{
	// 에러 메시지 정의
    // 0 = err_Already_Chatting,
	// 1 = err_Invalid_Room_Info,
	// 2 = err_Invalid_Secret_Code,
	// 3 = err_Guest_Count_Is_Over,
	// 4 = err_Invalid_User_Info,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1505 ; break ;
	case 1 : wErrMsgNum = 1509 ; break ;
	case 2 : wErrMsgNum = 1510 ; break ;
	case 3 : wErrMsgNum = 1511 ; break ;
	case 4 : wErrMsgNum = 1514 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(wErrMsgNum)) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Join_Room_Notice
//	DESC : 
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Join_Room_Notice(void* pMsg) 
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

	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;
	ASSERT(m_pGuestListDlg) ;
	ASSERT(m_pMainDlg) ;

	if(!m_pChatRoomDlg || !m_pGuestListDlg || !m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}


	// 참여자 인덱스 업데이트.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_MyRoom.dwUser[count] = pmsg->dwUser[count] ;
	}

	// 참여자 수 업데이트.
	m_MyRoom.byCurGuestCount = pmsg->byCount ;

	// 새로 참가한 유저 아이디 추가.
	m_MyRoom.dwUser[m_MyRoom.byCurGuestCount] = pmsg->user.dwPlayerID ;

	// 참여자 수 증가 처리.
	++m_MyRoom.byCurGuestCount ;



	// 채팅방 리스트에 방정보 업데이트.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	// 참여자 리스트 적용.
	if(!m_pGuestListDlg->AddGuest(&pmsg->user))
	{
		Throw_Error("Failed to add guest!!", __FUNCTION__) ;
		return ;
	}

	// 입장 공지 추가.
	m_pChatRoomDlg->AddCommonMsg(e_ChatMsg_StepIn, pmsg->user.name) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Guest_Ack
//	DESC : 강제 퇴장 당한 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Guest_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 방정보 초기화.
	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_SRV)) ;

	// 채팅방 리스트에 방정보 업데이트.
	m_pMainDlg->Init_RoomList((MSG_CR_ROOMLIST*)pmsg) ;

	// 채팅방 옵션 초기화.
	m_pOptionDlg->SetActive(FALSE) ;
	m_pOptionDlg->SetDlgToDefault() ;

	// 참여자 리스트 초기화.
	m_pGuestListDlg->SetActive(FALSE) ;
	m_pGuestListDlg->SetDlgToDefault() ;

	// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->CloseChatRoom() ;

	// HERO 로비 상태로 세팅.
	Set_ChatRoomState(e_State_In_Lobby) ;

	WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1513)) ;




	//// 함수 파라메터 체크.
	//ASSERT(pMsg) ;

	//if(!pMsg)
	//{
	//	Throw_Error(err_IMP, __FUNCTION__) ;
	//	return ;
	//}

	//// 원본 메시지 변환.
	//MSGBASE* pmsg = NULL ;
	//pmsg = (MSGBASE*)pMsg ;

	//ASSERT(pmsg) ;

	//if(!pmsg)
	//{
	//	Throw_Error(err_FCMTO, __FUNCTION__) ;
	//	return ;
	//}




	//--m_MyRoom.byCurGuestCount ;

	//if(m_MyRoom.byCurGuestCount == 0)
	//{
	//	// 채팅방 리스트에 방정보 삭제.
	//	m_pMainDlg->DeleteRoom(m_MyRoom.byIdx) ;
	//}
	//else
	//{
	//	ST_CR_ROOM_CLT myRoom ;
	//	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//	myRoom.byIdx				= m_MyRoom.byIdx ;
	//	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	//	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	//	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	//	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	//	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	//	// 채팅방 리스트에 방정보 업데이트.
	//	m_pMainDlg->Update_RoomInfo(&myRoom) ;
	//}

	//// 방정보 초기화.
	//memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//// 채팅방 옵션 초기화.
	//m_pOptionDlg->SetDlgToDefault() ;
	//m_pOptionDlg->SetActive(FALSE) ;

	//// 참여자 리스트 초기화.
	//m_pGuestListDlg->SetDlgToDefault() ;
	//m_pGuestListDlg->SetActive(FALSE) ;

	//// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;

	//// HERO 로비 상태로 세팅.
	//Set_ChatRoomState(e_State_In_Lobby) ;

	//// 강퇴 메시지 띄우기.
	//WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1513)) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Guest_Nack
//	DESC : 강제 퇴장 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Guest_Nack(void* pMsg) 
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_RoomInfo,
	// 1 = err_Invalid_OwnerInfo,
	// 2 = err_You_Are_Not_Owner,
	// 3 = err_Invalid_Target,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1509 ; break ;
	case 1 : wErrMsgNum = 1501 ; break ;
	case 2 : wErrMsgNum = 1487 ; break ;
	case 3 : wErrMsgNum = 1502 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Kick_Guest_Notice
//	DESC : 강퇴 공지 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Kick_Guest_Notice(void* pMsg) 
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



	// 참여자 인덱스 업데이트.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_MyRoom.dwUser[count] = pmsg->dwUser[count] ;
	}

	// 참여자 수 업데이트.
	m_MyRoom.byCurGuestCount = pmsg->byCount ;



	// 채팅방 리스트에 방정보 업데이트.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	// 참여자 리스트 적용.
	m_pGuestListDlg->DeleteGuest(pmsg->dwID) ;

	// 메시지 세팅 후, 채팅방에 출력.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[(MAX_CHAT_LENGTH+1) + (MAX_NAME_LENGTH+1)] = {0,} ;
	char tempMsg[(256+1) + (256+1)] = {0,} ;

	strcat(tempMsg, "[") ;
	strcat(tempMsg, "<") ;
	strcat(tempMsg, pmsg->name) ;
	strcat(tempMsg, ">") ;
	strcat(tempMsg, CHATMGR->GetChatMsg(1496)) ;
	strcat(tempMsg, "]") ;

	m_pChatRoomDlg->AddMsg(tempMsg, e_ChatMsg_System) ;

	// 나간사람 차단명단에서 삭제.
	//DeleteFreezeUser(pmsg->dwID) ;
	UnFreezeUser(pmsg->dwID) ;



	//if(pmsg->dwID == HEROID)
	//{
	//	ST_CR_ROOM_CLT myRoom ;
	//	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//	myRoom.byIdx				= m_MyRoom.byIdx ;
	//	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	//	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	//	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	//	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	//	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	//	// 채팅방 리스트에 방정보 업데이트.
	//	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	//	// 방정보 초기화.
	//	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//	// 채팅방 옵션 초기화.
	//	m_pOptionDlg->SetDlgToDefault() ;
	//	m_pOptionDlg->SetActive(FALSE) ;

	//	// 참여자 리스트 초기화.
	//	m_pGuestListDlg->SetDlgToDefault() ;
	//	m_pGuestListDlg->SetActive(FALSE) ;

	//	// 채팅창 초기화.
	//	m_pChatRoomDlg->SetActive(FALSE) ;
	//	m_pChatRoomDlg->SetDlgToDefault() ;

	//	// HERO 로비 상태로 세팅.
	//	Set_ChatRoomState(e_State_In_Lobby) ;

	//	// 강퇴 메시지 띄우기.
	//	WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1513)) ;
	//}
	//else
	//{
	//	// 유저 인덱스를 임시 보관한다.
	//	BYTE byCheckCount = 0 ;
	//	DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
	//	for(BYTE count = 0 ; count < m_MyRoom.byCurGuestCount ; ++count)
	//	{
	//		if(m_MyRoom.dwUser[count] == pmsg->dwID) continue ;

	//		dwPlayerID[byCheckCount] = m_MyRoom.dwUser[count] ;
	//		++byCheckCount ;
	//	}

	//	// 채팅방의 참여자 수 감소.
	//	--m_MyRoom.byCurGuestCount ;

	//	// 유저 인덱스 복원
	//	for(BYTE count = 0 ; count  < MAX_USER_PER_ROOM ; ++count)
	//	{
	//		m_MyRoom.dwUser[count] = dwPlayerID[count] ;
	//	}

	//	// 채팅방 리스트에 방정보 업데이트.
	//	ST_CR_ROOM_CLT myRoom ;
	//	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//	myRoom.byIdx				= m_MyRoom.byIdx ;
	//	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	//	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	//	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	//	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	//	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	//	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	//	// 참여자 리스트 적용.
	//	m_pGuestListDlg->DeleteGuest(pmsg->dwID) ;

	//	// 메시지 세팅 후, 채팅방에 출력.
	//	char tempMsg[(MAX_CHAT_LENGTH+1) + (MAX_NAME_LENGTH+1)] = {0,} ;

	//	strcat(tempMsg, "[") ;
	//	strcat(tempMsg, "<") ;
	//	strcat(tempMsg, pmsg->name) ;
	//	strcat(tempMsg, ">") ;
	//	strcat(tempMsg, CHATMGR->GetChatMsg(1496)) ;
	//	strcat(tempMsg, "]") ;

	//	m_pChatRoomDlg->AddMsg(tempMsg, e_ChatMsg_System) ;

	//	// 나간사람 차단명단에서 삭제.
	//	DeleteFreezeUser(pmsg->dwID) ;
	//}
}






//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Ack
//		 : 
//	DESC : 방 나오기 성공 처리를 하는 함수.
//		 : 
//		 : Ack는 마지막 남은 사람일 경우 처리를 해준다.
//		 : 마지막 남은
//		 : 
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 방정보 초기화.
	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_SRV)) ;

	// 채팅방 리스트에 방정보 업데이트.
	m_pMainDlg->Init_RoomList((MSG_CR_ROOMLIST*)pmsg) ;

	// 채팅방 옵션 초기화.
	m_pOptionDlg->SetActive(FALSE) ;
	m_pOptionDlg->SetDlgToDefault() ;

	// 참여자 리스트 초기화.
	m_pGuestListDlg->SetActive(FALSE) ;
	m_pGuestListDlg->SetDlgToDefault() ;

	// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->CloseChatRoom() ;

	// HERO 로비 상태로 세팅.
	Set_ChatRoomState(e_State_In_Lobby) ;


	//// 함수 파라메터 체크.
	//ASSERT(pMsg) ;

	//if(!pMsg)
	//{
	//	Throw_Error(err_IMP, __FUNCTION__) ;
	//	return ;
	//}

	//// 원본 메시지 변환.
	//MSGBASE* pmsg = NULL ;
	//pmsg = (MSGBASE*)pMsg ;

	//ASSERT(pmsg) ;

	//if(!pmsg)
	//{
	//	Throw_Error(err_FCMTO, __FUNCTION__) ;
	//	return ;
	//}



	//// 참여자 수 감소.
	//--m_MyRoom.byCurGuestCount ;

	//if(m_MyRoom.byCurGuestCount == 0)
	//{
	//	// 채팅방 리스트에 방정보 삭제.
	//	m_pMainDlg->DeleteRoom(m_MyRoom.byIdx) ;
	//}
	//else
	//{
	//	ST_CR_ROOM_CLT myRoom ;
	//	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//	myRoom.byIdx				= m_MyRoom.byIdx ;
	//	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	//	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	//	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	//	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	//	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	//	m_pMainDlg->Update_RoomInfo(&myRoom) ;
	//}

	//// 방정보 초기화.
	//memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	//// 채팅방 옵션 초기화.
	//m_pOptionDlg->SetDlgToDefault() ;
	//m_pOptionDlg->SetActive(FALSE) ;

	//// 참여자 리스트 초기화.
	//m_pGuestListDlg->SetDlgToDefault() ;
	//m_pGuestListDlg->SetActive(FALSE) ;

	//// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;

	//// HERO 로비 상태로 세팅.
	//Set_ChatRoomState(e_State_In_Lobby) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Nack
//	DESC : 채팅방 퇴장 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Nack(void* pMsg)
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_Room,
	// 1 = err_CanNot_Found_Outer,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1503 ; break ;
	case 1 : wErrMsgNum = 1514 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_EmptyRoom
//	DESC : 채팅방을 나왔을 때, 더이상 방이 없을 때 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_EmptyRoom(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 방정보 초기화.
	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_SRV)) ;

	// 채팅방 리스트에 방정보 업데이트.
	m_pMainDlg->Clear_RoomList() ;
	m_pMainDlg->Refresh_RoomList() ;

	// 채팅방 옵션 초기화.
	m_pOptionDlg->SetActive(FALSE) ;
	m_pOptionDlg->SetDlgToDefault() ;

	// 참여자 리스트 초기화.
	m_pGuestListDlg->SetActive(FALSE) ;
	m_pGuestListDlg->SetDlgToDefault() ;

	// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->CloseChatRoom() ;

	// HERO 로비 상태로 세팅.
	Set_ChatRoomState(e_State_In_Lobby) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Last_Man
//	DESC : 마지막으로 채팅방을 나왔을 때 처리를 한다. Ack와는 처리하는 내용이 다르다.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Last_Man(void* pMsg) 
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 방정보 초기화.
	memset(&m_MyRoom, 0, sizeof(ST_CR_ROOM_SRV)) ;

	// 채팅방 리스트에 방정보 업데이트.
	m_pMainDlg->Init_RoomList((MSG_CR_ROOMLIST*)pmsg) ;

	// 채팅방 옵션 초기화.
	m_pOptionDlg->SetActive(FALSE) ;
	m_pOptionDlg->SetDlgToDefault() ;

	// 참여자 리스트 초기화.
	m_pGuestListDlg->SetActive(FALSE) ;
	m_pGuestListDlg->SetDlgToDefault() ;

	// 채팅창 초기화.
	//m_pChatRoomDlg->SetActive(FALSE) ;
	//m_pChatRoomDlg->SetDlgToDefault() ;
	m_pChatRoomDlg->CloseChatRoom() ;

	// HERO 로비 상태로 세팅.
	Set_ChatRoomState(e_State_In_Lobby) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Notice
//	DESC : 다른 유저가 채팅방을 나갔다는 공지 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Notice(void* pMsg) 
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



	// 방장 인덱스 업데이트.
	//m_MyRoom.dwOwnerIdx = pmsg->dwID ;

	// 참여자 인덱스 업데이트.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_MyRoom.dwUser[count] = pmsg->dwUser[count] ;
	}

	// 참여자 수 업데이트.
	m_MyRoom.byCurGuestCount = pmsg->byCount ;



	//// 유저 인덱스를 임시 보관한다.
	//BYTE byCheckCount = 0 ;
	//DWORD dwPlayerID[ MAX_USER_PER_ROOM ] = {0, } ;
	//for(BYTE count = 0 ; count < m_MyRoom.byCurGuestCount ; ++count)
	//{
	//	if(m_MyRoom.dwUser[count] == pmsg->dwID) continue ;

	//	dwPlayerID[byCheckCount] = m_MyRoom.dwUser[count] ;
	//	++byCheckCount ;
	//}

	//// 채팅방의 참여자 수 감소.
	//--m_MyRoom.byCurGuestCount ;

	//// 유저 인덱스 복원
	//for(BYTE count = 0 ; count  < m_MyRoom.byCurGuestCount ; ++count)
	//{
	//	m_MyRoom.dwUser[count] = dwPlayerID[count] ;
	//}


	// 채팅방 리스트에 방정보 업데이트.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(&myRoom) ;

	// 참여자 리스트 적용.
	m_pGuestListDlg->DeleteGuest(pmsg->dwID) ;

	// 메시지 세팅 후, 채팅방에 출력.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[MAX_CHAT_LENGTH] = {0,} ;
	char tempMsg[256] = {0,} ;

	strcat(tempMsg, "[") ;
	strcat(tempMsg, "<") ;
	strcat(tempMsg, pmsg->name) ;
	strcat(tempMsg, ">") ;
	strcat(tempMsg, CHATMGR->GetChatMsg(1497)) ;
	strcat(tempMsg, "]") ;

	m_pChatRoomDlg->AddMsg(tempMsg, e_ChatMsg_System) ;

	// 나간사람 차단명단에서 삭제.
	//DeleteFreezeUser(pmsg->dwID) ;
	UnFreezeUser(pmsg->dwID) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : Out_Room_Change_Owner_Notice
//	DESC : 방장이 정상적이지 않은 경로로 채팅방을 나갔을 때, 남은 사람에게 처리하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Out_Room_Change_Owner_Notice(void* pMsg)
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



	// 방장 인덱스 업데이트.
	m_MyRoom.dwOwnerIdx = pmsg->dwID ;

	// 참여자 인덱스 업데이트.
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_MyRoom.dwUser[count] = pmsg->dwUser[count] ;
	}

	// 참여자 수 업데이트.
	m_MyRoom.byCurGuestCount = pmsg->byCount ;



	// 채팅방 리스트에 방정보 업데이트.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(&myRoom) ;



	// 공지 처리.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char NoticeMsg[128] = {0, } ;
	char NoticeMsg[256] = {0, } ;
	strcat(NoticeMsg, "[") ;
	strcat(NoticeMsg, "<") ;
	strcat(NoticeMsg, pmsg->name) ;
	strcat(NoticeMsg, ">") ;
	strcat(NoticeMsg, CHATMGR->GetChatMsg(1495)) ;
	strcat(NoticeMsg, "]") ;
	
	m_pChatRoomDlg->AddMsg(NoticeMsg, e_ChatMsg_System) ;

	m_pGuestListDlg->SetClassMark() ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : Request_Friend_Nack
//	DESC : 친구요청 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Request_Friend_Nack(void* pMsg)
{
	// 에러 메시지 정의.
	// 0 = err_Invalid_Requester_Info,
	// 1 = err_Invalid_Target_Info.
	// 2 = err_You_Are_Not_In_SameRoom,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}



	// 에러 메시지 확인.
	WORD wErrMsgNum = 0 ;
	switch(pmsg->bData)
	{
	case 0 : wErrMsgNum = 1514 ; break ;
	case 1 : wErrMsgNum = 1502 ; break ;
	case 2 : wErrMsgNum = 1504 ; break ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;
}






//-------------------------------------------------------------------------------------------------
//	NAME : RoomList_Ack
//	DESC : 채팅방 받기 성공 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::RoomList_Ack(void* pMsg) 
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 채팅방 리스트 개수 체크.
	if(pmsg->byCount == 0)
	{
		WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1490)) ;
	}

	// 채팅방 리스트 세팅.
	m_pMainDlg->Init_RoomList(pMsg) ;

	if(Get_ChatRoomState() == e_State_Away_From_Chatroom)
	{
		Set_ChatRoomState(e_State_In_Lobby) ;
	}
}






//-------------------------------------------------------------------------------------------------
//	NAME : RoomList_Nack
//	DESC : 채팅방 목록 받기 실패 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::RoomList_Nack(void* pMsg) 
{
	// 에러 메시지 정의.
	// 0 = err_Empty_Room,
	// 1 = err_Delay_Time,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	switch(pmsg->bData)
	{
	case 0 : m_pMainDlg->Err_EmptyRoom() ; return ;
	case 1 : WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1517)) ; return ;
	default : return ;
	}

	// 채팅방에 에러 메시지 출력.
	//WINDOWMGR->MsgBox( MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(wErrMsgNum)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchName_Ack
//	DESC : 이름으로 검색 성공 처리를 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchName_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_SEARCH_NAME* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_NAME*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}
	


	// 검색 결과 처리.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[128] = {0, } ;
	char tempMsg[256] = {0, } ;
	sprintf(tempMsg, CHATMGR->GetChatMsg(1536), pmsg->name, pmsg->byRoomIdx) ;
	WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, tempMsg) ; return ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchName_Nack
//	DESC : 이름으로 검색 실패 처리를 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchName_Nack(void* pMsg)
{
	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	switch(pmsg->bData)
	{
	case 0 : WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1514)) ; break ;
	case 1 : 
	case 2 : WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1537)) ; break ;
	default : return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchTitle_Ack
//	DESC : 채팅방 제목으로 채팅방 검색 성공 처리를 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchTitle_Ack(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMLIST* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMLIST*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 리스트 세팅.
	m_pMainDlg->Init_RoomList(pMsg) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : SearchTitle_Nack
//	DESC : 채팅방 제목으로 채팅방 검색 실패 처리를 하는 함수.
//  DATE : APRIL 24, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SearchTitle_Nack(void* pMsg)
{
	// 에러 메시지 정의.
	// 0 = err_CanNot_Found_Room,

	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	//// 에러 메시지 확인.
	//WORD wErrMsgNum = 0 ;
	//switch(pmsg->bData)
	//{
	//case 0 : wErrMsgNum = 1503 ; break ;
	//case 1 : wErrMsgNum = 1514 ; break ;
	//case 2 : wErrMsgNum = 1514 ; break ;
	//default : return ;
	//}

	//// 채팅방에 에러 메시지 출력.
	//AddChatRoomMsg(CHATMGR->GetChatMsg(wErrMsgNum), e_ChatMsg_System) ;

	m_pMainDlg->Clear_RoomList() ;

	WINDOWMGR->MsgBox(MBI_CHATROOM_NOTICE, MBT_OK, CHATMGR->GetChatMsg(1537)) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Created_Room
//	DESC : 서버에서 보내온 정보를로 채팅방리스트에 방을 추가하는 함수.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Created_Room(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_ROOMINFO_TO_NOTICE* pmsg = NULL ;
	pmsg = (MSG_CR_ROOMINFO_TO_NOTICE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	if(m_MyRoom.byIdx == pmsg->room.byIdx) return ;



	// 채팅방 리스트에 채팅방 추가하기.
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->room.byIdx ;
	myRoom.bySecretMode			= pmsg->room.bySecretMode ;
	myRoom.byRoomType			= pmsg->room.byRoomType ;
	myRoom.byCurGuestCount		= pmsg->room.byCurGuestCount ;
	myRoom.byTotalGuestCount	= pmsg->room.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pmsg->room.title, TITLE_SIZE) ;

	m_pMainDlg->Add_Room(&myRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Deleted_Room
//	DESC : 서버에서 보내온 정보를로 채팅방 리스트에서 방을 삭제하는 함수.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Deleted_Room(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE* pmsg = NULL ;
	pmsg = (MSG_BYTE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	if(m_MyRoom.byIdx == pmsg->bData) return ;



	// 채팅방 리스트에서 채팅방 삭제.
	m_pMainDlg->DeleteRoom(pmsg->bData) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_SecretMode
//	DESC : 서버에서 보내온 정보로 채팅방리스트에서, 정해진 방의 공개/비공개 설정을 변경한다.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_SecretMode(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	//if(m_MyRoom.byIdx == pmsg->bData1) return ;



	// 채팅방 리스트에서 채팅방 정보 업데이트.
	ST_CR_ROOM_CLT* pRoom = NULL ;
	pRoom = m_pMainDlg->GetRoomListItem(pmsg->bData1) ;

	if(!pRoom) return ;

	/*ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->bData1 ;
	myRoom.bySecretMode			= pmsg->bData2 ;
	myRoom.byRoomType			= pRoom->byRoomType ;
	myRoom.byCurGuestCount		= pRoom->byCurGuestCount ;
	myRoom.byTotalGuestCount	= pRoom->byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pRoom->title, TITLE_SIZE) ;*/

	pRoom->bySecretMode = pmsg->bData2 ;

	m_pMainDlg->Update_RoomInfo(pRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_RoomType
//	DESC : 서버에서 보내온 정보로 채팅방 리스트에서, 정해진 방의 채팅방 분류를 변경한다.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_RoomType(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	//if(m_MyRoom.byIdx == pmsg->bData1) return ;



	// 채팅방 리스트에서 채팅방 정보 업데이트.
	ST_CR_ROOM_CLT* pRoom = NULL ;
	pRoom = m_pMainDlg->GetRoomListItem(pmsg->bData1) ;

	if(!pRoom) return ;

	/*
	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->bData1 ;
	myRoom.bySecretMode			= pRoom->bySecretMode ;
	myRoom.byRoomType			= pmsg->bData2 ;
	myRoom.byCurGuestCount		= pRoom->byCurGuestCount ;
	myRoom.byTotalGuestCount	= pRoom->byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pRoom->title, TITLE_SIZE) ;*/

	pRoom->byRoomType	= pmsg->bData2 ;

	m_pMainDlg->Update_RoomInfo(pRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Title
//	DESC : 서버에서 보내온 정보로 채팅방 리스트에서, 정해진 방의 채팅방 제목을 변경한다.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Title(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_CR_SEARCH_TITLE* pmsg = NULL ;
	pmsg = (MSG_CR_SEARCH_TITLE*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	//if(m_MyRoom.byIdx == pmsg->byRoomIdx) return ;



	// 채팅방 리스트에서 채팅방 정보 업데이트.
	ST_CR_ROOM_CLT* pRoom = NULL ;
	pRoom = m_pMainDlg->GetRoomListItem(pmsg->byRoomIdx) ;

	if(!pRoom) return ;

	/*ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->byRoomIdx ;
	myRoom.bySecretMode			= pRoom->bySecretMode ;
	myRoom.byRoomType			= pRoom->byRoomType ;
	myRoom.byCurGuestCount		= pRoom->byCurGuestCount ;
	myRoom.byTotalGuestCount	= pRoom->byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pmsg->title, TITLE_SIZE) ;*/

	SafeStrCpy(pRoom->title, pmsg->title, TITLE_SIZE) ;

	m_pMainDlg->Update_RoomInfo(pRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Cur_GuestCount
//	DESC : 서버에서 보내온 정보로 채팅방 리스트에서, 정해진 방의 현재 참여자 수를 변경한다.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Cur_GuestCount(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	//if(m_MyRoom.byIdx == pmsg->bData1) return ;



	// 채팅방 리스트에서 채팅방 정보 업데이트.
	ST_CR_ROOM_CLT* pRoom = NULL ;
	pRoom = m_pMainDlg->GetRoomListItem(pmsg->bData1) ;

	if(!pRoom) return ;

	/*ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->bData1 ;
	myRoom.bySecretMode			= pRoom->bySecretMode ;
	myRoom.byRoomType			= pRoom->byRoomType ;
	myRoom.byCurGuestCount		= pmsg->bData2 ;
	myRoom.byTotalGuestCount	= pRoom->byTotalGuestCount ;

	SafeStrCpy(myRoom.title, pRoom->title, TITLE_SIZE) ;*/

	pRoom->byCurGuestCount = pmsg->bData2 ;

	m_pMainDlg->Update_RoomInfo(pRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateInfo_Total_GuestCount
//	DESC : 서버에서 보내온 정보로 채팅방 리스트에서, 정해진 방의 최대 참여자 수를 변경한다.
//  DATE : APRIL 29, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateInfo_Total_GuestCount(void* pMsg)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 원본 메시지 변환.
	MSG_BYTE2* pmsg = NULL ;
	pmsg = (MSG_BYTE2*)pMsg ;

	ASSERT(pmsg) ;

	if(!pmsg)
	{
		Throw_Error(err_FCMTO, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pMainDlg) ;

	if(!m_pMainDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}



	// 내 채팅방의 정보와 같으면 넘겨버린다.
	//if(m_MyRoom.byIdx == pmsg->bData1) return ;



	// 채팅방 리스트에서 채팅방 정보 업데이트.
	ST_CR_ROOM_CLT* pRoom = NULL ;
	pRoom = m_pMainDlg->GetRoomListItem(pmsg->bData1) ;

	if(!pRoom) return ;

	/*ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= pmsg->bData1 ;
	myRoom.bySecretMode			= pRoom->bySecretMode ;
	myRoom.byRoomType			= pRoom->byRoomType ;
	myRoom.byCurGuestCount		= pRoom->byCurGuestCount ;
	myRoom.byTotalGuestCount	= pmsg->bData2 ;

	SafeStrCpy(myRoom.title, pRoom->title, TITLE_SIZE) ;*/

	pRoom->byTotalGuestCount = pmsg->bData2 ;

	m_pMainDlg->Update_RoomInfo(pRoom) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : UpdateMyLastComTime
//	DESC : 플레이어가 실행한 마지막 명령 시간을 업데이트 한다.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UpdateMyLastComTime()
{
	m_dwLastMyComTime = gCurTime ; 
}





//-------------------------------------------------------------------------------------------------
//	NAME : IsEnableExecuteCom
//	DESC : 명령을 다시 실행할 시간이 지났는지 확인하는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
BYTE CChatRoomMgr::IsEnableExecuteCom()
{
	DWORD dwDelay = 0 ;
	dwDelay = gCurTime - m_dwLastMyComTime ;

	if(dwDelay >= CHATROOM_DELAY_TIME) return TRUE ;
	else return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : IsFreezed
//	DESC : 차단 된 유저인지 체크하는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
BOOL CChatRoomMgr::IsFreezed(DWORD dwPlayerID)
{
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		if(m_dwFreezedID[count] == dwPlayerID) return TRUE ;
	}

	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : FreezeUser
//	DESC : 차단 된 유저로 등록 시키는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::FreezeUser(DWORD dwPlayerID)
{
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		if(m_dwFreezedID[count] != 0) continue ;

		m_dwFreezedID[count] = dwPlayerID ;
		return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : UnFreezeUser
//	DESC : 차단 해제 시키는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::UnFreezeUser(DWORD dwPlayerID)
{
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		if(m_dwFreezedID[count] != dwPlayerID) continue ;

		m_dwFreezedID[count] = 0 ;
		return ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : ClearFreeze
//	DESC : 차단 된 유저를 모두 비우는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ClearFreeze()
{
	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
	{
		m_dwFreezedID[count] = 0 ;
	}
}





////-------------------------------------------------------------------------------------------------
////	NAME : DeleteFreezeUser
////	DESC : 선택된 유저 인덱스를 삭제한다.
////  DATE : APRIL 14, 2008 LYW
////-------------------------------------------------------------------------------------------------
//void CChatRoomMgr::DeleteFreezeUser(DWORD dwPlayerID)
//{
//	for(BYTE count = 0 ; count < MAX_USER_PER_ROOM ; ++count)
//	{
//		if(m_dwFreezedID[count] != dwPlayerID) continue ;
//
//		m_dwFreezedID[count] = 0 ;
//
//		return ;
//	}
//}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : 에러 메시지를 처리하는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error(BYTE errType, char* szCaption)
{
	// 함수 파라메터 체크.
	ASSERT(szCaption) ;

	if(!szCaption || strlen(szCaption) <= 1)
	{
#ifdef _MB_CRMGR_
		MessageBox( NULL, "Invalid err message!!", __FUNCTION__, MB_OK) ;
#endif //_MB_CRMGR_	
		return ;
	}

	// 에러 타입 확인.
	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempErr[128] = {0, } ;
	//switch(errType)
	//{
	//case err_IMP :		SafeStrCpy(tempErr, "Invalid a message parameter!!", 128) ;					break ;
	//case err_ICCTLS :	SafeStrCpy(tempErr, "Invalid child controls!!", 128) ;						break ;
	//case err_FCMTB :	SafeStrCpy(tempErr, "Failed to convert a message to base!!", 128) ;			break ;
	//case err_FCMTO :	SafeStrCpy(tempErr, "Failed to convert a message to original!!", 128) ;		break ;
	//default : break ;
	//}

	char tempErr[257] = {0, } ;
	switch(errType)
	{
	case err_IMP :		SafeStrCpy(tempErr, "Invalid a message parameter!!", 256) ;					break ;
	case err_ICCTLS :	SafeStrCpy(tempErr, "Invalid child controls!!", 256) ;						break ;
	case err_FCMTB :	SafeStrCpy(tempErr, "Failed to convert a message to base!!", 256) ;			break ;
	case err_FCMTO :	SafeStrCpy(tempErr, "Failed to convert a message to original!!", 256) ;		break ;
	default : break ;
	}

	// 에러 출력.
#ifdef _MB_CRMGR_
	MessageBox( NULL, tempErr, szCaption, MB_OK) ;
#endif //_MB_CRMGR_	
}





//-------------------------------------------------------------------------------------------------
//	NAME : Throw_Error
//	DESC : 에러 메시지를 처리하는 함수.
//  DATE : APRIL 14, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::Throw_Error(char* szErr, char* szCaption)
{
	// 함수 파라메터 체크.
	ASSERT(szCaption) ;

	if(!szErr || strlen(szErr) <= 1)
	{
#ifdef _MB_CRMGR_
		MessageBox( NULL, "Invalid err message!!", __FUNCTION__, MB_OK) ;
#endif //_MB_CRMGR_	
		return ;
	}

	if(!szCaption || strlen(szCaption) <= 1)
	{
#ifdef _MB_CRMGR_
		MessageBox( NULL, "Invalid caption!!", __FUNCTION__, MB_OK) ;
#endif //_MB_CRMGR_	
	}

	// 에러 출력.
#ifdef _MB_CRMGR_
	MessageBox( NULL, szErr, szCaption, MB_OK) ;
#endif //_MB_CRMGR_	
}





//-------------------------------------------------------------------------------------------------
//	NAME : SaveChat
//	DESC : 맵 이동 등, 필요할 때, 채팅방의 채팅 내용을 백업한다.
//  DATE : APRIL 11, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::SaveChat()
{
	// HERO가 채팅중인지 체크.
	if(Get_ChatRoomState() == e_State_In_Lobby) return ;

	// 리스트 다이얼로그 받기.
	cListDialog* pListDlg = NULL ;
	pListDlg = m_pChatRoomDlg->GetListDlg() ;

	ASSERT(pListDlg) ;

	if(!pListDlg)
	{
#ifdef _MB_CRMGR_
		MessageBox( NULL, "ERR-FRLD", "CRMGR-SC", MB_OK) ;
#endif //_MB_CRMGR_
		return ;
	}

	// 포인터 리스트 받기.
	cPtrList* pList = NULL ;
	pList = pListDlg->GetListItem() ;

	ASSERT(pList) ;

	if(!pList)
	{
#ifdef _MB_CRMGR_
		MessageBox( NULL, "ERR-FRPL", "CRMGR-SC", MB_OK) ;
#endif //_MB_CRMGR_
		return ;
	}

	// 채팅내용 저장.
	PTRLISTPOS pos = NULL ;
	pos = pList->GetHeadPosition() ;

	if(!pos) return ;

	while(pos)
	{
		ITEM* pItem = new ITEM ;
		*pItem = *(ITEM*)pList->GetNext( pos ) ;
		m_BackupList.AddTail( pItem ) ;
	}
}





//-------------------------------------------------------------------------------------------------
//	NAME : RestoreChat
//	DESC : 백업 된 채팅 내용을 복원한다.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::RestoreChat()
{
	if(!m_pChatRoomDlg || !m_pGuestListDlg || !m_pOptionDlg)
	{
		return ;
	}

	// HERO가 채팅중인지 체크.
	if(Get_ChatRoomState() != e_State_In_ChatRoom) return ;

	// 채팅방 옵션 적용.
	//if(!m_pOptionDlg->SetOption(&m_MyRoom))
	//{
	//	Throw_Error("Faild to setting option!!", __FUNCTION__) ;
	//	return ;
	//}
	SetOption(&m_MyRoom) ;

	if(m_pOptionDlg->IsActive()) m_pOptionDlg->SetActive(FALSE) ;

	if(m_pGuestListDlg->IsActive()) m_pGuestListDlg->SetActive(FALSE) ;

	// 리스트 다이얼로그 받기.
	cListDialog* pListDlg = NULL ;
	pListDlg = m_pChatRoomDlg->GetListDlg() ;

	ASSERT(pListDlg) ;

	if(!pListDlg)
	{
		Throw_Error("Failtd to receive a list dialog!!", __FUNCTION__) ;
		return ;
	}

	// 백업 리스트 포지션 받기.
	PTRLISTPOS pos = NULL ;
	pos = m_BackupList.GetHeadPosition() ;

	if(!pos) return ;

	// 채팅 내용 복원.
	ITEM* pItem = NULL ;
	while( pos )
	{
		pItem = (ITEM*)m_BackupList.GetNext( pos ) ;

		if( pItem )
		{
			pListDlg->AddItem( pItem->string, pItem->rgb, pItem->line ) ;
		}
	}

	ST_CR_ROOM_CLT myRoom ;
	memset(&myRoom, 0, sizeof(ST_CR_ROOM_CLT)) ;

	myRoom.byIdx				= m_MyRoom.byIdx ;
	myRoom.bySecretMode			= m_MyRoom.bySecretMode ;
	myRoom.byRoomType			= m_MyRoom.byRoomType ;
	myRoom.byCurGuestCount		= m_MyRoom.byCurGuestCount ;
	myRoom.byTotalGuestCount	= m_MyRoom.byTotalGuestCount ;

	SafeStrCpy(myRoom.title, m_MyRoom.title, TITLE_SIZE) ;

	m_pChatRoomDlg->SetRoomTitle(&myRoom) ;
	m_pChatRoomDlg->SetActive(TRUE) ;

	m_pGuestListDlg->SetClassMark() ;

	// 백업 리스트 해제.
	ReleaseBackupList() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : RestoreChat
//	DESC : 백업을 한 리스트를 해제한다.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::ReleaseBackupList()
{
	// 백업 리스트 받기.
	PTRLISTPOS pos = NULL ;
	pos = m_BackupList.GetHeadPosition() ;

	if(!pos) return ;

	// 백업 리스트 해제.
	ITEM* pItem = NULL ;
	while( pos )
	{
		pItem = (ITEM*)m_BackupList.GetNext( pos ) ;

		if( pItem ) delete pItem ;
	}

	m_BackupList.RemoveAll() ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : AddChatRoomMsg
//	DESC : 채팅방 창에 메시지를 추가한다.
//  DATE : APRIL 10, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::AddChatRoomMsg(char* pMsg, BYTE byType)
{
	// 함수 파라메터 체크.
	ASSERT(pMsg) ;
	ASSERT(strlen(pMsg) > 2) ;

	if(!pMsg)
	{
		Throw_Error(err_IMP, __FUNCTION__) ;
		return ;
	}

	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;

	if(!m_pChatRoomDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return ;
	}

	// 채팅방 창에 메시지를 추가한다.
	m_pChatRoomDlg->AddMsg(pMsg, byType) ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : GetChatRoomTextArea
//	DESC : 채팅방의 입력 컨트롤을 반환하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
//cTextArea* CChatRoomMgr::GetChatRoomTextArea()
cEditBox* CChatRoomMgr::GetChatRoomInputBox()
{
	// 하위 컨트롤 체크.
	ASSERT(m_pChatRoomDlg) ;

	if(!m_pChatRoomDlg)
	{
		Throw_Error(err_ICCTLS, __FUNCTION__) ;
		return NULL ;
	}

	// TextArea 받기.
	//cTextArea* pCtrl = NULL ;
	cEditBox* pCtrl = NULL ;
	pCtrl = m_pChatRoomDlg->GetInputBox() ;

	ASSERT(pCtrl) ;

	if(!pCtrl)
	{
		Throw_Error("Failed to receive textarea!!", __FUNCTION__) ;
		return NULL ;
	}

	return pCtrl ;
}





//-------------------------------------------------------------------------------------------------
//	NAME : MsgProc
//	DESC : 채팅방에서 발생하는 메시지 전송 처리를 하는 함수.
//  DATE : APRIL 16, 2008 LYW
//-------------------------------------------------------------------------------------------------
void CChatRoomMgr::MsgProc()
{
	// TextArea 받기.
	//cTextArea* pCtrl = NULL ;
	//pCtrl = GetChatRoomTextArea() ;
	cEditBox* pCtrl = NULL ;
	pCtrl = GetChatRoomInputBox() ;

	ASSERT(pCtrl) ;

	if(!pCtrl)
	{
		Throw_Error("Failed to receive textarea!!", __FUNCTION__) ;
		return ;
	}

	// 080704 LYW --- ChatRoomMgr : 임시 버퍼 사이즈 확장.
	//char tempMsg[MAX_CHAT_LENGTH+1] = {0, } ;
	char tempMsg[257] = {0, } ;
	//pCtrl->GetScriptText(tempMsg) ;
	//SafeStrCpy(tempMsg, pCtrl->GetEditText(), MAX_CHAT_LENGTH) ;
	SafeStrCpy(tempMsg, pCtrl->GetEditText(), 256) ;

	if(strlen(tempMsg) == 0) return ;

	// 필터링 처리.
	if( FILTERTABLE->FilterChat(tempMsg) )
	{
		m_pChatRoomDlg->AddMsg(CHATMGR->GetChatMsg(1225), e_ChatMsg_System) ;
		//pCtrl->SetScriptText("") ;
		pCtrl->SetEditText("") ;
		return;
	}

	if( gCurTime - m_dwLastMyChat < 1000 )	// 1초
	{
		AddChatRoomMsg(CHATMGR->GetChatMsg(248), e_ChatMsg_System) ;
		pCtrl->SetEditText("") ;
		return ;
	}
	else
	{
		// 마지막 채팅 시간 업데이트.
		UpdateChatLastTime(gCurTime) ;

		// 메시지 전송 처리.
		MSG_CR_MSG_BROADCAST msg ;
		memset(&msg, 0, sizeof(MSG_CR_MSG_BROADCAST)) ;

		msg.Category	= MP_CHATROOM ;
		msg.Protocol	= MP_CHATROOM_CHATMSG_NORMAL_SYN ;
		msg.dwObjectID	= HEROID ;

		SafeStrCpy(msg.name, HERO->GetObjectName(), MAX_NAME_LENGTH + 1) ;
		SafeStrCpy(msg.Msg, tempMsg, MAX_CHAT_LENGTH+1) ;

		BYTE byCount = 0 ;
		byCount = m_MyRoom.byCurGuestCount ;

		for(BYTE count = 0 ; count < byCount ; ++count)
		{
			msg.dwUser[count] = m_MyRoom.dwUser[count] ;

			++msg.byCount ;
		}

		NETWORK->Send( &msg, sizeof(MSG_CR_MSG_BROADCAST)) ;

		pCtrl->SetEditText("") ;
	}
}