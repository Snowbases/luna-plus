#include "stdafx.h"
#include ".\ChatManager.h"
#include "MHFile.h"
#include "Interface/cResourceDef.h"
#include "ChattingDlg.h"

#ifdef _CHEATENABLE_			
#include "CheatMsgParser.h"
#endif	//_CHEATENABLE_		

#include "ChatManager.h"
#include "MacroManager.h"
#include "ObjectManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "GameIn.h"
#include "GameResourceManager.h"
#include "cIMEex.h"
#include "cMsgBox.h"
#include "Input/cIMEWnd.h"

#include "Interface/cWindowManager.h"
#include "Interface/cEditBox.h"
#include "Interface/cListDialog.h"
#include "Interface/cTextArea.h"

// 070106 LYW --- Include option manager.
#include "OptionManager.h"
#include "FilteringTable.h"


#ifdef _GMTOOL_
#include "GMToolManager.h"
#include "MainGame.h"
#endif

#include "DateMatchingDlg.h"
#include "cMonsterSpeechManager.h"
#include "GMNotifyManager.h"
#include "ChatRoomMgr.h"
#include "ChatRoomDlg.h"
#include "MHTimeManager.h"

CChatManager::CChatManager(void)
{
	ZeroMemory(
		&m_ChatOption,
		sizeof(m_ChatOption));
	m_dwLastChatTime = 0;
	m_ForbidChatTime = 0;

	InitializeWhisperName();
	LoadChatMsg();
	LoadFaceCommandList();
	LoadActionCommandList();
}


CChatManager::~CChatManager(void)
{
	PTRLISTPOS pos = m_FaceCommandList.GetHeadPosition() ;									// 얼굴 교체 명령어 리스트에서 위치 포지션을 받아온다.

	for(sFACEINFO* pFaceInfo = (sFACEINFO*)m_FaceCommandList.GetNext( pos );
		0 < pFaceInfo;
		pFaceInfo = (sFACEINFO*)m_FaceCommandList.GetNext( pos ))
	{
		SAFE_DELETE( pFaceInfo );
	}

	m_FaceCommandList.RemoveAll() ;															// 얼굴 변경 명령 리스트를 모두 비운다.

	sACTION_COMMAND* pDeleteCommand ;

	PTRLISTPOS deletePos = NULL ;
	deletePos = m_ActionCommandList.GetHeadPosition() ;

	while( deletePos )																	// 파일의 끝까지 while을 돌린다.
	{
		pDeleteCommand = NULL ;
		pDeleteCommand = (sACTION_COMMAND*)m_ActionCommandList.GetNext(deletePos) ;

		if( !pDeleteCommand ) continue ;

		m_ActionCommandList.Remove(pDeleteCommand) ;

		delete pDeleteCommand ;
	}

	m_ActionCommandList.RemoveAll() ;
}

void	CChatManager::LoadChatMsg()
{
	CMHFile fp;
	fp.Init("Data/Interface/Windows/SystemMsg.bin", "rb" );
	char msg[MAX_PATH] = {0};

	while(FALSE == fp.IsEOF())
	{
		const DWORD nIndex = fp.GetDword();
		fp.GetStringInQuotation(msg);

		if(0 < _tcslen(msg))
		{
			mTextContainer[nIndex] = msg;
			fp.GetLine(
				msg,
				_countof(msg));
		}
	}
}


char*	CChatManager::GetChatMsg( int nMsgNum )
{
	const TextContainer::iterator iterator = mTextContainer.find(
		nMsgNum);

	return mTextContainer.end() == iterator ? "" : iterator->second.c_str();
}

void CChatManager::AddMsg(eTextClass nClass, LPCTSTR str, ...)
{
	if(0 == m_pChattingDlg)
	{
		return;
	}

	char msg[MAX_PATH] = {0};
	va_list argList;
	va_start(
		argList,
		str);
	vsprintf(
		msg,
		str,
		argList);
	va_end(
		argList);

	switch(nClass)
	{
	case CTC_SHOUT:
		{
			m_pChattingDlg->AddShoutMsg(
				msg,
				GetMsgColor(nClass));
			break;
		}
	case CTC_OPERATOR:
		{
			m_pChattingDlg->AddMsg(CTC_OPERATOR, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			NOTIFYMGR->AddMsg(msg);
			break;
		}
	case CTC_OPERATOR2:
		{
			NOTIFYMGR->AddMsg(msg);
			break;
		}
	case CTC_BILLING:
		{
			m_pChattingDlg->AddMsg(CTC_BILLING, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			NOTIFYMGR->AddMsg(msg, eNTC_REMAINTIME);
			break;
		}
	case CTC_GMCHAT:
		{
			m_pChattingDlg->AddMsg(CTC_GMCHAT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_GENERALCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOCHATMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GENERALCHAT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_PARTYCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOPARTYMSG])
			{
				m_pChattingDlg->AddMsg(CTC_PARTYCHAT, e_CONVERSATION_PARTY, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_FAMILY:
		{
			if(!m_ChatOption.bOption[CTO_NOFAMILYMSG])
			{
				m_pChattingDlg->AddMsg(CTC_FAMILY, e_CONVERSATION_FAMILY, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_GUILDCHAT:
		{
			if(!m_ChatOption.bOption[CTO_NOGUILDMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GUILDCHAT, e_CONVERSATION_GUILD, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_ALLIANCE:
		{
			if(!m_ChatOption.bOption[CTO_NOALLIANCEMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ALLIANCE, e_CONVERSATION_ALLIANCE, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_TRADE:
		{
			if(!m_ChatOption.bOption[CTO_NOSHOUTMSG])
			{
				m_pChattingDlg->AddMsg(CTC_TRADE, e_CONVERSATION_TRADE, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_SYSMSG:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_WHISPER:
		{
			m_pChattingDlg->AddMsg(CTC_WHISPER, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;

		}
	case CTC_ATTACK:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ATTACK, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_ATTACKED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_ATTACKED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_DEFENCE:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_DEFENCE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_DEFENCED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_DEFENCED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));	
			}

			break;
		}
	case CTC_KILLED:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_KILLED, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_MPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_MPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_HPWARNING:
		{
			if(!m_ChatOption.bOption[CTO_NOSYSMSG])
			{
				m_pChattingDlg->AddMsg(CTC_HPWARNING, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_GETITEM:
		{
			if(!m_ChatOption.bOption[CTO_NOITEMMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GETITEM, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}


			break;
		}
	case CTC_GETEXP:
		{
			if(!m_ChatOption.bOption[CTO_NOEXPMSG])
			{
				m_pChattingDlg->AddMsg(CTC_GETEXP,e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			}

			break;
		}
	case CTC_TOWHOLE:
		{
			m_pChattingDlg->AddMsg(CTC_TOWHOLE, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOPARTY:
		{
			m_pChattingDlg->AddMsg(CTC_TOPARTY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOFAMILY:
		{
			m_pChattingDlg->AddMsg(CTC_TOFAMILY, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_TOGUILD:
		{
			m_pChattingDlg->AddMsg(CTC_TOGUILD, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_MONSTERSPEECH:
		{
			m_pChattingDlg->AddMsg(CTC_MONSTERSPEECH, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_MONSTERSHOUT:
		{
			m_pChattingDlg->AddMsg(CTC_MONSTERSHOUT, e_CONVERSATION_ALL, msg, GetMsgColor(nClass));
			break;
		}
	case CTC_EXITCOUNT:
		{
			m_pChattingDlg->AddMsg(CTC_EXITCOUNT, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	default:
		{
			m_pChattingDlg->AddMsg(CTC_SYSMSG, e_CONVERSATION_SYSTEM, msg, GetMsgColor(nClass));
			break;
		}
	}
}

void CChatManager::LoadActionCommandList()
{
	CMHFile	fp;
	fp.Init(
		"Data/interface/Windows/Action_Command.bin",
		"rb");

	int nIndex = 0 ;
	char tempBuf[32] = {0, } ;
	BOOL bVehicleOpt = FALSE; 

	while( ! fp.IsEOF() )																	// 파일의 끝까지 while을 돌린다.
	{
		// 091029 ShinJS --- 주석처리 추가
		char buf[ MAX_PATH ] = {0,};

		fp.GetLine(buf, MAX_PATH);
		
		if( buf[0] == '/' && buf[1] == '/' )
		{
			fp.GetLineX( buf, MAX_PATH );
			continue;
		}
		
		memset(tempBuf, 0, sizeof(tempBuf)) ;
		sscanf(buf, "%d %s %d", &nIndex, tempBuf, &bVehicleOpt);

		if( strlen(tempBuf) <= 2 ) continue ;

		sACTION_COMMAND* pCommand = new sACTION_COMMAND ;									// 얼굴 정보 구조체 만큼 메모리를 할당하여 포인터로 넘긴다.

		pCommand->index = nIndex ;															// 인덱스를 읽는다.
		strcpy(pCommand->string, tempBuf) ;													// 스트링을 읽어 Descript에 복사한다.

		// 091022 ShinJS --- 탈것 탑승시 제한 여부
		pCommand->bInvalidOnVehicle = bVehicleOpt;

		m_ActionCommandList.AddTail(pCommand) ;												// 정보를 리스트에 추가한다.
	}
}

void CChatManager::LoadFaceCommandList()
{
	CMHFile	fp;
	fp.Init(
		"Data/interface/Windows/Chat_FaceList.bin",
		"rb");

	while( ! fp.IsEOF() )																	// 파일의 끝까지 while을 돌린다.
	{
		sFACEINFO* pFaceInfo = new sFACEINFO ;												// 얼굴 정보 구조체 만큼 메모리를 할당하여 포인터로 넘긴다.

		pFaceInfo->nIndex = fp.GetInt() ;													// 인덱스를 읽는다.
		strcpy(pFaceInfo->szDescript, fp.GetString()) ;										// 스트링을 읽어 Descript에 복사한다.
		strcpy(pFaceInfo->szCommand, fp.GetStringInQuotation()) ;							// 스트링을 읽어 Command에 복사한다.
		pFaceInfo->nFaceNum = fp.GetInt() ;													// 얼굴 번호을 읽는다.
		pFaceInfo->nLen = strlen(pFaceInfo->szCommand) ;									// 명령어의 길이를 구한다.

		m_FaceCommandList.AddTail(pFaceInfo) ;												// 정보를 리스트에 추가한다.
	}
}

int CChatManager::GetFaceNum( const TCHAR* str ) const
{
	// 문자를 하나씩 증가시켜서 해당 문자가 포함되었는지 찾는다.
	// 문자열에 멀티바이트 문자가 포함되어 있으므로, 증가시킬 때는 멀티바이트 단위로 증가시켜야 한다
	for(	const TCHAR* subString = str;
		*subString; 
		subString = _tcsinc( subString ) )
	{
		for(	PTRLISTPOS pos = m_FaceCommandList.GetHeadPosition();
			pos;
			)
		{
			const sFACEINFO* pFaceInfo = ( sFACEINFO* )m_FaceCommandList.GetNext( pos );
			ASSERT( pFaceInfo );

			if( ! _tcsncmp( subString, pFaceInfo->szCommand, _tcslen( pFaceInfo->szCommand ) ) )
			{
				return pFaceInfo->nFaceNum;
			}
		}
	}

	return -1;
}


void CChatManager::MsgProc( UINT msg, WPARAM wParam )
{
	if(		msg		== WM_KEYDOWN	&& 
		wParam	== VK_RETURN	&&
		!	CIMEWND->IsDocking() )
	{
		cMsgBox* pMsgBox = WINDOWMGR->GetFirstMsgBox();

		if( pMsgBox &&
			pMsgBox->PressDefaultBtn() )
		{
			return;
		}
	}

	if( ! m_pChattingDlg )
	{
		return;
	}

	cEditBox* pEdit = m_pChattingDlg->GetInputBox() ;
	if( pEdit == NULL ) return;

	// 채팅방의 텍스트 입력을 받는다.
	//cTextArea* pChatRoomCtrl = NULL ;
	//pChatRoomCtrl = CHATROOMMGR->GetChatRoomTextArea() ;
	cEditBox* pChatRoomCtrl = NULL ;
	pChatRoomCtrl = CHATROOMMGR->GetChatRoomInputBox() ;

	ASSERT(pChatRoomCtrl) ;

	if(!pChatRoomCtrl)
	{
		CHATROOMMGR->Throw_Error("Failed to receive chatroom textarea!!", __FUNCTION__) ;
		return ;
	}

	switch( msg )
	{
	case WM_CHAR:
	case WM_IME_CHAR:
	case WM_IME_COMPOSITION:
		{
			if( MACROMGR->IsChatMode()	&& !pEdit->IsFocus() && !CIMEWND->IsDocking() && pEdit->GetIME()->IsValidChar((unsigned char*)&wParam) && !pChatRoomCtrl->IsFocus())		
			{
				pEdit->SetFocusEdit( TRUE );
			}
		}
		break;

	case WM_KEYDOWN :
		{
			if( wParam == VK_RETURN )
			{
				// 080410 LYW --- ChatManager : Modified message proc.
				cEditBox*  pInputBox = GAMEIN->GetDateMatchingDlg()->GetChatingDlg()->GetEditBox() ;
				cTextArea* pTextArea = GAMEIN->GetDateMatchingDlg()->GetMyInfoDlg()->GetIntroduce() ;

				// 일반 채팅창 처리 체크.
				if (pInputBox->IsFocus())
				{
					char szText[256] = {0, } ;
					DWORD dwColor = RGB(0, 255, 255) ;

					sprintf(szText, "[%s]:%s", HERO->GetObjectName(), pInputBox->GetEditText()) ;		

					GAMEIN->GetDateMatchingDlg()->GetChatingDlg()->AddMsg(szText, dwColor);
					g_csResidentRegistManager.CLI_SendDateMatchingChatText(pInputBox->GetEditText()) ;
					pInputBox->SetEditText("");
				}
				// 채팅방 쪽 처리 체크.
				else if(pChatRoomCtrl->IsFocus())
				{
					CHATROOMMGR->MsgProc() ;
				}
				// 매칭 쪽 처리 체크.
				else if( ! pTextArea->IsFocus() )
				{
					if( !CIMEWND->IsDocking() )		//다른에디터에 포커스가 없다면.
					{	
						pEdit->SetFocusEdit( TRUE );
					}
					else
					{
						char* strChat = pEdit->GetEditText();

						if( *strChat )
						{
							OnInputChatMsg( strChat );
							pEdit->SetEditText("");
						}

						m_pChattingDlg->SetCommandToInputBox();

						pEdit->SetFocusEdit( FALSE );
					}
				}
			}		
		}
		break ;
	}
}

void CChatManager::OnInputChatMsg( char* str )
{
#ifdef _CHEATENABLE_
	if( !CHEATMGR->IsCheatEnable() )
#endif
		if( FILTERTABLE->FilterChat( str ) )
		{
			AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(1225) );
			return;
		}

		// 100618 ONS 서버시간과 채팅금지시간의 비교를 통해 채팅금지여부를 판단한다.
		// 클라이언트가 체크함으로써 서버에 패킷을 보내지 않도록 처리.
		if( m_ForbidChatTime > 0 )
		{
			__time64_t curTime = MHTIMEMGR->GetServerTime();
			if( curTime < m_ForbidChatTime )
			{
				ShowForbidChatTime( m_ForbidChatTime );
				return;
			}
			else
			{
				m_ForbidChatTime = 0;
			}
		}

		char nToken = *str;

		if( CanChatTime( nToken ) )
		{
			m_pChattingDlg->InspectMsg(str);
		}
}

BOOL CChatManager::CanChatTime( char cToken )
{
	if( gCurTime - m_dwLastChatTime < 1000 )	// 1초
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(248) );
		return FALSE;
	}

	m_dwLastChatTime = gCurTime;
	return TRUE;
}

void	CChatManager::NetworkMsgParse( BYTE Protocol, void* pMsg )
{
	switch(Protocol)
	{
	case MP_CHAT_ALL:
	case MP_CHAT_GM:			// 090731 ShinJS --- GM 채팅 추가
	case MP_CHAT_NORMAL :						Chat_Normal( pMsg ) ;										break ;
	case MP_CHAT_PARTY :						Chat_Party( pMsg ) ;										break ;
	case MP_CHAT_FAMILY :						Chat_Family( pMsg ) ;										break ;
	case MP_CHAT_GUILD :						Chat_Guild( pMsg ) ;										break ;
	case MP_CHAT_GUILDUNION :					Chat_Alliance( pMsg ) ;										break ;
	case MP_CHAT_TRADE :						Chat_Trade( pMsg ) ;										break ;
	case MP_CHAT_SYSTEM :						Chat_System( pMsg ) ;										break ;
	case MP_CHAT_WHOLE :						Chat_Whole( pMsg ) ;										break ;
	case MP_CHAT_WHISPER_GM:	// 090731 ShinJS --- GM 채팅 추가
	case MP_CHAT_WHISPER:						Chat_Whisper( pMsg ) ;										break ;
	case MP_CHAT_WHISPER_ACK:					Chat_Whisper_Ack( pMsg ) ;									break ;
	case MP_CHAT_WHISPER_NACK:					Chat_Whisper_Nack( pMsg ) ;									break ;
	case MP_CHAT_MONSTERSPEECH:					Chat_Monster_Speech( pMsg ) ;								break ;
	case MP_CHAT_SHOUT_ACK:						Chat_Shout_Ack(pMsg);										break;
	case MP_CHAT_SHOUT_NACK:					Chat_Shout_Nack(pMsg);										break;
//---KES CHEAT
	case MP_CHAT_FROMMONSTER_ALL:
		{
			TESTMSG* data = (TESTMSG*)pMsg;

			CObject* pObject = OBJECTMGR->GetObject(data->dwObjectID);
			if( !pObject ) return;

			OBJECTMGR->ShowChatBalloon( pObject, data->Msg );

		}
		break;
	case MP_CHAT_FORBIDCHAT:
		{
			stTime64t* pmsg = (stTime64t*)pMsg;
			m_ForbidChatTime = pmsg->time64t;
			ShowForbidChatTime( pmsg->time64t );
		}
		break;
	case MP_CHAT_PERMITCHAT:
		{
			m_ForbidChatTime = 0;
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(2313) );
		}
		break;
//------------
	}
}

void	CChatManager::Chat_Normal( void* pMsg )
{
	TESTMSG* const data = (TESTMSG*)pMsg;
	CObject* object = OBJECTMGR->GetObject(data->dwObjectID);

	if(0 == object)
	{
		return;
	}

	LPCTSTR keyword = _T("SystemMsg.bin\'");
	// 091019 LUJ, 문자열이 지정된 예약어로 시작되면 시스템 메시지로 간주한다
	const BOOL isSystemMessage = (0 == _tcsnicmp(data->Msg, keyword, _tcslen(keyword)));
	LPTSTR textMessage = textMessage = data->Msg;

	if(isSystemMessage)
	{
		LPCTSTR token = data->Msg + _tcslen(keyword);
		const DWORD messageIndex = _ttoi(token ? token : "");
		static TCHAR text[MAX_PATH];
		ZeroMemory(text, sizeof(text));
		_stprintf(
			text,
			"%s",
			GetChatMsg(messageIndex));
		textMessage = text;
	}

	AddMsg(
		CTC_GENERALCHAT,
		"[%s]: %s",
		object->GetObjectName(),
		textMessage);
	OBJECTMGR->ShowChatBalloon(
		OPTIONMGR->GetGameOption()->bNoBalloon ? 0 : object,
		textMessage);
}

void	CChatManager::Chat_Party( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_PARTYCHAT, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Family( void* pMsg )
{
	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.07.13
	// SEND_FAMILY_CHAT* data = (SEND_FAMILY_CHAT*)pMsg;
	MSG_CHAT* pstPacket = (MSG_CHAT*)pMsg;
	// E 패밀리 추가 added by hseos 2007.07.13

	AddMsg( CTC_FAMILY, "[%s]: %s", pstPacket->Name, pstPacket->Msg );
}
void	CChatManager::Chat_Guild( void* pMsg )
{
	// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
	// TESTMSGID -> MSG_GUILD_CHAT
	// pmsg->dwSenderID -> pmsg->dwObjectID
	MSG_GUILD_CHAT* pData = (MSG_GUILD_CHAT*)pMsg;
	char Sendername[MAX_NAME_LENGTH+1];
	SafeStrCpy(Sendername, GUILDMGR->GetMemberName(pData->dwSenderIdx), MAX_NAME_LENGTH+1);
	if(strcmp(Sendername, "")== 0)
		return;
	AddMsg( CTC_GUILDCHAT, "[%s]: %s", Sendername, pData->Msg );
}
void	CChatManager::Chat_Alliance( void* pMsg )
{
	// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경
	// MSG_CHAT_WITH_SENDERID -> MSG_GUILDUNION_CHAT
	MSG_GUILDUNION_CHAT* data = (MSG_GUILDUNION_CHAT*)pMsg;

	AddMsg( CTC_ALLIANCE, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Trade( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_TRADE, "[%s]: %s", data->Name, data->Msg );
}

void	CChatManager::Chat_System( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_SYSMSG, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Whole( void* pMsg )
{
	MSG_CHAT_WITH_SENDERID* data = (MSG_CHAT_WITH_SENDERID*)pMsg;

	AddMsg( CTC_WHOLE, "[%s]: %s", data->Name, data->Msg );
}
void	CChatManager::Chat_Whisper( void* pMsg )
{
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

	AddMsg( CTC_WHISPER, "From %s: %s", data->Name, data->Msg );
#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 2 );	//2:받았다.
#endif
	//m_pChattingDlg->AddReceiverName( data->Name ) ;
	AddWhisperName(data->Name) ;
}
void	CChatManager::Chat_Whisper_Ack( void* pMsg )
{
	MSG_CHAT* data = (MSG_CHAT*)pMsg;

#ifdef _GMTOOL_
	if( MAINGAME->GetUserLevel() == eUSERLEVEL_GM )
		AddMsg( CTC_GMCHAT, "To %s: %s", data->Name, data->Msg );
	else
		AddMsg( CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#else
	AddMsg( CTC_WHISPER, "To %s: %s", data->Name, data->Msg );
#endif

#ifdef _GMTOOL_
	GMTOOLMGR->AddChatMsg( data->Name, data->Msg, 1 );	//1:했다.
#endif
	//m_pChattingDlg->AddSenderName( data->Name ) ;

	AddWhisperName(data->Name) ;

	// 071223 LYW --- ChatManager : 귓말이 성공하면, 채팅 창에 귓말 입력 대기로 세팅한다.
	cEditBox* pInputBox = NULL ;
	pInputBox = m_pChattingDlg->GetInputBox() ;

	if( pInputBox )
	{
		char tempName[32] = {0, } ;
		sprintf(tempName, "/%s ", data->Name) ;
		pInputBox->SetEditText(tempName) ;
	}
}

void CChatManager::AddWhisperName(LPCTSTR name)
{
	if(4 > strlen(name))
	{
		return;
	}

	for(size_t count = 0 ; count < _countof(whisperName) ; ++count)
	{
		if(0 == strcmp(whisperName[count], name))
		{
			return;
		}
	}

	for(size_t count = 0; count < MAX_WNAME_COUNT - 1; ++count)
	{
		SafeStrCpy(
			whisperName[count],
			whisperName[count+1],
			_countof(whisperName[count]));
	}

	SafeStrCpy(
		whisperName[MAX_WNAME_COUNT - 1],
		name,
		_countof(whisperName[MAX_WNAME_COUNT - 1]));
}

char* CChatManager::GetWhisperName(size_t index)
{
	if(index >= _countof(whisperName))
	{
		return "";
	}

	return whisperName[index];
}

// 071001 LYW --- Add function to initialize whisper buffer.
void CChatManager::InitializeWhisperName()
{
	ZeroMemory(
		whisperName,
		sizeof(whisperName));
}


void	CChatManager::Chat_Whisper_Nack( void* pMsg )
{
	const MSG_BYTE* data = (MSG_BYTE*)pMsg;

	switch( data->bData )
	{
	case CHATERR_NO_NAME:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(210) );
			break;
		}
	case CHATERR_NOT_CONNECTED:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(374) );//confirm
			break;
		}
	case CHATERR_OPTION_NOWHISPER:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(201) );//confirm
			break;
		}
	default:
		{
			AddMsg( CTC_WHISPER, GetChatMsg(202) );//confirm
			break;
		}
	}
}


void	CChatManager::Chat_Monster_Speech( void* pMsg )
{
	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
	char* temp = MON_SPEECHMGR->GetMonsterSpeech(pmsg->dwData2);
	if( temp )
	{
		switch(pmsg->dwData1)
		{
		case 0:	//말풍선
			{
				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwObjectID);

				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}
			}
			break;
		case 1:	//말풍선 + 채팅창
			{
				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwObjectID);
				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}


				AddMsg( CTC_MONSTERSPEECH, "%s: %s", pMonster->GetSInfo()->Name, temp );
			}
			break;
		case 2:	//채널 외치기
			{
				BASE_MONSTER_LIST* pMonInfo = GAMERESRCMNGR->GetMonsterListInfo(pmsg->dwData4);
				if(pMonInfo) 
					AddMsg( CTC_MONSTERSHOUT, "%s: %s", pMonInfo->Name, temp );

				CMonster* pMonster = (CMonster*)OBJECTMGR->GetObject(pmsg->dwData3);
				if( pMonster )
				{
					OBJECTMGR->ShowChatBalloon( pMonster, temp);
				}
			}
			break;
		}
	}
}

void CChatManager::Chat_Shout_Ack(void*pMsg)
{
	MSG_CHAT_WORD* data = (MSG_CHAT_WORD*)pMsg;
	AddMsg(CTC_SHOUT, "%s", data->Msg);
}

void CChatManager::Chat_Shout_Nack(void*pMsg)
{
	AddMsg( CTC_WHISPER, GetChatMsg(1461));
}

void CChatManager::SaveChatList()
{
	cPtrList* pPtr = m_pChattingDlg->GetChatListDlg()->GetListItem() ;
	PTRLISTPOS pos = pPtr->GetHeadPosition() ;

	while(pos)
	{
		ITEM* pItem = new ITEM ;
		*pItem = *(ITEM*)pPtr->GetNext( pos ) ;
		m_ChatList.AddTail( pItem ) ;
	}

	cPtrList* pPtr2 = m_pChattingDlg->GetAssistListDlg()->GetListItem() ;
	PTRLISTPOS pos2 = pPtr2->GetHeadPosition();

	while( pos2 )
	{
		ITEM* pItem = new ITEM;
		*pItem = *(ITEM*)pPtr2->GetNext( pos2 );
		m_AssistList.AddTail( pItem );
	}

	cPtrList* pPtr3 = m_pChattingDlg->GetShoutListDlg()->GetListItem() ;
	PTRLISTPOS pos3 = pPtr3->GetHeadPosition();

	while( pos3 )
	{
		ITEM* pItem = new ITEM;
		*pItem = *(ITEM*)pPtr3->GetNext( pos3 );
		m_ShoutList.AddTail( pItem );
	}
}

void CChatManager::RestoreChatList()
{
	PTRLISTPOS pos = m_ChatList.GetHeadPosition() ;

	ITEM* pItem = NULL ;

	while( pos )
	{
		pItem = (ITEM*)m_ChatList.GetNext( pos ) ;

		if( pItem )
		{
			m_pChattingDlg->GetChatListDlg()->AddItem( pItem->string, pItem->rgb, pItem->line ) ;
		}
	}

	PTRLISTPOS pos2 = m_AssistList.GetHeadPosition() ;

	ITEM* pItem2 = NULL ;
	while( pos2 )
	{
		pItem2 = (ITEM*)m_AssistList.GetNext( pos2 ) ;

		if( pItem2 )
		{
			m_pChattingDlg->GetAssistListDlg()->AddItem( pItem2->string, pItem2->rgb, pItem2->line ) ;
		}
	}

	PTRLISTPOS pos3 = m_ShoutList.GetHeadPosition() ;

	ITEM* pItem3 = NULL ;
	while( pos3 )
	{
		pItem3 = (ITEM*)m_ShoutList.GetNext( pos3 ) ;

		if( pItem3 )
		{
			m_pChattingDlg->GetShoutListDlg()->AddItem( pItem3->string, pItem3->rgb, pItem3->line ) ;
		}
	}

	ReleaseChatList() ;
}

void CChatManager::ReleaseChatList()
{
	PTRLISTPOS pos = m_ChatList.GetHeadPosition();
	ITEM* pItem = 0;

	while( pos )
	{
		pItem = (ITEM*)m_ChatList.GetNext( pos ) ;

		if( pItem ) delete pItem ;
	}

	m_ChatList.RemoveAll() ;

	PTRLISTPOS pos2 = m_AssistList.GetHeadPosition() ;

	ITEM* pItem2 = NULL ;

	while( pos2 )
	{
		pItem2 = (ITEM*)m_AssistList.GetNext( pos2 ) ;

		if( pItem2 ) delete pItem2 ;
	}

	m_AssistList.RemoveAll() ;

	PTRLISTPOS pos3 = m_ShoutList.GetHeadPosition() ;

	ITEM* pItem3 = NULL ;

	while( pos3 )
	{
		pItem3 = (ITEM*)m_ShoutList.GetNext( pos3 ) ;

		if( pItem3 ) delete pItem3 ;
	}

	m_ShoutList.RemoveAll() ;
}

CChatManager* CChatManager::GetInstance()
{
	static CChatManager instance;

	return &instance;
}

void CChatManager::ShowForbidChatTime( __time64_t time )
{
	struct tm curTimeWhen = *_localtime64( &time );
	CHATMGR->AddMsg( CTC_SYSMSG,  CHATMGR->GetChatMsg(2312), 
		curTimeWhen.tm_year + 1900,
		curTimeWhen.tm_mon + 1,
		curTimeWhen.tm_mday,
		curTimeWhen.tm_hour,
		curTimeWhen.tm_min,
		curTimeWhen.tm_sec );
}