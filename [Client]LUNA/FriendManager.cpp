// FriendManager.cpp: implementation of the CFriendManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FriendManager.h"
#include "ObjectManager.h"
#include "Gamein.h"
#include "ChatManager.h"
#include ".\interface\cWindowManager.h"
#include "WindowIDEnum.h"
#include "cMsgBox.h"
// 061220 LYW --- Delete this code.
//#include "MainBarDialog.h"

#include "FriendDialog.h"

// 080422 LYW --- FriendManager : Include chatroom manager.
#include "./ChatRoomMgr.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CFriendManager)

BOOL FriendFunc(char* cheat)
{
	char code[256];
	sscanf(cheat,"%s",code);
	strupr(code);
	if(strcmp(code,"ADDFRIEND")==0)
	{
		char Name[MAX_NAME_LENGTH+1];
		sscanf(cheat,"%s %s",code,&Name);

		MSG_NAME msg;
		msg.Category = MP_FRIEND;
		msg.Protocol = MP_FRIEND_ADD_SYN;
		strcpy(msg.Name, Name);
		msg.dwObjectID = HEROID;
		
		NETWORK->Send(&msg, sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"DELFRIEND")==0)
	{
		char Name[MAX_NAME_LENGTH+1];
		sscanf(cheat,"%s %s",code,&Name);

		MSG_NAME msg;
		msg.Category = MP_FRIEND;
		msg.Protocol = MP_FRIEND_DEL_SYN;
		strcpy(msg.Name, Name);
		msg.dwObjectID = HEROID;
		
		NETWORK->Send(&msg, sizeof(msg));
		return TRUE;
	}
	else if(strcmp(code,"LISTFRIEND")==0)
	{
		return TRUE;
	}

	return FALSE;
}

CFriendManager::CFriendManager()
{
}

void CFriendManager::Init()
{
	m_RequestPlayerID = 0;
	strcpy(m_RequestPlayerName, "");
	m_bFriendInit = FALSE;
	m_Friend.Init();
}

CFriendManager::~CFriendManager()
{
}

void CFriendManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol)
	{
	case MP_FRIEND_ADD_INVITE:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			if(m_Friend.GetFriendNum() == MAX_FRIEND_NUM)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(919), pmsg->Name );
			if(GetRequestPlayerID() == 0)
			{
				SetRequestPlayer(pmsg->PlayerID, pmsg->Name);
				WINDOWMGR->MsgBox( MBI_FRIEND_ADD, MBT_YESNO, CHATMGR->GetChatMsg(915), pmsg->Name);
			}
		}
		break;
	case MP_FRIEND_ADD_ACK:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID * pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(916), pmsg->Name );
			m_Friend.Add(pmsg->PlayerID, pmsg->Name);
			GAMEIN->GetFriendDialog()->SetFriendList(1);

			// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
			if(CHATROOMMGR->IsRequestedFriend())
			{
				char tempMsg[128] = {0, } ;
				sprintf(tempMsg, CHATMGR->GetChatMsg(916), pmsg->Name) ;
				CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
			}
				
			//FriendListSyn(1);
		}
		break;
	case MP_FRIEND_DEL_ACK:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
			m_Friend.Delete(pmsg->dwData);
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(922), pmsg->Name );
			GAMEIN->GetFriendDialog()->SetFriendList(1);

			//BYTE page = GAMEIN->GetFriendDialog()->GetSelectedFriendPge();
			//FriendListSyn(page);
		}
		break;
/*	case MP_FRIEND_SHOW_LIST_ACK: //채팅창에서
		{
			char islogin[MAX_NAME_LENGTH+1];
			MSG_FRIEND_LIST* pmsg = (MSG_FRIEND_LIST*)pMsg;
			for(int i=0; i<pmsg->count; ++i)
			{
				if(pmsg->Friend[i].IsLoggIn == TRUE)
					strcpy(islogin, CHATMGR->GetChatMsg(931) );
				else
					strcpy(islogin, CHATMGR->GetChatMsg(932) );

				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(924), pmsg->Friend[i].Name, islogin );
			}
			
		}
		break;
*/	case MP_FRIEND_LOGIN_NOTIFY:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
		//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(925), pmsg->Name);
			m_Friend.SetLogin(pmsg->dwData, TRUE);
			if(GAMEIN->GetFriendDialog()->IsActive() == TRUE)
			{
				int page = GAMEIN->GetFriendDialog()->GetSelectedFriendPge();
				GAMEIN->GetFriendDialog()->SetFriendList(page);
				//FriendListSyn(1);
			}			
			
		}
		break;
	case MP_FRIEND_LOGOUT_NOTIFY_TO_CLIENT:
		{
			MSG_NAME_DWORD* pmsg = (MSG_NAME_DWORD*)pMsg;
		//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(926), pmsg->Name );
			m_Friend.SetLogin(pmsg->dwData, FALSE);
			if(GAMEIN->GetFriendDialog()->IsActive() == TRUE)
			{
				BYTE page = GAMEIN->GetFriendDialog()->GetSelectedFriendPge();
				GAMEIN->GetFriendDialog()->SetFriendList(page);
				//	FriendListSyn(page);
			}				
		}
		break;
	case MP_FRIEND_LIST_ACK: //다이얼로그에서 
		{
			MSG_FRIEND_LIST_DLG* pmsg = (MSG_FRIEND_LIST_DLG*)pMsg;
			m_Friend.SetInfo(pmsg->FriendList,pmsg->count);
			SetFriendInit(TRUE);

			GAMEIN->GetFriendDialog()->SetFriendList(1);
			GAMEIN->GetFriendDialog()->SetActive(TRUE);
		}
		break;
	case MP_FRIEND_DELID_ACK:
		{
			MSG_DWORD_WORD *pmsg = (MSG_DWORD_WORD*)pMsg;
			// 100223 ONS 친구삭제시 메세지 출력
			const FRIEND* pInfo = m_Friend.GetInfo(pmsg->dwData);
			if( pInfo )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(922), pInfo->Name );
			}
			m_Friend.Delete(pmsg->dwData);
			if(pmsg->wData == TRUE)
			{
				WORD page = GAMEIN->GetFriendDialog()->GetSelectedFriendPge();
			//	FriendListSyn(page);
				GAMEIN->GetFriendDialog()->SetFriendList(page);
			}				
		}
		break;
	case MP_FRIEND_ADDID_ACK:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(916), pmsg->Name );

			// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
			if(CHATROOMMGR->IsRequestedFriend())
			{
				char tempMsg[128] = {0, } ;
				sprintf(tempMsg, CHATMGR->GetChatMsg(916), pmsg->Name) ;
				CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
			}
		}
		break;
	case MP_FRIEND_ADD_ACCEPT_ACK:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(916), pmsg->Name);
			m_Friend.Add(pmsg->PlayerID, pmsg->Name);
			
			if(GAMEIN->GetFriendDialog()->IsActive() == TRUE)
			{
				GAMEIN->GetFriendDialog()->SetFriendList(1);
				//FriendListSyn(1);
			}

			// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
			if(CHATROOMMGR->IsRequestedFriend())
			{
				char tempMsg[128] = {0, } ;
				sprintf(tempMsg, CHATMGR->GetChatMsg(916), pmsg->Name) ;
				CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
			}
		}
		break;
	case MP_FRIEND_ADD_ACCEPT_NACK:
		{
			// 100305 ONS 허락여부를 묻지않고 친구를 추가하도록 수정.
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			if(pmsg->PlayerID == eFriend_AddInvalidUser)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(918), pmsg->Name );
			else if(pmsg->PlayerID == eFriend_AddFullList)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(919), pmsg->Name);
			else if(pmsg->PlayerID == eFriend_AddDeny)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(917), pmsg->Name);
			else if(pmsg->PlayerID == eFriend_NotLogin)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(920), pmsg->Name);
			else if(pmsg->PlayerID == eFriend_AlreadyRegist)
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(921), pmsg->Name);

			// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
			if(CHATROOMMGR->IsRequestedFriend())
			{
				// 100311 ONS 채팅방 대화상대에서의 친구추가 메세지관련 변경
				char buffer[256] = {0};
				if(pmsg->PlayerID == eFriend_AddInvalidUser)
					sprintf(buffer, CHATMGR->GetChatMsg(918), pmsg->Name);
				else if(pmsg->PlayerID == eFriend_AddFullList)
					sprintf(buffer, CHATMGR->GetChatMsg(919), pmsg->Name);
				else if(pmsg->PlayerID == eFriend_AddDeny)
					sprintf(buffer, CHATMGR->GetChatMsg(917), pmsg->Name);
				else if(pmsg->PlayerID == eFriend_NotLogin)
					sprintf(buffer, CHATMGR->GetChatMsg(920), pmsg->Name);
				else if(pmsg->PlayerID == eFriend_AlreadyRegist)
					sprintf(buffer, CHATMGR->GetChatMsg(921), pmsg->Name);
				else
					sprintf(buffer, CHATMGR->GetChatMsg(927));

				CHATROOMMGR->AddChatRoomMsg(buffer, e_ChatMsg_System) ;
			}
		}
		break;
	case MP_FRIEND_ADD_NACK:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			if(pmsg->PlayerID == eFriend_AddDeny) //거절
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(917), pmsg->Name );

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(917), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
			else if(pmsg->PlayerID == eFriend_AddInvalidUser) //없는 유저
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(918), pmsg->Name );

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(918), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
			else if(pmsg->PlayerID == eFriend_AddFullList) //리스트에 다 찼다. max = 20
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(919), pmsg->Name );

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(919), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
			else if(pmsg->PlayerID == eFriend_NotLogin) 
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(920), pmsg->Name );

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(920), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
			else if(pmsg->PlayerID == eFriend_OptionNoFriend)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(929));

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(929), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
			else if(pmsg->PlayerID == eFriend_AlreadyRegist)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(921), pmsg->Name );

				// 080420 LYW --- FriendManager : 친구요청 실패 메시지 출력.
				if(CHATROOMMGR->IsRequestedFriend())
				{
					char tempMsg[128] = {0, } ;
					sprintf(tempMsg, CHATMGR->GetChatMsg(921), pmsg->Name) ;
					CHATROOMMGR->AddChatRoomMsg(tempMsg, e_ChatMsg_System) ;
				}
			}
		}
		break;
	case MP_FRIEND_DEL_NACK:
		{
			MSG_FRIEND_MEMBER_ADDDELETEID* pmsg = (MSG_FRIEND_MEMBER_ADDDELETEID*)pMsg;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(923), pmsg->Name );
		}
		break;

	}
}

void CFriendManager::AddDelFriendSyn(char* pFriendName, BYTE Protocol)
{
	if(m_Friend.IsInFriendList(pFriendName) == TRUE)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(921), pFriendName);
		return;
	}
	//else if(m_Friend.GetFriendNum() == MAX_FRIEND_NUM)
	//{
	//	// 100305 ONS 리스트 최대인원수 초과시
	//	CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(919), pFriendName );
	//	return;
	//}

	MSG_NAME msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = Protocol; //MP_FRIEND_ADD_SYN or MP_FRIEND_DEL_SYN
	//strcpy(msg.Name, pFriendName);
	SafeStrCpy(msg.Name, pFriendName, MAX_NAME_LENGTH+1);
	msg.dwObjectID = HEROID;
	
	NETWORK->Send(&msg, sizeof(msg));

}

void CFriendManager::FriendInviteAccept()
{
	MSG_DWORD msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_ADD_ACCEPT;
	msg.dwData = GetRequestPlayerID();
	msg.dwObjectID = HEROID;
		
	NETWORK->Send(&msg, sizeof(msg));

	SetRequestPlayer(0, "");
}

void CFriendManager::FriendInviteDeny()
{
	MSG_FRIEND_MEMBER_ADDDELETEID msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_ADD_DENY;
	msg.PlayerID = GetRequestPlayerID();
	msg.dwObjectID = HEROID;
	strcpy(msg.Name, HERO->GetObjectName());
	
	NETWORK->Send(&msg, sizeof(msg));

	SetRequestPlayer(0, "");
}

void CFriendManager::SetRequestPlayer(DWORD FromPlayerID, char* Name)
{
	m_RequestPlayerID = FromPlayerID;
	strcpy(m_RequestPlayerName, Name);
}

void CFriendManager::FriendListSyn(WORD SelectedPage)
{
	if(m_bFriendInit == TRUE)
	{
		GAMEIN->GetFriendDialog()->SetActive(TRUE);
		GAMEIN->GetFriendDialog()->SetFriendList(1);
		return;
	}
	
	MSG_WORD msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_LIST_SYN;
	msg.wData = SelectedPage;
	msg.dwObjectID = HEROID;

	NETWORK->Send(&msg, sizeof(msg));
	GAMEIN->GetFriendDialog()->SetSelectedFriendPge(SelectedPage);

//	GAMEIN->GetFriendDialog()->SetActive(TRUE);
}

void CFriendManager::DelFriendSynbyID(DWORD FriendID,BOOL bLast)
{
	if(m_Friend.IsInFriendList(FriendID) == FALSE)
	{
		ASSERT(0);
		return;
	}
	MSG_DWORD2 msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_DELID_SYN;
	msg.dwObjectID = HEROID;
	msg.dwData1 = FriendID;
	msg.dwData2 = bLast;

	NETWORK->Send(&msg, sizeof(msg));
}

void CFriendManager::AddFriendSynbyID(DWORD TargetID, char* TargetName, char* FromName)
{
	if(m_Friend.IsInFriendList(TargetID) == TRUE)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(921), TargetName);
		return;
	}
	MSG_FRIEND_MEMBER_ADDDELETEID msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_ADDID_SYN;
	msg.dwObjectID = HEROID; //FROM PLAYER
	msg.PlayerID = TargetID; //TO PLAYER
	SafeStrCpy(msg.Name, FromName, MAX_NAME_LENGTH+1);

	NETWORK->Send(&msg, sizeof(msg));
}


void CFriendManager::LogInNotify()
{
	MSGBASE msg;
	msg.Category = MP_FRIEND;
	msg.Protocol = MP_FRIEND_LOGIN;
	msg.dwObjectID = HEROID;

	NETWORK->Send(&msg, sizeof(msg));
}

void CFriendManager::GetFriendListInfo(FRIEND_LIST* rtInfo, int pagenum)
{
	m_Friend.GetFriendListInfo(rtInfo, pagenum);
}