#include "stdafx.h"
#include "WindowIdEnum.h"
#include "ChattingDlg.h"
#include "Input/Keyboard.h"
#include "input/UserInput.h"
#include "./Interface/cButton.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cListDialog.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cStatic.h"
#include "../ConductInfo.h"
#include "..\hseos\Family\SHFamily.h"
#include "cMsgBox.h"

#include "FilteringTable.h"

#include "ChatManager.h"
#include "CheatMsgParser.h"
#include "OptionManager.h"
#include "ChatManager.h"
#include "PartyManager.h"
#include "GameResourceManager.h"
#include "ExchangeManager.h"
#include "StreetStallManager.h"
#include "ShowdownManager.h"
#include "MoveManager.h"
#include "ConductManager.h"
#include "ObjectManager.h"
#include "cWindowManager.h"

#include "ObjectStateManager.h"
#include "AppearanceManager.h"
#include "FriendManager.h"
#include "GameIn.h"
#include "MiniFriendDialog.h"

#include "ShoutDialog.h"
#include "FacialManager.h"
#include "../hseos/Farm/SHFarmManager.h"
#include "ChatRoomMgr.h"

CChattingDlg::CChattingDlg(void)
{
	m_pChatList		= NULL ;
	m_pAssistList	= NULL ;
	m_pShoutList	= NULL ;

	m_pInputBox		= NULL ;

	m_pExtendBtn	= NULL ;
	m_pReductionBtn	= NULL ;
	ZeroMemory(
		m_pTabButton,
		sizeof(m_pTabButton));

	m_type = WT_CHATTING ;
	m_byCurAssistance = e_CONVERSATION_ALL;
	m_byCurCommand = e_SMT_ALL;
	m_nNameIndex = 2;
	ZeroMemory(
		m_szWhisperName,
		sizeof(m_szWhisperName));
	ZeroMemory(
		m_szSendMsg,
		sizeof(m_szSendMsg));
}





//=========================================================================
//	NAME : ~CChattingDlg
//	DESC : The function destruct.
//=========================================================================
CChattingDlg::~CChattingDlg(void)
{
}





//=========================================================================
//	NAME : Linking
//	DESC : The function to link child controls.
//=========================================================================
void CChattingDlg::Linking()
{
 	m_pChatList		= (cListDialog*)GetWindowForID(CHAT_LIST) ;
	m_pAssistList	= (cListDialog*)GetWindowForID(CHAT_ASSIST_LIST) ;
	m_pAssistList->SetActive(FALSE) ;
	m_pShoutList	= (cListDialog*)GetWindowForID(CHAT_SHOUT_LIST) ;
	CallShoutDlg();

	m_pInputBox		= (cEditBox*)GetWindowForID(CHAT_EDITBOX) ;

	m_pExtendBtn	= (cButton*)GetWindowForID(CHAT_EXTEND_BTN) ;
	m_pReductionBtn	= (cButton*)GetWindowForID(CHAT_REDUCTION_BTN) ;
	m_pCloseBtn		= (cButton*)GetWindowForID(CHAT_CLOSE_BTN) ;
	m_pCloseBtn->SetActive(FALSE) ;

	for( BYTE count = 0 ; count < e_CONVERSATION_MAX ; ++count )
	{
		m_pTabButton[count] = (cPushupButton*)GetWindowForID(CHAT_BTN_ALL+count) ;
	}
	m_pTabButton[e_CONVERSATION_ALL]->SetPush(TRUE) ;
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetActive(FALSE) ;

	CONVERSATION btnIdx = CONVERSATION(OPTIONMGR->GetGameOption()->nCurChatMode & 0x7F);
	if (btnIdx < e_CONVERSATION_MAX)
	{
		for(int count = 0 ; count < e_CONVERSATION_MAX-1 ; ++count )
		{
			m_pTabButton[count]->SetPush(FALSE) ;
		}

		m_pTabButton[btnIdx]->SetPush(TRUE) ;

		SetCommandToInputBox() ;
		m_pInputBox->SetFocusEdit(FALSE) ;

		if (OPTIONMGR->GetGameOption()->nCurChatMode & 0x80)
		{
			CallAssistanceDlg( btnIdx ) ;
		}
	}

 	int nNum = OPTIONMGR->GetGameOption()->nChatLineNum-m_pChatList->GetMiddleImageNum();
	if (nNum > 0)
	{
		for(int i=0; i<nNum; i++)
		{
			ExtendDlg();;
		}
	}
	// E 옵션 추가 added by hseos 2007.06.19
}





//=========================================================================
//	NAME : SetCommandToInputBox
//	DESC : The function to setting command to input edit box.
//=========================================================================
void CChattingDlg::SetCommandToInputBox()
{
	switch(GetCommand())
	{
	case 0 : m_pInputBox->SetEditText("") ;		break ;
	case 1 : m_pInputBox->SetEditText("#") ;	break ;
	case 2 : m_pInputBox->SetEditText("@") ;	break ;
	case 3 : m_pInputBox->SetEditText("$") ;	break ;
	case 4 : m_pInputBox->SetEditText("%") ;	break ;
	case 5 : m_pInputBox->SetEditText("!") ;	break ;
	case 6 : m_pInputBox->SetEditText("") ;	break ;
	}
}





//=========================================================================
//	NAME : InspectMsg
//	DESC : The function to inspect message.
//=========================================================================
void CChattingDlg::InspectMsg(char* str)
{
	char cheatStr[128] = "" ;
	char* ptr ;

	if( strlen(str) == 0 )
	{
		return ;
	}

	strcpy( cheatStr, str ) ;
	ptr = cheatStr ;
	if( strlen(ptr) > 1 )
	{
		ptr++ ;

#ifdef _CHEATENABLE_
	if( CheatFunc( ptr ) )
	{
		return ;
	}
#endif

	}

	// 0901128 LYW --- ChattingDlg :캐릭터가 상점을 활성화 한 상태인제 확인한다.
	// 091209 ONS 캐릭터 사망후에도 채팅이 가능하도록 수정

	if( strlen(str) > 1 )
	{
		Check_FaceCommand( str );

		if( Check_LifeAction( str ) )
		{
			return ;
		}
		else if( Check_EmotionCommand( str ) )
		{
			return ;
		}
		else if( Check_EmotionWord( str ) )
		{
			return ;
		}
	}

	FilteringMsg(str) ;
	if( strlen(m_szSendMsg) == 0 )
	{
		return ;
	}

	SendMsg() ;
}





//=========================================================================
//	NAME : FilteringMsg
//	DESC : The function to filter message.
//=========================================================================
void CChattingDlg::FilteringMsg(char* str)
{
	char desc[128] = {0, } ;
	char* tempStr1 ;

	int length1 = 0 ;
	int length2 = 0 ;
	int result  = 0 ;

	length1		= strlen( str ) ;
	tempStr1	= strchr( str, 32 ) ;
	if( tempStr1 != NULL )
	{
		length2 = strlen( tempStr1 ) ;

		result = tempStr1 - str + 1 ;

		strncpy( desc, str, length1 - length2 ) ;

		InspectCommand( desc ) ;

		if( m_byCurCommand == e_SMT_WHISPER )
		{
			tempStr1++ ;
			char* whisperStr = strchr( tempStr1, 32 ) ;
			if( whisperStr == NULL )
			{
				char* pName = str ;
				++pName ;

				strcpy( m_szSendMsg, tempStr1 ) ;

				memset( m_szWhisperName, 0, strlen(m_szWhisperName) ) ;

				char* namePoint = strchr(pName, 32) ;
				strncpy( m_szWhisperName, pName, strlen(pName) - strlen(namePoint)) ;
				return ;
			}
			whisperStr++ ;
			strcpy( m_szSendMsg, whisperStr ) ;

			char* nameStr = strchr( tempStr1, 32 ) ;
			memset( m_szWhisperName, 0, strlen(m_szWhisperName) ) ;
			strncpy( m_szWhisperName, tempStr1, strlen(tempStr1) - strlen(nameStr)) ;				
		}
		else if( m_byCurCommand == e_SMT_WHISPER2 )
		{
			++tempStr1 ;
			strcpy( m_szSendMsg, tempStr1 ) ;
		}
		else if( m_byCurCommand != e_SMT_ALL )
		{
			if(strlen(desc)!=0)
			{
				strcat(desc, tempStr1) ;
				strcpy( m_szSendMsg, desc ) ;
			}
			else
			{
				tempStr1++ ;
				strcpy( m_szSendMsg, tempStr1 ) ;
			}
		}
		else
		{
			strcpy( m_szSendMsg, str ) ;
		}
	}
	else
	{
		InspectCommand( str ) ;

		strcpy( m_szSendMsg, str ) ;
	}
}





//=========================================================================
//	NAME : InspectCommand
//	DESC : The function to inspect command.
//=========================================================================
void CChattingDlg::InspectCommand( char* str )
{
	char buf[12] = {0, } ;
	char* tempStr = str ;

	char token1 = 0 ;
	char token2 = 0 ;

	m_byCurCommand = e_SMT_ALL;
	int commandKind  = 5;
	int commandCount = 4;

	// check other command.
	for( int count = 0 ; count < commandKind ; ++count )
	{
		for( int count2 = 0 ; count2 < commandCount ; ++count2 )
		{
			int msgNum = 261+(((count)*commandCount)+(count2)) ;

			strcpy(buf, CHATMGR->GetChatMsg(msgNum)) ;

			if( count2 == commandCount-1 )
			{
				token1 = *str ;

				token2 = *buf ;

				if( token1 == token2 )
				{
					++tempStr ;
					strcpy(str, tempStr) ;
					m_byCurCommand = SEND_MSG_TYPE(++count);
					return ;
				}
			}
			else
			{
				if( strcmp( tempStr, buf ) == 0 )
				{
					memset(str, 0, strlen(str)) ;
					m_byCurCommand = SEND_MSG_TYPE(++count);
					return ;
				}
			}
		}
	}

	// check whisper command.
	for( int count = 0 ; count < 3 ; ++count )
	{
		const int whisperCommand = 281;

		if( strcmp( str, CHATMGR->GetChatMsg(whisperCommand + count) ) == 0 )
		{
			m_byCurCommand = e_SMT_WHISPER ;
			return ;
		}
	}

	// check whisper command to user id.
	if( token1 == '/' )
	{
		char* tempWhisper = str ;
		++tempWhisper ;
		memset( m_szWhisperName, 0, strlen(m_szWhisperName) ) ;
		strncpy( m_szWhisperName, tempWhisper, strlen(tempWhisper) ) ;

		m_byCurCommand = e_SMT_WHISPER2 ;
		return ;
	}

}





//=========================================================================
//	NAME : FindUserName
//	DESC : The function to find user name.
//=========================================================================
void CChattingDlg::FindUserName( char* name, char* msg )
{
	char tempStr1[256] = { 0, } ;
	char tempStr2[256] = { 0, } ;
	char* UserName = tempStr1 ;
	char* Msg = tempStr2 ;

	memcpy( tempStr1, name, strlen(name) ) ;
	memcpy( tempStr2, msg, strlen(msg) ) ;

    ++UserName ;
	++Msg ;

	MSG_CHAT data ;

	data.Category	= MP_CHAT ;
	data.Protocol	= MP_CHAT_FINDUSER ;
	data.dwObjectID = HEROID ;
	SafeStrCpy( data.Name, UserName, MAX_NAME_LENGTH+1 ) ;
	SafeStrCpy( data.Msg, Msg, MAX_CHAT_LENGTH+1 ) ;

	NETWORK->Send(&data,data.GetMsgLength()) ;
}





//=========================================================================
//	NAME : Check_LifeAction
//	DESC : The function to check life action.
//=========================================================================
BOOL CChattingDlg::Check_LifeAction( char* str )
{
	char* pName = NULL ;
	char checkBuf[128] = {0, } ;

	pName = strchr(str, 32) ;

	int stringLength = 0 ;

	int nameLength = 0 ;

	stringLength = strlen(str) ;

	if( pName )
	{
		nameLength = strlen(pName) ;
	}

	memcpy(checkBuf, str, stringLength - nameLength) ;

	int nIndex = -1 ;

	cPtrList& pList = CHATMGR->GetCommandList();
	PTRLISTPOS pos = pList.GetHeadPosition();

	while(pos)
	{
		sACTION_COMMAND* const pCommand = (sACTION_COMMAND*)pList.GetNext(pos) ;

		if( !pCommand ) continue ;

		if( strcmp(checkBuf, pCommand->string) != 0 ) continue ;

		// 091022 ShinJS --- 탈것 탑승시 사용할수 없는 Action 인 경우
		if( pCommand->bInvalidOnVehicle &&
			HERO->IsGetOnVehicle() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1839 ) );
			return TRUE;
		}

		nIndex = pCommand->index ;
		break ;
	}

	switch(nIndex)
	{
	case 0 :	// 공격
		{
			g_UserInput.GetHeroMove()->AttackSelectedObject() ;
			return TRUE ;
		}
		break ;

	case 1 :	// 타겟
	case 2 :	// target
	case 3 :	// 대상
		{
			if( !pName )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(11)) ;
				return TRUE ;
			}

			++pName ;

			DWORD dwObjectID = 0 ;
			dwObjectID = OBJECTMGR->GetObjectByChatName( pName ) ;

			CObject* pObject = NULL ;
			pObject = OBJECTMGR->GetObject( dwObjectID );

			if( !pObject ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			BYTE objectKind = pObject->GetObjectKind() ;

			if( objectKind > 2 )	// 몬스터 예외 처리.
			{	
				dwObjectID = 0 ;
				dwObjectID = OBJECTMGR->GetNearMonsterByName(pName) ;

				pObject = OBJECTMGR->GetObject( dwObjectID );

				if( !pObject ) 
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
					return TRUE ;
				}
			}

			// 081210 LYW --- ChattingDlg : 타겟 명령어에서, 스텔스를 사용하는 유저는 타겟이 안잡히도록 한다.
			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				CPlayer* pTarget = (CPlayer*)pObject ;
				if( !pTarget ) return TRUE ;

				if( !pTarget->GetCharacterTotalInfo()->bVisible )
				{
					// HERO가 비홀더 스킬이 활성화 되어있으면, 대상을 잡을 수 있도록 한다.
					const DWORD dwBufIdx = 33601;

					if(FALSE == STATUSICONDLG->IsHasBuff(dwBufIdx))
					{
						CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
						return TRUE ;
					}
				}
			}

			float fDistance = 0.0f ;																// HERO와의 거리를 담을 변수를 선언한다.

			VECTOR3 vHeroPos;																		// HERO의 위치를 담을 벡터를 선언한다.
			HERO->GetPosition(&vHeroPos) ;															// HERO의 위치를 받는다.
			fDistance = CalcDistanceXZ(&vHeroPos, &pObject->GetBaseMoveInfo()->CurPosition) ;				// HERO와 몬스터의 거리를 구한다.

			if( fDistance > 2000.0f ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			OBJECTMGR->SetSelectedObject(pObject) ;

			return TRUE ;
		}
		break ;

	case 4 :	// 거래
		{
			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();
			}
			else
			{
				++pName ;
				pTarget = OBJECTMGR->GetObjectByName( pName );
			}

			if( !pTarget )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

            OBJECTMGR->SetSelectedObject(pTarget) ;

			EXCHANGEMGR->ApplyExchange() ;

			return TRUE ;
		}
		break ;

	case 5 :	// 노점
		{
			STREETSTALLMGR->ChangeDialogState();

			return TRUE ;
		}
		break ;

	case 6 :	// 파티
	case 7 :	// 파티개설
	case 8 :	// 파티만들기
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx != 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1038)) ;
				return TRUE ;
			}

			GAMEIN->GetPartyDialog()->SetActive(TRUE);

			return TRUE ;
		}
		break ;

	case 9 :	// 파티 초대
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(370)) ;
				return TRUE ;
			}

			if( HEROID != PARTYMGR->GetMasterID() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1039)) ;
				return TRUE ;
			}

			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();
			}
			else
			{
				++pName ;
				pTarget = OBJECTMGR->GetObjectByName( pName );
			}

			if( !pTarget ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			if( PARTYMGR->IsPartyMember(pTarget->GetID()) )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1038)) ;
				return TRUE ;
			}

			OBJECTMGR->SetSelectedObject(pTarget) ;

			PARTYMGR->AddPartyMemberSyn(pTarget->GetID()) ;

			return TRUE ;
		}
		break ;

	case 10 :	// 파티 추방
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(370)) ;
				return TRUE ;
			}

			if( HEROID != PARTYMGR->GetMasterID() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1039)) ;
				return TRUE ;
			}

			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();

				if( !pTarget ) 
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
					return TRUE ;
				}
				else
				{
					PARTYMGR->BanPartyMemberSyn(pTarget->GetID()) ;
				}
			}
			else
			{
				++pName ;

				if(PARTYMGR->GetParty().IsPartyMember(pName))
				{
					PARTYMGR->BanPartyMemberSyn(
						PARTYMGR->GetParty().GetMemberIdForName(pName));
				}
				else
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 410 ) );
					return TRUE ;
				}
			}

			return TRUE ;
		}
		break ;

	case 11 :	// 파장 위임
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(370)) ;
				return TRUE ;
			}

			if( HEROID != PARTYMGR->GetMasterID() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1039)) ;
				return TRUE ;
			}

			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();
			}
			else
			{
				++pName ;
				pTarget = OBJECTMGR->GetObjectByName( pName );
			}
			

			if( !pTarget ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			if( !PARTYMGR->IsPartyMember(pTarget->GetID()) )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1041)) ;
				return TRUE ;
			}

			OBJECTMGR->SetSelectedObject(pTarget) ;

			PARTYMGR->ChangeMasterPartySyn(HEROID, pTarget->GetID()) ;

			return TRUE ;
		}
		break ;

	case 12 :	// 파티 해제
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(370)) ;
				return TRUE ;
			}

			if( HEROID != PARTYMGR->GetMasterID() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1039)) ;
				return TRUE ;
			}

			if(PARTYMGR->IsProcessing() == TRUE)
				return TRUE ;
			WINDOWMGR->MsgBox(MBI_PARTYBREAKUP, MBT_YESNO, CHATMGR->GetChatMsg(651));

			return TRUE ;
		}
		break ;

	case 14 :	// 파티 탈퇴
		{
			DWORD dwPartyIdx = HERO->GetPartyIdx() ;

			if( dwPartyIdx == 0 )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(370)) ;
				return TRUE ;
			}

			if( HEROID == PARTYMGR->GetMasterID() )
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1042)) ;
				return TRUE ;
			}

			PARTYMGR->DelPartyMemberSyn() ;

			return TRUE ;
		}
		break ;

	case 16 :	// 대결 신청
		{
			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();
			}
			else
			{
				++pName ;
				pTarget = OBJECTMGR->GetObjectByName( pName );
			}

			if( !pTarget ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			BYTE objectKind = pTarget->GetObjectKind() ;

			if( objectKind != 1 )	// 유저 체크
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1043)) ;
				return TRUE ;
			}

            OBJECTMGR->SetSelectedObject(pTarget) ;

			SHOWDOWNMGR->ApplyShowdown() ;

			return TRUE ;
		}
		break ;

	case 17 :	// 친구 초대
	case 18 :	// 친구 추가
		{
			CObject* pTarget = NULL ;

			if( !pName )
			{
				pTarget = OBJECTMGR->GetSelectedObject();
			}
			else
			{
				++pName ;
				pTarget = OBJECTMGR->GetObjectByName( pName );
			}

			if( !pTarget ) 
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1037)) ;
				return TRUE ;
			}

			strupr(pName);

			if(strcmp("",pName) == 0) return FALSE ;

			char heroname[MAX_NAME_LENGTH+1] = {0,};

			sscanf(HERO->GetObjectName(), "%s", heroname);

			strupr(heroname);

			if(strcmp(heroname, pName) == 0)
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(676) );
				return TRUE ;
			}				
			FRIENDMGR->AddDelFriendSyn(pName, MP_FRIEND_ADD_SYN);
			GAMEIN->GetMiniFriendDialog()->SetActive(FALSE);

			return TRUE ;
		}
		break ;

	case 19 :	// 앉기
	case 20 :	// 서기
		{
			// desc_hseos_농장시스템_01
			// S 농장시스템 추가 added by hseos 2007.09.20
			if (g_csFarmManager.CLI_GetIngCharMotion()) return FALSE;
			// E 농장시스템 추가 added by hseos 2007.09.20

			CHero* hero = OBJECTMGR->GetHero();
			ASSERT( hero );

			BYTE IsRestMode = OBJECTSTATEMGR->GetObjectState(hero) ;

			BOOL bReady = FALSE ;

			if( IsRestMode == eObjectState_None || IsRestMode == eObjectState_Move || IsRestMode == eObjectState_Rest )
			{
				bReady = TRUE ;
			}

			if( !bReady )
			{
				return FALSE ;
			}

			if( IsRestMode == eObjectState_Rest )
			{
				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_None);

				IsRestMode = eObjectState_None;
			}
			else
			{
				MOVEMGR->HeroMoveStop();
				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Rest);

				IsRestMode = eObjectState_Rest;
			}

			MSG_BYTE msg ;

			msg.Category	= MP_EMOTION ;
			msg.Protocol	= MP_EMOTION_SITSTAND_SYN ;
			msg.dwObjectID	= gHeroID ;
			msg.bData		= IsRestMode ;

			NETWORK->Send(&msg, sizeof(MSG_BYTE));

			return TRUE ;
		}
		break ;

	case 21 :	// 조합
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break ;

	case 22 :	// 강화
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break ;	

	case 23 :	// 분해
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break ;

	case 24 :	// 인챈트
		{
			cDialog* dialog = WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG	 );
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break;
//---KES 상점검색 2008.3.11
	case 26:
		{
			//pName은 이름이 아니고 검색할 단어이다.

			if( pName )
			{
				STREETSTALLMGR->SetSearchWord( pName+1 );
			}
			else
			{
				STREETSTALLMGR->SetSearchWord( "" );
			}

			STREETSTALLMGR->SearchWordInArea();
		}
		return TRUE;
//---------------

//---KES 따라가기
	case 27:
		{
			CObject* pObject = OBJECTMGR->GetSelectedObject();

			if( pObject )
			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				if( HERO )
					HERO->SetFollowPlayer( pObject->GetID() );
			}
		}
		return TRUE;
	case 28:
		{
			cDialog* dialog = (cDialog*)GAMEIN->GetFishingDlg();
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break;
	case 30:
		{
			cDialog* dialog = (cDialog*)GAMEIN->GetCookDlg();
			ASSERT( dialog );

			dialog->SetActive( ! dialog->IsActive() );

			return TRUE ;
		}
		break;

	default : break ;
	}

	return FALSE ;
}





//=========================================================================
//	NAME : Check_EmotionCommand
//	DESC : The function to check command for emotion.
//=========================================================================
BOOL CChattingDlg::Check_EmotionCommand( char* str )
{
	CYHHashTable<cConductInfo>* const pTable = CONDUCTMGR->GetConductTable() ;
	pTable->SetPositionHead() ;

	for(cConductInfo* pCInfo = pTable->GetData();
		0 < pCInfo;
		pCInfo = pTable->GetData())
	{
		if( pCInfo->GetConductKind() == 4 )
		{
			// if conduct kind is same to emotion then check command.
			if( strcmp( str, pCInfo->GetConductName() ) == 0 )
			{
				// setting emotioni number.
				MOVEMGR->HeroMoveStop() ;
				CONDUCTMGR->SendToServer( pCInfo->GetMotionIdx() ) ;

				return TRUE ;
			}
		}
	}

	return FALSE ;
}





//=========================================================================
//	NAME : Check_EmotionWord
//	DESC : The function to check words for emotion.
//=========================================================================
BOOL CChattingDlg::Check_EmotionWord( char* str )
{
	int emoticonNum = GetEmoticonNum( str ) ;
	if( emoticonNum > -1 )
	{
		HERO->PlayEmoticon_Syn(emoticonNum) ;
	}
	else
	{
		return FALSE ;
	}

	return TRUE ;
}

BOOL CChattingDlg::Check_FaceCommand( char* str )
{
	const int faceNum = CHATMGR->GetFaceNum( str ) ;

	if( faceNum > -1 )
	{
		FACIALMGR->SendToServerFace(faceNum);
		return TRUE ;
	}

	return FALSE ;
}





//=========================================================================
//	NAME : SendMsg
//	DESC : The function to send chatting message.
//=========================================================================
void CChattingDlg::SendMsg()
{
	switch(m_byCurCommand)
	{
		case e_SMT_ALL :		Msg_All() ;			break ;
		case e_SMT_PARTY :		Msg_Party() ;		break ;
		case e_SMT_FAMILY :		Msg_Family() ;		break ;
		case e_SMT_GUILD :		Msg_Guild() ;		break ;
		case e_SMT_ALLIANCE :	Msg_Alliance() ;	break ;
		case e_SMT_TRADE :		Msg_Trade() ;		break ;
		case e_SMT_WHOLE :		Msg_Whole() ;		break ;
		case e_SMT_WHISPER :	Msg_Whisper() ;		break ;
		case e_SMT_WHISPER2 :	Msg_Whisper() ;		break ;
	}
}





//=========================================================================
//	NAME : Msg_All
//	DESC : The function to send normal message.
//=========================================================================
void CChattingDlg::Msg_All()
{
	TESTMSG data;
			data.Category	= MP_CHAT;
			data.Protocol	= MP_CHAT_ALL;
			data.dwObjectID	= HEROID;
	#ifdef _TESTCLIENT_

			char buf[256];
			sprintf( buf, "[%s]: %s", HERO->GetObjectName(), m_szSendMsg );
			CHATMGR->AddMsg( CTC_GENERALCHAT, buf, RGBA_MAKE(94, 94, 94, 255) );
	#endif
			SafeStrCpy(data.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1 );
			NETWORK->Send(&data,data.GetMsgLength());
}





//=========================================================================
//	NAME : Msg_Party
//	DESC : The function to send message to party.
//=========================================================================
void CChattingDlg::Msg_Party()
{
	#ifdef _TESTCLIENT_
  			CHATMGR->AddMsg( CTC_GENERALCHAT, CHATMGR->GetChatMsg(370), RGBA_MAKE(94, 94, 94, 255) );
#else
			if( !PARTYMGR->PartyChat( m_szSendMsg, HERO->GetObjectName() ) )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(370), RGBA_MAKE(94, 94, 94, 255) );
			}
#endif
}





//=========================================================================
//	NAME : Msg_Family
//	DESC : The function to send message to family.
//=========================================================================
void CChattingDlg::Msg_Family()
{
	if (HERO->GetFamily()->Get()->nID == 0)
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(369), RGBA_MAKE(94, 94, 94, 255) );
		return ;
	}

	MSG_CHAT stPacket;

	stPacket.Category	= MP_CHAT;
	stPacket.Protocol	= MP_CHAT_FAMILY;
	stPacket.dwObjectID	= HEROID;
	SafeStrCpy(stPacket.Name, HERO->GetObjectName(), MAX_NAME_LENGTH+1);
	SafeStrCpy(stPacket.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1);

	NETWORK->Send(&stPacket, stPacket.GetMsgLength());
	// E 패밀리 추가 added by hseos 2007.07.13
}





//=========================================================================
//	NAME : Msg_Guild
//	DESC : The function to send guild message to guild.
//=========================================================================
void CChattingDlg::Msg_Guild()
{
	if( !HERO->GetGuildIdx() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(325), RGBA_MAKE(94, 94, 94, 255) );
		return;
	}

	MSG_GUILD_CHAT data;									// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(TESTMSGID->MSG_GUILD_UNION_CHAT)
	data.Category = MP_CHAT;
	data.Protocol = MP_CHAT_GUILD;
	data.dwObjectID = HEROID;								// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(GuildIdx->HEROID)
	data.dwGuildIdx = HERO->GetGuildIdx();					// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(HEROID->GuildIdx)
	SafeStrCpy(data.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1 );
	NETWORK->Send(&data,data.GetMsgLength());
}





//=========================================================================
//	NAME : Msg_Alliance
//	DESC : The function to send message to alliance.
//=========================================================================
void CChattingDlg::Msg_Alliance()
{
	CHero* hero = OBJECTMGR->GetHero();
	ASSERT( hero );
	
	if( ! hero )
	{
		return;
	}
	else if( ! hero->GetGuildIdx() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(325), RGBA_MAKE(94, 94, 94, 255) );
		return;
	}
	else if( !hero->GetGuildUnionIdx() )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(371), RGBA_MAKE(94, 94, 94, 255) );
		return;
	}

	{
		MSG_GUILDUNION_CHAT message;							// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(MSG_CHAT_WITH_SENDERID->MSG_GUILD_UNION_CHAT)
		message.Category	= MP_CHAT;
		message.Protocol	= MP_CHAT_GUILDUNION;
		message.dwObjectID	= gHeroID;					// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(GuildUnionIdx->HEROID)
		message.dwGuildUnionIdx	= hero->GetGuildUnionIdx();		// 090317 ONS 에이전트서버의 패킷조작 방지처리에 따른 메세지 변경(HEROID->GuildUnionIdx)
		SafeStrCpy( message.Name, hero->GetObjectName(), sizeof( message.Name ) );
		SafeStrCpy( message.Msg, m_szSendMsg, sizeof( message.Msg ) );
		NETWORK->Send( &message, message.GetMsgLength() );
	}	
}





//=========================================================================
//	NAME : Msg_Trade
//	DESC : The function to send message for trade.
//=========================================================================
void CChattingDlg::Msg_Trade()
{
	MSG_CHAT_WITH_SENDERID msgChat;
	msgChat.Category = MP_CHAT;
	msgChat.Protocol = MP_CHAT_TRADE;
	msgChat.dwObjectID = HEROID;
	msgChat.dwSenderID = HEROID;
	SafeStrCpy( msgChat.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1 );
	SafeStrCpy( msgChat.Name, HERO->GetObjectName(), MAX_NAME_LENGTH+1 );
	NETWORK->Send( &msgChat, msgChat.GetMsgLength() );
}





//=========================================================================
//	NAME : Msg_Whole
//	DESC : The function to send message to whole.
//=========================================================================
void CChattingDlg::Msg_Whole()
{
	MSG_CHAT_WITH_SENDERID msgChat;
	msgChat.Category = MP_CHAT;
	msgChat.Protocol = MP_CHAT_WHOLE;
	msgChat.dwObjectID = HEROID;
	msgChat.dwSenderID = HEROID;
	SafeStrCpy( msgChat.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1 );
	SafeStrCpy( msgChat.Name, HERO->GetObjectName(), MAX_NAME_LENGTH+1 );
	NETWORK->Send( &msgChat, msgChat.GetMsgLength() );
}





//=========================================================================
//	NAME : Msg_Whisper
//	DESC : The function to send message to whisper.
//=========================================================================
void CChattingDlg::Msg_Whisper() 
{
	if( strcmp( m_szWhisperName, HERO->GetObjectName() ) == 0 )
	{
		CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg(367), RGBA_MAKE(94, 94, 94, 255) ) ;
		return ;
	}

	// desc_hseos_문자필터링01
	// S 문자필터링 추가 added by hseos 2007.06.25
	// ..필터에서 제외할 문자 처리를 위해 타입을 설정한다.
	FILTERTABLE->SetExceptionInvalidCharIncludeType(FET_WHISPER_CHARNAME);
	// E 문자필터링 추가 added by hseos 2007.06.25
	if( (FILTERTABLE->IsInvalidCharInclude((unsigned char*)m_szWhisperName)) == TRUE )
	{
		CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(368), RGBA_MAKE(94, 94, 94, 255));
		return;
	}

	MSG_CHAT data;
	data.Category	= MP_CHAT;
	data.Protocol	= MP_CHAT_WHISPER_SYN;
	data.dwObjectID	= HEROID;
	SafeStrCpy( data.Name, m_szWhisperName, MAX_NAME_LENGTH+1 );
	SafeStrCpy(data.Msg, m_szSendMsg, MAX_CHAT_LENGTH+1 );
	NETWORK->Send(&data,data.GetMsgLength());
}

int CChattingDlg::GetEmoticonNum(char* str)
{
	int returnNum = -1 ;

	cPtrList* pList = CONDUCTMGR->GetEmoticonList() ;

	BYTE listCount = pList->GetCount() ;
	BYTE count = 0 ;

	char* pResult = NULL ;
	EMOTICON* pEmoticon = NULL ;

	PTRLISTPOS pos = pList->GetHeadPosition() ;
	while(count < listCount)
	{
		pEmoticon = (EMOTICON*)pList->GetNext(pos) ;
		if( pEmoticon )
		{
			pResult = strstr(str, pEmoticon->command) ;

			if( pResult != NULL ) return pEmoticon->effectNum ;

			++count ;
		}
	}

	return returnNum ;
}





//=========================================================================
//	NAME : GetCommand
//	DESC : The function to receive command of chatting type.
//=========================================================================
int CChattingDlg::GetCommand()
{
	for( BYTE count = 0 ; count < e_CONVERSATION_MAX-1 ; ++count )
	{
		if( m_pTabButton[count]->IsPushed() ) return count ;
	}

	return 0 ;
}





//=========================================================================
//	NAME : AddMsg
//	DESC : The function to add message to each part.
//		 : Modified September 17, 2008 - LYW
//=========================================================================
//void CChattingDlg::AddMsg( BYTE byClass, char* msg, DWORD dwColor )
void CChattingDlg::AddMsg( BYTE byMessageType, BYTE byTabBtnType, char* msg, DWORD dwColor )
{
	char buf[MAX_PATH] = {0};

#ifdef _TL_LOCAL_

	int nLen = strlen( msg );

	BYTE brk[512];
	int nBrkNum = g_TBreak.FindThaiWordBreak( msg, nLen, brk, 512, FTWB_SEPARATESYMBOL );
	int nCutPos = 0;

	int nCount = 0;

	for( int i = 0 ; i < nBrkNum ; ++i )
	{
		++nCount;	//몇단어썼나
		nCutPos += brk[i];
		if( CFONT_OBJ->GetTextExtentEx(0, msg, nCutPos ) > CHATLIST_TEXTEXTENT )
		{
			if( nCount == 1 )	//한단어만 썼는데 넘어가버리면 강제로 끊는다.
			{
				for( int k = 1 ; k <= nCutPos ; ++k )
				{
					if( CFONT_OBJ->GetTextExtentEx(0, msg, k ) > CHATLIST_TEXTEXTENT )
					{
						nCutPos = nCutPos - brk[i] + k-1; //쓴 글자까지만
						strncpy( buf, msg, nCutPos );	//전단계까지
						buf[nCutPos] = 0;

						if(m_pTabButton[e_CONVERSATION_SYSTEM]->IsPushed() &&
							m_byCurAssistance == e_CONVERSATION_SYSTEM &&
							m_pAssistList->IsActive())
						{
							// 시스템 메시지는 모두, 단축 창에만 출력하도록 한다.
							if( byTabBtnType == e_CONVERSATION_SYSTEM )
							{
								// 귓속말/게임 팁은 메인 채팅창에 출력한다.
								if( byMessageType == CTC_WHISPER || byMessageType == CTC_TIPS || byMessageType == CTC_EXITCOUNT )
								{
									m_pChatList->AddItem( buf, dwColor );
								}
							}
							else
							{
								m_pChatList->AddItem( buf, dwColor );
							}
						}
						else
						{
							m_pChatList->AddItem( buf, dwColor );
						}

						msg += nCutPos;
						brk[i] -= nCutPos;	//나머지부분 계산
						nCutPos = 0;
						nCount = 0; //초기화
						--i;
						break;
					}
				}
			}
			else
			{
				nCutPos -= brk[i];
				strncpy( buf, msg, nCutPos );	//전단계까지
				buf[nCutPos] = 0;
				
				// 080917 LYW --- ChattingDlg : 시스템 탭이 활성화 되어 있을 때, 귓속말/팁은 메인창에 출력하고,
				// 나머지 메시지들은 시스템 단축 창에만 출력하도록 처리 - (원상미).
				if( m_pTabButton[e_CONVERSATION_SYSTEM]->IsPushed() 
				&& m_byCurAssistance == e_CONVERSATION_SYSTEM 
				&& m_pAssistList->IsActive() )	// 시스템 단축창이 활성화 되어 있으면,
				{
					// 시스템 메시지는 모두, 단축 창에만 출력하도록 한다.
					if( byTabBtnType == e_CONVERSATION_SYSTEM )
					{
						// 귓속말/게임 팁은 메인 채팅창에 출력한다.
						if( byMessageType == CTC_WHISPER || byMessageType == CTC_TIPS || byMessageType == CTC_EXITCOUNT )
						{
							m_pChatList->AddItem( buf, dwColor );
						}
					}
					else
					{
						m_pChatList->AddItem( buf, dwColor );
					}
				}
				else
				{
					m_pChatList->AddItem( buf, dwColor );
				}

				--i;
				msg += nCutPos;
				nCutPos = 0;
				nCount = 0; //초기화
			}

			if( *msg == ' ' )
			{
				++msg;	//라인 첫글자가 스페이스가 아니도록...
				++i;
			}
		}
	}

	if( nLen > 0 )
	{
		// 080917 LYW --- ChattingDlg : 시스템 탭이 활성화 되어 있을 때, 귓속말/팁은 메인창에 출력하고,
		// 나머지 메시지들은 시스템 단축 창에만 출력하도록 처리 - (원상미).
		if( m_pTabButton[e_CONVERSATION_SYSTEM]->IsPushed() 
			&& m_byCurAssistance == e_CONVERSATION_SYSTEM 
			&& m_pAssistList->IsActive() )	// 시스템 단축창이 활성화 되어 있으면,
		{
			// 시스템 메시지는 모두, 단축 창에만 출력하도록 한다.
			if( byTabBtnType == e_CONVERSATION_SYSTEM )
			{
				// 귓속말/게임 팁은 메인 채팅창에 출력한다.
				if( byMessageType == CTC_WHISPER || byMessageType == CTC_TIPS || byMessageType == CTC_EXITCOUNT )
				{
					m_pChatList->AddItem( msg, dwColor ) ;
				}
			}
			else
			{
				m_pChatList->AddItem( msg, dwColor ) ;
			}
		}
		else
		{
			m_pChatList->AddItem( msg, dwColor ) ;
		}
	}

#else

	// 091201 ShinJS --- Dialog에 설정된 TextRect 길이로 Text를 자른다.
	int nLen = strlen( msg );
	char* pLeftoverMsg = msg;
	LONG limitWidth = m_pChatList->GetTextWidth();
	LONG totalWidth = CFONT_OBJ->GetTextExtentEx( GetFontIdx(), msg, nLen );

	do
	{
		int nIndex = 0;
		LONG width = 0;
		while( nIndex < nLen )
		{
			int nCharSize = (IsDBCSLeadByte( *(pLeftoverMsg+nIndex) ) == TRUE ? 2 : 1);
			width += CFONT_OBJ->GetTextExtentEx( 0, (pLeftoverMsg+nIndex), nCharSize );
			if( width > limitWidth )
				break;

			nIndex += nCharSize;
		}

		if( nIndex <= 0 )
			break;

		ZeroMemory(
			buf,
			sizeof(buf));
		strncpy( buf, pLeftoverMsg, nIndex );
		pLeftoverMsg = pLeftoverMsg + nIndex;
		nLen = strlen( pLeftoverMsg );

		if(m_byCurAssistance == e_CONVERSATION_SYSTEM &&
			m_pAssistList->IsActive())
		{
			// 시스템 메시지는 모두, 단축 창에만 출력하도록 한다.
			if( byTabBtnType == e_CONVERSATION_SYSTEM )
			{
				// 귓속말/게임 팁은 메인 채팅창에 출력한다.
				if( byMessageType == CTC_WHISPER || byMessageType == CTC_TIPS || byMessageType == CTC_EXITCOUNT )
				{
					m_pChatList->AddItem( buf, dwColor ) ;
				}
				else
				{
					m_pAssistList->AddItem( buf, dwColor ) ;
				}
			}
			else
			{
				m_pChatList->AddItem( buf, dwColor ) ;
			}
		}
		else
		{
			m_pChatList->AddItem( buf, dwColor ) ;

			if(byTabBtnType == m_byCurAssistance)
			{
				m_pAssistList->AddItem( buf, dwColor ) ;
			}
		}
	}
	while(totalWidth > limitWidth);

#endif
}

void CChattingDlg::AddShoutMsg(char* msg, DWORD dwColor)
{
	char buf[ MAX_PATH ] = {0};

#ifdef _TL_LOCAL_

	int nLen = strlen( msg );

	BYTE brk[512];
	int nBrkNum = g_TBreak.FindThaiWordBreak( msg, nLen, brk, 512, FTWB_SEPARATESYMBOL );
	int nCutPos = 0;

	int nCount = 0;

	for( int i = 0 ; i < nBrkNum ; ++i )
	{
		++nCount;	//몇단어썼나
		nCutPos += brk[i];
		if( CFONT_OBJ->GetTextExtentEx(0, msg, nCutPos ) > CHATLIST_TEXTEXTENT )
		{
			if( nCount == 1 )	//한단어만 썼는데 넘어가버리면 강제로 끊는다.
			{
				for( int k = 1 ; k <= nCutPos ; ++k )
				{
					if( CFONT_OBJ->GetTextExtentEx(0, msg, k ) > CHATLIST_TEXTEXTENT )
					{
						nCutPos = nCutPos - brk[i] + k-1; //쓴 글자까지만
						strncpy( buf, msg, nCutPos );	//전단계까지
						buf[nCutPos] = 0;
						m_pChatList->AddItem( buf, dwColor );
						msg += nCutPos;
						brk[i] -= nCutPos;	//나머지부분 계산
						nCutPos = 0;
						nCount = 0; //초기화
						--i;
						break;
					}
				}
			}
			else
			{
				nCutPos -= brk[i];
				strncpy( buf, msg, nCutPos );	//전단계까지
				buf[nCutPos] = 0;
				//m_pChatList->AddItem( buf, dwColor );
				--i;
				msg += nCutPos;
				nCutPos = 0;
				nCount = 0; //초기화
			}

			if( *msg == ' ' )
			{
				++msg;	//라인 첫글자가 스페이스가 아니도록...
				++i;
			}
		}
	}

#else
	int nLen	= strlen( msg ) ;
	int nCpyNum = 0 ;

	while( nLen > MAX_TEXTLEN )
	{
		if( (msg + MAX_TEXTLEN ) != CharNext( CharPrev( msg, msg + MAX_TEXTLEN ) ) )
		{
			nCpyNum = MAX_TEXTLEN - 1 ;
		}
		else
		{
			nCpyNum = MAX_TEXTLEN ;
		}

		strncpy( buf, msg, nCpyNum ) ;
		buf[ nCpyNum ] = 0 ;

		//m_pChatList->AddItem( buf, dwColor ) ;

		nLen -= nCpyNum ;
		msg  += nCpyNum ;

		if( *msg == ' ' ) ++msg ;
	}
#endif

	if(strlen(buf) > 0)
	{
		m_pShoutList->AddItem(buf, dwColor);
	}

	if( nLen > 0 )
	{
		m_pShoutList->AddItem(msg, dwColor);
	}
}





//=========================================================================
//	NAME : ActionEvent
//	DESC : The function to process mouse events.
//=========================================================================
DWORD CChattingDlg::ActionEvent( CMouse* mouseInfo )
{
	if( !IsActive() ) return WE_NULL;

	if( !mouseInfo ) return WE_NULL;

	m_pInputBox->ActionEvent(mouseInfo);

	DWORD we = cDialog::ActionEvent( mouseInfo ) ;

	if( m_pExtendBtn->IsClickInside() )
	{
		ExtendDlg() ;
		return we ;
	}
	else if( m_pReductionBtn->IsClickInside() )
	{
		ReductionDlg() ;
		return we ;
	}

	if( we & WE_RBTNCLICK )
	{
		Event_RButtonClick(mouseInfo) ;
	}
	else if( we & WE_LBTNCLICK )
	{
		if(!Event_LButtonClick(mouseInfo))
		{
			Event_PushDown(mouseInfo) ;
		}
	}
	
	return we ;
}





//=========================================================================
//	NAME : Event_LButtonClick
//	DESC : The function to process left click event of button.
//=========================================================================
BOOL CChattingDlg::Event_LButtonClick(CMouse* mouseInfo)
{
	LONG xPos = (LONG)mouseInfo->GetMouseEventX() ;
	LONG yPos = (LONG)mouseInfo->GetMouseEventY() ;

	if( m_pExtendBtn->PtInWindow( xPos, yPos ) )
	{
		ExtendDlg() ;
		return TRUE ;
	}
	else if( m_pReductionBtn->PtInWindow( xPos, yPos ) )
	{
		ReductionDlg() ;
		return TRUE ;
	}
	else if( m_pCloseBtn->PtInWindow( xPos, yPos ) )
	{
		ResetAssist() ;

		// 080312 NYJ --- 채팅보조창(=팝업창) 닫을때 옵션 저장하는 코드 추가.
		sGAMEOPTION stOption = *OPTIONMGR->GetGameOption();
		stOption.nCurChatMode = m_byCurAssistance & 0x80;
		OPTIONMGR->SetGameOption(&stOption);

		return TRUE ;
	}

	return FALSE ;
}





//=========================================================================
//	NAME : Event_RButtonClick
//	DESC : The function to process right click event of button.
//=========================================================================
void CChattingDlg::Event_RButtonClick(CMouse* mouseInfo)
{
	CONVERSATION btnIdx = e_CONVERSATION_MAX;

	LONG xPos = (LONG)mouseInfo->GetMouseEventX() ;
	LONG yPos = (LONG)mouseInfo->GetMouseEventY() ;

	for(int count = e_CONVERSATION_ALL ; count < e_CONVERSATION_MAX-1 ; ++count )
	{
		if( m_pTabButton[count]->PtInWindow(xPos, yPos) )
		{
			btnIdx = CONVERSATION(count);
		}
	}

	if(e_CONVERSATION_MAX == btnIdx) return ;

	for(int count = e_CONVERSATION_ALL ; count < e_CONVERSATION_MAX-1 ; ++count )
	{
		m_pTabButton[count]->SetPush(FALSE) ;
	}

	m_pTabButton[btnIdx]->SetPush(TRUE) ;

	SetCommandToInputBox() ;
	m_pInputBox->SetFocusEdit(FALSE) ;

	CallAssistanceDlg( btnIdx ) ;

	sGAMEOPTION stOption = *OPTIONMGR->GetGameOption();
	stOption.nCurChatMode = btnIdx | 0x80;
	OPTIONMGR->SetGameOption(&stOption);
}





//=========================================================================
//	NAME : Event_PushDown
//	DESC : The function to process push down event of tab button.
//=========================================================================
void CChattingDlg::Event_PushDown(CMouse* mouseInfo)
{
	int btnIdx = -1 ;

	LONG xPos = mouseInfo->GetMouseEventX() ;
	LONG yPos = mouseInfo->GetMouseEventY() ;

	int count = 0 ;

	for( count = 0 ; count < e_CONVERSATION_MAX-1 ; ++count )
	{
		if( m_pTabButton[count]->PtInWindow( xPos, yPos ) )
		{
			btnIdx = count ;
		}
	}

	if( btnIdx == -1 ) return ;

	for( count = 0 ; count < e_CONVERSATION_MAX-1 ; ++count )
	{
		m_pTabButton[count]->SetPush(FALSE) ;
	}

	m_pTabButton[btnIdx]->SetPush(TRUE) ;

	SetCommandToInputBox() ;
	m_pInputBox->SetFocusEdit(TRUE) ;

	// desc_hseos_옵션01
	// S 옵션 추가 added by hseos 2007.06.19
	// ..채팅 모드를 저장. 맵이동시 불러옴
	sGAMEOPTION stOption = *OPTIONMGR->GetGameOption();
	stOption.nCurChatMode = btnIdx;
	OPTIONMGR->SetGameOption(&stOption);
	// E 옵션 추가 added by hseos 2007.06.19
}





//=========================================================================
//	NAME : ExtendDlg
//	DESC : The function to extend dialog.
//=========================================================================
void CChattingDlg::ExtendDlg()
{
 	int middleCount  = m_pChatList->GetMiddleImageNum() ;

	if( middleCount >= m_pChatList->GetMaxMiddleNum() ) return ;
	
	int middleHeight = m_pChatList->GetHeight(LD_MIDDLE) ;

	m_pShoutList->SetAbsXY( (LONG)m_pShoutList->GetAbsX(), (LONG)m_pShoutList->GetAbsY()-middleHeight ) ;

	m_pAssistList->SetAbsXY( (LONG)m_pAssistList->GetAbsX(), (LONG)m_pAssistList->GetAbsY()-middleHeight ) ;
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetAbsXY((LONG)m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetAbsX(), (LONG)m_pAssistList->GetAbsY() - m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight()) ;
	m_pCloseBtn->SetAbsXY((LONG)m_pCloseBtn->GetAbsX(), (LONG)m_pAssistList->GetAbsY()- m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight() + 5) ;

	m_pChatList->onSize(TRUE) ;

	// desc_hseos_옵션01
	// S 옵션 추가 added by hseos 2007.06.20
	// ..채팅창 라인 수를 저장. 맵이동시 불러옴
	sGAMEOPTION stOption = *OPTIONMGR->GetGameOption();
	stOption.nChatLineNum = middleCount;
	OPTIONMGR->SetGameOption(&stOption);
	// E 옵션 추가 added by hseos 2007.06.20
}



//=========================================================================
//	NAME : ReductionDlg
//	DESC : The function to reduction dialog.
//=========================================================================
void CChattingDlg::ReductionDlg()
{	
	int middleCount  = m_pChatList->GetMiddleImageNum() ;

	if( middleCount <= m_pChatList->GetMinMiddleNum() ) return ;

	int middleHeight = m_pChatList->GetHeight(LD_MIDDLE) ;

	m_pShoutList->SetAbsXY( (LONG)m_pShoutList->GetAbsX(), (LONG)m_pShoutList->GetAbsY()+middleHeight ) ;

	m_pAssistList->SetAbsXY( (LONG)m_pAssistList->GetAbsX(), (LONG)m_pAssistList->GetAbsY()+middleHeight ) ;
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetAbsXY((LONG)m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetAbsX(), (LONG)m_pAssistList->GetAbsY() - m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight()) ;
	m_pCloseBtn->SetAbsXY((LONG)m_pCloseBtn->GetAbsX(), (LONG)m_pAssistList->GetAbsY()- m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight() + 5) ;

	m_pChatList->onSize(FALSE) ;

	sGAMEOPTION stOption = *OPTIONMGR->GetGameOption();
	stOption.nChatLineNum = middleCount;
	OPTIONMGR->SetGameOption(&stOption);
}




//=========================================================================
//	NAME : ResetAssist
//	DESC : The function to resetting to assist chatting dialog.
//=========================================================================
void CChattingDlg::ResetAssist()
{
	m_pAssistList->SetActive(
		FALSE);
	m_pCloseBtn->SetActive(
		FALSE);
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetActive(
		FALSE);
}





//=========================================================================
//	NAME : GetCurMsgColor
//	DESC : The function to receive current message color.
//=========================================================================
DWORD CChattingDlg::GetCurMsgColor(BYTE msgType)
{
	eTextClass colorIdx = CTC_OPERATOR;

	switch(msgType)
	{
		case e_CONVERSATION_ALL :		colorIdx = CTC_GENERALCHAT ;	break ;
		case e_CONVERSATION_PARTY :		colorIdx = CTC_PARTYCHAT ;		break ;
		case e_CONVERSATION_FAMILY :	colorIdx = CTC_FAMILY ;			break ;
		case e_CONVERSATION_GUILD :		colorIdx = CTC_GUILDCHAT ;		break ;
		case e_CONVERSATION_ALLIANCE :	colorIdx = CTC_ALLIANCE ;		break ;
		case e_CONVERSATION_TRADE :		colorIdx = CTC_TRADE ;			break ;
		case e_CONVERSATION_SYSTEM : 	colorIdx = CTC_SYSMSG ;			break ;
	}

	return CHATMGR->GetMsgColor(colorIdx) ;
}

void CChattingDlg::CallAssistanceDlg(CONVERSATION byClass)
{
	m_byCurAssistance = byClass ;

	ResetAssist() ;

	LONG itemCount = m_pChatList->GetItemCount() ;

	BYTE count = 0 ;

	m_pAssistList->RemoveAll() ;

	DWORD curColor  = GetCurMsgColor(byClass) ;
	DWORD itemColor = 0 ;

	// 080918 LYW --- ChattingDlg : 시스템 메시지 작업 - (원상미)
	if( byClass == e_CONVERSATION_SYSTEM )
	{
		m_pAssistList->RemoveAll() ;
	}
	else
	{
		while( itemCount > count )
		{
			ITEM* pItem = m_pChatList->GetItem( count ) ;
			if( pItem )
			{
				itemColor = pItem->rgb ;

				if( curColor == itemColor )
				{
					m_pAssistList->AddItem(pItem->string, pItem->rgb) ;
				}
			}

			++count ;
		}
	}

	DWORD txtColor = RGBA_MAKE(255, 255, 255, 255) ;

	int middleCount  = m_pChatList->GetMiddleImageNum() ;
	int middleHeight = m_pChatList->GetHeight(LD_MIDDLE) ;

	int interval = (middleCount-2) * middleHeight + m_pChatList->GetHeight(LD_TOP) + m_pChatList->GetHeight(LD_DOWN) + 2 ;

	if(m_pShoutList->IsActive())
	{
		middleCount = m_pShoutList->GetMiddleImageNum();
		middleHeight = m_pShoutList->GetHeight(LD_MIDDLE);
		interval += middleCount * middleHeight + m_pShoutList->GetHeight(LD_TOP) + m_pShoutList->GetHeight(LD_DOWN) + 2 ;
	}

	m_pAssistList->SetAbsXY( (LONG)m_pAssistList->GetAbsX(), (LONG)GetAbsY()-interval ) ;
	m_pAssistList->SetActive( TRUE ) ;

	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetText( m_pTabButton[byClass]->GetButtonText(), txtColor, txtColor, txtColor ) ;
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetAbsXY((LONG)m_pAssistList->GetAbsX()+6, (LONG)m_pAssistList->GetAbsY() - m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight()) ;
	m_pTabButton[e_CONVERSATION_ASSISTANCE]->SetActive(TRUE) ;

	m_pCloseBtn->SetAbsXY((LONG)m_pAssistList->GetAbsX()+52, (LONG)m_pAssistList->GetAbsY() - m_pTabButton[e_CONVERSATION_ASSISTANCE]->GetHeight() + 5) ;
	m_pCloseBtn->SetActive(TRUE) ;
}

void CChattingDlg::CallShoutDlg()
{
	int middleCount  = m_pChatList->GetMiddleImageNum() ;
	int middleHeight = m_pChatList->GetHeight(LD_MIDDLE) ;

	int interval = (middleCount-2) * middleHeight + m_pShoutList->GetHeight(LD_TOP) + m_pShoutList->GetHeight(LD_DOWN) + 2 ;

	m_pShoutList->SetAbsXY( (LONG)m_pShoutList->GetAbsX(), (LONG)m_pShoutList->GetAbsY() -interval ) ;
	m_pShoutList->SetActive(TRUE);
}





//=========================================================================
//	NAME : ActionKeyboardEvent
//	DESC : The function to process keyboard event.
//=========================================================================
DWORD CChattingDlg::ActionKeyboardEvent( CKeyboard* keyInfo )
{
	DWORD we = WE_NULL ;

	we |= cDialog::ActionKeyboardEvent( keyInfo ) ;

	BOOL bFocused = m_pInputBox->IsFocus() ;
	if( !bFocused ) return we ;

	char nameStr[ MAX_NAME_LENGTH+1 ] = {0, } ;

	char* whisperName = NULL ;
	int nCurWNameIndex = 0 ;
	BOOL bPressedUp = FALSE ;
	BOOL bPressedDown = FALSE ;

	if( keyInfo->GetKeyPressed(KEY_UP) && keyInfo->GetKeyPressed(KEY_CONTROL) )
	{
		bPressedUp = TRUE ;

		nCurWNameIndex = m_nNameIndex -1 ;

		if( nCurWNameIndex < 0 ) nCurWNameIndex = MAX_WNAME_COUNT-1 ;

		whisperName = CHATMGR->GetWhisperName(nCurWNameIndex) ;
	}
	else if( keyInfo->GetKeyPressed(KEY_DOWN) && keyInfo->GetKeyPressed(KEY_CONTROL) )
	{
		bPressedDown = TRUE ;

		nCurWNameIndex = m_nNameIndex +1 ;

		if( nCurWNameIndex > MAX_WNAME_COUNT-1 ) nCurWNameIndex = 0 ;

		whisperName = CHATMGR->GetWhisperName(nCurWNameIndex) ;
	}

	if( !bPressedUp && !bPressedDown ) return we ;

	if( !whisperName || strlen(whisperName) < 4 )
	{
		whisperName = NULL ;
		whisperName = CHATMGR->GetWhisperName(m_nNameIndex) ;

		if( !whisperName || strlen(whisperName) < 4 ) return we ;
	}

	if( bPressedUp ) 
	{
		--m_nNameIndex ;

		if( m_nNameIndex < 0 ) m_nNameIndex = MAX_WNAME_COUNT-1 ;
	}
	else if( bPressedDown )
	{
		++m_nNameIndex ;

		if( m_nNameIndex > MAX_WNAME_COUNT-1 ) m_nNameIndex = 0 ;
	}

	char slash[12] = {0, } ;
	memcpy(slash, "/", strlen("/")) ;

	char* pCurPos = NULL ;
	char space[1] = {0, } ;
	space[0] = 32 ;

	strcat(nameStr, slash) ;
	strcat(nameStr, whisperName) ;
	pCurPos = nameStr ;
	pCurPos += strlen(nameStr) ;

	memcpy(pCurPos, space, sizeof(char)) ;

	if(strlen(nameStr) != 0)
	{
		m_pInputBox->SetEditText(nameStr) ;
	}

	return we ;
}

void CChattingDlg::SetRaceActive(WORD wRace, BOOL bActive)
{
	switch( wRace )
	{
	case e_CHATTINGDLG_CHAT:
		m_pChatList->SetActive(bActive);
		break;
	case e_CHATTINGDLG_ASSIST:
		m_pAssistList->SetActive(bActive);
		break;
	case e_CHATTINGDLG_GENERAL_SHOUT:
		m_pShoutList->SetActive(bActive);
		break;
	}

	// 창의 위치 조정 때문에... 다시 CallAssistanceDlg를 호출하여준다.
	if( m_pAssistList->IsActive() )
	{
		m_pAssistList->SetActive(FALSE);
		CallAssistanceDlg(m_byCurAssistance);
	}
}
